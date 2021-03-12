#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
// for sockaddr_in
#include <netinet/in.h>
#include <string.h>
// for read. Unix standard header
#include <stdbool.h>
#include <unistd.h>

void fatalError(char* formatString, ...);
char* loadFileText(char* fileName);
long getFileSize(FILE* file);
char* createResponse();
void sigintHandler(int num);
