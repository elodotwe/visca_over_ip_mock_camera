#include <sys/socket.h>
#include <netinet/ip.h>
#include <error.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include "jr_socket.h"

int jr_socket_setupServerSocket(int port, jr_server_socket *serverSocket) {
    serverSocket->_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket->_serverSocket < 0) {
        perror("socket");
        return -1;
    }

    // At present this is a debug-only server.
    // Bypass the OS-level protection against "thrashing" a socket closed and then back open.
    // This protection exists to prevent packets from a previous connection affecting this connection.
    // Not a concern when we're playing on a local Wifi connection with a toy project.
    int enable = 1;
    setsockopt(serverSocket->_serverSocket, IPPROTO_IP, SO_REUSEADDR, &enable, sizeof(enable));

    struct sockaddr_in address = { 0 };
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;
    int bindResult = bind(serverSocket->_serverSocket, (struct sockaddr *)&address, sizeof(address));
    if (bindResult == -1) {
        perror("bind");
        return -1;
    }

    int listenResult = listen(serverSocket->_serverSocket, 1);
    if (listenResult == -1) {
        perror("listen");
        return -1;
    }

    return 0;
}

int jr_socket_accept(jr_server_socket serverSocket, jr_socket *socket) {
    socket->_socket = accept(serverSocket._serverSocket, NULL, NULL);

    if (socket->_socket == -1) {
        perror("accept");
        return -1;
    }

    return 0;
}

int jr_socket_receive(jr_socket socket, char* buffer, int buffer_size) {
    int result = recv(socket._socket, buffer, buffer_size, 0);
    if (result == -1) {
        perror("recv");
        return -1;
    }

    return result;
}

void jr_socket_closeSocket(jr_socket socket) {
    close(socket._socket);
}

void jr_socket_closeServerSocket(jr_server_socket socket) {
    close(socket._serverSocket);
}