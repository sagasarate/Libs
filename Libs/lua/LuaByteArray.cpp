#include "stdafx.h"

IMPLEMENT_STATIC_META_CLASS(CLuaByteArray)





void CLuaByteArray::RegisterMemberFunctions(lua_State * pLuaState) const
{
	CLuaBaseStaticMetaClass::RegisterMemberFunctions(pLuaState);

	RegisterMetaFunction<CLuaByteArray>(pLuaState, "GetSize", &CLuaByteArray::LuaGetSize);
	RegisterMetaFunction<CLuaByteArray>(pLuaState, "SetValue", &CLuaByteArray::LuaSetValue);
	RegisterMetaFunction<CLuaByteArray>(pLuaState, "GetValue", &CLuaByteArray::LuaGetValue);
}

UINT CLuaByteArray::LuaGetSize(CLuaThread * pThreadInfo)
{
	return GetDataLen();
}
bool CLuaByteArray::LuaSetValue(CLuaThread * pThreadInfo, UINT Index, BYTE Value)
{
	if (Index < m_DataLen)
	{
		m_pData[Index] = Value;
		return true;
	}
	return false;
}
LUA_EMPTY_VALUE CLuaByteArray::LuaGetValue(CLuaThread * pThreadInfo, UINT Index)
{
	if (Index < m_DataLen)
	{
		pThreadInfo->PushValue(m_pData[Index]);
	}
	else
	{
		pThreadInfo->PushNil();
	}

	return LUA_EMPTY_VALUE();
}

LUA_EMPTY_VALUE CLuaByteArray::LuaToString(CLuaThread * pThreadInfo)
{
	if (m_pData&&m_DataLen)
	{
		CEasyStringA OutStr;

		OutStr.Resize(m_DataLen * 3, false);
		for (UINT i = 0; i < m_DataLen; i++)
		{
			sprintf_s((char *)OutStr.GetBuffer() + i * 3, OutStr.GetBufferSize() - i * 3, "%02X ", m_pData[i]);
		}
		OutStr.TrimBuffer();
		pThreadInfo->PushString(OutStr.GetBuffer());
	}
	else
	{
		pThreadInfo->PushString("empty");
	}
	return LUA_EMPTY_VALUE();
}