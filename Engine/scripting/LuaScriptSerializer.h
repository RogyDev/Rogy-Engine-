#ifndef R_LUASCRIPT_SERIALIZER_H
#define R_LUASCRIPT_SERIALIZER_H

#include "LuaScriptObject.h"
#include "../io/FSerializer.h"

class ScriptSerializer
{
public:
	ScriptSerializer();
	~ScriptSerializer();

	static void SerializeScriptObject(cereal::BinaryOutputArchive& ar, ScriptInstance* scr);
	static void LoadScriptObject(cereal::BinaryInputArchive& ar, ScriptInstance* scr);
private:

};


#endif // ! R_LUASCRIPT_SERIALIZER_H