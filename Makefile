HEADERS = server.h response.h
SOURCES = server.c response.c

.DEFAULT_TARGET: server
.PHONY: server
server: *.c *.h
	gcc -Wall -lGenericStructures -o server $(SOURCES)