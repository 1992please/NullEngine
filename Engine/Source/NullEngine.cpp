#include "NullEngine.h"
#include "Math/RotationTranslationMatrix.h"
#include "Math/PrespectiveMatrix.h"
#include "GL/gl3w.h"
#include "Drawables/CubeMesh.h"
#include "Drawables/TorusMesh.h"
#include "Drawables/FbxMesh.h"
#include "Drawables/TestTriangleMesh.h"
#include <sstream>
#include <iostream>

NullEngine::NullEngine()
{
	mDrawable = nullptr;
}

void NullEngine::StartUp()
{
	// initializations
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	mShader.CompileShader("Engine/Shaders/FragmentShading.glsl");

	Camera.InitCameraProjection(30.0f, (float)EngineConfigs.windowWidth, (float)EngineConfigs.windowHeight, 0.3f, 6000.0f);


	Input::SetMousePos(EngineConfigs.windowWidth / 2, EngineConfigs.windowHeight / 2);
	mDrawable = new TestTriangleMesh(200);
	mDrawable1 = new FbxMesh("Engine/InputFiles/basic_shapes/cube.fbx");
	mDrawable2 = new FbxMesh("Engine/InputFiles/humanoid.fbx");

	//mDrawable = new CubeMesh(1.0f);
}

void NullEngine::Update(float DeltaTime)
{
	Camera.MoveForward(mInput.GetAxis(EMovementAxis::MOVEFORWARD) * DeltaTime * 500);
	Camera.MoveRight(mInput.GetAxis(EMovementAxis::MOVERIGHT) * DeltaTime * 500);
	Camera.MoveUp(mInput.GetAxis(EMovementAxis::MOVEUP) * DeltaTime * 500);
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

	FMatrix View = Camera.GetViewMatrix();
	FVector WorldLight0(-5000.0f, -5000.0f, 5000.0f);
	FVector WorldLight1(5000.0f, -5000.0f, 5000.0f);
	FVector WorldLight2(-5000.0f, 5000.0f, 5000.0f);
	FVector WorldLight3(5000.0f, 5000.0f, 5000.0f);
	FVector WorldLight4(0.0f, 0.0f, 5000.0f);

	FVector LightDirection(1.0f, 1.0f, 1.0f);

	mShader.Use();

	mShader.SetUniform("Lights[0].Position", View.TransformPosition(WorldLight0));
	mShader.SetUniform("Lights[1].Position", View.TransformPosition(WorldLight1));
	mShader.SetUniform("Lights[2].Position", View.TransformPosition(WorldLight2));
	mShader.SetUniform("Lights[3].Position", View.TransformPosition(WorldLight3));
	mShader.SetUniform("Lights[4].Position", View.TransformVector(LightDirection));
						
	//mShader.SetUniform("Lights[0].L", 0.0f, 0.8f, 0.8f);
	//mShader.SetUniform("Lights[1].L", 0.0f, 0.0f, 0.8f);
	//mShader.SetUniform("Lights[2].L", 0.8f, 0.0f, 0.0f);
	//mShader.SetUniform("Lights[3].L", 0.0f, 0.8f, 0.0f);
	mShader.SetUniform("Lights[4].L", 0.8f, 0.8f, 0.8f);
						
	//mShader.SetUniform("Lights[0].La", 0.0f, 0.2f, 0.2f);
	//mShader.SetUniform("Lights[1].La", 0.0f, 0.0f, 0.2f);
	//mShader.SetUniform("Lights[2].La", 0.2f, 0.0f, 0.0f);
	//mShader.SetUniform("Lights[3].La", 0.0f, 0.2f, 0.0f);
	mShader.SetUniform("Lights[4].La", 0.2f, 0.2f, 0.2f);

	mShader.SetUniform("Material.Shininess", 100.0f);
	mShader.SetUniform("Material.Ka", 0.9f, 0.5f, 0.3f);
	mShader.SetUniform("Material.Kd", 0.9f, 0.5f, 0.3f);
	mShader.SetUniform("Material.Ks", 0.8f, 0.8f, 0.8f);


	{
		FRotationTranslationMatrix Model(FRotator(0, 0, 0.0f), FVector(0.0f, 0.0f, 0.0f));
		FMatrix ModelView = Model * View;
		FMatrix MVP = ModelView * Camera.GetProjectionMatrix();

		mShader.SetUniform("ModelView", ModelView);
		mShader.SetUniform("Model", Model);
		mShader.SetUniform("View", View);
		mShader.SetUniform("MVP", MVP);
		mDrawable->render();
	}
	
	{
		FRotationTranslationMatrix UnScaledModel(FRotator(0, 0, 0.0f), FVector(0.0f, 300.f, 0.0f));
		FMatrix Model = UnScaledModel.ApplyScale(100);
		FMatrix ModelView = Model * View;
		FMatrix MVP = ModelView * Camera.GetProjectionMatrix();

		mShader.SetUniform("ModelView", ModelView);
		mShader.SetUniform("Model", Model);
		mShader.SetUniform("View", View);
		mShader.SetUniform("MVP", MVP);
		mDrawable1->render();
	}

	{
		FRotationTranslationMatrix Model(FRotator(0, 0, 0.0f), FVector(0.0f, 0.0f, 0.0f));
		FMatrix ModelView = Model * View;
		FMatrix MVP = ModelView * Camera.GetProjectionMatrix();

		mShader.SetUniform("ModelView", ModelView);
		mShader.SetUniform("Model", Model);
		mShader.SetUniform("View", View);
		mShader.SetUniform("MVP", MVP);
		mDrawable2->render();
	}

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
