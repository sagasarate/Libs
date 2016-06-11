#pragma once



struct LuaTableElement
{
	LuaValue		Key;
	LuaValue		Value;
};

class CLuaTable :
	public CEasyArray<LuaTableElement>,
	public CNameObject
{
public:
	CLuaTable(void);
	~CLuaTable(void);

	void Release()
	{
		delete this;
	}
	void AddPair(const LuaValue& Key, const LuaValue& Value)
	{
		LuaTableElement * pElement = AddEmpty();
		pElement->Key = Key;
		pElement->Value = Value;
	}
	bool PushToLuaState(lua_State * pLuaState) const;
	bool GetFromLuaState(lua_State * pLuaState, int Index);
	
	bool MakePacket(CSmartStruct& Packet) const;
	bool MakePacket(CSmartStruct& Packet, WORD MemberID) const;
	bool ParsePacket(const CSmartStruct& Packet);
protected:
	bool PushElementToLuaState(UINT ElementIndex, lua_State * pLuaState) const;
	bool GetElementFromLuaState(lua_State * pLuaState, int Index);
	
};
