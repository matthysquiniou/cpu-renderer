#pragma once
#include <glm/vec4.hpp>
#include <memory>
#include <tuple>
#include "Vertex.h"
#include "Texture.h"
#include "ConstantBuffer.h"
#include "IVertexShader.h"
#include "IPixelShader.h"
#include "Rasterizer.h"
#include "main.h"
#include "CubeMap.h"
#include "DrawCommand.h"

class RenderAPI
{
public:
	RenderAPI();

	uint32_t CreateTexture(std::string path);
	uint32_t CreateCubeMap(std::string paths[6]); // order is top, bottom, right, left, front, back
	uint32_t CreateSpecularTexture(std::string path);
	uint32_t CreateVertexBuffer();
	uint32_t CreateConstantBuffer();
	template <typename VertexShaderType>
	uint32_t CreateVertexShader();
	template <typename PixelShaderType>
	uint32_t CreatePixelShader();
	
	uint32_t GetTextureIndex(uint32_t textureHandle);
	void UploadTextureData(uint32_t handle, std::vector<uint32_t>& textureData);
	void UploadTextureSpecular(uint32_t handle, float shininess, float specularStrength);
	void UploadVertexBuffer(uint32_t handle, std::vector<glm::vec3>& coordinates, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& uvs);
	void UploadConstantBuffer(uint32_t handle, ConstantBuffer& constantBuffer);

	void AddVertexBufferToDraw( // texture and cubeMap are optional but should be handled in VS PS
		uint32_t vertexBufferHandle,
		uint32_t constantBufferHandle,
		uint32_t vertexShaderHandle,
		uint32_t pixelShaderHandle,
		uint32_t textureHandle = -1,
		uint32_t cubeMapHandle = -1,
		uint32_t texturespecularHandle = -1,
		bool cullClockWise = false
	);

	void FlushDraw(FinalFrameBuffer& frameBuffer);

	static uint32_t SampleTexture(Texture& texture, glm::vec2 uv);
	static uint32_t SampleTexture(uint32_t textureIndex, glm::vec2 uv);
	static uint32_t SampleSpecularMap(uint32_t specularIndex, glm::vec2 uv);
	static glm::vec2 GetTextureSpecularInfo(uint32_t textureIndex); // x = shininess / y = strength

	static uint32_t SampleCubeMap(CubeMap& cubeMap, glm::vec3 direction);
private:
	void VertexShaderPass();
	void BackfaceCullingPass();
	void ClippingPass();
	void RasterizingPass(FinalFrameBuffer& frameBuffer);

	void ClipTriangle(uint32_t index, glm::vec4& equation, VerticesOutput& input, VerticesOutput& output);
	VertexOutput ClipIntersectEdge(VerticesOutput& result, uint32_t indexV0, uint32_t indexV1, float value0, float value1);

private:
	std::vector<Vertices> m_vertexBuffer;
	static std::vector<Texture> s_textures;
	std::vector<CubeMap> m_cubeMap;
	std::vector<ConstantBuffer> m_constantBuffer;
	std::vector<std::unique_ptr<IVertexShader>> m_vertexShaders;
	std::vector<std::unique_ptr<IPixelShader>> m_pixelShaders;

	std::vector<uint32_t> m_drawCommandIndexByTriangle;
	VerticesOutput m_vertexShaderOut;
	std::vector<float> m_depthBuffer;
	std::vector<RasterTriangle> m_clippedTriangles;
	std::vector<RasterTriangle> m_backfaceTriangles;
	// some other outs ?

	std::unordered_map<uint32_t, uint32_t> m_mapVBHandleToIndex; // replace map with sparseSet latter if bottleneck here 
	std::unordered_map<uint32_t, uint32_t> m_mapTextureHandleToIndex;
	std::unordered_map<uint32_t, uint32_t> m_mapCubeMapHandleToIndex;
	std::unordered_map<uint32_t, uint32_t> m_mapCBHandleToIndex;
	std::unordered_map<uint32_t, uint32_t> m_mapVSHandleToIndex;
	std::unordered_map<uint32_t, uint32_t> m_mapPSHandleToIndex;

	std::vector<DrawCommand> m_drawCommands; 

	uint32_t m_nextHandleId = 0;

	Rasterizer m_rasterizer;
	CubeMap m_skybox;
	bool m_skyboxSet = false;
};

#include "RenderAPI.inl"

