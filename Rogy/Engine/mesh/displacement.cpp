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

	sectorsCount = 8;
	TexRes = mHeightmap->getTexHeight();
	EachSectorRes = mHeightmap->getTexHeight() / sectorsCount;

	sectors.clear();

	{
		size_t indx = 0;
		size_t extants = (sectorsCount / 2);
		if (extants == 0)
			extants = sectorsCount;

		for (size_t x = 0; x < extants; x++)
		{
			for (size_t y = 0; y < extants; y++)
			{
				// Initialize the terrain sector
				sectors.emplace_back();
				TerrainSector& sector = sectors[indx];
				
				sector.Decimation = Decimate; // LOD1
				sector.VertexCount = (unsigned int)((float)EachSectorRes * sector.Decimation); 

				sector.uOffset.x = x;
				sector.uOffset.y = y;
				sector.Offset = sector.uOffset * sector.VertexCount;
				//sector.Offset.x = x * sector.VertexCount;
				//sector.Offset.y = y * sector.VertexCount;
				
				sector.bbox.BoxMin = glm::vec3(99999999.0f);
				sector.bbox.BoxMax = glm::vec3(-99999999.0f);

				sector.VertexCount++;// adding one to fill the gaps.

				SetSectorLODLevel(indx, Decimate);
				indx++;
			/*	// Initialize the sector's mesh data
				unsigned int VERTEX_COUNT = sector.VertexCount;
				unsigned int count = VERTEX_COUNT * VERTEX_COUNT;
				vector<Vertex> vertices;
				vector<unsigned int> indices;
				indices.resize(6 * (VERTEX_COUNT - 1)*(VERTEX_COUNT - 1));

				for (size_t i = 0; i < VERTEX_COUNT; i++)
				{
					for (size_t j = 0; j < VERTEX_COUNT; j++) 
					{
						Vertex vertex;

						size_t I = i + sector.Offset.x;
						size_t J = j + sector.Offset.y;

						vertex.Position.x = (float)J / ((float)VERTEX_COUNT - 1) * (Size);
						vertex.Position.z = (float)I / ((float)VERTEX_COUNT - 1) * (Size);

						float val = (float)sectorsCount / 4.0f;
						vertex.Position.y = GetHeight(J * extants / val / sector.Decimation, I * extants / val / sector.Decimation, mHeightmap);

						sector.bbox.BoxMin.x = glm::min(sector.bbox.BoxMin.x, vertex.Position.x);
						sector.bbox.BoxMin.y = glm::min(sector.bbox.BoxMin.y, vertex.Position.y);
						sector.bbox.BoxMin.z = glm::min(sector.bbox.BoxMin.z, vertex.Position.z);

						sector.bbox.BoxMax.x = glm::max(sector.bbox.BoxMax.x, vertex.Position.x);
						sector.bbox.BoxMax.y = glm::max(sector.bbox.BoxMax.y, vertex.Position.y);
						sector.bbox.BoxMax.z = glm::max(sector.bbox.BoxMax.z, vertex.Position.z);

						vertex.Normal.x = 0;
						vertex.Normal.y = 1;
						vertex.Normal.z = 0;
						vertex.Normal = CalculateNormals(J * extants / val / sector.Decimation, I * extants / val / sector.Decimation, mHeightmap);
						//vertex.Normal = CalculateNormals(J * extants / sector.Decimation, I * extants / sector.Decimation, mHeightmap);

						vertex.TexCoords.x = (float)j / ((float)(VERTEX_COUNT) - 1);
						vertex.TexCoords.y = (float)i / ((float)(VERTEX_COUNT) - 1);

						vertices.push_back(vertex);
					}
				}

				sector.mesh.faces.clear();

				int pointer = 0;
				for (size_t gz = 0; gz < VERTEX_COUNT - 1; gz++) {
					for (size_t gx = 0; gx < VERTEX_COUNT - 1; gx++) {
						int topLeft = (gz*VERTEX_COUNT) + gx;
						int topRight = topLeft + 1;
						int bottomLeft = ((gz + 1)*VERTEX_COUNT) + gx;
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
						sector.mesh.faces.push_back(mface);
						mface.indices.clear();
						mface.indices.push_back(topRight);
						mface.indices.push_back(bottomLeft);
						mface.indices.push_back(bottomRight);
						sector.mesh.faces.push_back(mface);
					}
				}

				sector.mesh.isDisplacement = true;
				sector.mesh.vertices = vertices;
				sector.mesh.indices = indices;
				*/
				// Calculating Normals
				/*for (size_t i = 0; i < sector.mesh.faces.size() - 1; i++)
				{
					Vertex a, b, c;
					a = sector.mesh.vertices[sector.mesh.faces[i].indices[0]];
					b = sector.mesh.vertices[sector.mesh.faces[i].indices[1]];
					c = sector.mesh.vertices[sector.mesh.faces[i].indices[2]];

					glm::vec3 N = glm::triangleNormal(a.Position, b.Position, c.Position);

					sector.mesh.vertices[sector.mesh.faces[i].indices[0]].Normal = N;
					sector.mesh.vertices[sector.mesh.faces[i].indices[1]].Normal = N;
					sector.mesh.vertices[sector.mesh.faces[i].indices[2]].Normal = N;

					// Shortcuts for vertices
					glm::vec3 & v0 = a.Position;
					glm::vec3 & v1 = b.Position;
					glm::vec3 & v2 = c.Position;

					glm::vec2 & uv0 = a.TexCoords;
					glm::vec2 & uv1 = b.TexCoords;
					glm::vec2 & uv2 = c.TexCoords;

					glm::vec3 edge1 = v1 - v0;
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

					sector.mesh.vertices[sector.mesh.faces[i].indices[0]].Tangent = tangent1;
					sector.mesh.vertices[sector.mesh.faces[i].indices[1]].Tangent = tangent1;
					sector.mesh.vertices[sector.mesh.faces[i].indices[2]].Tangent = tangent1;

					sector.mesh.vertices[sector.mesh.faces[i].indices[0]].Bitangent = bitangent1;
					sector.mesh.vertices[sector.mesh.faces[i].indices[1]].Bitangent = bitangent1;
					sector.mesh.vertices[sector.mesh.faces[i].indices[2]].Bitangent = bitangent1;

				}
				for (size_t i = 0; i < sector.mesh.vertices.size() - 1; i++)
				{
					sector.mesh.vertices[i].Normal = glm::normalize(sector.mesh.vertices[i].Normal);
				}*/

				//sector.mesh.setupMesh();
			}
		}
	}

	// Debugging
	for (size_t i = 0; i < sectors.size(); i++)
	{
		std::cout << "sector [" << i << "] (" << sectors[i].Offset.x << "," << sectors[i].Offset.y << ")";
		std::cout << std::endl;
	}
	return;
	
}

void Terrain::Render(glm::vec3 CamPos, Frustum& fr)
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//mesh.Draw();

	for (size_t i = 0; i < sectors.size(); i++)
	{
		
		if (FrustumCulling) {
			if (fr.IsBoxVisible(sectors[i].bbox.BoxMin, sectors[i].bbox.BoxMax))
			{
				sectors[i].visible = true;
				float distance = glm::distance(CamPos, sectors[i].bbox.Center());
				float Desired_decimation = 1.0f;

				if (distance < LOD0_distance)
					Desired_decimation = 1.0f;
				else if (distance < LOD1_distance)
					Desired_decimation = 0.5f;
				else if (distance < LOD2_distance)
					Desired_decimation = 0.25f;
				else// if (distance < LOD3_distance)
					Desired_decimation = 0.125f;
				//else
				//	Desired_decimation = 0.05f;

				//Desired_decimation = Desired_decimation * Decimate;
				//std::cout << "---------index: " << i << "\n";
				Desired_decimation *= Decimate;
				if (sectors[i].Decimation != Desired_decimation)
					SetSectorLODLevel(i, Desired_decimation);
			}
			else
				sectors[i].visible = false;
		}

		if (sectors[i].visible) {
			if (i == Debug) continue;
			sectors[i].mesh.Draw();
		}
	}

	//FrustumCulling = false;
}

void Terrain::UpdateMesh()
{
	Init();
}

void Terrain::SetSectorLODLevel(size_t indx, float decimation)
{
	//std::cout << "---------index: " << indx << "\n";
	//std::cout << "SetSectorLODLevel 1111\n";
	TerrainSector& sector = sectors[indx];
	int sectorSize = sectors.size();

	size_t extants = (sectorsCount / 2);
	sector.Decimation = decimation;
	sector.VertexCount = (unsigned int)((float)EachSectorRes * sector.Decimation);
	sector.Offset = sector.uOffset * sector.VertexCount;

	sector.bbox.BoxMin = glm::vec3(99999999.0f);
	sector.bbox.BoxMax = glm::vec3(-99999999.0f);

	sector.VertexCount += 1;// adding one to connect sectors

	float Z_p_decimation = sector.Decimation;
	float Z_m_decimation = sector.Decimation;
	float X_p_decimation = sector.Decimation;
	float X_m_decimation = sector.Decimation;

	int z_p_index = indx + extants;
	if(z_p_index >= 0 && z_p_index < sectorSize)
		Z_p_decimation = sectors[z_p_index].Decimation;

	int z_m_index = indx - extants;
	if (z_m_index >= 0 && z_m_index < sectorSize)
		Z_m_decimation = sectors[z_m_index].Decimation;

	int x_p_index = indx + 1;
	if (x_p_index >= 0 && x_p_index < sectorSize)
		X_p_decimation = sectors[x_p_index].Decimation;

	int x_m_index = indx - 1;
	if (x_m_index >= 0 && x_m_index < sectorSize)
		X_m_decimation = sectors[x_m_index].Decimation;

	// Initialize the sector's mesh data
	unsigned int VERTEX_COUNT = sector.VertexCount;
	unsigned int count = VERTEX_COUNT * VERTEX_COUNT;
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	indices.resize(6 * (VERTEX_COUNT - 1)*(VERTEX_COUNT - 1));

	bool isMiddleZ1 = false;
	bool isMiddleZ2 = false;
	bool isMiddleX1 = false;
	bool isMiddleX2 = false;
	for (size_t i = 0; i < VERTEX_COUNT; i++)
	{
		//isMiddleX = false;
		for (size_t j = 0; j < VERTEX_COUNT; j++)
		{
			Vertex vertex;

			size_t I = i + sector.Offset.x;
			size_t J = j + sector.Offset.y;

			vertex.Position.x = (float)J / ((float)VERTEX_COUNT - 1) * (Size);
			vertex.Position.z = (float)I / ((float)VERTEX_COUNT - 1) * (Size);

			float val = (float)sectorsCount / 4.0f;
			vertex.Position.y = GetHeight(J * extants / val / sector.Decimation, I * extants / val / sector.Decimation, mHeightmap);
			/*
			if (Debug != 55 && j == VERTEX_COUNT - 1) 
			{
				if (isMiddleX2 && i != 0 && i != VERTEX_COUNT - 1 && sector.Decimation > X_p_decimation)
				{
					float h1 = GetHeight((J) * extants / val / sector.Decimation, (I-1) * extants / val / sector.Decimation, mHeightmap);
					float h2 = GetHeight((J ) * extants / val / sector.Decimation, (I+1) * extants / val / sector.Decimation, mHeightmap);
					vertex.Position.y = glm::mix(h1, h2, 0.5f);
					isMiddleX2 = false;
				}
				else isMiddleX2 = true;
			}

			if (Debug != 55 && j == 0)
			{
				if (isMiddleX1 && i != 0 && i != VERTEX_COUNT - 1 && sector.Decimation > X_m_decimation)
				{
					float h1 = GetHeight((J)* extants / val / sector.Decimation, (I - 1) * extants / val / sector.Decimation, mHeightmap);
					float h2 = GetHeight((J)* extants / val / sector.Decimation, (I + 1) * extants / val / sector.Decimation, mHeightmap);
					vertex.Position.y = glm::mix(h1, h2, 0.5f);
					isMiddleX1 = false;
				}
				else isMiddleX1 = true;
			}

			if (Debug != 55 && i == 0 && sector.Decimation > Z_p_decimation)
			{
				if (isMiddleZ1)
				{
					float h1 = GetHeight((J-1)* extants / val / sector.Decimation, (I) * extants / val / sector.Decimation, mHeightmap);
					float h2 = GetHeight((J+1)* extants / val / sector.Decimation, (I) * extants / val / sector.Decimation, mHeightmap);
					vertex.Position.y = glm::mix(h1, h2, 0.5f);
					isMiddleZ1 = false;
				}
				else isMiddleZ1 = true;
			}

			if (Debug != 55 && i == VERTEX_COUNT - 1 && sector.Decimation > Z_m_decimation)
			{
				if (isMiddleZ2)
				{
					float h1 = GetHeight((J - 1)* extants / val / sector.Decimation, (I)* extants / val / sector.Decimation, mHeightmap);
					float h2 = GetHeight((J + 1)* extants / val / sector.Decimation, (I)* extants / val / sector.Decimation, mHeightmap);
					vertex.Position.y = glm::mix(h1, h2, 0.5f);
					isMiddleZ2 = false;
				}
				else isMiddleZ2 = true;
			}*/
			/*float Height0 = GetHeight((J-1) * extants / val / sector.Decimation, (I-1) * extants / val / sector.Decimation, mHeightmap);
			float Height1 = GetHeight((J - 1) * extants / val / sector.Decimation, (I) * extants / val / sector.Decimation, mHeightmap);
			float Height2 = GetHeight((J - 1) * extants / val / sector.Decimation, (I + 1)* extants / val / sector.Decimation, mHeightmap);

			float Height3 = GetHeight((J) * extants / val / sector.Decimation, (I - 1) * extants / val / sector.Decimation, mHeightmap);
			float Height4 = GetHeight((J) * extants / val / sector.Decimation, (I)* extants / val / sector.Decimation, mHeightmap);
			float Height5 = GetHeight((J) * extants / val / sector.Decimation, (I + 1)* extants / val / sector.Decimation, mHeightmap);


			float Height6 = GetHeight((J + 1) * extants / val / sector.Decimation, (I - 1) * extants / val / sector.Decimation, mHeightmap);
			float Height7 = GetHeight((J + 1) * extants / val / sector.Decimation, (I)* extants / val / sector.Decimation, mHeightmap);
			float Height8 = GetHeight((J + 1) * extants / val / sector.Decimation, (I + 1)* extants / val / sector.Decimation, mHeightmap);

			float smoothedHeight = Height0 + Height1 + Height2 + Height3 + Height4 + Height5 + Height6 + Height7 + Height8;
			smoothedHeight /= 8;
			vertex.Position.y = smoothedHeight;*/

			sector.bbox.BoxMin.x = glm::min(sector.bbox.BoxMin.x, vertex.Position.x);
			sector.bbox.BoxMin.y = glm::min(sector.bbox.BoxMin.y, vertex.Position.y);
			sector.bbox.BoxMin.z = glm::min(sector.bbox.BoxMin.z, vertex.Position.z);

			sector.bbox.BoxMax.x = glm::max(sector.bbox.BoxMax.x, vertex.Position.x);
			sector.bbox.BoxMax.y = glm::max(sector.bbox.BoxMax.y, vertex.Position.y);
			sector.bbox.BoxMax.z = glm::max(sector.bbox.BoxMax.z, vertex.Position.z);

			vertex.Normal.x = 0;
			vertex.Normal.y = 1;
			vertex.Normal.z = 0;
			vertex.Normal = CalculateNormals(J * extants / val / sector.Decimation, I * extants / val / sector.Decimation, mHeightmap);
			if (glm::abs(vertex.Normal.y) < 0.7)
				vertex.Blend = 1;
			//vertex.TexCoords.x = J / ((float)VERTEX_COUNT - 1)* extants / val / sector.Decimation;
			//vertex.TexCoords.y = I / ((float)VERTEX_COUNT - 1)* extants / val / sector.Decimation;
			vertex.TexCoords.x = (float)j / ((float)VERTEX_COUNT - 1);
			vertex.TexCoords.y = (float)i / ((float)VERTEX_COUNT - 1);
			//vertex.TexCoords.x /= (float)sectorsCount;
			//vertex.TexCoords.y /= (float)sectorsCount;
			vertices.push_back(vertex);
		}
		//isMiddleZ1 = !isMiddleZ1;
	}
	//std::cout << "SetSectorLODLevel 2222\n";
	sector.mesh.faces.clear();

	int pointer = 0;
	for (size_t gz = 0; gz < VERTEX_COUNT - 1; gz++) {
		for (size_t gx = 0; gx < VERTEX_COUNT - 1; gx++) {
			int topLeft = (gz*VERTEX_COUNT) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1)*VERTEX_COUNT) + gx;
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
			sector.mesh.faces.push_back(mface);
			mface.indices.clear();
			mface.indices.push_back(topRight);
			mface.indices.push_back(bottomLeft);
			mface.indices.push_back(bottomRight);
			sector.mesh.faces.push_back(mface);
		}
	}
	//std::cout << "SetSectorLODLevel 3333\n";
	sector.mesh.isDisplacement = true;
	sector.mesh.vertices = vertices;
	sector.mesh.indices = indices;
	//std::cout << "SetSectorLODLevel 4444\n";
	// Calculating Normals
	/*for (size_t i = 0; i < sector.mesh.faces.size() - 1; i++)
	{
		Vertex a, b, c;
		a = sector.mesh.vertices[sector.mesh.faces[i].indices[0]];
		b = sector.mesh.vertices[sector.mesh.faces[i].indices[1]];
		c = sector.mesh.vertices[sector.mesh.faces[i].indices[2]];

		glm::vec3 N = glm::triangleNormal(a.Position, b.Position, c.Position);

		sector.mesh.vertices[sector.mesh.faces[i].indices[0]].Normal = N;
		sector.mesh.vertices[sector.mesh.faces[i].indices[1]].Normal = N;
		sector.mesh.vertices[sector.mesh.faces[i].indices[2]].Normal = N;

		// Shortcuts for vertices
		glm::vec3 & v0 = a.Position;
		glm::vec3 & v1 = b.Position;
		glm::vec3 & v2 = c.Position;

		glm::vec2 & uv0 = a.TexCoords;
		glm::vec2 & uv1 = b.TexCoords;
		glm::vec2 & uv2 = c.TexCoords;

		glm::vec3 edge1 = v1 - v0;
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

		sector.mesh.vertices[sector.mesh.faces[i].indices[0]].Tangent = tangent1;
		sector.mesh.vertices[sector.mesh.faces[i].indices[1]].Tangent = tangent1;
		sector.mesh.vertices[sector.mesh.faces[i].indices[2]].Tangent = tangent1;

		sector.mesh.vertices[sector.mesh.faces[i].indices[0]].Bitangent = bitangent1;
		sector.mesh.vertices[sector.mesh.faces[i].indices[1]].Bitangent = bitangent1;
		sector.mesh.vertices[sector.mesh.faces[i].indices[2]].Bitangent = bitangent1;

	}//	std::cout << "SetSectorLODLevel 5555\n";
	for (size_t i = 0; i < sector.mesh.vertices.size() - 1; i++)
	{
		sector.mesh.vertices[i].Normal = glm::normalize(sector.mesh.vertices[i].Normal);
	}*/
	//std::cout << "SetSectorLODLevel 6666\n";
	sector.mesh.setupMesh();
}

void Terrain::CalculateNormals(Mesh& mMesh)
{
	for (size_t i = 0; i < mMesh.faces.size() - 1; i++)
	{
		Vertex a, b, c;
		a = mMesh.vertices[mMesh.faces[i].indices[0]];
		b = mMesh.vertices[mMesh.faces[i].indices[1]];
		c = mMesh.vertices[mMesh.faces[i].indices[2]];

		glm::vec3 N = glm::triangleNormal(a.Position, b.Position, c.Position);
		
		mMesh.vertices[mMesh.faces[i].indices[0]].Normal = N;
		mMesh.vertices[mMesh.faces[i].indices[1]].Normal = N;
		mMesh.vertices[mMesh.faces[i].indices[2]].Normal = N;

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

		mMesh.vertices[mMesh.faces[i].indices[0]].Tangent = tangent1;
		mMesh.vertices[mMesh.faces[i].indices[1]].Tangent = tangent1;
		mMesh.vertices[mMesh.faces[i].indices[2]].Tangent = tangent1;

		mMesh.vertices[mMesh.faces[i].indices[0]].Bitangent = bitangent1;
		mMesh.vertices[mMesh.faces[i].indices[1]].Bitangent = bitangent1;
		mMesh.vertices[mMesh.faces[i].indices[2]].Bitangent = bitangent1;

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

		mMesh.vertices[mMesh.faces[i].indices[0]].Tangent = tangent;
		mMesh.vertices[mMesh.faces[i].indices[1]].Tangent = tangent;
		mMesh.vertices[mMesh.faces[i].indices[2]].Tangent = tangent;

		mMesh.vertices[mMesh.faces[i].indices[0]].Bitangent = bitangent;
		mMesh.vertices[mMesh.faces[i].indices[1]].Bitangent = bitangent;
		mMesh.vertices[mMesh.faces[i].indices[2]].Bitangent = bitangent;*/
	}
	for (size_t i = 0; i < mMesh.vertices.size() - 1; i++)
	{
		mMesh.vertices[i].Normal = glm::normalize(mMesh.vertices[i].Normal);
	}
}

void Terrain::SetHeightAll(float y)
{
	for (size_t i = 0; i < mesh.vertices.size(); i++)
	{
		mesh.vertices[i].Position.y = y;
		mesh.vertices[i].Blend = 0;
	}
	CalculateNormals(mesh);
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
	CalculateNormals(mesh);
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
	CalculateNormals(mesh);
	mesh.setupMesh();
}



glm::vec3 Terrain::CalculateNormals(int x, int z, Texture * map)
{
	if (x == 0 || z == 0 || x == map->getTexHeight() || z == map->getTexHeight())
		return glm::vec3(0,1,0);
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