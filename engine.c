#include "engine.h"

void perspective(GameMemory* memory)
{   vector3 up, position, lookAt;
    vector3 rot;

    up.x = 0.0;
    up.y = 1.0;
    up.z = 0.0;

    position.x = 0;
    position.y = 0;
    position.z = -5.15;

    lookAt.x = 0.0f;
    lookAt.y = 0.0f;
    lookAt.z = 1.0f;

    rot.x = 0; //yaw
    rot.y = 0; //pitch
    rot.z = 0; //roll

    memory->SETWorldViewProjectionMatrix(up, position, lookAt, rot);
}

RENDER_FRAME(RenderFrame)
{
    float color[4] = {0.22f, 0.5f, 0.1f, 255};
    memory->SETClearColor(color);
    perspective(memory);

    unsigned int stride = sizeof(VERTEX);
    unsigned int offset = 0;
    memory->SETVertexBuffer(&stride, &offset);
    memory->Draw();
}
