#pragma once
#include "Core/CoreTypes.h"

class ITexture
{
public:
    virtual ~ITexture() = default;
    virtual uint32 GetWidth() const = 0;
	virtual uint32 GetHeight() const = 0;
	virtual uint32 GetRendererID() const = 0;
    virtual void SetData(const void* InData, uint32 InSize) = 0;
    virtual void Bind(uint32 InSlot = 0) const = 0;
    virtual bool operator==(const ITexture& other) const = 0;
};

class ITexture2D : public ITexture
{
public:
    static ITexture2D* Create(uint32 InWidth, uint32 InHeight);
    static ITexture2D* Create(const char* InPath);
};