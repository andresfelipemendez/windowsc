@echo off
del test.exe

set CompilerFlags= -Wno-missing-declarations -Wno-invalid-token-paste -Wno-unused-value -Wno-incompatible-pointer-types -Wno-int-conversion -Wno-implicit-function-declaration -Wno-nonportable-include-path -Wno-macro-redefined -std=c99 -x c

set LinkerDirs= /LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.18362.0\um\x64" /LIBPATH:"C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x64"
set LinkerFlags= /link %LinkerDirs% User32.lib Gdi32.lib dxguid.lib DXGI.lib D3D11.lib D3DX11.lib d3dcompiler.lib D3DX10.lib
set IncludeDirs=  /I"C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include" /I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\um"



REM clang %IncludeDirs% %LinkerDirs% %LinkerFlags% %CompilerFlags% test.c 
cl %IncludeDirs% test.c %LinkerFlags%
test.exe