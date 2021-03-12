HEADERS = *.h

.DEFAULT_TARGET: server
.PHONY: server
server: *.c *.h
	gcc -Wall -o server server.c