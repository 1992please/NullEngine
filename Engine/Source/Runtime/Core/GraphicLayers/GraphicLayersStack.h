#pragma once
#include "Core/CoreTypes.h"
#include "GraphicLayer.h"
#include "Core/Containers/Array.h"

class NE_API FGraphicLayersStack
{
public:
	FGraphicLayersStack();
	~FGraphicLayersStack();

	void PushLayer(FGraphicLayer* InLayer);
	void PopLayer(FGraphicLayer* InLayer);
	
	void PushOverlay(FGraphicLayer* InOverlay);
	void PopOverlay(FGraphicLayer* InOverlay);

	typedef FGraphicLayer** LayerIterator;

	FORCEINLINE FGraphicLayer**				begin() { return GraphicLayers.begin(); }
	FORCEINLINE FGraphicLayer*const *		begin() const { return GraphicLayers.begin(); }
	FORCEINLINE FGraphicLayer**				end() { return GraphicLayers.end(); }
	FORCEINLINE FGraphicLayer*const *		end() const { return GraphicLayers.end(); }

private:
	TArray<FGraphicLayer*> GraphicLayers;
	uint32 LayerInsertIndex;
};

