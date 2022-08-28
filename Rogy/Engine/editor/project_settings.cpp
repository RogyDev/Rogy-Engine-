#include "project_settings.h"

Project_Settings::Project_Settings()
{
}

Project_Settings::~Project_Settings()
{
}

void Project_Settings::Render()
{
	if (!isOn) return;

	ImGui::Begin("Project Settings", &isOn, ImGuiWindowFlags_MenuBar);

	if (ImGui::CollapsingHeader("Project", ImGuiTreeNodeFlags_DefaultOpen))
	{
		EditorProperty::BeginPreps("edit_proj_settings");
		EditorProperty::PrepName("GameName");
		EditorProperty::PrepName("CreatorName");
		EditorProperty::PrepName("Version");
		EditorProperty::PrepName("Main Scene");

		EditorProperty::NextPreps();
		EditorProperty::SetWeightPrep(); ImGui::InputText("##changeGame_name", &prj->GameName);
		EditorProperty::SetWeightPrep(); ImGui::InputText("##changeCreatorName", &prj->CreatorName);
		EditorProperty::SetWeightPrep(); ImGui::InputText("##changeVersionName", &prj->Version);
		if (ImGui::Button("Set ##MainscenePath"))
			ImGuiFileDialog::Instance()->OpenModal("ChooseFileMainScene", "Choose Main Scene", ".rscn", ".");
		ImGui::SameLine();
		if (ImGui::Button("Clear ##MainscenePath"))
			prj->MainScenePath = "";
		ImGui::SameLine(); ImGui::Text(prj->MainScenePath.c_str());

		EditorProperty::EndPreps();
		ImGui::Separator();
	}

	if (ImGui::CollapsingHeader("Window", ImGuiTreeNodeFlags_DefaultOpen))
	{
		EditorProperty::BeginPreps("edit_proj_window_settings");
		EditorProperty::PrepName("Default Fullscreen");
		EditorProperty::PrepName("Resolution Weight");
		EditorProperty::PrepName("Resolution Height");

		EditorProperty::NextPreps();
		ImGui::Checkbox("##defautlFs", &prj->DefaultFullScreen);
		EditorProperty::SetWeightPrep(); ImGui::InputInt("##changeSW", &prj->ResolutionWeight);
		EditorProperty::SetWeightPrep(); ImGui::InputInt("##changeSH", &prj->ResolutionHeight);
		EditorProperty::EndPreps();
		ImGui::Separator();
	}

	if (ImGui::CollapsingHeader("Quality", ImGuiTreeNodeFlags_DefaultOpen))
	{
		EditorProperty::BeginPreps("edit_proj_settings_q");
		//EditorProperty::PrepName("Sky Capture Resulotion");
		EditorProperty::PrepName("Use Instancing");
		EditorProperty::PrepName("Use Instancing for shadows");
		EditorProperty::PrepName("Use Depth Pre-Pass");

		EditorProperty::PrepName("EnableShadows");
		EditorProperty::PrepName("CascadesCount");
		EditorProperty::PrepName("PointShadowResolution");
		EditorProperty::PrepName("SpotShadowsResolution");
		EditorProperty::PrepName("Spot Shadows Limit");

		ImGui::Text("");
		EditorProperty::PrepName("Cascaded directional shadows");
		EditorProperty::PrepName("Shadows Distance");
		EditorProperty::PrepName("Cascaded Shadows Resolution");
		EditorProperty::PrepName("Cascade Split 1");
		EditorProperty::PrepName("Cascade Split 2");
		EditorProperty::PrepName("Cascade Split 3");

		EditorProperty::NextPreps();
		/*int captureRes = rndr->GetCaptureResolution();
		switch (captureRes)
		{
		case 64: captureRes = 0; break;
		case 128: captureRes = 1; break;
		case 256: captureRes = 2; break;
		case 512: captureRes = 3; break;
		case 1024: captureRes = 4; break;
		default: captureRes = 1; break;
		}
		//EditorProperty::SetWeightPrep(); ImGui::DragInt("##chagneSCRes", &prj->SkyCaptureResulotion, 1, 16, 4096);
		EditorProperty::SetWeightPrep(); ImGui::Combo("##ress", &captureRes, "_64\0_128\0_256\0_512\0_1024");
		//captureRes++;
		int capture_res = 64;
		for (size_t i = 0; i < captureRes; i++)
			capture_res *= 2;
		if (capture_res != rndr->GetCaptureResolution())
		{
			std::cout << "Reflection Capture Resolution: " << capture_res << std::endl;
			rndr->SetCaptureResolution(capture_res);
		}
		*/
		ImGui::Checkbox("##UseInstancing", &rndr->UseInstancing);
		ImGui::Checkbox("##UseInstancingForShadows", &rndr->UseInstancingForShadows);
		ImGui::Checkbox("##DepthPrePass", &rndr->DepthPrePass);

		ImGui::Checkbox("##EnableShadowsq", &prj->EnableShadows);
		EditorProperty::SetWeightPrep(); ImGui::DragInt("##changeCascadesCount", &prj->CascadesCount, 1, 1, 3);
		EditorProperty::SetWeightPrep(); ImGui::DragInt("##changePointShadowResolution", &prj->PointShadowResolution);
		EditorProperty::SetWeightPrep(); ImGui::DragInt("##changeSpotShadowsResolution", &prj->SpotShadowsResolution);
		int seted_mode = 0;
		if (prj->SpotShadowsLimit == 9)
			seted_mode = 1;
		else if (prj->SpotShadowsLimit == 16)
			seted_mode = 2;
		EditorProperty::SetWeightPrep(); ImGui::Combo("##modeds_type", &seted_mode, "_4\0_9\0_16");

		if (seted_mode == 0)
			prj->SpotShadowsLimit = 4;
		else if (seted_mode == 1)
			prj->SpotShadowsLimit = 9;
		else if (seted_mode == 2)
			prj->SpotShadowsLimit = 16;
		
		ImGui::Text("\n\n");
		EditorProperty::SetWeightPrep(); ImGui::DragFloat("##changeShadowDistance", &prj->ShadowDistance);
		EditorProperty::SetWeightPrep(); ImGui::DragInt("##changCascadedShadowMapsResolutione", &prj->CascadedShadowMapsResolution);
		EditorProperty::SetWeightPrep(); ImGui::DragFloat("##changeCascade Split 1", &prj->CascadeSplits[0], 0.01f, 0.1f, 1.0f);
		EditorProperty::SetWeightPrep(); ImGui::DragFloat("##changeCascade Split 2", &prj->CascadeSplits[1], 0.01f, 0.1f, 1.0f);
		EditorProperty::SetWeightPrep(); ImGui::DragFloat("##changeCascade Split 3", &prj->CascadeSplits[2], 0.01f, 0.1f, 1.0f);
		EditorProperty::SetWeightPrep(); ImGui::DragInt("##changeCascade Split 33", &rndr->csm);
		EditorProperty::EndPreps();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth());
		if (ImGui::Button("Apply and Save"))
		{
			rndr->m_ShadowMapper.SHADOW_MAP_CASCADE_COUNT = prj->CascadesCount;
			rndr->m_ShadowMapper.SetShadowDistance((size_t)prj->ShadowDistance);
			rndr->m_ShadowMapper.SetCascadesResolution(prj->CascadedShadowMapsResolution);
			rndr->m_PointShadowMapper.ResetShadowResolution(prj->PointShadowResolution);
			rndr->m_SpotShadowMapper.ResetShadowResolution(prj->SpotShadowsResolution);

			rndr->BakeGlobalLightData();
			rndr->BakeAllRefProbs();
		}
		ImGui::Separator();
	}

	if (ImGui::CollapsingHeader("Input", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Add Axis ##addAxis"))
			ImGui::OpenPopup("AddAxis ##addaxisPopup");

		for (size_t i = 0; i < input->GetAxisCount(); i++)
		{
			ImGui::Separator();
			std::string axisName = input->GetAllAxis()[i].name;
			std::string uniqueId = "Remove ##removeAxis"; uniqueId += axisName;
			std::string uniqueId2 = "Edit ##removeAxis"; uniqueId2 += axisName;
			if (ImGui::Button(uniqueId.c_str()))
			{
				input->RemoveAxis(axisName);
			}
			ImGui::SameLine(); if (ImGui::Button(uniqueId2.c_str()))
			{
				renameAxisIndex = i;
				ImGui::OpenPopup("Rename axis ##addaxisPopupRename");
			}
			ImGui::SameLine(); ImGui::Text(axisName.c_str());
			ImGui::SameLine(); ImGui::Text(" |1 ");
			ImGui::SameLine(); ImGui::Text(RKey::KeyToString(input->GetAllAxis()[i].key_up).c_str());
			ImGui::SameLine(); ImGui::Text(" |-1 ");
			ImGui::SameLine(); ImGui::Text(RKey::KeyToString(input->GetAllAxis()[i].key_down).c_str());
		}
		ImGui::Separator();
	}

	if (ImGui::CollapsingHeader("UI Fonts", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Add Font ##addAxis"))
			ImGui::OpenPopup("Add Font ##addaxisPopup");

		/*for (size_t i = 0; i < ui->fonts.size(); i++)
		{
			ImGui::Text(std::to_string(i + 1).c_str());
			ImGui::SameLine();
			ImGui::Text(ui->fonts[i].source.c_str());
		}*/
	}

	if (ImGui::BeginPopupModal("Add Font ##addaxisPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::InputText("Font path ##UIWidget_fontadd", &aNewfont);
		ImGui::DragInt("Font Size ##UIWidget_fontadd", &aNewfontSize);
		if (ImGui::Button("Cancel ##acfont"))
			ImGui::CloseCurrentPopup();
		ImGui::SameLine(); if (ImGui::Button("Add ##acfont"))
		{
			loadFont = aNewfont;
			lfs = aNewfontSize;
			//ui->LoadFont(aNewfont, aNewfontSize);
			aNewfont = "";
			aNewfontSize = 50;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("AddAxis ##addaxisPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (editfirstKey || editsecondKey) {
			int newKey = input->IsAnyKeyDown();
			if (editfirstKey)
				firstKey = newKey;
			if (editsecondKey)
				secondKey = newKey;

			if (newKey != -1) {
				editfirstKey = false;
				editsecondKey = false;
			}
		}

		ImGui::InputText("##changeAxisNmaeokid", &aNewAxis);
		if (ImGui::Button("KeyUp ##setKeyUp"))
		{
			editfirstKey = true;
			editsecondKey = false;
		}
		if (editfirstKey)
		{
			ImGui::SameLine();
			ImGui::Text("Press any key..");
		}
		ImGui::SameLine();	ImGui::Text(RKey::KeyToString(firstKey).c_str());
		if (ImGui::Button("KeyDown ##setKeyDown"))
		{
			editfirstKey = false;
			editsecondKey = true;
		}
		if (editsecondKey)
		{
			ImGui::SameLine();
			ImGui::Text("Press any key..");
		}
		ImGui::SameLine(); 	ImGui::Text(RKey::KeyToString(secondKey).c_str());
		if (ImGui::Button("Cancel ##cAxis"))
		{
			editfirstKey = false;
			editsecondKey = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine(); if (ImGui::Button("Add ##adAxis"))
		{
			editfirstKey = false;
			editsecondKey = false;
			input->AddAxis(aNewAxis, firstKey, secondKey);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("Rename axis ##addaxisPopupRename", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::InputText("##changeAxisNmaeokid", &input->GetAllAxis()[renameAxisIndex].name);
		ImGui::Text("Speed "); ImGui::SameLine();
		ImGui::DragFloat("##dragspeed", &input->GetAllAxis()[renameAxisIndex].speed);
		ImGui::SameLine(); if (ImGui::Button("OK ##okrename"))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}

	ImGui::End();

	if (ImGuiFileDialog::Instance()->FileDialog("ChooseFileMainScene"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk == true)
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
			RGetRelativePath(filePathName);
			prj->MainScenePath = filePathName;
		}
		// close
		ImGuiFileDialog::Instance()->CloseDialog("ChooseFileMainScene");
	}
}


