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
		m_ClassID = CLASS_ID;
		m_ClassName = "CLuaGrid";
		m_pData = NULL;
		m_DataLen = 0;
		m_IsSelfData = false;
	}
	CLuaGrid(void * pData,UINT DataLen)
	{
		m_ClassID = CLASS_ID;
		m_ClassName = "CLuaGrid";
		m_pData = NULL;
		m_DataLen = 0;
		m_IsSelfData = false;
		Attach(pData, DataLen);
	}
	CLuaGrid(void * pData, UINT DataLen, UINT Col, UINT Row)
	{
		m_ClassID = CLASS_ID;
		m_ClassName = "CLuaGrid";
		m_pData = NULL;
		m_DataLen = 0;
		m_IsSelfData = false;
		Attach(pData, DataLen, Col, Row);
	}
	CLuaGrid(UINT Col,UINT Row)
	{
		m_ClassID = CLASS_ID;
		m_ClassName = "CLuaGrid";
		m_pData = NULL;
		m_DataLen = 0;
		m_IsSelfData = false;
		Create(Col, Row);
	}
	CLuaGrid(const CLuaGrid& Value)
	{
		m_ClassID = CLASS_ID;
		m_ClassName = "CLuaGrid";
		m_pData = NULL;
		m_DataLen = 0;
		m_IsSelfData = false;
		*this = Value;
	}
	~CLuaGrid()
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

	bool Attach(void * pData, UINT DataLen)
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

	const BYTE * GetData() const
	{
		return m_pData;
	}
	UINT GetDataLen() const
	{
		return m_DataLen;
	}
	void SetMetaClass(lua_State * pLuaState) const;
protected:
	void RegisterMemberFunctions(lua_State * pLuaState) const;
protected:

	
	WORD LuaGetMaxCol(CLuaThread * pThreadInfo);
	WORD LuaGetMaxRow(CLuaThread * pThreadInfo);
	void LuaSetValue(CLuaThread * pThreadInfo, UINT Col, UINT Row, LuaValue Value);
	LuaValue LuaGetValue(CLuaThread * pThreadInfo, UINT Col, UINT Row);
};

namespace LuaWrap
{
	inline void Push(lua_State* L, const CLuaGrid value)
	{
		if (value.GetData() && value.GetDataLen())
		{
			void * pBuff = lua_newuserdata(L, value.GetDataLen());
			memcpy(pBuff, value.GetData(), value.GetDataLen());
			value.SetMetaClass(L);
		}
		else
		{
			lua_pushnil(L);
		}
	}
	inline bool	Match(TypeWrapper<CLuaGrid >, lua_State* L, int idx)
	{
		return lua_type(L, idx) == LUA_TUSERDATA;
	}
	inline CLuaGrid Get(TypeWrapper<CLuaGrid>, lua_State* L, int idx)
	{
		CLuaGrid LuaGrid;

		UINT Len = (UINT)lua_rawlen(L, idx);
		void * pBuff = lua_touserdata(L, idx);
		if (pBuff&&Len)
			LuaGrid.Attach(pBuff, Len);

		return LuaGrid;
	}
}

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_FETCH_OPERATION
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE CLuaGrid
#define LUA_WRAP_RETURN_FETCH_OPERATION CLuaGrid Ret=
#define LUA_WRAP_RETURN_PUSH_OPERATION Push(L, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"
