#include "Mesh.h"

Mesh::Mesh()
{
	m_transform = std::make_shared<TransformMatrix>();
}

void Mesh::MakePlane(float width, float lenght)
{
	float div2Width = width * 0.5f;
	float div2Lenght = lenght * 0.5f;

	m_vertices.Resize(6);
	
	m_vertices.AddVertex({ {-div2Width, 0, -div2Lenght},{0,1.f,0},{0,0} });
	m_vertices.AddVertex({ {-div2Width, 0, div2Lenght},{0,1.f,0},{0,1.f} });
	m_vertices.AddVertex({ {div2Width, 0, div2Lenght},{0,1.f,0},{1.f,1.f} });

	m_vertices.AddVertex({ {div2Width, 0, div2Lenght},{0,1.f,0},{1.f,1.f} });
	m_vertices.AddVertex({ {div2Width, 0, -div2Lenght},{0,1.f,0},{1.f,0} });
	m_vertices.AddVertex({ {-div2Width, 0, -div2Lenght},{0,1.f,0},{0,0} });

    UpdateBBox();
}

void Mesh::MakeCube(float size)
{
    m_vertices.Clear();
    float half = size * 0.5f;

    AddQuad({ -half, half, half }, { half, half, half }, { half, half,-half }, { -half, half,-half }, { 0, 1, 0 });
    AddQuad({ half,-half, half }, { half,-half,-half }, { half, half,-half }, { half, half, half }, { 1, 0, 0 });
    AddQuad({ -half,-half,-half }, { -half,-half, half }, { -half, half, half }, { -half, half,-half }, { -1, 0, 0 });
    AddQuad({ -half,-half,-half }, { half,-half,-half }, { half,-half, half }, { -half,-half, half }, { 0,-1, 0 });
    AddQuad({ -half,-half, half }, { half,-half, half }, { half, half, half }, { -half, half, half }, { 0, 0, 1 });
    AddQuad({ half,-half,-half }, { -half,-half,-half }, { -half, half,-half }, { half, half,-half }, { 0, 0,-1 });

    UpdateBBox();
}

void Mesh::MakeInvertedCube(float size)
{

    m_vertices.Clear();
    float half = size * 0.5f;

    AddQuad({ -half, half,-half }, { half, half,-half }, { half, half, half }, { -half, half, half }, { 0,-1, 0 });
    AddQuad({ half, half, half }, { half, half,-half }, { half,-half,-half }, { half,-half, half }, { -1, 0, 0 });
    AddQuad({ -half, half,-half }, { -half, half, half }, { -half,-half, half }, { -half,-half,-half }, { 1, 0, 0 });
    AddQuad({ -half,-half, half }, { half,-half, half }, { half,-half,-half }, { -half,-half,-half }, { 0, 1, 0 });
    AddQuad({ -half, half, half }, { half, half, half }, { half,-half, half }, { -half,-half, half }, { 0, 0,-1 });
    AddQuad({ half, half,-half }, { -half, half,-half }, { -half,-half,-half }, { half,-half,-half }, { 0, 0, 1 });

    UpdateBBox();
    
}

void Mesh::MakeParticleMesh(uint32_t numberParticle)
{
    m_vertices.Reserve(6);

    m_vertices.AddVertex({ {0, 0, 0},{0,1.f,0},{0,1.f} });
    m_vertices.AddVertex({ {0, 0, 0},{0,1.f,0},{0,0} });
    m_vertices.AddVertex({ {0, 0, 0},{0,1.f,0},{1.f,1.f} });

    m_vertices.AddVertex({ {0, 0, 0},{0,1.f,0},{1.f,1.f} });
    m_vertices.AddVertex({ {0, 0, 0},{0,1.f,0},{0,0} });
    m_vertices.AddVertex({ {0, 0, 0}, {0,1.f,0},{1.f,0} });

    m_vertices.DuplicateCurrentNTime(numberParticle);
}

TransformMatrix& Mesh::GetTransform()
{
    return *m_transform.get();
}

void Mesh::UpdateBBox()
{
    m_BBoxMin = { FLT_MAX,FLT_MAX,FLT_MAX };
    m_BBoxMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    for (size_t i = 0; i < m_vertices.m_positions.size(); i++)
    {
        glm::vec3& pos = m_vertices.m_positions[i];
        m_BBoxMin = glm::min(m_BBoxMin, pos);
        m_BBoxMax = glm::max(m_BBoxMax, pos);
    }

    for (int c = 0; c < 8; ++c)
    {
        m_BBoxCorners[c] = glm::vec3(
            (c & 1) ? m_BBoxMax.x : m_BBoxMin.x,
            (c & 2) ? m_BBoxMax.y : m_BBoxMin.y,
            (c & 4) ? m_BBoxMax.z : m_BBoxMin.z
        );
    }
}

void Mesh::AddTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 n)
{
    m_vertices.AddVertex({ a, n, {0.f, 0.f} });
    m_vertices.AddVertex({ b, n, {1.f, 0.f} });
    m_vertices.AddVertex({ c, n, {1.f, 1.f} });
}

void Mesh::AddQuad(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec3 n)
{
    AddTriangle(a, b, c, n);
    m_vertices.AddVertex({ a, n, {0.f, 0.f} });
    m_vertices.AddVertex({ c, n, {1.f, 1.f} });
    m_vertices.AddVertex({ d, n, {0.f, 1.f} });
}


