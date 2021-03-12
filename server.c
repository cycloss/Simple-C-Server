#include "server.h"

bool interruptCalled = false;

int main() {

    signal(SIGINT, sigintHandler);
    // Socket is a connection to a port at a certain ip address that can send and receive data
    // a door to a house that acts like other unix file descriptors
    // First must create the socket file descriptor. probably 3 as 0 is stdin, 1 stdout, 2 stderr
    // Address family INET (TCP). AF_UNIX is like a pipe
    // SOCK_DGRAM for udp

    int serverSocketFD = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocketFD < 0) {
        fatalError("Failed to create socket file descriptor. Error: %i\n", serverSocketFD);
    }

    // socket internet address
    // generic container for OS to identify address family
    struct sockaddr_in* sockAddr = calloc(1, sizeof(struct sockaddr_in));
    memset(sockAddr->sin_zero, '\0', sizeof(sockAddr->sin_zero));

    if (sockAddr == NULL) {
        fatalError("Calloc for sockaddr_in failed");
    }

    // make a system call to bind the socket file descriptor to a port
    // socket internet family. Same as when socketfd opened
    sockAddr->sin_family = AF_INET;

    const int PORT_NO = 8080;
    // socket internet port, clients choose 0 for auto assignment by OS
    // host to network short, converts short to network representation (different byte order)
    sockAddr->sin_port = htons(PORT_NO);
    // host to network long
    // Machine's IP, will be one for each network interface (wifi, ethernet)
    // let OS choose with 0.0.0.0 which is INADDR_ANY
    sockAddr->sin_addr.s_addr = htonl(INADDR_ANY);

    //bind (assign) sin_addr.s_addr to socket
    // must cast to sockadd*
    int bindRes = bind(serverSocketFD, (struct sockaddr*)sockAddr, sizeof(*sockAddr));

    if (bindRes < 0) {
        fatalError("Failed to bind socketFD to port %i. Error code: %i\n", PORT_NO, bindRes);
    }

    // listen to the socket, telling it that it should accept incoming connections
    // initial socket is only for accepting connections, not for transferring data
    // backlog is max number of queued pending connections before refusal
    printf("Listening to connection socket...\n");
    int listenRes = listen(serverSocketFD, 3);
    if (listenRes < 0) {
        fatalError("Failed to listen to socket. Error code: %i\n", listenRes);
    }

    while (!interruptCalled) {
        // accept a connection from the queue and assign it to a new socket
        // blocks until connection present in queue
        // first param is fd of connection socket
        // second param is pointer where connecting socket's info can be written to
        // third param sock len will contain the size of the new socket when accept returns
        // type http://localhost:8080 into the browser to make a client connection
        socklen_t socklen;
        printf("Accepting new connections...\n");
        int newSocket = accept(serverSocketFD, (struct sockaddr*)sockAddr, &socklen);
        if (newSocket < 0) {
            fatalError("Failed to accept a new connection. Error code: %i\n", socklen);
        }
        printf("Accepted new connection!\n");

        // char is 1 byte so will read every byte individually (int is 2 bytes)
        char readBuff[1024];
        // read bytes send by incoming connection
        // use read because of fread because read reads from an integer file descriptor, where as fread reads from a FILE*
        int bytesRead = read(newSocket, readBuff, sizeof(readBuff));

        if (bytesRead == 0) {
            fatalError("Could not read from new socket\n");
        }

        printf("\n========Connection Details=========\n");
        printf("%s\n", readBuff);
        printf("=========End Details=========\n");

        char* response = createResponse();
        if (response == NULL) {
            response = "HTTP/1.1 500 SERVER ERROR\n\n";
        }
        printf("\n========Sending Response=========\n");
        puts(response);
        printf("========End Response=========\n");
        // can write many times by setting content length to greater than the number of bytes you are sending at once.
        // Client will wait until all bytes have been received before displaying
        write(newSocket, response, strlen(response));
        free(response);
        close(newSocket);
        printf("Closed client socket\n\n");
    }
    close(serverSocketFD);
    puts("=======Server Finished======");
}

void fatalError(char* formatStr, ...) {
    fprintf(stderr, "Fatal Error: ");
    va_list args;
    va_start(args, formatStr);
    vfprintf(stderr, formatStr, args);
    exit(1);
}

char* createResponse() {
    char* bodyText = loadFileText("test.json");
    //TODO construct 'Content-Length:' properly
    //TODO test sending an image. optimum block load size is 2^12 (4096)
    char* header = "HTTP/1.1 200 OK\nContent-Type: application/json\n\n";
    char* response = malloc(strlen(bodyText) + strlen(header) + 1);
    if (bodyText == NULL || response == NULL) {
        return NULL;
    }
    response[0] = '\0';
    strcat(response, header);
    strcat(response, bodyText);
    free(bodyText);
    return response;
}

char* loadFileText(char* fileName) {

    FILE* file = fopen(fileName, "r");
    long byteSize = getFileSize(file);
    char* buff = malloc(byteSize + 1);
    if (buff == NULL) {
        return NULL;
    }
    // add in terminator
    buff[byteSize] = '\0';
    fread(buff, sizeof(char), byteSize, file);
    fclose(file);
    return buff;
}

long getFileSize(FILE* file) {
    fseek(file, 0, SEEK_END);
    long byteSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    return byteSize;
}

void sigintHandler(int num) {
    puts("Interrupt received");
    interruptCalled = true;
}
