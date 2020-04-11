@echo off

set CompilerFlags= -wd4005 -wd4133 -wd4047 -wd4024
set LinkerDirs= /LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.18362.0\um\x64" /LIBPATH:"C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x64"
set LinkerFlags= /link %LinkerDirs% User32.lib Gdi32.lib dxguid.lib DXGI.lib D3D11.lib D3DX11.lib d3dcompiler.lib D3DX10.lib engine.lib
set IncludeDirs=  /I"C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include" /I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\um"

cl %CompilerFlags% %IncludeDirs% Win32Main.c %LinkerFlags%
cl %IncludeDirs% engine.c /LD 

