#include "NullEngine.h"


NullEngine::NullEngine()
{
}

void NullEngine::StartUp()
{
	mShader.CompileShader("Engine/Shaders/TestShader.glsl");
	mShader.AddUniform("RotationMatrix");

	float positionData[] = {
		-0.8f, -0.8f, 0.0f,
		 0.8f, -0.8f, 0.0f,
		 0.0f,  0.8f, 0.0f };
	float colorData[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f };

	// Create and populate the buffer objects
	GLuint vboHandles[2];
	glGenBuffers(2, vboHandles);
	GLuint positionBufferHandle = vboHandles[0];
	GLuint colorBufferHandle = vboHandles[1];

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), positionData, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), colorData, GL_STATIC_DRAW);

	// Create and set-up the vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);  // Vertex position
	glEnableVertexAttribArray(1);  // Vertex color

	glBindVertexBuffer(0, positionBufferHandle, 0, sizeof(GLfloat) * 3);
	glBindVertexBuffer(1, colorBufferHandle, 0, sizeof(GLfloat) * 3);

	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(0, 0);
	glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(1, 1);
	glBindVertexArray(0);

}

void NullEngine::Update(float DeltaTime)
{
	angle += DeltaTime;
	if (angle >= 360.0f) angle -= 360.0f;
}

void NullEngine::Render()
{
	static const GLfloat green[] = { 0.0f, 0.25f, 0.0f, 1.0f };
	static const GLfloat one = 1.0f;

	FMatrix RotationMat = FMatrix::Rotate(FMatrix::Identity, angle, FVector(0.0f, 1.0f, 0.0f));

	glClearBufferfv(GL_COLOR, 0, green);
		//glClearBufferfv(GL_DEPTH, 0, &one);
	//glClear(GL_COLOR_BUFFER_BIT);
	mShader.Use();
	glUniformMatrix4fv(mShader("RotationMatrix"), 1, GL_FALSE, RotationMat);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

	mShader.UnUse();
}

void NullEngine::Shutdown()
{
	mShader.DeleteShaderProgram();
}

void NullEngine::Init()
{
	EngineBase::Init();
	//EngineConfigs.flags.fullscreen = 1;
}

NullEngine::~NullEngine()
{
}
