#pragma once
#include "Core/CoreTypes.h"
#include "Rendering/GraphicsContext/GraphicsContext.h"

class FOpenGlContext : public IGraphicsContext
{
public:
	FOpenGlContext(void* InWindow);

	virtual void Init() override;
	virtual void SwapBuffers() override;
private:
	struct GLFWwindow* WindowHandle;
};