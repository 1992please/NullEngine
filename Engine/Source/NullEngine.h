#pragma  once
#include "EngineBase.h"
#include "Shader.h"
#include "Drawables/TorusMesh.h"

class NullEngine : public EngineBase
{
private:
	virtual void StartUp() override;
	void Update(float DeltaTime) override;
	virtual void Render() override;
	virtual void Shutdown() override;
	virtual void Init();
	Shader mShader;
	TorusMesh Mesh;
public:
	NullEngine();
	~NullEngine();

private:
	float angle = 0;
};