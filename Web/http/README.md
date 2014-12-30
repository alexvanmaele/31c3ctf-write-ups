# http

**Category:** Web

**Points:** 15

**Description:**

> Check out our cool webserver. It is really fast because it is implemented in C. For security we use the versatility of ruby.
> 
> Get the source at: https://31c3ctf.aachen.ccc.de/uploads/http.tar.bz2
> 
> Some example sites hosted with our webserver:
> 
> - http://works.90.31c3ctf.aachen.ccc.de/works.html
> - http://31c3ctf.90.31c3ctf.aachen.ccc.de/announcements.html

## Write-up

The webserver was using a simple regex check in Ruby to filter malicious requests.

We noticed the firewall was only checking the last HOST parameter in a request, so we crafted an HTTP request containg two different HOST parameters. The first HOST parameter was malicious and attempted path traversal, while the second was a legitimate one.

```bash
$ echo -e "GET /passwd HTTP/1.1\r\nHost: ../../../../../../../etc\r\nHost: 90.31c3ctf.aachen.ccc.de\r\n\r\n" | nc 90.31c3ctf.aachen.ccc.de 80
```

Since the firewall only checked the last (second) one, our malicious parameter got through and was used to read out the passwd file containing the flag:
`31C3_b45fa9e4d5969e3c524bdcde15f84125`