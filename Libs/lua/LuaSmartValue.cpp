#include "stdafx.h"

IMPLEMENT_STATIC_META_CLASS(CLuaSmartValue)

void CLuaSmartValue::PushValueToLua(lua_State * L, const CSmartValue& Value)
{
	switch (Value.GetType())
	{
	case CSmartValue::VT_CHAR:
	case CSmartValue::VT_UCHAR:
	case CSmartValue::VT_SHORT:
	case CSmartValue::VT_USHORT:
	case CSmartValue::VT_INT:
	case CSmartValue::VT_UINT:
	case CSmartValue::VT_BIGINT:
	case CSmartValue::VT_UBIGINT:
		lua_pushinteger(L, Value);
		break;
	case CSmartValue::VT_FLOAT:
	case CSmartValue::VT_DOUBLE:
		lua_pushnumber(L, Value);
		break;
	case CSmartValue::VT_STRING_UTF8:
	case CSmartValue::VT_STRING_ANSI:
		lua_pushstring(L, (LPCSTR)Value);
		break;
	case CSmartValue::VT_STRING_UCS16:
		{
			CEasyStringA StringA;
			StringA = (LPCWSTR)Value;
			lua_pushstring(L, StringA);
		}
		break;
	case CSmartValue::VT_STRUCT:
		LuaWrap::Push(L, CLuaSmartStruct(Value));
		break;
	case CSmartValue::VT_BOOL:
		lua_pushboolean(L, Value);
		break;
	default:
		lua_pushnil(L);
	}
}

void CLuaSmartValue::RegisterMemberFunctions(lua_State * pLuaState) const
{
	CLuaBaseStaticMetaClass::RegisterMemberFunctions(pLuaState);
	
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, "GetData", &CLuaSmartValue::LuaGetData);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, "GetDataClone", &CLuaSmartValue::LuaGetDataClone);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, "GetDataLen", &CLuaSmartValue::LuaGetDataLen);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, "GetLength", &CLuaSmartValue::LuaGetLength);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, "Attach", &CLuaSmartValue::LuaAttach);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, "GetType", &CLuaSmartValue::LuaGetType);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, "SetValue", &CLuaSmartValue::LuaSetValue);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, "GetValue", &CLuaSmartValue::LuaGetValue);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, "GetValueAsByteArray", &CLuaSmartValue::LuaGetValueAsByteArray);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, "GetValueAsBoolean", &CLuaSmartValue::LuaGetValueAsBoolean);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, "GetValueAsSByte", &CLuaSmartValue::LuaGetValueAsSByte);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, "GetValueAsByte", &CLuaSmartValue::LuaGetValueAsByte);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, "GetValueAsShort", &CLuaSmartValue::LuaGetValueAsShort);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, "GetValueAsUShort", &CLuaSmartValue::LuaGetValueAsUShort);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, "GetValueAsInt", &CLuaSmartValue::LuaGetValueAsInt);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, "GetValueAsUInt", &CLuaSmartValue::LuaGetValueAsUInt);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, "GetValueAsInt64", &CLuaSmartValue::LuaGetValueAsInt64);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, "GetValueAsUInt64", &CLuaSmartValue::LuaGetValueAsUInt64);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, "GetValueAsFloat", &CLuaSmartValue::LuaGetValueAsFloat);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, "GetValueAsDouble", &CLuaSmartValue::LuaGetValueAsDouble);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, "GetValueAsString", &CLuaSmartValue::LuaGetValueAsString);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, "GetValueAsSmartStruct", &CLuaSmartValue::LuaGetValueAsSmartStruct);	
}

LUA_EMPTY_VALUE CLuaSmartValue::LuaGetData(CLuaThread * pThreadInfo)
{
	if (m_Data.GetData() && m_Data.GetDataLen())
		LuaWrap::PushRef(pThreadInfo->GetLuaState(), CLuaByteArray(m_Data.GetData(), m_Data.GetDataLen()));
	else
		pThreadInfo->PushNil();
	return LUA_EMPTY_VALUE();
}

LUA_EMPTY_VALUE CLuaSmartValue::LuaGetDataClone(CLuaThread * pThreadInfo)
{
	if (m_Data.GetData() && m_Data.GetDataLen())
		LuaWrap::Push(pThreadInfo->GetLuaState(), CLuaByteArray(m_Data.GetData(), m_Data.GetDataLen()));
	else
		pThreadInfo->PushNil();
	return LUA_EMPTY_VALUE();
}

UINT CLuaSmartValue::LuaGetDataLen(CLuaThread * pThreadInfo)
{
	return m_Data.GetDataLen();
}
UINT CLuaSmartValue::LuaGetLength(CLuaThread * pThreadInfo)
{
	return m_Data.GetLength();
}
bool CLuaSmartValue::LuaAttach(CLuaThread * pThreadInfo, LUA_EMPTY_VALUE)
{
	int Type = GetLuaObjectType(pThreadInfo->GetLuaState(), 2);
	switch (Type)
	{
	case CLuaByteArray::CLASS_ID:
	case CLuaSmartValue::CLASS_ID:
	case CLuaSmartStruct::CLASS_ID:
		{
			CLuaBaseStaticMetaClass * pObject = dynamic_cast<CLuaBaseStaticMetaClass *>((CLuaBaseStaticMetaClass *)lua_touserdata(pThreadInfo->GetLuaState(), 2));
			if (pObject)
			{
				return m_Data.Attach(pObject->GetData(), pObject->GetDataLen());
			}
		}
		break;
	}
	return false;
}
int CLuaSmartValue::LuaGetType(CLuaThread * pThreadInfo)
{
	return m_Data.GetType();
}
bool CLuaSmartValue::LuaSetValue(CLuaThread * pThreadInfo, LUA_EMPTY_VALUE)
{
	int Type = GetLuaObjectType(pThreadInfo->GetLuaState(), 2);
	switch (Type)
	{
	case LUA_TNIL:
		m_Data.Destory();
		return true;
	case LUA_TBOOLEAN:
		m_Data.SetValue(lua_toboolean(pThreadInfo->GetLuaState(), 2) != 0);
		return true;
	case LUA_TNUMBER:
		m_Data.SetValue(lua_tonumber(pThreadInfo->GetLuaState(), 2));
		return true;
	case LUA_TSTRING:
		m_Data.SetValue(lua_tostring(pThreadInfo->GetLuaState(), 2));
		return true;
	case LUA_TTABLE:
		return false;
	case LUA_TINTEGER:
		m_Data.SetValue(lua_tointeger(pThreadInfo->GetLuaState(), 2));
		return true;
	case CLuaByteArray::CLASS_ID:
		{
			CLuaSmartValue * pObject = dynamic_cast<CLuaSmartValue *>((CLuaSmartValue *)lua_touserdata(pThreadInfo->GetLuaState(), 2));
			if (pObject)
			{
				m_Data.SetString((LPCSTR)pObject->GetData(), pObject->GetDataLen());
				return true;
			}
		}
		break;
	case CLuaSmartValue::CLASS_ID:
		{
			CLuaSmartValue * pObject = dynamic_cast<CLuaSmartValue *>((CLuaSmartValue *)lua_touserdata(pThreadInfo->GetLuaState(), 2));
			if (pObject)
			{
				m_Data.SetValue(pObject->m_Data);
				return true;
			}
		}
		break;
	case CLuaSmartStruct::CLASS_ID:
		{
			CLuaSmartStruct * pObject = dynamic_cast<CLuaSmartStruct *>((CLuaSmartStruct *)lua_touserdata(pThreadInfo->GetLuaState(), 2));
			if (pObject)
			{
				m_Data.SetValue(pObject->GetValue());
				return true;
			}
		}
		break;
	}
	return false;
}
LUA_EMPTY_VALUE CLuaSmartValue::LuaGetValue(CLuaThread * pThreadInfo)
{
	CLuaSmartValue::PushValueToLua(pThreadInfo->GetLuaState(), m_Data);
	return LUA_EMPTY_VALUE();
}
LUA_EMPTY_VALUE CLuaSmartValue::LuaGetValueAsByteArray(CLuaThread * pThreadInfo)
{
	LPVOID pData = m_Data.GetValueData();
	UINT DataSize = m_Data.GetLength();
	if (pData&&DataSize)
	{
		LuaWrap::PushRef(pThreadInfo->GetLuaState(), CLuaByteArray(pData, DataSize));
	}
	else
	{
		pThreadInfo->PushNil();
	}
	return LUA_EMPTY_VALUE();
}
bool CLuaSmartValue::LuaGetValueAsBoolean(CLuaThread * pThreadInfo)
{
	return m_Data;
}
char CLuaSmartValue::LuaGetValueAsSByte(CLuaThread * pThreadInfo)
{
	return m_Data;
}
BYTE CLuaSmartValue::LuaGetValueAsByte(CLuaThread * pThreadInfo)
{
	return m_Data;
}
short CLuaSmartValue::LuaGetValueAsShort(CLuaThread * pThreadInfo)
{
	return m_Data;
}
WORD CLuaSmartValue::LuaGetValueAsUShort(CLuaThread * pThreadInfo)
{
	return m_Data;
}
int CLuaSmartValue::LuaGetValueAsInt(CLuaThread * pThreadInfo)
{
	return m_Data;
}
UINT CLuaSmartValue::LuaGetValueAsUInt(CLuaThread * pThreadInfo)
{
	return m_Data;
}
INT64 CLuaSmartValue::LuaGetValueAsInt64(CLuaThread * pThreadInfo)
{
	return m_Data;
}
UINT64 CLuaSmartValue::LuaGetValueAsUInt64(CLuaThread * pThreadInfo)
{
	return m_Data;
}
float CLuaSmartValue::LuaGetValueAsFloat(CLuaThread * pThreadInfo)
{
	return m_Data;
}
double CLuaSmartValue::LuaGetValueAsDouble(CLuaThread * pThreadInfo)
{
	return m_Data;
}
LPCTSTR CLuaSmartValue::LuaGetValueAsString(CLuaThread * pThreadInfo)
{
	return m_Data;
}
LUA_EMPTY_VALUE CLuaSmartValue::LuaGetValueAsSmartStruct(CLuaThread * pThreadInfo)
{
	LuaWrap::PushRef(pThreadInfo->GetLuaState(), CLuaSmartValue(m_Data));
	return LUA_EMPTY_VALUE();
}