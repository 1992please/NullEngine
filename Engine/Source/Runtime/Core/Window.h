#pragma once
#include "Core/CoreTypes.h"
#include "Core/Events/Event.h"
#include "Core/Containers/String.h"

DECLARE_DELEGATE(OnWindowEventCallback, IEvent&)

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

	virtual ~FWindow() = default;

	virtual void OnUpdate() = 0;

	// Window attributes
	virtual void SetVSync(bool enabled) = 0;
	bool IsVSync() const { return bVSync; };

	virtual void* GetNativeWindow() const = 0;

	static FWindow* Create(const FWindowDetails& InDetails = FWindowDetails());

	OnWindowEventCallback WindowEventCallback;

	FString Title;
	unsigned int Width, Height;
	bool bVSync;

};