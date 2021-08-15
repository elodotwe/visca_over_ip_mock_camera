#ifndef PTZ_H
#define PTZ_H

#include <stdint.h>

typedef struct {
    uint8_t sender;
    uint8_t receiver;
    uint8_t data[14];
    uint8_t dataLength;
} ptzFrame;

int ptzDataToFrame(uint8_t *data, int dataLength, ptzFrame *frame);

#endif