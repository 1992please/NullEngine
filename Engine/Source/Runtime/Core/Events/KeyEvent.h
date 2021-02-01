#pragma once
#include "Core/CoreTypes.h"
#include "Core/Application/KeyCodes.h"
#include "Event.h"

class FKeyEvent : public IEvent
{
public:
	inline int GetKeyCode() const { return m_KeyCode; }
	virtual int GetCategoryFlags() const override { return EventCategoryKeyboard | EventCategoryInput; }
protected:
	FKeyEvent(EKeyCode InKeyCode)
		: m_KeyCode(InKeyCode) {}

	EKeyCode m_KeyCode;
};

class FKeyPressedEvent : public FKeyEvent
{
public:
	FKeyPressedEvent(EKeyCode InKeyCode, int32 InRepeatCount)
		: FKeyEvent(InKeyCode), RepeatCount(InRepeatCount) {}

	inline int GetRepeatCount() const { return RepeatCount; }

	FString ToString() const override
	{
		return FString::Printf("KeyPressedEvent: %d (%d repeats)", m_KeyCode, RepeatCount);
	}

	EVENT_CLASS_TYPE(KeyPressed)
private:
	int32 RepeatCount;
};

class FKeyReleasedEvent : public FKeyEvent
{
public:
	FKeyReleasedEvent(EKeyCode InKeyCode)
		: FKeyEvent(InKeyCode) {}

	FString ToString() const override
	{
		return FString::Printf("KeyReleasedEvent: %d", m_KeyCode);
	}

	EVENT_CLASS_TYPE(KeyReleased)
};