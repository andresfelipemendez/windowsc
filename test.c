#include <windows.h>

static int Running;

LRESULT CALLBACK
MainWindowCallback(HWND Window,
                   UINT Message,
                   WPARAM WParam,
                   LPARAM LParam)
{
    LRESULT Result = 0;
    switch (Message)
    {
        case WM_SIZE:
        {
            OutputDebugStringA("WM_SIZE");
        }
        break;

        case WM_CLOSE:
        {
            Running = 0;
        }
        break;

        case WM_DESTROY:
        {
            Running = 0;
        }
        break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP");
        }
        break;

        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            int Width = Paint.rcPaint.right - Paint.rcPaint.left;

            static DWORD Operation = WHITENESS;

            PatBlt(DeviceContext, X, Y, Width, Height, Operation);
            if(Operation == WHITENESS) {
                Operation = BLACKNESS;
            } else  {
                Operation = WHITENESS;
            }

            EndPaint(Window, &Paint);
            OutputDebugStringA("WM_ACTIVATEAPP");
        }
        break;

        default:
        {
            //    OutputDebugStringA("default");
            Result = DefWindowProc(Window, Message, WParam, LParam);
        }
        break;
    }
    return (Result);
}

int CALLBACK WinMain(HINSTANCE Instance,
                     HINSTANCE PrevInstance,
                     LPSTR CommandLine,
                     int ShowCode)
{
    WNDCLASS wnd;
    wnd.cbClsExtra = 0;
    wnd.cbWndExtra = 0;
    wnd.hCursor = LoadCursor(0, IDC_ARROW);
    wnd.hIcon = LoadIcon(0, IDI_WINLOGO);
    wnd.lpszMenuName = 0;
    wnd.style = 0;
    wnd.hbrBackground = 0;
    wnd.lpfnWndProc = MainWindowCallback;
    wnd.hInstance = Instance;
    wnd.lpszClassName = "Window Class";

    RegisterClass(&wnd);

    HWND WindowHandle = CreateWindowEx(
        0,
        "Window Class",
        "Windows Programming",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        Instance,
        NULL);

    if (WindowHandle == NULL)
    {
        //Pause
        system("PAUSE");
        return -1;
    }

    ShowWindow(WindowHandle, ShowCode);

    Running = 1;
    MSG Message;
    while (Running > 0)
    {
        BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
        if (MessageResult > 0)
        {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
        else
        {
            break;
        }
    }

    return 0;
}