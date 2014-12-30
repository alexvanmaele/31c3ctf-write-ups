#!/usr/bin/ruby

class InputBuffer

	attr_reader :buffer

	def initialize(input)
		@input= input
		@buffer= ""
	end

	def read_nonblock(size)
		data= @input.read_nonblock(size)
		@buffer+= data
		data
	end

	def to_io
		@input
	end

end

class LineReader

	def initialize(input)
		@input= input
		@buffer= ""
		@eof= false
	end

	def read_line
		return if @eof
		until @buffer.include? "\r\n"
			begin
				@buffer+= @input.read_nonblock(4096)
			rescue ::Errno::EAGAIN
				IO.select([@input])
			rescue EOFError
				@eof= true
				return
			end
		end
		return if @buffer.start_with? "\r\n"
		line, @buffer= @buffer.split("\r\n", 2)
		line
	end

	def each
		while line= read_line
			yield line
		end
	end

	include Enumerable

end

class HTTPRequest

	attr_reader :path

	def initialize(line_reader)
		@path, @headers= parse(line_reader)
	end

	def [](name)
		@headers[name]
	end

	private

	def parse(line_reader)
		[parse_request_line(line_reader),
		 parse_headers(line_reader)]
	end

	def parse_request_line(line_reader)
		request_line= line_reader.read_line
		return if request_line.nil?
		request_line=~ /\AGET (.*) HTTP\/1\.[01]\z/
		$1
	end

	def parse_headers(line_reader)
		line_reader.collect do |line|
			[$1, $2] if line=~ /\A([^:]*): *(.*)\z/
		end.compact.inject({}) { |h, x| h[x[0]]= x[1]; h }
	end

end

class Firewall

	def acceptable?(request)
		raise NotImplementedError
	end

	def test(request)
		abort unless acceptable?(request)
	end

	def abort
		STDOUT.write "HTTP/1.0 403 Forbidden\r\n\r\nForbidden"
		exit 0
	end

end

if ARGV.size < 2
	STDERR.puts "usage: <fw-plugin> <exec-args>"
	exit 1
end

plugin= eval(File.read("./fw-plugin/"+ARGV[0]+".rb")).new

buffer= InputBuffer.new(STDIN)
line_reader= LineReader.new(buffer)
request= HTTPRequest.new(line_reader)

plugin.test(request)

r, w= IO.pipe
if w.write_nonblock(buffer.buffer) != buffer.buffer.size
	STDOUT.write "HTTP/1.0 413 Request Entity Too Large\r\n\r\n413 Request Entity Too Large"
	exit 0
end
STDIN.reopen(r)
exec(*ARGV[1..-1])

