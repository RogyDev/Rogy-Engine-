#include "model.h"

Model::Model(std::string const &path)
{
	loadModel(path);
}

void Model::Draw()
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw();
}

void Model::loadModel_fromDir()
{
	loadModel(mesh_dir);
}

Mesh* Model::GetFirstMesh()
{
	if (meshes.empty())
		return nullptr;
	//std::cout << "GETTING FIRST MESH";
	return &meshes[0];
}

void Model::loadModel(string const &path)
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
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}

	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));

	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);
	//processNodeAndScene(scene->mRootNode, scene, model_scene);

	CalcBoundingBox(0);

	/*Mesh& objMesh = meshes[0];

	xatlas::Atlas *atlas = xatlas::Create();
	xatlas::MeshDecl meshDecl;

	std::vector<glm::vec3> poses;
	std::vector<glm::vec3> norms;
	std::vector<glm::vec2> texCoords;

	if (!objMesh.vertices.empty())
	{
	for (size_t i = 0; i < objMesh.vertices.size(); i++)
	{
	poses.push_back(objMesh.vertices[i].Position);
	norms.push_back(objMesh.vertices[i].Normal);
	texCoords.push_back(objMesh.vertices[i].TexCoords);
	}
	meshDecl.vertexCount = (uint32_t)objMesh.vertices.size() ;
	meshDecl.vertexPositionData = poses.data();
	meshDecl.vertexPositionStride = sizeof(float) * 3;

	meshDecl.vertexNormalData = norms.data();
	meshDecl.vertexNormalStride = sizeof(float) * 3;

	meshDecl.vertexUvData = texCoords.data();
	meshDecl.vertexUvStride = sizeof(float) * 2;
	}
	meshDecl.indexCount = (uint32_t)objMesh.indices.size();
	meshDecl.indexData = objMesh.indices.data();
	meshDecl.indexFormat = xatlas::IndexFormat::UInt32;
	xatlas::AddMesh(atlas, meshDecl);
	xatlas::Generate(atlas);
	std::std::cout << "FIN GEN MESH mesh: " << atlas->meshes[0].vertexCount << std::std::endl;

	for (size_t i = 0; i < atlas->meshes[0].vertexCount; i++)
	{
	objMesh.vertices[i].TexCoords2 = glm::vec2(atlas->meshes[0].vertexArray[i].uv[0],
	atlas->meshes[0].vertexArray[i].uv[1]);
	objMesh.vertices[i].TexCoords2 = objMesh.vertices[i].TexCoords;
	}*/
}

void Model::clear()
{
	for (size_t i = 0; i < meshes.size(); i++)
	{
		meshes[i].clear();
	}
}

void Model::CalcBoundingBox(int mesh_index)
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

void Model::processNode(aiNode *node, const aiScene *scene)
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
void Model::processNodeAndScene(aiNode *node, const aiScene *scene, RModelNode* sn)
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

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
	// data to fill
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture0> textures;
	//std::cout << "CONTAIN UVs " << mesh->GetNumUVChannels() << "\n ";
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
			//std::cout << "CONTAIN UV1\n ";
			glm::vec2 vec;
			// a vertex can contain up to 8 different Texture0 coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple Texture0 coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;

			if (mesh->mTextureCoords[1]) // does the mesh contain Texture1 coordinates?
			{
				//std::cout << "CONTAIN UV2\n ";
				vec.x = mesh->mTextureCoords[1][i].x;
				vec.y = mesh->mTextureCoords[1][i].y;
				vertex.TexCoords2 = vec;
			}
			else
				vertex.TexCoords2 = vertex.TexCoords;
		}
		else {
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			vertex.TexCoords2 = glm::vec2(0.0f, 0.0f);
		}
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
	return Mesh(vertices, indices, textures);
}

vector<Texture0> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
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