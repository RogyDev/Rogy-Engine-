#include "displacement.h"
#include <glm/gtx/normal.hpp>
//#include "stb/stb_image.h"

IMPL_COMPONENT(Terrain)

Terrain::Terrain()
{
}

Terrain::~Terrain()
{
}


void Terrain::Init(Texture* height_map)
{
	if (height_map == nullptr && mHeightmap == nullptr)
	{
		std::cout << "Terrain warning: no heightmap\n";
		return;
	}

	if (height_map != nullptr)
		mHeightmap = height_map;

	VertexCount = mHeightmap->getTexHeight();
	VertexCount = (unsigned int)((float)VertexCount * Decimate);
	/*int x, y, comps;
	 data = stbi_load("Terrain.png", &x, &y, &comps, 3);
	std::cout << "Terrain::Load : " << heightmap->getTexName() << std::endl;*/

	//heightmap->getTexHeight();
	int count = VertexCount * VertexCount;
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	indices.resize(6 * (VertexCount - 1)*(VertexCount - 1));

	for (size_t i = 0; i < VertexCount; i++)
	{
		for (size_t j = 0; j < VertexCount; j++) {
			Vertex vertex;

			vertex.Position.x = (float)j / ((float)VertexCount - 1) * Size;
			vertex.Position.y = GetHeight(j/ Decimate, i / Decimate, mHeightmap);
			vertex.Position.z = (float)i / ((float)VertexCount - 1) * Size;

			
			vertex.Normal.x = 0;
			vertex.Normal.y = 1;
			vertex.Normal.z = 0;
			//vertex.Normal = CalculateNormals(j, i, mHeightmap);

			vertex.TexCoords.x = (float)j / ((float)VertexCount - 1);
			vertex.TexCoords.y = (float)i / ((float)VertexCount - 1);

			vertices.push_back(vertex);
		}
	}
	
	mesh.faces.clear();

	int pointer = 0;
	for (size_t gz = 0; gz < VertexCount - 1; gz++) {
		for (size_t gx = 0; gx < VertexCount - 1; gx++) {
			int topLeft = (gz*VertexCount) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1)*VertexCount) + gx;
			int bottomRight = bottomLeft + 1;
			indices[pointer++] = topLeft;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = topRight;
			indices[pointer++] = topRight;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = bottomRight;

			aFace mface;
			mface.indices.push_back(topLeft);
			mface.indices.push_back(bottomLeft);
			mface.indices.push_back(topRight);
			mesh.faces.push_back(mface);
			mface.indices.clear();
			mface.indices.push_back(topRight);
			mface.indices.push_back(bottomLeft);
			mface.indices.push_back(bottomRight);
			mesh.faces.push_back(mface);
		}
	}

	mesh.isDisplacement = true;
	mesh.vertices = vertices;
	mesh.indices = indices;

	CalculateNormals();
	mesh.setupMesh();
	
}

void Terrain::CalculateNormals()
{
	for (size_t i = 0; i < mesh.faces.size() - 1; i++)
	{
		Vertex a, b, c;
		a = mesh.vertices[mesh.faces[i].indices[0]];
		b = mesh.vertices[mesh.faces[i].indices[1]];
		c = mesh.vertices[mesh.faces[i].indices[2]];

		glm::vec3 N = glm::triangleNormal(a.Position, b.Position, c.Position);
		
		mesh.vertices[mesh.faces[i].indices[0]].Normal = N;
		mesh.vertices[mesh.faces[i].indices[1]].Normal = N;
		mesh.vertices[mesh.faces[i].indices[2]].Normal = N;

		// Shortcuts for vertices
		glm::vec3 & v0 = a.Position;
		glm::vec3 & v1 = b.Position;
		glm::vec3 & v2 = c.Position;

		glm::vec2 & uv0 = a.TexCoords;
		glm::vec2 & uv1 = b.TexCoords;
		glm::vec2 & uv2 = c.TexCoords;

		glm::vec3 edge1 = v1  - v0;
		glm::vec3 edge2 = v2 - v0;
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		glm::vec3 tangent1;
		glm::vec3 bitangent1;

		tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

		bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

		mesh.vertices[mesh.faces[i].indices[0]].Tangent = tangent1;
		mesh.vertices[mesh.faces[i].indices[1]].Tangent = tangent1;
		mesh.vertices[mesh.faces[i].indices[2]].Tangent = tangent1;

		mesh.vertices[mesh.faces[i].indices[0]].Bitangent = bitangent1;
		mesh.vertices[mesh.faces[i].indices[1]].Bitangent = bitangent1;
		mesh.vertices[mesh.faces[i].indices[2]].Bitangent = bitangent1;

		/*
		// Edges of the triangle : postion delta
		glm::vec3 deltaPos1 = v1 - v0;
		glm::vec3 deltaPos2 = v2 - v0;

		// UV delta
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

		mesh.vertices[mesh.faces[i].indices[0]].Tangent = tangent;
		mesh.vertices[mesh.faces[i].indices[1]].Tangent = tangent;
		mesh.vertices[mesh.faces[i].indices[2]].Tangent = tangent;

		mesh.vertices[mesh.faces[i].indices[0]].Bitangent = bitangent;
		mesh.vertices[mesh.faces[i].indices[1]].Bitangent = bitangent;
		mesh.vertices[mesh.faces[i].indices[2]].Bitangent = bitangent;*/
	}
	for (size_t i = 0; i < mesh.vertices.size() - 1; i++)
	{
		mesh.vertices[i].Normal = glm::normalize(mesh.vertices[i].Normal);
	}
}

void Terrain::SetHeightAll(float y)
{
	for (size_t i = 0; i < mesh.vertices.size(); i++)
	{
		mesh.vertices[i].Position.y = y;
		mesh.vertices[i].Blend = 0;
	}
	CalculateNormals();
	mesh.setupMesh();
}

void Terrain::SetAlpha(glm::vec3 pos, float rad, float amount)
{
	for (size_t i = 0; i < mesh.vertices.size(); i++)
	{
		float dist = glm::distance(mesh.vertices[i].Position, pos);
		if (dist < rad)
		{
			float add = (amount/100.0f) / (dist / rad);
			add /= 10;

			mesh.vertices[i].Blend += add;
			mesh.vertices[i].Blend = glm::clamp(mesh.vertices[i].Blend, 0.0f, 1.0f);
		}
	}
	CalculateNormals();
	mesh.setupMesh();
}

void Terrain::SetHeight(glm::vec3 pos, float rad, float amount)
{
	for (size_t i = 0; i < mesh.vertices.size(); i++)
	{
		float dist = glm::distance(glm::vec2(mesh.vertices[i].Position.x, mesh.vertices[i].Position.z), glm::vec2(pos.x, pos.z));
		if (dist < rad)
		{
			float add = (amount / 100.0f) * (rad/dist);

			/*if (useOneHeight)
			{
				if(mesh.vertices[i].Position.y < SameHeight)
					mesh.vertices[i].Position.y += add;
				else
					mesh.vertices[i].Position.y -= add;
			}
			else*/
			mesh.vertices[i].Position.y += add;
		}
	}
	CalculateNormals();
	mesh.setupMesh();
}

void Terrain::Render()
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	mesh.Draw();
}

void Terrain::UpdateMesh()
{
	Init();
}


glm::vec3 Terrain::CalculateNormals(int x, int z, Texture * map)
{
	float HL = (float)map->GetBlue(x - 1, z);
	float HR = (float)map->GetBlue(x + 1, z);
	float HD = (float)map->GetBlue(x, z - 1);
	float HU = (float)map->GetBlue(x, z + 1);

	glm::vec3 normal = glm::vec3(HL - HR, 2.0f, HD - HU);
	return glm::normalize(normal);
}

float Terrain::GetHeight(int x, int z, Texture * map)
{
	if(x < 0 || x >= map->getTexHeight() || z < 0 || z >= map->getTexHeight())
		return 0.0f;

	int MAX_PIXEL_COLOR = 256;

	float h = map->GetBlue(x, z);
	float d = h;
	
	//h += MAX_PIXEL_COLOR / 2.0f;
	h /= MAX_PIXEL_COLOR;// / 2.0f;
	h *= MaxHeight;
	//std::cout << x << "|" << z << " : " << h << "|" << d << std::endl;

	return h;
}