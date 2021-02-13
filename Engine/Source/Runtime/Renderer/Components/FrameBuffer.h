#pragma once
#include "Core/CoreTypes.h"

enum class EFramebufferTextureFormat
{
	None = 0,

	// Color
	RGBA8,

	// Depth/stencil
	DEPTH24STENCIL8,

	// Defaults
	Depth = DEPTH24STENCIL8
};

struct FFrameBufferInfo
{
	uint32 Width;
	uint32 Height;
	uint32 Samples;
	TArray<EFramebufferTextureFormat> Attachments;
	bool SwapChainTarget;

	FFrameBufferInfo()
		: Width(0), Height(0), Samples(1), SwapChainTarget(false)
	{}
};

class IFrameBuffer
{
public:
	virtual ~IFrameBuffer() = default;


	virtual void Bind() = 0;
	virtual void Unbind() = 0;

	virtual void Resize(uint32 InWidth, uint32 InHeight) = 0;

	virtual uint32 GetColorAttachmentRendererID(uint32 InIndex = 0) const = 0;


	virtual const FFrameBufferInfo& GetInfo() const = 0;
	static IFrameBuffer* Create(const FFrameBufferInfo& InBufferInfo);
};
