#include "TorusMesh.h"
#include "Math/NullMath.h"
TorusMesh::TorusMesh(float OuterRadius, float InnerRadius, uint32 nsides, uint32 nrings)
{
	uint32 faces = nsides * nrings;
	int nVerts = nsides * (nrings + 1);   // One extra ring to duplicate first ring

	// Points
	std::vector<float> p(3 * nVerts);
	// Normals
	std::vector<float> n(3 * nVerts);
	// Tex coords
	std::vector<float> tex(2 * nVerts);
	// Elements
	std::vector<uint32> el(6 * faces);

	// Generate the vertex data
	float ringFactor =  (float)nrings;
	float sideFactor = 2 * PI / nsides;
	int idx = 0, tidx = 0;
	for (uint32 ring = 0; ring <= nrings; ring++) {
		float u = ring * ringFactor;
		float cu = FMath::Cos(u);
		float su = FMath::Sin(u);
		for (uint32 side = 0; side < nsides; side++) {
			float v = side * sideFactor;
			float cv = FMath::Cos(v);
			float sv = FMath::Sin(v);
			float r = (OuterRadius + InnerRadius * cv);
			p[idx] = r * cu;
			p[idx + 1] = r * su;
			p[idx + 2] = InnerRadius * sv;
			n[idx] = cv * cu * r;
			n[idx + 1] = cv * su * r;
			n[idx + 2] = sv * r;
			tex[tidx] = u / (2 * PI);
			tex[tidx + 1] = v / (2 * PI);
			tidx += 2;
			// Normalize
			float len = sqrt(n[idx] * n[idx] +
				n[idx + 1] * n[idx + 1] +
				n[idx + 2] * n[idx + 2]);
			n[idx] /= len;
			n[idx + 1] /= len;
			n[idx + 2] /= len;
			idx += 3;
		}
	}

	idx = 0;
	for (uint32 ring = 0; ring < nrings; ring++) {
		uint32 ringStart = ring * nsides;
		uint32 nextRingStart = (ring + 1) * nsides;
		for (uint32 side = 0; side < nsides; side++) {
			int nextSide = (side + 1) % nsides;
			// The quad
			el[idx] = (ringStart + side);
			el[idx + 1] = (nextRingStart + side);
			el[idx + 2] = (nextRingStart + nextSide);
			el[idx + 3] = ringStart + side;
			el[idx + 4] = nextRingStart + nextSide;
			el[idx + 5] = (ringStart + nextSide);
			idx += 6;
		}
	}

	InitBuffers(&el, &p, &n, &tex);
}
