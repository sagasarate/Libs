#pragma once

class CLuaByteArray :
	public CLuaBaseStaticMetaClass
{
protected:
	BYTE *			m_pData;
	UINT			m_DataLen;
	bool			m_IsSelfData;

	DECLARE_STATIC_META_CLASS(CLuaByteArray)
public:
	static const int CLASS_ID = 10002;
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

	void CloneFromWidthDataBuffer(const CLuaByteArray& Value, BYTE * pDataBuffer)
	{
		Destory();
		m_DataLen = Value.m_DataLen;
		m_pData = pDataBuffer;
		m_IsSelfData = false;
		memcpy(m_pData, Value.m_pData, m_DataLen);
	}

	void Create(UINT Size)
	{
		Destory();
		m_DataLen = Size;
		m_pData = MONITORED_NEW_ARRAY(_T("CLuaByteArray"), BYTE, m_DataLen);
		m_IsSelfData = true;
		ZeroMemory(m_pData, m_DataLen);
	}

	virtual const char * GetMetaClassName() const override
	{
		return _T("CLuaByteArray");
	}
	virtual int GetMetaClassID() const override
	{
		return CLuaByteArray::CLASS_ID;
	}
	virtual size_t GetMetaClassSize() const override
	{
		return sizeof(CLuaByteArray);
	}
	virtual bool Attach(void * pData, UINT DataLen) override
	{
		Destory();
		m_pData = (BYTE *)pData;
		m_DataLen = DataLen;
		m_IsSelfData = false;
		return true;
	}
	virtual UINT GetDataLen() const override
	{
		return m_DataLen;
	}

	virtual const BYTE * GetData() const override
	{
		return m_pData;
	}

	virtual BYTE * GetData() override
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

	
protected:
	virtual void RegisterMemberFunctions(lua_State * pLuaState) const;
protected:
	UINT LuaGetSize(CLuaThread * pThreadInfo);
	bool LuaSetValue(CLuaThread * pThreadInfo, UINT Index, BYTE Value);
	LUA_EMPTY_VALUE LuaGetValue(CLuaThread * pThreadInfo, UINT Index);
	LUA_EMPTY_VALUE LuaToString(CLuaThread * pThreadInfo);
};

namespace LuaWrap
{
	inline void Push(lua_State* L, const CLuaByteArray& value)
	{
		UINT ClassSize = (UINT)value.GetMetaClassSize();
		UINT DataSize = value.GetDataLen();
		if (value.GetData() && ClassSize && DataSize)
		{
			BYTE * pBuff = (BYTE *)lua_newuserdata(L, ClassSize + DataSize);
#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#undef new
#endif
			::new(pBuff) CLuaByteArray();
#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#define new NEWNEW
#endif
			((CLuaByteArray *)pBuff)->CloneFromWidthDataBuffer(value, pBuff + ClassSize);
			value.SetMetaClass(L);
		}
		else
		{
			lua_pushnil(L);
		}
	}
	inline void PushRef(lua_State* L, const CLuaByteArray& value)
	{
		UINT ClassSize = (UINT)value.GetMetaClassSize();
		UINT DataSize = value.GetDataLen();
		if (value.GetData() && ClassSize && DataSize)
		{
			BYTE * pBuff = (BYTE *)lua_newuserdata(L, ClassSize);
#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#undef new
#endif
			::new(pBuff) CLuaByteArray();
#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#define new NEWNEW
#endif
			((CLuaByteArray *)pBuff)->Attach((void *)value.GetData(), value.GetDataLen());
			value.SetMetaClass(L);
		}
		else
		{
			lua_pushnil(L);
		}
	}
	inline bool	Match(TypeWrapper<CLuaByteArray>, lua_State* L, int idx)
	{
		return GetLuaObjectType(L, idx) == CLuaByteArray::CLASS_ID;
	}
	inline CLuaByteArray Get(TypeWrapper<CLuaByteArray>, lua_State* L, int idx)
	{
		CLuaByteArray Object;

		UINT Len = (UINT)lua_rawlen(L, idx);
		BYTE * pBuff = (BYTE *)lua_touserdata(L, idx);
		if (pBuff&&Len)
		{
			UINT ClassSize = (UINT)Object.GetMetaClassSize();
			Object.Attach(pBuff + ClassSize, Len - ClassSize);
		}
		return Object;
	}
	inline bool	Match(TypeWrapper<CLuaByteArray&>, lua_State* L, int idx)
	{
		return GetLuaObjectType(L, idx) == CLuaByteArray::CLASS_ID;
	}
	inline CLuaByteArray& Get(TypeWrapper<CLuaByteArray&>, lua_State* L, int idx)
	{
		BYTE * pBuff = (BYTE *)lua_touserdata(L, idx);		
		return *((CLuaByteArray *)pBuff);
	}
}

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE CLuaByteArray
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(L, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"