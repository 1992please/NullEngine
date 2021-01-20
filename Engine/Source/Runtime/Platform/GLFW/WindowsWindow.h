#pragma once
#include "Core/Window.h"
#include "GLFW/glfw3.h"

class FWindowsWindow : public FWindow
{
public:
	FWindowsWindow(const FWindowDetails& InDetails);
	virtual ~FWindowsWindow();

	void OnUpdate() override;

	unsigned int GetWidth() const override { return Data.Width; }
	unsigned int GetHeight() const override { return Data.Height; }

	// Window attributes
	void SetEventCallback(const EventCallbackFn& callback) override { Data.EventCallback = callback; }
	void SetVSync(bool enabled) override;
	bool IsVSync() const override;

	virtual void* GetNativeWindow() const { return glfwWindow; }
private:
	virtual void Init(const FWindowDetails& InDetails);
	virtual void Shutdown();
private:
	GLFWwindow* glfwWindow;

	struct WindowData
	{
		FString Title;
		unsigned int Width, Height;
		bool VSync;

		EventCallbackFn EventCallback;
	};

	WindowData Data;
};
