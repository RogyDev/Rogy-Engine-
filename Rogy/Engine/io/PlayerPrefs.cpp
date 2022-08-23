#include "PlayerPrefs.h"

// -------------------------------------------------
PlayerPrefs::PlayerPrefs()
{
}
// -------------------------------------------------
PlayerPrefs::~PlayerPrefs()
{
}
// -------------------------------------------------
void PlayerPrefs::AddString(const char * varName, std::string val)
{
	int indx = checkVar(varName);
	if (indx != -1) {
		infos[indx].str = val;
		return;
	}
	infos.emplace_back(PVarType::PPREF_VAR_STRING);
	infos[infos.size() - 1].name = varName;
	infos[infos.size() - 1].str = val;
}
// -------------------------------------------------
void PlayerPrefs::AddInt(const char * varName, int val)
{
	int indx = checkVar(varName);
	if (indx != -1) {
		infos[indx].intt = val;
		return;
	}
	infos.emplace_back(PVarType::PPREF_VAR_INT);
	infos[infos.size() - 1].name = varName;
	infos[infos.size() - 1].intt = val;
}
// -------------------------------------------------
void PlayerPrefs::AddFloat(const char * varName, float val)
{
	int indx = checkVar(varName);
	if (indx != -1) {
		infos[indx].floatt = val;
		return;
	}
	infos.emplace_back(PVarType::PPREF_VAR_FLOAT);
	infos[infos.size() - 1].name = varName;
	infos[infos.size() - 1].floatt = val;
}
// -------------------------------------------------
std::string PlayerPrefs::GetString(const char * varName)
{
	for (size_t i = 0; i < infos.size(); i++)
		if (infos[i].name == varName)
			return infos[i].str;
	return std::string();
}
// -------------------------------------------------
int PlayerPrefs::GetInt(const char * varName)
{
	for (size_t i = 0; i < infos.size(); i++)
		if (infos[i].name == varName) 
			return infos[i].intt;
	return 0;
}
// -------------------------------------------------
float PlayerPrefs::GetFloat(const char * varName)
{
	for (size_t i = 0; i < infos.size(); i++)
		if (infos[i].name == varName)
			return infos[i].floatt;
	return 0.0f;
}
// -------------------------------------------------
int PlayerPrefs::checkVar(const char * varName)
{
	for (size_t i = 0; i < infos.size(); i++)
	{
		if (infos[i].name == varName)
			return i;
	}
	return -1;
}
// -------------------------------------------------
void PlayerPrefs::Save()
{
	std::string saveP = save_path + "\\PlayerPrefs";
	std::ofstream os(saveP.c_str(), std::ios::binary);
	cereal::BinaryOutputArchive ar(os);
	ar(infos.size());
	for (size_t i = 0; i < infos.size(); i++)
	{
		ar((int)infos[i].type);
		ar(infos[i].name);
		if(infos[i].type == PPREF_VAR_STRING)
			ar(infos[i].str);
		else if (infos[i].type == PPREF_VAR_INT)
			ar(infos[i].intt);
		else if (infos[i].type == PPREF_VAR_FLOAT)
			ar(infos[i].floatt);
	}
	os.close();
}
// -------------------------------------------------
void PlayerPrefs::Load()
{
	std::filebuf fb;
	std::string loadP = save_path + "\\PlayerPrefs";
	if (fb.open(loadP.c_str(), std::ios::in))
	{
		std::istream is(&fb);
		cereal::BinaryInputArchive ar(is);
		size_t length; ar(length);
		int type; std::string var_name;
		for (size_t i = 0; i < length; i++)
		{
			ar(type);
			ar(var_name);
			if ((PVarType)type == PPREF_VAR_STRING)
			{
				std::string val; ar(val); AddString(var_name.c_str(), val);
			}
			else if ((PVarType)type == PPREF_VAR_INT)
			{
				int val; ar(val); AddInt(var_name.c_str(), val);
			}
			else if ((PVarType)type == PPREF_VAR_FLOAT)
			{
				float val; ar(val); AddFloat(var_name.c_str(), val);
			}
				
		}
		fb.close();
	}
}
// -------------------------------------------------