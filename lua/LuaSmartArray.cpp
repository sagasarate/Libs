#include "stdafx.h"

#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#undef new
#endif

IMPLEMENT_META_CLASS(CLuaSmartArray, _T("SmartArray"))

#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#define new NEWNEW
#endif

void CLuaSmartArray::Dump(CEasyString& Data) const
{
	Data.Format(_T("CLuaSmartArray(%u)(%s)"), m_Data.GetBufferLen(), IsInLuaHeap() ? _T("InLuaHeap") : _T("InCHeap"));
}

void CLuaSmartArray::ResgisterStaticFunctions(CBaseLuaVM* pLuaVM, LPCTSTR LibName)
{
	pLuaVM->RegisterStaticFunction(LibName, _T("New"), &CLuaSmartArray::LuaNew);
	pLuaVM->RegisterStaticFunction(LibName, _T("GetEmptyArraySize"), &CLuaSmartArray::LuaGetEmptyArraySize);
	pLuaVM->RegisterStaticFunction(LibName, _T("GetFixMemberSize"), &CLuaSmartArray::LuaGetFixMemberSize);
	pLuaVM->RegisterStaticFunction(LibName, _T("GetArrayMemberSize"), &CLuaSmartArray::LuaGetArrayMemberSize);
	pLuaVM->RegisterStaticFunction(LibName, _T("GetStructMemberSize"), &CLuaSmartArray::LuaGetStructMemberSize);
	pLuaVM->RegisterStaticFunction(LibName, _T("GetStringMemberSize"), &CLuaSmartArray::LuaGetStringMemberSize);
	pLuaVM->RegisterStaticFunction(LibName, _T("GetBinaryMemberSize"), &CLuaSmartArray::LuaGetBinaryMemberSize);
	pLuaVM->RegisterStaticFunction(LibName, _T("GetVariedMemberSize"), &CLuaSmartArray::LuaGetVariedMemberSize);
}
void CLuaSmartArray::RegisterMemberFunctions(lua_State* pLuaState) const
{
	CLuaBaseMetaClass::RegisterMemberFunctions(pLuaState);

	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("Clone"), &CLuaSmartArray::LuaClone);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("GetData"), &CLuaSmartArray::LuaGetData);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("GetDataLen"), &CLuaSmartArray::LuaGetDataLen);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("GetLength"), &CLuaSmartArray::LuaGetLength);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("GetBufferLen"), &CLuaSmartArray::LuaGetBufferLen);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("GetArrayLength"), &CLuaSmartArray::LuaGetArrayLength);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("GetFreeLen"), &CLuaSmartArray::LuaGetFreeLen);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("GetElementType"), &CLuaSmartArray::LuaGetElementType);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("GetElementSize"), &CLuaSmartArray::LuaGetElementSize);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("Attach"), &CLuaSmartArray::LuaAttach);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("CloneFrom"), &CLuaSmartArray::LuaCloneFrom);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("Clear"), &CLuaSmartArray::LuaClear);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("GetMember"), &CLuaSmartArray::LuaGetMember);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("GetMemberObject"), &CLuaSmartArray::LuaGetMemberObject);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("AddMember"), &CLuaSmartArray::LuaAddMember);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("AddMemberFloat"), &CLuaSmartArray::LuaAddMemberFloat);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("AddMemberDouble"), &CLuaSmartArray::LuaAddMemberDouble);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("AddMemberByte"), &CLuaSmartArray::LuaAddMemberByte);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("AddMemberSByte"), &CLuaSmartArray::LuaAddMemberSByte);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("AddMemberShort"), &CLuaSmartArray::LuaAddMemberShort);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("AddMemberUShort"), &CLuaSmartArray::LuaAddMemberUShort);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("AddMemberInt"), &CLuaSmartArray::LuaAddMemberInt);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("AddMemberUInt"), &CLuaSmartArray::LuaAddMemberUInt);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("AddMemberInt64"), &CLuaSmartArray::LuaAddMemberInt64);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("AddMemberUInt64"), &CLuaSmartArray::LuaAddMemberUInt64);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("GetFirstMemberPosition"), &CLuaSmartArray::LuaGetFirstMemberPosition);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("GetNextMember"), &CLuaSmartArray::LuaGetNextMember);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("GetNextMemberObject"), &CLuaSmartArray::LuaGetNextMemberObject);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("PrepareSubStruct"), &CLuaSmartArray::LuaPrepareSubStruct);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("PrepareSubArray"), &CLuaSmartArray::LuaPrepareSubArray);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("FinishMember"), &CLuaSmartArray::LuaFinishMember);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("Append"), &CLuaSmartArray::LuaAppend);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("Expand"), &CLuaSmartArray::LuaExpand);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("GetArray"), &CLuaSmartArray::LuaGetArray);
	RegisterMetaFunction<CLuaSmartArray>(pLuaState, _T("AddArray"), &CLuaSmartArray::LuaAddArray);
}


void CLuaSmartArray::OnGarbageCollect()
{
	if (m_IsInLuaHeap)
		this->~CLuaSmartArray();
}

LUA_EMPTY_VALUE CLuaSmartArray::LuaNew(CLuaThread* pLuaThread, LUA_EMPTY_VALUE)
{
	int ParamCount = lua_gettop(pLuaThread->GetLuaState());
	CLuaByteArray* pByteArray = NULL;
	UINT StartIndex = 0;
	UINT DataLen = 0;
	CLuaSmartArray* pObject = NULL;
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
		pObject = CLuaSmartArray::New(pLuaThread, pByteArray);
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
		pObject = CLuaSmartArray::New(pLuaThread, NULL);
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
UINT CLuaSmartArray::LuaGetEmptyArraySize(CLuaThread* pLuaThread)
{
	return CSmartArray::GetEmptyArraySize();
}
UINT CLuaSmartArray::LuaGetFixMemberSize(CLuaThread* pLuaThread, UINT TypeLen)
{
	return CSmartArray::GetFixMemberSize(TypeLen);
}
UINT CLuaSmartArray::LuaGetArrayMemberSize(CLuaThread* pLuaThread, UINT ArraySize)
{
	return CSmartArray::GetArrayMemberSize(ArraySize);
}
UINT CLuaSmartArray::LuaGetStructMemberSize(CLuaThread* pLuaThread, UINT StructSize)
{
	return CSmartArray::GetStructMemberSize(StructSize);
}
UINT CLuaSmartArray::LuaGetStringMemberSize(CLuaThread* pLuaThread, LPCTSTR Str)
{
	return CSmartArray::GetStringMemberSize(Str);
}
UINT CLuaSmartArray::LuaGetBinaryMemberSize(CLuaThread* pLuaThread, UINT DataLen)
{
	return CSmartArray::GetBinaryMemberSize(DataLen);
}
UINT CLuaSmartArray::LuaGetVariedMemberSize(CLuaThread* pLuaThread, LUA_EMPTY_VALUE)
{
	switch (lua_type(pLuaThread->GetLuaState(), 1))
	{
	case LUA_TNIL:
		return sizeof(BYTE);
	case LUA_TBOOLEAN:
		return CSmartArray::GetFixMemberSize(sizeof(bool));
	case LUA_TNUMBER:
		return CSmartArray::GetFixMemberSize(sizeof(double));
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
		return CSmartArray::GetStringMemberSize(szStr);
	}
	case LUA_TINTEGER:
		return CSmartArray::GetFixMemberSize(sizeof(__int64));
	}
	return sizeof(BYTE);
}
UINT CLuaSmartArray::LuaGetDataLen()
{
	return m_Data.GetDataLen();
}
UINT CLuaSmartArray::LuaGetLength()
{
	return m_Data.GetLength();
}
UINT CLuaSmartArray::LuaGetBufferLen()
{
	return m_Data.GetBufferLen();
}
UINT CLuaSmartArray::LuaGetArrayLength()
{
	return m_Data.GetArrayLength();
}
UINT CLuaSmartArray::LuaGetFreeLen()
{
	return m_Data.GetFreeLen();
}
LUA_EMPTY_VALUE CLuaSmartArray::LuaGetData()
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

BYTE CLuaSmartArray::LuaGetElementType()
{
	return m_Data.GetElementType();
}
UINT CLuaSmartArray::LuaGetElementSize()
{
	return m_Data.GetElementSize();
}
bool CLuaSmartArray::LuaAttach(CLuaBaseMetaClass* pObject)
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
		case CLuaSmartArray::CLASS_ID:
		{
			CLuaSmartArray* pValue = dynamic_cast<CLuaSmartArray*>(pObject);
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
LUA_EMPTY_VALUE CLuaSmartArray::LuaClone()
{
	CLuaSmartArray* pObject = CLuaSmartArray::New(m_pCurThread, NULL);
	if (pObject)
	{
		if (!pObject->GetValue().CloneFrom(GetValue()))
		{
			LogLua(_T("clone failed"));
		}
	}
	return LUA_EMPTY_VALUE();
}
bool CLuaSmartArray::LuaCloneFrom(CLuaSmartArray* pObject)
{
	if (pObject)
		return m_Data.CloneFrom(pObject->GetValue());
	return false;
}
void CLuaSmartArray::LuaClear()
{
	m_Data.Clear();
}
LUA_EMPTY_VALUE CLuaSmartArray::LuaGetMember(UINT Index)
{
	m_pCurThread->UnpackValue(m_Data.GetMember(Index), this);

	return LUA_EMPTY_VALUE();
}
LUA_EMPTY_VALUE CLuaSmartArray::LuaGetMemberObject(UINT Index)
{
	CLuaSmartValue* pObject = CLuaSmartValue::New(m_pCurThread, this);
	if (pObject)
	{
		pObject->Attach(m_Data.GetMember(Index));
	}
	return LUA_EMPTY_VALUE();
}
bool CLuaSmartArray::LuaAddMember(LUA_EMPTY_VALUE)
{
	CSmartValue Value;
	if (m_Data.PrepareMember(CSmartValue::VT_NULL, Value))
	{
		if (m_pCurThread->PackValue(Value, 2, true))
		{
			m_Data.FinishMember(Value.GetDataLen());
			return true;
		}
	}
	return false;
}

bool CLuaSmartArray::LuaAddMemberFloat(float Value)
{
	return m_Data.AddMember(Value);
}
bool CLuaSmartArray::LuaAddMemberDouble(double Value)
{
	return m_Data.AddMember(Value);
}
bool CLuaSmartArray::LuaAddMemberByte(BYTE Value)
{
	return m_Data.AddMember(Value);
}
bool CLuaSmartArray::LuaAddMemberSByte(char Value)
{
	return m_Data.AddMember(Value);
}
bool CLuaSmartArray::LuaAddMemberShort(short Value)
{
	return m_Data.AddMember(Value);
}
bool CLuaSmartArray::LuaAddMemberUShort(WORD Value)
{
	return m_Data.AddMember(Value);
}
bool CLuaSmartArray::LuaAddMemberInt(int Value)
{
	return m_Data.AddMember(Value);
}
bool CLuaSmartArray::LuaAddMemberUInt(UINT Value)
{
	return m_Data.AddMember(Value);
}
bool CLuaSmartArray::LuaAddMemberInt64(INT64 Value)
{
	return m_Data.AddMember(Value);
}
bool CLuaSmartArray::LuaAddMemberUInt64(UINT64 Value)
{
	return m_Data.AddMember(Value);
}
LUA_EMPTY_VALUE CLuaSmartArray::LuaGetFirstMemberPosition()
{
	LPVOID Pos = m_Data.GetFirstMemberPosition();
	if (Pos)
		m_pCurThread->PushValue(Pos);
	else
		m_pCurThread->PushNil();
	return LUA_EMPTY_VALUE();
}
LUA_CUSTOM_RETURN CLuaSmartArray::LuaGetNextMember(LPVOID Pos)
{
	m_pCurThread->UnpackValue(m_Data.GetNextMember(Pos), this);
	if (Pos)
		m_pCurThread->PushValue(Pos);
	else
		m_pCurThread->PushNil();
	LUA_CUSTOM_RETURN Return;
	Return.ReturnCount = 2;
	return Return;
}
LUA_CUSTOM_RETURN CLuaSmartArray::LuaGetNextMemberObject(LPVOID Pos)
{
	LUA_CUSTOM_RETURN Return;
	Return.ReturnCount = 0;
	CLuaSmartValue* pObject = CLuaSmartValue::New(m_pCurThread, this);
	if (pObject)
	{
		pObject->Attach(m_Data.GetNextMember(Pos));
		if (Pos)
			m_pCurThread->PushValue(Pos);
		else
			m_pCurThread->PushNil();
		Return.ReturnCount = 2;
	}
	return Return;
}
LUA_EMPTY_VALUE CLuaSmartArray::LuaPrepareSubStruct()
{
	CLuaSmartStruct* pObject = CLuaSmartStruct::New(m_pCurThread, this);
	if (pObject)
	{
		pObject->Attach(m_Data.PrepareSubStruct());
	}
	return LUA_EMPTY_VALUE();
}
LUA_EMPTY_VALUE CLuaSmartArray::LuaPrepareSubArray()
{
	CLuaSmartArray* pObject = CLuaSmartArray::New(m_pCurThread, this);
	if (pObject)
	{
		pObject->Attach(m_Data.PrepareArrayMember());
	}
	return LUA_EMPTY_VALUE();
}
bool CLuaSmartArray::LuaFinishMember(UINT MemberSize)
{
	return m_Data.FinishMember(MemberSize);
}
bool CLuaSmartArray::LuaAppend(CLuaSmartArray* pArray)
{
	if (pArray)
		return m_Data.Append(pArray->GetValue());
	return false;
}
bool CLuaSmartArray::LuaExpand(UINT ExpandSize)
{
	return m_Data.Expand(ExpandSize);
}
LUA_EMPTY_VALUE CLuaSmartArray::LuaGetArray()
{
	m_pCurThread->UnpackArray(m_Data, this);
	return LUA_EMPTY_VALUE();
}
bool CLuaSmartArray::LuaAddArray(LUA_EMPTY_VALUE)
{
	int ParamCount = lua_gettop(m_pCurThread->GetLuaState());
	BYTE ElementType = CSmartValue::VT_UNKNOWN;
	if (ParamCount >= 3)
	{
		if (lua_isnumber(m_pCurThread->GetLuaState(), 3))
			ElementType = (BYTE)lua_tointeger(m_pCurThread->GetLuaState(), 3);
	}
	return m_pCurThread->PackArray(m_Data, 2, ElementType);
	if (lua_istable(m_pCurThread->GetLuaState(), 2))
	{

		m_pCurThread->PushNil();
		while (lua_next(m_pCurThread->GetLuaState(), 2) != 0)
		{
			CSmartValue Value;
			if (m_Data.PrepareMember(CSmartValue::VT_NULL, Value))
			{
				if (m_pCurThread->PackValue(Value, -1, true))
				{
					m_Data.FinishMember(Value.GetDataLen());
				}
			}
			m_pCurThread->Pop(1);
		}
	}
	else
	{
		luaL_error(m_pCurThread->GetLuaState(), "param2 must table");
	}
	return false;
}
