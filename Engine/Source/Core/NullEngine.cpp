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
	lightAngle = 0.0f;
}

void NullEngine::StartUp()
{
	// initializations
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	mShader.CompileShader("Shaders/PhysicallyBasedRendering.glsl");

	Camera.InitCameraProjection(30.0f, (float)EngineConfigs.windowWidth, (float)EngineConfigs.windowHeight, 0.3f, 6000.0f);
	Camera.SetLocation(FVector(-500, 0, 100));
	Input::SetMousePos(EngineConfigs.windowWidth / 2, EngineConfigs.windowHeight / 2);
	mDrawable = new FbxMesh("InputFiles/basic_shapes/quad.fbx");
	mDrawable1 = new FbxMesh("InputFiles/basic_shapes/cube.fbx");
	mDrawable2 = new FbxMesh("InputFiles/spot_triangulated.fbx");

	//mDrawable = new CubeMesh(1.0f);
	mShader.Use();
	mShader.SetUniform("Fog.MaxDist", 1000.0f);
	mShader.SetUniform("Fog.MinDist", 100.0f);
	mShader.SetUniform("Fog.Color", 0.5f, 0.5f, 0.5f);
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

	lightAngle = FMath::Fmod(lightAngle + DeltaTime * 1.5f, 2 * PI);
	lightPos.X = FMath::Cos(lightAngle) * 700.0f;
	lightPos.Y = FMath::Sin(lightAngle) * 700.0f;
	lightPos.Z = 300.0f;
}

void NullEngine::Render()
{
	// clear buffers
	static const GLfloat green[] = { 0.2f, 0.3f, 0.3f, 1.0f };
	static const GLfloat one = 1.0f;
	glClearBufferfv(GL_COLOR, 0, green);
	glClearBufferfv(GL_DEPTH, 0, &one);

	FMatrix View = Camera.GetViewMatrix();

	mShader.Use();

	mShader.SetUniform("Light[0].Position", View.TransformVector(FVector(0, 0.15f, 1.0f)));
	mShader.SetUniform("Light[0].L", FVector(.3f));
	mShader.SetUniform("Light[1].Position", View.TransformPosition(lightPos));
	mShader.SetUniform("Light[1].L", FVector(450000.0f));
	mShader.SetUniform("Light[2].Position", View.TransformPosition(FVector(-700, 700, 300)));
	mShader.SetUniform("Light[2].L", FVector(450000.0f));
	


	//{
	//	FRotationTranslationMatrix UnScaledModel(FRotator(0, 0, 0.0f), FVector(0.0f, 0.f, 0.0f));
	//	FMatrix Model = UnScaledModel.ApplyScale(700);
	//	FMatrix ModelView = Model * View;
	//	FMatrix MVP = ModelView * Camera.GetProjectionMatrix();

	//	mShader.SetUniform("ModelView", ModelView);
	//	mShader.SetUniform("Model", Model);
	//	mShader.SetUniform("View", View);
	//	mShader.SetUniform("MVP", MVP);
	//	mDrawable->render();
	//}
	//
	//{
	//	FRotationTranslationMatrix UnScaledModel(FRotator(0, 0, 0.0f), FVector(0.0f, 300.f, 50.0f));
	//	FMatrix Model = UnScaledModel.ApplyScale(100);
	//	FMatrix ModelView = Model * View;
	//	FMatrix MVP = ModelView * Camera.GetProjectionMatrix();

	//	mShader.SetUniform("ModelView", ModelView);
	//	mShader.SetUniform("Model", Model);
	//	mShader.SetUniform("View", View);
	//	mShader.SetUniform("MVP", MVP);
	//	mDrawable1->render();
	//}

	// Draw dielectric cows with varying roughness
	int numCows = 9;
	FVector cowBaseColor(0.1f, 0.33f, 0.17f);
	for (int i = 0; i < numCows; i++) {
		float cowX = i * (1000.0f / (numCows - 1)) - 500.0f;
		float rough = (i + 1) * (1.0f / numCows);
		RenderSpot(FVector(cowX, 0, 0), rough, 0, cowBaseColor);
	}

	// Draw metal cows
	float metalRough = 0.43f;
	// Gold
	RenderSpot(FVector(-300.0f, 0.0f, 300.0f), metalRough, 1, FVector(1, 0.71f, 0.29f));
	// Copper  									  
	RenderSpot(FVector(-100.5f, 0.0f, 300.0f), metalRough, 1, FVector(0.95f, 0.64f, 0.54f));
	// Aluminum									  
	RenderSpot(FVector(-0.0f, 0.0f, 300.0f), metalRough, 1, FVector(0.91f, 0.92f, 0.92f));
	// Titanium
	RenderSpot(FVector(100.5f, 0.0f, 300.0f), metalRough, 1, FVector(0.542f, 0.497f, 0.449f));
	// Silver  
	RenderSpot(FVector(300.0f, 0.0f, 300.0f), metalRough, 1, FVector(0.95f, 0.93f, 0.88f));

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

void NullEngine::RenderSpot(FVector Pos, float rough, bool metal, FVector Color)
{
	{
		mShader.SetUniform("Material.Rough", rough);
		mShader.SetUniform("Material.Metal", (int)metal);
		mShader.SetUniform("Material.Color", Color);

		FRotationTranslationMatrix UnScaledModel(FRotator(0, 90, 0.0f), Pos);
		FMatrix Model = UnScaledModel.ApplyScale(100);
		FMatrix View = Camera.GetViewMatrix();
		FMatrix ModelView = Model * View;
		FMatrix MVP = ModelView * Camera.GetProjectionMatrix();

		mShader.SetUniform("ModelView", ModelView);
		mShader.SetUniform("Model", Model);
		mShader.SetUniform("View", View);
		mShader.SetUniform("MVP", MVP);
		mDrawable2->render();
	}
}

NullEngine::~NullEngine()
{
}
