#pragma once
#include "NullEngine.h"

class FEntityPropertiesPanel
{
public:
	FEntityPropertiesPanel(FScene& InScene, FEntity& SelectedEntity);
	void OnImGUIRender();

private:
	FScene& Scene;
	FEntity& SelectedEntity;
};