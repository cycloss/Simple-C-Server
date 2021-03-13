#include "server.h"
#include "response.h"

// TODO study this: http://www.cs.cmu.edu/afs/cs/academic/class/15213-s00/www/class28/tiny.c

int serverSocketFD;

int main() {

    signal(SIGINT, sigintHandler);
    // Socket is a connection to a port at a certain ip address that can send and receive data
    // a door to a house that acts like other unix file descriptors
    // First must create the socket file descriptor. probably 3 as 0 is stdin, 1 stdout, 2 stderr
    // Address family INET (TCP). AF_UNIX is like a pipe
    // SOCK_DGRAM for udp

    serverSocketFD = socket(AF_INET, SOCK_STREAM, 0);

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

    // bind (assign) sin_addr.s_addr to socket
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
        // TODO dynamically resize readBuff
        int bytesRead = read(newSocket, readBuff, sizeof(readBuff));

        if (bytesRead == 0) {
            fatalError("Could not read from new socket\n");
        }

        printf("\n========Connection Details=========\n");
        printf("%s\n", readBuff);
        printf("=========End Details=========\n");
        char* response = createResponse("page.html");
        printf("\n========Sending Response=========\n");

        if (response == NULL) {
            char* resp = "HTTP/1.1 500 SERVER ERROR\r\nContent-Length: 0\r\n\r\n";
            write(newSocket, resp, strlen(resp));
        } else {
            puts(response);
            printf("========End Response=========\n");
            write(newSocket, response, strlen(response));
            free(response);
        }

        // can write many times by setting content length to greater than the number of bytes you are sending at once.
        // Client will wait until all bytes have been received before displaying

        close(newSocket);
        printf("Closed client socket\n\n");
    }
    return 0;
}

void sigintHandler(int num) {
    close(serverSocketFD);
    puts("=======Server Finished======");
    exit(0);
}
