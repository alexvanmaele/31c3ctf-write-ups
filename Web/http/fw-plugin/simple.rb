
class SimpleFirewall < Firewall

	def acceptable?(request)
		if request.path=~ /\A\/[A-Za-z0-9]+(\.[A-Za-z0-9]+)?\z/ and
		   request["Host"]=~ /\A[A-Za-z0-9]+(\.[A-Za-z0-9]+)*(:[1-9][0-9]*)?\z/
			true
		else
			false
		end
	end


end

SimpleFirewall

