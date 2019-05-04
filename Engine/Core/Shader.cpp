#include "Shader.h"
#include <iostream>
#include <fstream>

static string vertex_seperator = "$vertex_shader";
static string fragment_seperator = "$fragment_shader";
static string geometry_seperator = "$geometry_shader";


uint32 Shader::ShaderTypes[3] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER};

Shader::Shader()
{
	_shaders[VERTEX_SHADER] = 0;
	_shaders[FRAGMENT_SHADER] = 0;
	_shaders[GEOMETRY_SHADER] = 0;
	_attributeList.clear();
	_uniformLocationList.clear();
}


Shader::~Shader()
{
	_attributeList.clear();
	_uniformLocationList.clear();
}

void Shader::LoadFromString(ShaderType InShaderType, const string& source)
{
	GLuint shader = glCreateShader(ShaderTypes[InShaderType]);
	
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
	_shaders[InShaderType] = shader;
}

void Shader::LoadFromFile(const string& filename)
{
	ifstream fp;
	fp.open(filename.c_str(), ios_base::in);
	if (fp) {
		string line, frag_buffer, vert_buffer, geo_buffer;
		int16 Mode = 0;
		while (getline(fp, line)) {
			if (line == vertex_seperator)
			{
				Mode = 0;
				continue;
			}
			else if (line == fragment_seperator)
			{
				Mode = 1;
				continue;
			}
			else if (line == geometry_seperator)
			{
				Mode = 2;
				continue;
			}
			switch (Mode)
			{
				case  0:
				{
					vert_buffer.append(line);
					vert_buffer.append("\r\n");
				}
				break;
				case  1:
				{
					frag_buffer.append(line);
					frag_buffer.append("\r\n");
				}
				break;
				case  2:
				{
					geo_buffer.append(line);
					geo_buffer.append("\r\n");
				}
				break;
			}

		}

		//copy to source
		if(!vert_buffer.empty())
			LoadFromString(VERTEX_SHADER, vert_buffer);
		if (!frag_buffer.empty())
			LoadFromString(FRAGMENT_SHADER, frag_buffer);
		if (!geo_buffer.empty())
			LoadFromString(GEOMETRY_SHADER, geo_buffer);
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
	if (_shaders[VERTEX_SHADER] != 0)
		glAttachShader(_program, _shaders[VERTEX_SHADER]);
	if (_shaders[FRAGMENT_SHADER] != 0)
		glAttachShader(_program, _shaders[FRAGMENT_SHADER]);
	if (_shaders[GEOMETRY_SHADER] != 0)
		glAttachShader(_program, _shaders[GEOMETRY_SHADER]);

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

	if (_shaders[VERTEX_SHADER] != 0)
		glDeleteShader(_shaders[VERTEX_SHADER]);
	if (_shaders[FRAGMENT_SHADER] != 0)
		glDeleteShader(_shaders[FRAGMENT_SHADER]);
	if (_shaders[GEOMETRY_SHADER] != 0)
		glDeleteShader(_shaders[GEOMETRY_SHADER]);
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
