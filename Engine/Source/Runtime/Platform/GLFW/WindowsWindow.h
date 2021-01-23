#pragma once
#include "Core/Window.h"

class FWindowsWindow : public FWindow
{
public:
	FWindowsWindow(const FWindowDetails& InDetails);
	virtual ~FWindowsWindow();

	void OnUpdate() override;

	// Window attributes
	void SetVSync(bool enabled) override;

	virtual void* GetNativeWindow() const { return glfwWindow; }
private:
	virtual void Init(const FWindowDetails& InDetails);
	virtual void Shutdown();
private:
	struct GLFWwindow* glfwWindow;
};
