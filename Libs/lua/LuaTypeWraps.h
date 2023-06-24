#pragma once

class CLuaBaseMetaClass;
class CLuaGrid;
class CLuaSmartValue;
class CLuaSmartStruct;
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

	inline void Push(CLuaThread* pLuaThread, bool value)				{ lua_pushboolean(pLuaThread->GetLuaState(), value); }
	inline void Push(CLuaThread* pLuaThread, char value)				{ lua_pushinteger(pLuaThread->GetLuaState(), value); }
	inline void Push(CLuaThread* pLuaThread, unsigned char value)		{ lua_pushinteger(pLuaThread->GetLuaState(), value); }
	inline void Push(CLuaThread* pLuaThread, short value)				{ lua_pushinteger(pLuaThread->GetLuaState(), value); }
	inline void Push(CLuaThread* pLuaThread, unsigned short value)		{ lua_pushinteger(pLuaThread->GetLuaState(), value); }
	inline void Push(CLuaThread* pLuaThread, int value)					{ lua_pushinteger(pLuaThread->GetLuaState(), value); }
	inline void Push(CLuaThread* pLuaThread, unsigned int value)		{ lua_pushinteger(pLuaThread->GetLuaState(), value); }
	//inline void Push(lua_State* L, long value)						{ lua_pushnumber(pLuaThread->GetLuaState(), value); }
	//inline void Push(lua_State* L, unsigned long value)				{ lua_pushnumber(pLuaThread->GetLuaState(), value); }
	inline void Push(CLuaThread* pLuaThread, __int64 value)				{ lua_pushinteger(pLuaThread->GetLuaState(), value); }
	inline void Push(CLuaThread* pLuaThread, unsigned __int64 value)	{ lua_pushinteger(pLuaThread->GetLuaState(), value); }
	inline void Push(CLuaThread* pLuaThread, double value)				{ lua_pushnumber(pLuaThread->GetLuaState(), (lua_Number)value); }
	inline void Push(CLuaThread* pLuaThread, float value)				{ lua_pushnumber(pLuaThread->GetLuaState(), (lua_Number)value); }
	inline void Push(CLuaThread* pLuaThread, const char* value)			
	{ 
		if (LUA_SCRIPT_CODE_PAGE == CEasyString::SYSTEM_STRING_CODE_PAGE)
		{
			lua_pushstring(pLuaThread->GetLuaState(), value);
		}
		else
		{
			CEasyStringA StringA;
			SystemStrToLuaStr(value, StringA);
			lua_pushstring(pLuaThread->GetLuaState(), StringA);
		}
	}
	inline void Push(CLuaThread* pLuaThread, const WCHAR* value)
	{ 
		CEasyStringA StringA;
		SystemStrToLuaStr(value, StringA);
		lua_pushstring(pLuaThread->GetLuaState(), StringA);
	}
	inline void Push(CLuaThread* pLuaThread, const CEasyString& value)
	{
		if (LUA_SCRIPT_CODE_PAGE == CEasyString::SYSTEM_STRING_CODE_PAGE)
		{
			lua_pushstring(pLuaThread->GetLuaState(), (LPCSTR)((LPCTSTR)value));
		}
		else
		{
			CEasyStringA StringA;
			SystemStrToLuaStr(value, StringA);
			lua_pushstring(pLuaThread->GetLuaState(), StringA);
		}
	}
	inline void Push(CLuaThread* pLuaThread, const LuaNil&)					{ lua_pushnil(pLuaThread->GetLuaState()); }
	inline void Push(CLuaThread* pLuaThread, lua_CFunction value)			{ lua_pushcclosure(pLuaThread->GetLuaState(), value, 0); }
	inline void Push(CLuaThread* pLuaThread, const void* value)				{ lua_pushlightuserdata(pLuaThread->GetLuaState(), (void*)value); }
	inline void Push(CLuaThread* pLuaThread, const LuaLightUserData& value)	{ lua_pushlightuserdata(pLuaThread->GetLuaState(), (void*)value.m_value); }
	
	inline void Push(CLuaThread* pLuaThread, LUA_EMPTY_VALUE value)
	{
		
	}
	inline void Push(CLuaThread* pLuaThread, CLuaBaseMetaClass * pObject);	
	inline void Push(CLuaThread* pLuaThread, const CVariedValue& value)
	{
		switch (value.GetType())
		{
		case VARIED_VALUE_TYPE::BOOLEAN:
			lua_pushboolean(pLuaThread->GetLuaState(), (bool)value);
			break;
		case VARIED_VALUE_TYPE::INT8:
		case VARIED_VALUE_TYPE::UINT8:
		case VARIED_VALUE_TYPE::INT16:
		case VARIED_VALUE_TYPE::UINT16:
		case VARIED_VALUE_TYPE::INT32:
		case VARIED_VALUE_TYPE::UINT32:
		case VARIED_VALUE_TYPE::INT64:
		case VARIED_VALUE_TYPE::UINT64:
			lua_pushinteger(pLuaThread->GetLuaState(), (__int64)value);
			break;
		case VARIED_VALUE_TYPE::FLOAT32:
		case VARIED_VALUE_TYPE::FLOAT64:
			lua_pushnumber(pLuaThread->GetLuaState(), (double)value);
			break;
		case VARIED_VALUE_TYPE::STRING:
			Push(pLuaThread, (LPCTSTR)value);
			break;
		case VARIED_VALUE_TYPE::BINARY:
			if (value.GetLength())
			{
				void* pBuffer = lua_newuserdata(pLuaThread->GetLuaState(), value.GetLength());
				memcpy(pBuffer, (const unsigned char*)value, value.GetLength());
			}
			else
			{
				pLuaThread->PushNil();
			}
			break;
		case VARIED_VALUE_TYPE::ARRAY:
			{
				lua_createtable(pLuaThread->GetLuaState(), value.GetLength(), 0);
				int Index = 1;
				for (const CVariedValue& Value : value)
				{
					pLuaThread->PushInteger(Index);
					pLuaThread->PushValue(Value);
					lua_settable(pLuaThread->GetLuaState(), -3);
					Index++;
				}
			}
			break;
		case VARIED_VALUE_TYPE::TABLE:
			{
				lua_newtable(pLuaThread->GetLuaState());
				void* Pos = value.GetFirstChildPos();
				while (Pos)
				{
					CVariedValue Key;
					const CVariedValue* pData = value.GetNextChild(Pos, Key);
					pLuaThread->PushValue(Key);
					pLuaThread->PushValue(*pData);
					lua_settable(pLuaThread->GetLuaState(), -3);
				}
			}
			break;
		default:
			pLuaThread->PushNil();
			break;
		}
	}

	inline bool	Match(TypeWrapper<bool>, lua_State* L, int idx)
	{
		return lua_isboolean(L, idx) || (lua_isnumber(L, idx) != 0);
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
	inline bool	Match(TypeWrapper<CEasyString>, lua_State* L, int idx)
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

	inline bool	Match(TypeWrapper<LUA_PARAMS&>, lua_State* L, int idx)
	{
		return lua_gettop(L) >= idx;
	}

	inline bool	Match(TypeWrapper<LUA_EMPTY_VALUE >, lua_State* L, int idx)
	{
		return lua_gettop(L) >= idx;
	}

	inline bool	Match(TypeWrapper<CLuaBaseMetaClass *>, lua_State* L, int idx);
	
	inline bool	Match(TypeWrapper<CVariedValue>, lua_State* L, int idx)
	{
		int Type = lua_type(L, idx);
		return Type == LUA_TNIL || Type == LUA_TBOOLEAN || Type == LUA_TNUMBER || Type == LUA_TINTEGER || Type == LUA_TSTRING || Type == LUA_TTABLE || Type == LUA_TUSERDATA;
	}
	inline bool	Match(TypeWrapper<CVariedValue&>, lua_State* L, int idx)
	{
		int Type = lua_type(L, idx);
		return Type == LUA_TNIL || Type == LUA_TBOOLEAN || Type == LUA_TNUMBER || Type == LUA_TINTEGER || Type == LUA_TSTRING || Type == LUA_TTABLE || Type == LUA_TUSERDATA;
	}

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
		if (LUA_SCRIPT_CODE_PAGE == CEasyString::SYSTEM_STRING_CODE_PAGE)
		{
			return static_cast<const char*>(lua_tostring(L, idx));
		}
		else
		{
			static CEasyStringA Temp;
			LuaStrToSystemStr(lua_tostring(L, idx), Temp);
			return Temp;
		}
	}
	inline const WCHAR*		Get(TypeWrapper<const WCHAR*>, lua_State* L, int idx)
	{
		static CEasyStringW StringW;
		LuaStrToSystemStr(lua_tostring(L, idx), StringW);
		return StringW;

	}
	inline CEasyString Get(TypeWrapper<CEasyString>, lua_State* L, int idx)
	{
		CEasyString Str;
		LuaStrToSystemStr(lua_tostring(L, idx), Str);
		return Str;

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
	inline LUA_PARAMS	Get(TypeWrapper<LUA_PARAMS&>, lua_State* L, int idx)
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
	inline LUA_EMPTY_VALUE	Get(TypeWrapper<LUA_EMPTY_VALUE>, lua_State* L, int idx)
	{
		return LUA_EMPTY_VALUE();
	}

	
	
	inline CLuaBaseMetaClass * Get(TypeWrapper<CLuaBaseMetaClass *>, lua_State* L, int idx);
	
	inline void	GetVariedValue(lua_State* L, int idx, CVariedValue& Value)
	{
		if (idx < 0)
			idx = lua_gettop(L) + idx + 1;
		switch (lua_type(L, idx))
		{
		case LUA_TBOOLEAN:
			Value = lua_toboolean(L, idx) != 0;
			break;
		case LUA_TNUMBER:
			Value = lua_tonumber(L, idx);
			break;
		case LUA_TSTRING:
			Value = lua_tostring(L, idx);
			break;
		case LUA_TINTEGER:
			Value = lua_tointeger(L, idx);
			break;
		case LUA_TTABLE:
			{
				size_t Len = lua_rawlen(L, idx);
				lua_pushnil(L);
				size_t Count = 0;
				while (lua_next(L, idx))
				{
					if (!lua_isnumber(L, -1))
					{
						lua_pop(L, 2);
						break;
					}	
					lua_pop(L, 1);
					Count++;
				}
				if (Len == Count)
				{
					Value.CreateArray(Count);
					lua_pushnil(L);
					while (lua_next(L, idx))
					{
						CVariedValue* pValue = Value.AddChildNil();
						if (pValue)
							GetVariedValue(L, -1, *pValue);
						lua_pop(L, 1);
					}
				}
				else
				{
					Value.CreateTable(Count);
					lua_pushnil(L);
					while (lua_next(L, idx))
					{
						CVariedValue Key;
						GetVariedValue(L, -2, Key);
						CVariedValue* pValue = Value.AddChildNil(Key);
						if (pValue)
							GetVariedValue(L, -1, *pValue);
						lua_pop(L, 1);
					}
				}
			}
			break;
		case LUA_TUSERDATA:
			{
				void* pData = lua_touserdata(L, idx);
				size_t Len = lua_rawlen(L, idx);
				if (pData && Len)
					Value.SetBinary((const unsigned char*)pData, Len);
			}
			break;
		}		
	}
	inline CVariedValue	Get(TypeWrapper<CVariedValue>, lua_State* L, int idx)
	{
		CVariedValue Value;
		GetVariedValue(L, idx, Value);
		return Value;
	}
	inline CVariedValue	Get(TypeWrapper<CVariedValue&>, lua_State* L, int idx)
	{
		CVariedValue Value;
		GetVariedValue(L, idx, Value);
		return Value;
	}
}