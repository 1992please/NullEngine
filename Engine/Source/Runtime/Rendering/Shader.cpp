#include "Shader.h"
#include "Platform/OpenGL/OpenGLShader.h"

IShader* IShader::Create(const FString& InFileName, const FString& InName /*= FString()*/)
{
	return new FOpenGLShader(InFileName, InName);
}
