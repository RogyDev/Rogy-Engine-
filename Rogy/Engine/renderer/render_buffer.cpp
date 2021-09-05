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
}