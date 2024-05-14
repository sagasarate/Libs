#pragma once
class CLuaSmartStruct :
	public CLuaBaseMetaClass
{
protected:
	CSmartStruct		m_Data;

	DECLARE_META_CLASS(CLuaSmartStruct, 20004);
public:
	CLuaSmartStruct()
	{

	}
	CLuaSmartStruct(void * pData, UINT DataLen)
	{
		Attach(pData, DataLen);
	}
	CLuaSmartStruct(UINT Size)
	{
		m_Data.Create(Size);
	}
	CLuaSmartStruct(const CSmartValue& Value)
	{
		m_Data = Value;
	}
	CLuaSmartStruct(const CSmartStruct& Value)
	{
		m_Data = Value;
	}
	CLuaSmartStruct(const CLuaSmartStruct& Value)
		:m_Data(Value.m_Data)
	{
	}
	virtual ~CLuaSmartStruct()
	{
		Destory();
	}

	CLuaSmartStruct& operator=(const CLuaSmartStruct& Value)
	{
		m_Data = Value.m_Data;
		return *this;
	}

	CLuaSmartStruct& operator=(const CSmartStruct& Value)
	{
		m_Data = Value;
		return *this;
	}

	CLuaSmartStruct& operator=(const CSmartValue& Value)
	{
		m_Data = Value;
		return *this;
	}

	void CloneFrom(const CLuaSmartStruct& Value)
	{
		m_Data.CloneFrom(Value.m_Data);
	}
	void CloneFrom(const CSmartStruct& Value)
	{
		m_Data.CloneFrom(Value);
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
		return CLuaSmartStruct::CLASS_NAME;
	}
	virtual int GetMetaClassID() const override
	{
		return CLuaSmartStruct::CLASS_ID;
	}
	virtual size_t GetMetaClassSize() const override
	{
		return sizeof(CLuaSmartStruct);
	}
	virtual void Dump(CEasyString& Data) const override;
	bool Attach(void * pData, UINT DataLen)
	{
		return m_Data.Attach(pData, DataLen, false);
	}
	bool Attach(const CSmartStruct& Data)
	{
		m_Data = Data;
		return true;
	}
	bool Attach(const CSmartValue& Data)
	{
		m_Data = Data;
		return true;
	}

	CSmartStruct& GetValue()
	{
		return m_Data;
	}

	const CSmartStruct& GetValue() const
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
	virtual void RegisterMemberFunctions(lua_State * pLuaState) const;
protected:
	virtual void OnGarbageCollect();
	static LUA_EMPTY_VALUE LuaNew(CLuaThread* pLuaThread, LUA_EMPTY_VALUE);
	static UINT LuaGetEmptyStructSize(CLuaThread* pLuaThread);
	static UINT LuaGetFixMemberSize(CLuaThread* pLuaThread, UINT TypeLen);
	static UINT LuaGetArrayMemberSize(CLuaThread* pLuaThread, UINT ArraySize);
	static UINT LuaGetStructMemberSize(CLuaThread* pLuaThread, UINT StructSize);
	static UINT LuaGetStringMemberSize(CLuaThread* pLuaThread, CEasyString Str);
	static UINT LuaGetBinaryMemberSize(CLuaThread* pLuaThread, UINT DataLen);
	static UINT LuaGetVariedMemberSize(CLuaThread* pLuaThread, LUA_EMPTY_VALUE);
	UINT LuaGetDataLen();
	UINT LuaGetLength();
	UINT LuaGetBufferLen();
	UINT LuaGetFreeLen();
	LUA_EMPTY_VALUE LuaGetData();	
	bool LuaAttach(CLuaBaseMetaClass* pObject);
	LUA_EMPTY_VALUE LuaClone();
	bool LuaCloneFrom(CLuaSmartStruct* pObject);
	void LuaClear();
	LUA_EMPTY_VALUE LuaGetMember(WORD MemberID);
	LUA_EMPTY_VALUE LuaGetMemberObject(WORD MemberID);
	bool LuaAddMember(WORD MemberID, LUA_EMPTY_VALUE);
	bool LuaAddMemberBoolean(WORD MemberID, bool Value);
	bool LuaAddMemberFloat(WORD MemberID, float Value);
	bool LuaAddMemberDouble(WORD MemberID, double Value);
	bool LuaAddMemberByte(WORD MemberID, BYTE Value);
	bool LuaAddMemberSByte(WORD MemberID, char Value);
	bool LuaAddMemberShort(WORD MemberID, short Value);
	bool LuaAddMemberUShort(WORD MemberID, WORD Value);
	bool LuaAddMemberInt(WORD MemberID, int Value);
	bool LuaAddMemberUInt(WORD MemberID, UINT Value);
	bool LuaAddMemberInt64(WORD MemberID, INT64 Value);
	bool LuaAddMemberUInt64(WORD MemberID, UINT64 Value);
	LUA_EMPTY_VALUE LuaGetFirstMemberPosition();
	LUA_CUSTOM_RETURN LuaGetNextMember(LPVOID Pos);
	LUA_CUSTOM_RETURN LuaGetNextMemberObject(LPVOID Pos);
	LUA_EMPTY_VALUE LuaPrepareSubStruct();
	LUA_EMPTY_VALUE LuaPrepareSubArray();
	bool LuaFinishMember(WORD MemberID, UINT MemberSize);
	bool LuaAppend(CLuaSmartValue* pStruct);
	bool LuaExpand(UINT ExpandSize);
	bool LuaIsMemberExist(WORD MemberID);
};


DECLARE_META_CLASS_MATCHER(CLuaSmartStruct)

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE CLuaSmartStruct*
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(pThreadInfo, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"