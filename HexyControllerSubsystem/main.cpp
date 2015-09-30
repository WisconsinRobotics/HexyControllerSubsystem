#include <Windows.h>
#include <stdio.h>

#include "Hexy.h"

Hexy hexy;

int main()
{
    if (!hexy.Initialize(L"COM3"))
        return -1;

    hexy.ZeroServos();

    return 0;
}