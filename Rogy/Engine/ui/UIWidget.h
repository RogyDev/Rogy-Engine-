#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <string>
#include <glm\glm.hpp>
#include "../scene/RComponent.h"
#include "UIComponents.h"
//#include "../resources/resources_manger.h"

class UIWidget : public Component
{
	BASE_COMPONENT()
public:
	ImVec2 r_pos;

	glm::vec2 Position = glm::vec2(50, 50);
	glm::vec2 Scale = glm::vec2(10, 10);
	float Rotation = 0.0f;
	glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 0.9f);
	float FrameRounding = 0.0f;
	bool BlurBackground = false;
	float Alpha = 1.0f;

	std::vector<UIWidgetComponent*> widgets;

	UIWidgetComponent* AddUIWidget(UIWidgetType widget_type);
	void RemoveUIWidget(unsigned int widget_ID);
	void DuplicateUIWidget(unsigned int widget_ID);

	template<typename T>
	T* GetWidgetType(UIWidgetComponent* comp)
	{
		if (comp == nullptr) return nullptr;
		return static_cast<T*>(comp);
	}

	template<typename T>
	T* GetWidgetWithName(std::string wname)
	{
		for (size_t i = 0; i < widgets.size(); i++)
		{
			if(widgets[i]->name == wname)
				return static_cast<T*>(widgets[i]);
		}
		return nullptr;
	}

	template<typename T>
	T* GetUIWidget(UIWidgetType widgetType , unsigned int widget_ID)
	{
		for (size_t i = 0; i < widgets.size(); i++)
		{
			if (widgets[i]->ID == widget_ID && widgets[i]->type == widgetType)
				return static_cast<T*>(widgets[i]);
		}
		return nullptr;
	}

	UIWidgetComponent* GetUIWidgetRow(unsigned int widget_ID)
	{
		for (size_t i = 0; i < widgets.size(); i++)
		{
			if (widgets[i]->ID == widget_ID)
				return widgets[i];
		}
		return nullptr;
	}

	unsigned int selectedWidget = 0;

	// Serialization
	// ----------------------------------------------------
	virtual void OnSave(YAML::Emitter& out) override;
	template <class Archive, class RES>
	void OnLoadCustom(YAML::Node& data, RES& res)
	{
		enabled = data["enabled"].as<bool>();
		Position = RYAML::GetVec2(data["Position"]);
		Scale = RYAML::GetVec2(data["Scale"]);
		Rotation = data["Rotation"].as<float>();
		color = RYAML::GetVec4(data["color"]);
		FrameRounding = data["FrameRounding"].as<float>();
		Alpha = data["Alpha"].as<float>();
		BlurBackground = data["BlurBackground"].as<bool>();
		size_t wdg_count = data["wdg_count"].as<size_t>();
		auto wdgs = data["Widgets"];
		//for (size_t i = 0; i < wdg_count; i++)
		for(auto ar : wdgs)
		{
			UIWidgetType wdgType = (UIWidgetType)(ar["Type"].as<size_t>());
			if ((UIWidgetType)wdgType == UI_WIDGET_TEXT)
			{
				UIWidgetText* w = GetWidgetType<UIWidgetText>(AddUIWidget(UI_WIDGET_TEXT));
				w->OnLoad(ar);
			}
			else if ((UIWidgetType)wdgType == UI_WIDGET_BUTTON)
			{
				UIWidgetButton* w = GetWidgetType<UIWidgetButton>(AddUIWidget(UI_WIDGET_BUTTON));
				w->load = true;
				w->OnLoad(ar);
				if (w->img_path != "")
					w->image = res.CreateTexture(w->img_path, w->img_path.c_str(), false);
			}
			else if ((UIWidgetType)wdgType == UI_WIDGET_LINE)
			{
				UIWidgetLine* w = GetWidgetType<UIWidgetLine>(AddUIWidget(UI_WIDGET_LINE));
				w->OnLoad(ar);
			}
			else if ((UIWidgetType)wdgType == UI_WIDGET_INPUT_TEXT)
			{
				UIWidgetInputText* w = GetWidgetType<UIWidgetInputText>(AddUIWidget(UI_WIDGET_INPUT_TEXT));
				w->OnLoad(ar);
			}
			else if ((UIWidgetType)wdgType == UI_WIDGET_PROGRESSBAR)
			{
				UIWidgetProgressBar* w = GetWidgetType<UIWidgetProgressBar>(AddUIWidget(UI_WIDGET_PROGRESSBAR));
				w->OnLoad(ar);
			}
			else if ((UIWidgetType)wdgType == UI_WIDGET_IMAGE)
			{
				UIWidgetImage* w = GetWidgetType<UIWidgetImage>(AddUIWidget(UI_WIDGET_IMAGE));
				w->load = true;
				w->OnLoad(ar);
				if (w->img_path != "")
					w->image = res.CreateTexture(w->img_path, w->img_path.c_str(), false);
			}
		}
	}

	template <class Archive>
	void SerializeSave(Archive & ar)
	{
		ar(enabled);
		ar(Position.x, Position.y);
		ar(Scale.x, Scale.y);
		ar(Rotation);
		ar(color.x, color.y, color.z, color.w);
		ar(FrameRounding);
		ar(Alpha);
		ar(BlurBackground);

		ar(widgets.size());
		for (size_t i = 0; i < widgets.size(); i++)
		{

			ar((int)widgets[i]->type);
			if (widgets[i]->type == UI_WIDGET_TEXT)
			{
				UIWidgetText* e = static_cast<UIWidgetText*>(widgets[i]);
				e->Serialize<Archive>(ar);
			}
			else if (widgets[i]->type == UI_WIDGET_BUTTON)
			{
				UIWidgetButton* e = static_cast<UIWidgetButton*>(widgets[i]);
				e->load = false;
				e->Serialize<Archive>(ar);
			}
			else if (widgets[i]->type == UI_WIDGET_LINE)
			{
				UIWidgetLine* e = static_cast<UIWidgetLine*>(widgets[i]);
				e->Serialize<Archive>(ar);
			}
			else if (widgets[i]->type == UI_WIDGET_INPUT_TEXT)
			{
				UIWidgetInputText* e = static_cast<UIWidgetInputText*>(widgets[i]);
				e->Serialize<Archive>(ar);
			}
			else if (widgets[i]->type == UI_WIDGET_PROGRESSBAR)
			{
				UIWidgetProgressBar* e = static_cast<UIWidgetProgressBar*>(widgets[i]);
				e->Serialize<Archive>(ar);
			}
			else if (widgets[i]->type == UI_WIDGET_IMAGE)
			{
				UIWidgetImage* e = static_cast<UIWidgetImage*>(widgets[i]);
				e->load = false;
				e->Serialize<Archive>(ar);
			}

		}
	}

	template <class Archive, class RES>
	void SerializeLoad(Archive & ar, RES& res)
	{
		ar(enabled);
		ar(Position.x, Position.y);
		ar(Scale.x, Scale.y);
		ar(Rotation);
		ar(color.x, color.y, color.z, color.w);
		ar(FrameRounding);
		ar(Alpha);
		ar(BlurBackground);

		size_t wdgt_Count;
		ar(wdgt_Count);
		for (size_t i = 0; i < wdgt_Count; i++)
		{
			int wdgType; ar(wdgType);
			if ((UIWidgetType)wdgType == UI_WIDGET_TEXT)
			{
				UIWidgetText* w = GetWidgetType<UIWidgetText>(AddUIWidget(UI_WIDGET_TEXT));
				w->Serialize<Archive>(ar);
			}
			else if ((UIWidgetType)wdgType == UI_WIDGET_BUTTON)
			{
				UIWidgetButton* w = GetWidgetType<UIWidgetButton>(AddUIWidget(UI_WIDGET_BUTTON));
				w->load = true;
				w->Serialize<Archive>(ar);
				if (w->img_path != "")
					w->image = res.CreateTexture(w->img_path, w->img_path.c_str(), false);
			}
			else if ((UIWidgetType)wdgType == UI_WIDGET_LINE)
			{
				UIWidgetLine* w = GetWidgetType<UIWidgetLine>(AddUIWidget(UI_WIDGET_LINE));
				w->Serialize<Archive>(ar);
			}
			else if ((UIWidgetType)wdgType == UI_WIDGET_INPUT_TEXT)
			{
				UIWidgetInputText* w = GetWidgetType<UIWidgetInputText>(AddUIWidget(UI_WIDGET_INPUT_TEXT));
				w->Serialize<Archive>(ar);
			}
			else if ((UIWidgetType)wdgType == UI_WIDGET_PROGRESSBAR)
			{
				UIWidgetProgressBar* w = GetWidgetType<UIWidgetProgressBar>(AddUIWidget(UI_WIDGET_PROGRESSBAR));
				w->Serialize<Archive>(ar);
			}
			else if ((UIWidgetType)wdgType == UI_WIDGET_IMAGE)
			{
				UIWidgetImage* w = GetWidgetType<UIWidgetImage>(AddUIWidget(UI_WIDGET_IMAGE));
				w->load = true;
				w->Serialize<Archive>(ar);
				if (w->img_path != "")
					w->image = res.CreateTexture(w->img_path, w->img_path.c_str(), false);
			}
		}
		
	}
	// ----------------------------------------------------
private:
	unsigned int uniqueID = 1;
};


#endif
