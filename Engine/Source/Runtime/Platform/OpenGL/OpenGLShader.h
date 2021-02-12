#pragma once
#include "Core/CoreTypes.h"
#include "Renderer/Components/Shader.h"

class FOpenGLShader : public IShader
{
public:
	FOpenGLShader(const FString& FileName, const FString& ShaderName);

	virtual bool Islinked() const override { return bLinked; }
	virtual void Bind() const override;
	virtual void Unbind() const override;

	virtual void SetVector2(const FString& InName, const FVector2& v) override;
	virtual void SetVector(const FString& InName, const FVector& v) override;
	virtual void SetVector4(const FString& InName, const FVector4& v) override;
	virtual void SetMatrix(const FString& InName, const FMatrix& m) override;
	virtual void SetFloat(const FString& InName, float val) override;
	virtual void SetInt(const FString& InName, int32 val) override;
	virtual void SetIntArray(const FString& InName, int32* values, uint32 count) override;
	virtual void SetBool(const FString& InName, bool val) override;
	virtual void SetUInt(const FString& InName, uint32 val) override;
	virtual const FString& GetName() const override { return ShaderName; }
private:
	void DeleteShaderProgram();
	uint32 LoadFromString(int32 ShaderIdx, const FString& InSource);
	void CreateAndLinkProgram(uint32* InShaders);
	void LoadUniformLocations();

	bool bLinked;
	uint32 RendererID;
	FString ShaderName;
	TMap<FString, int32> UniformLocations;
};