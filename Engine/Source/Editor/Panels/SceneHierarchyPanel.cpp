#include "SceneHierarchyPanel.h"
#include "imgui/imgui.h"

FSceneHierarchyPanel::FSceneHierarchyPanel(FScene& InScene)
	: Scene(InScene)
{

}

void FSceneHierarchyPanel::OnImGUIRender()
{
	ImGui::Begin("Scene Heirarchy");
	ImGui::End();

	ImGui::ShowDemoWindow();
}


