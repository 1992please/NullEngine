#pragma once
#include "Core/CoreTypes.h"
#include "Core/Events/Event.h"

DECLARE_DELEGATE(OnWindowEventCallback, IEvent&)

struct FWindowDetails
{
	FString Title;
	uint32 Width;
	uint32 Height;

	FWindowDetails(const FString& InTitle,
		uint32 InWidth = 1600,
		uint32 InHeight = 900)
		: Title(InTitle), Width(InWidth), Height(InHeight)
	{
	}
};

// Interface representing a desktop system based Window
class IApplicationWindow
{
public:

	virtual ~IApplicationWindow() = default;

	virtual void OnUpdate() = 0;

	// Window attributes
	virtual void SetVSync(bool enabled) = 0;
	FORCEINLINE uint32 GetWidth() const { return Width; }
	FORCEINLINE uint32 GetHeight() const { return Height; }
	FORCEINLINE bool IsVSync() const { return bVSync; }
	FORCEINLINE const FString& GetTitle() const { return Title; }
	virtual void* GetNativeWindow() const = 0;

	static IApplicationWindow* Create(const FWindowDetails& InDetails);

	OnWindowEventCallback WindowEventCallback;

protected:
	FString Title;
	uint32 Width;
	uint32 Height;
	bool bVSync;
};