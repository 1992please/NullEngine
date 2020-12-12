#pragma once
#include "Mesh.h"

class FbxMesh : public Mesh
{
public:
	FbxMesh(const char* InFileName);
};
