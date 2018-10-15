#pragma once

#include <DirectXMath.h>
#include <wrl.h>

class Geometry
{
public:

	enum GeomType { Cube, Sphere };

	Geometry();
	~Geometry();

	// Vertex data for a colored cube.
	struct VertexPosColorNorm
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Color;
		DirectX::XMFLOAT3 Normal;
	};

	int vertexCount;
	int indexCount;

	virtual VertexPosColorNorm* GetVertices();
	virtual WORD* GetIndicies();

private:

};

class Cube: public Geometry
{
public:
	Cube(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 size);

	virtual VertexPosColorNorm* GetVertices() override;
	virtual WORD* GetIndicies() override;

private:
	//Define Vertex Position & Color Data for the Cube.
	VertexPosColorNorm vertices[24];

	//Define Indicies for Triangles of Cube
	WORD indicies[36] =
	{
		2,   5,  8,  2,  8, 11,
		14, 20, 17, 14, 23, 20,
		12, 15,  3, 12,  3,  0,
		9,  6,  18,  9, 18, 21,
		4, 16,  19,  4, 19,  7,
		13, 1,  10, 13, 10, 22
	};
};

class SceneEnvironment : public Geometry
{
public:
	SceneEnvironment(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 size);

	virtual VertexPosColorNorm* GetVertices() override;
	virtual WORD* GetIndicies() override;

private:
	//Define Vertex Position & Color Data for the Environment.
	VertexPosColorNorm vertices[11];

	//Define Indicies for Triangles of Environment
	WORD indicies[15] =
	{
		0,   1,  2,  0,  2, 3,
		4,   6,  5,  4,  7, 6,
		8,  10,  9
	};
};

