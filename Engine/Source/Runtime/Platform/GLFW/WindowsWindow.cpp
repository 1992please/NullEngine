#include "WindowsWindow.h"
#include "Core/Logging/Logger.h"
#include "Core/Delegates/Delegate.h"
#include "Core/Events/ApplicationEvent.h"
#include "Core/Events/MouseEvent.h"
#include "Core/Events/KeyEvent.h"


static uint8_t s_GLFWWindowCount = 0;


static void GLFWErrorCallback(int error, const char* description)
{
	NE_CORE_ERROR("GLFW Error (%d): %s", error, description);
}


FWindow* FWindow::Create(const FWindowDetails& InDetails)
{
	return new FWindowsWindow(InDetails);
}

FWindowsWindow::FWindowsWindow(const FWindowDetails& InDetails)
{
	Init(InDetails);
}

FWindowsWindow::~FWindowsWindow()
{
	Shutdown();
}

void FWindowsWindow::OnUpdate()
{
	glfwPollEvents();
	glfwSwapBuffers(glfwWindow);
}

void FWindowsWindow::SetVSync(bool enabled)
{
	if (enabled)
		glfwSwapInterval(1);
	else
		glfwSwapInterval(0);

	bVSync = enabled;
}

void FWindowsWindow::Init(const FWindowDetails& InDetails)
{
	Width = InDetails.Width;
	Height = InDetails.Height;
	Title = InDetails.Title;

	NE_CORE_LOG("Creating window %s (%d, %d)", *(Title), Width, Height);


	if (s_GLFWWindowCount == 0)
	{
		int success = glfwInit();
		NE_ASSERT_F(success, "Could not initialize GLFW!");
		glfwSetErrorCallback(GLFWErrorCallback);
	}
	glfwWindow = glfwCreateWindow((int)Width, (int)Height, *(Title), nullptr, nullptr);
	glfwMakeContextCurrent(glfwWindow);
	glfwSetWindowUserPointer(glfwWindow, this);
	SetVSync(true);

	// set glfw callbacks
	glfwSetWindowSizeCallback(glfwWindow, [](GLFWwindow* InWindow, int InWidth, int InHeight)
	{
		FWindowsWindow* OurWindow = (FWindowsWindow*)glfwGetWindowUserPointer(InWindow);
		OurWindow->Width = InWidth;
		OurWindow->Height = InHeight;

		FWindowResizeEvent E(InWidth, InHeight);
		OurWindow->WindowEventCallback.Execute(E);

	});

	glfwSetWindowCloseCallback(glfwWindow, [](GLFWwindow* InWindow)
	{
		FWindowsWindow* OurWindow = (FWindowsWindow*)glfwGetWindowUserPointer(InWindow);
		FWindowCloseEvent E;
		OurWindow->WindowEventCallback.Execute(E);
	});

	glfwSetKeyCallback(glfwWindow, [](GLFWwindow* InWindow, int key, int scanCode, int action, int mods)
	{
		FWindowsWindow* OurWindow = (FWindowsWindow*)glfwGetWindowUserPointer(InWindow);
		switch (action)
		{
			case GLFW_PRESS:
			{
				FKeyPressedEvent E(key, 0);
				OurWindow->WindowEventCallback.Execute(E);
			}
			break;
			case GLFW_RELEASE:
			{
				FKeyReleasedEvent E(key);
				OurWindow->WindowEventCallback.Execute(E);
			}
			break;
			case GLFW_REPEAT:
			{
				FKeyPressedEvent E(key, 1);
				OurWindow->WindowEventCallback.Execute(E);
			}
			break;
		}
	});

	glfwSetMouseButtonCallback(glfwWindow, [](GLFWwindow* InWindow, int button, int action, int mods)
	{
		FWindowsWindow* OurWindow = (FWindowsWindow*)glfwGetWindowUserPointer(InWindow);
		switch (action)
		{
			case GLFW_PRESS:
			{
				FMouseButtonPressedEvent E(button);
				OurWindow->WindowEventCallback.Execute(E);
			}
			break;
			case GLFW_RELEASE:
			{
				FMouseButtonReleasedEvent E(button);
				OurWindow->WindowEventCallback.Execute(E);
			}
			break;
			case GLFW_REPEAT:
			{
				FMouseButtonPressedEvent E(button);
				OurWindow->WindowEventCallback.Execute(E);
			}
			break;
		}
	});

	glfwSetScrollCallback(glfwWindow, [](GLFWwindow* InWindow, double InXOffset, double InYOffset)
	{
		FWindowsWindow* OurWindow = (FWindowsWindow*)glfwGetWindowUserPointer(InWindow);
		FMouseScrolledEvent E((float)InXOffset, (float)InYOffset);
		OurWindow->WindowEventCallback.Execute(E);
	});

	glfwSetCursorPosCallback(glfwWindow, [](GLFWwindow* InWindow, double InXPos, double InYPos)
	{
		FWindowsWindow* OurWindow = (FWindowsWindow*)glfwGetWindowUserPointer(InWindow);
		FMouseMovedEvent E((float)InXPos, (float)InYPos);
		OurWindow->WindowEventCallback.Execute(E);
	});

}

void FWindowsWindow::Shutdown()
{
	glfwDestroyWindow(glfwWindow);
}
