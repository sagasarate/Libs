#pragma once

class CLuaBaseMetaClass;
class CLuaTable;
struct LuaValue
{
	int Type;
	union
	{
		bool				bValue;
		double				fValue;
		__int64				i64Value;
	};
	CEasyStringA			StrValue;
	CSmartPtr<CLuaTable>	TableValue;
	CLuaBaseMetaClass *		ObjectValue;
	CEasyBuffer				ObjectData;

	LuaValue()
	{
		Clear();
	}

	void Clear()
	{
		Type = LUA_TNIL;
		i64Value = 0;
		ObjectValue = NULL;
		TableValue = NULL;
	}

	LuaValue(bool Value)
	{
		*this = Value;
	}

	LuaValue(char Value)
	{
		*this = Value;
	}

	LuaValue(unsigned char Value)
	{
		*this = Value;
	}

	LuaValue(short Value)
	{
		*this = Value;
	}

	LuaValue(unsigned short Value)
	{
		*this = Value;
	}

	LuaValue(int Value)
	{
		*this = Value;
	}

	LuaValue(unsigned int Value)
	{
		*this = Value;
	}

	LuaValue(__int64 Value)
	{
		*this = Value;
	}

	LuaValue(unsigned __int64 Value)
	{
		*this = Value;
	}

	LuaValue(float Value)
	{
		*this = Value;
	}

	LuaValue(double Value)
	{
		*this = Value;
	}
	LuaValue(const char * Value)
	{
		*this = Value;
	}
	LuaValue(const CEasyString& Value)
	{
		*this = Value;
	}
	LuaValue(CLuaBaseMetaClass * Value)
	{
		*this = Value;
	}

	LuaValue(const CLuaTable& Value);

	LuaValue(const LuaValue& Value)
	{
		*this = Value;
	}

	operator bool()
	{
		switch (Type)
		{
		case LUA_TBOOLEAN:
			return bValue;
		case LUA_TNUMBER:
			return fValue != 0;
		case LUA_TINTEGER:
			return i64Value != 0;
		}
		return false;
	}

	operator char()
	{
		switch (Type)
		{
		case LUA_TBOOLEAN:
			return bValue ? 1 : 0;
		case LUA_TNUMBER:
			return (char)fValue;
		case LUA_TINTEGER:
			return (char)i64Value;
		}
		return 0;
	}

	operator unsigned char()
	{
		switch (Type)
		{
		case LUA_TBOOLEAN:
			return bValue ? 1 : 0;
		case LUA_TNUMBER:
			return (unsigned char)fValue;
		case LUA_TINTEGER:
			return (unsigned char)i64Value;
		}
		return 0;
	}

	operator short()
	{
		switch (Type)
		{
		case LUA_TBOOLEAN:
			return bValue ? 1 : 0;
		case LUA_TNUMBER:
			return (short)fValue;
		case LUA_TINTEGER:
			return (short)i64Value;
		}
		return 0;
	}

	operator unsigned short()
	{
		switch (Type)
		{
		case LUA_TBOOLEAN:
			return bValue ? 1 : 0;
		case LUA_TNUMBER:
			return (unsigned short)fValue;
		case LUA_TINTEGER:
			return (unsigned short)i64Value;
		}
		return 0;
	}

	operator int()
	{
		switch (Type)
		{
		case LUA_TBOOLEAN:
			return bValue ? 1 : 0;
		case LUA_TNUMBER:
			return (int)fValue;
		case LUA_TINTEGER:
			return (int)i64Value;
		}
		return 0;
	}

	operator unsigned int()
	{
		switch (Type)
		{
		case LUA_TBOOLEAN:
			return bValue ? 1 : 0;
		case LUA_TNUMBER:
			return (unsigned int)fValue;
		case LUA_TINTEGER:
			return (unsigned int)i64Value;
		}
		return 0;
	}

	operator __int64()
	{
		switch (Type)
		{
		case LUA_TBOOLEAN:
			return bValue ? 1 : 0;
		case LUA_TNUMBER:
			return (__int64)fValue;
		case LUA_TINTEGER:
			return (__int64)i64Value;
		}
		return 0;
	}

	operator unsigned __int64()
	{
		switch (Type)
		{
		case LUA_TBOOLEAN:
			return bValue ? 1 : 0;
		case LUA_TNUMBER:
			return (unsigned __int64)fValue;
		case LUA_TINTEGER:
			return (unsigned __int64)i64Value;
		}
		return 0;
	}

	operator float()
	{
		switch (Type)
		{
		case LUA_TBOOLEAN:
			return bValue ? 1.0f : 0.0f;
		case LUA_TNUMBER:
			return (float)fValue;
		case LUA_TINTEGER:
			return (float)i64Value;
		}
		return 0;
	}

	operator double()
	{
		switch (Type)
		{
		case LUA_TBOOLEAN:
			return bValue ? 1 : 0;
		case LUA_TNUMBER:
			return (double)fValue;
		case LUA_TINTEGER:
			return (double)i64Value;
		}
		return 0;
	}

	operator const char * ()
	{
		return StrValue;
	}

	operator const CEasyString ()
	{
		return StrValue;
	}

	operator CLuaBaseMetaClass *()
	{
		return ObjectValue;
	}

	bool operator=(bool Value)
	{
		Clear();
		Type = LUA_TBOOLEAN;
		bValue = Value;
		return bValue;
	}

	char operator=(char Value)
	{
		Clear();
		Type = LUA_TNUMBER;
		fValue = Value;
		return (char)fValue;
	}

	unsigned char operator=(unsigned char Value)
	{
		Clear();
		Type = LUA_TNUMBER;
		fValue = Value;
		return (unsigned char)fValue;
	}

	short operator=(short Value)
	{
		Clear();
		Type = LUA_TNUMBER;
		fValue = Value;
		return (short)fValue;
	}

	unsigned short operator=(unsigned short Value)
	{
		Clear();
		Type = LUA_TNUMBER;
		fValue = Value;
		return (unsigned short)fValue;
	}

	int operator=(int Value)
	{
		Clear();
		Type = LUA_TNUMBER;
		fValue = Value;
		return (int)fValue;
	}

	unsigned int operator=(unsigned int Value)
	{
		Clear();
		Type = LUA_TNUMBER;
		fValue = Value;
		return (unsigned int)fValue;
	}

	__int64 operator=(__int64 Value)
	{
		Clear();
		Type = LUA_TINTEGER;
		i64Value = Value;
		return i64Value;
	}

	unsigned __int64 operator=(unsigned __int64 Value)
	{
		Clear();
		Type = LUA_TINTEGER;
		i64Value = Value;
		return i64Value;
	}

	float operator=(float Value)
	{
		Clear();
		Type = LUA_TNUMBER;
		fValue = Value;
		return (float)fValue;
	}

	double operator=(double Value)
	{
		Clear();
		Type = LUA_TNUMBER;
		fValue = Value;
		return fValue;
	}
	const char * operator=(const char * Value)
	{
		Clear();
		Type = LUA_TSTRING;
		StrValue = Value;
		return StrValue;
	}
	const char * operator=(const CEasyString& Value)
	{
		Clear();
		Type = LUA_TSTRING;
		StrValue = Value;
		return StrValue;
	}

	LuaValue& operator=(CLuaBaseMetaClass * Value);

	LuaValue& operator=(const CLuaTable& Value);

	LuaValue& operator=(const LuaValue& Value);

	bool MakePacket(CSmartStruct& Packet, WORD MemberID) const;
	bool ParsePacket(const CSmartValue& Packet);

	static bool PushValueToLuaState(const LuaValue& Value, lua_State * pLuaState);
	static bool GetValueFromLuaState(LuaValue& Value, lua_State * pLuaState, int Index);
};

inline void PacketLuaValueArray(CSmartStruct& Packet, const CEasyArray<LuaValue>& LuaValueList)
{
	for (UINT i = 0; i < LuaValueList.GetCount(); i++)
	{
		LuaValueList.GetObjectConst(i).MakePacket(Packet, i + 1);
	}
}

inline void ParseLuaValueArray(const CSmartStruct& Packet, CEasyArray<LuaValue>& LuaValueList)
{
	void * Pos = Packet.GetFirstMemberPosition();
	while (Pos)
	{
		WORD MemberID;
		CSmartValue Value = Packet.GetNextMember(Pos, MemberID);
		LuaValue * pLuaValue = LuaValueList.AddEmpty();
		pLuaValue->ParsePacket(Value);
	}
}