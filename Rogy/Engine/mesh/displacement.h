#ifndef DISPLACEMENT_H
#define DISPLACEMENT_H

#include "../scene/RComponent.h"
#include "mesh.h"
#include "../shading/material.h"
#include "..\shading\texture.h"
#include "../camera/frustum.h"

struct TerrainSector
{
	Mesh mesh;
	glm::mat4 model;
	glm::uvec3 Offset;
	glm::uvec3 uOffset;
	BBox bbox;
	float Decimation; // Like LOD level but 1 = LOD0, 0.5 = LOD1 and so on. 
	unsigned int VertexCount;
	bool visible;

	TerrainSector() {}
};

class Terrain : public Component
{
	BASE_COMPONENT()
public:
	Terrain();
	~Terrain();

	std::vector<TerrainSector> sectors;

	glm::mat4 model;
	glm::vec3 pos;
	float Size = 50.0f;
	float MaxHeight = 100.0f;
	float Decimate = 1.0f;
	bool FrustumCulling = true;
	int Debug = -1;

	// LOD levels
	/*float LOD0_distance = 100.0f;
	float LOD1_distance = 200.0f;
	float LOD2_distance = 300.0f;
	float LOD3_distance = 600.0f;*/

	float LOD0_distance = 200.0f;
	float LOD1_distance = 500.0f;
	float LOD2_distance = 700.0f;
	float LOD3_distance = 1000.0f;

	// Editing
	float Raduis = 2.0f;
	float Intensity = 1.0f;
	float SameHeight = 0.0f;
	bool useOneHeight = false;
	bool edit_alpha = false;

	void Init(Texture* heightmap = nullptr);
	void Render(glm::vec3 CamPos, Frustum& fr);
	void UpdateMesh();
	void SetSectorLODLevel(size_t indx, float decimation);

	void SetHeightAll(float y);
	void SetHeight(glm::vec3 pos, float rad, float amount);
	void SetAlpha(glm::vec3 pos, float rad, float amount);
	void CalculateNormals(Mesh& mMesh);

	Texture* mHeightmap;

	Mesh mesh;
	Material* mat0 = nullptr;

private:
	//float SIZE = 50.0f;
	//int VERTEX_COUNT = 65;
	bool Updated = false;
	float GetHeight(int x, int y, Texture* map);
	glm::vec3 CalculateNormals(int x, int z, Texture* map);
	unsigned int sectorsCount;
	unsigned int TexRes;
	unsigned int EachSectorRes;
};


#endif