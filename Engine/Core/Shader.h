#pragma once
#include "GlobalDefs.h"
#include <string>
#include <map>

using namespace std;

class Shader
{
public:
	Shader();
	~Shader();
	void LoadFromFile(const string& filename);
	void CreateAndLinkProgram();
	void Use();
	void UnUse();
	void AddAttribute(const string& attribute);
	void AddUniform(const string& uniform);
	int32 operator[](const string& attribute);
	int32 operator()(const string& uniform);
	void DeleteShaderProgram();

private:
	enum ShaderType { VERTEX_SHADER, FRAGMENT_SHADER, GEOMETRY_SHADER };
	int32  _program;
	int32 _shaders[3];
	map<string, int32> _attributeList;
	map<string, int32> _uniformLocationList;
	static uint32 ShaderTypes[3];
	void LoadFromString(ShaderType InShaderType, const string& source);

};

