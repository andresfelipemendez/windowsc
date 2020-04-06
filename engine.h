#pragma once

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>

#define PI (3.14159265358979323846)

typedef struct {
    bool IsInitialized;

    uint32_t PermamentStoreageSize;
    void *PermamentStoreage;

    uint32_t TransientStoreageSize;
    void *TransientStoreage;

}GameMemory;

typedef struct {
} GameInput;

typedef struct
{
} GameOffscreenBuffer;

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

#define RENDER_FRAME(name) void name()
typedef RENDER_FRAME(render_frame);
RENDER_FRAME(EngineRenderFrameStub)
{
}
// static render_frame *RenderFrame_ = RenderFrameStub;
// #define RenderFrame RenderFrame_

//void RenderFrame();

void Clear(float* color);

void SetWorldViewProojectionMatrix(vector3 up, vector3 position, vector3 lookAt, vector3 rot);

void SetVertexBuffer(unsigned int *stride, unsigned int *offset);

void Draw();