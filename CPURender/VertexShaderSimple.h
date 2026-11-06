#pragma once
#include "IVertexShader.h"

class VertexShaderSimple : public IVertexShader
{
public:
	VertexOutput ComputeVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv, ConstantBuffer& constantBuffer, uint32_t vertexIndex) override;
};