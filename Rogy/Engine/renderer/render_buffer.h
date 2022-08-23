#ifndef RENDER_BUFFER_H
#define RENDER_BUFFER_H

#include "render_command.h"

// Bath Renderers togheter
struct DrawGroupe
{
	unsigned int matID; // model id
	unsigned int matID2; // mesh index
	unsigned int matID3; // material id

	std::vector<unsigned int> meshes;
	std::vector<DrawGroupe> DrawGroupesMeshes; // to organize same meshs toghter 

	DrawGroupe(unsigned int mid, unsigned int mid2 = 0, unsigned int mid3 = 0)
	{
		matID = mid;
		matID2 = mid2;
		matID3 = mid3;
	}

	void Add(unsigned int id)
	{
		meshes.push_back(id);
	}

	void AddOrganized(unsigned int id, unsigned int model_id, unsigned int mesh_indx)
	{
		for (size_t i = 0; i < DrawGroupesMeshes.size(); i++)
		{
			if (DrawGroupesMeshes[i].matID == model_id && DrawGroupesMeshes[i].matID2 == mesh_indx)
			{
				DrawGroupesMeshes[i].Add(id);
				return;
			}
		}
		size_t indx = DrawGroupesMeshes.size();
		DrawGroupesMeshes.emplace_back(model_id, mesh_indx, matID);
		DrawGroupesMeshes[indx].Add(id);
	}

	void Clear()
	{
		meshes.clear();

		for (size_t i = 0; i < DrawGroupesMeshes.size(); i++)
		{
			DrawGroupesMeshes[i].meshes.clear();
		}
		//DrawGroupesMeshes.clear();
	}
};

class RenderBuffer
{
public:
	RenderBuffer();
	~RenderBuffer();

	std::vector<DrawGroupe> DrawGroupes;
	std::vector<DrawGroupe> DrawGroupesMeshes;

	std::vector<RenderCommand> OpaqueRenderCommands;
	std::vector<RenderCommand> CutoutRenderCommands;
	std::vector<RenderCommand> AlphaRenderCommands;

	// pushes render state relevant to a single render call to the command buffer.
	void Push(Mesh* mesh, Material* material, glm::mat4 transform, BBox box, bool cast_shadows = true, glm::vec3 pos = glm::vec3(0), bool is_static = false, std::string lmPath = "", unsigned int entID = 0);
	void PushCutout(Mesh* mesh, Material* material, glm::mat4 transform, BBox box, bool cast_shadows = true, glm::vec3 pos = glm::vec3(0), bool is_static = false, std::string lmPath = "", unsigned int entID = 0);

	void PushRenderer(unsigned int eid, unsigned int mat_id);
	void PushMesh(unsigned int eid, unsigned int mesh_model_id, unsigned int mesh_id, unsigned int material_ID);
	void PushMeshByMaterial(unsigned int eid, unsigned int mesh_model_id, unsigned int mesh_id, unsigned int material_ID);
	// clears the command buffer; usually done after issuing all the stored render commands.
	void Clear();

private:

};

#endif