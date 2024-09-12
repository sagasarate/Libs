#pragma once


#include "src/lua.hpp"
#include "src/lobject.h"
#include "src/ldo.h"
#include "src/lgc.h"

extern "C" {
#include "luasocket/mime.h"
#include "luasocket/luasocket.h"
}

class CLuaBaseMetaClass;

extern UINT g_LuaLogChannel;

extern CEasyString::STRING_CODE_PAGE LUA_SCRIPT_CODE_PAGE;
extern void SystemStrToLuaStr(LPCSTR SystemStr, CEasyStringA & LuaStr);
extern void SystemStrToLuaStr(LPCWSTR SystemStr, CEasyStringA & LuaStr);
extern void LuaStrToSystemStr(const char* LuaStr, CEasyStringA & SystemStr);
extern void LuaStrToSystemStr(const char* LuaStr, CEasyStringW & SystemStr);
//extern int CompareStrOnLuaStack(lua_State * L, int idx, LPCTSTR szStr);

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

inline void LogLuaDirectWithTag(LPCTSTR Tag, LPCTSTR Msg)
{
	CLogManager::GetInstance()->PrintLogDirect(g_LuaLogChannel, ILogPrinter::LOG_LEVEL_NORMAL, Tag, Msg);
}

inline void LogLuaStackWithTag(lua_State* L, LPCTSTR Tag, LPCTSTR Prefix)
{
	if (lua_checkstack(L, 1))
	{
		LPCTSTR szStack = _T("");
		CEasyString StackTemp;
		luaL_traceback(L, L, NULL, 0);
		if (LUA_SCRIPT_CODE_PAGE == CEasyString::SYSTEM_STRING_CODE_PAGE)
		{
			szStack = (LPCTSTR)lua_tostring(L, -1);
		}
		else
		{
			LuaStrToSystemStr(lua_tostring(L, -1), StackTemp);
			szStack = StackTemp;
		}
		CLogManager::GetInstance()->PrintLog(g_LuaLogChannel, ILogPrinter::LOG_LEVEL_NORMAL, Tag, _T("%s\r\n%s"), Prefix, szStack);
		lua_pop(L, 1);
	}
	else
	{
		CLogManager::GetInstance()->PrintLogDirect(g_LuaLogChannel, ILogPrinter::LOG_LEVEL_NORMAL, Tag, _T("not have enough stack"));
	}
}

inline void LogLuaStrWithTag(int LogLevel, lua_State* L, int Idx, LPCTSTR Tag, LPCTSTR Prefix, bool BeDumpStack)
{
	LPCTSTR szLuaStr = _T("");
	CEasyString MsgTemp;
	if (LUA_SCRIPT_CODE_PAGE == CEasyString::SYSTEM_STRING_CODE_PAGE)
	{
		szLuaStr = (LPCTSTR)lua_tostring(L, Idx);
	}
	else
	{
		LuaStrToSystemStr(lua_tostring(L, Idx), MsgTemp);
		szLuaStr = MsgTemp;
	}
	if (!lua_checkstack(L, 1))
	{
		szLuaStr = _T("not have enough stack");
		BeDumpStack = false;
	}
	LPCTSTR szStack = NULL;
	CEasyString StackTemp;
	if (BeDumpStack)
	{
		luaL_traceback(L, L, NULL, 0);
		if (LUA_SCRIPT_CODE_PAGE == CEasyString::SYSTEM_STRING_CODE_PAGE)
		{
			szStack = (LPCTSTR)lua_tostring(L, -1);
		}
		else
		{
			LuaStrToSystemStr(lua_tostring(L, -1), StackTemp);
			szStack = StackTemp;
		}
	}
	if (szStack)
	{
		if (Prefix)
		{
			CLogManager::GetInstance()->PrintLog(g_LuaLogChannel, LogLevel, Tag, _T("%s:%s\r\n%s"), Prefix, szLuaStr, szStack);
		}
		else
		{
			CLogManager::GetInstance()->PrintLog(g_LuaLogChannel, LogLevel, Tag, _T("%s\r\n%s"), szLuaStr, szStack);
		}
	}
	else
	{
		if (Prefix)
		{
			CLogManager::GetInstance()->PrintLog(g_LuaLogChannel, LogLevel, Tag, _T("%s:%s"), Prefix, szLuaStr);
		}
		else
		{
			CLogManager::GetInstance()->PrintLogDirect(g_LuaLogChannel, LogLevel, Tag, szLuaStr);
		}
	}

	if (BeDumpStack)
		lua_pop(L, 1);
}

inline int ThrowLuaErr(lua_State* L, LPCTSTR szFormat, ...)
{
	if (LUA_SCRIPT_CODE_PAGE == CEasyString::SYSTEM_STRING_CODE_PAGE)
	{
		va_list vl;
		va_start(vl, szFormat);
		luaL_where(L, 1);
		lua_pushvfstring(L, (const char*)szFormat, vl);
		va_end(vl);
		lua_concat(L, 2);
		return lua_error(L);
	}
	else
	{
		va_list vl;
		CEasyString Msg;
		CEasyStringA LuaMsg;
		va_start(vl, szFormat);
		Msg.FormatVL(szFormat, vl);
		va_end(vl);
		luaL_where(L, 1);
		SystemStrToLuaStr(Msg, LuaMsg);
		lua_pushlstring(L, LuaMsg, LuaMsg.GetLength());
		lua_concat(L, 2);
		return lua_error(L);
	}
}

inline CEasyString LuaGetString(lua_State* L, int idx)
{
	CEasyString Str;
	LuaStrToSystemStr(lua_tostring(L, idx), Str);
	return Str;
}

inline bool LuaIsArray(lua_State* L, int idx)
{
	idx = lua_absindex(L, idx);
	if (lua_istable(L, idx))
	{
		return lua_rawlen(L, idx) > 0;
	}
	return false;
}

inline bool IsLuaTable(const CSmartStruct& Packet)
{
	return Packet.GetLength() == 0 || Packet.IsMemberExist(VARIED_MEMEBR_ID);
}

#define luaL_argassert(arg, _index_) if (!Match(TypeWrapper<P##arg>(), L, _index_)) \
				luaL_argerror(L, _index_, "bad argument")

#ifdef WIN32
#define LogLua(_Format, ...)	LogLuaWithTag(_T(__FUNCTION__), _Format, ##__VA_ARGS__)
#define LogLuaDebug(_Format, ...)	LogLuaDebugWithTag(_T(__FUNCTION__), _Format, ##__VA_ARGS__)
#define LogLuaDirect(Msg)	LogLuaDirectWithTag(_T(__FUNCTION__), Msg)
#define LogLuaStack(L,Prefix)	LogLuaStackWithTag(L, _T(__FUNCTION__), Prefix)
#define LogLuaStr(L,Idx,Prefix,BeDumpStack)	LogLuaStrWithTag(ILogPrinter::LOG_LEVEL_NORMAL, L, Idx, _T(__FUNCTION__), Prefix, BeDumpStack)
#define LogLuaStrDebug(L,Idx,Prefix,BeDumpStack)	LogLuaStrWithTag(ILogPrinter::LOG_LEVEL_DEBUG, L, Idx, _T(__FUNCTION__), Prefix, BeDumpStack)
#else
#define LogLua(_Format, ...)	LogLuaWithTag(__PRETTY_FUNCTION__, _Format, ##__VA_ARGS__)
#define LogLuaDebug(_Format, ...)	LogLuaDebugWithTag(__PRETTY_FUNCTION__, _Format, ##__VA_ARGS__)
#define LogLuaDirect(Msg)	LogLuaDirectWithTag(__PRETTY_FUNCTION__, Msg)
#define LogLuaStack(L,Prefix)	LogLuaStackWithTag(L, __PRETTY_FUNCTION__, Prefix)
#define LogLuaStr(L,Idx,Prefix,BeDumpStack)	LogLuaStrWithTag(ILogPrinter::LOG_LEVEL_NORMAL, L, Idx, __PRETTY_FUNCTION__, Prefix, BeDumpStack)
#define LogLuaStrDebug(L,Idx,Prefix,BeDumpStack)	LogLuaStrWithTag(ILogPrinter::LOG_LEVEL_DEBUG, L, Idx, __PRETTY_FUNCTION__, Prefix, BeDumpStack)
#endif

#define LUA_THREAD_RECYLE_CHECK_TIME				(10*1000)
#define LUA_YIELD_TIME_OUT							(10*60*1000)
#define LUA_EXECUTING	100
#define LUA_TINTEGER 31
#define LUA_META_CLASS_ID_START 80

enum LUA_RESERVE_YIELD_TYPE
{
	LUA_RESERVE_YIELD_TYPE_NONE,
	LUA_RESERVE_YIELD_TYPE_SLEEP,
	LUA_RESERVE_YIELD_TYPE_MAX,
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

struct LUA_EXCEPTION
{

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
		if (lua_getiuservalue(L, idx, 1) == LUA_TTABLE)
		{
			lua_pushstring(L, "_ClassID");
			lua_gettable(L, -2);
			Type = (int)lua_tointeger(L, -1);
			if (Type == 0)
				Type = LUA_TUSERDATA;
			lua_pop(L, 2);
		}
		else
		{
			lua_pop(L, 1);
		}
	}
	return Type;
}

template<typename T>
inline bool IsKindOfMetaClass(lua_State* L, int idx)
{
	int Type = lua_type(L, idx);
	if (Type == LUA_TUSERDATA)
	{
		CLuaBaseMetaClass* pObject = *((CLuaBaseMetaClass**)lua_touserdata(L, idx));
		if (pObject)
		{
			return (dynamic_cast<T*>(pObject)) != NULL;
		}
	}
	return false;
}


#include "LuaThread.h"

#include "LuaTypeWraps.h"








