#pragma once

class CLuaGrid :
	public CLuaBaseMetaClass
{
protected:
	CEasyArray<CEasyArray<CVariedValue> >	m_Data;

	DECLARE_META_CLASS(CLuaGrid, 20001);
public:
	CLuaGrid()
	{
	}	
	CLuaGrid(UINT Col,UINT Row)
	{
		Create(Col, Row);
	}
	CLuaGrid(const CLuaGrid& Value)
	{
		*this = Value;
	}
	virtual ~CLuaGrid()
	{
		Destory();
	}

	CLuaGrid& operator=(const CLuaGrid& Value)
	{
		m_Data = Value.m_Data;
		return *this;
	}

	void CloneFrom(const CLuaGrid& Value)
	{
		m_Data = Value.m_Data;
	}

	void Create(UINT Col, UINT Row)
	{
		Destory();
		m_Data.Resize(Row);
		for (CEasyArray<CVariedValue>& Line : m_Data)
		{
			Line.Resize(Col);
		}
	}
	virtual LPCTSTR GetMetaClassName() const override
	{
		return CLuaGrid::CLASS_NAME;
	}
	virtual int GetMetaClassID() const override
	{
		return CLuaGrid::CLASS_ID;
	}
	virtual size_t GetMetaClassSize() const override
	{
		return sizeof(CLuaGrid);
	}	
	virtual void Dump(CEasyString& Data) const override;	

	virtual void Destory()
	{
		m_Data.Clear();
	}

	UINT GetMaxCol() const
	{
		if (m_Data.GetCount())
			return m_Data[0].GetCount();
		else
			return 0;
	}

	UINT GetMaxRow() const
	{
		return m_Data.GetCount();
	}

	CVariedValue& GetValue(UINT Col, UINT Row)
	{
		if (Row < m_Data.GetCount())
		{
			CEasyArray<CVariedValue>& Line = m_Data[Row];
			if (Col < Line.GetCount())
			{
				return Line[Col];
			}
		}
		return CVariedValue::NIL;
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
	static LUA_EMPTY_VALUE LuaNew(CLuaThread* pLuaThread, UINT Col, UINT Row);
	LUA_EMPTY_VALUE Clone();
	void LuaSetValue(UINT Col, UINT Row, CVariedValue Value);
	CVariedValue& LuaGetValue(UINT Col, UINT Row);
};

DECLARE_META_CLASS_MATCHER(CLuaGrid)

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE CLuaGrid*
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(pThreadInfo, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"
