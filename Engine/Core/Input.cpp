#include "Input.h"
#include "EngineBase.h"

ECommand Input::FitchCommand()
{
	ECommand mtemp = mCommand;
	mCommand = ECommand::NONE;
	return mtemp;
}

void Input::Update(const float p_deltaTime)
{
	for(int i = 0; i < (int32)EMovementAxis::NUM; i++)
		mAixValues[i] = 0.0f;

	// Keyboard axis
	if (GetKey(EKeyboardKey::W))
		mAixValues[(int32)EMovementAxis::MOVEFORWARD] += 1;
	if (GetKey(EKeyboardKey::S))
		mAixValues[(int32)EMovementAxis::MOVEFORWARD] -= 1;
	if (GetKey(EKeyboardKey::D))
		mAixValues[(int32)EMovementAxis::MOVERIGHT] += 1;
	if (GetKey(EKeyboardKey::A))
		mAixValues[(int32)EMovementAxis::MOVERIGHT] -= 1;
	if (GetKey(EKeyboardKey::E))
		mAixValues[(int32)EMovementAxis::MOVEUP] += 1;
	if (GetKey(EKeyboardKey::Q))
		mAixValues[(int32)EMovementAxis::MOVEUP] -= 1;

	// Mouse axis
	double InMouseX, InMouseY;
	GetCursorPosition(&InMouseX, &InMouseY);

	if (bFirstUpdate)
	{
		MouseX = InMouseX;
		MouseY = InMouseY;
		bFirstUpdate = false;
	}

	mAixValues[(int32)EMovementAxis::TURN] = float(InMouseX - MouseX);
	mAixValues[(int32)EMovementAxis::LOOKUP] = float(InMouseY - MouseY);

	MouseX = InMouseX;
	MouseY = InMouseY;
}

void Input::OnKeyboardAction(EKeyboardKey key, EKeyState state)
{
	switch (key)
	{
		case EKeyboardKey::ESCAPE:
			if(state == EKeyState::Press)
			{
				mCommand = ECommand::QUIT;
			}
			break;
		default:
			mCommand = ECommand::NONE;
			break;
	}
}

void Input::OnMouseAction(EMouseKey key, EKeyState state)
{

}

void Input::OnMouseWheel(int32 yoffset)
{

}

bool Input::GetKey(EKeyboardKey key)
{
	return (bool)glfwGetKey(EngineBase::Engine->Window, (int)key);
}

void Input::SetMousePos(uint32 x, uint32 y)
{
	glfwSetCursorPos(EngineBase::Engine->Window, (double)x, (double)y);
}

void Input::GetCursorPosition(double* xpos, double* ypos)
{
	glfwGetCursorPos(EngineBase::Engine->Window, xpos, ypos);
}

