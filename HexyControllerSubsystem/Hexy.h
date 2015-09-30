#pragma once

#include <Windows.h>

#define INNER 0
#define MIDDLE 1
#define OUTER 2

#define SERVOS_PER_LEG 3
#define LEGS 6

#define RIGHT_FRONT 0
#define RIGHT_MIDDLE 1
#define RIGHT_BACK 2

#define LEFT_FRONT 3
#define LEFT_MIDDLE 4
#define LEFT_BACK 5

#define SERVO_RESET 1500

class Hexy
{
public:
    Hexy();
    ~Hexy();
    bool Initialize(wchar_t* comPort);
    bool ZeroServos();

private:
    HANDLE serialPort;
};
