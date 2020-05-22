#include <windows.h>
#include <d3d11.h>
#include <D3DX11async.h>
#include <XInput.h>
#include <d3dx10math.h>
#include "engine.h"

typedef struct
{
    D3DXMATRIX world;
    D3DXMATRIX view;
    D3DXMATRIX projection;
} MatrixBufferType;

typedef struct
{
    HMODULE gameCodeDLL;
    FILETIME DLLLastWriteTime;
    render_frame *RenderFrame;
} win32_engine_code;

typedef struct {
    float Pos[3];
    float Col[3];
} SimpleVertexCombined;

ID3D11Device *d3ddev;
ID3D11DeviceContext *d3dctx;
IDXGISwapChain *sc;

ID3D11Texture2D *d3dbb;
ID3D11RenderTargetView *view;

ID3D11InputLayout *m_layout;
ID3D11VertexShader *pVS;
ID3D11PixelShader *pPS;

ID3D11Buffer *pVBuffer;

ID3D11Buffer *g_pIndexBuffer = NULL;
ID3D11Buffer *g_pVertexBuffer = NULL;

ID3D11Buffer *m_matrixBuffer;

LARGE_INTEGER endCounter;
LARGE_INTEGER beginCounter;

float mxperframe = 0;
float posx = 0;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

DEBUG_PLATFORM_PRINT_CONSOLE(DEBUGPlatformPrintConsole)
{
    OutputDebugStringA(message);
}

SET_CLEAR_COLOR(SETClearColor)
{
    d3dctx->lpVtbl->ClearRenderTargetView(d3dctx, view, color);
}

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
X_INPUT_SET_STATE(XInputSetStateStub)
{
    return (0);
}
static x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_



inline FILETIME Win32GetLastWriteTime(char *Filename)
{
    FILETIME lastWriteTime = {0};
    WIN32_FIND_DATA FindData;
    HANDLE FindHandle = FindFirstFile(Filename, &FindData);
    if(FindHandle != INVALID_HANDLE_VALUE)
    {
        lastWriteTime = FindData.ftLastWriteTime;
        FindClose(FindHandle);
    }
    return (lastWriteTime);
}

static win32_engine_code Wind32LoadGame(void)
{
    win32_engine_code engineMethods = {0};

    char *SourceDLLName = "engine.dll";
    char *TempDLLName = "engine_temp.dll";

    engineMethods.DLLLastWriteTime = Win32GetLastWriteTime(SourceDLLName);
    CopyFile(SourceDLLName, TempDLLName, false);

    engineMethods.gameCodeDLL = LoadLibrary(TempDLLName);
    if (engineMethods.gameCodeDLL)
    {
        engineMethods.RenderFrame = (render_frame *)GetProcAddress(engineMethods.gameCodeDLL, "RenderFrame");
    } else {
        OutputDebugStringA("Load failed engine lib");
    }

    if (engineMethods.RenderFrame != NULL)
    {
    } else {
        OutputDebugStringA("failure  Loaded render function");
    }

    return (engineMethods);
}

void UnloadGameCode(win32_engine_code *enginemethods)
{
    FreeLibrary(enginemethods->gameCodeDLL);
    enginemethods->RenderFrame = NULL;
}

void Wind32LoadXInput(void)
{
    
    HMODULE XIinputLibrary = LoadLibrary("xinput1_3.dll");
    if (XIinputLibrary)
    {
        XInputGetState = (x_input_get_state *)GetProcAddress(XIinputLibrary, "XInputGetState");
        XInputSetState = (x_input_set_state *)GetProcAddress(XIinputLibrary, "XInputGetState");
    }
}

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
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            uint32_t VKCode = WParam;
            bool wasdown = ((LParam & (1 << 30)) != 0);
            bool isdown = ((LParam & (1 << 31)) == 0);
            switch (VKCode)
            {
                case 'W':

                    break;
                case 'A':
                    posx += mxperframe;
                    break;
                case 'S':

                    break;
                case 'D':
                    posx -= mxperframe;
                    break;
                case 'Q':

                    break;
                case 'E':

                    break;
                case VK_UP:

                    break;
                case VK_LEFT:

                    break;
                case VK_DOWN:

                    break;
                case VK_RIGHT:

                    break;
                case VK_ESCAPE:
                    PostQuitMessage(0);
                    return 0;
                case VK_SPACE:

                    break;

                default:
                    break;
            }
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
            {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
        };

    d3ddev->lpVtbl->CreateInputLayout(d3ddev, ied, 2, VS->lpVtbl->GetBufferPointer(VS), VS->lpVtbl->GetBufferSize(VS), &m_layout);
    d3dctx->lpVtbl->IASetInputLayout(d3dctx, m_layout);
}

void InitGraphics()
{
    unsigned int indices[] = {0, 1, 2};

    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(unsigned int) * 3;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = indices;
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;

    HRESULT hr = d3ddev->lpVtbl->CreateBuffer(d3ddev, &bufferDesc, &InitData, &g_pIndexBuffer);
    if (FAILED(hr))
    {
        checkres(hr);
    }

    SimpleVertexCombined verticesCombo[3] =
    {
        {{0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 0.5f}},
        {{0.5f, -0.5f, 0.0f}, {0.5f, 0.0f, 0.5f}},
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.5f, 0.5f}}
    };

    //D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = sizeof(SimpleVertexCombined) * 3;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    

    //D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = verticesCombo;
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;

    hr = d3ddev->lpVtbl->CreateBuffer(d3ddev, &bufferDesc, NULL, &g_pVertexBuffer);

    if (FAILED(hr))
    {
        checkres(hr);
    }

    D3D11_MAPPED_SUBRESOURCE ms;
    d3dctx->lpVtbl->Map(d3dctx, g_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
    memcpy(ms.pData, verticesCombo, sizeof(verticesCombo));
    d3dctx->lpVtbl->Unmap(d3dctx, g_pVertexBuffer, NULL);
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

void SetShaderParameters(
    D3DXMATRIX worldMatrix,
    D3DXMATRIX viewMatrix,
    D3DXMATRIX projectionMatrix)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBufferType *dataPtr;
    unsigned int bufferNumber;

    D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
    D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
    D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

    result = d3dctx->lpVtbl->Map(d3dctx, m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    dataPtr = (MatrixBufferType *)mappedResource.pData;

    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    d3dctx->lpVtbl->Unmap(d3dctx, m_matrixBuffer, 0);
    bufferNumber = 0;
    d3dctx->lpVtbl->VSSetConstantBuffers(d3dctx, bufferNumber, 1, &m_matrixBuffer);
}

SET_WORLD_VIEW_PROJECTION_MATRIX(SETWorldViewProjectionMatrix)
{

    D3DMATRIX viewMatrix, projectionMatrix, worldMatrix;
    D3DMATRIX rotationMatrix;

    D3DXMatrixRotationYawPitchRoll(&rotationMatrix, rot.x, rot.y, rot.z);

    D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
    D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

    lookAt = add(position, lookAt);

    D3DXMatrixLookAtLH(&viewMatrix, &position, &lookAt, &up);

    float fieldOfView = (float)PI / 4.0f;
    float screenAspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
    D3DXMatrixPerspectiveFovLH(&projectionMatrix, fieldOfView, screenAspect, 0.03f, 100.0f);

    D3DXMatrixIdentity(&worldMatrix);

    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;
    HRESULT result = d3ddev->lpVtbl->CreateBuffer(d3ddev, &matrixBufferDesc, NULL, &m_matrixBuffer);

    SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix);
}

SET_VERTEX_BUFFER(SETVertexBuffer)
{
    unsigned int off = 0;
    unsigned int str = sizeof(SimpleVertexCombined);
    d3dctx->lpVtbl->IASetIndexBuffer(d3dctx, g_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    d3dctx->lpVtbl->IASetVertexBuffers(d3dctx, 0, 1, &g_pVertexBuffer, &str, &off);
    d3dctx->lpVtbl->IASetPrimitiveTopology(d3dctx, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

DRAW(Draw) 
{
    d3dctx->lpVtbl->Draw(d3dctx, 3, 0);

    HRESULT res = sc->lpVtbl->Present(sc, 0, 0);
    if (res != S_OK)
    {
        OutputDebugStringA("Present failed");
        return;
    }
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

int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode)
{
    GameMemory gameMemory;
    gameMemory.DEBUGPlatformPrintConsole = DEBUGPlatformPrintConsole;
    gameMemory.SETClearColor = SETClearColor;
    gameMemory.SETWorldViewProjectionMatrix = SETWorldViewProjectionMatrix;
    gameMemory.SETVertexBuffer = SETVertexBuffer;
    gameMemory.Draw = Draw;

        LARGE_INTEGER perfCountFreq;
    QueryPerformanceFrequency(&perfCountFreq);
    int64_t perfCountFrequency = perfCountFreq.QuadPart;

    win32_engine_code enginemethods = Wind32LoadGame();
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

    OutputDebugStringA("start");
    QueryPerformanceCounter(&beginCounter);
    MSG Message = {0};
    uint32_t LoadCounter = 0;
    while(TRUE)
    {
        if (LoadCounter++ > 120)
        {
            UnloadGameCode(&enginemethods);
            enginemethods = Wind32LoadGame();
            LoadCounter = 0;
        }

        if (PeekMessage(&Message, 0, 0, 0, PM_REMOVE) > 0)
        {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
            if (Message.message == WM_QUIT)
            {
                break;
            }
        }

        for (DWORD ControllerIndex = 0; ControllerIndex < 4; ControllerIndex++)
        {
            XINPUT_STATE state;
            if(XInputGetState(ControllerIndex,&state) == ERROR_SUCCESS)
            {
                XINPUT_GAMEPAD *pad = &state.Gamepad;

                int16_t x = pad->sThumbLX;
                int16_t y = pad->sThumbLY;
                char str[80];

                sprintf(str, "My variable is %d %d\n", x, y);

            } else {
                //OutputDebugStringA("controller not connected");
            }
        }

        enginemethods.RenderFrame(&gameMemory);

        QueryPerformanceCounter(&endCounter);
        float elapsed = (float)(endCounter.QuadPart - beginCounter.QuadPart);
        mxperframe = ((1000.0f * elapsed) / (float)perfCountFrequency);
        mxperframe = mxperframe < 33.33f ? mxperframe : 33.33f; 
        char buffer[256];
      //  sprintf(buffer, "Millisecond/frame: %f ms\n", mxperframe);
        OutputDebugStringA(buffer);

        beginCounter = endCounter;
    }

    CleanD3D();

    return Message.wParam;
}

