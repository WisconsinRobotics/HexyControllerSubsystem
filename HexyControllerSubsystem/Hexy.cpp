#include <stdio.h>

#include "Hexy.h"

using namespace std;

#define MAX_MESSAGE_SIZE 11
#define HEXY_INIT_RESPONSE "SERVOTOR"

#define INIT_MESSAGE "V\n"
#define RESET_MESSAGE "C\n"

static char messageBuffer[MAX_MESSAGE_SIZE + 1];
static char receiveBuffer[MAX_PATH + 1];

const int SERVO_NUMBERS[][SERVOS_PER_LEG] = {
    { 24, 25, 26 }, // Upper right, shoulder, elbow, hand
    { 20, 21, 22 }, // Middle right, shoulder, elbow, hand
    { 16, 17, 18 }, // Lower right, shoulder, elbow, hand
    { 7, 6, 5 }, // Upper left, shoulder, elbow, hand
    { 11, 10, 9 }, // Middle left, shoulder, elbow, hand
    { 15, 14, 13 } // lower left, shoulder, elbow, hand
};

const int LEG_SET_A[] = { LEFT_FRONT, RIGHT_MIDDLE, LEFT_BACK };
const int LEG_SET_B[] = { RIGHT_FRONT, LEFT_MIDDLE, RIGHT_BACK };

Hexy::Hexy()
{
    serialPort = INVALID_HANDLE_VALUE;
    lift_leg_set = LEG_SET_A;
    walk_leg_set = LEG_SET_B;
}

Hexy::~Hexy()
{
    if (serialPort != INVALID_HANDLE_VALUE)
        CloseHandle(serialPort);
}

bool Hexy::Initialize(wchar_t *comPort)
{
    DWORD bytesRead, bytesWritten;
    DCB comState;

    // serial port initialization
    serialPort = CreateFile(
        comPort,
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        FILE_FLAG_NO_BUFFERING,
        NULL
        );

    if (serialPort == INVALID_HANDLE_VALUE)
        return false;

    // Manually configure COM port.
    // Yes this was necessary, do not try to optimize this out
    // or I will hunt you down. -Z
    comState.DCBlength = sizeof(DCB);
    comState.BaudRate = CBR_9600;
    comState.fBinary = 1;
    comState.Parity = 0;
    comState.fOutxCtsFlow = 0;
    comState.fOutxDsrFlow = 0;
    comState.fDtrControl = 1;
    comState.fDsrSensitivity = 1;
    comState.fTXContinueOnXoff = 1;
    comState.fOutX = 0;
    comState.fInX = 0;
    comState.fErrorChar = 0;
    comState.fNull = 0;
    comState.fRtsControl = 1;
    comState.fAbortOnError = 0;
    comState.fDummy2 = 0;
    comState.wReserved = 0;
    comState.XonLim = 52428;
    comState.XoffLim = 52428;
    comState.ByteSize = 8;
    comState.Parity = 0;
    comState.StopBits = 0;
    comState.XonChar = 0;
    comState.XoffChar = 0;
    comState.ErrorChar = 0;
    comState.EofChar = '\0';
    comState.EvtChar = '\0';
    comState.wReserved1 = 0;

    if (!SetCommState(serialPort, &comState))
        return false;

    // servo initialization
    if (!WriteFile(serialPort, INIT_MESSAGE, 2, &bytesWritten, NULL))
        return false;

    if (!ReadFile(serialPort, receiveBuffer, MAX_PATH, &bytesRead, NULL))
        return false;

    receiveBuffer[bytesRead] = '\0';

    //printf_s("[Hexy::Initialize] %s\n", servoInitBuffer);

    if (strncmp(HEXY_INIT_RESPONSE, receiveBuffer, 8) != 0)
        return false;

    return true;
}

bool Hexy::ZeroServos()
{
    DWORD bytesRead, bytesWritten;

    if (!WriteFile(serialPort, RESET_MESSAGE, 2, &bytesWritten, NULL))
        return false;

    if (!ReadFile(serialPort, receiveBuffer, MAX_PATH, &bytesRead, NULL))
        return false;

    return true;
}

bool Hexy::Stand()
{
    int index;
    int servoPos;
    int messageSize;
    DWORD bytesWritten;

    for (index = 0; index < 6; ++index)
    {
        servoPos = ConvertToServoFromDegree(-50);
        messageSize = sprintf_s(messageBuffer, MAX_MESSAGE_SIZE + 1, "#%dP%4dT0\n", SERVO_NUMBERS[index][SHOULDER_SERVO], servoPos);
        if (!WriteFile(serialPort, messageBuffer, messageSize, &bytesWritten, NULL))
            return false;

        servoPos = ConvertToServoFromDegree(30);
        messageSize = sprintf_s(messageBuffer, MAX_MESSAGE_SIZE + 1, "#%dP%4dT0\n", SERVO_NUMBERS[index][ELBOW_SERVO], servoPos);
        if (!WriteFile(serialPort, messageBuffer, messageSize, &bytesWritten, NULL))
            return false;

        servoPos = ConvertToServoFromDegree(-36);
        messageSize = sprintf_s(messageBuffer, MAX_MESSAGE_SIZE + 1, "#%dP%4dT0\n", SERVO_NUMBERS[index][HAND_SERVO], servoPos);
        if (!WriteFile(serialPort, messageBuffer, messageSize, &bytesWritten, NULL))
            return false;
    }

    return true;
}

bool Hexy::WalkForward()
{
    int index;
    const int *oldWalk;

    for (index = 0; index < 3; ++index)
    {
        SetServoPosition(SERVO_NUMBERS[lift_leg_set[index]][ELBOW_SERVO], -50);
    }

    for (index = 0; index < 3; ++index)
    {
        SetServoPosition(SERVO_NUMBERS[lift_leg_set[index]][SHOULDER_SERVO], 30);
    }

    for (index = 0; index < 3; ++index)
    {
        SetServoPosition(SERVO_NUMBERS[walk_leg_set[index]][SHOULDER_SERVO], -30);
    }

    oldWalk = walk_leg_set;
    walk_leg_set = lift_leg_set;
    lift_leg_set = oldWalk;

    return true;
}

int Hexy::ConvertToServoFromDegree(float angle)
{
    return (int)(angle * 11.11111111111111f + 1500.f);
}

bool Hexy::SetServoPosition(int servoNum, float angle)
{
    int servoPos;
    int messageSize;
    DWORD bytesWritten;

    servoPos = ConvertToServoFromDegree(angle);
    messageSize = sprintf_s(messageBuffer, MAX_MESSAGE_SIZE + 1, "#%dP%4dT0\n", servoNum, servoPos);
    if (!WriteFile(serialPort, messageBuffer, messageSize, &bytesWritten, NULL))
        return false;

    return true;
}