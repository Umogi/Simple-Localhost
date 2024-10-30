#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 4096

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    recv(client_socket, buffer, BUFFER_SIZE, 0);

    if (strstr(buffer, "GET /index.php")) {
        FILE *php_output = popen("php index.php", "r");
        if (php_output == NULL) {
            perror("Failed to run PHP script");
            close(client_socket);
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

    close(client_socket);
}


int main() {
    int server_socket;
    struct sockaddr_in address;
    int opt = 1;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 3) == -1) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket == -1) {
            perror("Client connection failed");
            continue;
        }

        handle_client(client_socket);
    }

    close(server_socket);
    return 0;
}
