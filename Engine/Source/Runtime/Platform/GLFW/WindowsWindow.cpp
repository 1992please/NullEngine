#include "NullPCH.h"
#include "WindowsWindow.h"
#include "Core/Events/ApplicationEvent.h"
#include "Core/Events/MouseEvent.h"
#include "Core/Events/KeyEvent.h"

#include "Renderer/Components/GraphicsContext.h"
#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

static uint8_t s_GLFWWindowCount = 0;


static void GLFWErrorCallback(int error, const char* description)
{
	NE_CORE_ERROR("GLFW Error (%d): %s", error, description);
}


IApplicationWindow* IApplicationWindow::Create(const FWindowDetails& InDetails)
{
	return new FWindowsWindow(InDetails);
}

FWindowsWindow::FWindowsWindow(const FWindowDetails& InDetails)
{
	NE_PROFILE_FUNCTION();

	Init(InDetails);
}

FWindowsWindow::~FWindowsWindow()
{
	NE_PROFILE_FUNCTION();

	Shutdown();
}

void FWindowsWindow::OnUpdate()
{
	NE_PROFILE_FUNCTION();

	glfwPollEvents();
	GraphicsContext->SwapBuffers();
}

void FWindowsWindow::SetVSync(bool enabled)
{
	NE_PROFILE_FUNCTION();

	if (enabled)
		glfwSwapInterval(1);
	else
		glfwSwapInterval(0);

	bVSync = enabled;
}

void* FWindowsWindow::GetOSWindow() const
{
	return glfwGetWin32Window(glfwWindow);
}

void FWindowsWindow::Init(const FWindowDetails& InDetails)
{
	NE_PROFILE_FUNCTION();

	Width = InDetails.Width;
	Height = InDetails.Height;
	Title = InDetails.Title;

	NE_CORE_LOG("Creating window %s (%d, %d)", *(Title), Width, Height);


	if (s_GLFWWindowCount == 0)
	{
		glfwSetErrorCallback(GLFWErrorCallback);
		int success = glfwInit();
		NE_CHECK_F(success, "Could not initialize GLFW!");
		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // required on mac
	}

	glfwWindow = glfwCreateWindow((int)Width, (int)Height, *(Title), nullptr, nullptr);
	glfwSetWindowUserPointer(glfwWindow, this);
	s_GLFWWindowCount++;

	GraphicsContext = IGraphicsContext::Create(glfwWindow);
	GraphicsContext->Init();

	
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
				FKeyPressedEvent E((EKeyCode) key, 0);
				OurWindow->WindowEventCallback.Execute(E);
			}
			break;
			case GLFW_RELEASE:
			{
				FKeyReleasedEvent E((EKeyCode)key);
				OurWindow->WindowEventCallback.Execute(E);
			}
			break;
			case GLFW_REPEAT:
			{
				FKeyPressedEvent E((EKeyCode)key, 1);
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
				FMouseButtonPressedEvent E((EKeyCode)button);
				OurWindow->WindowEventCallback.Execute(E);
			}
			break;
			case GLFW_RELEASE:
			{
				FMouseButtonReleasedEvent E((EKeyCode)button);
				OurWindow->WindowEventCallback.Execute(E);
			}
			break;
			case GLFW_REPEAT:
			{
				FMouseButtonPressedEvent E((EKeyCode)button);
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
	NE_PROFILE_FUNCTION();

	glfwDestroyWindow(glfwWindow);
}
