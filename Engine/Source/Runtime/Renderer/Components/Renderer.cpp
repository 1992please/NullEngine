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

void FRenderer::Submit(IShader* InShader, const IVertexArray* InVertexArray)
{
	InShader->Bind();
	InShader->SetMatrix("uViewProjection", SceneData->ViewProjectionMatrix);

	InVertexArray->Bind();
	FRenderCommand::DrawIndexed(InVertexArray);
}

