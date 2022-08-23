#include "render_buffer.h"

RenderBuffer::RenderBuffer()
{
}

RenderBuffer::~RenderBuffer()
{
}
void RenderBuffer::PushCutout(Mesh* mesh, Material* material, glm::mat4 transform, BBox box, bool cast_shadows, glm::vec3 pos, bool is_static, std::string lmPath, unsigned int entID)
{
	RenderCommand command;
	command.Mesh = mesh;
	command.Material = material;
	command.Transform = transform;
	command.bbox = box;
	command.cast_shadows = cast_shadows;
	command.position = pos;
	command.is_static = is_static;
	command.lightmapPath = lmPath;
	command.ID = entID;

	CutoutRenderCommands.push_back(command);
}

void RenderBuffer::PushRenderer(unsigned int eid, unsigned int mat_id)
{
	for (size_t i = 0; i < DrawGroupes.size(); i++)
	{
		if (DrawGroupes[i].matID == mat_id)
		{
			DrawGroupes[i].Add(eid);
			return;
		}
	}

	size_t indx = DrawGroupes.size();
	DrawGroupes.emplace_back(mat_id);
	DrawGroupes[indx].Add(eid);
}

void RenderBuffer::PushMesh(unsigned int eid, unsigned int mesh_model_id, unsigned int mesh_id, unsigned int material_ID)
{
	for (size_t i = 0; i < DrawGroupesMeshes.size(); i++)
	{
		if (DrawGroupesMeshes[i].matID == mesh_model_id && DrawGroupesMeshes[i].matID2 == mesh_id)
		{
			DrawGroupesMeshes[i].Add(eid);
			return;
		}
	}

	size_t indx = DrawGroupesMeshes.size();
	DrawGroupesMeshes.emplace_back(mesh_model_id, mesh_id, material_ID);
	DrawGroupesMeshes[indx].Add(eid);
}

void RenderBuffer::PushMeshByMaterial(unsigned int eid, unsigned int mesh_model_id, unsigned int mesh_id, unsigned int material_ID)
{
	for (size_t i = 0; i < DrawGroupes.size(); i++)
	{
		if (DrawGroupes[i].matID == material_ID)
		{
			DrawGroupes[i].AddOrganized(eid, mesh_model_id, mesh_id);
			return;
		}
	}

	size_t indx = DrawGroupes.size();
	DrawGroupes.emplace_back(material_ID);
	DrawGroupes[indx].AddOrganized(eid, mesh_model_id, mesh_id);
}


void RenderBuffer::Push(Mesh* mesh, Material* material, glm::mat4 transform, BBox box, bool cast_shadows, glm::vec3 pos, bool is_static, std::string lmPath, unsigned int entID)
{
	RenderCommand command;
	command.Mesh = mesh;
	command.Material = material;
	command.Transform = transform;
	command.bbox = box;
	command.cast_shadows = cast_shadows;
	command.position = pos;
	command.is_static = is_static;
	command.lightmapPath = lmPath;
	command.ID = entID;

	OpaqueRenderCommands.push_back(command);
}

void RenderBuffer::Clear()
{
	OpaqueRenderCommands.clear();
	CutoutRenderCommands.clear();
	AlphaRenderCommands.clear();

	for (size_t i = 0; i < DrawGroupes.size(); i++)
	{
		DrawGroupes[i].Clear();
	}

	for (size_t i = 0; i < DrawGroupesMeshes.size(); i++)
	{
		DrawGroupesMeshes[i].Clear();
	}
}