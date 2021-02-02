#pragma once

#include "Renderer/Components/RenderCommand.h"
#include "Core/Math/Matrix.h"

class FRenderer
{
public:
    static void BeginScene(class FOrthographicCamera& Camera);
    static void EndScene();
    static void Submit(class IShader* InShader, const IVertexArray* InVertexArray, const FMatrix& Model = FMatrix::Identity);
private:
	struct FSceneData
	{
		FMatrix ViewProjectionMatrix;
	};

	static FSceneData* SceneData;

};