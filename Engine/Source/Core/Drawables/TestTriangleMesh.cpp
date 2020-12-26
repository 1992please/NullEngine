#include "TestTriangleMesh.h"

TestTriangleMesh::TestTriangleMesh(float side)
{
	std::vector<float> p = {
	   0.0f, 0.0f, 0.0f, 
	   0.0f, side, 0.0f,
	   side ,0.0f, 0.0f
	};

	std::vector<float> n = {
		// Front
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
	};

	std::vector<float> tex = {
		// Front
		0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f
	};

	std::vector<uint32> el = {
		0,1,2
	};

	InitBuffers(&el, &p, &n, &tex);
}
