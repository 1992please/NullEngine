#pragma once
#include "Core/CoreTypes.h"
#include "Renderer/Components/VertexArray.h"
#include "Core/Math/Color.h"

class IRendererAPI
{
public:
    enum EType
    {
        Type_None = 0,
        Type_OpenGL = 1
    };

	virtual void InitRendererAPI() = 0;
	virtual void SetClearColor(const FLinearColor& Color) = 0;
	virtual void Clear() = 0;
	virtual void DrawIndexed(const IVertexArray* InVertexArray) = 0;

	FORCEINLINE static EType GetAPI() { return API; }
private:
    static EType API;

};