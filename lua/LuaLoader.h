#pragma once

template <typename Callee, typename SearchFunc, typename LoadFunc>
class LuaLoaderDispatcherHelper
{
public:
	static inline int LuaLoader(lua_State* L)
	{
		BYTE* buffer = (BYTE*)lua_touserdata(L, -1);
		if (buffer)
		{
			CBaseLuaVM* pVM = *((CBaseLuaVM**)buffer);
			Callee* pCaller = *((Callee**)(buffer + sizeof(CBaseLuaVM*)));
			LoadFunc pFn = *((LoadFunc*)(buffer + sizeof(CBaseLuaVM*) + sizeof(Callee*) + sizeof(SearchFunc)));
			if (pVM && pCaller && pFn)
			{
				LPCTSTR szModelName = NULL;
				CEasyString Temp;
				const char* szName = lua_tostring(L, -2);
				if (LUA_SCRIPT_CODE_PAGE != CEasyString::SYSTEM_STRING_CODE_PAGE)
				{
					LuaStrToSystemStr(szName, Temp);
					szModelName = Temp;
				}
				else
				{
					szModelName = (LPCTSTR)szName;
				}

				LPCTSTR pStr = (pCaller->*pFn)(szModelName);
				size_t ContentLen = 0;
				if (pStr)
				{
					CEasyStringA LuaStr;
					const char* pLuaStr = NULL;
					if (LUA_SCRIPT_CODE_PAGE != CEasyString::SYSTEM_STRING_CODE_PAGE)
					{
						//内码转换
						SystemStrToLuaStr(pStr, LuaStr);
						pLuaStr = LuaStr;
						ContentLen = LuaStr.GetLength();
					}
					else
					{
#ifndef UNICODE
						pLuaStr = pStr;
						ContentLen = CEasyString::GetStrLen(pLuaStr);
#endif // !UNICODE
					}
					if (pLuaStr)
					{
						int Result = luaL_loadbuffer(L, pLuaStr, ContentLen, szName);
						if (Result == LUA_OK)
						{
							Result = lua_pcall(L, 0, LUA_MULTRET, 0);
						}
						if (Result == LUA_OK)
						{
							pVM->AddLoadedModule(szName);
							//LogLuaDebug(_T("已成功加载模块%s"), szModelName);
						}
						else
						{
							if (lua_type(L, -1) == LUA_TSTRING)
							{
								luaL_error(L, lua_tostring(L, -1));
							}
							else
							{
								luaL_error(L, "load module[%s] failed", szName);
							}
						}
					}
					else
					{
						luaL_error(L, "invalid module[%s] script", szName);
					}
				}
				else
				{
					luaL_error(L, "module[%s] script not found", szName);
				}
			}
			else
			{
				luaL_error(L, "invalid upvalue");
			}
		}
		else
		{
			luaL_error(L, "invalid loader");
		}
		return 1;
	}
	static inline int LuaSearcher(lua_State* L)
	{
		BYTE* buffer = (BYTE*)lua_touserdata(L, lua_upvalueindex(1));
		if(buffer)
		{
			Callee* pCaller = *((Callee**)(buffer + sizeof(CBaseLuaVM*)));
			SearchFunc pFn = *((SearchFunc*)(buffer + sizeof(CBaseLuaVM*) + sizeof(Callee*)));
			if (pCaller && pFn)
			{
				LPCTSTR szModelName = NULL;
				CEasyString Temp;
				const char* szName = lua_tostring(L, -1);
				if (LUA_SCRIPT_CODE_PAGE != CEasyString::SYSTEM_STRING_CODE_PAGE)
				{
					LuaStrToSystemStr(szName, Temp);
					szModelName = Temp;
				}
				else
				{
					szModelName = (LPCTSTR)szName;
				}
				if((pCaller->*pFn)(szModelName))
				{
					lua_pushcfunction(L, LuaLoader);
					lua_pushvalue(L, lua_upvalueindex(1));
					return 2;
				}
			}
			else
			{
				LogLua(_T("invalid upvalue"));
			}
		}
		else
		{
			LogLua(_T("no upvalue"));
		}
		lua_pushnil(L);
		return 1;
	}
};

template <typename SearchFunc, typename LoadFunc>
class LuaLoaderDispatcherHelper2
{
public:
	static inline int LuaLoader(lua_State* L)
	{
		BYTE* buffer = (BYTE*)lua_touserdata(L, -1);
		if (buffer)
		{
			CBaseLuaVM* pVM = *((CBaseLuaVM**)buffer);
			LoadFunc pFn = *((LoadFunc*)(buffer + sizeof(CBaseLuaVM*) + sizeof(SearchFunc)));
			if (pVM && pFn)
			{
				LPCTSTR szModelName = NULL;
				CEasyString Temp;
				const char* szName = lua_tostring(L, -2);
				if (LUA_SCRIPT_CODE_PAGE != CEasyString::SYSTEM_STRING_CODE_PAGE)
				{
					LuaStrToSystemStr(szName, Temp);
					szModelName = Temp;
				}
				else
				{
					szModelName = (LPCTSTR)szName;
				}

				LPCTSTR pStr = pFn(szModelName);
				size_t ContentLen = 0;
				if (pStr)
				{
					CEasyStringA LuaStr;
					const char* pLuaStr = NULL;
					if (LUA_SCRIPT_CODE_PAGE != CEasyString::SYSTEM_STRING_CODE_PAGE)
					{
						//内码转换
						SystemStrToLuaStr(pStr, LuaStr);
						pLuaStr = LuaStr;
						ContentLen = LuaStr.GetLength();
					}
					else
					{
#ifndef UNICODE
						pLuaStr = pStr;
						ContentLen = CEasyString::GetStrLen(pLuaStr);
#endif // !UNICODE
					}
					if (pLuaStr)
					{
						int Result = luaL_loadbuffer(L, pLuaStr, ContentLen, szName);
						if (Result == LUA_OK)
						{
							Result = lua_pcall(L, 0, LUA_MULTRET, 0);
						}
						if (Result == LUA_OK)
						{
							pVM->AddLoadedModule(szName);
							//LogLuaDebug(_T("已成功加载模块%s"), szModelName);
						}
						else
						{
							if (lua_type(L, -1) == LUA_TSTRING)
							{
								luaL_error(L, lua_tostring(L, -1));
							}
							else
							{
								luaL_error(L, "load module[%s] failed", szName);
							}
						}
					}
					else
					{
						luaL_error(L, "invalid module[%s] script", szName);
					}
				}
				else
				{
					luaL_error(L, "module[%s] script not found", szName);
				}
			}
			else
			{
				luaL_error(L, "invalid upvalue");
			}
		}
		else
		{
			luaL_error(L, "invalid loader");
		}
		return 1;
	}
	static inline int LuaSearcher(lua_State* L)
	{
		BYTE* buffer = (BYTE*)lua_touserdata(L, lua_upvalueindex(1));
		if (buffer)
		{
			SearchFunc pFn = *((SearchFunc*)(buffer + sizeof(CBaseLuaVM*)));
			if (pFn)
			{
				LPCTSTR szModelName = NULL;
				CEasyString Temp;
				const char* szName = lua_tostring(L, -1);
				if (LUA_SCRIPT_CODE_PAGE != CEasyString::SYSTEM_STRING_CODE_PAGE)
				{
					LuaStrToSystemStr(szName, Temp);
					szModelName = Temp;
				}
				else
				{
					szModelName = (LPCTSTR)szName;
				}
				if (pFn(szModelName))
				{
					lua_pushcfunction(L, LuaLoader);
					lua_pushvalue(L, lua_upvalueindex(1));
					return 2;
				}
			}
			else
			{
				LogLua(_T("invalid upvalue"));
			}
		}
		else
		{
			LogLua(_T("no upvalue"));
		}
		lua_pushnil(L);
		return 1;
	}
};

template <typename SearchFunc, typename LoadFunc>
class LuaLoaderDispatcherHelper3
{
public:
	static inline int LuaLoader(lua_State* L)
	{
		BYTE* buffer = (BYTE*)lua_touserdata(L, -1);
		if (buffer)
		{
			CBaseLuaVM* pVM = *((CBaseLuaVM**)buffer);
			LoadFunc pFn = *((LoadFunc*)(buffer + sizeof(CBaseLuaVM*) + sizeof(SearchFunc)));
			if (pVM && pFn)
			{
				LPCTSTR szModelName = NULL;
				CEasyString Temp;
				const char* szName = lua_tostring(L, -2);
				if (LUA_SCRIPT_CODE_PAGE != CEasyString::SYSTEM_STRING_CODE_PAGE)
				{
					LuaStrToSystemStr(szName, Temp);
					szModelName = Temp;
				}
				else
				{
					szModelName = (LPCTSTR)szName;
				}

				return pFn(L, szModelName);
			}
			else
			{
				luaL_error(L, "invalid upvalue");
			}
		}
		else
		{
			luaL_error(L, "invalid loader");
		}
		return 0;
	}
	static inline int LuaSearcher(lua_State* L)
	{
		BYTE* buffer = (BYTE*)lua_touserdata(L, lua_upvalueindex(1));
		if (buffer)
		{
			SearchFunc pFn = *((SearchFunc*)(buffer + sizeof(CBaseLuaVM*)));
			if (pFn)
			{
				LPCTSTR szModelName = NULL;
				CEasyString Temp;
				const char* szName = lua_tostring(L, -1);
				if (LUA_SCRIPT_CODE_PAGE != CEasyString::SYSTEM_STRING_CODE_PAGE)
				{
					LuaStrToSystemStr(szName, Temp);
					szModelName = Temp;
				}
				else
				{
					szModelName = (LPCTSTR)szName;
				}
				if (pFn(szModelName))
				{
					lua_pushcfunction(L, LuaLoader);
					lua_pushvalue(L, lua_upvalueindex(1));
					return 2;
				}
			}
			else
			{
				LogLua(_T("invalid upvalue"));
			}
		}
		else
		{
			LogLua(_T("no upvalue"));
		}
		lua_pushnil(L);
		return 1;
	}
};