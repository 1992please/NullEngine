#pragma once
#include "Rendering/GraphicLayers/GraphicLayer.h"

class FImGuiLayer :public FGraphicLayer
{
public:
	FImGuiLayer();
	~FImGuiLayer();
	void OnAttach() override;
	void OnDettach() override;
	void OnUpdate(float DeltaTime) override;
	void OnEvent(IEvent& InEvent) override;

private:
	bool bBlockEvents = true;
};

