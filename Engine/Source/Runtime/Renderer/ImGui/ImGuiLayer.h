#pragma once
#include "Core/CoreTypes.h"
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

	FORCEINLINE void SetBlockingEvents(bool InBlockEvents) { bBlockEvents = InBlockEvents; }
	void SetDarkThemedColor();
private:
	bool bBlockEvents;
};

