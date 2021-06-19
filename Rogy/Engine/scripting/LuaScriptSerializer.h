#ifndef R_LUASCRIPT_SERIALIZER_H
#define R_LUASCRIPT_SERIALIZER_H

#include "LuaScriptObject.h"
#include "../io/FSerializer.h"

class ScriptSerializer
{
public:
	ScriptSerializer();
	~ScriptSerializer();

	static void SerializeScriptObject(YAML::Emitter& out, ScriptInstance* scr);
	static void LoadScriptObject(YAML::Node& out, ScriptInstance* scr);
private:

};


#endif // ! R_LUASCRIPT_SERIALIZER_H