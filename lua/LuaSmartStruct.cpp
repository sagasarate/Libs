#include "stdafx.h"

#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#undef new
#endif

IMPLEMENT_META_CLASS(CLuaSmartStruct, _T("SmartStruct"))


#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#define new NEWNEW
#endif

void CLuaSmartStruct::Dump(CEasyString& Data) const
{
	Data.Format(_T("CLuaSmartStruct(%u)(%s)"), m_Data.GetBufferLen(), IsInLuaHeap() ? _T("InLuaHeap") : _T("InCHeap"));
}
void CLuaSmartStruct::ResgisterStaticFunctions(CBaseLuaVM* pLuaVM, LPCTSTR LibName)
{
	pLuaVM->RegisterStaticFunction(LibName, _T("New"), &CLuaSmartStruct::LuaNew);
	pLuaVM->RegisterStaticFunction(LibName, _T("GetEmptyStructSize"), &CLuaSmartStruct::LuaGetEmptyStructSize);
	pLuaVM->RegisterStaticFunction(LibName, _T("GetFixMemberSize"), &CLuaSmartStruct::LuaGetFixMemberSize);
	pLuaVM->RegisterStaticFunction(LibName, _T("GetArrayMemberSize"), &CLuaSmartStruct::LuaGetArrayMemberSize);
	pLuaVM->RegisterStaticFunction(LibName, _T("GetStructMemberSize"), &CLuaSmartStruct::LuaGetStructMemberSize);
	pLuaVM->RegisterStaticFunction(LibName, _T("GetStringMemberSize"), &CLuaSmartStruct::LuaGetStringMemberSize);
	pLuaVM->RegisterStaticFunction(LibName, _T("GetBinaryMemberSize"), &CLuaSmartStruct::LuaGetBinaryMemberSize);
	pLuaVM->RegisterStaticFunction(LibName, _T("GetVariedMemberSize"), &CLuaSmartStruct::LuaGetVariedMemberSize);
}
void CLuaSmartStruct::RegisterMemberFunctions(lua_State * pLuaState) const
{
	CLuaBaseMetaClass::RegisterMemberFunctions(pLuaState);

	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("Clone"), &CLuaSmartStruct::LuaClone);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("GetData"), &CLuaSmartStruct::LuaGetData);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("GetDataLen"), &CLuaSmartStruct::LuaGetDataLen);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("GetLength"), &CLuaSmartStruct::LuaGetLength);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("GetBufferLen"), &CLuaSmartStruct::LuaGetBufferLen);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("GetFreeLen"), &CLuaSmartStruct::LuaGetFreeLen);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("Attach"), &CLuaSmartStruct::LuaAttach);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("CloneFrom"), &CLuaSmartStruct::LuaCloneFrom);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("Clear"), &CLuaSmartStruct::LuaClear);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("GetMember"), &CLuaSmartStruct::LuaGetMember);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("GetMemberObject"), &CLuaSmartStruct::LuaGetMemberObject);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("AddMember"), &CLuaSmartStruct::LuaAddMember);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("AddMemberBoolean"), &CLuaSmartStruct::LuaAddMemberBoolean);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("AddMemberFloat"), &CLuaSmartStruct::LuaAddMemberFloat);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("AddMemberDouble"), &CLuaSmartStruct::LuaAddMemberDouble);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("AddMemberByte"), &CLuaSmartStruct::LuaAddMemberByte);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("AddMemberSByte"), &CLuaSmartStruct::LuaAddMemberSByte);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("AddMemberShort"), &CLuaSmartStruct::LuaAddMemberShort);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("AddMemberUShort"), &CLuaSmartStruct::LuaAddMemberUShort);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("AddMemberInt"), &CLuaSmartStruct::LuaAddMemberInt);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("AddMemberUInt"), &CLuaSmartStruct::LuaAddMemberUInt);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("AddMemberInt64"), &CLuaSmartStruct::LuaAddMemberInt64);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("AddMemberUInt64"), &CLuaSmartStruct::LuaAddMemberUInt64);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("GetFirstMemberPosition"), &CLuaSmartStruct::LuaGetFirstMemberPosition);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("GetNextMember"), &CLuaSmartStruct::LuaGetNextMember);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("GetNextMemberObject"), &CLuaSmartStruct::LuaGetNextMemberObject);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("PrepareSubStruct"), &CLuaSmartStruct::LuaPrepareSubStruct);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("PrepareSubArray"), &CLuaSmartStruct::LuaPrepareSubArray);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("FinishMember"), &CLuaSmartStruct::LuaFinishMember);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("Append"), &CLuaSmartStruct::LuaAppend);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("Expand"), &CLuaSmartStruct::LuaExpand);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, _T("IsMemberExist"), &CLuaSmartStruct::LuaIsMemberExist);
}
void CLuaSmartStruct::OnGarbageCollect()
{
	if (m_IsInLuaHeap)
		this->~CLuaSmartStruct();
}
LUA_EMPTY_VALUE CLuaSmartStruct::LuaNew(CLuaThread* pLuaThread, LUA_EMPTY_VALUE)
{
	int ParamCount = lua_gettop(pLuaThread->GetLuaState());
	CLuaByteArray* pByteArray = NULL;
	UINT StartIndex = 0;
	UINT DataLen = 0;
	CLuaSmartStruct* pObject = NULL;
	bool IsEmpty = true;
	if (ParamCount)
	{
		pByteArray = pLuaThread->GetValue<CLuaByteArray*>(1);
		if (pByteArray)
		{
			if (ParamCount == 2)
			{
				StartIndex = (UINT)lua_tointeger(pLuaThread->GetLuaState(), 2);
				if (StartIndex < pByteArray->GetDataLen())
				{
					DataLen = pByteArray->GetDataLen() - StartIndex;
				}
				else
				{
					LogLua(_T("invalid StartIndex=%u on ByteArray(%u)"), StartIndex, pByteArray->GetDataLen());
				}
			}
			else if (ParamCount == 3)
			{
				StartIndex = (UINT)lua_tointeger(pLuaThread->GetLuaState(), 2);
				DataLen = (UINT)lua_tointeger(pLuaThread->GetLuaState(), 3);
			}
			else if (ParamCount >= 4)
			{
				StartIndex = (UINT)lua_tointeger(pLuaThread->GetLuaState(), 2);
				DataLen = (UINT)lua_tointeger(pLuaThread->GetLuaState(), 3);
				IsEmpty = lua_toboolean(pLuaThread->GetLuaState(), 4) != 0;
			}
			else
			{
				DataLen = pByteArray->GetDataLen();
			}
			
			if (StartIndex + DataLen > pByteArray->GetDataLen())
			{
				LogLua(_T("invalid StartIndex=%u DataLen=%u on ByteArray(%u)"), StartIndex, DataLen, pByteArray->GetDataLen());
				DataLen = 0;
			}
		}
		else
		{
			DataLen = (UINT)lua_tointeger(pLuaThread->GetLuaState(), 1);
		}		
	}	

	if (pByteArray && DataLen)
	{
		pObject = CLuaSmartStruct::New(pLuaThread, pByteArray);
		if (pObject)
		{
			if (pObject->GetValue().Attach(pByteArray->GetData() + StartIndex, DataLen, IsEmpty))
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
	else if (DataLen)
	{
		pObject = CLuaSmartStruct::New(pLuaThread, NULL);
		if (pObject)
		{
			pObject->Create(DataLen);
		}
		else
		{
			LogLua(_T("create failed"));
		}
	}
	else
	{
		LogLua(_T("invalid param size=%u"), DataLen);
	}

	if (pObject == NULL)
		lua_pushnil(pLuaThread->GetLuaState());
	return LUA_EMPTY_VALUE();
}
UINT CLuaSmartStruct::LuaGetEmptyStructSize(CLuaThread* pLuaThread)
{
	return CSmartStruct::GetEmptyStructSize();
}
UINT CLuaSmartStruct::LuaGetFixMemberSize(CLuaThread* pLuaThread, UINT TypeLen)
{
	return CSmartStruct::GetFixMemberSize(TypeLen);
}
UINT CLuaSmartStruct::LuaGetArrayMemberSize(CLuaThread* pLuaThread, UINT ArraySize)
{
	return CSmartStruct::GetArrayMemberSize(ArraySize);
}
UINT CLuaSmartStruct::LuaGetStructMemberSize(CLuaThread* pLuaThread, UINT StructSize)
{
	return CSmartStruct::GetStructMemberSize(StructSize);
}
UINT CLuaSmartStruct::LuaGetStringMemberSize(CLuaThread* pLuaThread, CEasyString Str)
{
	return CSmartStruct::GetStringMemberSize(Str);
}
UINT CLuaSmartStruct::LuaGetBinaryMemberSize(CLuaThread* pLuaThread, UINT DataLen)
{
	return CSmartStruct::GetBinaryMemberSize(DataLen);
}
UINT CLuaSmartStruct::LuaGetVariedMemberSize(CLuaThread* pLuaThread, LUA_EMPTY_VALUE)
{
	switch (lua_type(pLuaThread->GetLuaState(), 1))
	{
	case LUA_TNIL:
		return sizeof(WORD) + sizeof(BYTE);
	case LUA_TBOOLEAN:
		return CSmartStruct::GetFixMemberSize(sizeof(bool));
	case LUA_TNUMBER:
		return CSmartStruct::GetFixMemberSize(sizeof(double));
	case LUA_TSTRING:
		{
			LPCTSTR szStr;
			CEasyString Temp;
			if (LUA_SCRIPT_CODE_PAGE == CEasyString::SYSTEM_STRING_CODE_PAGE)
			{
				szStr = (LPCTSTR)lua_tostring(pLuaThread->GetLuaState(), 1);
			}
			else
			{
				LuaStrToSystemStr(lua_tostring(pLuaThread->GetLuaState(), 1), Temp);
				szStr = Temp;
			}
			return CSmartStruct::GetStringMemberSize(szStr);
		}
	case LUA_TINTEGER:
		return CSmartStruct::GetFixMemberSize(sizeof(__int64));
	}
	return sizeof(WORD) + sizeof(BYTE);
}
UINT CLuaSmartStruct::LuaGetDataLen()
{
	return m_Data.GetDataLen();
}
UINT CLuaSmartStruct::LuaGetLength()
{
	return m_Data.GetLength();
}
UINT CLuaSmartStruct::LuaGetBufferLen()
{
	return m_Data.GetBufferLen();
}
UINT CLuaSmartStruct::LuaGetFreeLen()
{
	return m_Data.GetFreeLen();
}
LUA_EMPTY_VALUE CLuaSmartStruct::LuaGetData()
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

bool CLuaSmartStruct::LuaAttach(CLuaBaseMetaClass* pObject)
{
	bool Attached = false;
	if(pObject)
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
					if (ParamCount  >= 5)
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
LUA_EMPTY_VALUE CLuaSmartStruct::LuaClone()
{
	CLuaSmartStruct* pObject = CLuaSmartStruct::New(m_pCurThread, NULL);
	if (pObject)
	{
		if (!pObject->GetValue().CloneFrom(GetValue()))
		{
			LogLua(_T("clone failed"));
		}
	}	
	return LUA_EMPTY_VALUE();
}
bool CLuaSmartStruct::LuaCloneFrom(CLuaSmartStruct* pObject)
{
	if (pObject)
		return m_Data.CloneFrom(pObject->GetValue());
	return false;
}
void CLuaSmartStruct::LuaClear()
{
	m_Data.Clear();
}
LUA_EMPTY_VALUE CLuaSmartStruct::LuaGetMember(WORD MemberID)
{
	m_pCurThread->UnpackValue(m_Data.GetMember(MemberID), this);
	
	return LUA_EMPTY_VALUE();
}
LUA_EMPTY_VALUE CLuaSmartStruct::LuaGetMemberObject(WORD MemberID)
{
	CLuaSmartValue* pObject = CLuaSmartValue::New(m_pCurThread, this);
	if (pObject)
	{
		pObject->Attach(m_Data.GetMember(MemberID));		
	}
	return LUA_EMPTY_VALUE();
}
bool CLuaSmartStruct::LuaAddMember(WORD MemberID, LUA_EMPTY_VALUE)
{
	CSmartValue Value;
	if (m_Data.PrepareMember(CSmartValue::VT_NULL, Value))
	{
		if (m_pCurThread->PackValue(Value, 3, true))
		{
			m_Data.FinishMember(MemberID, Value.GetDataLen());
			return true;
		}
	}
	return false;
}
bool CLuaSmartStruct::LuaAddMemberBoolean(WORD MemberID, bool Value)
{
	return m_Data.AddMember(MemberID, Value);
}
bool CLuaSmartStruct::LuaAddMemberFloat(WORD MemberID, float Value)
{
	return m_Data.AddMember(MemberID, Value);
}
bool CLuaSmartStruct::LuaAddMemberDouble(WORD MemberID, double Value)
{
	return m_Data.AddMember(MemberID, Value);
}
bool CLuaSmartStruct::LuaAddMemberByte(WORD MemberID, BYTE Value)
{
	return m_Data.AddMember(MemberID, Value);
}
bool CLuaSmartStruct::LuaAddMemberSByte(WORD MemberID, char Value)
{
	return m_Data.AddMember(MemberID, Value);
}
bool CLuaSmartStruct::LuaAddMemberShort(WORD MemberID, short Value)
{
	return m_Data.AddMember(MemberID, Value);
}
bool CLuaSmartStruct::LuaAddMemberUShort(WORD MemberID, WORD Value)
{
	return m_Data.AddMember(MemberID, Value);
}
bool CLuaSmartStruct::LuaAddMemberInt(WORD MemberID, int Value)
{
	return m_Data.AddMember(MemberID, Value);
}
bool CLuaSmartStruct::LuaAddMemberUInt(WORD MemberID, UINT Value)
{
	return m_Data.AddMember(MemberID, Value);
}
bool CLuaSmartStruct::LuaAddMemberInt64(WORD MemberID, INT64 Value)
{
	return m_Data.AddMember(MemberID, Value);
}
bool CLuaSmartStruct::LuaAddMemberUInt64(WORD MemberID, UINT64 Value)
{
	return m_Data.AddMember(MemberID, Value);
}
LUA_EMPTY_VALUE CLuaSmartStruct::LuaGetFirstMemberPosition()
{
	LPVOID Pos = m_Data.GetFirstMemberPosition();
	if (Pos)
		m_pCurThread->PushValue(Pos);
	else
		m_pCurThread->PushNil();
	return LUA_EMPTY_VALUE();
}
LUA_CUSTOM_RETURN CLuaSmartStruct::LuaGetNextMember(LPVOID Pos)
{
	WORD MemberID;
	m_pCurThread->UnpackValue(m_Data.GetNextMember(Pos, MemberID), this);
	m_pCurThread->PushInteger(MemberID);
	if (Pos)
		m_pCurThread->PushValue(Pos);
	else
		m_pCurThread->PushNil();
	LUA_CUSTOM_RETURN Return;
	Return.ReturnCount = 3;
	return Return;
}
LUA_CUSTOM_RETURN CLuaSmartStruct::LuaGetNextMemberObject(LPVOID Pos)
{
	LUA_CUSTOM_RETURN Return;
	Return.ReturnCount = 0;
	CLuaSmartValue* pObject = CLuaSmartValue::New(m_pCurThread, this);
	if (pObject)
	{
		WORD MemberID;
		pObject->Attach(m_Data.GetNextMember(Pos, MemberID));		
		m_pCurThread->PushInteger(MemberID);
		if (Pos)
			m_pCurThread->PushValue(Pos);
		else
			m_pCurThread->PushNil();
		Return.ReturnCount = 3;
	}	
	return Return;
}
LUA_EMPTY_VALUE CLuaSmartStruct::LuaPrepareSubStruct()
{
	CLuaSmartStruct* pObject = New(m_pCurThread, this);
	if (pObject)
	{
		pObject->Attach(m_Data.PrepareSubStruct());		
	}
	return LUA_EMPTY_VALUE();
}
LUA_EMPTY_VALUE CLuaSmartStruct::LuaPrepareSubArray()
{
	CLuaSmartArray* pObject = CLuaSmartArray::New(m_pCurThread, this);
	if (pObject)
	{
		pObject->Attach(m_Data.PrepareSubArray());
	}
	return LUA_EMPTY_VALUE();
}
bool CLuaSmartStruct::LuaFinishMember(WORD MemberID, UINT MemberSize)
{
	return m_Data.FinishMember(MemberID, MemberSize);
}
bool CLuaSmartStruct::LuaAppend(CLuaSmartValue* pStruct)
{
	if(pStruct)
		return m_Data.Append(pStruct->GetValue());
	return false;
}
bool CLuaSmartStruct::LuaExpand(UINT ExpandSize)
{
	return m_Data.Expand(ExpandSize);
}
bool CLuaSmartStruct::LuaIsMemberExist(WORD MemberID)
{
	return m_Data.IsMemberExist(MemberID);
}