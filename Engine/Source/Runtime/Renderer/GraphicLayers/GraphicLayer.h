#pragma once
#include "Core/CoreTypes.h"
#include "Core/Events/Event.h"

class FGraphicLayer
{
public:
	FGraphicLayer(const FString& Name = "Graphics Layer");
	virtual void OnAttach() {}
	virtual void OnDettach() {}
	virtual void OnUpdate(float DeltaTime) {}
	virtual void OnImGuiRender() {}
	virtual void OnEvent(IEvent& InEvent) {}
	virtual ~FGraphicLayer();

	FORCEINLINE const FString& GetName() const { return LayerName; }
protected:
	FString LayerName;
};

