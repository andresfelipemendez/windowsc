#include <windows.h>
#include <d3d11.h>

#include <stdbool.h>
#include <stdint.h>

ID3D11Device *d3ddev;
ID3D11DeviceContext *d3dctx;
IDXGISwapChain *sc;

ID3D11Texture2D *d3dbb;
ID3D11RenderTargetView *view;

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
    IDXGIFactory *factory;
    IDXGIAdapter *adapter;
    IDXGIOutput *output;
    DXGI_OUTPUT_DESC od;

    CreateDXGIFactory(&IID_IDXGIFactory, &factory);
    factory->lpVtbl->EnumAdapters(factory, 0, &adapter);
    adapter->lpVtbl->EnumOutputs(adapter, 0, &output);
    output->lpVtbl->GetDesc(output, &od);
    output->lpVtbl->Release(output);

    DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    scd.BufferCount = 1;                                
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  
    scd.OutputWindow = hWnd;                            
    scd.SampleDesc.Count = 4;                           
    scd.Windowed = TRUE;

    D3D_FEATURE_LEVEL FeatureLevelsRequested = D3D_FEATURE_LEVEL_11_0;
    UINT numLevelsRequested = 1;
    D3D_FEATURE_LEVEL FeatureLevelsSupported;
    HRESULT hr;
    hr = D3D11CreateDeviceAndSwapChain(NULL,
                                       D3D_DRIVER_TYPE_HARDWARE,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       D3D11_SDK_VERSION,
                                       &scd,
                                       &sc,
                                       &d3ddev,
                                       NULL,
                                       &d3dctx);
    

    hr = sc->lpVtbl->GetBuffer(sc, 0, &IID_ID3D11Texture2D, (void **)&d3dbb);
    if (FAILED(hr))
    {
        OutputDebugStringA("GetBuffer FAILED");
        return;
    }
    
    D3D11_RENDER_TARGET_VIEW_DESC vd;
    D3D11_VIEWPORT vp;
    vd.Format = DXGI_FORMAT_UNKNOWN;
    vd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    vd.Texture2D.MipSlice = 0;
    hr = d3ddev->lpVtbl->CreateRenderTargetView(d3ddev, d3dbb, NULL, &view);

    d3dctx->lpVtbl->OMSetRenderTargets(d3dctx, 1, &view, NULL);
    vp.Width = 640;
    vp.Height = 480;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    d3dctx->lpVtbl->RSSetViewports(d3dctx, 1, &vp);
}

void CleanD3D()
{
    sc->lpVtbl->Release(sc);
    view->lpVtbl->Release(view);
    d3ddev->lpVtbl->Release(d3ddev);
    d3dctx->lpVtbl->Release(d3dctx);
}

void RenderFrame()
{
    float color[4] = {255,255,0,255};
    d3dctx->lpVtbl->ClearRenderTargetView(d3dctx, view, color);

    HRESULT res = sc->lpVtbl->Present(sc, 0, 0);
    if (res != S_OK)
    {
        OutputDebugStringA("Present failed");
        return;
    }
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

/*
if (FAILED(hr))
{
    OutputDebugStringA("FAILED CreateRenderTargetView ");
    if (hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE)
    {
        OutputDebugStringA("DXGI_ERROR_NOT_CURRENTLY_AVAILABLE ");
    }

    if (hr == D3D11_CREATE_DEVICE_DEBUG)
    {
        OutputDebugStringA("DXGI_ERROR_NOT_CURRENTLY_AVAILABLE ");
    }

    switch (hr)
    {
    case D3D11_ERROR_FILE_NOT_FOUND:
        OutputDebugStringA("D3D11_ERROR_FILE_NOT_FOUND");
        break;
    case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS:
        OutputDebugStringA("D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS");
        break;
    case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS:
        OutputDebugStringA("D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS");
        break;
    case D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD:
        OutputDebugStringA("D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD");
        break;
    case E_FAIL:
        OutputDebugStringA("E_FAIL");
        break;
    case E_INVALIDARG:
        OutputDebugStringA("E_INVALIDARG");
        break;
    case E_OUTOFMEMORY:
        OutputDebugStringA("E_OUTOFMEMORY");
        break;
    case E_NOTIMPL:
        OutputDebugStringA("E_NOTIMPL");
        break;
    case S_FALSE:
        OutputDebugStringA("S_FALSE");
        break;
    default:
        OutputDebugStringA("??");
        break;
    }
}
*/