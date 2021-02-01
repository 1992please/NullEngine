#pragma once
#include "Renderer/GraphicLayers/GraphicLayer.h"

class FImGuiLayer :public FGraphicLayer
{
public:
	FImGuiLayer();
	~FImGuiLayer();
	virtual void OnAttach() override;
	virtual void OnDettach() override;
	virtual void OnEvent(IEvent& InEvent) override;
	virtual void OnImGuiRender() override;
	void Begin();
	void End();
private:
	bool bBlockEvents = true;
};

