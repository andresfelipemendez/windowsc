#include <windows.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint8_t uint8;
typedef uint32_t uint32;

static bool Running;
static BITMAPINFO BitmapInfo;
static void *BitmapMemory;
static int BitMapWidth;
static int BitMapHeight;

void DrawGradient(int XOffset, int YOffset )
{
    int BytesPerPixel = 4;
    int BitmapMemorySize = (BitMapWidth * BitMapHeight) * BytesPerPixel;
    BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

    int Pitch = BitMapWidth * BytesPerPixel;
    uint8 *Row = (uint8 *)BitmapMemory;

    for (int y = 0; y < BitMapHeight; ++y)
    {
        uint8 *Pixel = (uint8 *)Row;
        for (int x = 0; x < BitMapWidth; ++x)
        {
            /*

            */
            *Pixel = (uint8)x + XOffset;
            ++Pixel;

            *Pixel = (uint8)y + YOffset;
            ++Pixel;

            *Pixel = 0;
            ++Pixel;

            *Pixel = 0;
            ++Pixel;
        }
        Row += Pitch;
    }
}

void Win32ResizeDIBSection(int Width, int Height)
{
    if (BitmapMemory)
    {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }

    BitMapWidth = Width;
    BitMapHeight = Height;

    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = Width; 
    BitmapInfo.bmiHeader.biHeight = Height;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;

}
void
Win32UpdateWindow(HDC DeviceContext, RECT * WindowRect, int X,int Y,int Width,int Height)
{

    int WindowHeight = WindowRect->bottom - WindowRect->top;
    int WindowWidth = WindowRect->right - WindowRect->left;

    StretchDIBits(
        DeviceContext,
        // X, Y, Width, Height,
        // X, Y, Width, Height,
        0, 0, BitMapWidth, BitMapHeight,
        0, 0, WindowWidth, WindowHeight,
        BitmapMemory,
        &BitmapInfo,
        DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK
Win32MainWindowCallback(HWND Window,
                   UINT Message,
                   WPARAM WParam,
                   LPARAM LParam)
{
    LRESULT Result = 0;
    switch (Message)
    {
        case WM_SIZE:
        {
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            int Width = ClientRect.right - ClientRect.left;
            int Height = ClientRect.bottom - ClientRect.top;
            Win32ResizeDIBSection(Width, Height);
            OutputDebugStringA("WM_SIZE");
        }
        break;

        case WM_CLOSE:
        {
            Running = false;
        }
        break;

        case WM_DESTROY:
        {
            Running = false;
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

            RECT ClientRect;
            GetClientRect(Window, &ClientRect);

            Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);

            EndPaint(Window, &Paint);
            OutputDebugStringA("WM_ACTIVATEAPP");
        }
        break;

        default:
        {
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
    wnd.lpfnWndProc = Win32MainWindowCallback;
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
        system("PAUSE");
        return -1;
    }

    ShowWindow(WindowHandle, ShowCode);
    int XOffset = 0;
    int YOffset = 0;
    
    Running = true;
    while (Running > 0)
    {
       
        MSG Message;
        while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE) > 0)
        {
            if(Message.message == WM_QUIT) {
                Running = false;
            }
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }

        DrawGradient(XOffset, YOffset);

        HDC DeviceContext = GetDC(WindowHandle);
        RECT ClientRect;
        GetClientRect(WindowHandle, &ClientRect);
        int Height = ClientRect.bottom - ClientRect.top;
        int Width = ClientRect.right - ClientRect.left;
        Win32UpdateWindow(DeviceContext, &ClientRect, 0, 0, Width, Height);
        ReleaseDC(WindowHandle, DeviceContext);

        ++XOffset;
        ++YOffset;
    }

    return 0;
}