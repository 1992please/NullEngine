#pragma once
#include "Core/Application/ApplicationInput.h"

class FWindowsInput : public IApplicationInput
{
	virtual bool IsKeyPressedImpl(int keycode) override;
	virtual bool IsMouseButtonPressedImpl(int button) override;
	virtual FVector2 GetMousePosImpl() override;
};