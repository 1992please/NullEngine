#pragma once
#include "Core/CoreTypes.h"
#include "Core/Application/KeyCodes.h"
#include "Core/Math/Vector2.h"

class IApplicationInput
{
public:
	FORCEINLINE static bool IsKeyPressed(EKeyCode InKeycode) { return Instance->IsKeyPressedImpl(InKeycode); }
	FORCEINLINE static bool IsMouseButtonPressed(EKeyCode InButton) { return Instance->IsMouseButtonPressedImpl(InButton); }
	FORCEINLINE static FVector2 GetMousePos() { return Instance->GetMousePosImpl(); }
protected:
	virtual bool IsKeyPressedImpl(int keycode) = 0;
	virtual bool IsMouseButtonPressedImpl(int button) = 0;
	virtual FVector2 GetMousePosImpl() = 0;
private:
	static IApplicationInput* Instance;
};
