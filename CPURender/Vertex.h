#pragma once
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>

struct Vertex
{
	glm::vec3 m_coordonate;
	glm::vec3 m_normal;
	glm::vec2 m_uv;
};

struct Vertices
{
	std::vector<glm::vec3> m_positions;
	std::vector<glm::vec3> m_normals;
	std::vector<glm::vec2> m_uvs;

	void Resize(size_t size);
	void Reserve(size_t size);
	void SetVertices(std::vector<glm::vec3>& coordonates, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& uvs);
	void AddVertex(Vertex vertex);
	void DuplicateCurrentNTime(uint32_t time);
	void Clear();
};

struct VertexOutput
{
	glm::vec4 m_clipPosition;
	glm::vec3 m_worldPosition;
	glm::vec3 m_normal;
	glm::vec2 m_uv;
};

struct VerticesOutput {
	std::vector<glm::vec4> m_clipPositions;
	std::vector<glm::vec3> m_worldPositions;
	std::vector<glm::vec3> m_normals;
	std::vector<glm::vec2> m_uvs;

	void Resize(size_t size);
	void Reserve(size_t size);
	void Clear();
	void AddVertexOutput(VertexOutput& vertex);
	void AddVertexOutput(VerticesOutput& vertices, uint32_t indexVertice);
	void AddVertexOutputAt(VerticesOutput& vertices, uint32_t indexVertice, uint32_t index);
	void AddVertexOutputAt(VertexOutput& vertex, uint32_t index);
	VertexOutput GetVertexAt(uint32_t index);
};

struct RasterVertexData
{
	glm::vec2 m_screenPos;  
	float m_invW;    
	glm::vec3 m_worldPos;
	glm::vec3 m_normal;
	glm::vec2 m_uv;
	uint32_t m_triangleIndex;
};

struct RasterTriangle
{
	RasterVertexData m_v[3];
	uint32_t m_drawCommandIndex;
	float m_invArea2;        
	uint32_t m_xMin, m_xMax;
	uint32_t m_yMin, m_yMax;

	glm::vec2 baryA, baryB, baryC;     
	glm::vec3 wordlPosA, wordlPosB, wordlPosC;
	glm::vec3 normalA, normalB, normalC;
	glm::vec2 uvA, uvB, uvC;
	float iwA, iwB, iwC;
};