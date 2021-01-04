#ifndef R_SCRIPTM_SYSTEM_H
#define R_SCRIPTM_SYSTEM_H

//#include <RTools.h>

#include "LuaScriptObject.h"

#include <LuaBridge\LuaBridge.h>
#include <fstream>
#include <sstream>

#include "../io/debug_tools.h"

#include <dirent.h>
using namespace luabridge;

struct RScriptInfo
{
	std::string file_path;
	std::string class_name;
	bool valide = false;

	RScriptInfo(std::string path, std::string name)
	{
		file_path = path;
		class_name = name;
	}
};

class ScriptManager
{
public:
	DebugTool* debug;
	ScriptManager();
	~ScriptManager();
	//Entity et;
	std::vector<ScriptInstance*> m_Instances;
	std::vector<RScriptInfo> m_loaded_scripts;

	void Init();
	void LoadMainScripts();
	void Close();
	void Update();
	void GetScriptsInPath(const char* path);

	// Script methods
	void BeginGame();
	void OnTick(float dt);

	// Classes instances control
	ScriptInstance* InstanceClass(const char* class_name);
	void DeleteInstance(int iid);
	void ClearInstances();

	void ReloadLuaInterpreter();
	void RecompileScripts(bool auto_reinstance_object = true);
	void ReInstanceObjects();

	ScriptInstance* InstanceComponentClass(unsigned int ent, const char* class_name);

	// Loads lua script
	bool AddLuaFile(const char* path, const char* class_name);
	bool LoadLuaFile(const char* path, const char* class_name);
	
	lua_State *L = nullptr;
	bool RecompilingScripts = false;

private:
	bool initialized = false;

	void ReplaceSTR(std::string& str, const std::string& oldStr, const std::string& newStr);
};

#endif // ! R_SCRIPT_SYSTEM_H
