#!/bin/bash
exec socat "TCP-LISTEN:80,reuseaddr=1,fork" "EXEC:./fw.rb simple ./serve_file,su=nobody,nofork" 2> >(tee -a ../www.log)
