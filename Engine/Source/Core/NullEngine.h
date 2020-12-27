#pragma  once
#include "EngineBase.h"
#include "Shader.h"
#include "FreeCamera.h"


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
	class Drawable* mDrawable1;
	class Drawable* mDrawable2;
	class Drawable* mDrawable3;

	FreeCamera Camera;
	uint32 VBO, VAO;

	float lightAngle;
	FVector lightPos;
	void RenderSpot(FVector Pos, float rough, bool metal, FVector Color);
public:
	NullEngine();
	~NullEngine();

private:
	float angle = 0;
};