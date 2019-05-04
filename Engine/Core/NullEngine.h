#pragma  once
#include "EngineBase.h"

class NullEngine : public EngineBase
{

private:
	virtual void StartUp() override;
	void Update(float DeltaTime) override;
	virtual void Shutdown() override;
	virtual void Init();
	class Shader* mShader;
	GLuint vertex_array_object;
public:
	NullEngine();
	~NullEngine();
};