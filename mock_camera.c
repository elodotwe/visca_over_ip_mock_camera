#include <stdio.h>
#include <jr_socket.h>

#include <jr_hex_print.h>
#include <jr_visca.h>
#include <string.h>

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

    int count = 0;
    char buffer[1024];
    jr_viscaFrame frame;

    int latestCount;
    while ((latestCount = jr_socket_receive(clientSocket, buffer + count, 1024 - count)) > 0) {
        count += latestCount;
        printf("recv: ");
        hex_print(buffer, count);
        printf("\n");
        int consumed;
        do {
            consumed = jr_viscaDataToFrame(buffer, count, &frame);
            if (consumed) {
                printf("found %d-byte frame: ", consumed);
                printf("sender: %d, receiver: %d, data: ", frame.sender, frame.receiver);
                hex_print(frame.data, frame.dataLength);
                printf("\n");
                union jr_viscaMessageParameters messageParameters;
                int messageType = jr_viscaDecodeFrame(frame, &messageParameters);
                printf("message type %d\n", messageType);
                switch (messageType)
                {
                case JR_VISCA_MESSAGE_PAN_TILT_POSITION_INQ:
                    printf("pan tilt inq\n");
                    break;
                case JR_VISCA_MESSAGE_ZOOM_POSITION_INQ:
                    printf("zoom inq\n");
                    break;
                default:
                    printf("unknown\n");
                    break;
                }

                count -= consumed;
                // Crappy naive buffer management-- move remaining bytes up to buffer[0].
                // Maybe later we'll replace this with a circular buffer or something.
                // For now I just want it to work.
                memmove(buffer, buffer + consumed, count);
            }
        } while (consumed);
    }

    printf("Connection spun down, terminating.\n");

    jr_socket_closeSocket(clientSocket);
    jr_socket_closeServerSocket(serverSocket);
    return 0;
}

