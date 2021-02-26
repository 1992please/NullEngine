#include "SceneHierarchyPanel.h"
#include "imgui/imgui.h"

FSceneHierarchyPanel::FSceneHierarchyPanel(FScene& InScene, FEntity& InSelectedEntity)
	: Scene(InScene)
	, SelectedEntity(InSelectedEntity)
{

}

void FSceneHierarchyPanel::OnImGUIRender()
{
	ImGui::Begin("Scene Heirarchy");
	for (const int32 EntityID : Scene.GetSceneStorage())
	{
		DrawEntityNode(FEntity(EntityID, &Scene));
	}

	if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0))
		SelectedEntity = {};

	ImGui::End();
}

void FSceneHierarchyPanel::DrawEntityNode(FEntity Entity)
{
	FTagComponent& TagComponent = Entity.GetComponent<FTagComponent>();
	ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ((SelectedEntity == Entity) ? ImGuiTreeNodeFlags_Selected : 0);
	bool bOpened = (ImGui::TreeNodeEx((void*)(size_t)Entity.GetID(), Flags, TagComponent.Name));
	if (ImGui::IsItemClicked())
	{
		SelectedEntity = Entity;
	}

	if(bOpened)
	{
		ImGui::TreePop();
	}
}

