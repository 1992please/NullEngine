#pragma once
#include "Core/CoreTypes.h"
#include "Buffers.h"
#include "Core/Containers/Array.h"

class IVertexArray
{
public:
	virtual ~IVertexArray() = default;

    virtual void Bind() const = 0;
    virtual void UnBind() const = 0;

	virtual void AddVertexBuffer(IVertexBuffer* InVertexBuffer) = 0;
	virtual void SetIndexBuffer(IIndexBuffer* InIndexBuffer) = 0;
	virtual const TArray<IVertexBuffer*>& GetVertexBuffers() const = 0;
	virtual const IIndexBuffer* GetIndexBuffer() const = 0;


    static IVertexArray* Create();
};