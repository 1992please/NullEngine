#include "SceneHierarchyPanel.h"
#include "imgui/imgui.h"

FSceneHierarchyPanel::FSceneHierarchyPanel(FScene& InScene)
	: Scene(InScene)
{

}

void FSceneHierarchyPanel::OnImGUIRender()
{
	ImGui::Begin("Scene Heirarchy");
	for (const int32 EntityID : Scene.GetSceneStorage())
	{
		DrawEntityNode(FEntity(EntityID, &Scene));
	}

	ImGui::End();

	ImGui::ShowDemoWindow();
}

void FSceneHierarchyPanel::DrawEntityNode(FEntity Entity)
{
	FTagComponent& TagComponent = Entity.GetComponent<FTagComponent>();
	ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ((SelectedEntity == Entity) ? ImGuiTreeNodeFlags_Selected : 0);
	if (ImGui::TreeNodeEx((void*)(size_t)Entity.GetID(), Flags, TagComponent.Name))
	{
		ImGui::TreePop();
	}
}

