#pragma once
#include "Core/CoreTypes.h"
#include "Core/Application/KeyCodes.h"
#include "Core/Math/Vector2.h"

class FApplicationInput
{
public:
	static bool IsKeyPressed(EKeyCode InKeycode);
	static bool IsMouseButtonPressed(EKeyCode InButton);
	static FVector2 GetMousePos();
};
