#include "NullPCH.h"
#include "OpenGLFrameBuffer.h"
#include "GL/gl3w.h"


static const uint32 MaxFramebufferSize = 8192;

namespace FrameBufferUtils {

	static GLenum TextureTarget(bool IsMultiSampled)
	{
		return IsMultiSampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	}

	static void CreateTextures(bool IsMultiSampled, uint32* OutID, uint32 InCount)
	{
		glCreateTextures(TextureTarget(IsMultiSampled), InCount, OutID);
	}

	static void BindTexture(bool IsMultiSampled, uint32 InID)
	{
		glBindTexture(TextureTarget(IsMultiSampled), InID);
	}

	static void AttachColorTexture(uint32 InID, int32 InSamples, GLenum InFormat, uint32 InWidth, uint32 InHeight, int32 InIndex)
	{
		bool IsMultiSampled = InSamples > 1;
		if (IsMultiSampled)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, InSamples, InFormat, InWidth, InHeight, GL_FALSE);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, InFormat, InWidth, InHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + InIndex, TextureTarget(IsMultiSampled), InID, 0);
	}

	static void AttachDepthTexture(uint32 InID, int32 InSamples, GLenum InFormat, GLenum InAttachmentType, uint32 InWidth, uint32 InHeight)
	{
		bool IsMultiSampled = InSamples > 1;
		if (IsMultiSampled)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, InSamples, InFormat, InWidth, InHeight, GL_FALSE);
		}
		else
		{
			glTexStorage2D(GL_TEXTURE_2D, 1, InFormat, InWidth, InHeight);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, InAttachmentType, TextureTarget(IsMultiSampled), InID, 0);
	}

	static bool IsDepthFormat(EFramebufferTextureFormat InFormat)
	{
		switch (InFormat)
		{
			case EFramebufferTextureFormat::DEPTH24STENCIL8:  return true;
		}

		return false;
	}

}


FOpenGLFrameBuffer::FOpenGLFrameBuffer(const FFrameBufferInfo& InBufferInfo)
	: BufferInfo(InBufferInfo), RendererID(0), DepthAttachmentType(EFramebufferTextureFormat::None)
{
	for (EFramebufferTextureFormat Attachment : InBufferInfo.Attachments)
	{
		if (!FrameBufferUtils::IsDepthFormat(Attachment))
			ColorAttachmentsType.Add(Attachment);
		else
			DepthAttachmentType = Attachment;
	}

	Invalidate();
}

FOpenGLFrameBuffer::~FOpenGLFrameBuffer()
{
	glDeleteFramebuffers(1, &RendererID);
}

void FOpenGLFrameBuffer::Invalidate()
{
	if (RendererID)
	{
		glDeleteFramebuffers(1, &RendererID);
		glDeleteTextures(ColorAttachments.Num(), ColorAttachments.GetData());
		glDeleteTextures(1, &DepthAttachment);

		ColorAttachments.Empty();
		DepthAttachment = 0;
	}

	glCreateFramebuffers(1, &RendererID);
	glBindFramebuffer(GL_FRAMEBUFFER, RendererID);

	const bool IsMultiSample = BufferInfo.Samples > 1;

	if (ColorAttachmentsType.Num() > 0)
	{
		ColorAttachments.SetNum(ColorAttachmentsType.Num());
		FrameBufferUtils::CreateTextures(IsMultiSample, ColorAttachments.GetData(), ColorAttachments.Num());

		for (int32 i = 0; i < ColorAttachments.Num(); i++)
		{
			FrameBufferUtils::BindTexture(IsMultiSample, ColorAttachments[i]);
			switch (ColorAttachmentsType[i])
			{
				case EFramebufferTextureFormat::RGBA8:
					FrameBufferUtils::AttachColorTexture(ColorAttachments[i], BufferInfo.Samples, GL_RGBA8, BufferInfo.Width, BufferInfo.Height, i);
					break;
			}
		}
	}

	if (DepthAttachmentType != EFramebufferTextureFormat::None)
	{
		FrameBufferUtils::CreateTextures(IsMultiSample, &DepthAttachment, 1);
		FrameBufferUtils::BindTexture(IsMultiSample, DepthAttachment);
		switch (DepthAttachmentType)
		{
			case EFramebufferTextureFormat::DEPTH24STENCIL8:
				FrameBufferUtils::AttachDepthTexture(DepthAttachment, BufferInfo.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, BufferInfo.Width, BufferInfo.Height);
				break;
		}
	}


	if (ColorAttachments.Num() > 1)
	{
		NE_ASSERT(ColorAttachments.Num() <= 4);
		GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(ColorAttachments.Num(), buffers);
	}
	else if (ColorAttachments.Num() == 0)
	{
		// Only depth-pass
		glDrawBuffer(GL_NONE);
	}

	NE_ASSERT_F(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FOpenGLFrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, RendererID);
	//glViewport(0, 0, BufferInfo.Width, BufferInfo.Height);
}

void FOpenGLFrameBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FOpenGLFrameBuffer::Resize(uint32 InWidth, uint32 InHeight)
{

}
