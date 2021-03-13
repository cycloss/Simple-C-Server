#include "response.h"

char* createResponse(char* fileName) {
    HttpBody httpBody = { NULL, 0 };
    loadBody(fileName, &httpBody);
    if (httpBody.body == NULL) {
        puts("Unable to build response");
        return NULL;
    }
    char* header = createHeader(httpBody.byteSize);
    if (header == NULL) {
        puts("Unable to creater response header");
        return NULL;
    }
    stringBuilder* builder = createStringBuilder();
    appendToBuilder(builder, header);
    appendToBuilder(builder, httpBody.body);
    free(httpBody.body);
    char* response = builder->string;
    freeBuilder(builder, false);
    return response;
}

void loadBody(char* fileName, HttpBody* bodyP) {
    FILE* file = fopen(fileName, "r");
    if (file == NULL) {
        return;
    }
    long byteSize = getFileSize(file);
    //NO NEED TO ADD NULL TERMINATOR FOR http message.
    //Only need it if you're going to use a string function or print it out
    char* buff = malloc(byteSize + 1);
    if (buff == NULL) {
        return;
    }
    buff[byteSize] = '\0';
    fread(buff, sizeof(char), byteSize, file);
    fclose(file);
    bodyP->body = buff;
    bodyP->byteSize = byteSize;
}

long getFileSize(FILE* file) {
    fseek(file, 0, SEEK_END);
    long byteSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    return byteSize;
}

char* createHeader(long bodyByteSize) {

    //add one for null term
    int bSizeDigits = getDigitCount(bodyByteSize) + 1;
    char contentLen[bSizeDigits];
    snprintf(contentLen, bSizeDigits, "%ld", bodyByteSize);

    stringBuilder* sb = createStringBuilder();
    appendToBuilder(sb, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n");
    appendToBuilder(sb, "Content-Length: ");
    appendToBuilder(sb, contentLen);
    appendToBuilder(sb, "\r\n\r\n");
    char* retStr = sb->string;
    freeBuilder(sb, false);
    return retStr;
}

int getDigitCount(long num) {
    int i = 0;
    for (; num > 0; i++, num /= 10) {
    }
    return i;
}