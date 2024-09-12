#pragma once
class CLuaSmartValue :
	public CLuaBaseMetaClass
{
protected:
	CSmartValue		m_Data;

	DECLARE_META_CLASS(CLuaSmartValue, 20003);
public:
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
	

	void Create(int Type, UINT Size)
	{
		m_Data.Create(Type, Size);
	}

	virtual LPCTSTR GetMetaClassName() const override
	{
		return CLuaSmartValue::CLASS_NAME;
	}
	virtual int GetMetaClassID() const override
	{
		return CLuaSmartValue::CLASS_ID;
	}
	virtual size_t GetMetaClassSize() const override
	{
		return sizeof(CLuaSmartValue);
	}
	virtual void Dump(CEasyString& Data) const override;
	bool Attach(void * pData, UINT DataLen)
	{
		return m_Data.Attach(pData, DataLen);
	}
	bool Attach(const CSmartValue& Data)
	{
		m_Data = Data;
		return true;
	}
	bool Attach(const CSmartStruct& Data)
	{
		m_Data = Data;
		return true;
	}	
	bool Attach(const CSmartArray& Data)
	{
		m_Data = Data;
		return true;
	}
	CSmartValue& GetValue()
	{
		return m_Data;
	}
	const CSmartValue& GetValue() const
	{
		return m_Data;
	}
	void Destory()
	{
		m_Data.Destory();
	}
	static void ResgisterStaticFunctions(CBaseLuaVM* pLuaVM, LPCTSTR LibName);
	static void ResgisterStaticFunctions(CBaseLuaVM* pLuaVM)
	{
		ResgisterStaticFunctions(pLuaVM, CLASS_NAME);
	}
protected:
	virtual void RegisterMemberFunctions(lua_State * pLuaState) const;
protected:
	virtual void OnGarbageCollect();
	static LUA_EMPTY_VALUE LuaNew(CLuaThread* pLuaThread, LUA_EMPTY_VALUE);
	LUA_EMPTY_VALUE LuaClone();
	LUA_EMPTY_VALUE LuaGetData();
	UINT LuaGetDataLen();	
	UINT LuaGetLength();
	UINT LuaGetBufferLen();
	bool LuaAttach(CLuaBaseMetaClass* pObject);
	int LuaGetType();
	LUA_EMPTY_VALUE LuaGetValue();
	bool LuaSetValue(LUA_EMPTY_VALUE);
	LUA_EMPTY_VALUE LuaGetValueAsByteArray();	
	bool LuaGetValueAsBoolean();
	char LuaGetValueAsSByte();
	BYTE LuaGetValueAsByte();	
	short LuaGetValueAsShort();
	WORD LuaGetValueAsUShort();
	int LuaGetValueAsInt();
	UINT LuaGetValueAsUInt();
	INT64 LuaGetValueAsInt64();
	UINT64 LuaGetValueAsUInt64();
	float LuaGetValueAsFloat();
	double LuaGetValueAsDouble();
	LPCTSTR LuaGetValueAsString();
	LUA_EMPTY_VALUE LuaGetValueAsStruct();
	LUA_EMPTY_VALUE LuaGetValueAsArray();
};

DECLARE_META_CLASS_MATCHER(CLuaSmartValue)

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE CLuaSmartValue*
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(pThreadInfo, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"