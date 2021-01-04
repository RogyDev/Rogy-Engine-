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
	if(usable)
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
	std::cout << "DeleteObject " <<  class_name << "\n";
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
		if (var_name[0] == '_' || lua_isfunction(L, -1))
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
		else
			properties.push_back(ScriptVar(VAR_Null, var_name));

		lua_pop(L, 1);
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