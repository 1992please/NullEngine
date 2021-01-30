#pragma once

#include "Renderer/Components/RenderCommand.h"

class Renderer
{
public:
    static void BeginScene();
    static void EndScene();
    static void Submit(const IVertexArray* InVertexArray);
};