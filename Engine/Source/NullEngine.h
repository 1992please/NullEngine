#pragma  once
#include "EngineBase.h"
#include "Shader.h"
#include "FreeCamera.h"
#include "Math/Vector.h"

class NullEngine : public EngineBase
{
private:
	virtual void StartUp() override;
	void Update(float DeltaTime) override;
	virtual void Render() override;
	virtual void Shutdown() override;
	virtual void Init();
	Shader mShader;
	class Drawable* mDrawable;
	FreeCamera Camera;
	uint32 VBO, VAO;

public:
	NullEngine();
	~NullEngine();

private:
	float angle = 0;
};