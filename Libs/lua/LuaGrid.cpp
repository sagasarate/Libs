#include "stdafx.h"

IMPLEMENT_STATIC_META_CLASS(CLuaGrid)


void CLuaGrid::RegisterMemberFunctions(lua_State * pLuaState) const
{
	CLuaBaseStaticMetaClass::RegisterMemberFunctions(pLuaState);

	RegisterMetaFunction<CLuaGrid>(pLuaState, "GetMaxCol", &CLuaGrid::LuaGetMaxCol);
	RegisterMetaFunction<CLuaGrid>(pLuaState, "GetMaxRow", &CLuaGrid::LuaGetMaxRow);
	RegisterMetaFunction<CLuaGrid>(pLuaState, "SetValue", &CLuaGrid::LuaSetValue);
	RegisterMetaFunction<CLuaGrid>(pLuaState, "GetValue", &CLuaGrid::LuaGetValue);
}


WORD CLuaGrid::LuaGetMaxCol(CLuaThread * pThreadInfo)
{
	return GetMaxCol();
}
WORD CLuaGrid::LuaGetMaxRow(CLuaThread * pThreadInfo)
{
	return GetMaxRow();
}
void CLuaGrid::LuaSetValue(CLuaThread * pThreadInfo, UINT Col, UINT Row, LuaValue Value)
{
	GetValue(Col, Row) = Value;
}
LuaValue CLuaGrid::LuaGetValue(CLuaThread * pThreadInfo, UINT Col, UINT Row)
{
	return GetValue(Col, Row);
}