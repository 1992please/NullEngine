#include "OpenGLContext.h"
#include "Core/Assert/Assert.h"
#include "GL/gl3w.h"
#include "GLFW/glfw3.h"

FOpenGlContext::FOpenGlContext(void* InWindow)
{
	WindowHandle = (GLFWwindow*) InWindow;
}

void FOpenGlContext::Init()
{
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	glfwMakeContextCurrent(WindowHandle);

	bool gl3wInitError = gl3wInit() != 0;
	bool gl3wOpenGlSupported = gl3wIsSupported(4, 6) != 0;
	NE_ASSERT_F(!gl3wInitError, "failed to initialize OpenGL\n");
	NE_ASSERT_F(gl3wOpenGlSupported, "OpenGL 4.6 not supported\n");
	NE_CORE_LOG("VENDOR: %s", glGetString(GL_VENDOR));
	NE_CORE_LOG("VERSION: %s", glGetString(GL_VERSION));
	NE_CORE_LOG("RENDERER: %s", glGetString(GL_RENDERER));

}

void FOpenGlContext::SwapBuffers()
{
	glfwSwapBuffers(WindowHandle);
}

