#include "NullPCH.h"
#include "OpenGLTexture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GL/gl3w.h"

FOpenGLTexture2D::FOpenGLTexture2D(uint32 InWidth, uint32 InHeight)
	: Width(InWidth), Height(InHeight)
{
	NE_PROFILE_FUNCTION();

	InternalFormat = GL_RGBA8;
	DataFormat = GL_RGBA;

	glCreateTextures(GL_TEXTURE_2D, 1, &RendererID);
	glTextureStorage2D(RendererID, 1, InternalFormat, Width, Height);

	glTextureParameteri(RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureParameteri(RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

FOpenGLTexture2D::FOpenGLTexture2D(const char* InPath)
	:InternalFormat(0), DataFormat(0), Path(InPath)
{
	NE_PROFILE_FUNCTION();
	int width, height, channels;
	stbi_set_flip_vertically_on_load(1);
	stbi_uc* ImageData = stbi_load(*Path, &width, &height, &channels, 0);

	NE_CHECK(ImageData);

	Width = width;
	Height = height;

	if (channels == 4)
	{
		InternalFormat = GL_RGBA8;
		DataFormat = GL_RGBA;
	}
	else if (channels == 3)
	{
		InternalFormat = GL_RGB8;
		DataFormat = GL_RGB;
	}

	NE_CHECK(InternalFormat & DataFormat);


	glCreateTextures(GL_TEXTURE_2D, 1, &RendererID);
	glTextureStorage2D(RendererID, 1, InternalFormat, Width, Height);

	glTextureParameteri(RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureParameteri(RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureSubImage2D(RendererID, 0, 0, 0, Width, Height, DataFormat, GL_UNSIGNED_BYTE, ImageData);

	stbi_image_free(ImageData);
}

FOpenGLTexture2D::~FOpenGLTexture2D()
{
	NE_PROFILE_FUNCTION();

	glDeleteTextures(1, &RendererID);
}

void FOpenGLTexture2D::SetData(const void* InData, uint32 InSize)
{
	NE_PROFILE_FUNCTION();

	uint32 nChannels = DataFormat == GL_RGBA ? 4 : 3;
	NE_CHECK_F(InSize == Width * Height * nChannels, "Data must be entire texture!");
	glTextureSubImage2D(RendererID, 0, 0, 0, Width, Height, DataFormat, GL_UNSIGNED_BYTE, InData);
}

void FOpenGLTexture2D::Bind(uint32 InSlot /*= 0*/) const
{
	NE_PROFILE_FUNCTION();

	glBindTextureUnit(InSlot, RendererID);
}

bool FOpenGLTexture2D::operator==(const ITexture& other) const
{
	return RendererID == other.GetRendererID();
}

