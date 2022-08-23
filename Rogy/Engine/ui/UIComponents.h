#ifndef UI_COMPONENTS_H
#define UI_COMPONENTS_H

#include <string>
#include <glm\glm.hpp>
#include "../scene/RComponent.h"
#include "imgui.h"
#include "../shading/texture.h"

#define UI_WIDGET_TYPE(NAME, TYPE)\
public: NAME(unsigned int uid){type = TYPE; ID = uid;}\

// Widget Type
//-------------------------------------------------
enum UIWidgetType
{
	UI_WIDGET_TEXT,
	UI_WIDGET_BUTTON,
	UI_WIDGET_IMAGE,
	UI_WIDGET_PROGRESSBAR,
	UI_WIDGET_INPUT_TEXT,
	UI_WIDGET_ARRAYS,
	UI_WIDGET_LINE,
};

// Base class for all UI widgets
//-------------------------------------------------
class UIWidgetComponent
{
public:
	bool Enabled = true;
	glm::vec2 Position = glm::vec2(1.0f, 1.0f);
	glm::vec2 Size = glm::vec2(50.0f, 50.0f);
	float Rotation = 0.0f;
	float Alpha = 1.0f;
	UIWidgetType type;
	unsigned int ID;
	std::string name;
	unsigned int fontIndex;

	glm::vec2 toGlm(ImVec2& v){
		return glm::vec2(v.x, v.y);
	}
	ImVec2 toIm(glm::vec2& v){
		return ImVec2(v.x, v.y);
	}

	glm::vec4 toGlm(ImVec4& v) {
		return glm::vec4(v.x, v.y, v.z, v.w);
	}
	ImVec4 toIm(glm::vec4& v) {
		return ImVec4(v.x, v.y, v.z, v.w);
	}

	void SaveBase(YAML::Emitter& out)
	{
		//out << YAML::BeginMap;
		out << YAML::Key << "Type" << YAML::Value << (size_t)type;
		out << YAML::Key << "Enabled" << YAML::Value << Enabled;
		out << YAML::Key << "Position"; RYAML::SerVec2(out, (Position));
		out << YAML::Key << "Size"; RYAML::SerVec2(out, (Size));
		out << YAML::Key << "Rotation" << YAML::Value << Rotation;
		out << YAML::Key << "Alpha" << YAML::Value << Alpha;
		out << YAML::Key << "name" << YAML::Value << name;
	}

	void LoadBase(YAML::Node& data)
	{
		Enabled = data["Enabled"].as<bool>();
		Position = RYAML::GetVec2(data["Position"]);
		Size = RYAML::GetVec2(data["Size"]);
		Rotation = data["Rotation"].as<float>();
		Alpha = data["Alpha"].as<float>();
		name = data["name"].as<std::string>();
	}

	virtual void OnSave(YAML::Emitter& out) {}
	virtual void OnLoad(YAML::Node& data) {}

	template <class Archive>
	void BaseSerialize(Archive & ar)
	{
		ar(Enabled);
		ar(Position.x, Position.y);
		ar(Size.x, Size.y);
		ar(Rotation);
		ar(Alpha);
		ar(name);
	}
};

// Text Widget
//-------------------------------------------------
class UIWidgetText : public UIWidgetComponent
{
	UI_WIDGET_TYPE(UIWidgetText, UI_WIDGET_TEXT)

	std::string text = "Text";
	float Scale = 1.0f;
	bool Wrap = false;
	ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	virtual void OnSave(YAML::Emitter& out) override
	{
		SaveBase(out);
		out << YAML::Key << "text" << YAML::Value << text;
		out << YAML::Key << "Scale" << YAML::Value << Scale;
		out << YAML::Key << "Wrap" << YAML::Value << Wrap;
		out << YAML::Key << "color"; RYAML::SerVec4(out, toGlm(color));
	}

	void OnLoad(YAML::Node& data)
	{
		LoadBase(data);
		text = data["text"].as<std::string>();
		Scale = data["Scale"].as<float>();
		Wrap = data["Wrap"].as<bool>();
		color = toIm(RYAML::GetVec4(data["color"]));
	}

};

// Button Widget
//-------------------------------------------------
class UIWidgetButton : public UIWidgetComponent
{
	UI_WIDGET_TYPE(UIWidgetButton, UI_WIDGET_BUTTON)

	std::string text = "Button";
	float Scale = 1.0f;
	ImVec4 TextColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	ImVec4 Color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 HoveredColor = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
	ImVec4 PressColor = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
	float FrameRounding = 0.0f;
	bool TextAsButton = false;
	
	// Onclick 
	std::string invokeFunc = "";

	Texture* image = nullptr;
	std::string img_path;
	bool load = false;

	virtual void OnSave(YAML::Emitter& out) override
	{
		SaveBase(out);
		out << YAML::Key << "text" << YAML::Value << text;
		out << YAML::Key << "Scale" << YAML::Value << Scale;
		out << YAML::Key << "Color"; RYAML::SerVec4(out, toGlm(Color));
		out << YAML::Key << "TextColor"; RYAML::SerVec4(out, toGlm(TextColor));
		out << YAML::Key << "HoveredColor"; RYAML::SerVec4(out, toGlm(HoveredColor));
		out << YAML::Key << "PressColor"; RYAML::SerVec4(out, toGlm(PressColor));
		out << YAML::Key << "FrameRounding" << YAML::Value << FrameRounding;
		out << YAML::Key << "TextAsButton" << YAML::Value << TextAsButton;
		out << YAML::Key << "invokeFunc" << YAML::Value << invokeFunc;
		if (image != nullptr)
			img_path = image->getTexName();
		out << YAML::Key << "img_path" << YAML::Value << img_path;
	}

	void OnLoad(YAML::Node& data)
	{
		LoadBase(data);
		text = data["text"].as<std::string>();
		Scale = data["Scale"].as<float>();
		Color = toIm(RYAML::GetVec4(data["Color"]));
		TextColor = toIm(RYAML::GetVec4(data["TextColor"]));
		HoveredColor = toIm(RYAML::GetVec4(data["HoveredColor"]));
		PressColor = toIm(RYAML::GetVec4(data["PressColor"]));
		FrameRounding = data["FrameRounding"].as<float>();
		TextAsButton = data["TextAsButton"].as<bool>();
		invokeFunc = data["invokeFunc"].as<std::string>();
		img_path = data["img_path"].as<std::string>();
	}
};

// Image Widget
//-------------------------------------------------
class UIWidgetImage : public UIWidgetComponent
{
	UI_WIDGET_TYPE(UIWidgetImage, UI_WIDGET_IMAGE)

	ImVec4 Color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	Texture* image = nullptr;
	float FrameRounding = 0.0f;

	std::string img_path;
	bool load = false;

	virtual void OnSave(YAML::Emitter& out) override
	{
		SaveBase(out);
		out << YAML::Key << "Color"; RYAML::SerVec4(out, toGlm(Color));
		out << YAML::Key << "FrameRounding" << YAML::Value << FrameRounding;
		if (image != nullptr)
			img_path = image->getTexName();
		out << YAML::Key << "img_path" << YAML::Value << img_path;
	}

	void OnLoad(YAML::Node& data)
	{
		LoadBase(data);
		Color = toIm(RYAML::GetVec4(data["Color"]));
		FrameRounding = data["FrameRounding"].as<float>();
		img_path = data["img_path"].as<std::string>();
	}

};

// ProgressBar Widget
//-------------------------------------------------
class UIWidgetProgressBar : public UIWidgetComponent
{
	UI_WIDGET_TYPE(UIWidgetProgressBar, UI_WIDGET_PROGRESSBAR)

	float Value = 0.5f;
	ImVec4 BarColor = ImVec4(0.0f, 0.5f, 0.1f, 1.0f);
	ImVec4 Color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	float FrameRounding = 0.0f;
	bool RightToLeft = false;

	virtual void OnSave(YAML::Emitter& out) override
	{
		SaveBase(out);
		out << YAML::Key << "Value" << YAML::Value << Value;
		out << YAML::Key << "BarColor"; RYAML::SerVec4(out, toGlm(BarColor));
		out << YAML::Key << "Color"; RYAML::SerVec4(out, toGlm(Color));
		out << YAML::Key << "FrameRounding" << YAML::Value << FrameRounding;
		out << YAML::Key << "RightToLeft" << YAML::Value << RightToLeft;
	}

	void OnLoad(YAML::Node& data)
	{
		LoadBase(data);
		Value = data["Value"].as<float>();
		BarColor = toIm(RYAML::GetVec4(data["BarColor"]));
		Color = toIm(RYAML::GetVec4(data["Color"]));
		FrameRounding = data["FrameRounding"].as<float>();
		RightToLeft = data["RightToLeft"].as<bool>();
	}
};

// Line Widget
//-------------------------------------------------
class UIWidgetLine : public UIWidgetComponent
{
	UI_WIDGET_TYPE(UIWidgetLine, UI_WIDGET_LINE)

	bool VerticalLine = false;
	bool Custom = false;
	float thickness = 1.0f;
	ImVec4 Color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec2 Point2 = ImVec2(1.0f, 1.0f);

	virtual void OnSave(YAML::Emitter& out) override
	{
		SaveBase(out);
		out << YAML::Key << "VerticalLine" << YAML::Value << VerticalLine;
		out << YAML::Key << "Custom" << YAML::Value << Custom;
		out << YAML::Key << "thickness" << YAML::Value << thickness;
		out << YAML::Key << "Color"; RYAML::SerVec4(out, toGlm(Color));
		out << YAML::Key << "Point2"; RYAML::SerVec2(out, toGlm(Point2));
	}

	void OnLoad(YAML::Node& data)
	{
		LoadBase(data);
		VerticalLine = data["VerticalLine"].as<bool>();
		Custom = data["Custom"].as<bool>();
		thickness = data["thickness"].as<float>();
		Color = toIm(RYAML::GetVec4(data["Color"]));
		Point2 = toIm(RYAML::GetVec2(data["Point2"]));
	}
};

// InputTex Widget
//-------------------------------------------------
class UIWidgetInputText : public UIWidgetComponent
{
	UI_WIDGET_TYPE(UIWidgetInputText, UI_WIDGET_INPUT_TEXT)

	std::string text = "Text";
	std::string Hint = "Enter Text";
	float Scale = 1.0f;
	bool Multiline = false;
	bool password = false;
	bool readOnly = false;
	ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 bgColor = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);
	ImVec4 HintColor = ImVec4(0.60f, 0.60f, 0.60f, 1.0f);

	virtual void OnSave(YAML::Emitter& out) override
	{
		SaveBase(out);
		out << YAML::Key << "text" << YAML::Value << text;
		out << YAML::Key << "Hint" << YAML::Value << Hint;
		out << YAML::Key << "Scale" << YAML::Value << Scale;
		out << YAML::Key << "Multiline" << YAML::Value << Multiline;
		out << YAML::Key << "password" << YAML::Value << password;
		out << YAML::Key << "readOnly" << YAML::Value << readOnly;
		out << YAML::Key << "color"; RYAML::SerVec4(out, toGlm(color));
		out << YAML::Key << "bgColor"; RYAML::SerVec4(out, toGlm(bgColor));
		out << YAML::Key << "HintColor"; RYAML::SerVec4(out, toGlm(HintColor));
	}

	void OnLoad(YAML::Node& data)
	{
		LoadBase(data);
		text = data["text"].as<std::string>();
		Hint = data["Hint"].as<std::string>();
		Scale = data["Scale"].as<float>();
		Multiline = data["Multiline"].as<bool>();
		password = data["password"].as<bool>();
		readOnly = data["readOnly"].as<bool>();
		color = toIm(RYAML::GetVec4(data["color"]));
		bgColor = toIm(RYAML::GetVec4(data["bgColor"]));
		HintColor = toIm(RYAML::GetVec4(data["HintColor"]));
	}

};


#endif
