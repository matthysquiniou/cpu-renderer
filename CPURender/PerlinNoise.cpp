#include "PerlinNoise.h"
#include <random>
#include <ctime>
PerlinNoise::PerlinNoise()
{
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	MakePermutation();
}

Mesh& PerlinNoise::GetProceduralMap()
{ 
	return m_mesh; 
}

void PerlinNoise::Shuffle()
{
	for (size_t i = m_permutation.size() - 1; i > 0;i--)
	{
		int index = std::rand() %( i + 0);
		int iValue = m_permutation[i];

		m_permutation[i] = m_permutation[index];
		m_permutation[index] = iValue;
	}
}

void PerlinNoise::MakePermutation()
{
	m_permutation.clear();
	for (int i = 0; i < 256; i++)
	{
		m_permutation.push_back(i);
	}

	Shuffle();

	for (int i = 0; i < 256; i++)
	{
		m_permutation.push_back(m_permutation[i]);
	}

}


int PerlinNoise::Color(glm::vec3 rgb)
{
	rgb = glm::clamp(rgb, 0.0f, 1.0f);

	int r = (int)(rgb.r * 255.0f);
	int g = (int)(rgb.g * 255.0f);
	int b = (int)(rgb.b * 255.0f);

	return (255 << 24) | (r << 16) | (g << 8) | b;
}

glm::vec2 PerlinNoise::GetConstantVector(int value)
{
	int h = value & 3;

	if (h == 0)
		return glm::vec2(1.0, 1.0);
	else if (h == 1)
		return glm::vec2(-1.0, 1.0);
	else if (h == 2)
		return glm::vec2(-1.0, -1.0);
	else
		return glm::vec2(1.0, -1.0);
}

float PerlinNoise::Fade(float t)
{
	return ((6 * t - 15) * t + 10) * t * t * t;
}

float PerlinNoise::Lerp(float t, float a1, float a2)
{
	return a1 + t * (a2 - a1); 
}



float PerlinNoise::Noise2D(float x, float y)
{

	int valueX = (int)std::floor(x) & 255;
	int valueY = (int)std::floor(y) & 255;

	float marginX = x - (int)std::floor(x);
	float marginY = y - (int)std::floor(y);

	glm::vec2 topRight = { marginX - 1.0, marginY - 1.0 };
	glm::vec2 topLeft = { marginX, marginY - 1.0 };
	glm::vec2 bottomRight = { marginX - 1.0, marginY };
	glm::vec2 bottomLeft = { marginX, marginY };


	int valueTopRight = m_permutation[m_permutation[(valueX + 1) & 255] + valueY + 1];
	int valueTopLeft = m_permutation[m_permutation[valueX] + valueY + 1];
	int valueBottomRight = m_permutation[m_permutation[valueX + 1] + valueY];
	int valueBottomLeft = m_permutation[m_permutation[valueX] + valueY];

	float dotTopRight = glm::dot(topRight, (GetConstantVector(valueTopRight)));
	float dotTopLeft = glm::dot(topLeft, (GetConstantVector(valueTopLeft)));
	float dotBottomRight = glm::dot(bottomRight, (GetConstantVector(valueBottomRight)));
	float dotBottomLeft = glm::dot(bottomLeft, (GetConstantVector(valueBottomLeft)));

	float u = Fade(marginX);
	float v = Fade(marginY);


	return Lerp(v,
		Lerp(u, dotBottomLeft, dotBottomRight),
		Lerp(u, dotTopLeft, dotTopRight)
	);
}

float PerlinNoise::FractalBrownianMotion(float x, float y, int numOctaves)
{
	float result = 0.0f;
	float amplitude = 1.0f;
	float frequency = 0.005f;

	for (int octave = 0; octave < numOctaves; octave++)
	{
		float n = amplitude * Noise2D(x * frequency, y * frequency);
		result += n;

		amplitude *= 0.5f;
		frequency *= 2.0f;
	}

	return result;
}

glm::vec3 PerlinNoise::CreateNormal(int col, int row)
{
	glm::vec3 A = m_heightMap[col][row];
	glm::vec3 B = m_heightMap[col][row + 1];
	glm::vec3 C = m_heightMap[col + 1][row];
	glm::vec3 D = m_heightMap[col + 1][row + 1];

	glm::vec3 AB = B - A;
	glm::vec3 AC = C - A;
	glm::vec3 n1 = glm::normalize(glm::cross(AB, AC));

	glm::vec3 AD = D - A;
	glm::vec3 n2 = glm::normalize(glm::cross(AD, AC));

	glm::vec3 nFinal = glm::normalize(n1 + n2);

	return nFinal;
}
void PerlinNoise::CreatePlaneVertex( int col, int row)
{
	std::vector<glm::vec2> index;

	index.push_back({ col,row });
	index.push_back({ col,row +1 });
	index.push_back({ col + 1,row });
	index.push_back({ col + 1,row + 1});

	glm::vec3 normal = CreateNormal(col,row);

	std::vector<Vertex> vertices;

	for (int i = 0;i < 4;i++)
	{
		Vertex v;

		v.m_coordonate = m_heightMap[static_cast<size_t>(index[i].x)][static_cast<size_t>(index[i].y)];
		v.m_uv.x = index[i].x / (static_cast<float>(m_heightMap.size()) - 1.f);
		v.m_uv.y = index[i].y / (static_cast<float>(m_heightMap[static_cast<size_t>(index[i].x)].size()) - 1.f);
		v.m_normal = normal;

		vertices.push_back(v);
	}
	
	m_mesh.m_vertices.AddVertex(vertices[0]);
	m_mesh.m_vertices.AddVertex(vertices[1]);
	m_mesh.m_vertices.AddVertex(vertices[2]);

	m_mesh.m_vertices.AddVertex(vertices[2]);
	m_mesh.m_vertices.AddVertex(vertices[1]);
	m_mesh.m_vertices.AddVertex(vertices[3]);
	return ;
}
void PerlinNoise::CreatePerlinNoise()
{
	for (int x = 0 ; x < WIDTHMAP ; x++)
	{
		std::vector<glm::vec3> rowvec;

		for (int z = 0 ; z < HEIGHTMAP ; z++)
		{

			float n = FractalBrownianMotion((float)x, (float)z, 4);;

			n += 1.0;
			n /= 2.0;

			float y = 0;
			if (n < 0.5f)
			{
				float factor = n / 0.5f;
				y= glm::mix(static_cast<float>(MINHEIGHT), static_cast<float>(MIDHEIGHT), factor);

				float averageMinMid = (MINHEIGHT + MIDHEIGHT) * 0.5f;
				if (y <= averageMinMid)
					y = averageMinMid - 1.3f;
			}
			else if (n < 0.75)
			{
				float factor = (n - 0.5f) / 0.5f; 
				y = glm::mix(static_cast<float>(MIDHEIGHT), static_cast<float>(HIGHHEIGHT), factor);
			}
			else
			{
				float factor = (n - 0.5f) / 0.5f;
				y = glm::mix(static_cast<float>(HIGHHEIGHT), static_cast<float>(MAXHEIGHT), factor);
			}

			m_pixelToDraw.push_back({ x ,n,z });
			int spacing = 4;
			rowvec.push_back({ spacing *(x - WIDTHMAP /2)  ,y, spacing * (z - HEIGHTMAP /2 )});
		}
		m_heightMap.push_back(rowvec);
	}
}
int PerlinNoise::GetFieldColor(float n)
{
	glm::vec3 color;


	glm::vec3 bleuFonce = glm::vec3(0.0f, 0.1f, 0.4f);
	glm::vec3 vertClair = glm::vec3(0.1f, 0.8f, 0.1f);
	glm::vec3 vertFonce = glm::vec3(0.02f, 0.35f, 0.02f);

	if (n < 0.5f)
	{
		float factor = n / 0.5f;
		color = glm::mix(bleuFonce, vertClair, factor);
	}
	else
	{

		float factor = (n - 0.5f) / 0.5f;
		color = glm::mix(vertClair, vertFonce, factor);
	}

	int c = Color(color);
		return c;
}
void PerlinNoise::MakeProceduralMap()
{
	CreatePerlinNoise();

	for (int i = 0; i < m_heightMap.size();i++)
	{
		if (i == m_heightMap.size() - 1)
			continue;


		for (int j = 0; j < m_heightMap[i].size();j++)
		{
			if (j == m_heightMap[i].size() - 1)
				continue;

			CreatePlaneVertex(i, j);

		}
	}
	m_mesh.UpdateBBox();
}

void PerlinNoise::RenderPerlinNoise(FinalFrameBuffer& frameBuffer)
{
	for (glm::vec3 v : m_pixelToDraw)
	{
		frameBuffer.put_pixel(static_cast<uint32_t>(v.x), static_cast<uint32_t>(v.z), static_cast<uint32_t>(GetFieldColor(v.y)));
	}
}


