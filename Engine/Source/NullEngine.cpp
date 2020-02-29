#include "NullEngine.h"
#include "Math/RotationTranslationMatrix.h"
#include "Math/PrespectiveMatrix.h"
#include "GL/gl3w.h"
#include "Drawables/CubeMesh.h"
#include "Drawables/TorusMesh.h"
NullEngine::NullEngine()
{
	mDrawable = nullptr;
}

void NullEngine::StartUp()
{
	// initializations
	glEnable(GL_DEPTH_TEST);
	mShader.CompileShader("Engine/Shaders/TestShader.glsl");

	Camera.InitCameraProjection(30.0f, (float)EngineConfigs.windowWidth, (float)EngineConfigs.windowHeight, 0.3f, 6000.0f);


	Input::SetMousePos(EngineConfigs.windowWidth / 2, EngineConfigs.windowHeight / 2);
	mDrawable = new TorusMesh(0.7f, 0.3f, 100, 100);
}

void NullEngine::Update(float DeltaTime)
{
	Camera.MoveForward(mInput.GetAxis(EMovementAxis::MOVEFORWARD) * DeltaTime * 5);
	Camera.MoveRight(mInput.GetAxis(EMovementAxis::MOVERIGHT) * DeltaTime * 5);
	Camera.MoveUp(mInput.GetAxis(EMovementAxis::MOVEUP) * DeltaTime * 5);
	if (mInput.GetMouseKey(EMouseKey::Right))
	{
		Camera.Turn(mInput.GetAxis(EMovementAxis::TURN) * DeltaTime * 200);
		Camera.LookUp(-mInput.GetAxis(EMovementAxis::LOOKUP) * DeltaTime * 200);
	}
	Camera.OnUpdate(DeltaTime);
}

void NullEngine::Render()
{
	// clear buffers
	static const GLfloat green[] = { 0.2f, 0.3f, 0.3f, 1.0f };
	static const GLfloat one = 1.0f;
	glClearBufferfv(GL_COLOR, 0, green);
	glClearBufferfv(GL_DEPTH, 0, &one);

	FRotationTranslationMatrix Model(FRotator(-35, 35, 0.0f), FVector(0.0f));
	FMatrix MVP = Model* Camera.GetViewMatrix() * Camera.GetProjectionMatrix();
	//FVector TestVect(0.5f, -0.5f, 0.f);
	//FVector TestVect1(-0.5f, -0.5f, 0.f);
	//FVector TestVect2(0.0f, 0.5f, 0.f);
	//FVector Result = Model.TransformPosition(TestVect);
	//FVector Result1 = Model.TransformPosition(TestVect1);
	//FVector Result2 = Model.TransformPosition(TestVect2);
	//FVector4 Result3 = ProjectionMatrix.TransformFVector4(Result);
	//FVector4 Result4 = ProjectionMatrix.TransformFVector4(Result1);
	//FVector4 Result5 = ProjectionMatrix.TransformFVector4(Result2);
	//printf("%f\n", CameraPos.Z);
	mShader.Use();
	
	mShader.SetUniform("Kd", 0.9f, 0.5f, 0.3f);
	mShader.SetUniform("Ld", 1.0f, 1.0f, 1.0f);
	mShader.SetUniform("LightPosition", 5.0f, 5.0f, 2.0f);

	mShader.SetUniform("Model", Model);
	mShader.SetUniform("MVP", MVP);
	//mShader.SetUniform("Projection", ProjectionMatrix);
	//mShader.SetUniform("UnlitColor", FVector(1.0f,1.0f,1.0f));
	//mShader.SetUniform("Kd", 0.9f, 0.5f, 0.3f);
	//mShader.SetUniform("Ld", 1.0f, 1.0f, 1.0f);
	//mShader.SetUniform("LightPosition", View.TransformFVector4(FVector4(5.0f, 5.0f, 2.0f, 1.0f)));

	mDrawable->render();
	//glBindVertexArray(VAO);
	//glDrawArrays(GL_TRIANGLES, 0, 3);

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
