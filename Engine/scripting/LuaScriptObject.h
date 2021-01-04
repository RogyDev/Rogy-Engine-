#ifndef R_LUASCRIPT_OBJECT_H
#define R_LUASCRIPT_OBJECT_H

#include <iostream>
#include <string>
#include <vector>
#include <luainc.h>

enum ScriptVarType
{
	VAR_Null,
	VAR_Int,
	VAR_String,
	VAR_Float,
	VAR_Bool,
	VAR_Table,
};

struct ScriptVar
{
	ScriptVarType type;
	std::string name;

	// table vars in case the ScriptVarType is Table 
	std::vector<ScriptVar> vars;

	ScriptVar()
	{
		type = VAR_Null;
		name = "";
	}
	ScriptVar(ScriptVarType _type, std::string _name)
	{
		type = _type;
		name = _name;
	}
};

class ScriptInstance
{
public:
	ScriptInstance();
	ScriptInstance(lua_State *Ls);
	ScriptInstance(lua_State *Ls, const char* className);
	~ScriptInstance();

	/* Create an object from a Lua class.  */
	void CreateObject(const char* className);

	/* Delete the object created from a Lua class. */
	void DeleteObject();

	/* Call a method in the class object with 0 paramaters. */
	void CallMethod(const char* methodName);

	/* Put the object table in stack */
	void Use();

	/* Prepare method to call, after you can push arguments with lua_pushxxx functions. */
	void PrepareMethod(std::string methodName);

	/* Call method with the arguments number you pushed before after the PrepareMethod call. */
	void CallMethod(int args_lenght);

	/* Lock for the public variables in the class object and put then in properties vector. */
	void ResgisterPublicMembers();

	/* Set a variable value with type of "int" */
	void SetVar(const char* var_name, int val);
	/* Set a variable value with type of "bool" */
	void SetVar(const char* var_name, bool val);
	/* Set a variable value with type of "std::string" */
	void SetVar(const char* var_name, std::string val);
	/* Set a variable value with type of "float" */
	void SetVar(const char* var_name, float val);

	/* Get a variable value with type of "int" */
	bool		GetVarBool(const char* var_name);
	/* Get a variable value with type of "bool" */
	int			GetVarInt(const char* var_name);
	/* Get a variable value with type of "float" */
	float		GetVarFloat(const char* var_name);
	/* Get a variable value with type of "std::string" */
	std::string GetVarString(const char* var_name);

	bool HasVar(const char* var_name, ScriptVarType var_type);

	/* Check for the variable type : since Lua is dynamic language variables types can 
	   be changed in runtime, using this function you can check for the variable type
	   (ofcource the var must be public so we can found it in properties) and if the 
	   type changed at runtime it will change the var type in properties to the new type.
	   note: if the new type is nil the variable will be PRIVATE.
	*/
	void CheckVarType(const char* var_name);

	/* Get error of the last function call
	   empty string means no errors.
	*/
	std::string GetLastError();

	// class name
	std::string class_name;
	// Last prepared method name, should not modify.
	std::string prep_method;
	// The main Lua State
	lua_State* L;
	// Lua reference index to the created class instance.
	int ref_idx;
	// Is the instance valid?
	bool usable = false;

	// Delete this script instance as garabage? 
	bool GC = false;
	int ent_id = -2;

	bool hasUpdate = false;
	bool hasStart = false;
	bool hasDestroy = false;
	bool hasOnCollision = false;

	/* Vector of the object public properties */
	std::vector<ScriptVar> properties;

private:
	std::string last_error;
};

#endif // ! R_LUASCRIPT_OBJECT_H