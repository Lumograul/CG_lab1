    #include "DisplayWin32.h"
    #include "Game.h"

DisplayWin32::DisplayWin32(LPCWSTR applicationName, int width, int height)
    : ClientWidth(width), ClientHeight(height) {

    hInstance = GetModuleHandle(nullptr);

    WNDCLASSEX wc = {};
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc; // Используем статическую функцию
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(DisplayWin32*);
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = applicationName;
    wc.cbSize = sizeof(WNDCLASSEX);

    RegisterClassEx(&wc);

    RECT windowRect = { 0, 0, static_cast<LONG>(ClientWidth), static_cast<LONG>(ClientHeight) };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME;
    int posX = (GetSystemMetrics(SM_CXSCREEN) - ClientWidth) / 2;
    int posY = (GetSystemMetrics(SM_CYSCREEN) - ClientHeight) / 2;

    hWnd = CreateWindowEx(
        WS_EX_APPWINDOW, applicationName, applicationName,
        dwStyle,
        posX, posY,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr, nullptr, hInstance, this
    );

    // Устанавливаем указатель на `this` в `HWND`
    SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);
    SetFocus(hWnd);
    ShowCursor(true);
}


    void DisplayWin32::setInputDevice(InputDevice* inpDevice) {
        inputDevice = inpDevice;
    }

    DisplayWin32::~DisplayWin32() {
        DestroyWindow(hWnd);
        UnregisterClass(L"My3DApp", hInstance);
    }

    LRESULT CALLBACK DisplayWin32::WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam) {    
        DisplayWin32* display = reinterpret_cast<DisplayWin32*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

        switch (umessage) {
        case WM_KEYDOWN:
            std::cout << "Key: " << static_cast<unsigned int>(wparam) << std::endl;
            if (static_cast<unsigned int>(wparam) == 27) PostQuitMessage(0);
            return 0;
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        case WM_INPUT:
        {
            UINT dwSize = 0;
            GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
            LPBYTE lpb = new BYTE[dwSize];
            if (lpb == nullptr) {
                return 0;
            }

            if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
                OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

            RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb);
         

        if (display && display->inputDevice) {
            if (raw->header.dwType == RIM_TYPEKEYBOARD)
            {
                /*printf(" Kbd: make=%04i Flags:%04i Reserved:%04i ExtraInformation:%08i, msg=%04i VK=%i \n",
                    raw->data.keyboard.MakeCode,
                    raw->data.keyboard.Flags,
                    raw->data.keyboard.Reserved,
                    raw->data.keyboard.ExtraInformation,
                    raw->data.keyboard.Message,
                    raw->data.keyboard.VKey)*/;
                display->inputDevice->OnKeyDown({
                    raw->data.keyboard.MakeCode,
                    raw->data.keyboard.Flags,
                    raw->data.keyboard.VKey,
                    raw->data.keyboard.Message
                    });
            }
            else if (raw->header.dwType == RIM_TYPEMOUSE)
            {
                //printf(" Mouse: X=%04d Y:%04d \n", raw->data.mouse.lLastX, raw->data.mouse.lLastY);
                display->inputDevice->OnMouseMove({
                    raw->data.mouse.usFlags,
                    raw->data.mouse.usButtonFlags,
                    static_cast<int>(raw->data.mouse.ulExtraInformation),
                    static_cast<int>(raw->data.mouse.ulRawButtons),
                    static_cast<short>(raw->data.mouse.usButtonData),
                    raw->data.mouse.lLastX,
                    raw->data.mouse.lLastY
                    });
            }
        }

            delete[] lpb;
            return DefWindowProc(hwnd, umessage, wparam, lparam);
        }
        default:
            return DefWindowProc(hwnd, umessage, wparam, lparam);
        }
    }
