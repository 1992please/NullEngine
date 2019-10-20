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

	CameraPos = FVector(0.0f, 0.0f, 0.0f);
	
	mDrawable = new TorusMesh(1, 2, 10, 30);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// positions         // colors
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
		 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 

	};

	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	CameraPos.X = 5.f;
}

void NullEngine::Update(float DeltaTime)
{
	CameraPos.X += mInput.GetAxis(EMovementAxis::MOVEFORWARD) * DeltaTime * 5;
	CameraPos.Y += mInput.GetAxis(EMovementAxis::MOVERIGHT) * DeltaTime * 5;
	CameraPos.Z += mInput.GetAxis(EMovementAxis::MOVEUP) * DeltaTime * 5;
}

void NullEngine::Render()
{
	// clear buffers
	static const GLfloat green[] = { 0.2f, 0.3f, 0.3f, 1.0f };
	static const GLfloat one = 1.0f;
	glClearBufferfv(GL_COLOR, 0, green);
	glClearBufferfv(GL_DEPTH, 0, &one);

	FRotationTranslationMatrix Model(FRotator(0), FVector(0.0f));
	FLookAtMatrix View(CameraPos, FVector(0.0f), FVector(0.0f, 0.0f, 1.0f));
	FPerspectiveMatrix ProjectionMatrix(FMath::DegreesToRadian(30.0f), (float)EngineConfigs.windowWidth, (float)EngineConfigs.windowHeight, 0.3f, 6000.0f);
	FMatrix MMP = Model* View * ProjectionMatrix;
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
	
	mShader.SetUniform("MVP", MMP);
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
