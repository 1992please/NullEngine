#include "OpenGLRendererAPI.h"
#include "GL/gl3w.h"

void FOpenGLRendererAPI::InitRendererAPI()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void FOpenGLRendererAPI::SetClearColor(const FLinearColor& Color)
{
	glClearColor(Color.R, Color.G, Color.B, Color.A);
}

void FOpenGLRendererAPI::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FOpenGLRendererAPI::DrawIndexed(const IVertexArray* InVertexArray)
{
	glDrawElements(GL_TRIANGLES, InVertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
}
