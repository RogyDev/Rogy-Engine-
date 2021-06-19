#include "UIRenderer.h"
#include "Text.h"
IMPL_COMPONENT(UIText)
IMPL_COMPONENT(UIWidget)

#include "../scene/Entity.h"

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
	ui_frame.loadShader("core//shaders//UIFrame.rsh");
	ui_frame.use();
	ui_frame.setInt("Image", 0);

	/*button = new UIButton();
	img = new UIImage();
	checker = new UIChecker();
	checker->Position = glm::vec2(300, 300);
	checker->Scale = glm::vec2(30, 30);

	button->Position = glm::vec2(300, 300);
	button->Scale = glm::vec2(80, 30);
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
	*/

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
	

	/*float vertices[] = {
		// pos      // tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->quadVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	*/
	winFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_UnsavedDocument | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar;
		
	//winFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;
	childFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_UnsavedDocument | ImGuiWindowFlags_NoBringToFrontOnFocus;
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
		//std::cout << "UI UPDATE PROJ\n";
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
	/*ui_button.use();
	ui_button.SetMat4("projection", projection);

	//scr_height
	//glm::vec2 
	glm::vec2 pos = glm::vec2(mouseX, mouseY);
	//pos.y = scr_height - img->Position.y + img->Scale.y * 2 - 5;
	glm::vec2 sca = img->Scale;

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
	glEnable(GL_CULL_FACE);*/
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

float UIRenderer::TextSize(float posX, glm::vec2 & Scale, std::string & text)
{
	float scale = Scale.x;
	float x = posX;
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
		x += (Characters[*c].Advance >> 6) * scale;
	return x;
}

void UIRenderer::BeginFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void UIRenderer::EndFrame()
{
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		//GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		//glfwMakeContextCurrent(backup_current_context);
	}
}

void UIRenderer::RenderHUD(float dt)
{
	uiWidgets.ClearRemovedComponents();

	auto wds = uiWidgets.GetComponents();
	ImVec2 platformPos = ImGui::GetPlatformIO().Platform_GetWindowPos(ImGui::GetMainViewport());
	for (size_t i = 0; i < wds.size(); i++)
	{
		UIWidget* wdg = wds[i];
		if (wdg->enabled)
		{
			wdg->r_pos = ImVec2(platformPos.x + scr_left + (wdg->Position.x * scr_weight / 100), platformPos.y + 55 + (wdg->Position.y * scr_height / 100));
			ImGui::SetNextWindowPos(wdg->r_pos);
			ImVec2 ss = ImVec2((wdg->Scale.x * scr_weight/100), wdg->Scale.y * scr_height/100);
			ImGui::SetNextWindowSizeConstraints(ss, ss);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, wdg->FrameRounding);
			
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(wdg->color.x, wdg->color.y, wdg->color.z, wdg->color.w * wdg->Alpha));
			ImGui::Begin("Win1" + i, NULL, winFlags);

			for (size_t i = 0; i < wdg->widgets.size(); i++)
			{
				if (!wdg->widgets[i]->Enabled)
					continue;

				if (wdg->widgets[i]->type == UI_WIDGET_TEXT)
				{
					UIWidgetText* text = static_cast<UIWidgetText*>(wdg->widgets[i]);
					if (text->Wrap)
					{
						ImVec2 pos_arg = vec2mul(vec2mul(text->Position, ImVec2(0.01f, 0.01f)), ImGui::GetCurrentWindow()->SizeFull);
						ImVec2 size_arg = vec2mul(vec2mul(text->Size, ImVec2(0.01f, 0.01f)), ImGui::GetCurrentWindow()->SizeFull);

						ImGui::GetCurrentWindow()->DC.CursorPos = vec2Add(pos_arg, ImGui::GetCurrentWindow()->Pos);
						ImGui::PushStyleColor(ImGuiCol_Text, colorWithAlpha(text->color, text->Alpha * wdg->Alpha));
						ImGui::SetNextWindowBgAlpha(0.0f);
						ImGui::BeginChild("textWrapped", size_arg, false, childFlags);
						ImGui::SetWindowFontScale(text->Scale);
						ImGui::TextWrapped(text->text.c_str());
						ImGui::PopStyleColor();
						ImGui::EndChild();
					}
					else
						TextEx(text->text.c_str(), text->Position, text->Scale, colorWithAlpha(text->color, text->Alpha * wdg->Alpha));
					continue;
				}
				else if (wdg->widgets[i]->type == UI_WIDGET_BUTTON)
				{
					UIWidgetButton* button = static_cast<UIWidgetButton*>(wdg->widgets[i]);
					ImGui::PushStyleColor(ImGuiCol_Text, button->TextColor);
					if (ButtonEx(button->text.c_str(), button->Position, button->Size, button->Scale,
						button->TextAsButton, button->FrameRounding, colorWithAlpha(button->Color, button->Alpha * wdg->Alpha),
						colorWithAlpha(button->HoveredColor, button->Alpha * wdg->Alpha), colorWithAlpha(button->PressColor, button->Alpha * wdg->Alpha),
						colorWithAlpha(button->TextColor, button->Alpha * wdg->Alpha), button->image))
					{
						Entity* ent = (Entity*)(wdg->entity);
						ent->InvokeScriptFunc(button->invokeFunc.c_str());
					}
					ImGui::PopStyleColor();
					continue;
				}
				else if (wdg->widgets[i]->type == UI_WIDGET_IMAGE)
				{
					UIWidgetImage* image = static_cast<UIWidgetImage*>(wdg->widgets[i]);
					if (image->image != nullptr)
						ImageEx(image->image, image->Position, image->Size, colorWithAlpha(image->Color, image->Alpha * wdg->Alpha));
					else
						FrameEx(image->Position, image->Size, colorWithAlpha(image->Color, image->Alpha * wdg->Alpha), image->FrameRounding);
					continue;
				}
				else if (wdg->widgets[i]->type == UI_WIDGET_PROGRESSBAR)
				{
					UIWidgetProgressBar* pbar = static_cast<UIWidgetProgressBar*>(wdg->widgets[i]);
					BarEx(pbar->Position, pbar->Size, colorWithAlpha(pbar->Color, pbar->Alpha * wdg->Alpha), colorWithAlpha(pbar->BarColor, pbar->Alpha * wdg->Alpha), pbar->Value, pbar->FrameRounding, pbar->RightToLeft);
					continue;
				}
				else if (wdg->widgets[i]->type == UI_WIDGET_LINE)
				{
					UIWidgetLine* pbar = static_cast<UIWidgetLine*>(wdg->widgets[i]);
					ImVec2 wpos = ImGui::GetCurrentWindow()->Pos;
					if(!pbar->Custom)
					{
						pbar->Point2 = pbar->Position;
						if (pbar->VerticalLine)
							pbar->Point2.y += pbar->Size.y;
						else
							pbar->Point2.x += pbar->Size.x;
					}
					ImVec2 pnt1 = vec2mul(vec2mul(pbar->Position, ImVec2(0.01f, 0.01f)), ImGui::GetCurrentWindow()->SizeFull);
					ImVec2 pnt2 = vec2mul(vec2mul(pbar->Point2, ImVec2(0.01f, 0.01f)), ImGui::GetCurrentWindow()->SizeFull);
					ImGui::GetCurrentWindow()->DrawList->AddLine(vec2Add(wpos, pnt1), vec2Add(wpos, pnt2), ImGui::GetColorU32(colorWithAlpha(pbar->Color, pbar->Alpha * wdg->Alpha)), pbar->thickness);
					continue;
				}
				else if (wdg->widgets[i]->type == UI_WIDGET_INPUT_TEXT)
				{
					UIWidgetInputText* itext = static_cast<UIWidgetInputText*>(wdg->widgets[i]);
					ImVec2 wpos = ImGui::GetCurrentWindow()->Pos;

					ImVec2 pos_arg = vec2mul(vec2mul(itext->Position, ImVec2(0.01f, 0.01f)), ImGui::GetCurrentWindow()->SizeFull);
					ImVec2 size_arg = vec2mul(vec2mul(itext->Size, ImVec2(0.01f, 0.01f)), ImGui::GetCurrentWindow()->SizeFull);

					ImGui::GetCurrentWindow()->DC.CursorPos = vec2Add(pos_arg, ImGui::GetCurrentWindow()->Pos);
					ImGui::PushStyleColor(ImGuiCol_Text, colorWithAlpha(itext->color, itext->Alpha * wdg->Alpha));
					ImGui::PushStyleColor(ImGuiCol_TextDisabled, colorWithAlpha(itext->HintColor, itext->Alpha * wdg->Alpha));
					ImGui::PushStyleColor(ImGuiCol_FrameBg, colorWithAlpha(itext->bgColor, itext->Alpha * wdg->Alpha));
					ImGui::SetNextWindowBgAlpha(0.0f);
					ImGui::BeginChild("itextWrapped", size_arg, false, childFlags);
					ImGui::SetWindowFontScale(itext->Scale);
					ImGuiInputTextFlags input_flags = ImGuiInputTextFlags_None;
					if (itext->password)
						input_flags |= ImGuiInputTextFlags_Password;
					if (itext->readOnly)
						input_flags |= ImGuiInputTextFlags_ReadOnly;
					if (itext->Multiline)
						ImGui::InputTextMultiline("##itext", &itext->text, size_arg, input_flags);
					else
						ImGui::InputTextWithHint("##itext", itext->Hint.c_str(), &itext->text, input_flags);
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::EndChild();

					continue;
				}
			}
			ImGui::End();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
		}
	}
}

void UIRenderer::RenderBlurFrames(unsigned int blurImage)
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	ui_frame.use();
	ui_frame.SetMat4("projection", projection);
	ui_frame.SetVec2("ss", glm::vec2(scr_weight, scr_height));
	ui_frame.SetVec2("sp", glm::vec2(scr_left, scr_top));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blurImage);

	auto wds = uiWidgets.GetComponents();
	ImVec2 platformPos = ImGui::GetPlatformIO().Platform_GetWindowPos(ImGui::GetMainViewport());
	for (size_t i = 0; i < wds.size(); i++)
	{
		UIWidget* wdg = wds[i];
		if (wdg->enabled && wdg->BlurBackground && wdg->Alpha >= 1.0f)
		{
			BlurFrame(ImVec2((wdg->Position.x * scr_weight / 100), (wdg->Position.y * scr_height / 100)),
				glm::vec2((wdg->Scale.x / 2 * scr_weight / 100), (wdg->Scale.y / 2 * scr_height / 100)));
		}
	}
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

bool UIRenderer::BlurFrame(ImVec2 & pos_, glm::vec2 & size_)
{
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(pos_.x + size_.x , pos_.y + size_.y , 0.0f));
	model = glm::scale(model, glm::vec3(size_, 1.0f));

	ui_frame.SetVec4("Color", glm::vec4(0.1f, 0.5f, 0.5f, 1.0f));
	ui_frame.SetMat4("model", model);
	RenderQuad();

	return true;
}

bool UIRenderer::TextEx(const char * label, const ImVec2 & pos_ar, float textSize, const ImVec4 & color)
{
	ImVec2 pos_arg = vec2mul(vec2mul(pos_ar, ImVec2(0.01f, 0.01f)), ImGui::GetCurrentWindow()->SizeFull);
	ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFont()->FontSize * textSize, vec2Add(pos_arg, ImGui::GetCurrentWindow()->Pos), ImGui::GetColorU32(color), label);
	return true;
}


bool UIRenderer::ImageEx(Texture* texture, const ImVec2 & pos_ar, const ImVec2 & size_ar, const ImVec4 & color, const ImVec2 & uvMin , const ImVec2 & uvMax)
{
	ImVec2 pos_arg = vec2mul(vec2mul(pos_ar, ImVec2(0.01f, 0.01f)), ImGui::GetCurrentWindow()->SizeFull);
	ImVec2 size_arg = vec2mul(vec2mul(size_ar, ImVec2(0.01f, 0.01f)), ImGui::GetCurrentWindow()->SizeFull);
	ImVec2 pos = vec2Add(pos_arg, ImGui::GetCurrentWindow()->Pos);
	ImRect bb(pos, vec2Add(pos, size_arg));
	ImGui::GetCurrentWindow()->DrawList->AddImage((GLuint*)texture->getTexID(), bb.Min, bb.Max, uvMin, uvMax, ImGui::GetColorU32(color));
	return true;
}


bool UIRenderer::ButtonEx(const char* label, const ImVec2& pos_ar, const ImVec2& size_ar, float textSize, bool textAsButton, float FrameRounding, ImVec4& color, ImVec4& color_hl, ImVec4& color_pr, ImVec4& text_color , Texture* texture)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true, -1.0f, textSize);

	ImVec2 pos_arg = vec2mul(vec2mul(pos_ar, ImVec2(0.01f, 0.01f)), window->SizeFull);
	ImVec2 pos = vec2Add(pos_arg, window->Pos);

	ImVec2 size_arg = vec2mul(vec2mul(size_ar, ImVec2(0.01f, 0.01f)), window->SizeFull);

	ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, vec2Add(pos, size));
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	//if (window->DC.ItemFlags & ImGuiItemFlags_ButtonRepeat)
	//	flags |= ImGuiButtonFlags_Repeat;
	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

	// Render
	const ImU32 col = ImGui::GetColorU32((held && hovered) ? color_pr : hovered ? color_hl : color);
	ImGui::RenderNavHighlight(bb, id);
	if (texture != nullptr)
	{
		window->DrawList->AddImage((GLuint*)texture->getTexID(), bb.Min, bb.Max, ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(col));
	}
	else ImGui::RenderFrame(bb.Min, bb.Max, col, true, FrameRounding);
	ImGui::PushStyleColor(ImGuiCol_Text, text_color);
	ImGui::RenderTextClippedEx(g.CurrentWindow->DrawList, vec2Add(bb.Min, style.FramePadding), vec2Rem(bb.Max, style.FramePadding), label, NULL, &label_size, style.ButtonTextAlign, &bb, textSize / 2 * g.Font->FontSize);
	ImGui::PopStyleColor();
	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.LastItemStatusFlags);
	return pressed;
}

bool UIRenderer::FrameEx(const ImVec2 & pos_ar, const ImVec2 & size_ar, ImVec4 & color, float FrameRounding)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImVec2 pos_arg = vec2mul(vec2mul(pos_ar, ImVec2(0.01f, 0.01f)), window->SizeFull);
	ImVec2 size_arg = vec2mul(vec2mul(size_ar, ImVec2(0.01f, 0.01f)), window->SizeFull);

	ImVec2 pos = vec2Add(pos_arg, window->Pos);
	const ImRect bb(pos, vec2Add(pos, size_arg));
	ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(color), true, FrameRounding);
	return true;
}

bool UIRenderer::BarEx(const ImVec2 & pos_ar, const ImVec2 & size_ar, ImVec4 & color, ImVec4 & Barcolor, float val, float FrameRounding, bool right_to_left)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImVec2 pos_arg = vec2mul(vec2mul(pos_ar, ImVec2(0.01f, 0.01f)), window->SizeFull);
	ImVec2 size_arg = vec2mul(vec2mul(size_ar, ImVec2(0.01f, 0.01f)), window->SizeFull);

	ImVec2 pos = vec2Add(pos_arg, window->Pos);
	const ImRect bb(pos, vec2Add(pos, size_arg));
	ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(color), true, FrameRounding);
	
	if (right_to_left)
	{
		float xPos = (glm::abs(pos.x - bb.Max.x) * val) + minVal(pos.x, bb.Max.x);
		ImGui::RenderFrame(ImVec2(xPos, pos.y), bb.Max, ImGui::GetColorU32(Barcolor), true, FrameRounding);
	}
	else
		ImGui::RenderFrame(pos, vec2Add(pos, ImVec2(size_arg.x * val, size_arg.y)), ImGui::GetColorU32(Barcolor), true, FrameRounding);
	return true;
}

ImVec4 UIRenderer::colorWithAlpha(ImVec4 col, float alpha)
{
	return ImVec4(col.x, col.y, col.z, col.w * alpha);
}

ImVec2 UIRenderer::vec2Add(ImVec2  a, ImVec2  b)
{
	return ImVec2(a.x + b.x, a.y + b.y);
}

ImVec2 UIRenderer::vec2Rem(ImVec2 a, ImVec2 b)
{
	return ImVec2(a.x - b.x, a.y - b.y);
}

ImVec2 UIRenderer::vec2mul(ImVec2 a, ImVec2 b)
{
	return ImVec2(a.x * b.x, a.y * b.y);
}
