#include "Shader.h"
#include <iostream>
#include <fstream>

static string shader_seperators[5] = { "$vertex_shader",
"$control_shader",
"$evaluation_shader",
"$geometry_shader",
"$fragment_shader"};

uint32 Shader::ShaderTypes[] = {GL_VERTEX_SHADER,
GL_TESS_CONTROL_SHADER,
GL_TESS_EVALUATION_SHADER,
GL_GEOMETRY_SHADER,
GL_FRAGMENT_SHADER};

Shader::Shader()
{
	for(int i = 0; i < SHADER_STAGES_NO; i++)
		_shaders[i] = 0;

	_attributeList.clear();
	_uniformLocationList.clear();
}


Shader::~Shader()
{
	_attributeList.clear();
	_uniformLocationList.clear();
}

void Shader::LoadFromString(int32 ShaderIdx, const string& source)
{
	if(source.empty())
		return;

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
		cerr << "Compile log: " << infoLog << endl;
		delete[] infoLog;
	}
	_shaders[ShaderIdx] = shader;
}

void Shader::CompileShader(const string& filename)
{
	ifstream fp;
	fp.open(filename.c_str(), ios_base::in);
	if (fp) {
		string line, shader_buffers[SHADER_STAGES_NO];
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

		for (int i = 0; i < SHADER_STAGES_NO; i++)
		{
			LoadFromString(i, shader_buffers[i]);
		}
	}
	else {
		cerr << "Error loading shader: " << filename << endl;
	}
	fp.close();

	CreateAndLinkProgram();
}

void Shader::CreateAndLinkProgram()
{
	_program = glCreateProgram();

	for (int i = 0; i < SHADER_STAGES_NO; i++)
		if (_shaders[i] != 0)
			glAttachShader(_program, _shaders[i]);

	//link and check whether the program links fine
	GLint status;
	glLinkProgram(_program);
	glGetProgramiv(_program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;

		glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *infoLog = new GLchar[infoLogLength];
		glGetProgramInfoLog(_program, infoLogLength, NULL, infoLog);
		cerr << "Link log: " << infoLog << endl;
		delete[] infoLog;
	}

	for (int i = 0; i < SHADER_STAGES_NO; i++)
		if (_shaders[i] != 0)
			glDeleteShader(_shaders[i]);

}

void Shader::Use()
{
	glUseProgram(_program);
}

void Shader::UnUse()
{
	glUseProgram(0);
}

void Shader::AddAttribute(const string& attribute)
{
	_attributeList[attribute] = glGetAttribLocation(_program, attribute.c_str());
}

void Shader::AddUniform(const string& uniform)
{
	_uniformLocationList[uniform] = glGetUniformLocation(_program, uniform.c_str());
}

int32 Shader::operator[](const string& attribute)
{
	return _attributeList[attribute];
}

int32 Shader::operator()(const string& uniform)
{
	return _uniformLocationList[uniform];
}

void Shader::DeleteShaderProgram()
{
	glDeleteProgram(_program);
}
