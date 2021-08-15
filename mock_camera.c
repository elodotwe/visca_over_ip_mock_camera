#include <stdio.h>
#include "jr_socket.h"

void hex_print(char *buf, int buf_size);

int main() {
    printf("hello\n");

    jr_server_socket serverSocket;

    if (jr_socket_setupServerSocket(5678, &serverSocket) == -1) {
        printf("Setup failed\n");
        return -1;
    }

    jr_socket clientSocket;
    if (jr_socket_accept(serverSocket, &clientSocket) == -1) {
        printf("Accept failed");
        return -2;
    }

    int count;
    char buffer[1024];

    while ((count = jr_socket_receive(clientSocket, buffer, 1024)) > 0) {
        printf("recv: ");
        hex_print(buffer, count);
        printf("\n");
    }

    printf("Connection spun down, terminating.\n");

    jr_socket_closeSocket(clientSocket);
    jr_socket_closeServerSocket(serverSocket);
    return 0;
}

void hex_print(char *buf, int buf_size) {
    for (int i = 0; i < buf_size; i++) {
        printf("%02hhx ", buf[i]);
    }
}