#pragma once
class CLuaSmartStruct :
	public CLuaBaseStaticMetaClass
{
protected:
	CSmartStruct		m_Data;

	DECLARE_STATIC_META_CLASS(CLuaSmartStruct)
public:
	static const int CLASS_ID = 10004;
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

	void CloneFrom(const CSmartValue& Value)
	{
		m_Data.CloneFrom(Value);
	}

	void CloneFromWidthDataBuffer(const CLuaSmartStruct& Value, BYTE * pDataBuffer)
	{
		memcpy(pDataBuffer, Value.m_Data.GetData(), Value.m_Data.GetDataLen());
		m_Data.Attach(pDataBuffer, Value.m_Data.GetDataLen(), false);
	}

	void Create(UINT Size)
	{
		m_Data.Create(Size);
	}

	virtual const char * GetMetaClassName() const override
	{
		return _T("CLuaSmartStruct");
	}
	virtual int GetMetaClassID() const override
	{
		return CLuaSmartStruct::CLASS_ID;
	}
	virtual size_t GetMetaClassSize() const override
	{
		return sizeof(CLuaSmartStruct);
	}
	virtual bool Attach(void * pData, UINT DataLen) override
	{
		return m_Data.Attach(pData, DataLen, false);
	}
	virtual UINT GetDataLen() const override
	{
		return m_Data.GetDataLen();
	}

	virtual const BYTE * GetData() const override
	{
		return (BYTE *)m_Data.GetData();
	}
	virtual BYTE * GetData() override
	{
		return (BYTE *)m_Data.GetData();
	}

	CSmartStruct& GetValue()
	{
		return m_Data;
	}

	void Destory()
	{
		m_Data.Destory();
	}
protected:
	virtual void RegisterMemberFunctions(lua_State * pLuaState) const;
protected:
	LUA_EMPTY_VALUE LuaGetData(CLuaThread * pThreadInfo);
	LUA_EMPTY_VALUE LuaGetDataClone(CLuaThread * pThreadInfo);
	UINT LuaGetDataLen(CLuaThread * pThreadInfo);
	UINT LuaGetLength(CLuaThread * pThreadInfo);
	UINT LuaGetFreeLen(CLuaThread * pThreadInfo);
	bool LuaAttach(CLuaThread * pThreadInfo, LUA_EMPTY_VALUE);
	bool LuaCloneFrom(CLuaThread * pThreadInfo, CLuaSmartStruct& Data);
	LUA_EMPTY_VALUE LuaGetMember(CLuaThread * pThreadInfo, WORD MemberID);
	LUA_EMPTY_VALUE LuaGetMemberObject(CLuaThread * pThreadInfo, WORD MemberID);
	bool LuaAddMember(CLuaThread * pThreadInfo, WORD MemberID, LUA_EMPTY_VALUE);
	bool LuaAddMemberFloat(CLuaThread * pThreadInfo, WORD MemberID, float Value);
	bool LuaAddMemberDouble(CLuaThread * pThreadInfo, WORD MemberID, double Value);
	bool LuaAddMemberByte(CLuaThread * pThreadInfo, WORD MemberID, BYTE Value);
	bool LuaAddMemberSByte(CLuaThread * pThreadInfo, WORD MemberID, char Value);
	bool LuaAddMemberShort(CLuaThread * pThreadInfo, WORD MemberID, short Value);
	bool LuaAddMemberUShort(CLuaThread * pThreadInfo, WORD MemberID, WORD Value);
	bool LuaAddMemberInt(CLuaThread * pThreadInfo, WORD MemberID, int Value);
	bool LuaAddMemberUInt(CLuaThread * pThreadInfo, WORD MemberID, UINT Value);
	bool LuaAddMemberInt64(CLuaThread * pThreadInfo, WORD MemberID, INT64 Value);
	bool LuaAddMemberUInt64(CLuaThread * pThreadInfo, WORD MemberID, UINT64 Value);
	LUA_EMPTY_VALUE LuaGetFirstMemberPosition(CLuaThread * pThreadInfo);
	LUA_CUSTOM_RETURN LuaGetNextMember(CLuaThread * pThreadInfo, LPVOID Pos);
	LUA_CUSTOM_RETURN LuaGetNextMemberObject(CLuaThread * pThreadInfo, LPVOID Pos);
	LUA_EMPTY_VALUE LuaPrepareSubStruct(CLuaThread * pThreadInfo);
	bool LuaFinishMember(CLuaThread * pThreadInfo, WORD MemberID, UINT MemberSize);
	bool LuaAppend(CLuaThread * pThreadInfo, CLuaSmartValue& Struct);
	bool LuaIsMemberExist(CLuaThread * pThreadInfo, WORD MemberID);
};

namespace LuaWrap
{
	inline void Push(lua_State* L, const CLuaSmartStruct& value)
	{
		UINT ClassSize = (UINT)value.GetMetaClassSize();
		UINT DataSize = value.GetDataLen();
		if (value.GetData() && ClassSize && DataSize)
		{
			BYTE * pBuff = (BYTE *)lua_newuserdata(L, ClassSize + DataSize);
#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#undef new
#endif
			::new(pBuff) CLuaSmartStruct();
#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#define new NEWNEW
#endif
			((CLuaSmartStruct *)pBuff)->CloneFromWidthDataBuffer(value, pBuff + ClassSize);
			value.SetMetaClass(L);
		}
		else
		{
			lua_pushnil(L);
		}
	}
	inline void PushRef(lua_State* L, const CLuaSmartStruct& value)
	{
		UINT ClassSize = (UINT)value.GetMetaClassSize();
		if (ClassSize)
		{
			BYTE * pBuff = (BYTE *)lua_newuserdata(L, ClassSize);
#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#undef new
#endif
			::new(pBuff) CLuaSmartStruct();
#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#define new NEWNEW
#endif
			((CLuaSmartStruct *)pBuff)->Attach((void *)value.GetData(), value.GetDataLen());
			value.SetMetaClass(L);
		}
		else
		{
			lua_pushnil(L);
		}
	}
	inline bool	Match(TypeWrapper<CLuaSmartStruct>, lua_State* L, int idx)
	{
		return GetLuaObjectType(L, idx) == CLuaSmartStruct::CLASS_ID;
	}
	inline CLuaSmartStruct Get(TypeWrapper<CLuaSmartStruct>, lua_State* L, int idx)
	{
		CLuaSmartStruct Object;

		UINT Len = (UINT)lua_rawlen(L, idx);
		BYTE * pBuff = (BYTE *)lua_touserdata(L, idx);
		if (pBuff&&Len)
		{
			UINT ClassSize = (UINT)Object.GetMetaClassSize();
			Object.Attach(pBuff + ClassSize, Len - ClassSize);
		}
		return Object;
	}
	inline bool	Match(TypeWrapper<CLuaSmartStruct&>, lua_State* L, int idx)
	{
		return GetLuaObjectType(L, idx) == CLuaSmartStruct::CLASS_ID;
	}
	inline CLuaSmartStruct& Get(TypeWrapper<CLuaSmartStruct&>, lua_State* L, int idx)
	{
		BYTE * pBuff = (BYTE *)lua_touserdata(L, idx);
		return *((CLuaSmartStruct *)pBuff);
	}
}

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE CLuaSmartStruct
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(L, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"