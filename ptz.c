#include "ptz.h"

#include <string.h>

// Returns number of bytes consumed
int ptzDataToFrame(uint8_t *data, int dataLength, ptzFrame *frame) {
    // We only decode a frame if the entire frame is present, i.e. 0xff terminator is present.
    int terminatorIndex;
    for (terminatorIndex = 0; terminatorIndex < dataLength; terminatorIndex++) {
        if (data[terminatorIndex] == 0xff) {
            break;
        }
    }

    // If we didn't find a terminator, the index will == dataLength.
    if (!terminatorIndex < dataLength) {
        // No bytes consumed, since we're waiting for more bytes to arrive.
        return 0;
    }

    // First byte is header containing sender and receiver addresses.
    frame->sender = (data[0] >> 4) & 0x7;
    frame->receiver = data[0] & 0x7;

    // N bytes of packet data between header byte and 0xff terminator.
    memcpy(frame->data, data + 1, terminatorIndex - 1);

    return terminatorIndex + 1;
}

