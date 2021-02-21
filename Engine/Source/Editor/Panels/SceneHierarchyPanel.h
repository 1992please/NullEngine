#pragma once
#include "NullEngine.h"

class FSceneHierarchyPanel
{
public:
	FSceneHierarchyPanel(FScene& InScene);

	void OnImGUIRender();
	FORCEINLINE FEntity GetSelectedEntity() const { return SelectedEntity; }
	FORCEINLINE void SetSelectedEntity(FEntity InEntity) { SelectedEntity = InEntity; }

private:
	FScene& Scene;
	FEntity SelectedEntity;
};