#pragma  once
#include "EngineBase.h"
#include "Shader.h"

class NullEngine : public EngineBase
{
private:
	virtual void StartUp() override;
	void Update(float DeltaTime) override;
	virtual void Render() override;
	virtual void Shutdown() override;
	virtual void Init();
	Shader mShader;
	uint32 vao;
public:
	NullEngine();
	~NullEngine();

private:
	float angle = 0;
};