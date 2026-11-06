#include "Vertex.h"

void Vertices::Resize(size_t size)
{
	m_positions.resize(size);
	m_normals.resize(size);
	m_uvs.resize(size);
}

void Vertices::Reserve(size_t size)
{
	m_positions.reserve(size);
	m_normals.reserve(size);
	m_uvs.reserve(size);
}

void Vertices::SetVertices(std::vector<glm::vec3>& coordonates, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& uvs)
{
	if (normals.size() != coordonates.size() || uvs.size() != coordonates.size())
		return; // TODO raise error
	Resize(coordonates.size());
	std::copy(coordonates.begin(), coordonates.end(), m_positions.begin());
	std::copy(normals.begin(), normals.end(), m_normals.begin());
	std::copy(uvs.begin(), uvs.end(), m_uvs.begin());
}

void Vertices::AddVertex(Vertex vertex)
{
	m_positions.push_back(vertex.m_coordonate);
	m_normals.push_back(vertex.m_normal);
	m_uvs.push_back(vertex.m_uv);
}

void Vertices::DuplicateCurrentNTime(uint32_t time)
{
	if (time <= 1)
		return;

	size_t originalSize = m_positions.size();

	Resize(originalSize * time);

	for (size_t i = 1; i < time; ++i)
	{
		std::copy_n(m_positions.begin(), originalSize,m_positions.begin() + i * originalSize);
		std::copy_n(m_normals.begin(), originalSize,m_normals.begin() + i * originalSize);
		std::copy_n(m_uvs.begin(),originalSize,m_uvs.begin() + i * originalSize);
	}
}

void Vertices::Clear()
{
	m_positions.clear();
	m_normals.clear();
	m_uvs.clear();
}

void VerticesOutput::Resize(size_t size)
{
	m_clipPositions.resize(size);
	m_worldPositions.resize(size);
	m_normals.resize(size);
	m_uvs.resize(size);
}

void VerticesOutput::Reserve(size_t size)
{
	m_clipPositions.reserve(size);
	m_worldPositions.reserve(size);
	m_normals.reserve(size);
	m_uvs.reserve(size);
}

void VerticesOutput::Clear()
{
	m_clipPositions.clear();
	m_worldPositions.clear();
	m_normals.clear();
	m_uvs.clear();
}

void VerticesOutput::AddVertexOutput(VertexOutput& vertex)
{
	m_clipPositions.push_back(vertex.m_clipPosition);
	m_worldPositions.push_back(vertex.m_worldPosition);
	m_normals.push_back(vertex.m_normal);
	m_uvs.push_back(vertex.m_uv);
}

void VerticesOutput::AddVertexOutput(VerticesOutput& vertices, uint32_t indexVertice)
{
	m_clipPositions.push_back(vertices.m_clipPositions[indexVertice]);
	m_worldPositions.push_back(vertices.m_worldPositions[indexVertice]);
	m_normals.push_back(vertices.m_normals[indexVertice]);
	m_uvs.push_back(vertices.m_uvs[indexVertice]);
}

void VerticesOutput::AddVertexOutputAt(VerticesOutput& vertices, uint32_t indexVertice, uint32_t index)
{
	if (index == m_clipPositions.size())
		return AddVertexOutput(vertices, indexVertice);
	m_clipPositions[index] = vertices.m_clipPositions[indexVertice];
	m_worldPositions[index] = vertices.m_worldPositions[indexVertice];
	m_normals[index] = vertices.m_normals[indexVertice];
	m_uvs[index] = vertices.m_uvs[indexVertice];
}

void VerticesOutput::AddVertexOutputAt(VertexOutput& vertex, uint32_t index)
{
	if (index == m_clipPositions.size())
		return AddVertexOutput(vertex);
	m_clipPositions[index] = vertex.m_clipPosition;
	m_worldPositions[index] = vertex.m_worldPosition;
	m_normals[index] = vertex.m_normal;
	m_uvs[index] = vertex.m_uv;
}

VertexOutput VerticesOutput::GetVertexAt(uint32_t index)
{
	if (!m_clipPositions.empty() || index > m_clipPositions.size() - 1)
		return VertexOutput();
	return VertexOutput(m_clipPositions[index], m_worldPositions[index], m_normals[index], m_uvs[index]);
}
