#pragma once
#include "Core/Game Components/Model.h"
#include "Core/Vulkan/Texture.h"

static const struct BlockTemplate
{
	Vertex TopFace[4]
	{
		{{-1.00, -1.00,  1.00 }, { 1.00, 1.00, 1.00 }, { 0.00, -1.00,  0.00 }, { 0.875f, 0.500f }, {0} },
		{{ 1.00, -1.00, -1.00 }, { 1.00, 1.00, 1.00 }, { 0.00, -1.00,  0.00 }, { 0.625f, 0.750f }, {0} },
		{{ 1.00, -1.00,  1.00 }, { 1.00, 1.00, 1.00 }, { 0.00, -1.00,  0.00 }, { 0.625f, 0.500f }, {0} },
		{{-1.00, -1.00, -1.00 }, { 1.00, 1.00, 1.00 }, { 0.00, -1.00,  0.00 }, { 0.875f, 0.750f }, {0} }
	};

	Vertex FrontFace[4]
	{
		{{ 1.00, -1.00, -1.00 }, { 1.00, 1.00, 1.00 }, { 0.00,  0.00, -1.00 }, { 0.625f, 0.750f }, {0} },
		{{-1.00,  1.00, -1.00 }, { 1.00, 1.00, 1.00 }, { 0.00,  0.00, -1.00 }, { 0.375f, 1.000f }, {0} },
		{{ 1.00,  1.00, -1.00 }, { 1.00, 1.00, 1.00 }, { 0.00,  0.00, -1.00 }, { 0.375f, 0.750f }, {0} },
		{{-1.00, -1.00, -1.00 }, { 1.00, 1.00, 1.00 }, { 0.00,  0.00, -1.00 }, { 0.625f, 1.000f }, {0} }
	};

	Vertex LeftFace[4]
	{
		{{-1.00, -1.00, -1.00 }, { 1.00, 1.00, 1.00 }, {-1.00,  0.00,  0.00 }, { 0.625f, 0.000f }, {0} },
		{{-1.00,  1.00,  1.00 }, { 1.00, 1.00, 1.00 }, {-1.00,  0.00,  0.00 }, { 0.375f, 0.250f }, {0} },
		{{-1.00,  1.00, -1.00 }, { 1.00, 1.00, 1.00 }, {-1.00,  0.00,  0.00 }, { 0.375f, 0.000f }, {0} },
		{{-1.00, -1.00,  1.00 }, { 1.00, 1.00, 1.00 }, {-1.00,  0.00,  0.00 }, { 0.625f, 0.250f }, {0} }
	};

	Vertex BottomFace[4]
	{
		{{ 1.00,  1.00,  1.00 }, { 1.00, 1.00, 1.00 }, { 0.00,  1.00,  0.00 }, { 0.375f, 0.500f }, {0} },
		{{-1.00,  1.00, -1.00 }, { 1.00, 1.00, 1.00 }, { 0.00,  1.00,  0.00 }, { 0.125f, 0.750f }, {0} },
		{{-1.00,  1.00,  1.00 }, { 1.00, 1.00, 1.00 }, { 0.00,  1.00,  0.00 }, { 0.125f, 0.500f }, {0} },
		{{ 1.00,  1.00, -1.00 }, { 1.00, 1.00, 1.00 }, { 0.00,  1.00,  0.00 }, { 0.375f, 0.750f }, {0} }
	};

	Vertex RightFace[4]
	{
		{{ 1.00, -1.00,  1.00 }, { 1.00, 1.00, 1.00 }, { 1.00,  0.00,  0.00 }, { 0.625f, 0.500f }, {0} },
		{{ 1.00,  1.00, -1.00 }, { 1.00, 1.00, 1.00 }, { 1.00,  0.00,  0.00 }, { 0.375f, 0.750f }, {0} },
		{{ 1.00,  1.00,  1.00 }, { 1.00, 1.00, 1.00 }, { 1.00,  0.00,  0.00 }, { 0.375f, 0.500f }, {0} },
		{{ 1.00, -1.00, -1.00 }, { 1.00, 1.00, 1.00 }, { 1.00,  0.00,  0.00 }, { 0.625f, 0.750f }, {0} }
	};

	Vertex BackFace[4]
	{
		{{-1.00, -1.00,  1.00 }, { 1.00, 1.00, 1.00 }, { 0.00,  0.00,  1.00 }, { 0.625f, 0.250f }, {0} },
		{{ 1.00,  1.00,  1.00 }, { 1.00, 1.00, 1.00 }, { 0.00,  0.00,  1.00 }, { 0.375f, 0.500f }, {0} },
		{{-1.00,  1.00,  1.00 }, { 1.00, 1.00, 1.00 }, { 0.00,  0.00,  1.00 }, { 0.375f, 0.250f }, {0} },
		{{ 1.00, -1.00,  1.00 }, { 1.00, 1.00, 1.00 }, { 0.00,  0.00,  1.00 }, { 0.625f, 0.500f }, {0} }
	};

	uint32_t Indices[6]{ 0, 1, 2,    0, 3, 1 };
};

enum FACE
{
	TOP,
	BOTTOM,
	LEFT,
	RIGHT,
	FRONT,
	BACK
};

struct Face
{
	Texture& Texture;
	std::vector<FACE> UsedByFaces;
};

struct BlockData
{
	std::string name;
	std::vector<Face> faces;
};