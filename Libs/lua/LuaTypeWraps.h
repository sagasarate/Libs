#pragma once

class CLuaBaseMetaClass;
class CLuaGrid;
namespace LuaWrap
{
	struct LuaLightUserData
	{
		LuaLightUserData(const void* value) :
			m_value(value)
		{
		}

		const void* m_value;
	};


	struct LuaUserData
	{
		LuaUserData(const void* value) :
			m_value(value)
		{
		}

		const void* m_value;
	};

	struct LuaNil
	{
	};	

	inline void Push(lua_State* L, bool value)				{ lua_pushboolean(L, value); }
	inline void Push(lua_State* L, char value)				{ lua_pushnumber(L, value); }
	inline void Push(lua_State* L, unsigned char value)		{ lua_pushnumber(L, value); }
	inline void Push(lua_State* L, short value)				{ lua_pushnumber(L, value); }
	inline void Push(lua_State* L, unsigned short value)	{ lua_pushnumber(L, value); }
	inline void Push(lua_State* L, int value)				{ lua_pushnumber(L, value); }
	inline void Push(lua_State* L, unsigned int value)		{ lua_pushnumber(L, value); }
	//inline void Push(lua_State* L, long value)				{ lua_pushnumber(L, value); }
	//inline void Push(lua_State* L, unsigned long value)		{ lua_pushnumber(L, value); }
	inline void Push(lua_State* L, __int64 value)			{ lua_pushinteger(L, value); }
	inline void Push(lua_State* L, unsigned __int64 value)	{ lua_pushinteger(L, value); }
	inline void Push(lua_State* L, double value)			{ lua_pushnumber(L, (lua_Number)value); }
	inline void Push(lua_State* L, float value)				{ lua_pushnumber(L, (lua_Number)value); }
	inline void Push(lua_State* L, const char* value)		{ lua_pushstring(L, value); }
	inline void Push(lua_State* L, const WCHAR* value)		
	{ 
		CEasyStringA StringA;
		StringA = value;
		lua_pushstring(L, StringA);
	}
	inline void Push(lua_State* L, const LuaNil&)			{ lua_pushnil(L); }
	inline void Push(lua_State* L, lua_CFunction value)		{ lua_pushcclosure(L, value, 0); }
	inline void Push(lua_State* L, const void* value)		{ lua_pushlightuserdata(L, (void*)value); }
	inline void Push(lua_State* L, const LuaLightUserData& value)	{ lua_pushlightuserdata(L, (void*)value.m_value); }
	inline void Push(lua_State* L, const LuaValue& value)	
	{ 
		LuaValue::PushValueToLuaState(value, L);
	}
	
	inline void Push(lua_State* L, const CLuaTable& value)
	{
		value.PushToLuaState(L);
	}

	inline void Push(lua_State* L, const CEasyArray<LuaValue>& value)
	{
		for (UINT i = 0; i < value.GetCount(); i++)
		{
			LuaValue::PushValueToLuaState(value.GetObjectConst(i), L);
		}
	}
	inline void Push(lua_State* L, LUA_EMPTY_VALUE value)
	{
		
	}
	inline void Push(lua_State* L, CLuaBaseMetaClass * pObject);
	
	inline void Push(lua_State* L, const CLuaGrid value);

	inline bool	Match(TypeWrapper<bool>, lua_State* L, int idx)
	{
		return lua_isboolean(L, idx);
	}
	inline bool	Match(TypeWrapper<char>, lua_State* L, int idx)
	{
		return lua_isnumber(L, idx) != 0;
	}
	inline bool	Match(TypeWrapper<unsigned char>, lua_State* L, int idx)
	{
		return lua_isnumber(L, idx) != 0;
	}
	inline bool	Match(TypeWrapper<short>, lua_State* L, int idx)
	{
		return lua_isnumber(L, idx) != 0;
	}
	inline bool	Match(TypeWrapper<unsigned short>, lua_State* L, int idx)
	{
		return lua_isnumber(L, idx) != 0;
	}
	inline bool	Match(TypeWrapper<int>, lua_State* L, int idx)
	{
		return lua_isnumber(L, idx) != 0;
	}
	inline bool	Match(TypeWrapper<unsigned int>, lua_State* L, int idx)
	{
		return lua_isnumber(L, idx) != 0;
	}
	//inline bool	Match(TypeWrapper<long>, lua_State* L, int idx)
	//{
	//	return lua_isnumber(L, idx);
	//}
	//inline bool	Match(TypeWrapper<unsigned long>, lua_State* L, int idx)
	//{
	//	return lua_isnumber(L, idx);
	//}
	inline bool	Match(TypeWrapper<__int64>, lua_State* L, int idx)
	{
		return lua_isnumber(L, idx) != 0;
	}
	inline bool	Match(TypeWrapper<unsigned __int64>, lua_State* L, int idx)
	{
		return lua_isnumber(L, idx) != 0;
	}
	inline bool	Match(TypeWrapper<float>, lua_State* L, int idx)
	{
		return lua_isnumber(L, idx) != 0;
	}
	inline bool	Match(TypeWrapper<double>, lua_State* L, int idx)
	{
		return lua_isnumber(L, idx) != 0;
	}
	inline bool	Match(TypeWrapper<const char*>, lua_State* L, int idx)
	{
		return lua_isstring(L, idx) != 0;
	}
	inline bool	Match(TypeWrapper<const WCHAR*>, lua_State* L, int idx)
	{
		return lua_isstring(L, idx) != 0;
	}
	inline bool	Match(TypeWrapper<lua_State*>, lua_State* L, int idx)
	{
		return lua_type(L, idx) == LUA_TNONE;
	}
	inline bool	Match(TypeWrapper<void*>, lua_State* L, int idx)
	{
		return lua_type(L, idx) == LUA_TLIGHTUSERDATA;
	}

	inline bool	Match(TypeWrapper<LuaValue>, lua_State* L, int idx)
	{
		return lua_gettop(L) >= idx;
	}	
	

	inline bool	Match(TypeWrapper<CLuaTable>, lua_State* L, int idx)
	{
		return lua_type(L, idx) == LUA_TTABLE;
	}

	inline bool	Match(TypeWrapper<CEasyArray<LuaValue> >, lua_State* L, int idx)
	{
		return lua_gettop(L) >= idx;
	}

	inline bool	Match(TypeWrapper<LUA_PARAMS >, lua_State* L, int idx)
	{
		return lua_gettop(L) >= idx;
	}

	inline bool	Match(TypeWrapper<LUA_EMPTY_VALUE >, lua_State* L, int idx)
	{
		return lua_gettop(L) >= idx;
	}

	inline bool	Match(TypeWrapper<CLuaBaseMetaClass *>, lua_State* L, int idx);
	
	inline bool	Match(TypeWrapper<CLuaGrid >, lua_State* L, int idx);
	

	inline void				Get(TypeWrapper<void>, lua_State*, int)
	{  }
	inline bool				Get(TypeWrapper<bool>, lua_State* L, int idx)
	{
		return lua_toboolean(L, idx) != 0;
	}
	inline char				Get(TypeWrapper<char>, lua_State* L, int idx)
	{
		return static_cast<char>(lua_tonumber(L, idx));
	}
	inline unsigned char	Get(TypeWrapper<unsigned char>, lua_State* L, int idx)
	{
		return static_cast<unsigned char>(lua_tonumber(L, idx));
	}
	inline short			Get(TypeWrapper<short>, lua_State* L, int idx)
	{
		return static_cast<short>(lua_tonumber(L, idx));
	}
	inline unsigned short	Get(TypeWrapper<unsigned short>, lua_State* L, int idx)
	{
		return static_cast<unsigned short>(lua_tonumber(L, idx));
	}
	inline int				Get(TypeWrapper<int>, lua_State* L, int idx)
	{
		return static_cast<int>(lua_tonumber(L, idx));
	}
	inline unsigned int		Get(TypeWrapper<unsigned int>, lua_State* L, int idx)
	{
		return static_cast<unsigned int>(lua_tonumber(L, idx));
	}
	//inline long				Get(TypeWrapper<long>, lua_State* L, int idx)
	//{
	//	return static_cast<long>(lua_tonumber(L, idx));
	//}
	//inline unsigned long	Get(TypeWrapper<unsigned long>, lua_State* L, int idx)
	//{
	//	return static_cast<unsigned long>(lua_tonumber(L, idx));
	//}
	inline __int64				Get(TypeWrapper<__int64>, lua_State* L, int idx)
	{
		return static_cast<__int64>(lua_tointeger(L, idx));
	}
	inline unsigned __int64	Get(TypeWrapper<unsigned __int64>, lua_State* L, int idx)
	{
		return static_cast<unsigned __int64>(lua_tointeger(L, idx));
	}
	inline float			Get(TypeWrapper<float>, lua_State* L, int idx)
	{
		return static_cast<float>(lua_tonumber(L, idx));
	}
	inline double			Get(TypeWrapper<double>, lua_State* L, int idx)
	{
		return static_cast<double>(lua_tonumber(L, idx));
	}
	inline const char*		Get(TypeWrapper<const char*>, lua_State* L, int idx)
	{
		return static_cast<const char*>(lua_tostring(L, idx));
	}
	inline const WCHAR*		Get(TypeWrapper<const WCHAR*>, lua_State* L, int idx)
	{
		static CEasyStringW StringW;
		StringW = lua_tostring(L, idx);
		return StringW;
	}
	inline LuaNil			Get(TypeWrapper<LuaNil>, lua_State* L, int idx)
	{
		(void)L, (void)idx;  return LuaNil();
	}
	inline lua_CFunction	Get(TypeWrapper<lua_CFunction>, lua_State* L, int idx)
	{
		return static_cast<lua_CFunction>(lua_tocfunction(L, idx));
	}
	inline void*			Get(TypeWrapper<void*>, lua_State* L, int idx)
	{
		return static_cast<void*>(lua_touserdata(L, idx));
	}
	inline lua_State*		Get(TypeWrapper<lua_State*>, lua_State* L, int /*idx*/)
	{
		return L;
	}
	
	inline LuaValue	Get(TypeWrapper<LuaValue>, lua_State* L, int idx)
	{
		LuaValue Value;

		LuaValue::GetValueFromLuaState(Value, L, idx);

		return Value;
	}	

	inline CLuaTable	Get(TypeWrapper<CLuaTable>, lua_State* L, int idx)
	{
		CLuaTable LuaTable;

		LuaTable.GetFromLuaState(L, idx);
		
		return LuaTable;
	}

	inline CEasyArray<LuaValue>	Get(TypeWrapper< CEasyArray<LuaValue> >, lua_State* L, int idx)
	{
		CEasyArray<LuaValue> LuaValueList;

		int ParamCount = lua_gettop(L);
		if (ParamCount>=idx)
		{
			LuaValueList.Resize(ParamCount - idx + 1);
			for (int i = idx; i <= ParamCount; i++)
			{
				LuaValue::GetValueFromLuaState(LuaValueList[i - idx], L, i);
			}
		}
		return LuaValueList;
	}
	inline LUA_PARAMS	Get(TypeWrapper< LUA_PARAMS >, lua_State* L, int idx)
	{
		LUA_PARAMS LuaParams;

		int ParamCount = lua_gettop(L);
		if (ParamCount >= idx)
		{
			LuaParams.pLuaState = L;
			LuaParams.StartIndex = idx;
			LuaParams.ParamCount = ParamCount - idx + 1;			
		}
		return LuaParams;
	}
	inline LUA_EMPTY_VALUE	Get(TypeWrapper< LUA_EMPTY_VALUE >, lua_State* L, int idx)
	{
		return LUA_EMPTY_VALUE();
	}

	
	
	inline CLuaBaseMetaClass * Get(TypeWrapper<CLuaBaseMetaClass *>, lua_State* L, int idx);

	
	inline CLuaGrid Get(TypeWrapper<CLuaGrid>, lua_State* L, int idx);
}