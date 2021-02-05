#include "Renderer.h"
#include "Renderer/Components/RendererCamera.h"
#include "Renderer/Components/Shader.h"
#include "Renderer/Components/Renderer2D.h"

FRenderer::FSceneData* FRenderer::SceneData = new FRenderer::FSceneData;

void FRenderer::InitRenderer()
{
	FRenderCommand::Init();
	FRenderer2D::Init();
}

void FRenderer::OnWindowResize(uint32 InWidth, uint32 InHeight)
{
	FRenderCommand::SetViewport(0, 0, InWidth, InHeight);
}

void FRenderer::BeginScene(FOrthographicCamera& Camera)
{
	SceneData->ViewProjectionMatrix = Camera.GetViewProjectionMatrix();
}

void FRenderer::EndScene()
{

}

void FRenderer::Submit(IShader* InShader, const IVertexArray* InVertexArray, const FMatrix& Model)
{
	InShader->Bind();
	InShader->SetMatrix("u_ViewProjection", SceneData->ViewProjectionMatrix);
	InShader->SetMatrix("u_Model", Model);
	InVertexArray->Bind();
	FRenderCommand::DrawIndexed(InVertexArray);
}

