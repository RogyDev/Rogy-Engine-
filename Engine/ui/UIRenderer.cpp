#include "UIRenderer.h"
#include "Text.h"
IMPL_COMPONENT(UIText)

UIRenderer::UIRenderer()
{
}

UIRenderer::~UIRenderer()
{
}
UIButton* button;
UIChecker* checker;

void UIRenderer::Init()
{
	button = new UIButton();
	img = new UIImage();
	checker = new UIChecker();
	checker->Position = glm::vec2(300, 300);
	checker->Scale = glm::vec2(30, 30);

	button->Position = glm::vec2(300, 300);
	button->Scale = glm::vec2(70, 15);
	//button->MainColor = glm::vec3(0.5f, 0.3f, 0.1f);
	button->TextColor = glm::vec3(0.0f);
	button->alpha = 1.0f;

	ui_button.loadShader("core//shaders//UIButton.rsh");
	ui_button.use();
	ui_button.setInt("Image", 0);
	ui_text.loadShader("core//shaders//UIText.rsh");
	ui_text.use();
	ui_text.setInt("text", 0);
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	if (FT_New_Face(ft, "core/font/DroidSans.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	FT_Set_Pixel_Sizes(face, 0, 48);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		UIChar character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			(unsigned int)face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, UIChar>(c, character));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	// Initialize Quad
	float quadVertices[] = {
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};
	// setup plane VAO
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void UIRenderer::SetScreenSize(int left, int top, int w, int h)
{
	if (scr_left != left || scr_top != top || scr_weight != w || scr_height != h)
	{
		scr_left = left;
		scr_top = top;
		scr_weight = w;
		scr_height = h;
		//projection = glm::ortho(0.0f, (float)w, 0.0f, (float)h);
		projection = glm::ortho(0.0f, (float)w, (float)h, 0.0f);
		//projection = glm::ortho(0.0f, (float)w, (float)h, 0.0f, -1.0f, 1.0f);
	}
}

void UIRenderer::Render(int mouseX, int mouseY, bool LeftClicked)
{
	button->CheckIntersection(mouseX, mouseY, LeftClicked);
	//checker->CheckIntersection(mouseX, mouseY, LeftClicked);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ui_text.use();
	ui_text.SetMat4("projection", projection);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);
	
	auto t = texts.GetComponents();
	for (size_t i = 0; i < t.size(); i++)
	{
		if (!t[i]->enabled || t[i]->alpha == 0.0f) continue;
		RenderText(t[i]->Position, t[i]->Scale, t[i]->color, t[i]->alpha, t[i]->text);
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Render Checker 
	// ---------------------------------------
	/*ui_button.use();
	ui_button.SetMat4("projection", projection);

	//scr_height
	glm::vec2 pos = checker->Position;
	pos.y = scr_height - checker->Position.y + checker->Scale.y;
	glm::vec2 sca = checker->Scale;

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(pos, 0.0f));
	model = glm::scale(model, glm::vec3(sca, 1.0f));

	ui_button.SetMat4("model", model);
	ui_button.SetVec3("Color", glm::vec3(1.0f));
	ui_button.SetFloat("Alpha", checker->alpha);
	glActiveTexture(GL_TEXTURE0);
	ui_button.setBool("use_image", true);
	if (checker->checked)
		Checked->useTexture();
	else
		Unchecked->useTexture();
	RenderQuad();
	*/
	// Render Image 
	// ---------------------------------------
	ui_button.use();
	ui_button.SetMat4("projection", projection);

	//scr_height
	//glm::vec2 
	glm::vec2 pos = glm::vec2(mouseX, mouseY);
	//pos.y = scr_height - img->Position.y + img->Scale.y * 2 - 5;
	glm::vec2 
		sca = img->Scale;

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(pos, 0.0f));
	model = glm::scale(model, glm::vec3(sca, 1.0f));

	ui_button.SetMat4("model", model);
	ui_button.SetVec3("Color", img->color);
	ui_button.SetFloat("Alpha", img->alpha);
	if (img->texture != nullptr)
	{
		glActiveTexture(GL_TEXTURE0);
		img->texture->useTexture();
		ui_button.setBool("use_image", true);
	}
	else ui_button.setBool("use_image", false);
	RenderQuad();
	
	// Render Button 
	
	ui_button.use();
	//scr_height
	//glm::vec2 
		pos = button->Position;
	pos.y -= button->Scale.y * 2;
	//glm::vec2 
		sca = button->Scale;

	model = glm::translate(glm::mat4(1.0f), glm::vec3(pos, 0.0f));
	model = glm::scale(model, glm::vec3(sca, 1.0f));

	ui_button.SetMat4("model", model);
	ui_button.SetMat4("projection", projection);
	ui_button.SetVec3("Color", button->color);
	ui_button.SetFloat("Alpha", button->alpha);
	RenderQuad();

	ui_text.use();
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	pos.x -= button->Scale.x - button->text_offset.x;
	pos.y -= button->text_offset.y;

	if (button->enabled || button->alpha != 0.0f) 
		RenderText(pos, button->text_size, button->texCol, button->alpha, button->text);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void UIRenderer::RenderQuad()
{
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void UIRenderer::RenderText(glm::vec2 &Pos, glm::vec2 &Scale, glm::vec3& color, float alpha, std::string& text)
{
	ui_text.SetVec3("textColor", color);
	ui_text.SetFloat("textAlpha", alpha);

	float scale = Scale.x;
	float x = Pos.x;
	float y = Pos.y;
	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		UIChar ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		//GLfloat ypos = y + (ch.Size.y - ch.Bearing.y) * scale;
		GLfloat ypos = y + (this->Characters['H'].Bearing.y - ch.Bearing.y) * scale;
		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		GLfloat vertices[6][4] = {
			{ xpos, ypos + h, 0.0, 1.0 },
			{ xpos + w, ypos, 1.0, 0.0 },
			{ xpos, ypos, 0.0, 0.0 },
			{ xpos, ypos + h, 0.0, 1.0 },
			{ xpos + w, ypos + h, 1.0, 1.0 },
			{ xpos + w, ypos, 1.0, 0.0 }
		};
		/*GLfloat vertices[6][4] = {
			{ xpos, ypos + h, 0.0, 1.0 },
			{ xpos + w, ypos, 1.0, 0.0 },
			{ xpos, ypos, 0.0, 0.0 },
			{ xpos, ypos + h, 0.0, 1.0 },
			{ xpos + w, ypos + h, 1.0, 1.0 },
			{ xpos + w, ypos, 1.0, 0.0 }
		};
		*/
		/*GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos, ypos + h, 0.0, 0.0 },
			{ xpos, ypos, 0.0, 1.0 },
			{ xpos + w, ypos, 1.0, 1.0 },
			{ xpos, ypos + h, 0.0, 0.0 },
			{ xpos + w, ypos, 1.0, 1.0 },
			{ xpos + w, ypos + h, 1.0, 0.0 }
		};*/
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6= 64)
	}
}
