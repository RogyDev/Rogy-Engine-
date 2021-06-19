#include "LuaScriptSerializer.h"

// ---------------------------------------------------------------------
ScriptSerializer::ScriptSerializer()
{
}
// ---------------------------------------------------------------------
ScriptSerializer::~ScriptSerializer()
{
}
// ---------------------------------------------------------------------
void ScriptSerializer::SerializeScriptObject(YAML::Emitter& out, ScriptInstance* scr)
{
	out << YAML::Key << "Script_Properties" << YAML::BeginSeq;
	out << YAML::Flow;
	out << scr->properties.size();
	for (size_t i = 0; i < scr->properties.size(); i++)
	{
		out << scr->properties[i].name;
		out << (int)scr->properties[i].type;

		if (scr->properties[i].type == ScriptVarType::VAR_Int)
			out <<  scr->GetVarInt(scr->properties[i].name.c_str());
		else if (scr->properties[i].type == ScriptVarType::VAR_Float)
			out <<  scr->GetVarFloat(scr->properties[i].name.c_str());
		else if (scr->properties[i].type == ScriptVarType::VAR_Bool)
			out <<  scr->GetVarBool(scr->properties[i].name.c_str());
		else if (scr->properties[i].type == ScriptVarType::VAR_String)
			out << scr->GetVarString(scr->properties[i].name.c_str());
		else if (scr->properties[i].type == ScriptVarType::VAR_ASSET)
			out<< scr->GetAssetPath(scr->properties[i].name);
		else if (scr->properties[i].type == ScriptVarType::VAR_VEC3)
		{
			glm::vec3& t = scr->GetVec3(scr->properties[i].name);
			RYAML::SerVec3(out, t);
		}
	}
	out << YAML::EndSeq;
}
// ---------------------------------------------------------------------
void ScriptSerializer::LoadScriptObject(YAML::Node& out, ScriptInstance* scr)
{
	YAML::Node props = out["Script_Properties"];
	//if (out.IsSequence()) 
	{
		size_t prop_size = props[0].as<size_t>();
		std::string prp_name;
		int prp_type_indx;
		ScriptVarType prp_type;

		for (size_t i = 1; i < (prop_size )*3; i++)
		{
			prp_name = props[i].as<std::string>(); i++;
			
			prp_type_indx = props[i].as<int>(); i++;
			prp_type = (ScriptVarType)prp_type_indx;

			if (prp_type == ScriptVarType::VAR_Int)
			{
				int val = props[i].as<int>();
				if (scr->HasVar(prp_name.c_str(), prp_type))
					scr->SetVar(prp_name.c_str(), val);
			}
			else if (prp_type == ScriptVarType::VAR_Float)
			{
				float val = props[i].as<float>();
				if (scr->HasVar(prp_name.c_str(), prp_type))
					scr->SetVar(prp_name.c_str(), val);
			}
			else if (prp_type == ScriptVarType::VAR_Bool)
			{
				bool val = props[i].as<bool>();
				if (scr->HasVar(prp_name.c_str(), prp_type))
					scr->SetVar(prp_name.c_str(), val);
			}
			else if (prp_type == ScriptVarType::VAR_String)
			{
				std::string val = props[i].as<std::string>();
				if (scr->HasVar(prp_name.c_str(), prp_type))
					scr->SetVar(prp_name.c_str(), val);
			}
			else if (prp_type == ScriptVarType::VAR_ASSET)
			{
				std::string val = props[i].as<std::string>();
				if (scr->HasVar(prp_name.c_str(), prp_type))
					scr->SetAssetPath(prp_name, val);
			}
			else if (prp_type == ScriptVarType::VAR_VEC3)
			{
				glm::vec3 val = RYAML::GetVec3(props[i]);
				if (scr->HasVar(prp_name.c_str(), prp_type))
					scr->SetVec3(prp_name, val);
			}
		}
	}
}
// ---------------------------------------------------------------------