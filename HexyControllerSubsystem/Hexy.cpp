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
    { 24, 25, 26 },
    { 20, 21, 22 },
    { 16, 17, 18 },
    { 7, 6, 5 },
    { 11, 10, 9 },
    { 15, 14, 13 }
};

Hexy::Hexy()
{
}

Hexy::~Hexy()
{
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
    // #%dP%.4dT0\r
    /*
    int legIndex;
    int servoIndex;
    int messageSize;

    for (legIndex = 0; legIndex < LEGS; ++legIndex)
    {
    for (servoIndex = 0; servoIndex < SERVOS_PER_LEG; ++servoIndex)
    {
    messageSize = sprintf_s(messageBuffer, MAX_MESSAGE_SIZE + 1, "#%dP%4dT0\r", SERVO_NUMBERS[legIndex][servoIndex], SERVO_RESET);
    }
    }
    */
    return true;
}