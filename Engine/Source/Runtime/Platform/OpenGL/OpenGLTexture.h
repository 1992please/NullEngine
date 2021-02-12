#pragma once
#include "Core/CoreTypes.h"
#include "Renderer/Components/Texture.h"

class FOpenGLTexture2D : public ITexture2D
{
public:
	FOpenGLTexture2D(uint32 InWidth, uint32 InHeight);
	FOpenGLTexture2D(const char* InPath);
	virtual ~FOpenGLTexture2D();

	virtual uint32 GetWidth() const override { return Width; }
	virtual uint32 GetHeight() const override { return Height; }
	virtual uint32 GetRendererID() const override { return RendererID; }
	virtual void SetData(const void* InData, uint32 InSize) override;
	virtual void Bind(uint32 InSlot = 0) const override;
	virtual bool operator==(const ITexture& other) const override;

private:
	FString Path;
	uint32 Width;
	uint32 Height;
	uint32 RendererID;

	uint32 InternalFormat;
	uint32 DataFormat;
};