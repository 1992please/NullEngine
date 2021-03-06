#include "NullPCH.h"
#include "OpenGLRendererAPI.h"
#include "GL/gl3w.h"

void FOpenGLRendererAPI::Init()
{
	NE_PROFILE_FUNCTION();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
}

void FOpenGLRendererAPI::SetViewport(uint32 X, uint32 Y, uint32 InWidth, uint32 InHeight)
{
	glViewport(X, Y, InWidth, InHeight);
}

void FOpenGLRendererAPI::SetClearColor(const FLinearColor& Color)
{
	glClearColor(Color.R, Color.G, Color.B, Color.A);
}

void FOpenGLRendererAPI::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FOpenGLRendererAPI::DrawIndexed(const IVertexArray* InVertexArray, uint32 IndexCount)
{
	const uint32 Count = IndexCount ? IndexCount : InVertexArray->GetIndexBuffer()->GetCount();
	glDrawElements(GL_TRIANGLES, Count, GL_UNSIGNED_INT, nullptr);
}
