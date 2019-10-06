#pragma once

#include <vector>
#include "drawable.h"

class Mesh : public Drawable {

protected:

	uint32 nVerts;     // Number of vertices
	uint32 vao;        // The Vertex Array Object

	// Vertex buffers
	std::vector<uint32> buffers;

	virtual void InitBuffers(
		std::vector<uint32> * indices,
		std::vector<float> * points,
		std::vector<float> * normals,
		std::vector<float> * texCoords = nullptr,
		std::vector<float> * tangents = nullptr
	);

	virtual void deleteBuffers();

public:
	virtual ~Mesh();
	virtual void render() const;
	uint32 getVao() const { return vao; }
	uint32 getElementBuffer() { return buffers[0]; }
	uint32 getPositionBuffer() { return buffers[1]; }
	uint32 getNormalBuffer() { return buffers[2]; }
	uint32 getTcBuffer() { if (buffers.size() > 3) return buffers[3]; else return 0; }
	uint32 getNumVerts() { return nVerts; }
};
