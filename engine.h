#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>

#define DllExport __declspec(dllexport)
#define PI (3.14159265358979323846)

typedef struct
{
    float X, Y, Z;
    float color[4];
} VERTEX;

typedef struct {
    union {
        struct {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
        float m[4][4];
    };
} matrix;

typedef struct {
    float x, y ,z;
} vector3;

vector3 add(vector3 a, vector3 b)
{
    vector3 r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    r.z = a.z + b.z;
    return r;
}

#define SET_CLEAR_COLOR(name) void name(float *color)
typedef SET_CLEAR_COLOR(set_clear_color);

#define DEBUG_PLATFORM_PRINT_CONSOLE(name) void name(char *message)
typedef DEBUG_PLATFORM_PRINT_CONSOLE(debug_platform_print_console);

typedef struct {
    debug_platform_print_console *DEBUGPlatformPrintConsole;
    set_clear_color *SETClearColor;
} GameMemory ;

#define RENDER_FRAME(name) DllExport void name(GameMemory* memory)
typedef RENDER_FRAME(render_frame);

void SetWorldViewProojectionMatrix(vector3 up, vector3 position, vector3 lookAt, vector3 rot);

void SetVertexBuffer(unsigned int *stride, unsigned int *offset);

void Draw();