#pragma once

#include "CoreTypes.h"
#include "Containers/String.h"

enum class EventType
{
	None = 0,
	WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
	AppTick, AppUpdate, AppRender,
	KeyPressed, KeyReleased,
	MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
};

enum EventCategory
{
	EventCategoryNone			= 0,
	EventCategoryApplication	= 1 << 0,
	EventCategoryInput			= 1 << 1,
	EventCategoryKeyboard		= 1 << 2,
	EventCategoryMouse			= 1 << 3,
	EventCategoryMouseButton	= 1 << 4
};

class NE_API FEvent
{
	friend class EventDispatcher;
public:
	virtual EventType GetEventType() const = 0;
	virtual const char* GetName() const = 0;
	virtual int GetCategoryFlags() const = 0;
	FORCEINLINE virtual FString ToString() const { return GetName(); }

	FORCEINLINE bool IsInCategory(EventCategory Category)
	{
		return GetCategoryFlags() & Category;
	}
protected:
	bool bHandled = false;
};

class FEventDispatcher
{
public:
	FEventDispatcher(FEvent& InEvent)
		: Event(InEvent)
	{
	}

	template<typename T>
	bool Dispatch(bool(*Func)(T&))
	{
		if (Event.GetEventType() == T::GetStaticType())
		{
			Event.bHandled = Func(*(T*)&Event);
			return true;
		}
		return false;
	}
private:
	FEvent& Event;
};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }