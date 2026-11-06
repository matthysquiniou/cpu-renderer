#include "RenderAPI.h"

template<typename VertexShaderType>
inline uint32_t RenderAPI::CreateVertexShader()
{
	static_assert(std::is_base_of_v<IVertexShader, VertexShaderType>, "VertexShaderType must inherit from IVertexShader");
	std::unique_ptr<VertexShaderType> vs = std::make_unique<VertexShaderType>();
	uint32_t handleId = m_nextHandleId++;
	m_mapVSHandleToIndex[handleId] = static_cast<uint32_t>(m_vertexShaders.size());
	m_vertexShaders.push_back(std::move(vs));
	return handleId;
}

template<typename PixelShaderType>
inline uint32_t RenderAPI::CreatePixelShader()
{
	static_assert(std::is_base_of_v<IPixelShader, PixelShaderType>, "PixelShaderType must inherit from IPixelShader");
	std::unique_ptr<PixelShaderType> ps = std::make_unique<PixelShaderType>();
	uint32_t handleId = m_nextHandleId++;
	m_mapPSHandleToIndex[handleId] = static_cast<uint32_t>(m_pixelShaders.size());
	m_pixelShaders.push_back(std::move(ps));
	return handleId;
}