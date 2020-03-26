//https : //community.khronos.org/t/avoiding-the-default-framebuffer-blit-overhead/68813

#include <windows.h>

#define INITGUID
#include <d3d11.h>

#include <stdbool.h>
#include <stdint.h>

        ID3D11Device *d3ddev;
ID3D11DeviceContext *d3dctx;
IDXGISwapChain *sc;
DXGI_SWAP_CHAIN_DESC scd;
ID3D11Texture2D *d3dbb;
ID3D11RenderTargetView *view;

void InitD3D(HWND hWnd);
void CleanD3D(void);

typedef uint8_t uint8;
typedef uint32_t uint32;

static bool Running;
static BITMAPINFO BitmapInfo;
static void *BitmapMemory;
static int BitMapWidth;
static int BitMapHeight;

LRESULT CALLBACK
Win32MainWindowCallback(HWND Window,
                   UINT Message,
                   WPARAM WParam,
                   LPARAM LParam)
{
    LRESULT Result = 0;
    switch (Message)
    {
        case WM_DESTROY:
        {
            Running = false;
            PostQuitMessage(0);
            return 0;
        }
        break;
    }
    return DefWindowProc(Window, Message, WParam, LParam);
}

void InitD3D(HWND hWnd)
{
    OutputDebugStringA("D3D11CreateDeviceAndSwapChain");

    IDXGIFactory *factory;
    IDXGIAdapter *adapter;
    IDXGIOutput *output;
    DXGI_OUTPUT_DESC od;

    CreateDXGIFactory(&IID_IDXGIFactory, &factory);
    factory->lpVtbl->EnumAdapters(factory, 0, &adapter);
    adapter->lpVtbl->EnumOutputs(adapter, 0, &output);
    output->lpVtbl->GetDesc(output, &od);
    output->lpVtbl->Release(output);

    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    scd.BufferCount = 1;
    scd.BufferDesc.Width = 1920;
    scd.BufferDesc.Height = 1080;
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.SampleDesc.Count = 4;
    scd.OutputWindow = hWnd;
    scd.Windowed = TRUE;

    D3D11CreateDeviceAndSwapChain(  
        adapter,
        D3D_DRIVER_TYPE_UNKNOWN,
        NULL,
        D3D11_CREATE_DEVICE_SINGLETHREADED,    
        NULL,
        0,
        D3D11_SDK_VERSION,
        &scd,
        &sc,
        &d3ddev,
        NULL,
        &d3dctx);

    sc->lpVtbl->GetBuffer(sc, 0, &IID_ID3D11Texture2D, (void **)&d3dbb);

    D3D11_RENDER_TARGET_VIEW_DESC vd;
    D3D11_VIEWPORT vp;
    vd.Format = DXGI_FORMAT_UNKNOWN;
    vd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    vd.Texture2D.MipSlice = 0;
    d3ddev->lpVtbl->CreateRenderTargetView(d3ddev, d3dbb, &vd, &view);

    d3dctx->lpVtbl->OMSetRenderTargets(d3dctx, 1, &view, NULL);
    vp.TopLeftX = vp.TopLeftY = 0;
    vp.Width = 1920;
    vp.Height = 1080;
    vp.MinDepth = 0;
    vp.MaxDepth = 1;
    d3dctx->lpVtbl->RSSetViewports(d3dctx, 1, &vp);
}

void RenderFrame(void){
    float color[4] = {rand() % 256 * (1.0f / 256), 0, 0, 1};
    d3dctx->lpVtbl->ClearRenderTargetView(d3dctx, view, color);
    sc->lpVtbl->Present(sc, 0, 0);
   
}

void CleanD3D()
{
    sc->lpVtbl->Release(sc);
    view->lpVtbl->Release(view);
    d3ddev->lpVtbl->Release(d3ddev);
    d3dctx->lpVtbl->Release(d3dctx);
}

int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode)
{
    HWND WindowHandle;
    WNDCLASSEX wnd;
    ZeroMemory(&wnd, sizeof(WNDCLASSEX));

    wnd.cbSize = sizeof(WNDCLASSEX);
    wnd.style = CS_HREDRAW | CS_VREDRAW;
    wnd.lpfnWndProc = Win32MainWindowCallback;
    wnd.hInstance = Instance;
    wnd.hCursor = LoadCursor(0, IDC_ARROW);
    wnd.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wnd.lpszClassName = "Window Class";

    RegisterClassEx(&wnd);

    RECT wr = {0, 0, 1920, 1080};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    WindowHandle = CreateWindowEx(
        0,
        "Window Class",
        "Windows Programming",
        WS_OVERLAPPEDWINDOW,
        0,
        0,
        wr.right - wr.left,
        wr.bottom - wr.top,
        NULL,
        NULL,
        Instance,
        NULL);

    if (WindowHandle == NULL)
    {
        system("PAUSE");
        return -1;
    }

    InitD3D(WindowHandle);

    ShowWindow(WindowHandle, ShowCode);

    MSG Message = {0};
    while(TRUE)
    {
        if (PeekMessage(&Message, 0, 0, 0, PM_REMOVE) > 0)
        {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
            if(Message.message == WM_QUIT) {
                break;
            }
        } else {
            
        }

        RenderFrame();
    }

    CleanD3D();

    return Message.wParam;
}