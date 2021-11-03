#pragma once
class CLuaSmartValue :
	public CLuaBaseStaticMetaClass
{
protected:
	CSmartValue		m_Data;

	DECLARE_STATIC_META_CLASS(CLuaSmartValue)
public:
	static const int CLASS_ID = 10003;
	CLuaSmartValue()
	{

	}
	CLuaSmartValue(void * pData, UINT DataLen)
	{		
		Attach(pData, DataLen);
	}
	CLuaSmartValue(int Type, UINT Size)
	{
		m_Data.Create(Type, Size);
	}
	CLuaSmartValue(const CSmartValue& Value)
	{
		m_Data = Value;
	}
	CLuaSmartValue(const CLuaSmartValue& Value)
		:m_Data(Value.m_Data)
	{
	}
	virtual ~CLuaSmartValue()
	{
		Destory();
	}

	CLuaSmartValue& operator=(const CLuaSmartValue& Value)
	{
		m_Data = Value.m_Data;
		return *this;
	}

	CLuaSmartValue& operator=(const CSmartValue& Value)
	{
		m_Data = m_Data;
		return *this;
	}

	void CloneFrom(const CLuaSmartValue& Value)
	{
		m_Data.CloneFrom(Value.m_Data);
	}

	void CloneFrom(const CSmartValue& Value)
	{
		m_Data.CloneFrom(Value);
	}

	void CloneFromWidthDataBuffer(const CLuaSmartValue& Value, BYTE * pDataBuffer)
	{
		memcpy(pDataBuffer, Value.m_Data.GetData(), Value.m_Data.GetDataLen());
		m_Data.Attach(pDataBuffer, Value.m_Data.GetDataLen());
	}

	void Create(int Type, UINT Size)
	{
		m_Data.Create(Type, Size);
	}

	virtual const char * GetMetaClassName() const override
	{
		return _T("CLuaSmartValue");
	}
	virtual int GetMetaClassID() const override
	{
		return CLuaSmartValue::CLASS_ID;
	}
	virtual size_t GetMetaClassSize() const override
	{
		return sizeof(CLuaSmartValue);
	}
	virtual bool Attach(void * pData, UINT DataLen) override
	{
		return m_Data.Attach(pData, DataLen);
	}
	virtual UINT GetDataLen() const override
	{
		return m_Data.GetDataLen();
	}

	virtual const BYTE * GetData() const override
	{
		return (BYTE *)m_Data.GetData();
	}

	virtual BYTE * GetData() override
	{
		return (BYTE *)m_Data.GetData();
	}

	CSmartValue& GetValue()
	{
		return m_Data;
	}

	void Destory()
	{
		m_Data.Destory();
	}
	static void PushValueToLua(lua_State * L, const CSmartValue& Value);
protected:
	virtual void RegisterMemberFunctions(lua_State * pLuaState) const;
protected:
	LUA_EMPTY_VALUE LuaGetData(CLuaThread * pThreadInfo);
	LUA_EMPTY_VALUE LuaGetDataClone(CLuaThread * pThreadInfo);
	UINT LuaGetDataLen(CLuaThread * pThreadInfo);	
	UINT LuaGetLength(CLuaThread * pThreadInfo);
	bool LuaAttach(CLuaThread * pThreadInfo, LUA_EMPTY_VALUE);
	int LuaGetType(CLuaThread * pThreadInfo);
	LUA_EMPTY_VALUE LuaGetValue(CLuaThread * pThreadInfo);
	bool LuaSetValue(CLuaThread * pThreadInfo, LUA_EMPTY_VALUE);
	LUA_EMPTY_VALUE LuaGetValueAsByteArray(CLuaThread * pThreadInfo);	
	bool LuaGetValueAsBoolean(CLuaThread * pThreadInfo);
	char LuaGetValueAsSByte(CLuaThread * pThreadInfo);
	BYTE LuaGetValueAsByte(CLuaThread * pThreadInfo);	
	short LuaGetValueAsShort(CLuaThread * pThreadInfo);
	WORD LuaGetValueAsUShort(CLuaThread * pThreadInfo);
	int LuaGetValueAsInt(CLuaThread * pThreadInfo);
	UINT LuaGetValueAsUInt(CLuaThread * pThreadInfo);
	INT64 LuaGetValueAsInt64(CLuaThread * pThreadInfo);
	UINT64 LuaGetValueAsUInt64(CLuaThread * pThreadInfo);
	float LuaGetValueAsFloat(CLuaThread * pThreadInfo);
	double LuaGetValueAsDouble(CLuaThread * pThreadInfo);
	LPCTSTR LuaGetValueAsString(CLuaThread * pThreadInfo);
	LUA_EMPTY_VALUE LuaGetValueAsSmartStruct(CLuaThread * pThreadInfo);
};

namespace LuaWrap
{
	inline void Push(lua_State* L, const CLuaSmartValue& value)
	{
		UINT ClassSize = (UINT)value.GetMetaClassSize();
		UINT DataSize = value.GetDataLen();
		if (value.GetData() && ClassSize && DataSize)
		{
			BYTE * pBuff = (BYTE *)lua_newuserdata(L, ClassSize + DataSize);
#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#undef new
#endif
			::new(pBuff) CLuaSmartValue();
#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#define new NEWNEW
#endif
			((CLuaSmartValue *)pBuff)->CloneFromWidthDataBuffer(value, pBuff + ClassSize);
			value.SetMetaClass(L);
		}
		else
		{
			lua_pushnil(L);
		}
	}
	inline void PushRef(lua_State* L, const CLuaSmartValue& value)
	{
		UINT ClassSize = (UINT)value.GetMetaClassSize();
		UINT DataSize = value.GetDataLen();
		if (value.GetData() && ClassSize && DataSize)
		{
			BYTE * pBuff = (BYTE *)lua_newuserdata(L, ClassSize + DataSize);
#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#undef new
#endif
			::new(pBuff) CLuaSmartValue();
#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#define new NEWNEW
#endif
			((CLuaSmartValue *)pBuff)->Attach((void *)value.GetData(), value.GetDataLen());
			value.SetMetaClass(L);
		}
		else
		{
			lua_pushnil(L);
		}
	}
	inline bool	Match(TypeWrapper<CLuaSmartValue>, lua_State* L, int idx)
	{
		return GetLuaObjectType(L, idx) == CLuaSmartValue::CLASS_ID;
	}
	inline CLuaSmartValue Get(TypeWrapper<CLuaSmartValue>, lua_State* L, int idx)
	{
		CLuaSmartValue Object;

		UINT Len = (UINT)lua_rawlen(L, idx);
		BYTE * pBuff = (BYTE *)lua_touserdata(L, idx);
		if (pBuff&&Len)
		{
			UINT ClassSize = (UINT)Object.GetMetaClassSize();
			Object.Attach(pBuff + ClassSize, Len - ClassSize);
		}
		return Object;
	}
	inline bool	Match(TypeWrapper<CLuaSmartValue&>, lua_State* L, int idx)
	{
		return GetLuaObjectType(L, idx) == CLuaSmartValue::CLASS_ID;
	}
	inline CLuaSmartValue& Get(TypeWrapper<CLuaSmartValue&>, lua_State* L, int idx)
	{
		BYTE * pBuff = (BYTE *)lua_touserdata(L, idx);
		return *((CLuaSmartValue *)pBuff);
	}
}

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE CLuaSmartValue
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(L, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"