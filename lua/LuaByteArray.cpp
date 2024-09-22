#include "stdafx.h"

#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#undef new
#endif

IMPLEMENT_META_CLASS(CLuaByteArray, _T("ByteArray"))

#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#define new NEWNEW
#endif

void CLuaByteArray::Dump(CEasyString& Data) const
{
	Data.Format(_T("CLuaByteArray(%u)(%s)"), m_DataLen, IsInLuaHeap() ? _T("InLuaHeap") : _T("InCHeap"));
}

void CLuaByteArray::ResgisterStaticFunctions(CBaseLuaVM* pLuaVM, LPCTSTR LibName)
{
	pLuaVM->RegisterStaticFunction(LibName, _T("New"), &CLuaByteArray::LuaNew);
}
void CLuaByteArray::RegisterMemberFunctions(lua_State * pLuaState) const
{
	CLuaBaseMetaClass::RegisterMemberFunctions(pLuaState);

	RegisterMetaFunction<CLuaByteArray>(pLuaState, _T("Clone"), &CLuaByteArray::LuaClone);
	RegisterMetaFunction<CLuaByteArray>(pLuaState, _T("GetDataLen"), &CLuaByteArray::GetDataLen);
	RegisterMetaFunction<CLuaByteArray>(pLuaState, _T("SetValue"), &CLuaByteArray::SetValue);
	RegisterMetaFunction<CLuaByteArray>(pLuaState, _T("GetValue"), &CLuaByteArray::LuaGetValue);
	RegisterMetaFunction<CLuaByteArray>(pLuaState, _T("SubArray"), &CLuaByteArray::LuaSubArray);
}

void CLuaByteArray::OnGarbageCollect()
{
	if (m_IsInLuaHeap)
		this->~CLuaByteArray();
}
LUA_EMPTY_VALUE CLuaByteArray::LuaNew(CLuaThread* pLuaThread, UINT Size)
{
	CLuaByteArray* pObject = CLuaByteArray::New(pLuaThread, NULL);
	if (pObject)
	{
		pObject->Create(Size);
	}	
	return LUA_EMPTY_VALUE();
}
LUA_EMPTY_VALUE CLuaByteArray::LuaClone()
{
	CLuaByteArray* pObject = CLuaByteArray::New(m_pCurThread, NULL);
	if (pObject)
	{
		pObject->CloneFrom(*pObject);
	}	
	return LUA_EMPTY_VALUE();
}

bool CLuaByteArray::SetValue(UINT Index, BYTE Value)
{
	if (Index < m_DataLen)
	{
		m_pData[Index] = Value;
		return true;
	}
	return false;
}
LUA_EMPTY_VALUE CLuaByteArray::LuaGetValue(UINT Index)
{
	if (Index < m_DataLen)
	{
		m_pCurThread->PushValue(m_pData[Index]);
	}
	else
	{
		m_pCurThread->PushNil();
	}
	return LUA_EMPTY_VALUE();
}

LUA_EMPTY_VALUE CLuaByteArray::LuaToString()
{
	if (m_pData && m_DataLen)
	{
		CEasyStringA OutStr;

		OutStr.Resize(m_DataLen * 3, false);
		for (UINT i = 0; i < m_DataLen; i++)
		{
			sprintf_s((char*)OutStr.GetBuffer() + i * 3, OutStr.GetBufferSize() - i * 3, "%02X ", m_pData[i]);
		}
		OutStr.TrimBuffer();
		m_pCurThread->PushString(OutStr.GetBuffer());
	}
	else
	{
		m_pCurThread->PushString("empty");
	}
	
	return LUA_EMPTY_VALUE();
}
LUA_EMPTY_VALUE CLuaByteArray::LuaSubArray(UINT StartIndex)
{
	int ParamCount = lua_gettop(m_pCurThread->GetLuaState());
	UINT DataLen = 0;
	if (ParamCount >= 2)
	{
		DataLen = (UINT)lua_tointeger(m_pCurThread->GetLuaState(), 2);
		if (StartIndex + DataLen > m_DataLen)
		{
			DataLen = 0;
			LogLua(_T("StartIndex(%u) DataLen(%u) out of bound(%u)"), StartIndex, DataLen, m_DataLen);
		}
	}
	else
	{
		if (StartIndex < m_DataLen)
		{
			DataLen = m_DataLen - StartIndex;
		}
		else
		{
			LogLua(_T("StartIndex(%u) out of bound(%u)"), StartIndex, m_DataLen);
		}
	}
	if (DataLen)
	{
		CLuaByteArray* pObject = CLuaByteArray::New(m_pCurThread, this);
		if (pObject)
		{
			pObject->Attach(m_pData + StartIndex, DataLen);
		}		
	}
	else
	{
		lua_pushnil(m_pCurThread->GetLuaState());
	}
	return LUA_EMPTY_VALUE();
}