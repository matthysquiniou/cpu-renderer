#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include "Vertex.h"
#include <glm/matrix.hpp>
#include "TransformMatrix.h"



struct Mesh
{
	Vertices m_vertices;
	std::shared_ptr<TransformMatrix> m_transform;

	glm::vec3 m_BBoxMin;
	glm::vec3 m_BBoxMax;
	glm::vec3 m_BBoxCorners[8];

	uint32_t m_textureHandle = -1;
	uint32_t m_vertexBufferHandle; 
	uint32_t m_constantBufferHandle; 
	uint32_t m_vertexShaderHandle;
	uint32_t m_pixelShaderHandle;
	uint32_t m_cubeMapHandle = -1;

	Mesh();

	void MakePlane(float width = 1, float lenght = 1);
	void MakeCube(float size = 1.0f);
	void MakeInvertedCube(float size = 1.0f);

	void MakeParticleMesh(uint32_t numberParticle);

	TransformMatrix& GetTransform();
	void UpdateBBox();

private:
	void AddTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 n);
	void AddQuad(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec3 n);

};