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

void CLuaSmartValue::PushValueToLua(CLuaThread* pLuaThread, const CSmartValue& Value, CLuaBaseMetaClass* pParent)
{
	switch (Value.GetType())
	{
	case CSmartValue::VT_CHAR:
	case CSmartValue::VT_UCHAR:
	case CSmartValue::VT_SHORT:
	case CSmartValue::VT_USHORT:
	case CSmartValue::VT_INT:
	case CSmartValue::VT_UINT:
	case CSmartValue::VT_BIGINT:
	case CSmartValue::VT_UBIGINT:
		lua_pushinteger(pLuaThread->GetLuaState(), Value);
		break;
	case CSmartValue::VT_FLOAT:
	case CSmartValue::VT_DOUBLE:
		lua_pushnumber(pLuaThread->GetLuaState(), Value);
		break;
	case CSmartValue::VT_STRING_UTF8:
		{
			LPCSTR szStr;
			CEasyStringA Temp;
			if (LUA_SCRIPT_CODE_PAGE == CEasyString::STRING_CODE_PAGE_UTF8)
			{
				szStr = (LPCSTR)Value;
			}
			else
			{
				SystemStrToLuaStr((LPCSTR)Value, Temp);
				szStr = Temp;
			}
			lua_pushstring(pLuaThread->GetLuaState(), szStr);
		}
		break;
	case CSmartValue::VT_STRING_ANSI:
		{
			LPCSTR szStr;
			CEasyStringA Temp;
			if (LUA_SCRIPT_CODE_PAGE == CEasyString::STRING_CODE_PAGE_ANSI)
			{
				szStr = (LPCSTR)Value;
			}
			else
			{
				SystemStrToLuaStr((LPCSTR)Value, Temp);
				szStr = Temp;
			}
			lua_pushstring(pLuaThread->GetLuaState(), szStr);
		}
		break;
	case CSmartValue::VT_STRING_UCS16:
		{
			CEasyStringA StringA;
			SystemStrToLuaStr((LPCWSTR)Value, StringA);
			lua_pushstring(pLuaThread->GetLuaState(), StringA);
		}
		break;
	case CSmartValue::VT_STRUCT:
		{
			CLuaSmartStruct* pObject = CLuaSmartStruct::New(pLuaThread, pParent);
			if (pObject)
				pObject->Attach(Value);
		}
		break;
	case CSmartValue::VT_ARRAY:
		{
			CLuaSmartArray* pObject = CLuaSmartArray::New(pLuaThread, pParent);
			if (pObject)
				pObject->Attach(Value);
		}
		break;
	case CSmartValue::VT_BOOL:
		lua_pushboolean(pLuaThread->GetLuaState(), Value);
		break;
	default:
		lua_pushnil(pLuaThread->GetLuaState());
	}
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
			DataLen = lua_tointeger(pLuaThread->GetLuaState(), 1);
			ClearType = lua_tointeger(pLuaThread->GetLuaState(), 2);			
		}
		else
		{
			pByteArray = pLuaThread->GetValue<CLuaByteArray*>(2);
			if (pByteArray)
			{
				ClearType = lua_tointeger(pLuaThread->GetLuaState(), 3);
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
			ClearType = lua_tointeger(pLuaThread->GetLuaState(), 2);
			StartIndex = lua_tointeger(pLuaThread->GetLuaState(), 3);
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
			ClearType = lua_tointeger(pLuaThread->GetLuaState(), 2);
			StartIndex = lua_tointeger(pLuaThread->GetLuaState(), 3);
			DataLen = lua_tointeger(pLuaThread->GetLuaState(), 4);
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
						StartIndex = lua_tointeger(m_pCurThread->GetLuaState(), 4);
						DataLen = lua_tointeger(m_pCurThread->GetLuaState(), 5);
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
	int Type = GetLuaObjectType(m_pCurThread->GetLuaState(), 2);
	switch (Type)
	{
	case LUA_TNIL:
		m_Data.Destory();
		return true;
	case LUA_TBOOLEAN:
		m_Data.SetValue(lua_toboolean(m_pCurThread->GetLuaState(), 2) != 0);
		return true;
	case LUA_TNUMBER:
		m_Data.SetValue(lua_tonumber(m_pCurThread->GetLuaState(), 2));
		return true;
	case LUA_TSTRING:
		{
			LPCTSTR szStr;
			CEasyString Temp;
			if (LUA_SCRIPT_CODE_PAGE == CEasyString::SYSTEM_STRING_CODE_PAGE)
			{
				szStr = (LPCTSTR)lua_tostring(m_pCurThread->GetLuaState(), 2);
			}
			else
			{
				LuaStrToSystemStr(lua_tostring(m_pCurThread->GetLuaState(), 2), Temp);
				szStr = Temp;
			}
			m_Data.SetValue(szStr);
		}		
		return true;
	case LUA_TTABLE:
		return false;
	case LUA_TINTEGER:
		m_Data.SetValue(lua_tointeger(m_pCurThread->GetLuaState(), 2));
		return true;
	case CLuaByteArray::CLASS_ID:
		{
			CLuaSmartValue * pObject = dynamic_cast<CLuaSmartValue *>((CLuaBaseMetaClass *)lua_touserdata(m_pCurThread->GetLuaState(), 2));
			if (pObject)
			{
				m_Data.SetString((LPCSTR)pObject->GetValue().GetData(), pObject->GetValue().GetDataLen());
				return true;
			}
		}
		break;
	case CLuaSmartValue::CLASS_ID:
		{
			CLuaSmartValue * pObject = dynamic_cast<CLuaSmartValue *>((CLuaBaseMetaClass*)lua_touserdata(m_pCurThread->GetLuaState(), 2));
			if (pObject)
			{
				m_Data.SetValue(pObject->m_Data);
				return true;
			}
		}
		break;
	case CLuaSmartStruct::CLASS_ID:
		{
			CLuaSmartStruct * pObject = dynamic_cast<CLuaSmartStruct *>((CLuaBaseMetaClass*)lua_touserdata(m_pCurThread->GetLuaState(), 2));
			if (pObject)
			{
				m_Data.SetValue(pObject->GetValue());
				return true;
			}
		}
		break;
	}
	return false;
}
LUA_EMPTY_VALUE CLuaSmartValue::LuaGetValue()
{
	CLuaSmartValue::PushValueToLua(m_pCurThread, m_Data, this);
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