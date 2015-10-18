#include <Windows.h>
#include <stdio.h>

#include "Hexy.h"

Hexy hexy;

int main()
{
    if (!hexy.Initialize(L"COM3"))
        return -1;

    hexy.ZeroServos();
    Sleep(1000);

    hexy.Stand();

    while (true)
    {
        Sleep(1000);
        hexy.WalkForward();
    }

    return 0;
}