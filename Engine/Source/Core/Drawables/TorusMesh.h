#pragma once
#include "Mesh.h"

class TorusMesh : public Mesh
{
public:
	TorusMesh(float OuterRadius, float InnerRadius, uint32 nsides, uint32 nrings);
};