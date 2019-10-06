#include "EngineBase.h"
#include "GL/gl3w.h"
#include "GLFW/glfw3.h"

void EngineBase::KeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
{
	EngineBase::Engine->mInput.OnKeyboardAction(EKeyboardKey(key), EKeyState(action));
}

void EngineBase::MouseButtonCallback(GLFWwindow* pWindow, int Button, int Action, int Mode)
{
	EngineBase::Engine->mInput.OnMouseAction(EMouseKey(Button), EKeyState(Action));
}

void EngineBase::MouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	EngineBase::Engine->mInput.OnMouseWheel(static_cast<int>(yoffset));
}

void EngineBase::glfw_FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	EngineBase::Engine->OnResize(width, height);
}

//void EngineBase::glfw_ReSizeCallback(GLFWwindow* window, int width, int height)
//{
//	EngineBase::Engine->OnResize(width, height);
//}


void DebugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam)
{
	//reinterpret_cast<EngineBase *>(userParam)->OnDebugMessage(source, type, id, severity, length, message);
}

EngineBase* EngineBase::Engine = nullptr;

EngineBase::EngineBase() : CurrentTime(0), bIsRunning(false)
{
	if (!Engine)
	{
		Engine = this;
	}
	else
	{
		fprintf(stderr, "No Two instances of the same engine can run in the same app\n");
	}

	strcpy_s(EngineConfigs.title, "NULL ENGINE");
	EngineConfigs.windowWidth = 800;
	EngineConfigs.windowHeight = 800;
	EngineConfigs.windowPosX = 100;
	EngineConfigs.windowPosY = 100;
	EngineConfigs.majorVersion = 4;
	EngineConfigs.minorVersion = 3;
	EngineConfigs.samples = 0;
	EngineConfigs.flags.all = 0;
	EngineConfigs.flags.cursor = 1;
#ifdef _DEBUG
	EngineConfigs.flags.debug = 1;
#endif

}

void EngineBase::OnResize(uint32 Width, uint32 Height)
{
	EngineConfigs.windowWidth = Width;
	EngineConfigs.windowHeight = Height;
	glViewport(0, 0, Width, Height);
}

void EngineBase::Run()
{
	bIsRunning = true;

	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return;
	}

	Init();

	// Window Settings
	glfwWindowHint(GLFW_SAMPLES, EngineConfigs.samples);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, EngineConfigs.majorVersion);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, EngineConfigs.minorVersion);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_STEREO, EngineConfigs.flags.stereo ? GL_TRUE : GL_FALSE);
#ifdef _DEBUG
	{
		int Major, Minor, Rev;
		glfwGetVersion(&Major, &Minor, &Rev);
		printf("GLFW %d.%d.%d initialized\n", Major, Minor, Rev);
	}
#endif
	if(EngineConfigs.flags.debug)
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	if (EngineConfigs.flags.robust)
	{
		glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_LOSE_CONTEXT_ON_RESET);
	}

	// Create the Window
	Window = glfwCreateWindow(EngineConfigs.windowWidth, EngineConfigs.windowHeight, EngineConfigs.title, EngineConfigs.flags.fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);

	if (!Window) {
		fprintf(stderr, "Failed to open window\n");
		return;
	}

	glfwMakeContextCurrent(Window);

	glfwSetFramebufferSizeCallback(Window, glfw_FrameBufferSizeCallback);
	//glfwSetWindowSizeCallback(Window, glfw_ReSizeCallback);
	glfwSetKeyCallback(Window, KeyCallback);
	glfwSetMouseButtonCallback(Window, MouseButtonCallback);
	glfwSetScrollCallback(Window, MouseWheelCallback);

	if (!EngineConfigs.flags.cursor)
	{
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}

	// GL Initaition
	gl3wInit();
	glfwSwapInterval(0);
	// get version info
#ifdef _DEBUG
	fprintf(stderr, "VENDOR: %s\n", (char *)glGetString(GL_VENDOR));
	fprintf(stderr, "VERSION: %s\n", (char *)glGetString(GL_VERSION));
	fprintf(stderr, "RENDERER: %s\n", (char *)glGetString(GL_RENDERER));
#endif

	if (EngineConfigs.flags.debug)
	{
		if (gl3wIsSupported(EngineConfigs.majorVersion, EngineConfigs.minorVersion))
		{
			glDebugMessageCallback((GLDEBUGPROC)DebugCallBack, this);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		}
	}

	StartUp();
	MainLoop();
	Shutdown();

	glfwDestroyWindow(Window);
	glfwTerminate();
}

void EngineBase::MainLoop()
{
	double LastTime = glfwGetTime();
	do
	{
		double lCurrentTime = glfwGetTime();
		CurrentTime = (float)lCurrentTime;
		float DeltaTime = (float)(CurrentTime - LastTime);
		Update(DeltaTime);
		Render();
		mInput.Update(DeltaTime);
		LastTime = lCurrentTime;
		glfwSwapBuffers(Window);
		glfwPollEvents();
		bIsRunning &= (glfwWindowShouldClose(Window) != GL_TRUE);
	} while (bIsRunning);
}