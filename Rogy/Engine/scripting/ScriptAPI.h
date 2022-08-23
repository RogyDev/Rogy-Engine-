#ifndef LUA_SCRIPT_API_H
#define LUA_SCRIPT_API_H

#include "ScriptManager.h"

namespace EngineAPI_LUA
{
	static void RegisterInputAPI(lua_State* L);
	static void RegisterDebugAPI(lua_State* L);
	static void RegisterUIAPI(lua_State* L);
	static void RegisterRendererAPI(lua_State* L);
	static void RegisterMathAPI(lua_State* L);
	static void RegisterAudioAPI(lua_State* L);
	static void RegisterPlayerPrefsAPI(lua_State* L);
	static void RegisterPhysicsAPI(lua_State* L);
	template<typename T>
	static void RegisterSceneAPI(lua_State* L);

	template<typename T>
	static void RegisterAPI(lua_State* L);

}

#endif // LUA_SCRIPT_API_H