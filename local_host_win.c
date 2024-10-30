#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 4096

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    recv(client_socket, buffer, BUFFER_SIZE, 0);

    if (strstr(buffer, "GET /index.php")) {
        FILE *php_output = popen("php index.php", "r");
        if (php_output == NULL) {
            perror("Failed to run PHP script");
            closesocket(client_socket);
            return;
        }

        char php_response[BUFFER_SIZE];
        fread(php_response, sizeof(char), BUFFER_SIZE, php_output);
        pclose(php_output);

        char response[BUFFER_SIZE];
        snprintf(response, BUFFER_SIZE,
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/html\r\n"
                 "Content-Length: %ld\r\n\r\n"
                 "%s",
                 strlen(php_response), php_response);

        send(client_socket, response, strlen(response), 0);
    } else {
        const char *html_content =
            "<html><body><h1>404 Not Found</h1><p>File not found.</p></body></html>";
        char response[BUFFER_SIZE];
        snprintf(response, BUFFER_SIZE,
                 "HTTP/1.1 404 Not Found\r\n"
                 "Content-Type: text/html\r\n"
                 "Content-Length: %ld\r\n\r\n"
                 "%s",
                 strlen(html_content), html_content);

        send(client_socket, response, strlen(response), 0);
    }

    closesocket(client_socket);
}


int main() {
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed.\n");
        WSACleanup();
        return 1;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("Bind failed.\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, 3) == SOCKET_ERROR) {
        printf("Listen failed.\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        SOCKET client_socket = accept(server_socket, NULL, NULL);
        if (client_socket == INVALID_SOCKET) {
            printf("Client connection failed.\n");
            continue;
        }

        handle_client(client_socket);
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
