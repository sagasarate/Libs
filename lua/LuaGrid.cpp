#include "stdafx.h"

#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#undef new
#endif

IMPLEMENT_META_CLASS(CLuaGrid, _T("Grid"))

#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#define new NEWNEW
#endif

#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#define new NEWNEW
#endif

void CLuaGrid::Dump(CEasyString& Data) const
{
	UINT Row = (UINT)m_Data.GetCount();
	UINT Col = 0;
	if (Row)
		Col = (UINT)m_Data[0].GetCount();
	Data.Format(_T("CLuaGrid(%u,%u)(%s)"), Col, Row, IsInLuaHeap() ? _T("InLuaHeap") : _T("InCHeap"));
}
void CLuaGrid::ResgisterStaticFunctions(CBaseLuaVM* pLuaVM, LPCTSTR LibName)
{
	pLuaVM->RegisterStaticFunction(LibName, _T("New"), &CLuaGrid::LuaNew);
}
void CLuaGrid::RegisterMemberFunctions(lua_State * pLuaState) const
{
	CLuaBaseMetaClass::RegisterMemberFunctions(pLuaState);

	RegisterMetaFunction<CLuaGrid>(pLuaState, _T("GetMaxCol"), &CLuaGrid::GetMaxCol);
	RegisterMetaFunction<CLuaGrid>(pLuaState, _T("GetMaxRow"), &CLuaGrid::GetMaxRow);
	RegisterMetaFunction<CLuaGrid>(pLuaState, _T("SetValue"), &CLuaGrid::LuaSetValue);
	RegisterMetaFunction<CLuaGrid>(pLuaState, _T("GetValue"), &CLuaGrid::LuaGetValue);
}

void CLuaGrid::OnGarbageCollect()
{
	if (m_IsInLuaHeap)
		this->~CLuaGrid();
}
LUA_EMPTY_VALUE CLuaGrid::LuaNew(CLuaThread* pLuaThread, UINT Col, UINT Row)
{
	CLuaGrid* pObject = CLuaGrid::New(pLuaThread, NULL);
	if (pObject)
	{
		pObject->Create(Col, Row);
	}	
	return LUA_EMPTY_VALUE();
}
LUA_EMPTY_VALUE CLuaGrid::Clone()
{
	CLuaGrid* pObject = CLuaGrid::New(m_pCurThread, NULL);
	if (pObject)
	{
		pObject->CloneFrom(*pObject);
	}	
	return LUA_EMPTY_VALUE();
}

void CLuaGrid::LuaSetValue(UINT Col, UINT Row, CVariedValue Value)
{
	GetValue(Col, Row) = Value;
}
CVariedValue& CLuaGrid::LuaGetValue(UINT Col, UINT Row)
{
	return GetValue(Col, Row);
}