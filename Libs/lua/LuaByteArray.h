#pragma once

class CLuaByteArray :
	public CLuaBaseMetaClass
{
protected:
	BYTE *			m_pData;
	UINT			m_DataLen;
	bool			m_IsSelfData;

	DECLARE_META_CLASS(CLuaByteArray, 20002);
public:
	CLuaByteArray()
	{
		m_pData = NULL;
		m_DataLen = 0;
		m_IsSelfData = false;
	}
	CLuaByteArray(void * pData,UINT DataLen)
	{
		m_pData = NULL;
		m_DataLen = 0;
		m_IsSelfData = false;
		Attach(pData, DataLen);
	}	
	CLuaByteArray(UINT Size)
	{
		m_pData = NULL;
		m_DataLen = 0;
		m_IsSelfData = false;
		Create(Size);
	}
	CLuaByteArray(const CLuaByteArray& Value)
	{
		m_pData = NULL;
		m_DataLen = 0;
		m_IsSelfData = false;
		*this = Value;
	}
	virtual ~CLuaByteArray()
	{
		Destory();
	}

	CLuaByteArray& operator=(const CLuaByteArray& Value)
	{
		Destory();
		if (Value.m_IsSelfData)
		{
			CloneFrom(Value);
		}
		else
		{
			m_pData = Value.m_pData;
			m_DataLen = Value.m_DataLen;
			m_IsSelfData = false;
		}
		return *this;
	}

	void CloneFrom(const CLuaByteArray& Value)
	{
		Destory();
		m_DataLen = Value.m_DataLen;
		m_pData = MONITORED_NEW_ARRAY(_T("CLuaByteArray"), BYTE, m_DataLen);
		m_IsSelfData = true;
		memcpy(m_pData, Value.m_pData, m_DataLen);
	}

	void CloneFrom(void * pData, UINT DataLen)
	{
		Destory();
		m_DataLen = DataLen;
		m_pData = MONITORED_NEW_ARRAY(_T("CLuaByteArray"), BYTE, m_DataLen);
		m_IsSelfData = true;
		memcpy(m_pData, pData, m_DataLen);
	}
	

	void Create(UINT Size)
	{
		Destory();
		m_DataLen = Size;
		m_pData = MONITORED_NEW_ARRAY(_T("CLuaByteArray"), BYTE, m_DataLen);
		ZeroMemory(m_pData, m_DataLen);
		m_IsSelfData = true;
		ZeroMemory(m_pData, m_DataLen);
	}

	virtual LPCTSTR GetMetaClassName() const override
	{
		return CLuaByteArray::CLASS_NAME;
	}
	virtual int GetMetaClassID() const override
	{
		return CLuaByteArray::CLASS_ID;
	}
	virtual size_t GetMetaClassSize() const override
	{
		return sizeof(CLuaByteArray);
	}
	virtual void Dump(CEasyString& Data) const override;
	bool Attach(void* pData, UINT DataLen)
	{
		if (pData && DataLen)
		{
			Destory();
			m_pData = (BYTE*)pData;
			m_DataLen = DataLen;
			m_IsSelfData = false;
			return true;
		}
		return false;
	}
	UINT GetDataLen() const
	{
		return m_DataLen;
	}

	const BYTE * GetData() const
	{
		return m_pData;
	}

	BYTE * GetData()
	{
		return m_pData;
	}

	void Destory()
	{
		if (m_IsSelfData)
		{
			SAFE_DELETE_ARRAY(m_pData);
		}
		m_pData = NULL;
		m_DataLen = 0;
		m_IsSelfData = false;
	}
	bool SetValue(UINT Index, BYTE Value);
	static void ResgisterStaticFunctions(CBaseLuaVM* pLuaVM, LPCTSTR LibName);
	static void ResgisterStaticFunctions(CBaseLuaVM* pLuaVM)
	{
		ResgisterStaticFunctions(pLuaVM, CLASS_NAME);
	}
protected:
	virtual void RegisterMemberFunctions(lua_State * pLuaState) const;
protected:
	virtual void OnGarbageCollect();
	static LUA_EMPTY_VALUE LuaNew(CLuaThread* pLuaThread, UINT Size);
	LUA_EMPTY_VALUE LuaClone();
	LUA_EMPTY_VALUE LuaGetValue(UINT Index);
	LUA_EMPTY_VALUE LuaToString();
	LUA_EMPTY_VALUE LuaSubArray(UINT StartIndex);
};


DECLARE_META_CLASS_MATCHER(CLuaByteArray)

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE CLuaByteArray*
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(pThreadInfo, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"