#include "can_communications.h"

EventQueue can_queue(32 * EVENTS_EVENT_SIZE);

CAN can1(PA_11, PA_12, 1000000);

typedef union {
    float value;
    char buffer[sizeof(float)];
} canPacket_t;

volatile canPacket_t canPacket = {.value = 0.0f};

void can_send(void)
{
    const float i = 0.01f;
    canPacket.value += i;
    can1.write(CANMessage(0x1, (const char *)&canPacket.buffer, sizeof(float)));
}