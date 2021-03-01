#include "SceneHierarchyPanel.h"
#include "imgui/imgui.h"

FSceneHierarchyPanel::FSceneHierarchyPanel(FScene& InScene, FEntity& InSelectedEntity)
	: Scene(InScene)
	, SelectedEntity(InSelectedEntity)
{

}

void FSceneHierarchyPanel::OnImGUIRender()
{
	ImGui::Begin("Scene Heirarchy", 0, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Add"))
		{
			if (ImGui::MenuItem("Entity"))
			{
				Scene.CreateEntity();
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

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
	ImGuiTreeNodeFlags Flags = 
		ImGuiTreeNodeFlags_OpenOnArrow | 
		ImGuiTreeNodeFlags_SpanAvailWidth | 
		((SelectedEntity == Entity) ? ImGuiTreeNodeFlags_Selected : 0);
	bool bOpened = ImGui::TreeNodeEx((void*)(size_t)Entity.GetID(), Flags, TagComponent.Name);

	if (ImGui::IsItemClicked())
	{
		SelectedEntity = Entity;
	}

	bool bShouldDeleteEntity = false;

	if (ImGui::BeginPopupContextItem())
	{
		bShouldDeleteEntity = (ImGui::MenuItem("Delete Entity"));
		ImGui::EndPopup();
	}

	if(bOpened)
	{
		ImGui::TreePop();
	}

	if (bShouldDeleteEntity)
	{
		if (Entity == SelectedEntity)
		{
			SelectedEntity = {};
		}

		Scene.DestroyEntity(Entity.GetID());
	}
}

