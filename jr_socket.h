#ifndef JRSOCKET_H
#define JRSOCKET_H

typedef struct _jr_socket {
    int _socket;
} jr_socket;

typedef struct _jr_server_socket {
    int _serverSocket;
} jr_server_socket;

int jr_socket_setupServerSocket(int port, jr_server_socket *serverSocket);

int jr_socket_accept(jr_server_socket serverSocket, jr_socket *socket);

int jr_socket_receive(jr_socket socket, char* buffer, int buffer_size);

void jr_socket_closeSocket(jr_socket socket);

void jr_socket_closeServerSocket(jr_server_socket socket);

#endif