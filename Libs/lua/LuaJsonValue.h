#pragma once

class CLuaJsonValue :
	public CLuaBaseStaticMetaClass
{
protected:
	rapidjson::Value	m_Data;

	DECLARE_STATIC_META_CLASS(CLuaJsonValue)
public:
	static const int CLASS_ID = 10005;
	CLuaJsonValue()
	{

	}
	CLuaJsonValue(rapidjson::Value& Value)
	{
		m_Data = Value;
	}
	CLuaJsonValue(CLuaJsonValue& Value)
	{
		m_Data = Value.m_Data;
	}
	~CLuaJsonValue()
	{
		Destory();
	}

	CLuaJsonValue& operator=(rapidjson::Value& Value)
	{
		m_Data = Value;
		return *this;
	}

	CLuaJsonValue& operator=(CLuaJsonValue& Value)
	{
		m_Data = Value.m_Data;
		return *this;
	}

	virtual const char * GetMetaClassName() const override
	{
		return _T("CLuaJsonValue");
	}
	virtual int GetMetaClassID() const override
	{
		return CLuaJsonValue::CLASS_ID;
	}
	virtual size_t GetMetaClassSize() const override
	{
		return sizeof(CLuaJsonValue);
	}
	virtual bool Attach(void * pData, UINT DataLen) override
	{
		return false;
	}
	virtual UINT GetDataLen() const override
	{
		return 0;
	}

	virtual const BYTE * GetData() const override
	{
		return NULL;
	}
	virtual BYTE * GetData() override
	{
		return NULL;
	}

	rapidjson::Value& GetValue()
	{
		return m_Data;
	}

	void Destory()
	{
		
	}
protected:
	virtual void RegisterMemberFunctions(lua_State * pLuaState) const;
protected:
	LPCSTR LuaGetType(CLuaThread * pThreadInfo);
	bool LuaIsNull(CLuaThread * pThreadInfo);
	bool LuaIsBool(CLuaThread * pThreadInfo);
	bool LuaIsObject(CLuaThread * pThreadInfo);
	bool LuaIsArray(CLuaThread * pThreadInfo);
	bool LuaIsNumber(CLuaThread * pThreadInfo);
	bool LuaIsInt(CLuaThread * pThreadInfo);
	bool LuaIsUint(CLuaThread * pThreadInfo);
	bool LuaIsInt64(CLuaThread * pThreadInfo);
	bool LuaIsUint64(CLuaThread * pThreadInfo);
	bool LuaIsDouble(CLuaThread * pThreadInfo);
	bool LuaIsString(CLuaThread * pThreadInfo);

	bool LuaGetBool(CLuaThread * pThreadInfo);
	int LuaGetInt(CLuaThread * pThreadInfo);
	UINT LuaGetUInt(CLuaThread * pThreadInfo);
	INT64 LuaGetInt64(CLuaThread * pThreadInfo);
	UINT64 LuaGetUInt64(CLuaThread * pThreadInfo);
	double LuaGetDouble(CLuaThread * pThreadInfo);
	LPCTSTR LuaGetString(CLuaThread * pThreadInfo);
	UINT LuaGetStringLength(CLuaThread * pThreadInfo);

	bool LuaHasMember(CLuaThread * pThreadInfo, LPCTSTR MemberName);
	UINT LuaGetSize(CLuaThread * pThreadInfo);
	UINT LuaGetMemberCount(CLuaThread * pThreadInfo);
	LUA_EMPTY_VALUE LuaGetMember(CLuaThread * pThreadInfo);
};

namespace LuaWrap
{
	inline void Push(lua_State* L, CLuaJsonValue& value)
	{
		UINT ClassSize = value.GetMetaClassSize();
		if (ClassSize)
		{
			BYTE * pBuff = (BYTE *)lua_newuserdata(L, ClassSize);
#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#undef new
#endif
			::new(pBuff) CLuaJsonValue(value);
#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#define new NEWNEW
#endif
			value.SetMetaClass(L);
		}
		else
		{
			lua_pushnil(L);
		}
	}
	
	inline bool	Match(TypeWrapper<CLuaJsonValue>, lua_State* L, int idx)
	{
		return GetLuaObjectType(L, idx) == CLuaJsonValue::CLASS_ID;
	}
	inline CLuaJsonValue Get(TypeWrapper<CLuaJsonValue>, lua_State* L, int idx)
	{
		CLuaJsonValue Object;

		UINT Len = (UINT)lua_rawlen(L, idx);
		BYTE * pBuff = (BYTE *)lua_touserdata(L, idx);
		if (pBuff&&Len)
		{
			Object = *(dynamic_cast<CLuaJsonValue *>((CLuaJsonValue *)lua_touserdata(L, idx)));
		}
		return Object;
	}
	inline bool	Match(TypeWrapper<CLuaJsonValue&>, lua_State* L, int idx)
	{
		return GetLuaObjectType(L, idx) == CLuaJsonValue::CLASS_ID;
	}
	inline CLuaJsonValue& Get(TypeWrapper<CLuaJsonValue&>, lua_State* L, int idx)
	{
		BYTE * pBuff = (BYTE *)lua_touserdata(L, idx);
		return *((CLuaJsonValue *)pBuff);
	}
}

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE CLuaJsonValue
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(L, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"