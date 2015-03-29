#pragma once
#include "src/lua.hpp"
#include "src/lobject.h"
#include "src/ldo.h"
#include "src/lgc.h"

#define luaL_argassert(arg, _index_) if (!Match(TypeWrapper<P##arg>(), L, _index_)) \
				luaL_argerror(L, _index_, "bad argument")

#define LUA_THREAD_RECYLE_CHECK_TIME				(60*1000)
#define LUA_YIELD_TIME_OUT							(10*60*1000)
#define LUA_OK 0
#define LUA_TINTEGER 31

enum LUA_YIELD_TYPE
{
	LUA_YIELD_TYPE_NONE,
	LUA_YIELD_TYPE_SLEEP,
	LUA_YIELD_TYPE_MAX,
};





template<class T> struct TypeWrapper {};



#include "LuaTable.h"

#include "LuaThread.h"
#include "LuaScript.h"

#include "LuaTypeWraps.h"
#include "LuaCallWraps.h"



#include "LuaScriptPool.h"

//#include "LuaPlusCD.h"


#include "LuaBaseMetaClass.h"
