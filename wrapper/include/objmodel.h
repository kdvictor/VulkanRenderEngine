#pragma once
#include "vertexdata.h"

class ObjModel
{
public:
	VertexData* LoadObjModel(const char* const& pFilePath, unsigned int** pIndexes, unsigned int& vetexCount, unsigned int& indexCount);

public:
	VertexData* mVertexes;
	unsigned int* mIndices;
	unsigned int mIndexCount;
};
