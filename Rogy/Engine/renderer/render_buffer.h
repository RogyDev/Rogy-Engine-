#ifndef RENDER_BUFFER_H
#define RENDER_BUFFER_H

#include "render_command.h"

class RenderBuffer
{
public:
	RenderBuffer();
	~RenderBuffer();

	std::vector<RenderCommand> OpaqueRenderCommands;
	std::vector<RenderCommand> CutoutRenderCommands;
	std::vector<RenderCommand> AlphaRenderCommands;

	// pushes render state relevant to a single render call to the command buffer.
	void Push(Mesh* mesh, Material* material, glm::mat4 transform, BBox box, bool cast_shadows = true, glm::vec3 pos = glm::vec3(0), bool is_static = false, std::string lmPath = "", unsigned int entID = 0);
	void PushCutout(Mesh* mesh, Material* material, glm::mat4 transform, BBox box, bool cast_shadows = true, glm::vec3 pos = glm::vec3(0), bool is_static = false, std::string lmPath = "", unsigned int entID = 0);

	// clears the command buffer; usually done after issuing all the stored render commands.
	void Clear();

private:

};

#endif