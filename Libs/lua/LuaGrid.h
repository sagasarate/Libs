#pragma once

class CLuaGrid :
	public CLuaBaseStaticMetaClass
{
protected:
	BYTE *			m_pData;
	UINT			m_DataLen;
	bool			m_IsSelfData;

	DECLARE_STATIC_META_CLASS(CLuaGrid)
public:
	static const int CLASS_ID = 10001;
	CLuaGrid()
	{
		m_pData = NULL;
		m_DataLen = 0;
		m_IsSelfData = false;
	}
	CLuaGrid(void * pData,UINT DataLen)
	{
		m_pData = NULL;
		m_DataLen = 0;
		m_IsSelfData = false;
		Attach(pData, DataLen);
	}
	CLuaGrid(void * pData, UINT DataLen, UINT Col, UINT Row)
	{
		m_pData = NULL;
		m_DataLen = 0;
		m_IsSelfData = false;
		Attach(pData, DataLen, Col, Row);
	}
	CLuaGrid(UINT Col,UINT Row)
	{
		m_pData = NULL;
		m_DataLen = 0;
		m_IsSelfData = false;
		Create(Col, Row);
	}
	CLuaGrid(const CLuaGrid& Value)
	{
		m_pData = NULL;
		m_DataLen = 0;
		m_IsSelfData = false;
		*this = Value;
	}
	virtual ~CLuaGrid()
	{
		Destory();
	}

	CLuaGrid& operator=(const CLuaGrid& Value)
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

	void CloneFrom(const CLuaGrid& Value)
	{
		Destory();
		m_DataLen = Value.m_DataLen;
		m_pData = MONITORED_NEW_ARRAY(_T("CLuaGrid"), BYTE, m_DataLen);
		m_IsSelfData = true;
		memcpy(m_pData, Value.m_pData, m_DataLen);
	}

	void CloneFromWidthDataBuffer(const CLuaGrid& Value, BYTE * pDataBuffer)
	{
		Destory();
		m_DataLen = Value.m_DataLen;
		m_pData = pDataBuffer;
		m_IsSelfData = false;
		memcpy(m_pData, Value.m_pData, m_DataLen);
	}

	void Create(UINT Col, UINT Row)
	{
		Destory();
		m_DataLen = Col*Row*sizeof(LuaValue) + sizeof(WORD) * 2;
		m_pData = MONITORED_NEW_ARRAY(_T("CLuaGrid"), BYTE, m_DataLen);
		m_IsSelfData = true;
		ZeroMemory(m_pData, m_DataLen);
		*((WORD *)m_pData) = Col;
		*(((WORD *)m_pData) + 1) = Row;
	}
	virtual const char * GetMetaClassName() const override
	{
		return _T("CLuaGrid");
	}
	virtual int GetMetaClassID() const override
	{
		return CLuaGrid::CLASS_ID;
	}
	virtual size_t GetMetaClassSize() const override
	{
		return sizeof(CLuaGrid);
	}	
	virtual bool Attach(void * pData, UINT DataLen) override
	{
		Destory();
		m_pData = (BYTE *)pData;
		m_DataLen = DataLen;
		m_IsSelfData = false;
		return CheckData();
	}
	bool Attach(void * pData, UINT DataLen, UINT Col, UINT Row)
	{
		Destory();
		m_pData = (BYTE *)pData;
		m_DataLen = DataLen;
		m_IsSelfData = false;
		if (m_DataLen >= Col*Row*sizeof(LuaValue) + sizeof(WORD) * 2)
		{
			ZeroMemory(m_pData, m_DataLen);
			*((WORD *)m_pData) = Col;
			*(((WORD *)m_pData) + 1) = Row;
			return true;
		}
		else
		{
			Destory();
			return false;
		}
	}

	virtual void Destory()
	{
		if (m_IsSelfData)
		{
			SAFE_DELETE_ARRAY(m_pData);
		}
		m_pData = NULL;
		m_DataLen = 0;
		m_IsSelfData = false;
	}

	bool CheckData()
	{
		if (m_DataLen > sizeof(WORD) * 2)
		{
			if (m_DataLen >= GetMaxCol()*GetMaxRow()*sizeof(LuaValue) + sizeof(WORD) * 2)
			{
				return true;
			}
		}
		Destory();
		return false;
	}

	WORD GetMaxCol() const
	{
		if (m_pData)
			return *((WORD *)m_pData);
		else
			return 0;
	}

	WORD GetMaxRow() const
	{
		if (m_pData)
			return *(((WORD *)m_pData) + 1);
		else
			return 0;
	}

	LuaValue& GetValue(UINT Col, UINT Row)
	{
		static LuaValue EmptyValue;
		if (m_pData)
		{
			if (Col < GetMaxCol() && Row < GetMaxRow())
			{
				UINT ValueOffset = (GetMaxCol()*Row + Col)*sizeof(LuaValue) + sizeof(WORD) * 2;
				return *((LuaValue *)(m_pData + ValueOffset));
			}
		}
		return EmptyValue;
	}

	virtual const BYTE * GetData() const override
	{
		return m_pData;
	}
	virtual BYTE * GetData() override
	{
		return m_pData;
	}
	virtual UINT GetDataLen() const override
	{
		return m_DataLen;
	}
protected:
	virtual void RegisterMemberFunctions(lua_State * pLuaState) const;
protected:
	WORD LuaGetMaxCol(CLuaThread * pThreadInfo);
	WORD LuaGetMaxRow(CLuaThread * pThreadInfo);
	void LuaSetValue(CLuaThread * pThreadInfo, UINT Col, UINT Row, LuaValue Value);
	LuaValue LuaGetValue(CLuaThread * pThreadInfo, UINT Col, UINT Row);
};

namespace LuaWrap
{
	inline void Push(lua_State* L, const CLuaGrid& value)
	{
		UINT ClassSize = (UINT)value.GetMetaClassSize();
		UINT DataSize = value.GetDataLen();
		if (value.GetData() && ClassSize && DataSize)
		{
			BYTE * pBuff = (BYTE *)lua_newuserdata(L, ClassSize + DataSize);
#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#undef new
#endif
			::new(pBuff) CLuaGrid();
#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#define new NEWNEW
#endif
			((CLuaGrid *)pBuff)->CloneFromWidthDataBuffer(value, pBuff + ClassSize);
			value.SetMetaClass(L);
		}
		else
		{
			lua_pushnil(L);
		}
	}
	inline bool	Match(TypeWrapper<CLuaGrid >, lua_State* L, int idx)
	{
		return GetLuaObjectType(L, idx) == CLuaGrid::CLASS_ID;
	}
	inline CLuaGrid Get(TypeWrapper<CLuaGrid>, lua_State* L, int idx)
	{
		CLuaGrid Object;

		UINT Len = (UINT)lua_rawlen(L, idx);
		BYTE * pBuff = (BYTE *)lua_touserdata(L, idx);
		if (pBuff&&Len)
		{
			UINT ClassSize = (UINT)Object.GetMetaClassSize();
			Object.Attach(pBuff + ClassSize, Len - ClassSize);
		}
		return Object;
	}
	inline bool	Match(TypeWrapper<CLuaGrid&>, lua_State* L, int idx)
	{
		return GetLuaObjectType(L, idx) == CLuaGrid::CLASS_ID;
	}
	inline CLuaGrid Get(TypeWrapper<CLuaGrid&>, lua_State* L, int idx)
	{
		BYTE * pBuff = (BYTE *)lua_touserdata(L, idx);
		return *((CLuaGrid *)pBuff);
	}
}

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE CLuaGrid
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(L, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"
