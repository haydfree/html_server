#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define ERROR -1

int server_socket;
typedef struct sockaddr_in ServerAddress;
ServerAddress *p_ServerAddress;

enum AddressFamily { IPV4 = AF_INET, IPV6 = AF_INET6 };
enum SocketType { TCP = SOCK_STREAM, UDP = SOCK_DGRAM };
enum SocketProtocol {TCP_PROTOCOL = IPPROTO_TCP, UDP_PROTOCOL = IPPROTO_UDP };
enum AddressOption { ANY = INADDR_ANY, LOOPBACK = INADDR_LOOPBACK };

void init_server_address() {
    p_ServerAddress = malloc(sizeof(ServerAddress));
    if (p_ServerAddress == NULL) {
        perror("Server address is null. \n");
        exit(1);
    }

    memset(p_ServerAddress, 0, sizeof(ServerAddress));
}

int main() {
    server_socket = socket(IPV4, TCP, TCP_PROTOCOL); 
    if (server_socket == ERROR) {
        perror("Socket error");  
        exit(1);
    }

    int port = htons(PORT);

    init_server_address();
    p_ServerAddress->sin_family = IPV4;
    p_ServerAddress->sin_addr.s_addr = ANY;
    p_ServerAddress->sin_port = port;

    int bind_res = bind(server_socket, (struct sockaddr *) p_ServerAddress, sizeof(ServerAddress));
    if (bind_res == ERROR) {
        perror("Bind error");
        exit(1);
    }

    printf("Server initialized on %s:%d\n", inet_ntoa(p_ServerAddress->sin_addr), ntohs(p_ServerAddress->sin_port));


    close(server_socket); 
    free(p_ServerAddress);
    return 0;
}
