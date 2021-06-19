#ifndef SK_SKELETAL_MODEL_H
#define SK_SKELETAL_MODEL_H

#include "Skeleton.h"
#include "Bone.h"
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>

#define MAX_BONE_WEIGHTS 4

class SK_Model
{
public:

	static glm::mat4 AiToGLMMat4(aiMatrix4x4& in_mat)
	{
		glm::mat4 tmp;
		tmp[0][0] = in_mat.a1;
		tmp[1][0] = in_mat.a2;
		tmp[2][0] = in_mat.a3;
		tmp[3][0] = in_mat.a4;

		tmp[0][1] = in_mat.b1;
		tmp[1][1] = in_mat.b2;
		tmp[2][1] = in_mat.b3;
		tmp[3][1] = in_mat.b4;

		tmp[0][2] = in_mat.c1;
		tmp[1][2] = in_mat.c2;
		tmp[2][2] = in_mat.c3;
		tmp[3][2] = in_mat.c4;

		tmp[0][3] = in_mat.d1;
		tmp[1][3] = in_mat.d2;
		tmp[2][3] = in_mat.d3;
		tmp[3][3] = in_mat.d4;
		
		return tmp;
	}

	/*  Model Data */
	std::vector<SkeletalMesh> meshes;

	std::string directory;
	std::string mesh_dir;
	BBox bbox;
	int id;
	unsigned int num_anims = 0;

	SK_Model(){}

	SK_Model(std::string const &path);
	// draws the model, and thus all its meshes
	void Draw();
	// Load model from mesh_dir path.
	void loadModel_fromDir();
	SkeletalMesh* GetFirstMesh();
	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	bool loadModel(std::string const &path);
	void clear();

	std::map<std::string, BoneInfo> m_BoneInfoMap; 
	int m_BoneCounter = 0;

	void SetVertexBoneDataToDefault(SK_Vertex& vertex);
	void SetVertexBoneData(SK_Vertex& vertex, int boneID, float weight);
	void ExtractBoneWeightForVertices(std::vector<SK_Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
private:


private:
	// Calculate model's bbox (bounding box) using a specefied mesh in the mesh array. (default = 0)
	void CalcBoundingBox(int mesh_index = 0);

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode *node, const aiScene *scene);
	SkeletalMesh processMesh(aiMesh *mesh, const aiScene *scene);
};


#endif // ! SK_SKELETAL_MODEL_H