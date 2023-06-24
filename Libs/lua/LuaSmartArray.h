#pragma once

class CLuaSmartArray :
	public CLuaBaseMetaClass
{
protected:
	CSmartArray		m_Data;

	DECLARE_META_CLASS(CLuaSmartArray, 20005);
public:
	CLuaSmartArray()
	{

	}
	CLuaSmartArray(void* pData, UINT DataLen)
	{
		Attach(pData, DataLen);
	}
	CLuaSmartArray(UINT Size)
	{
		m_Data.Create(Size);
	}
	CLuaSmartArray(const CSmartValue& Value)
	{
		m_Data = Value;
	}
	CLuaSmartArray(const CSmartArray& Value)
	{
		m_Data = Value;
	}
	CLuaSmartArray(const CLuaSmartArray& Value)
		:m_Data(Value.m_Data)
	{
	}
	virtual ~CLuaSmartArray()
	{
		Destory();
	}

	CLuaSmartArray& operator=(const CLuaSmartArray& Value)
	{
		m_Data = Value.m_Data;
		return *this;
	}

	CLuaSmartArray& operator=(const CSmartArray& Value)
	{
		m_Data = Value;
		return *this;
	}

	CLuaSmartArray& operator=(const CSmartValue& Value)
	{
		m_Data = Value;
		return *this;
	}

	void CloneFrom(const CLuaSmartArray& Value)
	{
		m_Data.CloneFrom(Value.m_Data);
	}

	void CloneFrom(const CSmartValue& Value)
	{
		m_Data.CloneFrom(Value);
	}
	void Create(UINT Size)
	{
		m_Data.Create(Size);
	}

	virtual LPCTSTR GetMetaClassName() const override
	{
		return CLuaSmartArray::CLASS_NAME;
	}
	virtual int GetMetaClassID() const override
	{
		return CLuaSmartArray::CLASS_ID;
	}
	virtual size_t GetMetaClassSize() const override
	{
		return sizeof(CLuaSmartArray);
	}
	virtual void Dump(CEasyString& Data) const override;
	bool Attach(void* pData, UINT DataLen)
	{
		return m_Data.Attach(pData, DataLen, false);
	}
	bool Attach(const CSmartArray& Data)
	{
		m_Data = Data;
		return true;
	}
	bool Attach(const CSmartValue& Data)
	{
		m_Data = Data;
		return true;
	}

	CSmartArray& GetValue()
	{
		return m_Data;
	}

	const CSmartArray& GetValue() const
	{
		return m_Data;
	}

	virtual void Destory()
	{
		m_Data.Destory();
	}




	static void ResgisterStaticFunctions(CBaseLuaVM* pLuaVM, LPCTSTR LibName);
	static void ResgisterStaticFunctions(CBaseLuaVM* pLuaVM)
	{
		ResgisterStaticFunctions(pLuaVM, CLASS_NAME);
	}
protected:
	virtual void RegisterMemberFunctions(lua_State* pLuaState) const;
	bool AddMember(lua_State* L, int Idx);
protected:
	virtual void OnGarbageCollect();
	static LUA_EMPTY_VALUE LuaNew(CLuaThread* pLuaThread, LUA_EMPTY_VALUE);
	static UINT LuaGetEmptyStructSize(CLuaThread* pLuaThread);
	static UINT LuaGetFixMemberSize(CLuaThread* pLuaThread, UINT TypeLen);
	static UINT LuaGetArrayMemberSize(CLuaThread* pLuaThread, UINT ArraySize);
	static UINT LuaGetStructMemberSize(CLuaThread* pLuaThread, UINT StructSize);
	static UINT LuaGetStringMemberSize(CLuaThread* pLuaThread, LPCTSTR Str);
	static UINT LuaGetBinaryMemberSize(CLuaThread* pLuaThread, UINT DataLen);
	static UINT LuaGetVariedMemberSize(CLuaThread* pLuaThread, LUA_EMPTY_VALUE);
	UINT LuaGetDataLen();
	UINT LuaGetLength();
	UINT LuaGetArrayLength();
	UINT LuaGetFreeLen();
	LUA_EMPTY_VALUE LuaGetData();
	BYTE LuaGetElementType();
	UINT LuaGetElementSize();
	bool LuaAttach(CLuaBaseMetaClass* pObject);
	LUA_EMPTY_VALUE LuaClone();
	bool LuaCloneFrom(CLuaSmartArray* pObject);
	void LuaClear();
	LUA_EMPTY_VALUE LuaGetMember(UINT Index);
	LUA_EMPTY_VALUE LuaGetMemberObject(UINT Index);
	bool LuaAddMember(LUA_EMPTY_VALUE);
	bool LuaAddMemberFloat(float Value);
	bool LuaAddMemberDouble(double Value);
	bool LuaAddMemberByte(BYTE Value);
	bool LuaAddMemberSByte(char Value);
	bool LuaAddMemberShort(short Value);
	bool LuaAddMemberUShort(WORD Value);
	bool LuaAddMemberInt(int Value);
	bool LuaAddMemberUInt(UINT Value);
	bool LuaAddMemberInt64(INT64 Value);
	bool LuaAddMemberUInt64(UINT64 Value);
	LUA_EMPTY_VALUE LuaGetFirstMemberPosition();
	LUA_CUSTOM_RETURN LuaGetNextMember(LPVOID Pos);
	LUA_CUSTOM_RETURN LuaGetNextMemberObject(LPVOID Pos);
	LUA_EMPTY_VALUE LuaPrepareSubStruct();
	LUA_EMPTY_VALUE LuaPrepareSubArray();
	bool LuaFinishMember(UINT MemberSize);
	bool LuaAppend(CLuaSmartArray* pArray);
	bool LuaExpand(UINT ExpandSize);
	LUA_EMPTY_VALUE LuaGetArray();
	bool LuaAddArray(BYTE ElementType, LUA_EMPTY_VALUE);
};

DECLARE_META_CLASS_MATCHER(CLuaSmartArray)

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE CLuaSmartArray*
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(pThreadInfo, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"