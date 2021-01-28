#pragma once
#include "Core/CoreTypes.h"
#include "Core/Containers/String.h"
#include "Core/Math/NullMath.h"

class IShader
{
public:
	// Make it non-copyable
	virtual ~IShader() = default;

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	virtual void SetVector2(const FString& InName, const FVector2& v) = 0;
	virtual void SetVector(const FString& InName, const FVector& v) = 0;
	virtual void SetVector4(const FString& InName, const FVector4& v) = 0;
	virtual void SetMatrix(const FString& InName, const FMatrix& m) = 0;
	virtual void SetFloat(const FString& InName, float val) = 0;
	virtual void SetInt(const FString& InName, int32 val) = 0;
	virtual void SetBool(const FString& InName, bool val) = 0;
	virtual void SetUInt(const FString& InName, uint32 val) = 0;

	virtual const FString& GetName() const = 0;

	static IShader* Create(const FString& InFileName, const FString& InName = FString());
};