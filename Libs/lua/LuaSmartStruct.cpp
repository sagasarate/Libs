#include "stdafx.h"


IMPLEMENT_STATIC_META_CLASS(CLuaSmartStruct)

void CLuaSmartStruct::RegisterMemberFunctions(lua_State * pLuaState) const
{
	CLuaBaseStaticMetaClass::RegisterMemberFunctions(pLuaState);

	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "GetData", &CLuaSmartStruct::LuaGetData);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "GetDataClone", &CLuaSmartStruct::LuaGetDataClone);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "GetDataLen", &CLuaSmartStruct::LuaGetDataLen);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "GetLength", &CLuaSmartStruct::LuaGetLength);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "GetFreeLen", &CLuaSmartStruct::LuaGetFreeLen);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "CloneFrom", &CLuaSmartStruct::LuaCloneFrom);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "GetMember", &CLuaSmartStruct::LuaGetMember);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "GetMemberObject", &CLuaSmartStruct::LuaGetMemberObject);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "AddMember", &CLuaSmartStruct::LuaAddMember);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "AddMemberFloat", &CLuaSmartStruct::LuaAddMemberFloat);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "AddMemberDouble", &CLuaSmartStruct::LuaAddMemberDouble);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "AddMemberByte", &CLuaSmartStruct::LuaAddMemberByte);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "AddMemberSByte", &CLuaSmartStruct::LuaAddMemberSByte);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "AddMemberShort", &CLuaSmartStruct::LuaAddMemberShort);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "AddMemberUShort", &CLuaSmartStruct::LuaAddMemberUShort);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "AddMemberInt", &CLuaSmartStruct::LuaAddMemberInt);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "AddMemberUInt", &CLuaSmartStruct::LuaAddMemberUInt);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "AddMemberInt64", &CLuaSmartStruct::LuaAddMemberInt64);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "AddMemberUInt64", &CLuaSmartStruct::LuaAddMemberUInt64);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "GetFirstMemberPosition", &CLuaSmartStruct::LuaGetFirstMemberPosition);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "GetNextMember", &CLuaSmartStruct::LuaGetNextMember);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "GetNextMemberObject", &CLuaSmartStruct::LuaGetNextMemberObject);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "PrepareSubStruct", &CLuaSmartStruct::LuaPrepareSubStruct);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "FinishMember", &CLuaSmartStruct::LuaFinishMember);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "Append", &CLuaSmartStruct::LuaAppend);
	RegisterMetaFunction<CLuaSmartStruct>(pLuaState, "IsMemberExist", &CLuaSmartStruct::LuaIsMemberExist);
}

LUA_EMPTY_VALUE CLuaSmartStruct::LuaGetData(CLuaThread * pThreadInfo)
{
	if (m_Data.GetData() && m_Data.GetDataLen())
		LuaWrap::PushRef(pThreadInfo->GetLuaState(), CLuaByteArray(m_Data.GetData(), m_Data.GetDataLen()));
	else
		pThreadInfo->PushNil();
	return LUA_EMPTY_VALUE();
}
LUA_EMPTY_VALUE CLuaSmartStruct::LuaGetDataClone(CLuaThread * pThreadInfo)
{
	if (m_Data.GetData() && m_Data.GetDataLen())
		LuaWrap::Push(pThreadInfo->GetLuaState(), CLuaByteArray(m_Data.GetData(), m_Data.GetDataLen()));
	else
		pThreadInfo->PushNil();
	return LUA_EMPTY_VALUE();
}
UINT CLuaSmartStruct::LuaGetDataLen(CLuaThread * pThreadInfo)
{
	return m_Data.GetDataLen();
}
UINT CLuaSmartStruct::LuaGetLength(CLuaThread * pThreadInfo)
{
	return m_Data.GetLength();
}
UINT CLuaSmartStruct::LuaGetFreeLen(CLuaThread * pThreadInfo)
{
	return m_Data.GetFreeLen();
}
bool CLuaSmartStruct::LuaAttach(CLuaThread * pThreadInfo, LUA_EMPTY_VALUE)
{
	int Type = GetLuaObjectType(pThreadInfo->GetLuaState(), 2);
	switch (Type)
	{
	case CLuaByteArray::CLASS_ID:
	case CLuaSmartValue::CLASS_ID:
	case CLuaSmartStruct::CLASS_ID:	
		{
			CLuaBaseStaticMetaClass * pObject = dynamic_cast<CLuaBaseStaticMetaClass *>((CLuaBaseStaticMetaClass *)lua_touserdata(pThreadInfo->GetLuaState(), 2));
			if (pObject)
			{
				return m_Data.Attach(pObject->GetData(), pObject->GetDataLen(), false);
			}
		}
		break;
	}
	return false;
}
bool CLuaSmartStruct::LuaCloneFrom(CLuaThread * pThreadInfo, CLuaSmartStruct& Data)
{
	return m_Data.CloneFrom(Data.GetValue());
}
LUA_EMPTY_VALUE CLuaSmartStruct::LuaGetMember(CLuaThread * pThreadInfo, WORD MemberID)
{
	CLuaSmartValue::PushValueToLua(pThreadInfo->GetLuaState(), m_Data.GetMember(MemberID));	
	return LUA_EMPTY_VALUE();
}
LUA_EMPTY_VALUE CLuaSmartStruct::LuaGetMemberObject(CLuaThread * pThreadInfo, WORD MemberID)
{
	LuaWrap::PushRef(pThreadInfo->GetLuaState(), CLuaSmartValue(m_Data.GetMember(MemberID)));
	return LUA_EMPTY_VALUE();
}
bool CLuaSmartStruct::LuaAddMember(CLuaThread * pThreadInfo, WORD MemberID, LUA_EMPTY_VALUE)
{
	int Type = GetLuaObjectType(pThreadInfo->GetLuaState(), 3);
	switch (Type)
	{
	case LUA_TNIL:
		m_Data.AddMemberNull(MemberID);
		return true;
	case LUA_TBOOLEAN:
		m_Data.AddMember(MemberID,lua_toboolean(pThreadInfo->GetLuaState(), 3) != 0);
		return true;
	case LUA_TNUMBER:
		m_Data.AddMember(MemberID, lua_tonumber(pThreadInfo->GetLuaState(), 3));
		return true;
	case LUA_TSTRING:
		m_Data.AddMember(MemberID, lua_tostring(pThreadInfo->GetLuaState(), 3));
		return true;
	case LUA_TTABLE:
		return false;
	case LUA_TINTEGER:
		{
			UINT64 Value = lua_tointeger(pThreadInfo->GetLuaState(), 3);
			if(Value<=0xFF)
				m_Data.AddMember(MemberID, (BYTE)Value);
			else if(Value <= 0xFFFF)
				m_Data.AddMember(MemberID, (WORD)Value);
			else if (Value <= 0xFFFFFFFF)
				m_Data.AddMember(MemberID, (UINT)Value);
			else
				m_Data.AddMember(MemberID,Value);
		}
		return true;
	case CLuaByteArray::CLASS_ID:
		{
			CLuaByteArray * pObject = dynamic_cast<CLuaByteArray *>((CLuaByteArray *)lua_touserdata(pThreadInfo->GetLuaState(), 3));
			if (pObject)
			{
				m_Data.AddMember(MemberID, (LPCSTR)pObject->GetData(), pObject->GetDataLen());
			}
		}
		break;
	case CLuaSmartValue::CLASS_ID:
		{
			CLuaSmartValue * pObject = dynamic_cast<CLuaSmartValue *>((CLuaSmartValue *)lua_touserdata(pThreadInfo->GetLuaState(), 3));
			if (pObject)
			{
				m_Data.AddMember(MemberID, pObject->GetValue());
				return true;
			}
		}
		break;
	case CLuaSmartStruct::CLASS_ID:
		{
			CLuaSmartStruct * pObject = dynamic_cast<CLuaSmartStruct *>((CLuaSmartStruct *)lua_touserdata(pThreadInfo->GetLuaState(), 3));
			if (pObject)
			{
				m_Data.AddMember(MemberID, pObject->GetValue());
				return true;
			}
		}
		break;
	}
	return false;
}

bool CLuaSmartStruct::LuaAddMemberFloat(CLuaThread * pThreadInfo, WORD MemberID, float Value)
{
	return m_Data.AddMember(MemberID, Value);
}
bool CLuaSmartStruct::LuaAddMemberDouble(CLuaThread * pThreadInfo, WORD MemberID, double Value)
{
	return m_Data.AddMember(MemberID, Value);
}
bool CLuaSmartStruct::LuaAddMemberByte(CLuaThread * pThreadInfo, WORD MemberID, BYTE Value)
{
	return m_Data.AddMember(MemberID, Value);
}
bool CLuaSmartStruct::LuaAddMemberSByte(CLuaThread * pThreadInfo, WORD MemberID, char Value)
{
	return m_Data.AddMember(MemberID, Value);
}
bool CLuaSmartStruct::LuaAddMemberShort(CLuaThread * pThreadInfo, WORD MemberID, short Value)
{
	return m_Data.AddMember(MemberID, Value);
}
bool CLuaSmartStruct::LuaAddMemberUShort(CLuaThread * pThreadInfo, WORD MemberID, WORD Value)
{
	return m_Data.AddMember(MemberID, Value);
}
bool CLuaSmartStruct::LuaAddMemberInt(CLuaThread * pThreadInfo, WORD MemberID, int Value)
{
	return m_Data.AddMember(MemberID, Value);
}
bool CLuaSmartStruct::LuaAddMemberUInt(CLuaThread * pThreadInfo, WORD MemberID, UINT Value)
{
	return m_Data.AddMember(MemberID, Value);
}
bool CLuaSmartStruct::LuaAddMemberInt64(CLuaThread * pThreadInfo, WORD MemberID, INT64 Value)
{
	return m_Data.AddMember(MemberID, Value);
}
bool CLuaSmartStruct::LuaAddMemberUInt64(CLuaThread * pThreadInfo, WORD MemberID, UINT64 Value)
{
	return m_Data.AddMember(MemberID, Value);
}
LUA_EMPTY_VALUE CLuaSmartStruct::LuaGetFirstMemberPosition(CLuaThread * pThreadInfo)
{
	LPVOID Pos = m_Data.GetFirstMemberPosition();
	if (Pos)
		pThreadInfo->PushValue(Pos);
	else
		pThreadInfo->PushNil();
	return LUA_EMPTY_VALUE();
}
LUA_CUSTOM_RETURN CLuaSmartStruct::LuaGetNextMember(CLuaThread * pThreadInfo, LPVOID Pos)
{
	WORD MemberID;
	CLuaSmartValue::PushValueToLua(pThreadInfo->GetLuaState(), m_Data.GetNextMember(Pos, MemberID));
	pThreadInfo->PushInteger(MemberID);
	if (Pos)
		pThreadInfo->PushValue(Pos);
	else
		pThreadInfo->PushNil();
	LUA_CUSTOM_RETURN Return;
	Return.ReturnCount = 3;
	return Return;
}
LUA_CUSTOM_RETURN CLuaSmartStruct::LuaGetNextMemberObject(CLuaThread * pThreadInfo, LPVOID Pos)
{
	WORD MemberID;
	LuaWrap::PushRef(pThreadInfo->GetLuaState(), CLuaSmartValue(m_Data.GetNextMember(Pos, MemberID)));
	pThreadInfo->PushInteger(MemberID);
	if (Pos)
		pThreadInfo->PushValue(Pos);
	else
		pThreadInfo->PushNil();
	LUA_CUSTOM_RETURN Return;
	Return.ReturnCount = 3;
	return Return;
}
LUA_EMPTY_VALUE CLuaSmartStruct::LuaPrepareSubStruct(CLuaThread * pThreadInfo)
{
	CSmartStruct Value = m_Data.PrepareSubStruct();
	LuaWrap::PushRef(pThreadInfo->GetLuaState(), Value);
	return LUA_EMPTY_VALUE();
}
bool CLuaSmartStruct::LuaFinishMember(CLuaThread * pThreadInfo, WORD MemberID, UINT MemberSize)
{
	return m_Data.FinishMember(MemberID, MemberSize);
}
bool CLuaSmartStruct::LuaAppend(CLuaThread * pThreadInfo, CLuaSmartValue& Struct)
{
	return m_Data.Append(Struct.GetValue());
}
bool CLuaSmartStruct::LuaIsMemberExist(CLuaThread * pThreadInfo, WORD MemberID)
{
	return m_Data.IsMemberExist(MemberID);
}