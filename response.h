#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utilities.h>

typedef struct {
    char* body;
    long byteSize;
} HttpBody;

char* createResponse(char* fileName);
void loadBody(char* fileName, HttpBody* bodyP);
long getFileSize(FILE* file);
char* createHeader(long bodyByteSize);
int getDigitCount(long num);