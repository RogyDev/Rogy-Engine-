#ifndef MODEL_H
#define MODEL_H

#include <GL\glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "skeletalMesh.h"
#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

//#include <theklaAtlas\thekla\thekla_atlas.h>

static aiMatrix4x4 GLMMat4ToAi(glm::mat4 mat)
{
	return aiMatrix4x4(mat[0][0], mat[0][1], mat[0][2], mat[0][3],
		mat[1][0], mat[1][1], mat[1][2], mat[1][3],
		mat[2][0], mat[2][1], mat[2][2], mat[2][3],
		mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
}

static glm::mat4 AiToGLMMat4(aiMatrix4x4& in_mat)
{
	glm::mat4 tmp;
	tmp[0][0] = in_mat.a1;
	tmp[1][0] = in_mat.b1;
	tmp[2][0] = in_mat.c1;
	tmp[3][0] = in_mat.d1;

	tmp[0][1] = in_mat.a2;
	tmp[1][1] = in_mat.b2;
	tmp[2][1] = in_mat.c2;
	tmp[3][1] = in_mat.d2;

	tmp[0][2] = in_mat.a3;
	tmp[1][2] = in_mat.b3;
	tmp[2][2] = in_mat.c3;
	tmp[3][2] = in_mat.d3;

	tmp[0][3] = in_mat.a4;
	tmp[1][3] = in_mat.b4;
	tmp[2][3] = in_mat.c4;
	tmp[3][3] = in_mat.d4;
	return tmp;
}

static unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture0 failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

struct RModelNode
{
	std::string name;
	int idx;
	std::vector<RModelNode*> children;

	RModelNode()
	{
		name = "";
		idx = -1;
	}

	RModelNode(std::string _name, int _idx)
	{
		name = _name;
		idx = _idx;
	}
};


class Model
{
public:
	/*  Model Data */
	vector<Texture0> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<Mesh> meshes;
	SkeletalMesh sk_mesh;

	string directory;
	string mesh_dir;
	BBox bbox;
	RModelNode* model_scene;

	bool gammaCorrection;
	int id;

	/*  Functions   */
	// constructor, expects a filepath to a 3D model.
	Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
	{
		loadModel(path);
	}

	// draws the model, and thus all its meshes
	void Draw()
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw();
	}

	// Load model from mesh_dir path.
	void loadModel_fromDir()
	{
		loadModel(mesh_dir);
	}

	Mesh* GetFirstMesh()
	{
		if (meshes.empty())
			return nullptr;
		std::cout << "GETTING FIRST MESH";
		return &meshes[0];
	}
	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string const &path)
	{
		if (path == "")
			return;

		mesh_dir = path;
		
		// read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}

		// retrieve the directory path of the filepath
		directory = path.substr(0, path.find_last_of('/'));

		// process ASSIMP's root node recursively
		processNode(scene->mRootNode, scene);
		//processNodeAndScene(scene->mRootNode, scene, model_scene);

	/*	using namespace Thekla;

		Mesh* obj_mesh = &meshes[0];
		float* flat_array1 = &obj_mesh->vertices[0].Position.x;

		Atlas_Input_Mesh input_mesh;
		input_mesh.vertex_count = obj_mesh->vertices.size();
		input_mesh.vertex_array = (Atlas_Input_Vertex *)flat_array1;
		input_mesh.face_count = obj_mesh->;
		input_mesh.face_array = (Atlas_Input_Face *)obj_mesh->face_array;

		// Generate Atlas_Output_Mesh.
		Atlas_Options atlas_options;
		atlas_set_default_options(&atlas_options);

		// Avoid brute force packing, since it can be unusably slow in some situations.
		atlas_options.packer_options.witness.packing_quality = 1;

		Atlas_Error error = Atlas_Error_Success;
		Atlas_Output_Mesh * output_mesh = atlas_generate(&input_mesh, &atlas_options, &error);*/

		CalcBoundingBox(0);
	}

	void clear()
	{
		for (size_t i = 0; i < meshes.size(); i++)
		{
			meshes[i].clear();
		}
	}

private:
	/*  Functions   */

	// Calculate model's bbox (bounding box) using a specefied mesh in the mesh array. (default = 0)
	void CalcBoundingBox(int mesh_index = 0)
	{
		if (meshes.size() <= 0)
			return;

		for (size_t j = 0; j < meshes.size(); j++)
		{
			meshes[j].path = mesh_dir;
			meshes[j].index = j;

			if (meshes[j].vertices.size() <= 0)
				continue;
			Mesh* aMesh = &meshes[j];
			// store min/max point in local coordinates for calculating approximate bounding box.
			std::vector<glm::vec3> positions;
			positions.resize(aMesh->vertices.size());
			
			aMesh->bbox.BoxMin = glm::vec3(99999.0);
			aMesh->bbox.BoxMax = glm::vec3(-99999.0);

			for (unsigned int i = 0; i < aMesh->vertices.size(); ++i)
			{
				positions[i] = glm::vec3(aMesh->vertices[i].Position.x, aMesh->vertices[i].Position.y, aMesh->vertices[i].Position.z);

				if (positions[i].x < aMesh->bbox.BoxMin.x) aMesh->bbox.BoxMin.x = positions[i].x;
				if (positions[i].y < aMesh->bbox.BoxMin.y) aMesh->bbox.BoxMin.y = positions[i].y;
				if (positions[i].z < aMesh->bbox.BoxMin.z) aMesh->bbox.BoxMin.z = positions[i].z;

				if (positions[i].x > aMesh->bbox.BoxMax.x) aMesh->bbox.BoxMax.x = positions[i].x;
				if (positions[i].y > aMesh->bbox.BoxMax.y) aMesh->bbox.BoxMax.y = positions[i].y;
				if (positions[i].z > aMesh->bbox.BoxMax.z) aMesh->bbox.BoxMax.z = positions[i].z;
			}
			aMesh->bbox.radius = glm::distance(aMesh->bbox.BoxMin, aMesh->bbox.BoxMax) / 2;
		}
		bbox = meshes[mesh_index].bbox;

		/*if (meshes.size() <= 0)
			return;
		
		if (meshes[mesh_index].vertices.size() <= 0)
			return;

		Mesh* aMesh = &meshes[mesh_index];

		// store min/max point in local coordinates for calculating approximate bounding box.
		std::vector<glm::vec3> positions;
		positions.resize(aMesh->vertices.size());

		bbox.BoxMin = glm::vec3(99999.0);
		bbox.BoxMax = glm::vec3(-99999.0);

		/*for ( Vertex ver : aMesh->vertices)
		{
			// Update MaxCoords
			bbox.BoxMax.x = std::fmax(ver.Position.x, bbox.BoxMax.x);
			bbox.BoxMax.y = std::fmax(ver.Position.y, bbox.BoxMax.y);
			bbox.BoxMax.z = std::fmax(ver.Position.z, bbox.BoxMax.z);

			// Update MinCoords
			bbox.BoxMin.x = std::fmin(ver.Position.x, bbox.BoxMin.x);
			bbox.BoxMin.y = std::fmin(ver.Position.y, bbox.BoxMin.y);
			bbox.BoxMin.z = std::fmin(ver.Position.z, bbox.BoxMin.z);
		}
		
		for (unsigned int i = 0; i < aMesh->vertices.size(); ++i)
		{
			positions[i] = glm::vec3(aMesh->vertices[i].Position.x, aMesh->vertices[i].Position.y, aMesh->vertices[i].Position.z);

			if (positions[i].x < bbox.BoxMin.x) bbox.BoxMin.x = positions[i].x;
			if (positions[i].y < bbox.BoxMin.y) bbox.BoxMin.y = positions[i].y;
			if (positions[i].z < bbox.BoxMin.z) bbox.BoxMin.z = positions[i].z;

			if (positions[i].x > bbox.BoxMax.x) bbox.BoxMax.x = positions[i].x;
			if (positions[i].y > bbox.BoxMax.y) bbox.BoxMax.y = positions[i].y;
			if (positions[i].z > bbox.BoxMax.z) bbox.BoxMax.z = positions[i].z;
		}
		bbox.radius = glm::distance(bbox.BoxMin, bbox.BoxMax) / 2;
		*/
	}

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode *node, const aiScene *scene)
	{

		// process each mesh located at the current node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}

	}
	void processNodeAndScene(aiNode *node, const aiScene *scene, RModelNode* sn)
	{

		// process each mesh located at the current node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		sn->name = node->mName.C_Str();
		sn->idx = node->mMeshes[0];

		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			RModelNode* n = new RModelNode();
			sn->children.push_back(n); 
			processNodeAndScene(node->mChildren[i], scene, n);
		}

	}

	Mesh processMesh(aiMesh *mesh, const aiScene *scene)
	{
		// data to fill
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture0> textures;

		// Walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
							  // positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			// normals
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
			// Texture0 coordinates
			if (mesh->mTextureCoords[0]) // does the mesh contain Texture0 coordinates?
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different Texture0 coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple Texture0 coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
			vertices.push_back(vertex);
		}
		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// process materials
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// we assume a convention for sampler names in the shaders. Each diffuse Texture0 should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other Texture0 as the following list summarizes:
		// diffuse: texture_diffuseN
		// specular: texture_specularN
		// normal: texture_normalN
		/*
		// 1. diffuse maps
		vector<Texture0> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		vector<Texture0> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<Texture0> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<Texture0> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
		*/
		// return a mesh object created from the extracted mesh data
		return Mesh(vertices, indices, textures);
	}

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture0 struct.
	vector<Texture0> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
	{
		vector<Texture0> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			// check if Texture0 was loaded before and if so, continue to next iteration: skip loading a new Texture0
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // a Texture0 with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)
			{   // if Texture0 hasn't been loaded already, load it
				Texture0 Texture0;
				Texture0.id = TextureFromFile(str.C_Str(), this->directory, false);
				Texture0.type = typeName;
				Texture0.path = str.C_Str();
				textures.push_back(Texture0);
				textures_loaded.push_back(Texture0);  // store it as Texture0 loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			}
		}
		return textures;
	}
};

#endif