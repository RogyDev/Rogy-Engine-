#include "ScriptManager.h"

// --------------------------------------------
ScriptManager::ScriptManager()
{
}
// --------------------------------------------
ScriptManager::~ScriptManager()
{
}
// --------------------------------------------
void ScriptManager::ReplaceSTR(std::string& str, const std::string& oldStr, const std::string& newStr)
{
	std::string::size_type pos = 0u;
	while ((pos = str.find(oldStr, pos)) != std::string::npos)
	{
		str.replace(pos, oldStr.length(), newStr);
		pos += newStr.length();
	}
}
// --------------------------------------------
void ScriptManager::Close()
{
	if (!initialized) return;

	ClearInstances();

	// Close lua
	lua_close(L);
}
// --------------------------------------------
void ScriptManager::ClearInstances()
{
	for (size_t i = 0; i < m_Instances.size(); i++)
	{
		m_Instances[i]->DeleteObject();
		delete m_Instances[i];
	}
	m_Instances.clear();
}
// --------------------------------------------
void Lua_LoadScript(lua_State *L, const char* path)
{
	if (luaL_loadfile(L, path))
	{
		std::cerr << "Failed To load lua class type" << std::endl;
		std::cerr << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}

	if (lua_pcall(L, 0, LUA_MULTRET, 0)) {
		std::cerr << "Failed To load lua class type" << std::endl;
		std::cerr << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
}

// --------------------------------------------
void printMessage(const std::string& s) {
	std::cout << "Script : " << s << std::endl;
}
// --------------------------------------------
void ScriptManager::Init()
{
	L = luaL_newstate();
	luaL_openlibs(L);
	luaopen_io(L);
}
// --------------------------------------------
void ScriptManager::LoadMainScripts()
{
	Lua_LoadScript(L, "core\\scripts\\class.lua");
	Lua_LoadScript(L, "core\\scripts\\ScriptComponent.lua");
}
// --------------------------------------------
bool ScriptManager::AddLuaFile(const char* path, const char* class_name)
{
	for (size_t i = 0; i < m_loaded_scripts.size(); i++)
	{
		if (m_loaded_scripts[i].file_path.c_str() == path)
		{
			std::cout << "ScriptManager : Script already added! -> " << path << std::endl;
			return false;
		}
	}
	m_loaded_scripts.push_back(RScriptInfo(path, class_name));
	m_loaded_scripts[m_loaded_scripts.size() - 1].valide = false;
	return true;
}
// --------------------------------------------
void ScriptManager::GetScriptsInPath(const char* path, bool first)
{
	//std::cout << path << std::endl;
	if(first)
		m_loaded_scripts.clear();

	struct dirent **files;

	int n = scandir(path, &files, nullptr, alphasort);
	if (n > 0)
	{
		for (int i = 0; i < n; i++)
		{
			if (files[i]->d_type == DT_DIR && files[i]->d_name[0] != '.')
			{
				std::string apath = path;
				apath += "\\";
				apath += files[i]->d_name;
				GetScriptsInPath(apath.c_str());
			}
			else if (files[i]->d_type == DT_REG)
			{
				std::string fname = files[i]->d_name;
				
				int point_pos = 0;
				for (size_t i = fname.length() - 1; i > 0; i--)
				{
					if (fname[i] == '.')
					{
						point_pos = i;
						break;
					}
				}
				std::string tp = fname.substr(point_pos + 1, std::string::npos);
				fname = fname.substr(0, point_pos);
				
				if (tp == "lua" || tp == "LUA")
				{ 
					tp = path; tp += "\\"; tp += files[i]->d_name;
					AddLuaFile(tp.c_str(), fname.c_str());
					//std::cout << "script : " << tp << std::endl;
				}
			}
			free(files[i]);
		}
		free(files);
	}
}
// --------------------------------------------
bool ScriptManager::LoadLuaFile(const char* path, const char* class_name)
{
	std::cout << "Compiling script -> " << class_name  << " | " << path << std::endl;

	if (!RecompilingScripts)
	{
		for (size_t i = 0; i < m_loaded_scripts.size(); i++)
		{
			if (m_loaded_scripts[i].file_path.c_str() == path)
			{
				std::cout << "ScriptManager : Script already loaded! -> " << path << std::endl;
				return true;
			}
		}
	}
	// script validation
	bool svalid = true;

	// Load script from path
	std::ifstream infile(path);
	std::stringstream strs;
	strs << infile.rdbuf();
	std::string str = strs.str();
	infile.close();
	// Preprocess script
	//ReplaceSTR(str, "//", "--");
	ReplaceSTR(str, "!=", "~=");
	ReplaceSTR(str, "@:", "self:");
	//ReplaceSTR(str, "::", "self:");
	ReplaceSTR(str, "@", "self.");
	ReplaceSTR(str, "else if", "elseif");
	//ReplaceSTR(str, ")\n{", ")\n");
	//ReplaceSTR(str, "};\n", "end");

	// Compile (print errors if found)
	if (luaL_loadstring(L, str.c_str()))
	{
		svalid = false;
		//std::cerr << "Something went wrong loading the chunk (syntax error?)" << std::endl;
		std::cout << "error : ";
		std::cout << lua_tostring(L, -1) << std::endl;
		debug->Error(lua_tostring(L, -1));
		lua_pop(L, 1);
	}
	if (svalid && lua_pcall(L, 0, LUA_MULTRET, 0)) {
		svalid = false;
		//std::cerr << "Something went wrong during execution" << std::endl;
		std::cout << "(execution) error : ";
		std::cout << lua_tostring(L, -1) << std::endl;
		debug->Error(lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	// Register class name
	//bool sfound = false;
	if (svalid)
	{
		bool found = false;
		for (size_t i = 0; i < m_loaded_scripts.size(); i++)
		{
			if (m_loaded_scripts[i].file_path.c_str() == path)
			{
				m_loaded_scripts[i].valide = true;
				found = true;
				return true;
			}
		}
		if (!found)
		{
			m_loaded_scripts.push_back(RScriptInfo(path, class_name));
			m_loaded_scripts[m_loaded_scripts.size() - 1].valide = true;
		}
	}
	else // remove script as unloaded if failed to compile
	{
		for (size_t i = 0; i < m_loaded_scripts.size(); i++)
		{
			if (m_loaded_scripts[i].file_path.c_str() == path)
			{
				//m_loaded_scripts.erase(m_loaded_scripts.begin() + i);
				m_loaded_scripts[i].valide = false;
			}
		}
	}

	return svalid;
}
// --------------------------------------------
ScriptInstance* ScriptManager::InstanceComponentClass(unsigned int ent, const char* class_name)
{
	ScriptInstance* script = InstanceClass(class_name);
	script->ent_id = ent;
	script->PrepareMethod("_SetupTheScriptComponent");
	lua_pushnumber(L, ent);
	script->CallMethod(1);
	
	return script;
}
// --------------------------------------------
void ScriptManager::ReloadLuaInterpreter()
{
	RecompilingScripts = true;
	if (initialized)
	{
		for (size_t i = 0; i < m_Instances.size(); i++)
		{
			m_Instances[i]->DeleteObject();
		}
		lua_close(L); // Close lua
	}
	Init(); // reinitialize lua
}
void ScriptManager::RecompileScripts(bool auto_reinstance_object)
{	
	for (size_t i = 0; i < m_Instances.size(); i++)
		m_Instances[i]->CacheVarsValues();

	LoadMainScripts();
	GetScriptsInPath(ProjectResourcesFolder.c_str(), true);
	std::cout << "-------------------------- Compiling scripts ------------------------" << std::endl;
	// recompile scripts
	for (size_t i = 0; i < m_loaded_scripts.size(); i++)
	{
		LoadLuaFile(m_loaded_scripts[i].file_path.c_str(), m_loaded_scripts[i].class_name.c_str());
	}
	std::cout << "-------------------------- Finished Compiling ------------------------" << std::endl;
	// reinstance objects
	if (auto_reinstance_object)
		ReInstanceObjects();

	RecompilingScripts = false;
}
// --------------------------------------------
void ScriptManager::ReInstanceObjects()
{
	for (size_t i = 0; i < m_Instances.size(); i++)
	{
		m_Instances[i]->L = L;
		m_Instances[i]->CreateObject(m_Instances[i]->class_name.c_str());
		m_Instances[i]->ResetVarsValues();
	}
}
// --------------------------------------------
ScriptInstance* ScriptManager::InstanceClass(const char* class_name)
{
	bool class_found = false;
	for (size_t i = 0; i < m_loaded_scripts.size(); i++)
	{
		if (m_loaded_scripts[i].class_name == class_name)
		{
			class_found = true;
			break;
		}
	}
	if (!class_found) return nullptr;


	ScriptInstance* ins = new ScriptInstance(L, class_name);
	if (!ins->usable) // failed to create instance
	{
		ins->DeleteObject();
		delete ins;
		return nullptr;
	}

	m_Instances.push_back(ins);

	return ins;
}
// --------------------------------------------
void ScriptManager::DeleteInstance(int iid)
{
	for (size_t i = 0; i < m_Instances.size(); i++)
	{
		if (m_Instances[i]->ref_idx == iid)
		{
			m_Instances[i]->DeleteObject();
			delete m_Instances[i];
			m_Instances.erase(m_Instances.begin() + i);
		}
	}
}
// --------------------------------------------
void ScriptManager::Update()
{
	for (size_t i = 0; i < m_Instances.size(); i++)
	{
		if (m_Instances[i]->GC == true)
		{
			//DeleteInstance(m_Instances[i]->ref_idx);
			m_Instances[i]->DeleteObject();
			delete m_Instances[i];
			m_Instances.erase(m_Instances.begin() + i);
		}
	}
}
// --------------------------------------------
void ScriptManager::BeginGame()
{
	std::string aerr;
	for (size_t i = 0; i < m_Instances.size(); i++)
	{
		if (!m_Instances[i]->hasStart) continue;

		m_Instances[i]->CallMethod("OnStart");
		aerr = m_Instances[i]->GetLastError();
		if (aerr != "")
			debug->Error(aerr);
	}
}
// --------------------------------------------
void ScriptManager::OnTick(float dt)
{
	std::string aerr;
	for (size_t i = 0; i < m_Instances.size(); i++)
	{
		if (!m_Instances[i]->hasUpdate) continue;

		m_Instances[i]->PrepareMethod("OnUpdate");
		lua_pushnumber(L, dt);
		m_Instances[i]->CallMethod(1);

		aerr = m_Instances[i]->GetLastError();
		if (aerr != "")
			debug->Error(aerr);
	}
}
// --------------------------------------------
void ScriptManager::OnPreTick(float dt)
{
	std::string aerr;
	for (size_t i = 0; i < m_Instances.size(); i++)
	{
		if (!m_Instances[i]->hasPreUpdate) continue;

		m_Instances[i]->PrepareMethod("OnPreUpdate");
		lua_pushnumber(L, dt);
		m_Instances[i]->CallMethod(1);

		aerr = m_Instances[i]->GetLastError();
		if (aerr != "")
			debug->Error(aerr);
	}
}
// --------------------------------------------
void ScriptManager::OnPhyTick(float dt)
{
	std::string aerr;
	for (size_t i = 0; i < m_Instances.size(); i++)
	{
		if (!m_Instances[i]->hasPhysicsUpdate) continue;

		m_Instances[i]->PrepareMethod("OnPhysicsUpdate");
		lua_pushnumber(L, dt);
		m_Instances[i]->CallMethod(1);

		aerr = m_Instances[i]->GetLastError();
		if (aerr != "")
			debug->Error(aerr);
	}
}
// --------------------------------------------