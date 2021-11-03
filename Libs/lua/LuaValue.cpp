#include "stdafx.h"

LuaValue::LuaValue(const CLuaTable& Value)
{	
	*this = Value;
}

LuaValue& LuaValue::operator=(CLuaBaseMetaClass * Value)
{
	Clear();
	Type = Value->GetMetaClassID();
	ObjectValue = Value;
	return *this;
}

LuaValue& LuaValue::operator=(const CLuaTable& Value)
{
	Clear();
	Type = LUA_TTABLE;
	if (TableValue == NULL)
		TableValue = MONITORED_NEW(_T("LuaValue"), CLuaTable);
	*TableValue = Value;
	return *this;
}

LuaValue& LuaValue::operator=(const LuaValue& Value)
{
	Clear();
	Type = Value.Type;
	i64Value = Value.i64Value;
	StrValue = Value.StrValue;
	ObjectValue = Value.ObjectValue;
	if (Value.TableValue != NULL)
	{
		TableValue = MONITORED_NEW(_T("LuaValue"), CLuaTable);
		*TableValue = *Value.TableValue;
	}

	return *this;
}

bool LuaValue::MakePacket(CSmartStruct& Packet, WORD MemberID) const
{
	switch (Type)
	{
	case LUA_TNIL:
		return Packet.AddMemberNull(MemberID);
	case LUA_TBOOLEAN:
		return Packet.AddMember(MemberID, bValue);
	case LUA_TNUMBER:
		return Packet.AddMember(MemberID, fValue);
	case LUA_TSTRING:
		return Packet.AddMember(MemberID, (LPCSTR)StrValue);
	case LUA_TTABLE:
		if (TableValue != NULL)
			return TableValue->MakePacket(Packet, MemberID);
		else
			return false;
	case LUA_TINTEGER:
		return Packet.AddMember(MemberID, i64Value);
	default:
		LogLua(_T("LuaValue::MakePacket:Unsupport Type %d"), Type);
	}
	return false;
}

bool LuaValue::ParsePacket(const CSmartValue& Packet)
{
	switch (Packet.GetType())
	{
	case CSmartValue::VT_NULL:
		Type = LUA_TNIL;
		break;
	case CSmartValue::VT_CHAR:
	case CSmartValue::VT_UCHAR:
	case CSmartValue::VT_SHORT:
	case CSmartValue::VT_USHORT:
	case CSmartValue::VT_INT:
	case CSmartValue::VT_UINT:
	case CSmartValue::VT_FLOAT:
	case CSmartValue::VT_DOUBLE:
		Type = LUA_TNUMBER;
		fValue = (double)Packet;
		break;
	case CSmartValue::VT_UBIGINT:
	case CSmartValue::VT_BIGINT:
		Type = LUA_TINTEGER;
		i64Value = (INT64)Packet;
		break;
	case CSmartValue::VT_STRING_UTF8:
	case CSmartValue::VT_STRING_ANSI:
	case CSmartValue::VT_STRING_UCS16:
		Type = LUA_TSTRING;
		Packet.GetString(StrValue);		
	case CSmartValue::VT_STRUCT:
		Type = LUA_TTABLE;
		TableValue = MONITORED_NEW(_T("LuaValue"), CLuaTable);
		return TableValue->ParsePacket(CSmartStruct(Packet));
		break;
	case CSmartValue::VT_BOOL:
		Type = LUA_TBOOLEAN;
		bValue = (bool)Packet;
		break;
	default:
		Type = LUA_TNIL;
		LogLua(_T("LuaValue::ParsePacket:Unsupport Type %d"), Packet.GetType());
		return false;
	}
	return true;
}

bool LuaValue::PushValueToLuaState(const LuaValue& Value, lua_State * pLuaState)
{
	switch (Value.Type)
	{
	case LUA_TNIL:
		lua_pushnil(pLuaState);
		return true;
	case LUA_TBOOLEAN:
		lua_pushboolean(pLuaState, Value.bValue);
		return true;
	case LUA_TNUMBER:
		lua_pushnumber(pLuaState, Value.fValue);
		return true;
	case LUA_TSTRING:
#ifdef UNICODE
	{
		CEasyStringA Temp = Value.StrValue;
		lua_pushstring(pLuaState, Temp);
	}
#else
		lua_pushstring(pLuaState, Value.StrValue);
#endif
		return true;
	case LUA_TTABLE:
		if (Value.TableValue != NULL)
			Value.TableValue->PushToLuaState(pLuaState);
		else
			lua_pushnil(pLuaState);
		return true;
	case LUA_TINTEGER:
		LuaWrap::Push(pLuaState, Value.i64Value);
		return true;
	default:
		if (Value.Type >= CLuaBaseMetaClass::CLASS_ID)
		{
			void *buf = lua_newuserdata(pLuaState, sizeof(CLuaBaseMetaClass *));
			memcpy(buf, &Value.ObjectValue, sizeof(CLuaBaseMetaClass *));
			if (Value.ObjectValue)
				Value.ObjectValue->SetMetaClass(pLuaState);
			return true;
		}
		else if (Value.Type >= CLuaBaseStaticMetaClass::CLASS_ID)
		{
			if (Value.ObjectData.GetUsedSize() >= sizeof(CLuaBaseStaticMetaClass))
			{
				void * pBuff = lua_newuserdata(pLuaState, Value.ObjectData.GetUsedSize());
				memcpy(pBuff, Value.ObjectData.GetBuffer(), Value.ObjectData.GetUsedSize());
				if (CLuaBaseStaticMetaClass::SetMetaClassByObject(pLuaState, (CLuaBaseStaticMetaClass *)pBuff))
				{
					return true;
				}
				else
				{
					lua_pop(pLuaState, 1);
					lua_pushnil(pLuaState);
				}
			}
			else
			{
				lua_pushnil(pLuaState);
			}
		}
		else
		{
			lua_pushnil(pLuaState);
		}
	}
	return false;
}
bool LuaValue::GetValueFromLuaState(LuaValue& Value, lua_State * pLuaState, int Index)
{
	Value.Type = GetLuaObjectType(pLuaState, Index);
	switch (Value.Type)
	{
	case LUA_TNIL:
		return true;
	case LUA_TBOOLEAN:
		Value.bValue = lua_toboolean(pLuaState, Index) != 0;
		return true;
	case LUA_TNUMBER:
		Value.fValue = lua_tonumber(pLuaState, Index);
		return true;
	case LUA_TSTRING:
		Value.StrValue = lua_tostring(pLuaState, Index);
		return true;
	case LUA_TTABLE:
		Value.TableValue = MONITORED_NEW(_T("LuaValue"), CLuaTable);
		Value.TableValue->GetFromLuaState(pLuaState, Index);
	case LUA_TINTEGER:
		Value.i64Value = lua_tointeger(pLuaState, Index);
		return true;
	default:
		if (Value.Type >= CLuaBaseMetaClass::CLASS_ID)
		{
			Value.ObjectValue = *((CLuaBaseMetaClass **)lua_touserdata(pLuaState, Index));
			return true;
		}
		else if (Value.Type >= CLuaBaseStaticMetaClass::CLASS_ID)
		{
			void * pData = lua_touserdata(pLuaState, Index);
			size_t DataLen = lua_rawlen(pLuaState, Index);
			if (pData&&DataLen >= sizeof(CLuaBaseStaticMetaClass))
			{
				Value.ObjectData.Create((UINT)DataLen);
				Value.ObjectData.PushBack(pData, (UINT)DataLen);
				return true;
			}
		}
	}
	return false;
}