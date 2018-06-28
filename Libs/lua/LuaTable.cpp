#include "stdafx.h"



CLuaTable::CLuaTable(void)
{
}

CLuaTable::~CLuaTable(void)
{
}

bool CLuaTable::PushToLuaState(lua_State * pLuaState) const
{
	lua_newtable(pLuaState);
	for (size_t i = 0; i < GetCount(); i++)
	{
		if (PushElementToLuaState((UINT)i, pLuaState))
		{
			lua_settable(pLuaState, -3);
		}
		else
		{
			LogLua(_T("Table Element[%d] ValueType Is Invalid"), i);
			return false;
		}
	}
	return true;
}
bool CLuaTable::GetFromLuaState(lua_State * pLuaState, int Index)
{
	if (!lua_istable(pLuaState, Index))
	{
		LogLua(_T("is not table"));
		return false;
	}
	Clear();
	lua_pushnil(pLuaState);  /* 第一个 key */
	if (Index < 0)
		Index--;
	while (lua_next(pLuaState, Index) != 0)
	{
		/* 用一下 'key' （在索引 -2 处） 和 'value' （在索引 -1 处） */
		if (!GetElementFromLuaState(pLuaState, -1))
		{
			LogLua(_T("unsupport table value"));
			return false;
		}
		lua_pop(pLuaState, 1);
	}
	return true;
}

bool CLuaTable::PushElementToLuaState(UINT ElementIndex, lua_State * pLuaState) const
{
	if (ElementIndex >= GetCount())
		return false;
	const LuaTableElement * pElement = &GetObjectConst(ElementIndex);
	if (!LuaValue::PushValueToLuaState(pElement->Key, pLuaState))
		return false;
	if (!LuaValue::PushValueToLuaState(pElement->Value, pLuaState))
		return false;
	return true;	
}
bool CLuaTable::GetElementFromLuaState(lua_State * pLuaState, int Index)
{
	LuaTableElement Element;
	if (!LuaValue::GetValueFromLuaState(Element.Value, pLuaState, Index))
		return false;
	if (!LuaValue::GetValueFromLuaState(Element.Key, pLuaState, Index - 1))
		return false;
	Add(Element);
	return true;
}



bool CLuaTable::MakePacket(CSmartStruct& Packet) const
{
	for (size_t i = 0; i < GetCount(); i++)
	{
		if (!GetObjectConst(i).Key.MakePacket(Packet, CLuaThread::SSID_LUA_VALUE_TABLE_KEY))
			return false;
		if (!GetObjectConst(i).Value.MakePacket(Packet, CLuaThread::SSID_LUA_VALUE_TABLE_VALUE))
			return false;
	}
	return true;
}

bool CLuaTable::MakePacket(CSmartStruct& Packet, WORD MemberID) const
{
	UINT BufferSize;
	void * pBuffer = Packet.PrepareMember(BufferSize);
	CSmartStruct SubPacket(pBuffer, BufferSize, true);
	if (!MakePacket(SubPacket))
		return false;
	return Packet.FinishMember(MemberID, SubPacket.GetDataLen());
}

bool CLuaTable::ParsePacket(const CSmartStruct& Packet)
{
	LuaTableElement Element;
	void * Pos = Packet.GetFirstMemberPosition();
	while (Pos)
	{
		WORD MemberID;
		CSmartValue Value = Packet.GetNextMember(Pos, MemberID);
		if (MemberID == CLuaThread::SSID_LUA_VALUE_TABLE_KEY)
		{
			Element.Key.ParsePacket(Value);
		}
		else if (MemberID == CLuaThread::SSID_LUA_VALUE_TABLE_VALUE)
		{
			Element.Value.ParsePacket(Value);
			Add(Element);
			Element.Key.Clear();
			Element.Value.Clear();
		}
	}
	return true;
}