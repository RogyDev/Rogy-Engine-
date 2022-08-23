#include "debug_editor.h"
// --------------------------------------------------------
Editor_Debug::Editor_Debug()
{
}
// --------------------------------------------------------
Editor_Debug::~Editor_Debug()
{
}
// --------------------------------------------------------
void Editor_Debug::Render()
{
	if (debuger->hasMessages)
	{
		is_open = true;
		debuger->hasMessages = false;
	}

	if (!is_open) return;

	ImGui::Begin("Console", &is_open, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::Button("Clear"))
			debuger->Clear();

		ImGui::EndMenuBar();
	}
	size_t msgCount = debuger->messages.size();
	ImGui::Text(std::to_string(msgCount).c_str());

	if (debuger == nullptr)
	{
		ImGui::Text("Debuger not found!");
		ImGui::End();
		return;
	}
	ImGui::BeginChild("#dddBBTOOLS");

	ImGui::Separator();
	for (size_t i = 0; i < msgCount; i++)
	{
		if (debuger->messages[i].type == LT_Message)
		{

			ImGui::Selectable(std::string("Log : " + debuger->messages[i].msg).c_str());
		}
		else if (debuger->messages[i].type == LT_Warning)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 0.8f));
			ImGui::Selectable(std::string("Warning : " + debuger->messages[i].msg).c_str());
			ImGui::PopStyleColor();
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 0.8f));
			ImGui::Selectable(std::string("Error : " + debuger->messages[i].msg).c_str());
			ImGui::PopStyleColor();
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(debuger->messages[i].msg.c_str());
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
		ImGui::Separator();
	}
	ImGui::EndChild();
	ImGui::End();
}
// --------------------------------------------------------