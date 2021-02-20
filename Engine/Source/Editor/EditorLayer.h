#include "NullEngine.h"
#include "EditorViewport/EditorCamera.h"

class FEditorLayer : public FGraphicLayer
{
public:
	FEditorLayer();
	virtual ~FEditorLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDettach() override;

	void OnUpdate(float DeltaTime) override;
	virtual void OnImGuiRender() override;
	void OnEvent(IEvent& InEvent) override;

private:


	IShader* FlatShader;
	class ITexture2D* MarioTexture;
	class ITexture2D* SpriteSheet;

	class IVertexArray* VertexArray;
	class IFrameBuffer* FrameBuffer;
	FVector CameraPosition;
	float CameraRotation;


	FLinearColor SquareColor;

	FScene* Scene;
	FEntity Entity;
	FEntity CameraEntity;

	FEditorCamera EditorCamera;

	bool bIsViewportFocused;
	bool bIsViewportHovered;
	FVector2 ViewportSize;
};