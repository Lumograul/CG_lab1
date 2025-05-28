#pragma once

#include <windows.h>
#include <iostream>

#include "InputDevice.h"


class DisplayWin32 {
public:
    HWND hWnd;
    HINSTANCE hInstance;
    int ClientWidth;
    int ClientHeight;

    DisplayWin32(LPCWSTR applicationName, int width, int height);
    ~DisplayWin32();

    void setInputDevice(InputDevice* inpDevice);

    InputDevice* inputDevice = nullptr;
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
};
