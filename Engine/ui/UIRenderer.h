#ifndef UI_RENDERER_H
#define UI_RENDERER_H

#include "Text.h"
#include "Button.h"
#include "Image.h"
#include "Checker.h"

#include "ft2build.h"
#include "freetype\freetype.h"
#include <map>
#include "shader.h"

#include <glm/gtc/matrix_transform.hpp>

struct UIChar {
	GLuint TextureID; // ID handle of the glyph texture
	glm::ivec2 Size; // Size of glyph
	glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
	GLuint Advance; // Offset to advance to next glyph
};

class UIRenderer
{
public:
	UIRenderer();
	~UIRenderer();

	void Init();
	void SetScreenSize(int left, int top, int w, int h);
	void Render(int mouseX, int mouseY, bool LeftClicked);
	void RenderQuad();
	void RenderText(glm::vec2 &Pos, glm::vec2 &Scale, glm::vec3& color, float alpha, std::string& text);
	ComponentArray<UIText> texts;
	UIImage* img;
	Texture* Checked = nullptr;
	Texture* Unchecked = nullptr;
private:
	Shader ui_text, ui_button;
	GLuint VAO, VBO;
	GLuint quadVAO, quadVBO;

	FT_Library ft;
	FT_Face face;
	glm::mat4 projection;
	std::map<GLchar, UIChar> Characters;
	int scr_left, scr_top, scr_weight, scr_height;
};


#endif
