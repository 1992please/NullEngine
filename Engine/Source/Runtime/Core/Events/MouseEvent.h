#pragma once
#include "Core/CoreTypes.h"
#include "Event.h"

class NE_API FMouseMovedEvent : public IEvent
{
public:
	EVENT_CLASS_TYPE(MouseMoved)

	FMouseMovedEvent(float InX, float InY)
		: MouseX(InX), MouseY(InY) {}

	FORCEINLINE float GetX() const { return MouseX; }
	FORCEINLINE float GetY() const { return MouseY; }

	FString ToString() const override
	{
		return FString::Printf("MouseMovedEvent: %f, %f", MouseX, MouseY);
	}

	virtual int GetCategoryFlags() const override { return EventCategoryMouse | EventCategoryInput; }
private:
	float MouseX, MouseY;
};

class NE_API FMouseScrolledEvent : public IEvent
{
public:
	EVENT_CLASS_TYPE(MouseScrolled)

	FMouseScrolledEvent(float InXOffset, float InYOffset)
		: XOffset(InXOffset), YOffset(InYOffset){}

	FORCEINLINE float GetXOffset() const { return XOffset; }
	FORCEINLINE float GetYOffset() const { return YOffset; }

	FString ToString() const override
	{
		return FString::Printf("MouseScrolledEvent: %f, %f", XOffset, YOffset);
	}

	virtual int GetCategoryFlags() const override { return EventCategoryMouse | EventCategoryInput; }
private:
	float XOffset;
	float YOffset;
};

class NE_API FMouseButtonEvent : public IEvent
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
