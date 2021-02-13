#pragma once
#include "Core/CoreTypes.h"
#include "Renderer/Components/FrameBuffer.h"

class FOpenGLFrameBuffer : public IFrameBuffer
{
public:
	FOpenGLFrameBuffer(const FFrameBufferInfo& InBufferInfo);
	virtual ~FOpenGLFrameBuffer();

	void Invalidate();

	virtual void Bind() override;
	virtual void Unbind() override;

	virtual void Resize(uint32 InWidth, uint32 InHeight) override;

	virtual uint32 GetColorAttachmentRendererID(uint32 InIndex = 0) const override { return ColorAttachments[InIndex]; }


	virtual const FFrameBufferInfo& GetInfo() const override { return BufferInfo; }

private:
	uint32 RendererID;
	FFrameBufferInfo BufferInfo;

	TArray<EFramebufferTextureFormat> ColorAttachmentsType;
	EFramebufferTextureFormat DepthAttachmentType;

	TArray<uint32> ColorAttachments;
	uint32 DepthAttachment;
};