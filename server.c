#include "server.h"

int main() {

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

    while (true) {
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
        printf("=========End Details========\n");
        char* message = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
        printf("Sending message: \"%s\"...\n", message);
        write(newSocket, message, strlen(message));
        close(newSocket);
        printf("Closed socket\n\n");
    }
}

void fatalError(char* formatStr, ...) {
    fprintf(stderr, "Fatal Error: ");
    va_list args;
    va_start(args, formatStr);
    vfprintf(stderr, formatStr, args);
    exit(1);
}