#include "SkeletalModel.h"
#include "Animator.h"

SK_Model::SK_Model(std::string const &path)
{
	loadModel(path);
}

void SK_Model::Draw()
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw();
	//meshes[0].Draw();
}

void SK_Model::loadModel_fromDir()
{
	loadModel(mesh_dir);
}

SkeletalMesh* SK_Model::GetFirstMesh()
{
	if (meshes.empty())
		return nullptr;
	//std::cout << "GETTING FIRST Skeletal MESH";
	return &meshes[0];
}

bool SK_Model::loadModel(std::string const &path)
{
	if (path == "")
		return false;
	std::cout << "Loading skeletal mesh : " << path << std::endl;
	mesh_dir = path;

	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return false;
	}

	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));

	if (scene->HasAnimations())
		num_anims = scene->mNumAnimations;

	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);

	CalcBoundingBox(0);
	return true;
}

void SK_Model::clear()
{
	for (size_t i = 0; i < meshes.size(); i++)
	{
		meshes[i].clear();
	}
}

void SK_Model::SetVertexBoneDataToDefault(SK_Vertex & vertex)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
		vertex.m_BoneIDs[i] = -1;
		vertex.m_Weights[i] = 0.0f;
	}
}

void SK_Model::CalcBoundingBox(int mesh_index)
{
	if (meshes.size() <= 0)
		return;

	for (size_t j = 0; j < meshes.size(); j++)
	{
		meshes[j].path = mesh_dir;
		meshes[j].index = j;

		if (meshes[j].vertices.size() <= 0)
			continue;
		SkeletalMesh* aMesh = &meshes[j];
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
}

void SK_Model::processNode(aiNode *node, const aiScene *scene)
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
SkeletalMesh SK_Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
	// data to fill
	std::vector<SK_Vertex> vertices;
	std::vector<unsigned int> indices;
	//std::cout << "CONTAIN UVs " << mesh->GetNumUVChannels() << "\n ";
	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		SK_Vertex vertex;
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		
		SetVertexBoneDataToDefault(vertex);

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
		else vertex.TexCoords = glm::vec2(0.0f, 0.0f);
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

	ExtractBoneWeightForVertices(vertices, mesh, scene);
	
	return SkeletalMesh(vertices, indices);
}

void SK_Model::SetVertexBoneData(SK_Vertex& vertex, int boneID, float weight)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
	{
		if (vertex.m_BoneIDs[i] < 0)
		{
			vertex.m_Weights[i] = weight;
			vertex.m_BoneIDs[i] = boneID;
			break;
		}
	}
}

void SK_Model::ExtractBoneWeightForVertices(std::vector<SK_Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
	for (size_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
	{
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
		{
			BoneInfo newBoneInfo;
			newBoneInfo.id = m_BoneCounter;
			newBoneInfo.offset = AiToGLMMat4(mesh->mBones[boneIndex]->mOffsetMatrix);
			m_BoneInfoMap[boneName] = newBoneInfo;
			boneID = m_BoneCounter;
			m_BoneCounter++;
		}
		else
		{
			boneID = m_BoneInfoMap[boneName].id;
		}
		assert(boneID != -1);
		auto weights = mesh->mBones[boneIndex]->mWeights;
		size_t numWeights = mesh->mBones[boneIndex]->mNumWeights;

		for (size_t weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			size_t vertexId = mesh->mBones[boneIndex]->mWeights[weightIndex].mVertexId;//weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			assert(vertexId <= vertices.size());
			SetVertexBoneData(vertices[vertexId], boneID, weight);
		}
	}
}
