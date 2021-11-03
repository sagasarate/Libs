#pragma once


#include "src/lua.hpp"
#include "src/lobject.h"
#include "src/ldo.h"
#include "src/lgc.h"

#define luaL_argassert(arg, _index_) if (!Match(TypeWrapper<P##arg>(), L, _index_)) \
				luaL_argerror(L, _index_, "bad argument")






#define LUA_THREAD_RECYLE_CHECK_TIME				(10*1000)
#define LUA_YIELD_TIME_OUT							(10*60*1000)
#define LUA_OK 0
#define LUA_TINTEGER 31
#define LUA_META_CLASS_ID_START 80

enum LUA_YIELD_TYPE
{
	LUA_YIELD_TYPE_NONE,
	LUA_YIELD_TYPE_SLEEP,
	LUA_YIELD_TYPE_MAX,
};



struct LUA_PARAMS
{
	lua_State * pLuaState;
	int			StartIndex;
	int			ParamCount;
	LUA_PARAMS()
	{
		pLuaState = NULL;
		StartIndex = 0;
		ParamCount = 0;
	}
};

struct LUA_EMPTY_VALUE
{

};

struct LUA_CUSTOM_RETURN
{
	UINT ReturnCount;
};

struct LUA_CFUN_INFO
{
	CEasyString		FunName;
	lua_CFunction	pLuaCFun;
	BYTE			FunAddr[16];
	UINT			FunSize;
};

template<class T> struct TypeWrapper {};

typedef lua_State * LPLUA_STATE;

class CBaseScriptHost :
	public CNameObject
{

};

class ITimeManager
{
public:
	virtual float GetTime() = 0;
	virtual float GetDeltaTime() = 0;
	virtual float GetTimeRate() = 0;
	virtual void SetTimeRate(float TimeRate) = 0;
	virtual void AddTimeClip(float ClipLenght) = 0;
};


inline int GetLuaObjectType(lua_State* L, int idx)
{
	int Type = lua_type(L, idx);
	if (Type == LUA_TNUMBER)
	{
		if (lua_isinteger(L, idx))
			Type = LUA_TINTEGER;
	}
	else if (Type == LUA_TUSERDATA)
	{
		if (lua_getmetatable(L, idx))
		{
			lua_pushstring(L, "__index");
			lua_gettable(L, -2);
			if (lua_type(L, -1) == LUA_TTABLE)
			{
				lua_pushstring(L, "_ClassID");
				lua_gettable(L, -2);
				Type = (int)lua_tonumber(L, -1);
				if (Type == 0)
					Type = LUA_TUSERDATA;
				lua_pop(L, 1);
			}
			lua_pop(L, 2);

		}
	}
	return Type;
}


#include "LuaValue.h"
#include "LuaTable.h"


#include "LuaTypeWraps.h"

#include "LuaThread.h"
#include "BaseLuaVM.h"
#include "LuaScript.h"
#include "LuaVM.h"


extern UINT g_LuaLogChannel;

inline void LogLuaWithTag(LPCTSTR Tag, LPCTSTR Format, ...)
{
	va_list vl;

	va_start(vl, Format);
	CLogManager::GetInstance()->PrintLogVL(g_LuaLogChannel, ILogPrinter::LOG_LEVEL_NORMAL, Tag, Format, vl);
	va_end(vl);
}

inline void LogLuaDebugWithTag(LPCTSTR Tag, LPCTSTR Format, ...)
{
	va_list vl;

	va_start(vl, Format);
	CLogManager::GetInstance()->PrintLogVL(g_LuaLogChannel, ILogPrinter::LOG_LEVEL_DEBUG, Tag, Format, vl);
	va_end(vl);
}


#ifdef WIN32
#define LogLua(_Format, ...)	LogLuaWithTag(__FUNCTION__, _Format, ##__VA_ARGS__)
#define LogLuaDebug(_Format, ...)	LogLuaDebugWithTag(__FUNCTION__, _Format, ##__VA_ARGS__)
#else
#define LogLua(_Format, ...)	LogLuaWithTag(__PRETTY_FUNCTION__, _Format, ##__VA_ARGS__)
#define LogLuaDebug(_Format, ...)	LogLuaDebugWithTag(__PRETTY_FUNCTION__, _Format, ##__VA_ARGS__)
#endif