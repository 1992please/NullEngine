#include "Shader.h"
#include "GL/gl3w.h"
#include <iostream>
#include <fstream>
#include "Util/NullMemory.h"
#include "Math/Vector.h"
#include "Math/Vector4.h"
#include "Math/Vector2.h"
#include "Math/Matrix.h"

static std::string shader_seperators[5] = { 
	"$vertex_shader",
	"$control_shader",
	"$evaluation_shader",
	"$geometry_shader",
	"$fragment_shader"
};

static uint32 ShaderTypes[] = {
	GL_VERTEX_SHADER,
	GL_TESS_CONTROL_SHADER,
	GL_TESS_EVALUATION_SHADER,
	GL_GEOMETRY_SHADER,
	GL_FRAGMENT_SHADER
};

Shader::Shader()
{
	bLinked = false;
}


Shader::~Shader()
{
	if(bLinked)
		DeleteShaderProgram();
}

uint32 Shader::LoadFromString(int32 ShaderIdx, const std::string& source)
{
	if(source.empty())
		return 0;

	if (bLinked)
	{
		bLinked = false;
		DeleteShaderProgram();
	}

	GLuint shader = glCreateShader(ShaderTypes[ShaderIdx]);
	
	const char* ptmp = source.c_str();
	glShaderSource(shader, 1, &ptmp, NULL);

	//check whether the shader loads fine
	GLint status;
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *infoLog = new GLchar[infoLogLength];
		glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);
		std::cerr << "Compile log: " << infoLog << std::endl;
		delete[] infoLog;
	}

	return shader;
}

void Shader::CompileShader(const std::string& filename)
{
	uint32 Shaders[SHADER_STAGES_NO];
	FMemory::Memzero(Shaders, sizeof(Shaders));

	std::ifstream fp;
	fp.open(filename.c_str(), std::ios_base::in);
	if (fp) {
		std::string line, shader_buffers[SHADER_STAGES_NO];
		int16 Mode = 0;
		while (getline(fp, line)) {
			bool ModeChanged = false;
			for (int i = 0; i < SHADER_STAGES_NO; i++)
			{
				if (line == shader_seperators[i])
				{
					Mode = i;
					ModeChanged = true;
					break;
				}
			}

			if (!ModeChanged)
			{
				shader_buffers[Mode].append(line);
				shader_buffers[Mode].append("\r\n");
			}
		}

		for (int ShaderIdx = 0; ShaderIdx < SHADER_STAGES_NO; ShaderIdx++)
		{
			 Shaders[ShaderIdx] = LoadFromString(ShaderIdx, shader_buffers[ShaderIdx]);
		}
	}
	else {
		std::cerr << "Error loading shader: " << filename << std::endl;
	}
	fp.close();

	CreateAndLinkProgram(Shaders);
}

void Shader::CreateAndLinkProgram(uint32* _shaders)
{
	Handle = glCreateProgram();

	for (int i = 0; i < SHADER_STAGES_NO; i++)
		if (_shaders[i] != 0)
			glAttachShader(Handle, _shaders[i]);

	//link and check whether the program links fine
	GLint status;
	glLinkProgram(Handle);
	glGetProgramiv(Handle, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;

		glGetProgramiv(Handle, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *infoLog = new GLchar[infoLogLength];
		glGetProgramInfoLog(Handle, infoLogLength, NULL, infoLog);
		std::cerr << "Link log: " << infoLog << std::endl;
		delete[] infoLog;
	}
	else
	{
		FindUniformLocations();
		bLinked = true;
	}

	for (int i = 0; i < SHADER_STAGES_NO; i++)
		if (_shaders[i] != 0)
			glDeleteShader(_shaders[i]);

}

void Shader::Use()
{
	if(bLinked)
		glUseProgram(Handle);
}

void Shader::DeleteShaderProgram()
{
	UniformLocations.clear();
	glDeleteProgram(Handle);
}

int Shader::GetUniformLocation(const char *name) {
	std::map<std::string, int>::iterator UniformElement = UniformLocations.find(name);
	if (UniformLocations.end() == UniformElement) {
		GLint loc = glGetUniformLocation(Handle, name);
		UniformLocations[name] = loc;
		if (loc == -1)
		{
			printf("Shader Uniform \"%s\" is not active.\n", name);
		}
		return loc;
	}
	
	return UniformElement->second;
}

void Shader::BindAttribLocation(uint32 location, const char *name)
{
	glBindAttribLocation(Handle, location, name);
}

void Shader::BindFragDataLocation(uint32 location, const char *name)
{
	glBindFragDataLocation(Handle, location, name);
}

void Shader::SetUniform(const char *name, float x, float y, float z)
{
	GLint loc = GetUniformLocation(name);
	glUniform3f(loc, x, y, z);
}

void Shader::SetUniform(const char *name, const FVector2& v)
{
	GLint loc = GetUniformLocation(name);
	glUniform2f(loc, v.X, v.Y);
}

void Shader::SetUniform(const char *name, const FVector& v)
{
	SetUniform(name, v.X, v.Y, v.Z);
}

void Shader::SetUniform(const char *name, const FVector4& v)
{
	GLint loc = GetUniformLocation(name);
	glUniform4f(loc, v.X, v.Y, v.Z, v.W);
}

void Shader::SetUniform(const char *name, float val)
{
	GLint loc = GetUniformLocation(name);
	glUniform1f(loc, val);
}

void Shader::SetUniform(const char *name, int32 val)
{
	GLint loc = GetUniformLocation(name);
	glUniform1i(loc, val);
}

void Shader::SetUniform(const char *name, bool val)
{
	GLint loc = GetUniformLocation(name);
	glUniform1i(loc, val);
}

void Shader::SetUniform(const char *name, uint32 val)
{
	GLint loc = GetUniformLocation(name);
	glUniform1ui(loc, val);
}

void Shader::SetUniform(const char *name, const FMatrix& m)
{
	GLint loc = GetUniformLocation(name);
	glUniformMatrix4fv(loc, 1, GL_TRUE, m);
}

void Shader::FindUniformLocations()
{
	UniformLocations.clear();

	GLint numUniforms = 0;

	glGetProgramInterfaceiv(Handle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);
	GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };


	for (GLint i = 0; i < numUniforms; ++i) {
		GLint results[4];
		glGetProgramResourceiv(Handle, GL_UNIFORM, i, 4, properties, 4, NULL, results);

		if (results[3] != -1) continue;  // Skip uniforms in blocks
		GLint nameBufSize = results[0] + 1;
		char * name = new char[nameBufSize];
		glGetProgramResourceName(Handle, GL_UNIFORM, i, nameBufSize, NULL, name);
		UniformLocations[name] = results[2];
		delete[] name;
	}
}
