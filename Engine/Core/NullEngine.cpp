#include "NullEngine.h"
#include <math.h>
#include "Shader.h"

NullEngine::NullEngine()
{
	mShader = new Shader();
}

void NullEngine::StartUp()
{
	mShader->LoadFromFile("Engine/Shaders/TestShader.glsl");
	glCreateVertexArrays(1, &vertex_array_object);
	glBindVertexArray(vertex_array_object);
}

void NullEngine::Update(float DeltaTime)
{
	const GLfloat color[] = { (float)sin(CurrentTime) * 0.5f + 0.5f,
		(float)cos(CurrentTime) * 0.5f + 0.5f,
		0.0f, 1.0f };
	glClearBufferfv(GL_COLOR, 0, color);

	// Use the program object we created earlier for rendering
	mShader->Use();

	// Draw one point
	glDrawArrays(GL_TRIANGLES, 0, 3);
	//glPointSize(40.0f);
	mShader->UnUse();
}

void NullEngine::Shutdown()
{
	mShader->DeleteShaderProgram();
	glDeleteVertexArrays(1, &vertex_array_object);
}

void NullEngine::Init()
{
	EngineBase::Init();
	//EngineConfigs.flags.fullscreen = 1;
}

NullEngine::~NullEngine()
{
	delete mShader;
}
