#pragma once
#include "Vertex.h"
#include "ConstantBuffer.h"

class IVertexShader
{
public:
	virtual ~IVertexShader() = default;
	virtual VertexOutput ComputeVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv, ConstantBuffer& constantBuffer, uint32_t vertexIndex) = 0;
};