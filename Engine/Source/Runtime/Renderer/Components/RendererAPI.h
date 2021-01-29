#pragma once
#include "Core/CoreTypes.h"

class IRendererAPI
{
public:
    enum EType
    {
        Type_None = 0,
        Type_OpenGL = 1
    };

	FORCEINLINE static EType GetAPI() { return API; }

private:
    static EType API;
};