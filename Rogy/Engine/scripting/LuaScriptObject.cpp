#include "LuaScriptObject.h"

//--------------------------------------------------------------
ScriptInstance::ScriptInstance()
{
}
//--------------------------------------------------------------
ScriptInstance::ScriptInstance(lua_State *Ls)
{
	L = Ls;
}
//--------------------------------------------------------------
ScriptInstance::ScriptInstance(lua_State *Ls, const char* className)
{
	L = Ls;
	CreateObject(className);
}
//--------------------------------------------------------------
ScriptInstance::~ScriptInstance()
{
	if (usable)
		DeleteObject();
}
//--------------------------------------------------------------
void ScriptInstance::CreateObject(const char* className)
{
	lua_newtable(L);
	lua_getglobal(L, className);
	lua_pcall(L, 0, 1, 0);
	ref_idx = luaL_ref(L, LUA_REGISTRYINDEX);
	usable = true;
	class_name = std::string(className);

	ResgisterPublicMembers();

	if (ent_id >= 0)
	{
		PrepareMethod("_SetupTheScriptComponent");
		lua_pushnumber(L, ent_id);
		CallMethod(1);
	}
}
//--------------------------------------------------------------
void ScriptInstance::DeleteObject()
{
	//std::cout << "DeleteObject " <<  class_name << "\n";
	luaL_unref(L, LUA_REGISTRYINDEX, ref_idx);
	usable = false;
}
//--------------------------------------------------------------
void ScriptInstance::ResgisterPublicMembers()
{
	Use();
	lua_getfield(L, -1, "_");

	if (!properties.empty())
		properties.clear();
	functions.clear();

	lua_pushnil(L);
	while (lua_next(L, -2) != 0)
	{
		//std::cout << luaL_typename(L, -2) << " = " << luaL_typename(L, -1) << std::endl;
		//std::cout << lua_tostring(L, -2) << " = " << lua_tonumber(L, -1) << std::endl;
		std::string var_name = lua_tostring(L, -2);

		if (lua_isfunction(L, -1) && var_name == "OnStart")
			hasStart = true;
		if (lua_isfunction(L, -1) && var_name == "OnUpdate")
			hasUpdate = true;
		if (lua_isfunction(L, -1) && var_name == "OnDestroy")
			hasDestroy = true;
		if (lua_isfunction(L, -1) && var_name == "OnCollision")
			hasOnCollision = true;

		const char* vn = var_name.c_str();
		if (var_name[0] == '_'/* || lua_isfunction(L, -1)*/)
		{
			lua_pop(L, 1);
			continue;
		}

		if (lua_isinteger(L, -1))
		{
			properties.push_back(ScriptVar(VAR_Int, var_name));
		}
		else if (lua_isnumber(L, -1))
		{
			properties.push_back(ScriptVar(VAR_Float, var_name));
		}
		else if (lua_isstring(L, -1))
		{
			properties.push_back(ScriptVar(VAR_String, var_name));
		}
		else if (lua_isboolean(L, -1))
		{
			properties.push_back(ScriptVar(VAR_Bool, var_name));
		}
		else if (lua_istable(L, -1))
		{
			properties.push_back(ScriptVar(VAR_Table, var_name));
		}
		else if (lua_isuserdata(L, -1))
		{
			properties.push_back(ScriptVar(VAR_USERDATA, var_name));
		}
		else if (lua_isfunction(L, -1))
			functions.push_back(var_name);
		else
			properties.push_back(ScriptVar(VAR_Null, var_name));

		lua_pop(L, 1);
	}

	for (size_t i = 0; i < properties.size(); i++)
	{
		if (properties[i].type == VAR_USERDATA)
		{
			lua_getglobal(L, "AssetType");
			if (lua_isfunction(L, -1))
			{
				Use();
				lua_pushstring(L, properties[i].name.c_str());
				lua_pcall(L, 2, 1, 0);
				std::string asset_type = lua_tostring(L, -1);
				//std::cout << "||" << asset_type << "||";
				if (asset_type == "asset") 
					properties[i].type = VAR_ASSET;
				else if(asset_type == "VEC3")
					properties[i].type = VAR_VEC3;
				lua_pop(L, 0);
			}
		}
	}
}
//--------------------------------------------------------------
void ScriptInstance::SetVar(const char* var_name, int val)
{
	Use();
	// Type check
	lua_getfield(L, -1, var_name);
	if (!lua_isinteger(L, -1))	return;

	// Set the field
	PrepareMethod("set");
	lua_pushstring(L, var_name);
	lua_pushinteger(L, val);
	CallMethod(2);
}
//--------------------------------------------------------------
void ScriptInstance::SetVar(const char* var_name, bool val)
{
	Use();
	// Type check
	lua_getfield(L, -1, var_name);
	if (!lua_isboolean(L, -1))	return;

	// Set the field
	PrepareMethod("set");
	lua_pushstring(L, var_name);
	lua_pushboolean(L, (int)val);
	CallMethod(2);
}
//--------------------------------------------------------------
void ScriptInstance::SetVar(const char* var_name, std::string val)
{
	Use();
	// Type check
	lua_getfield(L, -1, var_name);
	if (!lua_isstring(L, -1))	return;

	// Set the field
	PrepareMethod("set");
	lua_pushstring(L, var_name);
	lua_pushstring(L, val.c_str());
	CallMethod(2);
}
//--------------------------------------------------------------
void ScriptInstance::SetVar(const char* var_name, float val)
{
	Use();
	// Type check
	lua_getfield(L, -1, var_name);
	if (!lua_isnumber(L, -1))	return;

	// Set the field
	PrepareMethod("set");
	lua_pushstring(L, var_name);
	lua_pushnumber(L, (float)val);
	CallMethod(2);
}
//--------------------------------------------------------------
bool ScriptInstance::HasVar(const char* var_name, ScriptVarType var_type)
{
	for (size_t i = 0; i < properties.size(); i++)
	{
		if (properties[i].name == var_name && properties[i].type == var_type)
			return true;
	}
	return false;
}
//--------------------------------------------------------------
bool ScriptInstance::HasFunc(const char * var_name)
{
	for (size_t i = 0; i < functions.size(); i++)
		if (functions[i] == var_name)
			return true;
	return false;
}
//--------------------------------------------------------------
std::string ScriptInstance::GetAssetPath(std::string& var_name)
{
	lua_getglobal(L, "GetAssetPath");
	if (lua_isfunction(L, -1))
	{
		Use();
		lua_pushstring(L, var_name.c_str());
		lua_pcall(L, 2, 1, 0);
		std::string asset_p = lua_tostring(L, -1);
		lua_pop(L, 0);
		return asset_p;
	}
	return std::string();
}
//--------------------------------------------------------------
void ScriptInstance::SetAssetPath(std::string& var_name, std::string& new_path)
{
	lua_getglobal(L, "SetAssetPath");
	if (lua_isfunction(L, -1))
	{
		Use();
		lua_pushstring(L, var_name.c_str());
		lua_pushstring(L, new_path.c_str());
		lua_pcall(L, 3, 1, 0);
		lua_pop(L, 0);
	}
}
//--------------------------------------------------------------
glm::vec3 ScriptInstance::GetVec3(std::string & var_name)
{
	float xx, yy, zz;
	for (size_t i = 0; i < 3; i++)
	{
		lua_getglobal(L, "GetVec3XYZ");
		Use();
		lua_pushstring(L, var_name.c_str());
		lua_pushinteger(L, i);
		lua_pcall(L, 3, 1, 0);

		if (i == 0)	xx = (float)lua_tonumber(L, -1);
		else if(i == 1)	yy = (float)lua_tonumber(L, -1);
		else zz = (float)lua_tonumber(L, -1);

		lua_pop(L, 0);
	}
	return glm::vec3(xx, yy, zz);
}
//--------------------------------------------------------------
void ScriptInstance::SetVec3(std::string & var_name, glm::vec3 & v)
{
	for (size_t i = 0; i < 3; i++)
	{
		lua_getglobal(L, "SetVec3XYZ");
		Use();
		lua_pushstring(L, var_name.c_str());
		lua_pushinteger(L, i);

		if (i == 0)	lua_pushnumber(L, v.x);
		else if (i == 1)	lua_pushnumber(L, v.y);
		else lua_pushnumber(L, v.z);

		lua_pcall(L, 4, 1, 0);
		lua_pop(L, 0);
	}
}
//--------------------------------------------------------------
int ScriptInstance::GetVarInt(const char* var_name)
{
	Use();
	lua_getfield(L, -1, var_name);

	if (!lua_isinteger(L, -1))
	{
		CheckVarType(var_name);
		return 0;
	}

	int luavar = (int)lua_tointeger(L, -1);
	lua_pop(L, 1);
	return luavar;
}
//--------------------------------------------------------------
bool ScriptInstance::GetVarBool(const char* var_name)
{
	Use();
	lua_getfield(L, -1, var_name);

	if (!lua_isboolean(L, -1))
	{
		CheckVarType(var_name);
		return false;
	}

	int luavar = lua_toboolean(L, -1);
	lua_pop(L, 1);

	if (luavar == 1) return true;
	else return false;
}
//--------------------------------------------------------------
float ScriptInstance::GetVarFloat(const char* var_name)
{
	Use();
	lua_getfield(L, -1, var_name);

	if (!lua_isnumber(L, -1))
	{
		CheckVarType(var_name);
		return 0.0f;
	}

	float luavar = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);
	return luavar;
}
//--------------------------------------------------------------
std::string ScriptInstance::GetVarString(const char* var_name)
{
	Use();
	lua_getfield(L, -1, var_name);

	if (!lua_isstring(L, -1))
	{
		CheckVarType(var_name);
		return std::string("");
	}

	std::string luavar = lua_tostring(L, -1);
	lua_pop(L, 1);
	return luavar;
}
//--------------------------------------------------------------
void ScriptInstance::CheckVarType(const char* var_name)
{
	Use();
	lua_getfield(L, -1, var_name);
	for (size_t i = 0; i < properties.size(); i++)
	{
		if (properties[i].name == var_name)
		{
			if (lua_isinteger(L, -1))
				properties[i].type = VAR_Int;

			else if (lua_isnumber(L, -1))
				properties[i].type = VAR_Float;

			else if (lua_isstring(L, -1))
				properties[i].type = VAR_String;

			else if (lua_isboolean(L, -1))
				properties[i].type = VAR_Bool;
			else
				properties[i].type = VAR_Null;

			break;
		}
	}
}
//--------------------------------------------------------------
void ScriptInstance::Use()
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, ref_idx);
}
//--------------------------------------------------------------
void ScriptInstance::CallMethod(const char* methodName)
{
	if (!usable)
		return;
	//std::cerr << "CallMethod : " << methodName << "\n";
	lua_rawgeti(L, LUA_REGISTRYINDEX, ref_idx);
	lua_getfield(L, -1, methodName);
	lua_pushvalue(L, -2);
	if (lua_pcall(L, 1, 0, 0)) {
		std::cerr << "execution error : ";
		std::cerr << lua_tostring(L, -1) << std::endl;
		last_error = lua_tostring(L, -1);
		lua_pop(L, 1);
	}
	lua_pop(L, 0);
}
//--------------------------------------------------------------
void ScriptInstance::PrepareMethod(std::string methodName)
{
	prep_method = methodName;

	lua_rawgeti(L, LUA_REGISTRYINDEX, ref_idx);
	lua_getfield(L, -1, prep_method.c_str());
	lua_pushvalue(L, -2);
}
//--------------------------------------------------------------
void ScriptInstance::CallMethod(int args_lenght)
{
	if (lua_pcall(L, args_lenght + 1, 0, 0)) {
		std::cerr << "execution error : ";
		std::cerr << lua_tostring(L, -1) << std::endl;
		last_error = lua_tostring(L, -1);
		lua_pop(L, 1);
	}
	lua_pop(L, 0);
}
//--------------------------------------------------------------
std::string ScriptInstance::GetLastError()
{
	std::string a = last_error;
	last_error = "";
	return a;
}
//--------------------------------------------------------------
void ScriptInstance::CacheVarsValues()
{
	for (size_t i = 0; i < properties.size(); i++)
	{
		if (properties[i].type == ScriptVarType::VAR_Null || properties[i].type == ScriptVarType::VAR_Table)
			continue;

		cached_properties.emplace_back();
		int o = cached_properties.size() - 1;
		cached_properties[o].type = properties[i].type;
		cached_properties[o].name = properties[i].name;

		if (properties[i].type == ScriptVarType::VAR_Float)
			cached_properties[o].f = GetVarFloat(properties[i].name.c_str());

		else if (properties[i].type == ScriptVarType::VAR_Bool)
			cached_properties[o].b = GetVarBool(properties[i].name.c_str());
		
		else if (properties[i].type == ScriptVarType::VAR_Int)
			cached_properties[o].i = GetVarInt(properties[i].name.c_str());

		else if (properties[i].type == ScriptVarType::VAR_String)
			cached_properties[o].str = GetVarString(properties[i].name.c_str());

		else if (properties[i].type == ScriptVarType::VAR_VEC3)
			cached_properties[o].v3 = GetVec3(properties[i].name);

		else if (properties[i].type == ScriptVarType::VAR_ASSET)
			cached_properties[o].str = GetAssetPath(properties[i].name);
	}
}
//--------------------------------------------------------------
void ScriptInstance::ResetVarsValues()
{
	for (size_t i = 0; i < cached_properties.size(); i++)
	{
		if (cached_properties[i].type == ScriptVarType::VAR_Float)
			SetVar(cached_properties[i].name.c_str(), cached_properties[i].f);
		else if (cached_properties[i].type == ScriptVarType::VAR_Int)
			SetVar(cached_properties[i].name.c_str(), cached_properties[i].i);
		else if (cached_properties[i].type == ScriptVarType::VAR_Bool)
			SetVar(cached_properties[i].name.c_str(), cached_properties[i].b);
		else if (cached_properties[i].type == ScriptVarType::VAR_String)
			SetVar(cached_properties[i].name.c_str(), cached_properties[i].str);
		else if (cached_properties[i].type == ScriptVarType::VAR_VEC3)
			SetVec3(cached_properties[i].name, cached_properties[i].v3);
		else if (cached_properties[i].type == ScriptVarType::VAR_ASSET)
			SetAssetPath(cached_properties[i].name, cached_properties[i].str);
	}
	cached_properties.clear();
	cached_properties.shrink_to_fit();
}
//--------------------------------------------------------------