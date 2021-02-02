#include "Renderer.h"
#include "Renderer/Components/RendererCamera.h"
#include "Renderer/Components/Shader.h"

FRenderer::FSceneData* FRenderer::SceneData = new FRenderer::FSceneData;

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

