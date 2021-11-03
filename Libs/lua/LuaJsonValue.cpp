#include "stdafx.h"

IMPLEMENT_STATIC_META_CLASS(CLuaJsonValue)



void CLuaJsonValue::RegisterMemberFunctions(lua_State * pLuaState) const
{
	CLuaBaseStaticMetaClass::RegisterMemberFunctions(pLuaState);

	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "GetType", &CLuaJsonValue::LuaGetType);
	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "IsNull", &CLuaJsonValue::LuaIsNull);
	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "IsBool", &CLuaJsonValue::LuaIsBool);
	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "IsObject", &CLuaJsonValue::LuaIsObject);
	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "IsArray", &CLuaJsonValue::LuaIsArray);
	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "IsNumber", &CLuaJsonValue::LuaIsNumber);
	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "IsInt", &CLuaJsonValue::LuaIsInt);
	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "IsUint", &CLuaJsonValue::LuaIsUint);
	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "IsInt64", &CLuaJsonValue::LuaIsInt64);
	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "IsUint64", &CLuaJsonValue::LuaIsUint64);
	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "IsDouble", &CLuaJsonValue::LuaIsDouble);
	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "IsString", &CLuaJsonValue::LuaIsString);

	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "GetBool", &CLuaJsonValue::LuaGetBool);
	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "GetInt", &CLuaJsonValue::LuaGetInt);
	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "GetUInt", &CLuaJsonValue::LuaGetUInt);
	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "GetInt64", &CLuaJsonValue::LuaGetInt64);
	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "GetUInt64", &CLuaJsonValue::LuaGetUInt64);
	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "GetDouble", &CLuaJsonValue::LuaGetDouble);
	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "GetString", &CLuaJsonValue::LuaGetString);
	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "GetStringLength", &CLuaJsonValue::LuaGetStringLength);

	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "HasMember", &CLuaJsonValue::LuaHasMember);
	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "GetSize", &CLuaJsonValue::LuaGetSize);
	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "GetMemberCount", &CLuaJsonValue::LuaGetMemberCount);
	RegisterMetaFunction<CLuaJsonValue>(pLuaState, "GetMember", &CLuaJsonValue::LuaGetMember);
}


LPCSTR CLuaJsonValue::LuaGetType(CLuaThread * pThreadInfo)
{
	if (m_Data.IsNull())
		return "null";
	else if (m_Data.IsBool())
		return "bool";
	else if (m_Data.IsObject())
		return "object";
	else if (m_Data.IsArray())
		return "array";
	else if (m_Data.IsNumber())
		return "number";
	else if (m_Data.IsInt())
		return "int";
	else if (m_Data.IsUint())
		return "uint";
	else if (m_Data.IsInt64())
		return "int64";
	else if (m_Data.IsUint64())
		return "uint64";
	else if (m_Data.IsDouble())
		return "double";
	else if (m_Data.IsString())
		return "string";
	else
		return "unknow";		
}

bool CLuaJsonValue::LuaIsNull(CLuaThread * pThreadInfo)
{
	return m_Data.IsNull();
}
bool CLuaJsonValue::LuaIsBool(CLuaThread * pThreadInfo)
{
	return m_Data.IsBool();
}
bool CLuaJsonValue::LuaIsObject(CLuaThread * pThreadInfo)
{
	return m_Data.IsObject();
}
bool CLuaJsonValue::LuaIsArray(CLuaThread * pThreadInfo)
{
	return m_Data.IsArray();
}
bool CLuaJsonValue::LuaIsNumber(CLuaThread * pThreadInfo)
{
	return m_Data.IsNumber();
}
bool CLuaJsonValue::LuaIsInt(CLuaThread * pThreadInfo)
{
	return m_Data.IsInt();
}
bool CLuaJsonValue::LuaIsUint(CLuaThread * pThreadInfo)
{
	return m_Data.IsUint();
}
bool CLuaJsonValue::LuaIsInt64(CLuaThread * pThreadInfo)
{
	return m_Data.IsInt64();
}
bool CLuaJsonValue::LuaIsUint64(CLuaThread * pThreadInfo)
{
	return m_Data.IsUint64();
}
bool CLuaJsonValue::LuaIsDouble(CLuaThread * pThreadInfo)
{
	return m_Data.IsDouble();
}
bool CLuaJsonValue::LuaIsString(CLuaThread * pThreadInfo)
{
	return m_Data.IsString();
}

bool CLuaJsonValue::LuaGetBool(CLuaThread * pThreadInfo)
{
	return m_Data.GetBool();
}
int CLuaJsonValue::LuaGetInt(CLuaThread * pThreadInfo)
{
	return m_Data.GetInt();
}
UINT CLuaJsonValue::LuaGetUInt(CLuaThread * pThreadInfo)
{
	return m_Data.GetUint();
}
INT64 CLuaJsonValue::LuaGetInt64(CLuaThread * pThreadInfo)
{
	return m_Data.GetInt64();
}
UINT64 CLuaJsonValue::LuaGetUInt64(CLuaThread * pThreadInfo)
{
	return m_Data.GetUint64();
}
double CLuaJsonValue::LuaGetDouble(CLuaThread * pThreadInfo)
{
	return m_Data.GetDouble();
}
LPCTSTR CLuaJsonValue::LuaGetString(CLuaThread * pThreadInfo)
{
	return m_Data.GetString();
}
UINT CLuaJsonValue::LuaGetStringLength(CLuaThread * pThreadInfo)
{
	return m_Data.GetStringLength();
}
bool CLuaJsonValue::LuaHasMember(CLuaThread * pThreadInfo, LPCTSTR MemberName)
{
	return m_Data.HasMember(MemberName);
}
UINT CLuaJsonValue::LuaGetSize(CLuaThread * pThreadInfo)
{
	if (m_Data.IsArray())
		return m_Data.Size();
	else if (m_Data.IsObject())
		return m_Data.MemberCount();
	else
		luaL_error(pThreadInfo->GetLuaState(), "json not array or object");
	return 0;
}
UINT CLuaJsonValue::LuaGetMemberCount(CLuaThread * pThreadInfo)
{
	if (m_Data.IsArray())
		return m_Data.Size();
	else if (m_Data.IsObject())
		return m_Data.MemberCount();
	else
		luaL_error(pThreadInfo->GetLuaState(), "json not array or object");
	return 0;
}
LUA_EMPTY_VALUE CLuaJsonValue::LuaGetMember(CLuaThread * pThreadInfo)
{
	int Type = lua_type(pThreadInfo->GetLuaState(), -1);
	if (Type == LUA_TNUMBER)
	{
		if (m_Data.IsArray())
		{
			UINT64 Index = lua_tointeger(pThreadInfo->GetLuaState(), -1);
			Index--;
			if (Index < m_Data.Size())
			{
				LuaWrap::Push(pThreadInfo->GetLuaState(), CLuaJsonValue(m_Data[(rapidjson::SizeType)Index]));
			}
			else
			{
				lua_pushnil(pThreadInfo->GetLuaState());
			}
		}
		else
		{
			luaL_argerror(pThreadInfo->GetLuaState(), 1, "json must array");
		}
	}
	else if (Type == LUA_TSTRING)
	{
		if (m_Data.IsObject())
		{
#ifdef UNICODE
			CEasyStringW MemberName = lua_tostring(L, -1);
#else
			LPCSTR MemberName = lua_tostring(pThreadInfo->GetLuaState(), -1);
#endif
			if (m_Data.HasMember(MemberName))
			{
				LuaWrap::Push(pThreadInfo->GetLuaState(), CLuaJsonValue(m_Data[MemberName]));
			}
			else
			{
				lua_pushnil(pThreadInfo->GetLuaState());
			}
		}
		else
		{
			luaL_argerror(pThreadInfo->GetLuaState(), 1, "json must object");
		}
	}
	else
	{
		lua_pushnil(pThreadInfo->GetLuaState());
	}
	return LUA_EMPTY_VALUE();
}
