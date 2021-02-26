#pragma once
#include "NullEngine.h"

class FSceneHierarchyPanel
{
public:
	FSceneHierarchyPanel(FScene& InScene, FEntity& SelectedEntity);

	void OnImGUIRender();
private:
	void DrawEntityNode(FEntity Entity);
private:
	FScene& Scene;
	FEntity& SelectedEntity;
};