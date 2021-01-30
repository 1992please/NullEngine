#include "Renderer.h"

void Renderer::BeginScene()
{

}

void Renderer::EndScene()
{

}

void Renderer::Submit(const IVertexArray* InVertexArray)
{
	FRenderCommand::DrawIndexed(InVertexArray);
}

