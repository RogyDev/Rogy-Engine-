#ifndef UI_RENDERER_H
#define UI_RENDERER_H

#include "Text.h"
#include "Button.h"
#include "Image.h"
#include "Checker.h"

#include "UIWidget.h"

#include "ft2build.h"
#include "freetype\freetype.h"
#include <map>
#include "shader.h"

#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

struct UIChar {
	GLuint TextureID; // ID handle of the glyph texture
	glm::ivec2 Size; // Size of glyph
	glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
	GLuint Advance; // Offset to advance to next glyph
};

struct UIFont {
	ImFont* font = nullptr;
	std::string source;
	UIFont(ImFont* afont, std::string sorce)
	{
		font = afont;
		source = sorce;
	}
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
	float TextSize(float posX, glm::vec2 &Scale, std::string& text);

	void Update();
	void BeginFrame();
	void EndFrame();

	void LoadFont(std::string src, int f_size);
	ImFont* GetFont(unsigned int f_indx);

	ComponentArray<UIText> texts;

	ComponentArray<UIWidget> uiWidgets;
	void RenderHUD(float dt);


	UIImage* img;
	Texture* Checked = nullptr;
	Texture* Unchecked = nullptr;

	
	void RenderBlurFrames(unsigned int blurImage);
	bool BlurFrame(ImVec2& pos_, glm::vec2& size_);
private:
	
	bool ButtonEx(const char* label, const ImVec2& pos_arg, const ImVec2& size_arg, float textSize, bool textAsButton,float FrameRounding ,ImVec4& color, ImVec4& color_hl, ImVec4& color_pr, ImVec4& text_color, Texture* texture= nullptr);
	bool FrameEx(const ImVec2& pos_arg, const ImVec2& size_arg, ImVec4& color, float FrameRounding);
	bool BarEx(const ImVec2& pos_arg, const ImVec2& size_arg, ImVec4& color, ImVec4& Barcolor, float val, float FrameRounding, bool right_to_left);
	bool TextEx(const char * label, const ImVec2 & pos_arg, float textSize, const ImVec4 & color);
	bool ImageEx(Texture* texture, const ImVec2 & pos_arg, const ImVec2 & size_arg, const ImVec4 & color, const ImVec2 & uvMin = ImVec2(0.f, 0.f), const ImVec2 & uvMax = ImVec2(1.f, 1.f));

	float minVal(float a, float b)
	{
		if (a < b) return a;
		return b;
	}
	ImVec4 colorWithAlpha(ImVec4 col, float alpha);
	ImVec2 vec2Add(ImVec2 a, ImVec2 b);
	ImVec2 vec2Rem(ImVec2 a, ImVec2 b);
	ImVec2 vec2mul(ImVec2 a, ImVec2 b);
	Shader ui_text, ui_frame;

	GLuint VAO, VBO;
	GLuint quadVAO, quadVBO;

	ImGuiWindowFlags winFlags;
	ImGuiWindowFlags childFlags;

	FT_Library ft;
	FT_Face face;
	glm::mat4 projection;
	std::map<GLchar, UIChar> Characters;
	int scr_left, scr_top, scr_weight, scr_height;
	
public:
	std::vector<UIFont> fonts;
};


#endif
