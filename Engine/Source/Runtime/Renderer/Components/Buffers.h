#pragma once
#include "Core/CoreTypes.h"
#include "Core/Containers/Array.h"
#include "Core/Containers/String.h"

enum class  EShaderDataType
{
	None = 0,
	Float,
	Float2,
	Float3,
	Float4,
	Mat3,
	Mat4,
	Int,
	Int2,
	Int3,
	Int4,
	Bool
};

struct FBufferElement
{
	FBufferElement() {}

	FBufferElement(const FString& InName, EShaderDataType InType, bool InNormalized = false)
		: Name(InName), Type(InType), Size(GetElementTypeSize(InType)), Offset(0), bNormalized(InNormalized)
	{

	}

	static uint32 GetElementTypeSize(EShaderDataType InType)
	{
		switch (InType)
		{
			case EShaderDataType::Float:	return 4;
			case EShaderDataType::Float2:	return 4 * 2;
			case EShaderDataType::Float3:	return 4 * 3;
			case EShaderDataType::Float4:	return 4 * 4;
			case EShaderDataType::Mat3:		return 4 * 3 * 3;
			case EShaderDataType::Mat4:		return 4 * 4 * 4;
			case EShaderDataType::Int:		return 4;
			case EShaderDataType::Int2:		return 4 * 2;
			case EShaderDataType::Int3:		return 4 * 3;
			case EShaderDataType::Int4:		return 4 * 4;
			case EShaderDataType::Bool:		return 4;
		}
		NE_ASSERT_F(false, "Unknown Element Type");
		return 0;
	}

	uint32 GetComponentCount() const
	{
		switch (Type)
		{
			case EShaderDataType::Float:	return 1;
			case EShaderDataType::Float2:	return 2;
			case EShaderDataType::Float3:	return 3;
			case EShaderDataType::Float4:	return 4;
			case EShaderDataType::Mat3:		return 3 * 3;
			case EShaderDataType::Mat4:		return 4 * 4;
			case EShaderDataType::Int:		return 1;
			case EShaderDataType::Int2:		return 2;
			case EShaderDataType::Int3:		return 3;
			case EShaderDataType::Int4:		return 4;
			case EShaderDataType::Bool:		return 1;
		}
		NE_ASSERT_F(false, "Unknown Element Type");
		return 0;
	}

	EShaderDataType Type;
	FString Name;
	uint32 Offset;
	uint32 Size;
	bool bNormalized;
};

class FBufferLayout
{
public:
	FBufferLayout() {}

	FORCEINLINE FBufferLayout(const std::initializer_list<FBufferElement>& InElements)
		:Elements(InElements) 
	{
		uint32 CurrentOffset = 0;
		for (FBufferElement& Ele : Elements)
		{
			Ele.Offset = CurrentOffset;
			CurrentOffset += Ele.Size;
		}
		Stride = CurrentOffset;
	}

	FORCEINLINE const TArray<FBufferElement>& GetElements() const { return Elements; }
	FORCEINLINE	uint32 GetStride() const { return Stride; }

	FORCEINLINE FBufferElement*				begin() { return Elements.begin(); }
	FORCEINLINE const FBufferElement*		begin() const { return Elements.begin(); }
	FORCEINLINE FBufferElement*				end() { return Elements.end(); }
	FORCEINLINE const FBufferElement*		end() const { return Elements.end(); }
private:
	TArray<FBufferElement>  Elements;
	uint32 Stride = 0;
};

class IVertexBuffer
{
public:
	virtual ~IVertexBuffer() = default;

    virtual void Bind() const = 0;
    virtual void UnBind() const = 0;
	virtual void SetLayout(const FBufferLayout& InLayout) = 0;
	virtual const FBufferLayout& GetLayout() const = 0;

    static IVertexBuffer* Create(float* InVertices, uint32 InSize);
};

class IIndexBuffer
{
public:
	virtual ~IIndexBuffer() = default;

    virtual void Bind() const = 0;
    virtual void UnBind() const = 0;
	virtual uint32 GetCount() const = 0;

    static IIndexBuffer* Create(uint32* InIndices, uint32 InCount);


};