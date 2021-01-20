#pragma once
#include "CoreTypes.h"
#include "Event.h"

class NE_API FMouseMovedEvent : public FEvent
{
public:
	EVENT_CLASS_TYPE(MouseMoved)

	FMouseMovedEvent(float InX, float InY)
		: MouseX(InX), MouseY(InY) {}

	FORCEINLINE float GetX() const { return MouseX; }
	FORCEINLINE float GetY() const { return MouseY; }

	FString ToString() const override
	{
		return FString::Printf("MouseMovedEvent: %d, %d", MouseX, MouseY);
	}

	virtual int GetCategoryFlags() const override { return EventCategoryMouse | EventCategoryInput; }
private:
	float MouseX, MouseY;
};

class NE_API FMouseScrolledEvent : public FEvent
{
public:
	EVENT_CLASS_TYPE(MouseScrolled)

	FMouseScrolledEvent(float InXOffset, float InYOffset)
		: XOffset(InXOffset){}

	FORCEINLINE float GetXOffset() const { return XOffset; }

	FString ToString() const override
	{
		return FString::Printf("MouseScrolledEvent: %d", XOffset);
	}

	virtual int GetCategoryFlags() const override { return EventCategoryMouse | EventCategoryInput; }
private:
	float XOffset;
};

class NE_API FMouseButtonEvent : public FEvent
{
public:
	FORCEINLINE int GetMouseButton() const { return Button; }
	virtual int GetCategoryFlags() const override { return EventCategoryMouse | EventCategoryInput; }
protected:
	FMouseButtonEvent(int InButton)
		: Button(InButton) {}

	int Button;
};

class NE_API FMouseButtonPressedEvent : public FMouseButtonEvent
{
public:
	EVENT_CLASS_TYPE(MouseButtonPressed)

	FMouseButtonPressedEvent(int InButton)
		: FMouseButtonEvent(InButton) {}

	FString ToString() const override
	{
		return FString::Printf("MouseButtonPressedEvent: %d", Button);
	}
};

class NE_API FMouseButtonReleasedEvent : public FMouseButtonEvent
{
public:
	EVENT_CLASS_TYPE(MouseButtonReleased)

	FMouseButtonReleasedEvent(int button)
		: FMouseButtonEvent(button) {}

	FString ToString() const override
	{
		return FString::Printf("MouseButtonReleasedEvent: %d", Button);
	}
};
