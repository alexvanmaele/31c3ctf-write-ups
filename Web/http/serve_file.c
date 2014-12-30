
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>

void transmit(const char *buffer, size_t buf_size);
bool read_line(char *buffer, size_t *buf_size);
void log_request(const char *host, const char *path, const char *status);

int main(int argc, char **argv) {
	if (argc != 1) {
		fprintf(stderr, "usage: %s", argv[0]);
		exit(1);
	}

	const char *path= NULL;
	const char *host= NULL;
	size_t buf_size= 4096;
	char *buffer= calloc(buf_size, sizeof(*buffer));

	if (!read_line(buffer, &buf_size)) {
		goto invalid;
	}
	if (strncmp(buffer, "GET /", sizeof("GET /")-1) != 0) {
		goto invalid;
	}
	buffer+= sizeof("GET /")-1;
	buf_size-= sizeof("GET /")-1;
	path= buffer;
	char *space= strchr(buffer, ' ');
	if (space == NULL) {
		goto invalid;
	}
	buf_size-= space-buffer;
	buffer= space;
	*buffer= 0;
	buffer+= 1;
	buf_size-= 1;
	if ((strncmp(buffer, "HTTP/1.0\r\n", sizeof("HTTP/1.0\r\n")-1) != 0) &&
	    (strncmp(buffer, "HTTP/1.1\r\n", sizeof("HTTP/1.1\r\n")-1) != 0)) {
		goto invalid;
	}
	buffer+= sizeof("HTTP/1.0\r\n")-1;
	buf_size-= sizeof("HTTP/1.0\r\n")-1;

	for (;;) {
		if (!read_line(buffer, &buf_size)) {
			goto invalid;
		}
		if (*buffer == '\r') {
			goto invalid;
		}
		if (strncmp(buffer, "Host: ", sizeof("Host: ")-1) == 0) {
			break;
		}
		char *eol= strchr(buffer, '\r');
		buf_size-= eol-buffer-2;
		buffer= eol+2;
	}
	buffer+= sizeof("Host: ")-1;
	buf_size-= sizeof("Host: ")-1;
	host= buffer;
	char *cr= strchr(buffer, '\r');
	*cr= 0;

	if (chdir("www-data") == -1) {
		perror("chdir");
		exit(2);
	}

	if (chdir(host) == -1) {
		goto _404;
	}

	int fd= open(path, O_RDONLY);
	if (fd == -1) {
		goto _404;
	}
	struct stat stat;
	if (fstat(fd, &stat) == -1) {
		goto _404;
	}
	const char *file= mmap(NULL, stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (file == NULL) {
		goto _404;
	}
	close(fd);

	log_request(host, path, "200");

	transmit("HTTP/1.0 200 OK\r\n\r\n", sizeof("HTTP/1.0 200 OK\r\n\r\n")-1);
	transmit(file, stat.st_size);
	munmap((void *)file, stat.st_size);

	exit(0);

invalid:
	log_request(host, path, "400");
	transmit("HTTP/1.0 400 Bad Request\r\n\r\n", sizeof("HTTP/1.0 400 Bad Request\r\n\r\n")-1);
	exit(0);
_404:
	log_request(host, path, "404");
	transmit("HTTP/1.0 404 Not Found\r\n\r\n", sizeof("HTTP/1.0 404 Not Found\r\n\r\n")-1);
	exit(0);
}


void transmit(const char *buffer, size_t buf_size) {
	size_t buf_pos= 0;
	while (buf_pos < buf_size) {
		int size= write(1, buffer+buf_pos, buf_size-buf_pos);
		if (size == -1) {
			perror("write");
			exit(2);
		}
		buf_pos+= size;
	}
}


bool read_line(char *buffer, size_t *buf_size) {
	char *eol= strchr(buffer, '\r');
	if (eol != NULL) {
		if (*(eol+1) != '\n') {
			return false;
		}
		return true;
	}

	char *buf_end= buffer+strlen(buffer);
	int size= read(0, buf_end, *buf_size-(buf_end-buffer)-1);
	if (size == -1) {
		perror("read");
		exit(2);
	}
	*buf_size+= size;
	buffer[*buf_size]= 0;
	
	eol= strchr(buffer, '\r');
	if (eol != NULL) {
		if (*(eol+1) != '\n') {
			return false;
		}
		return true;
	}
	return false;
}


void log_request(const char *host, const char *path, const char *status) {
	time_t time_t;
	time(&time_t);
	struct tm tm;
	localtime_r(&time_t, &tm);
	char time[32];
	strftime(time, sizeof(time), "%d/%b/%Y:%H:%M:%S %z", &tm);
	fprintf(stderr, "- - - [%s] \"GET http://%s/%s HTTP/1.0\" %s -\n", time, host, path, status);
}

