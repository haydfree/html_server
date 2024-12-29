#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define ERROR -1
#define BACKLOG_SIZE 5
#define BUFFER_SIZE 1024

int serverSocket;
typedef struct sockaddr_in Address;
Address *p_serverAddress;
Address *p_clientAddress;

enum AddressFamily { IPV4 = AF_INET, IPV6 = AF_INET6 };
enum SocketType { TCP = SOCK_STREAM, UDP = SOCK_DGRAM };
enum SocketProtocol {TCP_PROTOCOL = IPPROTO_TCP, UDP_PROTOCOL = IPPROTO_UDP };
enum AddressOption { ANY = INADDR_ANY, LOOPBACK = INADDR_LOOPBACK };
enum ErrorType {SVR_ADDR_INIT, SVR_SOCK_INIT, CLI_ADDR_INIT, CLI_SOCK_INIT, BIND, LISTEN, RECV };
const char *ErrorTypeStr[7] = {"SVR_ADDR_INIT", "SVR_SOCK_INIT", "CLI_ADDR_INIT", 
                            "CLI_SOCK_INIT", "BIND", "LISTEN", "RECV" };

void checkForErrors(int data, enum ErrorType type) {
    if (data != -1) { return; }
    printf("Error at: %s \n", ErrorTypeStr[type]);
}

void checkForNull(void *data, enum ErrorType type) {
    if (data != NULL) { return; }
    checkForErrors(-1, type);
}

void initServerAddress(int serverPort) {
    p_serverAddress = malloc(sizeof(Address));
    checkForNull(p_serverAddress, SVR_ADDR_INIT);
    memset(p_serverAddress, 0, sizeof(Address));

    serverPort = htons(serverPort);
    p_serverAddress->sin_family = IPV4;
    p_serverAddress->sin_addr.s_addr = ANY;
    p_serverAddress->sin_port = serverPort;
}

int main() {
    serverSocket = socket(IPV4, TCP, TCP_PROTOCOL); 
    checkForErrors(serverSocket, SVR_SOCK_INIT);
    
    initServerAddress(PORT);

    int bindRes = bind(serverSocket, (struct sockaddr *) p_serverAddress, sizeof(Address));
    checkForErrors(bindRes, BIND);

    int listenRes = listen(serverSocket, BACKLOG_SIZE);
    checkForErrors(listenRes, LISTEN);

    while (1) {
        int clientSocket = accept(serverSocket, NULL, NULL);
        checkForErrors(clientSocket, CLI_SOCK_INIT);

        char buffer[BUFFER_SIZE];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        checkForErrors(bytesReceived, RECV);
        buffer[bytesReceived] = '\0';

        const char *httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, world!";
        send(clientSocket, httpResponse, strlen(httpResponse), 0);

        close(clientSocket);
    }

    close(serverSocket); 
    free(p_serverAddress);
    return 0;
}
