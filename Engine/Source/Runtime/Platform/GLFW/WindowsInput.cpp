#include "NullPCH.h"
#include "Core/Application/ApplicationInput.h"
#include "GLFW/glfw3.h"
#include "Core/Application/Application.h"
#include "Core/Application/ApplicationWindow.h"

bool FApplicationInput::IsKeyPressed(EKeyCode keycode)
{
	int state = glfwGetKey((GLFWwindow*)FApplication::GetApplication()->GetWindow()->GetNativeWindow(), (int)keycode);
	return state == GLFW_PRESS;
}

bool FApplicationInput::IsMouseButtonPressed(EKeyCode button)
{
	int state = glfwGetMouseButton((GLFWwindow*)FApplication::GetApplication()->GetWindow()->GetNativeWindow(), (int)button);
	return state == GLFW_PRESS;
}

FVector2 FApplicationInput::GetMousePos()
{
	double x, y;
	glfwGetCursorPos((GLFWwindow*)FApplication::GetApplication()->GetWindow()->GetNativeWindow(), &x, &y);
	return FVector2((float)x, (float)y);
}
