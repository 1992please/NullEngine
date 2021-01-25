#include "WindowsInput.h"
#include "GLFW/glfw3.h"
#include "Core/Application/Application.h"
#include "Core/Application/ApplicationWindow.h"


IApplicationInput* IApplicationInput::Instance = new FWindowsInput;

bool FWindowsInput::IsKeyPressedImpl(int keycode)
{
	int state = glfwGetKey((GLFWwindow*)FApplication::GetApplication()->GetWindow()->GetNativeWindow(), keycode);
	return state == GLFW_PRESS;
}

bool FWindowsInput::IsMouseButtonPressedImpl(int button)
{
	int state = glfwGetMouseButton((GLFWwindow*)FApplication::GetApplication()->GetWindow()->GetNativeWindow(), button);
	return state == GLFW_PRESS;
}

FVector2 FWindowsInput::GetMousePosImpl()
{
	double x, y;
	glfwGetCursorPos((GLFWwindow*)FApplication::GetApplication()->GetWindow()->GetNativeWindow(), &x, &y);
	return FVector2((float)x, (float)y);
}
