#ifndef ROGY_PLAYER_PREFS_H
#define ROGY_PLAYER_PREFS_H

#include <string>
#include <glm\glm.hpp>
#include <vector>
#include "FSerializer.h"

// Class for saving player informrmation
// -------------------------------------------------------
class PlayerPrefs
{
	enum PVarType
	{
		PPREF_VAR_STRING,
		PPREF_VAR_INT,
		PPREF_VAR_FLOAT,
	};
	struct pVal
	{
		PVarType type;
		std::string name;
		std::string str;
		int intt;
		float floatt;
		pVal(PVarType t)
		{
			type = t;
		}
	};
public:
	PlayerPrefs();
	~PlayerPrefs();
	
	void AddString(const char* varName, std::string val);
	void AddInt(const char* varName, int val);
	void AddFloat(const char* varName, float val);

	std::string GetString(const char* varName);
	int GetInt(const char* varName);
	float GetFloat(const char* varName);

	int checkVar(const char* varName);
	void Save();
	void Load();

	std::string save_path = "core\\PlayerPrefs";
private:
	std::vector<pVal> infos;
};


#endif // ! ROGY_PLAYER_PREFS_H