#pragma once

#include "Core.h"


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
	None						= 0,
	EventCategoryApplication	= 1 << 0,
	EventCategoryInput			= 1 << 1,
	EventCategoryKeyboard		= 1 << 2,
	EventCategoryMouse			= 1 << 3,
	EventCategoryMouseButton	= 1 << 4
};