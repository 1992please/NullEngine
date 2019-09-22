#pragma once
#include "GlobalDefs.h"
#include <string>
#include <map>

using namespace std;
#define SHADER_STAGES_NO 5
class Shader
{
public:
	Shader();
	~Shader();
	// Make it non-copyable
	Shader(const Shader &) = delete;
	Shader& operator=(const Shader&) = delete;

	void CompileShader(const string& filename);
	void CreateAndLinkProgram();
	void Use();
	void UnUse();
	void AddAttribute(const string& attribute);
	void AddUniform(const string& uniform);
	int32 operator[](const string& attribute);
	int32 operator()(const string& uniform);
	void DeleteShaderProgram();

private:
	int32  _program;
	int32 _shaders[SHADER_STAGES_NO];
	map<string, int32> _attributeList;
	map<string, int32> _uniformLocationList;
	static uint32 ShaderTypes[];
	void LoadFromString(int32 ShaderIdx, const string& source);

};

