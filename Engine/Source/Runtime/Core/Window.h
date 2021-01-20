#pragma once
#include "CoreTypes.h"
#include "Events/Event.h"
#include "Containers/String.h"

struct FWindowDetails
{
	FString Title;
	uint32 Width;
	uint32 Height;

	FWindowDetails(const FString& InTitle = "Null Engine",
		uint32 InWidth = 1600,
		uint32 InHeight = 900)
		: Title(InTitle), Width(InWidth), Height(InHeight)
	{
	}
};

// Interface representing a desktop system based Window
class FWindow
{
public:
	typedef void(*EventCallbackFn)(FEvent&);
	virtual ~FWindow() = default;

	virtual void OnUpdate() = 0;

	virtual uint32 GetWidth() const = 0;
	virtual uint32 GetHeight() const = 0;

	// Window attributes
	virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
	virtual void SetVSync(bool enabled) = 0;
	virtual bool IsVSync() const = 0;

	virtual void* GetNativeWindow() const = 0;

	static FWindow* Create(const FWindowDetails& InDetails = FWindowDetails());
};