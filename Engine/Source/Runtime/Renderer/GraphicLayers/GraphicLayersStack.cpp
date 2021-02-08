#include "NullPCH.h"
#include "GraphicLayersStack.h"



FGraphicLayersStack::FGraphicLayersStack()
{
	LayerInsertIndex = 0;
}

FGraphicLayersStack::~FGraphicLayersStack()
{
	for (FGraphicLayer* Layer : GraphicLayers)
	{
		Layer->OnDettach();
		delete Layer;
	}
}

void FGraphicLayersStack::PushLayer(FGraphicLayer* InLayer)
{
	GraphicLayers.Insert(InLayer, LayerInsertIndex);
	LayerInsertIndex++;
	InLayer->OnAttach();
}

void FGraphicLayersStack::PopLayer(FGraphicLayer* InLayer)
{
	if (GraphicLayers.RemoveSingle(InLayer))
	{
		InLayer->OnDettach();
		LayerInsertIndex--;
	}
}

void FGraphicLayersStack::PushOverlay(FGraphicLayer* InOverlay)
{
	GraphicLayers.Push(InOverlay);
	InOverlay->OnAttach();
}

void FGraphicLayersStack::PopOverlay(FGraphicLayer* InOverlay)
{
	if (GraphicLayers.RemoveSingle(InOverlay))
	{
		InOverlay->OnDettach();
	}
}
