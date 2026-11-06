#pragma once
#include "Vertex.h"
#include "ConstantBuffer.h"

class IVertexShader;
class IPixelShader;
class Texture;
class CubeMap;
class SpecularMap;

struct DrawCommand
{
	Vertices& m_vertexBuffer;
	ConstantBuffer& m_constantBuffer;
	IVertexShader* m_vertexShader;
	IPixelShader* m_pixelShader;
	Texture* m_textureAlbedo;
	CubeMap* m_cubeMap;
	Texture* m_textureSpecular;
	bool m_cullClockWise = true;
};