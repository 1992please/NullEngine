#pragma once
#include "Core/CoreTypes.h"
#include "Event.h"

class FWindowResizeEvent : public IEvent
{
public:
	EVENT_CLASS_TYPE(WindowResize)

	FWindowResizeEvent(unsigned int InWidth, unsigned int InHeight)
		: Width(InWidth), Height(InHeight) {}

	FORCEINLINE unsigned int GetWidth() const { return Width; }
	FORCEINLINE unsigned int GetHeight() const { return Height; }

	FString ToString() const override
	{
		return FString::Printf("WindowResizeEvent: %d, %d", Width, Height);
	}

	FORCEINLINE virtual int GetCategoryFlags() const override { return EventCategoryApplication; }
private:
	unsigned int Width, Height;
};

class FWindowCloseEvent : public IEvent
{
public:
	EVENT_CLASS_TYPE(WindowClose)

	FWindowCloseEvent() {}


	FORCEINLINE virtual int GetCategoryFlags() const override { return EventCategoryApplication; }
};

class FAppTickEvent : public IEvent
{
public:
	EVENT_CLASS_TYPE(AppTick)

	FAppTickEvent() {}


	FORCEINLINE virtual int GetCategoryFlags() const override { return EventCategoryApplication; }
};

class FAppUpdateEvent : public IEvent
{
public:
	EVENT_CLASS_TYPE(AppUpdate)

	FAppUpdateEvent() {}

	FORCEINLINE virtual int GetCategoryFlags() const override { return EventCategoryApplication; }
};

class FAppRenderEvent : public IEvent
{
public:
	EVENT_CLASS_TYPE(AppRender)

	FAppRenderEvent() {}

	FORCEINLINE virtual int GetCategoryFlags() const override { return EventCategoryApplication; }
};
