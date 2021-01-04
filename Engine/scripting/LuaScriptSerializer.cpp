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
void ScriptSerializer::SerializeScriptObject(cereal::BinaryOutputArchive& ar, ScriptInstance* scr)
{
	ar(scr->properties.size());
	for (size_t i = 0; i < scr->properties.size(); i++)
	{
		ar(scr->properties[i].name);
		ar((int)scr->properties[i].type);

		if (scr->properties[i].type == ScriptVarType::VAR_Int)
			ar(scr->GetVarInt(scr->properties[i].name.c_str()));
		if (scr->properties[i].type == ScriptVarType::VAR_Float)
			ar(scr->GetVarFloat(scr->properties[i].name.c_str()));
		if (scr->properties[i].type == ScriptVarType::VAR_Bool)
			ar(scr->GetVarBool(scr->properties[i].name.c_str()));
		if (scr->properties[i].type == ScriptVarType::VAR_String)
			ar(scr->GetVarString(scr->properties[i].name.c_str()));
	}
}
// ---------------------------------------------------------------------
void ScriptSerializer::LoadScriptObject(cereal::BinaryInputArchive& ar, ScriptInstance* scr)
{
	size_t prp_count = 0;
	ar(prp_count);
	for (size_t i = 0; i < prp_count; i++)
	{
		std::string prp_name;
		int prp_type_indx;
		ScriptVarType prp_type;
		ar(prp_name);
		ar(prp_type_indx);
		prp_type = (ScriptVarType)prp_type_indx;
		
		if (prp_type == ScriptVarType::VAR_Int)
		{
			int val; ar(val);
			if (scr->HasVar(prp_name.c_str(), prp_type))
				scr->SetVar(prp_name.c_str(), val);
		}
		else if (prp_type == ScriptVarType::VAR_Float)
		{
			float val; ar(val);
			if (scr->HasVar(prp_name.c_str(), prp_type)) 
				scr->SetVar(prp_name.c_str(), val);
		}
		else if (prp_type == ScriptVarType::VAR_Bool)
		{
			bool val; ar(val);
			if (scr->HasVar(prp_name.c_str(), prp_type))
				scr->SetVar(prp_name.c_str(), val);
		}
		else if (prp_type == ScriptVarType::VAR_String)
		{
			std::string val; ar(val);
			if (scr->HasVar(prp_name.c_str(), prp_type))
				scr->SetVar(prp_name.c_str(), val);
		}
	}
}
// ---------------------------------------------------------------------