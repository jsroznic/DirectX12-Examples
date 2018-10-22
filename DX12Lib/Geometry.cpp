#include <Geometry.h>
#include <stdio.h>  

using namespace DirectX;

Geometry::Geometry()
{

}

Geometry::~Geometry()
{
}

Geometry::VertexPosColorNorm* Geometry::GetVertices() {
	return nullptr;
}

WORD* Geometry::GetIndicies() {
	return nullptr;
}

Cube::Cube(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 size) {
	// Initialize Vertices
	// 0
	vertices[0] = { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) };  // 0x
	vertices[1] = { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) };  // 0y
	vertices[2] = { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) };  // 0z
	// 1
	vertices[3] = { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) };   // 1x
	vertices[4] = { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) };    // 1y
	vertices[5] = { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) };   // 1z
	// 2
	vertices[6] = { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) };     // 2x
	vertices[7] = { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) };     // 2y
	vertices[8] = { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) };    // 2z
	// 3
	vertices[9] = { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) };    // 3x
	vertices[10] = { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) };  // 3y
	vertices[11] = { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) };  // 3z
	// 4
	vertices[12] = { XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) }; // 4x
	vertices[13] = { XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) }; // 4y
	vertices[14] = { XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) };  // 4z
	// 5
	vertices[15] = { XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) }; // 5x
	vertices[16] = { XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) };  // 5y
	vertices[17] = { XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) };  // 5z
	// 6
	vertices[18] = { XMFLOAT3(1.0f,  1.0f,  1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) };   // 6x
	vertices[19] = { XMFLOAT3(1.0f,  1.0f,  1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) };   // 6y
	vertices[20] = { XMFLOAT3(1.0f,  1.0f,  1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) };   // 6z
	// 7
	vertices[21] = { XMFLOAT3(1.0f, -1.0f,  1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) };   // 7x
	vertices[22] = { XMFLOAT3(1.0f, -1.0f,  1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) };  // 7y
	vertices[23] = { XMFLOAT3(1.0f, -1.0f,  1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) };   // 7z

	vertexCount = 24;
	indexCount = 36;
}

Geometry::VertexPosColorNorm* Cube::GetVertices() {
	return vertices;
}

WORD* Cube::GetIndicies() {
	return indicies;
}


SceneEnvironment::SceneEnvironment(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 size) {
	// Initialize Vertices - Back
	vertices[0] = { XMFLOAT3(-8.0f, -2.0f, -20.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) };
	vertices[1] = { XMFLOAT3(8.0f, -2.0f, -20.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) };
	vertices[2] = { XMFLOAT3(8.0f, 10.0f, -20.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) };
	vertices[3] = { XMFLOAT3(-8.0f, 10.0f, -20.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) };

	// Floor
	vertices[4] = { XMFLOAT3(-8.0f, -2.0f, -20.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) };
	vertices[5] = { XMFLOAT3(8.0f, -2.0f, -20.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) };
	vertices[6] = { XMFLOAT3(8.0f, -2.0f, -10.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) };
	vertices[7] = { XMFLOAT3(-8.0f, -2.0f, -10.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) };

	// Side
	vertices[8] = { XMFLOAT3(-8.0f, -2.0f, -20.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) };
	vertices[9] = { XMFLOAT3(-8.0f, -2.0f, -10.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) };
	vertices[10] = { XMFLOAT3(-8.0f, 10.0f, -20.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) };
	

	vertexCount = 11;
	indexCount = 15;
}

Geometry::VertexPosColorNorm* SceneEnvironment::GetVertices() {
	return vertices;
}

WORD* SceneEnvironment::GetIndicies() {
	return indicies;
}


Sphere::Sphere(DirectX::XMFLOAT3 position, float scale, DirectX::XMFLOAT4 color) {

	XMFLOAT3 rgbColor = XMFLOAT3(color.x, color.y, color.z);
	size_t verticalSegments = 20;
	if (verticalSegments < 3)
		verticalSegments = 3;

	float radius = scale / 2.0;
	size_t horizontalSegments = verticalSegments * 2;
	vertexCount = 0;
	indexCount = 0;

	// Create rings of vertices at progressively higher latitudes.
	for (size_t i = 0; i <= verticalSegments; i++)
	{

		float latitude = (i * XM_PI / verticalSegments) - XM_PIDIV2;
		float dy, dxz;

		XMScalarSinCos(&dy, &dxz, latitude);

		// Create a single ring of vertices at this latitude.
		for (size_t j = 0; j <= horizontalSegments; j++)
		{

			float longitude = j * XM_2PI / horizontalSegments;
			float dx, dz;

			XMScalarSinCos(&dx, &dz, longitude);

			dx *= dxz;
			dz *= dxz;

			XMFLOAT3 norm = XMFLOAT3(dx, dy, dz);
			XMVECTOR normal = XMVectorSet(dx, dy, dz, 0);

			vertices[vertexCount++] = { XMFLOAT3(norm.x*radius + position.x, norm.y*radius + position.y, norm.z*radius + position.z), rgbColor, norm };
		}
	}

	// Fill the index buffer with triangles joining each pair of latitude rings.
	size_t stride = horizontalSegments + 1;

	for (size_t i = 0; i < verticalSegments; i++)
	{
		for (size_t j = 0; j <= horizontalSegments; j++)
		{
			size_t nextI = i + 1;
			size_t nextJ = (j + 1) % stride;

			indicies[indexCount++] = i * stride + j;
			indicies[indexCount++] = nextI * stride + j;
			indicies[indexCount++] = i * stride + nextJ;

			indicies[indexCount++] = i * stride + nextJ;
			indicies[indexCount++] = nextI * stride + j;
			indicies[indexCount++] = nextI * stride + nextJ;
		}
	}

	char buffer[256];
	sprintf_s(buffer, "******** Indicies: %d\n",indexCount);
	OutputDebugStringA(buffer);
	sprintf_s(buffer, "******** Verts: %d\n", vertexCount);
	OutputDebugStringA(buffer);

}

Geometry::VertexPosColorNorm* Sphere::GetVertices() {
	return vertices;
}

WORD* Sphere::GetIndicies() {
	return indicies;
}


CombinedGeometry::CombinedGeometry() {
	vertexCount = 0;
	indexCount = 0;
}

CombinedGeometry::~CombinedGeometry()
{
	if (vertexCount > 0)
		free(vertices);
	if (indexCount > 0)
		free(indicies);
}

Geometry::VertexPosColorNorm* CombinedGeometry::GetVertices() {
	return vertices;
}

WORD* CombinedGeometry::GetIndicies() {
	return indicies;
}

void CombinedGeometry::AddGeometry(Geometry * geom) {
	// Add Vertices
	if (vertexCount == 0) {
		vertices = (VertexPosColorNorm *)malloc(sizeof(VertexPosColorNorm) * geom->vertexCount);
	}
	else {
		vertices = (VertexPosColorNorm *)realloc(vertices, sizeof(VertexPosColorNorm) * (vertexCount + geom->vertexCount));
	}
	for (int i = 0; i < geom->vertexCount; i++) {
		vertices[i + vertexCount] = geom->GetVertices()[i];
	}
	
	// Add Indices
	if (indexCount == 0) {
		indicies = (WORD *)malloc(sizeof(WORD) * geom->indexCount);
	}
	else {
		indicies = (WORD *)realloc(indicies, sizeof(WORD) * (indexCount + geom->indexCount));
	}
	for (int i = 0; i < geom->indexCount; i++) {
		indicies[i + indexCount] = geom->GetIndicies()[i] + vertexCount;
	}
	vertexCount += geom->vertexCount;
	indexCount += geom->indexCount;
}