#pragma once
#include "Core/Math/Color.h"

class FSpriteComponent
{
public:
	FLinearColor Color;

	FSpriteComponent() : Color(FLinearColor::White) {}
};