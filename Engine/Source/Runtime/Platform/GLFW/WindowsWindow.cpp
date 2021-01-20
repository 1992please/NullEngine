#include "WindowsWindow.h"
#include "Logging/Logger.h"

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

	Data.VSync = enabled;
}

bool FWindowsWindow::IsVSync() const
{
	return Data.VSync;
}

void FWindowsWindow::Init(const FWindowDetails& InDetails)
{
	Data.Width = InDetails.Width;
	Data.Height = InDetails.Height;
	Data.Title = InDetails.Title;

	NE_CORE_LOG("Creating window %s (%d, %d)", *(Data.Title), Data.Width, Data.Height);


	if (s_GLFWWindowCount == 0)
	{
		int success = glfwInit();
		NE_ASSERT_F(success, "Could not initialize GLFW!");
		glfwSetErrorCallback(GLFWErrorCallback);
	}
	glfwWindow = glfwCreateWindow((int)Data.Width, (int)Data.Height, *(Data.Title), nullptr, nullptr);
	glfwMakeContextCurrent(glfwWindow);
	glfwSetWindowUserPointer(glfwWindow, &Data);
	SetVSync(true);
}

void FWindowsWindow::Shutdown()
{
	glfwDestroyWindow(glfwWindow);
}
