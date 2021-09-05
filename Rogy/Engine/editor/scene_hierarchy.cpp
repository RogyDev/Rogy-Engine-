#include "scene_hierarchy.h"

#include "RTools.h"

Scene_hierarchy::Scene_hierarchy()
{
}

Scene_hierarchy::~Scene_hierarchy()
{
}

void Scene_hierarchy::SetSelection(EnttID eid)
{
	sel_entt.clear();
	sel_entt.push_back(eid);
}

bool Scene_hierarchy::Empty()
{
	return sel_entt.empty();
}

EnttID Scene_hierarchy::GetSelected()
{
	if (Empty())
		return -1;
	return sel_entt[0];
}
void Scene_hierarchy::LoopInChild(Entity* child)
{
	child->is_Selected = false;

	bool itemClick = false;
	EnttID cel_item = 0;

	bool hasChilds = child->ChildCount() > 0;
	ImGuiTreeNodeFlags c_flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
	if (!hasChilds)
	{
		c_flags |= ImGuiTreeNodeFlags_Leaf;
	}

	if (sel_entt.size() > 0)
	{
		for (size_t i = 0; i < sel_entt.size(); i++)
		{
			if (sel_entt[i] == child->ID)
			{
				c_flags |= ImGuiTreeNodeFlags_Selected;
				child->is_Selected = true;
			}
		}
	}
	//if (child->ID != -1 && child->IsPrefab())
	//{
		//ImGui::PushFont(icon_small); ImGui::Text("4"); ImGui::PopFont(); ImGui::SameLine();
	//}

	if (child->ID != -1 && child->IsPrefab()) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.8f, 1.0f, 1.0f));
		
	bool is_ON = ImGui::TreeNodeEx((void*)(intptr_t)child->ID, c_flags, child->name.c_str());

	if (child->ID != -1 && child->IsPrefab()) ImGui::PopStyleColor();
	

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload("DND_DEMO_CELL", &child->ID, sizeof(EnttID));
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
		{
			IM_ASSERT(payload->DataSize == sizeof(EnttID));
			EnttID payload_n = *(const int*)payload->Data;
			scene->SetEntityParent(payload_n, child->ID);
		}
		ImGui::EndDragDropTarget();
	}

	/*if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
	{
		int n_next = (ImGui::GetMouseDragDelta(0).y < 0.5f ? -1 : 1);
		if (n_next == 1)
		{
			child->MovePlace(true);
			ImGui::ResetMouseDragDelta();
		}
		else if(n_next == -1)
		{
			child->MovePlace(false);
			ImGui::ResetMouseDragDelta();
		}
	}*/
	if (ImGui::IsItemClicked(1))
	{
		ent_ed_popup = true;
	}
	
	bool is_open_c = ImGui::IsItemClicked();
	if(is_ON)
	{
		if (hasChilds)
		{
			for (size_t i = 0; i < child->ChildCount(); i++)
			{
				LoopInChild(child->Children[i]);
			}
		}
		ImGui::TreePop();
	}
	
	if (is_open_c)
	{
		cel_item = child->ID;

		itemClick = true;
	}
	c_flags = 0;

	
	if (itemClick)
	{
		if (ImGui::GetIO().KeyCtrl) {
			if(!child->root)
				sel_entt.push_back(cel_item);
		}
		else
		{
			sel_entt.clear();
			if (!child->root)
				sel_entt.push_back(cel_item);
		}
	}
}

void Scene_hierarchy::SaveACopy()
{
	Entity* target = scene->FindEntity(sel_entt[0]);
	if (target != nullptr)
	{
		copyParentID = target->parent->ID;
		target->SaveEntityFile("savedCopy", true);
		//std::ofstream os("savedCopy", std::ios::binary);
		//cereal::BinaryOutputArchive archive(os);
		//target->SaveBP(archive, true);
		//os.close();
	}
}
void Scene_hierarchy::PastACopy()
{
	scene->PushRequest(SR_PAST_COPY);
}
void Scene_hierarchy::Render()
{
	ImGui::Begin("Scene", NULL, ImGuiWindowFlags_NoCollapse);// | ImGuiWindowFlags_MenuBar);
	ImGui::BeginChild("scn_Child");
	ImGui::PushFont(icon_small);
	if (ImGui::Button("X")) // Add new entity
		ImGui::OpenPopup("scene_edit_menu");
	ImGui::SameLine();
	if (ImGui::MenuItem("T")) // Diselect
		sel_entt.clear();
	ImGui::Separator();

	/*if (ImGui::BeginMenuBar())
	{
		if (ImGui::Button("X")) // Add new entity
			ImGui::OpenPopup("scene_edit_menu");

		if (ImGui::MenuItem("T")) // Diselect
			sel_entt.clear();

		if (ImGui::MenuItem("3")) // Save Scene
		{ }

		ImGui::EndMenuBar();
	}*/
	ImGui::PopFont();

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	LoopInChild(&scene->Root);


	if (ImGui::BeginPopup("scene_edit_menu"))
	{
		if (ImGui::Selectable("New Entity")) // Add new entity
			scene->AddEntity(-1, "New Entity");

		if (ImGui::Selectable("Load Entity")) // load entity
			ImGuiFileDialog::Instance()->OpenModal("ChooseBPModel", "Select Prefab", ".rbp\0", ".");

		ImGui::EndPopup();
	}
	if (ent_ed_popup == true) {
		ImGui::OpenPopup("entity_edit_menu");
		ent_ed_popup = false;
	}
	// Shortcuts
	if (input->GetKey(RKey::KEY_LCTRL)) {
		if (input->GetKeyDown(RKey::KEY_N)) 
			scene->NewScene();
		else if (input->GetKeyDown(RKey::KEY_S))
		{
			if (scene->path == "")
				ImGuiFileDialog::Instance()->OpenModal("SaveScnModel", "Save Scene", ".rscn\0", ".");
			else scene->SaveScene(scene->path);
		}
		else if (input->GetKeyDown(RKey::KEY_O))
			ImGuiFileDialog::Instance()->OpenModal("LoadScnModel", "Open Scene", ".rscn\0", ".");
	}
	if (ImGui::IsWindowHovered())
	{
		if (!sel_entt.empty()) {
			if (input->GetKey(RKey::KEY_LCTRL)) {
				if (input->GetKeyDown(RKey::KEY_C))
					SaveACopy();
				else if (input->GetKeyDown(RKey::KEY_V))
					PastACopy();
				else if (input->GetKeyDown(RKey::KEY_D))
				{
					SaveACopy();
					PastACopy();
				}
				else if (input->GetKeyDown(RKey::KEY_UP))
					scene->FindEntity(sel_entt[0])->MovePlace(true);
				else if (input->GetKeyDown(RKey::KEY_DOWN))
					scene->FindEntity(sel_entt[0])->MovePlace(false);
				else if (input->GetKeyDown(RKey::KEY_X))
					scene->AddEntity(sel_entt[0], "New Entity");
			}
			if (input->GetKeyDown(RKey::KEY_DELETE))
			{
				for (size_t i = 0; i < sel_entt.size(); i++)
					scene->RemoveEntity(sel_entt[i]);
				sel_entt.clear();
			}
		}
		else if (input->GetKey(RKey::KEY_LCTRL) && input->GetKeyDown(RKey::KEY_X))
					scene->AddEntity("New Entity");
	}
	if (ImGui::BeginPopup("entity_edit_menu"))
	{
			if (ImGui::Selectable("New Child")) // Add new entity child
				scene->AddEntity(sel_entt[0], "New Entity");
			if (ImGui::Selectable("Move Up")) // Move Selected entity up
				scene->FindEntity(sel_entt[0])->MovePlace(true);
			if (ImGui::Selectable("Move Down")) // Move Selected entity down
				scene->FindEntity(sel_entt[0])->MovePlace(false);

			if (ImGui::Selectable("Copy"))
				SaveACopy();
			if (ImGui::Selectable("Past"))
				PastACopy();

			if (ImGui::Selectable("Delete"))  // Remove Selected entities
			{
				for (size_t i = 0; i < sel_entt.size(); i++)
				{
					scene->RemoveEntity(sel_entt[i]);
				}
				sel_entt.clear();
			}
			if (ImGui::Selectable("Unpack entity"))
			{
				scene->FindEntity(sel_entt[0])->Unpack();
			}
		ImGui::EndPopup();
	}
	ImGui::EndChild();
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RES_FILE_RBP"))
		{
			IM_ASSERT(payload->DataSize == sizeof(std::string));
			std::string payload_n = *(std::string*)payload->Data;
			scene->SpawnEntity(payload_n);
		}
		else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RES_FILE_SCENE"))
		{
			IM_ASSERT(payload->DataSize == sizeof(std::string));
			std::string payload_n = *(std::string*)payload->Data;
			std::cout << "Droped " << payload_n << std::endl;
			scene->LoadScene(payload_n);
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::End();
	

	if (ImGuiFileDialog::Instance()->FileDialog("ChooseBPModel"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk == true)
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
			RGetRelativePath(filePathName);
			// action
			scene->SpawnEntity(filePathName);
		}
		// close
		ImGuiFileDialog::Instance()->CloseDialog("ChooseBPModel");
	}
}