#include "FbxMesh.h"
#include "FbxUtil.h"

FbxMesh::FbxMesh(const char* InFileName)
{
	FCachedMesh* CachedMesh = nullptr;
	FbxUtil::GetStaticMesh(InFileName, CachedMesh);
	if (CachedMesh && CachedMesh->IndicesData && CachedMesh->VerticesData && CachedMesh->NormalsData)
	{

		std::vector<uint32> indices(CachedMesh->IndicesData, CachedMesh->IndicesData + CachedMesh->mPolygonCount * 3);
		std::vector<float> points(CachedMesh->VerticesData, CachedMesh->VerticesData + CachedMesh->mPolygonVertexCount * 3);
		std::vector<float> normals(CachedMesh->NormalsData, CachedMesh->NormalsData + CachedMesh->mPolygonVertexCount * 3);
		if (CachedMesh->UVsData)
		{
			std::vector<float> texCoords(CachedMesh->UVsData, CachedMesh->UVsData + CachedMesh->mPolygonVertexCount * 2);
			InitBuffers(&indices, &points, &normals, &texCoords);
		}
		else
		{
			InitBuffers(&indices, &points, &normals);
		}
	}
}
