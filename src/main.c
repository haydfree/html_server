#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

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

typedef struct HttpHeader {
    char *name;
    char *value;
} HttpHeader;

typedef struct HttpRequest {
    char *method;
    char *path;
    HttpHeader *headers;
    char *body;
} HttpRequest;

typedef struct HttpResponse {
    int statusCode;
    HttpHeader *headers;
    char *body;
} HttpResponse;

void checkForErrors(int code, int *data, char *file, const char *func, int line, char *msg) {
    if (*data != -1) { return; }
    fprintf(stderr, 
            "Error: %s\n"
            "Code: %d\n" 
            "Location: %s:%d in %s\n",
            msg, code, file, line, func
            );
    exit(code);
}

void checkForNull(int code, void *data, char *file, const char *func, int line, char *msg) {
    if (data != NULL) { return; }
    checkForErrors(code, data, file, func, line, msg);
}

void initServerAddress(int serverPort) {
    p_serverAddress = malloc(sizeof(Address));
    checkForNull(-1, p_serverAddress, __FILE__, __func__, __LINE__, "p_serverAddress is null");
    memset(p_serverAddress, 0, sizeof(Address));

    serverPort = htons(serverPort);
    p_serverAddress->sin_family = IPV4;
    p_serverAddress->sin_addr.s_addr = ANY;
    p_serverAddress->sin_port = serverPort;
}

HttpRequest *buildRequest(char *method, char *path, HttpHeader *headers, char *body) {
    HttpRequest *req = malloc(sizeof(HttpRequest));
    checkForNull(-1, req, __FILE__, __func__, __LINE__, "request is null");

    req->method = method;
    req->path = path;
    req->headers = headers;
    req->body = body;

    return req;
}

void parseRequest(char *requestStr) { 
    char *method, *path, *body;
    HttpHeader *headers;

    if (strncmp(requestStr, "GET", 3) == 0) { printf("get request \n"); method = "GET"; } 
    if (strncmp(requestStr, "POST", 4) == 0) { printf("post request \n"); method = "POST"; } 
    if (strncmp(requestStr, "PUT", 3) == 0) { printf("put request \n"); method = "PUT"; } 
    if (strncmp(requestStr, "DELETE", 6) == 0) { printf("delete request \n"); method = "DELETE"; } 

    
}

void sendRequest();

void buildResponse();
void parseResponse();
void receiveResponse();

void handleRoute();
void handleRequest();

int main() {
    serverSocket = socket(IPV4, TCP, TCP_PROTOCOL); 
    checkForErrors(-1, &serverSocket, __FILE__, __func__, __LINE__, "serverSocket");

    initServerAddress(PORT);

    int bindRes = bind(serverSocket, (struct sockaddr *) p_serverAddress, sizeof(Address));
    checkForErrors(-1, &bindRes, __FILE__, __func__, __LINE__, "bindres");

    int listenRes = listen(serverSocket, BACKLOG_SIZE);
    checkForErrors(-1, &listenRes, __FILE__, __func__, __LINE__, "listenRes");

    while (1) {
        int clientSocket = accept(serverSocket, NULL, NULL);
        checkForErrors(-1, &clientSocket, __FILE__, __func__, __LINE__, "clientSocket");


        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        checkForErrors(-1, &bytesReceived, __FILE__, __func__, __LINE__, "bytesReceived");

        buffer[bytesReceived] = '\0';

        char blankBuf[256];
        printf("request: %s\n", buffer);
        sscanf(buffer, "%255s", blankBuf);
        printf("blankBuf: %s", blankBuf);
        parseRequest(buffer);
        const char *httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, world!";
        send(clientSocket, httpResponse, strlen(httpResponse), 0);

        close(clientSocket);
    }

    close(serverSocket); 
    free(p_serverAddress);
    return 0;
}
