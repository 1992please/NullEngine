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
	void Use();
	void UnUse();
	void DeleteShaderProgram();
	FORCEINLINE uint32 GetID() {return Handle;}
private:
	int32  Handle;
	bool bLinked;
	std::map<string, int> UniformLocations;
	uint32 LoadFromString(int32 ShaderIdx, const string& source);
	void CreateAndLinkProgram(uint32* _shaders);
	inline int32 GetUniformLocation(const char *name);
public:
	void BindAttribLocation(uint32 location, const char *name);
	void BindFragDataLocation(uint32 location, const char *name);

	void SetUniform(const char *name, float x, float y, float z);
	void SetUniform(const char *name, const struct FVector2& v);
	void SetUniform(const char *name, const struct FVector& v);
	void SetUniform(const char *name, const struct FVector4& v);
	//void SetUniform(const char *name, const glm::mat4 &m);
	//void SetUniform(const char *name, const glm::mat3 &m);
	void SetUniform(const char *name, float val);
	void SetUniform(const char *name, int32 val);
	void SetUniform(const char *name, bool val);
	void SetUniform(const char *name, uint32 val);

	//void findUniformLocations();
	//void printActiveUniforms();
	//void printActiveUniformBlocks();
	//void printActiveAttribs();

	//const char *getTypeString(GLenum type);
};

