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

void initAddresses() {
    p_serverAddress = malloc(sizeof(Address));
    if (p_serverAddress == NULL) {
        perror("Server address is null. \n");
        exit(1);
    }

    memset(p_serverAddress, 0, sizeof(Address));

    p_clientAddress = malloc(sizeof(Address));
    if (p_clientAddress == NULL) {
        perror("Client address is null. \n");
        exit(1);
    }

    memset(p_clientAddress , 0, sizeof(Address));
}

int main() {
    serverSocket = socket(IPV4, TCP, TCP_PROTOCOL); 
    if (serverSocket == ERROR) {
        perror("Socket error");  
        exit(1);
    }

    int port = htons(PORT);

    initAddresses();
    p_serverAddress->sin_family = IPV4;
    p_serverAddress->sin_addr.s_addr = ANY;
    p_serverAddress->sin_port = port;

    int bindRes = bind(serverSocket, (struct sockaddr *) p_serverAddress, sizeof(Address));
    if (bindRes == ERROR) {
        perror("Bind error");
        exit(1);
    }

    printf("Server initialized on %s:%d\n", inet_ntoa(p_serverAddress->sin_addr), ntohs(p_serverAddress->sin_port));

    int listenRes = listen(serverSocket, BACKLOG_SIZE);
    if (listenRes == ERROR) {
        perror("Listen error");
        exit(1);
    }

    int clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket == ERROR) {
        perror("Accept error");
        exit(1);
    }

    close(serverSocket); 
    free(p_serverAddress);
    return 0;
}
