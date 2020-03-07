#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
) {
    MessageBoxA(0, "This is winc", "win c", MB_OK|MB_ICONINFORMATION);
    return 0;
}