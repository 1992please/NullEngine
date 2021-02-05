#pragma once
#include "Core/CoreTypes.h"
#include "Renderer/Components/RenderCommand.h"
#include "Core/Math/Matrix.h"

class FRenderer
{
public:
	static void InitRenderer();
	static void OnWindowResize(uint32 InWidth, uint32 InHeight);
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