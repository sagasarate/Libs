#pragma once

inline bool CheckLuaObject(lua_State* L, int idx, LPCTSTR ClassName)
{
#ifndef UNICODE
	if (LUA_SCRIPT_CODE_PAGE != CEasyString::SYSTEM_STRING_CODE_PAGE)
	{

		return luaL_testudata(L, idx, ClassName) != NULL;
	}
	else
#endif
	{
		CEasyStringA TypeName;
		SystemStrToLuaStr(ClassName, TypeName);
		return luaL_testudata(L, idx, TypeName) != NULL;
	}
}

#define DECLARE_META_CLASS(ClassName,ClassID) \
public:\
	static const int CLASS_ID = ClassID;\
	static TCHAR* CLASS_NAME;\
	static ClassName* New(CLuaThread* pLuaThread, CLuaBaseMetaClass* pBindObj);

#define DECLARE_META_CLASS_WITHOUT_NEW(ClassName,ClassID) \
public:\
	static const int CLASS_ID = ClassID;\
	static TCHAR* CLASS_NAME;


#define IMPLEMENT_META_CLASS(ClassName, LuaClassName) \
TCHAR* ClassName::CLASS_NAME = LuaClassName;\
ClassName* ClassName::New(CLuaThread* pLuaThread, CLuaBaseMetaClass* pBindObj)\
{\
	CBaseLuaVM* pVM = pLuaThread->GetLuaVM();\
	UINT BuffSize = sizeof(ClassName*) + sizeof(ClassName);\
	BYTE* pBuffer = (BYTE*)lua_newuserdatauv(pVM->GetLuaState(), BuffSize, 1);\
	ClassName* pObject = (ClassName*)(pBuffer + sizeof(ClassName*));\
	::new(pObject) ClassName();\
	pObject->m_IsInLuaHeap = true;\
	memcpy(pBuffer, &pObject, sizeof(ClassName*));\
	pObject->SetMetaClass(pVM->GetLuaState());\
	pObject->RegisterLuaObject(pVM, lua_gettop(pVM->GetLuaState()));\
	if(pVM->GetLuaState() != pLuaThread->GetLuaState())\
	{\
		lua_pop(pVM->GetLuaState(), 1);\
		pObject->PushToLua(pLuaThread);\
	}\
	if (pBindObj && pBindObj->IsInLuaHeap())\
	{\
		if (pBindObj->PushToLua(pLuaThread))\
		{\
			pObject->AddBindObject(pLuaThread->GetLuaState(), -2);\
		}\
		else\
		{\
			lua_pop(pLuaThread->GetLuaState(), 1);\
		}\
	}\
	return pObject;\
}

#define IMPLEMENT_META_CLASS_WITHOUT_NEW(ClassName, LuaClassName) \
TCHAR* ClassName::CLASS_NAME = LuaClassName;

#define DECLARE_META_CLASS_MATCHER(ClassName) \
namespace LuaWrap\
{\
	inline void Push(CLuaThread* pLuaThread, ClassName* pObject)\
	{\
		if (pObject)\
			pObject->PushToLua(pLuaThread);\
		else\
			lua_pushnil(pLuaThread->GetLuaState());\
	}\
	inline bool	Match(TypeWrapper<ClassName*>, lua_State* L, int idx)\
	{\
		int Type = GetLuaObjectType(L, idx);\
		if (Type == LUA_TNIL)\
		{\
			return true;\
		}\
		else if (Type >= CLuaBaseMetaClass::CLASS_ID)\
		{\
			return IsKindOfMetaClass<ClassName>(L, idx);\
		}\
		return false;\
	}\
	inline ClassName* Get(TypeWrapper<ClassName*>, lua_State* L, int idx)\
	{\
		if (GetLuaObjectType(L, idx) >= CLuaBaseMetaClass::CLASS_ID)\
		{\
			ClassName* pObject = dynamic_cast<ClassName*>(*((CLuaBaseMetaClass**)lua_touserdata(L, idx)));\
			return pObject;\
		}\
		return NULL;\
	}\
}


class CLuaBaseMetaClass :
	public CBaseScriptHost
{	
public:
	enum OBJECT_MERGE_TYPE
	{
		OBJECT_MERGE_TYPE_OVERWRITE_ALL,
		OBJECT_MERGE_TYPE_OVERWRITE_FUNCTION,
		OBJECT_MERGE_TYPE_OVERWRITE_NONE,
	};
protected:
	bool			m_IsInLuaHeap;
	CBaseLuaVM*		m_pRegisterVM;
	CLuaThread*		m_pCurThread;

	DECLARE_META_CLASS_WITHOUT_NEW(CLuaBaseMetaClass, 20000);
public:

	CLuaBaseMetaClass();	
	virtual ~CLuaBaseMetaClass();

	void SetMetaClass(lua_State* pLuaState);
	virtual LPCTSTR GetMetaClassName() const = 0;
	virtual int GetMetaClassID() const = 0;
	virtual size_t GetMetaClassSize() const = 0;
	virtual void Dump(CEasyString& Data) const;
	//void CheckAPI(lua_State * pLuaState, CEasyArray<CEasyStringA>& Functions) const;
	bool IsInLuaHeap() const
	{
		return m_IsInLuaHeap;
	}
	void SetCurLuaThread(CLuaThread* pLuaThread)
	{
		m_pCurThread = pLuaThread;
	}
	bool RegisterLuaObject(CBaseLuaVM* pVM, int ObjIndex);
	bool UnregisterLuaObject();
	bool PushToLua(CLuaThread* pLuaThread);
	//static CLuaBaseMetaClass* New(lua_State* pLuaState);
	bool MergeLuaObject(CLuaThread* pLuaThread, int Index, OBJECT_MERGE_TYPE MergeType);//将一个Lua对象合并到自身，Index不能是伪索引
	bool PrepareCall(CLuaThread* pLuaThread, LPCTSTR szMemberName) throw(LUA_EXCEPTION);
	CLuaThread* PrepareCall(LPCTSTR szMemberName);
	template<typename T>
	T GetMember(LPCTSTR MemberName) throw(LUA_EXCEPTION);
	template<typename T>
	void SetMember(LPCTSTR MemberName, const T& Value) throw(LUA_EXCEPTION);
protected:
	void InitClassMember(lua_State* L);
	void RegisterMetaClass(lua_State * pLuaState) const;
	virtual void RegisterMemberFunctions(lua_State * pLuaState) const;
	virtual void OnGarbageCollect() = 0;
	virtual bool GetProperty(lua_State* L, LPCTSTR szPropertyName);
	virtual bool SetProperty(lua_State* L, LPCTSTR szPropertyName);
	static int DoGarbageCollect(lua_State* L);
	static int DoGetMember(lua_State* L);
	static int DoSetMember(lua_State* L);
	void AddBindObject(lua_State* L, int Index);
	static bool IsProperty(lua_State* L);

	template <typename Callee, typename Func>
	void RegisterMetaFunction(lua_State* pLuaState, LPCTSTR szFuncName, Func func) const;

	template <typename Callee, typename Func>
	void RegisterMetaPropertyGet(lua_State* pLuaState, LPCTSTR szPropertyName, Func func) const;

	template <typename Callee, typename Func>
	void RegisterMetaPropertySet(lua_State* pLuaState, LPCTSTR szPropertyName, Func func) const;

	template <typename Func>
	void RegisterMetaStaticFunction(lua_State* pLuaState, LPCTSTR szFuncName, Func func) const;

	void RegisterMetaCFun(lua_State * pLuaState, LPCTSTR szFuncName, lua_CFunction function, void* func, unsigned int sizeofFunc) const;
	void RegisterMetaProperty(lua_State* pLuaState, LPCTSTR szPropertyName, bool IsGet, lua_CFunction function, void* func, unsigned int sizeofFunc) const;

protected:

	LPCTSTR LuaGetClassName();
	bool LuaMergeObject(LUA_EMPTY_VALUE Param1);
	bool LuaMemberExist(LPCSTR MemberName);
};

//typedef CLuaBaseMetaClass * LPLUABASEMETACLASS;

namespace LuaWrap
{

	inline bool	Match(TypeWrapper<CLuaBaseMetaClass *>, lua_State* L, int idx)
	{
		int Type = GetLuaObjectType(L, idx);
		if (Type == LUA_TNIL)
		{
			return true;
		}
		else if (Type >= CLuaBaseMetaClass::CLASS_ID)
		{
			//LogLua(_T("Match %d as CLuaBaseMetaClass"), Type);
			return true;
		}
		return false;
	}
	inline CLuaBaseMetaClass * Get(TypeWrapper<CLuaBaseMetaClass *>, lua_State* L, int idx)
	{
		if (GetLuaObjectType(L, idx) >= CLuaBaseMetaClass::CLASS_ID)
		{
			CLuaBaseMetaClass * pObject = dynamic_cast<CLuaBaseMetaClass *>(*((CLuaBaseMetaClass **)lua_touserdata(L, idx)));

			return pObject;
		}
		return NULL;
	}
}

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION



#define LUA_WRAP_CALL_RETURN_TYPE CLuaBaseMetaClass *
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(pThreadInfo, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"
