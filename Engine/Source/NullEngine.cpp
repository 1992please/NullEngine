#include "NullEngine.h"
#include "Math/RotationTranslationMatrix.h"
#include "Math/PrespectiveMatrix.h"
#include "GL/gl3w.h"

NullEngine::NullEngine() : Mesh(0.7f, 0.3f, 30, 30)
{
}

void NullEngine::StartUp()
{
	mShader.CompileShader("Engine/Shaders/TestShader.glsl");


	glEnable(GL_DEPTH_TEST);

}

void NullEngine::Update(float DeltaTime)
{
}

void NullEngine::Render()
{
	FRotationTranslationMatrix Model(FRotator(35,0, -35), FVector(0.0f));
	FLookAtMatrix View(FVector(0.0f, 0.0f, 2.0f), FVector(0.0f), FVector(0.0f, 1.0f, 0.0f));
	FPerspectiveMatrix ProjectionMatrix(FMath::DegreesToRadian(70.0f), (float)EngineConfigs.windowWidth, (float)EngineConfigs.windowHeight, 0.3f, 100.0f);

	mShader.Use();
	mShader.SetUniform("Kd", 0.9f, 0.5f, 0.3f);
	mShader.SetUniform("Ld", 1.0f, 1.0f, 1.0f);
	mShader.SetUniform("LightPosition", View.TransformFVector4(FVector4(5.0f, 5.0f, 2.0f, 1.0f)));



}

void NullEngine::Shutdown()
{
}

void NullEngine::Init()
{
	EngineBase::Init();
	//EngineConfigs.flags.fullscreen = 1;
}

NullEngine::~NullEngine()
{
}
