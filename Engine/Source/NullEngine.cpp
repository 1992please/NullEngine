#include "NullEngine.h"
#include "Math/RotationTranslationMatrix.h"
#include "Math/PrespectiveMatrix.h"
#include "GL/gl3w.h"
#include "Drawables/CubeMesh.h"
#include "Drawables/TorusMesh.h"
#include "Drawables/FbxMesh.h"
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
	mShader.CompileShader("Engine/Shaders/TestShader.glsl");

	Camera.InitCameraProjection(30.0f, (float)EngineConfigs.windowWidth, (float)EngineConfigs.windowHeight, 0.3f, 6000.0f);


	Input::SetMousePos(EngineConfigs.windowWidth / 2, EngineConfigs.windowHeight / 2);
	mDrawable = new FbxMesh("Engine/InputFiles/basic_shapes/quad.fbx");
	mMeshObject = new FbxMesh("Engine/InputFiles/basic_shapes/quad.fbx");
	//mDrawable = new CubeMesh(1.0f);
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

	FMatrix View = Camera.GetViewMatrix();
	FVector WorldLight(5.0f, 5.0f, 2.0f);

	mShader.Use();
	
	GLuint adsIndex = glGetSubroutineIndex(mShader.GetHandle(),
		GL_VERTEX_SHADER,
		"phongModel");
	GLuint diffuseIndex = glGetSubroutineIndex(mShader.GetHandle(),
		GL_VERTEX_SHADER,
		"diffuseOnly");

	mShader.SetUniform("Material.Kd", 0.9f, 0.5f, 0.3f);
	mShader.SetUniform("Light.Ld", 1.0f, 1.0f, 1.0f);
	mShader.SetUniform("Light.Position", FVector(View.TransformPosition(WorldLight)));
	mShader.SetUniform("Material.Ka", 0.9f, 0.5f, 0.3f);
	mShader.SetUniform("Light.La", 0.1f, 0.1f, 0.1f);
	mShader.SetUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
	mShader.SetUniform("Light.Ls", 1.0f, 1.0f, 1.0f);
	mShader.SetUniform("Material.Shininess", 100.0f);

	{
		glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &adsIndex);
		FRotationTranslationMatrix Model(FRotator(0, 0, 0.0f), FVector(-2.0f, 0.0f, 0.0f));
		FMatrix ModelView = Model * View;
		FMatrix MVP = ModelView * Camera.GetProjectionMatrix();

		mShader.SetUniform("ModelView", ModelView);
		mShader.SetUniform("Model", Model);
		mShader.SetUniform("View", View);
		mShader.SetUniform("MVP", MVP);
		//mDrawable->render();
		mMeshObject->render();
	}

	{
		glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &diffuseIndex);
		FRotationTranslationMatrix Model(FRotator(0, 0, 0.0f), FVector(2.0f, 0.0f, 0.0f));
		FMatrix ModelView = Model * View;
		FMatrix MVP = ModelView * Camera.GetProjectionMatrix();

		mShader.SetUniform("ModelView", ModelView);
		mShader.SetUniform("Model", Model);
		mShader.SetUniform("View", View);
		mShader.SetUniform("MVP", MVP);
		mDrawable->render();
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
