#pragma once
#include "CoreTypes.h"
#include "Event.h"

class NE_API FWindowResizeEvent : public FEvent
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

class NE_API FWindowCloseEvent : public FEvent
{
public:
	EVENT_CLASS_TYPE(WindowClose)

	FWindowCloseEvent() {}


	FORCEINLINE virtual int GetCategoryFlags() const override { return EventCategoryApplication; }
};

class NE_API FAppTickEvent : public FEvent
{
public:
	EVENT_CLASS_TYPE(AppTick)

	FAppTickEvent() {}


	FORCEINLINE virtual int GetCategoryFlags() const override { return EventCategoryApplication; }
};

class NE_API FAppUpdateEvent : public FEvent
{
public:
	EVENT_CLASS_TYPE(AppUpdate)

	FAppUpdateEvent() {}

	FORCEINLINE virtual int GetCategoryFlags() const override { return EventCategoryApplication; }
};

class NE_API FAppRenderEvent : public FEvent
{
public:
	EVENT_CLASS_TYPE(AppRender)

	FAppRenderEvent() {}

	FORCEINLINE virtual int GetCategoryFlags() const override { return EventCategoryApplication; }
};
