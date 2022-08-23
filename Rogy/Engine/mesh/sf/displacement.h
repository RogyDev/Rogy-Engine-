#ifndef DISPLACEMENT_H
#define DISPLACEMENT_H

#include "../scene/RComponent.h"
#include "mesh.h"
#include "../shading/material.h"
#include "..\shading\texture.h"

class Terrain : public Component
{
	BASE_COMPONENT()
public:
	Terrain();
	~Terrain();

	glm::mat4 model;
	glm::vec3 pos;
	float Size = 50.0f;
	float MaxHeight = 10.0f;
	float Decimate = 0.5f;
	unsigned int VertexCount = 256;
	// Editing
	float Raduis = 2.0f;
	float Intensity = 1.0f;
	float SameHeight = 0.0f;
	bool useOneHeight = false;
	bool edit_alpha = false;

	void Init(Texture* heightmap = nullptr);
	void Render();
	void UpdateMesh();
	
	void SetHeightAll(float y);
	void SetHeight(glm::vec3 pos, float rad, float amount);
	void SetAlpha(glm::vec3 pos, float rad, float amount);
	void CalculateNormals();

	Texture* mHeightmap;

	Mesh mesh;
	Material* mat0 = nullptr;

private:
	//float SIZE = 50.0f;
	//int VERTEX_COUNT = 65;
	bool Updated = false;
	float GetHeight(int x, int y, Texture* map);
	glm::vec3 CalculateNormals(int x, int z, Texture* map);
};


#endif