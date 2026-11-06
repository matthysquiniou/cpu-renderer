#pragma once
#include "main.h"
#include <vector>
#include <tuple>
#include <glm/glm.hpp>
#include <memory>
#include "Vertex.h"
#include "DrawCommand.h"

constexpr uint32_t RASTER_CHUNK_SIZE = 32;
constexpr uint32_t CHUNKS_X = (WIDTH + RASTER_CHUNK_SIZE - 1) / RASTER_CHUNK_SIZE;
constexpr uint32_t CHUNKS_Y = (HEIGHT + RASTER_CHUNK_SIZE - 1) / RASTER_CHUNK_SIZE;

class IPixelShader;
class Texture;
struct ConstantBuffer;

class Rasterizer
{
public:
	Rasterizer();
	void Update(std::vector<RasterTriangle>& triangles);

	void Render(
		std::vector<RasterTriangle>& triangles,
		std::vector<std::unique_ptr<IPixelShader>>& pixelShaders,
		std::vector<Texture>& textures,
		std::vector<ConstantBuffer>& cbs,
		std::vector<DrawCommand>& drawCommands,
		FinalFrameBuffer& colorBuffer,
		std::vector<float>& depthBuffer
	);
private:
	float CalculateArea(glm::vec2 a, glm::vec2 b, glm::vec2 p);

	void RasterizeLine(RasterTriangle& tri, uint32_t y,
		uint32_t xStart, uint32_t xEnd,
		std::vector<float>& depthBuffer,
		FinalFrameBuffer& colorBuffer,
		IPixelShader& ps, ConstantBuffer& cb,
		Texture* texAlbedo, CubeMap* cube,
		Texture* texSpecular
	);
	
private:
	std::vector<std::vector<uint32_t>> m_tileTriangles;
};

