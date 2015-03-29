#include "stdafx.h"

CLuaTable::CLuaTable(void)
{
}

CLuaTable::~CLuaTable(void)
{
}

bool CLuaTable::PushToLuaState(lua_State * pLuaState)
{
	lua_newtable(pLuaState);
	for (size_t i = 0; i < GetCount(); i++)
	{
		if (PushElementToLuaState(i, pLuaState))
		{
			lua_settable(pLuaState, -3);
		}
		else
		{
			luaL_error(pLuaState, "Table Element[%d] ValueType Is Invalid", i);
			return false;
		}
	}
	return true;
}
bool CLuaTable::GetFromLuaState(lua_State * pLuaState, int Index)
{
	if (!lua_istable(pLuaState, Index))
	{
		luaL_error(pLuaState, "is not table");
		return false;
	}
	lua_pushnil(pLuaState);  /* 第一个 key */
	if (Index < 0)
		Index--;
	while (lua_next(pLuaState, Index) != 0)
	{
		/* 用一下 'key' （在索引 -2 处） 和 'value' （在索引 -1 处） */
		if (!GetElementFromLuaState(pLuaState, -1))
		{
			luaL_error(pLuaState, "unsupport table value");
			return false;
		}
		lua_pop(pLuaState, 1);
	}
	return true;
}

bool CLuaTable::PushElementToLuaState(UINT ElementIndex, lua_State * pLuaState)
{
	if (ElementIndex >= GetCount())
		return false;
	LuaTableElement * pElement = GetObject(ElementIndex);
	if (!PushValueToLuaState(pElement->Key, pLuaState))
		return false;
	if (!PushValueToLuaState(pElement->Value, pLuaState))
		return false;
	return true;	
}
bool CLuaTable::GetElementFromLuaState(lua_State * pLuaState, int Index)
{
	LuaTableElement Element;
	if (!GetValueFromLuaState(Element.Value, pLuaState, Index))
		return false;
	if (!GetValueFromLuaState(Element.Key, pLuaState, Index - 1))
		return false;
	Add(Element);
	return true;
}

bool CLuaTable::PushValueToLuaState(LuaValue& Value, lua_State * pLuaState)
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
		if (Value.TableValue!=NULL)
			Value.TableValue->PushToLuaState(pLuaState);
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
		else
		{
			return false;
		}
	}
}
bool CLuaTable::GetValueFromLuaState(LuaValue& Value, lua_State * pLuaState, int Index)
{
	Value.Type = LuaWrap::GetLuaObjectType(pLuaState, Index);
	switch (Value.Type)
	{
	case LUA_TNIL:
		return true;
	case LUA_TBOOLEAN:
		Value.bValue = lua_toboolean(pLuaState, Index);
		return true;
	case LUA_TNUMBER:
		Value.fValue = lua_tonumber(pLuaState, Index);
		return true;
	case LUA_TSTRING:
		Value.StrValue = lua_tostring(pLuaState, Index);
		return true;
	case LUA_TTABLE:
		Value.TableValue = new CLuaTable();
		return Value.TableValue->GetFromLuaState(pLuaState, Index);
	case LUA_TINTEGER:
		Value.i64Value = lua_tointeger(pLuaState, Index);
		return true;
	default:
		if (Value.Type >= CLuaBaseMetaClass::CLASS_ID)
		{
			Value.ObjectValue = *((CLuaBaseMetaClass **)lua_touserdata(pLuaState, Index));
			return true;
		}
		else
		{
			return false;
		}
	}
}