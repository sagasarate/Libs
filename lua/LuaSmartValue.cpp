#include "stdafx.h"

#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#undef new
#endif

IMPLEMENT_META_CLASS(CLuaSmartValue, _T("SmartValue"))

#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#define new NEWNEW
#endif



void CLuaSmartValue::Dump(CEasyString& Data) const
{
	Data.Format(_T("CLuaSmartValue(%u)(%s)"), m_Data.GetBufferLen(), IsInLuaHeap() ? _T("InLuaHeap"): _T("InCHeap"));
}

void CLuaSmartValue::ResgisterStaticFunctions(CBaseLuaVM* pLuaVM, LPCTSTR LibName)
{
	pLuaVM->RegisterStaticFunction(LibName, _T("New"), &CLuaSmartValue::LuaNew);
}
void CLuaSmartValue::RegisterMemberFunctions(lua_State * pLuaState) const
{
	CLuaBaseMetaClass::RegisterMemberFunctions(pLuaState);
	
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("Clone"), &CLuaSmartValue::LuaClone);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("GetData"), &CLuaSmartValue::LuaGetData);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("GetDataLen"), &CLuaSmartValue::LuaGetDataLen);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("GetLength"), &CLuaSmartValue::LuaGetLength);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("GetBufferLen"), &CLuaSmartValue::LuaGetBufferLen);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("Attach"), &CLuaSmartValue::LuaAttach);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("GetType"), &CLuaSmartValue::LuaGetType);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("SetValue"), &CLuaSmartValue::LuaSetValue);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("GetValue"), &CLuaSmartValue::LuaGetValue);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("GetValueAsByteArray"), &CLuaSmartValue::LuaGetValueAsByteArray);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("GetValueAsBoolean"), &CLuaSmartValue::LuaGetValueAsBoolean);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("GetValueAsSByte"), &CLuaSmartValue::LuaGetValueAsSByte);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("GetValueAsByte"), &CLuaSmartValue::LuaGetValueAsByte);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("GetValueAsShort"), &CLuaSmartValue::LuaGetValueAsShort);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("GetValueAsUShort"), &CLuaSmartValue::LuaGetValueAsUShort);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("GetValueAsInt"), &CLuaSmartValue::LuaGetValueAsInt);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("GetValueAsUInt"), &CLuaSmartValue::LuaGetValueAsUInt);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("GetValueAsInt64"), &CLuaSmartValue::LuaGetValueAsInt64);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("GetValueAsUInt64"), &CLuaSmartValue::LuaGetValueAsUInt64);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("GetValueAsFloat"), &CLuaSmartValue::LuaGetValueAsFloat);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("GetValueAsDouble"), &CLuaSmartValue::LuaGetValueAsDouble);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("GetValueAsString"), &CLuaSmartValue::LuaGetValueAsString);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("GetValueAsStruct"), &CLuaSmartValue::LuaGetValueAsStruct);
	RegisterMetaFunction<CLuaSmartValue>(pLuaState, _T("GetValueAsArray"), &CLuaSmartValue::LuaGetValueAsArray);
}
void CLuaSmartValue::OnGarbageCollect()
{
	if (m_IsInLuaHeap)
		this->~CLuaSmartValue();
}
LUA_EMPTY_VALUE CLuaSmartValue::LuaNew(CLuaThread* pLuaThread, LUA_EMPTY_VALUE)
{
	int ParamCount = lua_gettop(pLuaThread->GetLuaState());	
	CLuaByteArray* pByteArray = NULL;
	UINT StartIndex = 0;
	UINT DataLen = 0;
	CLuaSmartValue* pObject = NULL;
	int ClearType = CSmartValue::VT_UNKNOWN;
	if (ParamCount == 1)
	{
		pByteArray = pLuaThread->GetValue<CLuaByteArray*>(2);
		if (pByteArray)
		{
			DataLen = pByteArray->GetDataLen();
		}
		else
		{
			LogLua(_T("param 1 not a ByteArray"));
		}
	}
	else if (ParamCount == 2)
	{
		if (lua_isnumber(pLuaThread->GetLuaState(), 1) && lua_isnumber(pLuaThread->GetLuaState(), 2))
		{
			DataLen = (UINT)lua_tointeger(pLuaThread->GetLuaState(), 1);
			ClearType = (int)lua_tointeger(pLuaThread->GetLuaState(), 2);			
		}
		else
		{
			pByteArray = pLuaThread->GetValue<CLuaByteArray*>(2);
			if (pByteArray)
			{
				ClearType = (int)lua_tointeger(pLuaThread->GetLuaState(), 3);
			}
			else
			{
				LogLua(_T("param 1 not a ByteArray"));
			}			
		}
	}
	else if (ParamCount == 3)
	{
		pByteArray = pLuaThread->GetValue<CLuaByteArray*>(1);
		if (pByteArray)
		{
			ClearType = (UINT)lua_tointeger(pLuaThread->GetLuaState(), 2);
			StartIndex = (UINT)lua_tointeger(pLuaThread->GetLuaState(), 3);
			if (StartIndex < pByteArray->GetDataLen())
			{
				DataLen = pByteArray->GetDataLen() - StartIndex;
			}
			else
			{
				LogLua(_T("invalid StartIndex=%u on ByteArray(%u)"), StartIndex, pByteArray->GetDataLen());
			}
		}
		else
		{
			LogLua(_T("param 1 not a ByteArray"));
		}
	}
	else if (ParamCount >= 4)
	{
		pByteArray = pLuaThread->GetValue<CLuaByteArray*>(1);
		if (pByteArray)
		{
			ClearType = (int)lua_tointeger(pLuaThread->GetLuaState(), 2);
			StartIndex = (UINT)lua_tointeger(pLuaThread->GetLuaState(), 3);
			DataLen = (UINT)lua_tointeger(pLuaThread->GetLuaState(), 4);
			if (StartIndex + DataLen > pByteArray->GetDataLen())
			{
				LogLua(_T("invalid StartIndex=%u DataLen=%u on ByteArray(%u)"), StartIndex, DataLen, pByteArray->GetDataLen());
				DataLen = 0;
			}
		}
		else
		{
			LogLua(_T("param 1 not a ByteArray"));
		}
	}
	else
	{
		ClearType = CSmartValue::VT_NULL;
	}

	if (pByteArray && DataLen)
	{
		pObject = CLuaSmartValue::New(pLuaThread, pByteArray);
		if (pObject)
		{
			if (pObject->GetValue().Attach(pByteArray->GetData() + StartIndex, DataLen, ClearType))
			{
				
			}
			else
			{
				LogLua(_T("attach failed"));
			}
		}
		else
		{
			LogLua(_T("create failed"));
		}
	}
	else if (ClearType < CSmartValue::VT_UNKNOWN)
	{
		pObject = CLuaSmartValue::New(pLuaThread, NULL);
		if (pObject)
		{
			pObject->Create(ClearType, DataLen);
		}
		else
		{
			LogLua(_T("create failed"));
		}
	}
	else
	{
		LogLua(_T("invalid param type=%d size=%u"), ClearType, DataLen);
	}

	if (pObject == NULL)
		lua_pushnil(pLuaThread->GetLuaState());
	return LUA_EMPTY_VALUE();
}
LUA_EMPTY_VALUE CLuaSmartValue::LuaClone()
{
	CLuaSmartValue* pObject = CLuaSmartValue::New(m_pCurThread, NULL);
	if (pObject)
	{
		if (!pObject->GetValue().CloneFrom(GetValue()))
		{
			LogLua(_T("clone failed"));
		}
	}	
	return LUA_EMPTY_VALUE();
}
LUA_EMPTY_VALUE CLuaSmartValue::LuaGetData()
{
	if (m_Data.GetData() && m_Data.GetDataLen())
	{
		CLuaByteArray* pObject = CLuaByteArray::New(m_pCurThread, this);
		if (pObject)
		{
			pObject->Attach(m_Data.GetData(), m_Data.GetDataLen());
		}		
	}		
	else
	{
		m_pCurThread->PushNil();
	}		
	return LUA_EMPTY_VALUE();
}



UINT CLuaSmartValue::LuaGetDataLen()
{
	return m_Data.GetDataLen();
}
UINT CLuaSmartValue::LuaGetLength()
{
	return m_Data.GetLength();
}
UINT CLuaSmartValue::LuaGetBufferLen()
{
	return m_Data.GetBufferLen();
}
bool CLuaSmartValue::LuaAttach(CLuaBaseMetaClass* pObject)
{
	bool Attached = false;
	if (pObject)
	{
		int ParamCount = lua_gettop(m_pCurThread->GetLuaState());
		switch (pObject->GetMetaClassID())
		{
		case CLuaByteArray::CLASS_ID:			
			{
				CLuaByteArray* pByteArray = dynamic_cast<CLuaByteArray*>(pObject);
				if (pByteArray)
				{
					bool IsEmpty = true;
					if (ParamCount >= 3)
					{
						IsEmpty = lua_toboolean(m_pCurThread->GetLuaState(), 3);
					}
					UINT StartIndex = 0;
					UINT DataLen = pByteArray->GetDataLen();
					if (ParamCount >= 5)
					{
						StartIndex = (UINT)lua_tointeger(m_pCurThread->GetLuaState(), 4);
						DataLen = (UINT)lua_tointeger(m_pCurThread->GetLuaState(), 5);
						if (StartIndex + DataLen > pByteArray->GetDataLen())
						{
							luaL_error(m_pCurThread->GetLuaState(), "start(%u) len(%u) out of byte array range(%u)",
								StartIndex, DataLen, pByteArray->GetDataLen());
							return false;
						}
					}
					m_Data.Attach(pByteArray->GetData() + StartIndex, DataLen, IsEmpty);
					Attached = true;
				}
			}
			break;
		case CLuaSmartValue::CLASS_ID:
			{
				CLuaSmartValue* pValue = dynamic_cast<CLuaSmartValue*>(pObject);
				if (pValue)
				{
					m_Data = pValue->GetValue();
					Attached = true;
				}
			}
			break;
		case CLuaSmartStruct::CLASS_ID:
			{
				CLuaSmartStruct* pValue = dynamic_cast<CLuaSmartStruct*>(pObject);
				if (pValue)
				{
					m_Data = pValue->GetValue();
					Attached = true;
				}
			}
			break;
		}

		if (Attached && IsInLuaHeap())
		{
			//是内部对象，需要关联引用，避免被回收
			if (PushToLua(m_pCurThread))
			{
				if (pObject->PushToLua(m_pCurThread))
					AddBindObject(m_pCurThread->GetLuaState(), -2);
				else
					lua_pop(m_pCurThread->GetLuaState(), 1);
			}
			lua_pop(m_pCurThread->GetLuaState(), 1);
		}
	}
	return Attached;
}
int CLuaSmartValue::LuaGetType()
{
	return m_Data.GetType();
}
bool CLuaSmartValue::LuaSetValue(LUA_EMPTY_VALUE)
{
	int Type = CSmartValue::VT_UNKNOWN;
	int ParamCount = lua_gettop(m_pCurThread->GetLuaState());
	bool CanChageType = false;
	if (ParamCount >= 3)
	{
		if (lua_isnumber(m_pCurThread->GetLuaState(), 3))
		{
			Type = (int)lua_tointeger(m_pCurThread->GetLuaState(), 3);
			if (Type > CSmartValue::VT_UNKNOWN)
				Type = CSmartValue::VT_UNKNOWN;
			CanChageType = true;
		}
	}
	return m_pCurThread->PackValue(m_Data, 2, Type, CanChageType);
}
LUA_EMPTY_VALUE CLuaSmartValue::LuaGetValue()
{
	m_pCurThread->UnpackValue(m_Data, this);
	return LUA_EMPTY_VALUE();
}
LUA_EMPTY_VALUE CLuaSmartValue::LuaGetValueAsByteArray()
{
	LPVOID pData = m_Data.GetValueData();
	UINT DataSize = m_Data.GetLength();
	if (pData)
	{
		CLuaByteArray* pObject = CLuaByteArray::New(m_pCurThread, this);
		if (pObject)
		{
			pObject->Attach((BYTE*)m_Data, m_Data.GetLength());
		}		
	}
	else
	{
		m_pCurThread->PushNil();
	}
	return LUA_EMPTY_VALUE();
}
bool CLuaSmartValue::LuaGetValueAsBoolean()
{
	return m_Data;
}
char CLuaSmartValue::LuaGetValueAsSByte()
{
	return m_Data;
}
BYTE CLuaSmartValue::LuaGetValueAsByte()
{
	return m_Data;
}
short CLuaSmartValue::LuaGetValueAsShort()
{
	return m_Data;
}
WORD CLuaSmartValue::LuaGetValueAsUShort()
{
	return m_Data;
}
int CLuaSmartValue::LuaGetValueAsInt()
{
	return m_Data;
}
UINT CLuaSmartValue::LuaGetValueAsUInt()
{
	return m_Data;
}
INT64 CLuaSmartValue::LuaGetValueAsInt64()
{
	return m_Data;
}
UINT64 CLuaSmartValue::LuaGetValueAsUInt64()
{
	return m_Data;
}
float CLuaSmartValue::LuaGetValueAsFloat()
{
	return m_Data;
}
double CLuaSmartValue::LuaGetValueAsDouble()
{
	return m_Data;
}
LPCTSTR CLuaSmartValue::LuaGetValueAsString()
{
	return m_Data;
}
LUA_EMPTY_VALUE CLuaSmartValue::LuaGetValueAsStruct()
{
	CLuaSmartStruct* pObject = CLuaSmartStruct::New(m_pCurThread, this);
	if (pObject)
	{
		pObject->Attach(m_Data);		
	}
	return LUA_EMPTY_VALUE();
}
LUA_EMPTY_VALUE CLuaSmartValue::LuaGetValueAsArray()
{
	CLuaSmartArray* pObject = CLuaSmartArray::New(m_pCurThread, this);
	if (pObject)
	{
		pObject->Attach(m_Data);
	}
	return LUA_EMPTY_VALUE();
}