#include <windows.h>
#include <d3d11.h>
#include <D3DX11async.h>
#include <XInput.h>

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>


ID3D11Device *d3ddev;
ID3D11DeviceContext *d3dctx;
IDXGISwapChain *sc;

ID3D11Texture2D *d3dbb;
ID3D11RenderTargetView *view;

ID3D11InputLayout *pLayout;
ID3D11VertexShader *pVS;
ID3D11PixelShader *pPS;
ID3D11Buffer *pVBuffer;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

#define X_INPUT_GET_STATE(name) DWORD name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
    return (0);
}
static x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

#define X_INPUT_SET_STATE(name) DWORD name(DWORD dwUserIndex, XINPUT_VIBRATION *pState)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_GET_STATE(XInputSetStateStub)
{
    return (0);
}
static x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

void Wind32LoadXInput(void)
{
    HMODULE XIinputLibrary = LoadLibrary("xinput1_3.dll");
    if (XIinputLibrary)
    {
        XInputGetState = (x_input_get_state*)GetProcAddress(XIinputLibrary, "XInputGetState");
        XInputSetState = (x_input_set_state*)GetProcAddress(XIinputLibrary, "XInputGetState");
    }
}

typedef struct
{
    float X, Y, Z;
    float color[4];
} VERTEX;

void checkres(HRESULT hr)
{
    if (FAILED(hr))
    {
        switch (hr)
        {
        case DXGI_ERROR_NOT_CURRENTLY_AVAILABLE:
            OutputDebugStringA("DXGI_ERROR_NOT_CURRENTLY_AVAILABLE");
            break;
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
    else
    {
        OutputDebugStringA("SUCCES!!!");
    }
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
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        break;
    }
    return DefWindowProc(Window, Message, WParam, LParam);
}

void InitPipeline()
{
    HRESULT hr;
    ID3D10Blob *VS, *PS;
    ID3D10Blob* msg = NULL;
    hr = D3DCompileFromFile(L"shader.shader", 0, 0, "VShader", "vs_4_0", 0, 0, &VS, &msg);
    hr = D3DCompileFromFile(L"shader.shader", 0, 0, "PShader", "ps_4_0", 0, 0, &PS, &msg);

    hr = d3ddev->lpVtbl->CreateVertexShader(d3ddev, VS->lpVtbl->GetBufferPointer(VS), VS->lpVtbl->GetBufferSize(VS), NULL, &pVS);
    d3ddev->lpVtbl->CreatePixelShader(d3ddev, PS->lpVtbl->GetBufferPointer(PS), PS->lpVtbl->GetBufferSize(PS), NULL, &pPS);

    d3dctx->lpVtbl->VSSetShader(d3dctx, pVS, 0, 0);
    d3dctx->lpVtbl->PSSetShader(d3dctx, pPS, 0, 0);

    D3D11_INPUT_ELEMENT_DESC ied[] =
        {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
        };

    d3ddev->lpVtbl->CreateInputLayout(d3ddev, ied, 2, VS->lpVtbl->GetBufferPointer(VS), VS->lpVtbl->GetBufferSize(VS), &pLayout);
    d3dctx->lpVtbl->IASetInputLayout(d3dctx, pLayout);
}

void InitGraphics()
{
    VERTEX OurVertices[3] = {
        {0.0f, 0.5f, 0.0f, {1.0f, 0.0f, 0.0f, 1.0f}},
        {0.45f, -0.5f, 0.0f, {0.0f, 1.0f, 0.0f, 1.0f}},
        {-0.45f, -0.5f, 0.0f, {0.0f, 0.0f, 1.0f, 1.0f}}};

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(VERTEX) * 3;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    d3ddev->lpVtbl->CreateBuffer(d3ddev, &bd, NULL, &pVBuffer);

    D3D11_MAPPED_SUBRESOURCE ms;
    d3dctx->lpVtbl->Map(d3dctx, pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
    memcpy(ms.pData, OurVertices, sizeof(OurVertices));
    d3dctx->lpVtbl->Unmap(d3dctx, pVBuffer, NULL);
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
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    D3D_FEATURE_LEVEL FeatureLevelsRequested = D3D_FEATURE_LEVEL_11_0;
    UINT numLevelsRequested = 1;
    D3D_FEATURE_LEVEL FeatureLevelsSupported;
    HRESULT hr;
    hr = D3D11CreateDeviceAndSwapChain(
        NULL,
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
    vp.Width = SCREEN_WIDTH;
    vp.Height = SCREEN_HEIGHT;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    d3dctx->lpVtbl->RSSetViewports(d3dctx, 1, &vp);


    InitPipeline();
    InitGraphics();
}

void CleanD3D()
{
    sc->lpVtbl->SetFullscreenState(sc, FALSE, NULL);

    pVS->lpVtbl->Release(pVS);
    pPS->lpVtbl->Release(pPS);

    sc->lpVtbl->Release(sc);
    view->lpVtbl->Release(view);
    d3ddev->lpVtbl->Release(d3ddev);
    d3dctx->lpVtbl->Release(d3dctx);
}

void RenderFrame()
{
    float color[4] = {255,255,0,255};
    d3dctx->lpVtbl->ClearRenderTargetView(d3dctx, view, color);

    UINT stride =sizeof(VERTEX);
    UINT offset = 0;
    d3dctx->lpVtbl->IASetVertexBuffers(d3dctx, 0, 1, &pVBuffer, &stride, &offset);

    d3dctx->lpVtbl->IASetPrimitiveTopology(d3dctx, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    d3dctx->lpVtbl->Draw(d3dctx, 3, 0);

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
    Wind32LoadXInput();

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
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
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
        }

        for (DWORD ControllerIndex = 0; ControllerIndex < 4; ControllerIndex++)
        {
            XINPUT_STATE state;
            if(XInputGetState(ControllerIndex,&state) == ERROR_SUCCESS)
            {
                OutputDebugStringA("connected");
                XINPUT_GAMEPAD *pad = &state.Gamepad;

                int16_t x = pad->sThumbLX;
                int16_t y = pad->sThumbLY;
                char str[80];

                sprintf(str, "My variable is %d %d\n", x, y);
                OutputDebugString(str);

                OutputDebugStringA(str);

            } else {
                //OutputDebugStringA("controller not connected");
            }
            /* code */
        }
        

        RenderFrame();
    }

    CleanD3D();

    return Message.wParam;
}

