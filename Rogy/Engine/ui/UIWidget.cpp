#include "UIWidget.h"
// ------------------------------------------------------------------------------------
UIWidgetComponent* UIWidget::AddUIWidget(UIWidgetType widget_type)
{
	if (widget_type == UIWidgetType::UI_WIDGET_TEXT)
	{
		UIWidgetComponent* new_widget = new UIWidgetText(uniqueID);
		new_widget->name = "Text";
		widgets.push_back(new_widget);
		uniqueID++;
		return new_widget;
	}
	else if (widget_type == UIWidgetType::UI_WIDGET_BUTTON)
	{
		UIWidgetComponent* new_widget = new UIWidgetButton(uniqueID);
		new_widget->name = "Button";
		widgets.push_back(new_widget);
		uniqueID++;
		return new_widget;
	}
	else if (widget_type == UIWidgetType::UI_WIDGET_IMAGE)
	{
		UIWidgetComponent* new_widget = new UIWidgetImage(uniqueID);
		new_widget->name = "Image";
		widgets.push_back(new_widget);
		uniqueID++;
		return new_widget;
	}
	else if (widget_type == UIWidgetType::UI_WIDGET_PROGRESSBAR)
	{
		UIWidgetComponent* new_widget = new UIWidgetProgressBar(uniqueID);
		new_widget->name = "Progress Bar";
		widgets.push_back(new_widget);
		uniqueID++;
		return new_widget;
	}
	else if (widget_type == UIWidgetType::UI_WIDGET_LINE)
	{
		UIWidgetComponent* new_widget = new UIWidgetLine(uniqueID);
		new_widget->name = "Line";
		widgets.push_back(new_widget);
		uniqueID++;
		return new_widget;
	}
	else if (widget_type == UIWidgetType::UI_WIDGET_INPUT_TEXT)
	{
		UIWidgetInputText* new_widget = new UIWidgetInputText(uniqueID);
		new_widget->name = "InputText";
		widgets.push_back(new_widget);
		uniqueID++;
		return new_widget;
	}
	return nullptr;
}
// ------------------------------------------------------------------------------------
void UIWidget::RemoveUIWidget(unsigned int widget_ID)
{
	for (size_t i = 0; i < widgets.size(); i++)
	{
		if (widgets[i]->ID == widget_ID)
		{
			UIWidgetComponent* comp = widgets[i];
			widgets.erase(widgets.begin() + i);
			delete comp;
		}
	}
}
// ------------------------------------------------------------------------------------
void UIWidget::DuplicateUIWidget(unsigned int widget_ID)
{
	UIWidgetComponent* wdg = GetUIWidgetRow(widget_ID);
	if (wdg == nullptr) return;

	if (wdg->type == UI_WIDGET_TEXT)
	{
		UIWidgetText* e = static_cast<UIWidgetText*>(wdg);
		UIWidgetText* n = static_cast<UIWidgetText*>(AddUIWidget(UI_WIDGET_TEXT));
		n->Enabled = e->Enabled;
		n->Position = e->Position;
		n->Size = e->Size;
		n->Rotation = e->Rotation;
		n->Alpha = e->Alpha;
		n->name = e->name;

		n->text = e->text;
		n->Scale = e->Scale;
		n->Wrap = e->Wrap;
		n->color = e->color;
		return;
	}

	else if (wdg->type == UI_WIDGET_BUTTON)
	{
		UIWidgetButton* e = static_cast<UIWidgetButton*>(wdg);
		UIWidgetButton* n = static_cast<UIWidgetButton*>(AddUIWidget(UI_WIDGET_BUTTON));
		n->Enabled = e->Enabled;
		n->Position = e->Position;
		n->Size = e->Size;
		n->Rotation = e->Rotation;
		n->Alpha = e->Alpha;
		n->name = e->name;

		n->text = e->text;
		n->Scale = e->Scale;
		n->TextColor = e->TextColor;
		n->Color = e->Color;
		n->HoveredColor = e->HoveredColor;
		n->PressColor = e->PressColor;
		n->FrameRounding = e->FrameRounding;
		n->TextAsButton = e->TextAsButton;
		n->invokeFunc = e->invokeFunc;
		n->image = e->image;
		return;
	}
	else if (wdg->type == UI_WIDGET_IMAGE)
	{
		UIWidgetImage* e = static_cast<UIWidgetImage*>(wdg);
		UIWidgetImage* n = static_cast<UIWidgetImage*>(AddUIWidget(UI_WIDGET_IMAGE));
		n->Enabled = e->Enabled;
		n->Position = e->Position;
		n->Size = e->Size;
		n->Rotation = e->Rotation;
		n->Alpha = e->Alpha;
		n->name = e->name;

		n->Color = e->Color;
		n->FrameRounding = e->FrameRounding;
		n->image = e->image;

		return;
	}
	else if (wdg->type == UI_WIDGET_PROGRESSBAR)
	{
		UIWidgetProgressBar* e = static_cast<UIWidgetProgressBar*>(wdg);
		UIWidgetProgressBar* n = static_cast<UIWidgetProgressBar*>(AddUIWidget(UI_WIDGET_PROGRESSBAR));
		n->Enabled = e->Enabled;
		n->Position = e->Position;
		n->Size = e->Size;
		n->Rotation = e->Rotation;
		n->Alpha = e->Alpha;
		n->name = e->name;

		n->Color = e->Color;
		n->BarColor = e->BarColor;
		n->Value = e->Value;
		n->RightToLeft = e->RightToLeft;
		n->FrameRounding = e->FrameRounding;
		return;
	}
	else if (wdg->type == UI_WIDGET_INPUT_TEXT)
	{
		UIWidgetInputText* e = static_cast<UIWidgetInputText*>(wdg);
		UIWidgetInputText* n = static_cast<UIWidgetInputText*>(AddUIWidget(UI_WIDGET_INPUT_TEXT));
		n->Enabled = e->Enabled;
		n->Position = e->Position;
		n->Size = e->Size;
		n->Rotation = e->Rotation;
		n->Alpha = e->Alpha;
		n->name = e->name;

		n->text = e->text;
		n->Hint = e->Hint;
		n->Scale = e->Scale;
		n->Multiline = e->Multiline;
		n->password = e->password;
		n->readOnly = e->readOnly;
		n->color = e->color;
		n->bgColor = e->bgColor;
		n->HintColor = e->HintColor;
		return;
	}

	else if (wdg->type == UI_WIDGET_LINE)
	{
		UIWidgetLine* e = static_cast<UIWidgetLine*>(wdg);
		UIWidgetLine* n = static_cast<UIWidgetLine*>(AddUIWidget(UI_WIDGET_LINE));
		n->Enabled = e->Enabled;
		n->Position = e->Position;
		n->Size = e->Size;
		n->Rotation = e->Rotation;
		n->Alpha = e->Alpha;
		n->name = e->name;

		n->VerticalLine = e->VerticalLine;
		n->Custom = e->Custom;
		n->thickness = e->thickness;
		n->Color = e->Color;
		n->Point2 = e->Point2;
		return;
	}
}
// ------------------------------------------------------------------------------------
void UIWidget::OnSave(YAML::Emitter& out)
{
	out << YAML::Key << "UIWidget" << YAML::BeginMap;

	out << YAML::Key << "enabled" << YAML::Value << enabled;
	out << YAML::Key << "Position"; RYAML::SerVec2(out, Position);
	out << YAML::Key << "Scale"; RYAML::SerVec2(out, Scale);
	out << YAML::Key << "Rotation" << YAML::Value << Rotation;
	out << YAML::Key << "color"; RYAML::SerVec4(out, color);
	out << YAML::Key << "FrameRounding" << YAML::Value << FrameRounding;
	out << YAML::Key << "Alpha" << YAML::Value << Alpha;
	out << YAML::Key << "BlurBackground" << YAML::Value << BlurBackground;
	out << YAML::Key << "wdg_count" << YAML::Value << widgets.size();

	out << YAML::Key << "Widgets" << YAML::Value << YAML::BeginSeq;
	for (size_t i = 0; i < widgets.size(); i++)
	{
		UIWidgetComponent* w = widgets[i];
		out << YAML::BeginMap;
		w->OnSave(out);
		out << YAML::EndMap;
	}
	out << YAML::EndSeq;

	out << YAML::EndMap;
}
// ------------------------------------------------------------------------------------