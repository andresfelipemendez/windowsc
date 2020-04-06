@echo off
del a.exe
set CompilerFlags= -Wno-missing-declarations -Wno-invalid-token-paste -Wno-unused-value -Wno-incompatible-pointer-types -Wno-int-conversion -Wno-implicit-function-declaration -Wno-nonportable-include-path -Wno-macro-redefined -std=c99 -x c
set LinkerFlags= -lUser32 -lGdi32 -ldxguid -lDXGI -lD3D11 -lD3DX11 -lD3DCompiler -lD3DX10
set LinkerDirs= -L"C:\Program Files (x86)\Windows Kits\10\Lib" -L"C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x64"
set IncludeDirs= -I"C:\Program Files (x86)\Windows Kits\10\Include" -I"C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include"


clang %IncludeDirs% %LinkerDirs% %LinkerFlags% %CompilerFlags% -shared -o Test.dll engine.c 
clang %IncludeDirs% %LinkerDirs% %LinkerFlags% %CompilerFlags% test.c 
a.exe