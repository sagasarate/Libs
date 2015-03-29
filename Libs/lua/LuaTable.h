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
	CEasyString				StrValue;
	CLuaBaseMetaClass *		ObjectValue;
	CSmartPtr<CLuaTable>	TableValue;

	LuaValue()
	{
		Clear();
	}

	void Clear()
	{
		Type = LUA_TNIL;
		i64Value = 0;
		ObjectValue = NULL;
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
			return bValue ? 1 : 0;
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

	bool operator=(bool Value)
	{
		Type = LUA_TBOOLEAN;
		bValue = Value;
		return bValue;
	}

	char operator=(char Value)
	{
		Type = LUA_TNUMBER;
		fValue = Value;
		return fValue;
	}

	unsigned char operator=(unsigned char Value)
	{
		Type = LUA_TNUMBER;
		fValue = Value;
		return fValue;
	}

	short operator=(short Value)
	{
		Type = LUA_TNUMBER;
		fValue = Value;
		return fValue;
	}

	unsigned short operator=(unsigned short Value)
	{
		Type = LUA_TNUMBER;
		fValue = Value;
		return fValue;
	}

	int operator=(int Value)
	{
		Type = LUA_TNUMBER;
		fValue = Value;
		return fValue;
	}

	unsigned int operator=(unsigned int Value)
	{
		Type = LUA_TNUMBER;
		fValue = Value;
		return fValue;
	}

	__int64 operator=(__int64 Value)
	{
		Type = LUA_TNUMBER;
		i64Value = Value;
		return i64Value;
	}

	unsigned __int64 operator=(unsigned __int64 Value)
	{
		Type = LUA_TNUMBER;
		i64Value = Value;
		return i64Value;
	}

	float operator=(float Value)
	{
		Type = LUA_TNUMBER;
		fValue = Value;
		return fValue;
	}

	double operator=(double Value)
	{
		Type = LUA_TNUMBER;
		fValue = Value;
		return fValue;
	}
};

struct LuaTableElement
{
	LuaValue		Key;
	LuaValue		Value;
};

class CLuaTable :
	public CEasyArray<LuaTableElement>
{
public:
	CLuaTable(void);
	~CLuaTable(void);

	void Release()
	{
		delete this;
	}

	bool PushToLuaState(lua_State * pLuaState);
	bool GetFromLuaState(lua_State * pLuaState, int Index);
	static bool PushValueToLuaState(LuaValue& Value, lua_State * pLuaState);
	static bool GetValueFromLuaState(LuaValue& Value, lua_State * pLuaState, int Index);
protected:
	bool PushElementToLuaState(UINT ElementIndex, lua_State * pLuaState);
	bool GetElementFromLuaState(lua_State * pLuaState, int Index);
	
};
