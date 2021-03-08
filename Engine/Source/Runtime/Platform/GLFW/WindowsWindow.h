#pragma once
#include "Core/Application/ApplicationWindow.h"

class FWindowsWindow : public IApplicationWindow
{
public:
	FWindowsWindow(const FWindowDetails& InDetails);
	virtual ~FWindowsWindow();

	void OnUpdate() override;

	// Window attributes
	void SetVSync(bool enabled) override;

	virtual void* GetNativeWindow() const override { return glfwWindow; }
	virtual void* GetOSWindow() const override;
private:
	virtual void Init(const FWindowDetails& InDetails);
	virtual void Shutdown();
private:
	struct GLFWwindow* glfwWindow;
	class IGraphicsContext* GraphicsContext;
};
