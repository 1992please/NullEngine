#pragma once
#include "CoreTypes.h"
#include "Event.h"

class NE_API FKeyEvent : public FEvent
{
public:
	inline int GetKeyCode() const { return m_KeyCode; }
	virtual int GetCategoryFlags() const override { return EventCategoryKeyboard | EventCategoryInput; }
protected:
	FKeyEvent(int InKeyCode)
		: m_KeyCode(InKeyCode) {}

	int m_KeyCode;
};

class NE_API FKeyPressedEvent : public FKeyEvent
{
public:
	FKeyPressedEvent(int InKeyCode, int InRepeatCount)
		: FKeyEvent(InKeyCode), RepeatCount(InRepeatCount) {}

	inline int GetRepeatCount() const { return RepeatCount; }

	FString ToString() const override
	{
		return FString::Printf("KeyPressedEvent: %d (%d repeats)", m_KeyCode, RepeatCount);
	}

	EVENT_CLASS_TYPE(KeyPressed)
private:
	int RepeatCount;
};

class NE_API FKeyReleasedEvent : public FKeyEvent
{
public:
	FKeyReleasedEvent(int InKeyCode)
		: FKeyEvent(InKeyCode) {}

	FString ToString() const override
	{
		return FString::Printf("KeyReleasedEvent: %d", m_KeyCode);
	}

	EVENT_CLASS_TYPE(KeyReleased)
};