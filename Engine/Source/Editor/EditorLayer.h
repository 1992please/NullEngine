#include "NullEngine.h"
#include "EditorViewport/EditorCamera.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/EntityPropertiesPanel.h"

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
	bool OnKeyPressed(class FKeyPressedEvent& InEvent);
private:
	void NewScene();
	void OpenScene();
	void SaveScene();
	void SaveSceneAs();
private:
	IShader* FlatShader;
	class ITexture2D* MarioTexture;
	class ITexture2D* SpriteSheet;

	class IVertexArray* VertexArray;
	class IFrameBuffer* FrameBuffer;
	FVector CameraPosition;
	float CameraRotation;

	FScene Scene;
	FString CurrentScenePath;

	FEntity CameraEntity;

	FEntity SelectedEntity;
	FSceneHierarchyPanel SceneHierarchyPanel;
	FEntityPropertiesPanel EntityPropertiesPanel;
	FEditorCamera EditorCamera;
	int32 GizmoType;

	bool bIsViewportFocused;
	bool bIsViewportHovered;
	FVector2 ViewportSize;

	TArray<uint8> SceneSerializationData;
};