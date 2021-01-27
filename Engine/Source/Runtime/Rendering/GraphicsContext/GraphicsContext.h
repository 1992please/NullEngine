#pragma once
#include "Core/CoreTypes.h"

class IGraphicsContext
{
public:
	virtual ~IGraphicsContext() = default;

	virtual void Init() = 0;
	virtual void SwapBuffers() = 0;

	static IGraphicsContext* Create(void* InWindow);
};