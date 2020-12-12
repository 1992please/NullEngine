#pragma once
#include "GlobalDefs.h"
#include <vector>

struct FCachedMaterial
{

	struct CachedColorChannel
	{
		char* TexturePath;
		float mColor[4];

		CachedColorChannel()
		{
			mColor[0] = 0.0f;
			mColor[1] = 0.0f;
			mColor[2] = 0.0f;
			mColor[3] = 1.0f;
			TexturePath = NULL;
		}
	};

	CachedColorChannel mEmissive;
	CachedColorChannel mAmbient;
	CachedColorChannel mDiffuse;
	CachedColorChannel mSpecular;
	float mShinness;
};

struct FCachedMesh
{
	float* VerticesData;
	float* NormalsData;
	float* UVsData;
	unsigned int* IndicesData;
	int mPolygonVertexCount;
	int mPolygonCount;
	int* mSubMeshesOffsets;
	int* mSubMeshesTriangleCounts;
	FCachedMaterial** mMaterials;
	int mSubMeshesCount;
	FCachedMesh() : mPolygonVertexCount(0), mPolygonCount(0), mSubMeshesCount(0) {}
	~FCachedMesh()
	{
		if (VerticesData)
			delete[] VerticesData;
		if (NormalsData)
			delete[] NormalsData;
		if (UVsData)
			delete[] UVsData;
		if (IndicesData)
			delete[] IndicesData;
		if (mSubMeshesOffsets)
			delete[] mSubMeshesOffsets;
		if (mSubMeshesTriangleCounts)
			delete[] mSubMeshesTriangleCounts;
		if (mMaterials)
		{
			for (int lMaterialIndex = 0; lMaterialIndex < mSubMeshesCount; ++lMaterialIndex)
			{
				if (mMaterials[lMaterialIndex])
					delete mMaterials[lMaterialIndex];
			}
			delete[] mMaterials;
		}
	}
};

class FbxUtil
{
public:

	static void GetStaticMeshes(const char* InFileName, std::vector<FCachedMesh*>& Meshes);
	static void GetStaticMesh(const char* InFileName, FCachedMesh*& pMesh);
	static void DeAlocate(FCachedMesh* pMesh);
};