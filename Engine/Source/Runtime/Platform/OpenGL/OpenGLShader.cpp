#include "NullPCH.h"
#include "OpenGLShader.h"
#include "Core/Templates/NullTemplate.h"
#include "GL/gl3w.h"
#include "Core/Misc/FileHelper.h"
#include "Core/Misc/Paths.h"

#define UNIFORM_NAME_BUFFER_SIZE 128

const static char* ShaderTypeStr = "#type";

const static char* ShaderSeperator[5] = {
	"#type vertex",
	"#type control",
	"#type evaluation",
	"#type geometry",
	"#type fragment"
};

const static uint32 ShaderTypes[] = {
	GL_VERTEX_SHADER,
	GL_TESS_CONTROL_SHADER,
	GL_TESS_EVALUATION_SHADER,
	GL_GEOMETRY_SHADER,
	GL_FRAGMENT_SHADER
};

FOpenGLShader::FOpenGLShader(const FString& InFileName, const FString& InShaderName)
	: ShaderName(InShaderName)
	, bLinked(false)
{
	NE_PROFILE_FUNCTION();

	if (ShaderName.IsEmpty())
	{
		ShaderName = FPaths::GetBaseFilename(InFileName);
	}

	FString FileContent;
	if (FFileHelper::ReadFromTextFile(InFileName, FileContent))
	{
		const int32 ShadersNo = NE_ARRAY_COUNT(ShaderTypes);
		uint32 ShaderIDs[ShadersNo];
		FMemory::Memzero(ShaderIDs, sizeof(ShaderIDs));

		for (int ShaderIndex = 0; ShaderIndex < ShadersNo; ShaderIndex++)
		{
			FString ShaderString;
			ShaderString.Reset();

			int32 Pos = FileContent.Find(ShaderSeperator[ShaderIndex], ESearchCase::CaseSensitive);
			if (Pos != INDEX_NONE)
			{
				Pos += FCString::Strlen(ShaderSeperator[ShaderIndex]);
				Pos = FileContent.Find("\n", ESearchCase::CaseSensitive, ESearchDir::FromStart, Pos);
				if (Pos != INDEX_NONE && ++Pos < FileContent.Len())
				{
					int32 StartPos = Pos;
					int32 EndPos = FileContent.Find(ShaderTypeStr, ESearchCase::CaseSensitive, ESearchDir::FromStart, Pos);
					int32 ShaderStrCount = EndPos == INDEX_NONE ? (FileContent.Len() - StartPos) : (EndPos - StartPos);

					ShaderString = FString(ShaderStrCount, &FileContent[StartPos]);
				}
			}

			ShaderIDs[ShaderIndex] = LoadFromString(ShaderIndex, ShaderString);
		}

		CreateAndLinkProgram(ShaderIDs);
	}
	else 
	{
		NE_CORE_ERROR("Error loading shader: %s", *InFileName);
	}
}

void FOpenGLShader::Bind() const
{
	NE_PROFILE_FUNCTION();

	if (bLinked)
	{
		glUseProgram(RendererID);
	}
}

void FOpenGLShader::Unbind() const
{
	NE_PROFILE_FUNCTION();

	glUseProgram(0);
}

void FOpenGLShader::SetVector2(const FString& InName, const FVector2& v)
{
	NE_PROFILE_FUNCTION();

	glUniform2f(UniformLocations[InName], v.X, v.Y);
}

void FOpenGLShader::SetVector(const FString& InName, const FVector& v)
{
	NE_PROFILE_FUNCTION();

	glUniform3f(UniformLocations[InName], v.X, v.Y, v.Z);
}

void FOpenGLShader::SetVector4(const FString& InName, const FVector4& v)
{
	NE_PROFILE_FUNCTION();

	glUniform4f(UniformLocations[InName], v.X, v.Y, v.Z, v.W);
}

void FOpenGLShader::SetMatrix(const FString& InName, const FMatrix& m)
{
	NE_PROFILE_FUNCTION();

	glUniformMatrix4fv(UniformLocations[InName], 1, GL_FALSE, m);
}

void FOpenGLShader::SetFloat(const FString& InName, float val)
{
	NE_PROFILE_FUNCTION();

	glUniform1f(UniformLocations[InName], val);
}

void FOpenGLShader::SetInt(const FString& InName, int32 val)
{
	NE_PROFILE_FUNCTION();

	glUniform1i(UniformLocations[InName], val);
}

void FOpenGLShader::SetBool(const FString& InName, bool val)
{
	NE_PROFILE_FUNCTION();

	glUniform1i(UniformLocations[InName], val);
}

void FOpenGLShader::SetUInt(const FString& InName, uint32 val)
{
	NE_PROFILE_FUNCTION();

	glUniform1ui(UniformLocations[InName], val);
}

uint32 FOpenGLShader::LoadFromString(int32 ShaderIdx, const FString& InSource)
{
	NE_PROFILE_FUNCTION();

	if (InSource.IsEmpty())
		return 0;

	if (bLinked)
	{
		bLinked = false;
		DeleteShaderProgram();
	}

	GLuint ShaderID = glCreateShader(ShaderTypes[ShaderIdx]);

	const char* RawSource = *InSource;
	glShaderSource(ShaderID, 1, &RawSource, NULL);

	//check whether the shader loads fine
	GLint status;
	glCompileShader(ShaderID);
	glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
		TArray<char> infoLog(infoLogLength);
		glGetShaderInfoLog(ShaderID, infoLogLength, NULL, infoLog.GetData());
		NE_CORE_ERROR("Compile log ( %s ):\n%s", ShaderSeperator[ShaderIdx], infoLog.GetData());
	}

	return ShaderID;
}

void FOpenGLShader::CreateAndLinkProgram(uint32* InShaders)
{
	NE_PROFILE_FUNCTION();

	RendererID = glCreateProgram();

	const int32 ShadersNo = NE_ARRAY_COUNT(ShaderTypes);

	for (int i = 0; i < ShadersNo; i++)
		if (InShaders[i] != 0)
			glAttachShader(RendererID, InShaders[i]);

	//link and check whether the program links fine
	GLint status;
	glLinkProgram(RendererID);
	glGetProgramiv(RendererID, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;

		glGetProgramiv(RendererID, GL_INFO_LOG_LENGTH, &infoLogLength);
		TArray<char> infoLog(infoLogLength);
		glGetProgramInfoLog(RendererID, infoLogLength, NULL, infoLog.GetData());
		NE_ASSERT_F(false, "Link log:\n%s", infoLog.GetData());
	}
	else
	{
		LoadUniformLocations();
		bLinked = true;
	}

	for (int i = 0; i < ShadersNo; i++)
		if (InShaders[i] != 0)
			glDeleteShader(InShaders[i]);
}

void FOpenGLShader::LoadUniformLocations()
{
	NE_PROFILE_FUNCTION();

	GLint NumUniforms = 0;

	glGetProgramInterfaceiv(RendererID, GL_UNIFORM, GL_ACTIVE_RESOURCES, &NumUniforms);
	GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };

	UniformLocations.Empty(NumUniforms);
	char UniformName[UNIFORM_NAME_BUFFER_SIZE];
	for (GLint UniformIndex = 0; UniformIndex < NumUniforms; ++UniformIndex) {
		GLint Results[4];
		glGetProgramResourceiv(RendererID, GL_UNIFORM, UniformIndex, 4, properties, 4, NULL, Results);
		if (Results[3] != -1) continue;  // Skip uniforms in blocks

		GLint nameBufSize = Results[0] + 1;

		NE_ASSERT_F(nameBufSize <= UNIFORM_NAME_BUFFER_SIZE, "Uniform buffer is not large engough.");

		glGetProgramResourceName(RendererID, GL_UNIFORM, UniformIndex, nameBufSize, NULL, UniformName);

		UniformLocations.Add(UniformName, Results[2]);
	}
}

void FOpenGLShader::DeleteShaderProgram()
{
	NE_PROFILE_FUNCTION();

	if (bLinked)
	{
		bLinked = false;
		glDeleteProgram(RendererID);
	}
}
