#pragma once
#include "Core/CoreTypes.h"
#include "Buffers.h"

class IVertexArray
{
public:
	virtual ~IVertexArray() = default;

    virtual void Bind() const = 0;
    virtual void UnBind() const = 0;

	virtual void SetVertexBuffer(IVertexBuffer* InVertexBuffer) = 0;
	virtual void SetIndexBuffer(IIndexBuffer* InIndexBuffer) = 0;
	virtual IVertexBuffer* GetVertexBuffer() const = 0;
	virtual const IIndexBuffer* GetIndexBuffer() const = 0;


    static IVertexArray* Create();
};