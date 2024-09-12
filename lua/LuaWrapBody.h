#pragma once

#include "LuaCallWraps.h"

inline void CLuaThread::PushString(LPCSTR Value)
{
	LuaWrap::Push(this, Value);
}
inline void CLuaThread::PushString(LPCWSTR Value)
{
	LuaWrap::Push(this, Value);
}
template<typename T>
inline void CLuaThread::PushValue(T Value)
{
	LuaWrap::Push(this, Value);
}

template<typename T>
inline T CLuaThread::PopValue()
{
	if (LuaWrap::Match(TypeWrapper<T>(), m_pLuaState, -1))
	{
		T Result = LuaWrap::Get(TypeWrapper<T>(), m_pLuaState, -1);
		lua_pop(m_pLuaState, 1);
		return Result;
	}
	return T();
}
template<typename T>
inline T CLuaThread::GetValue(int Index)
{
	if (LuaWrap::Match(TypeWrapper<T>(), m_pLuaState, Index))
	{
		T Result = LuaWrap::Get(TypeWrapper<T>(), m_pLuaState, Index);
		return Result;
	}
	return T();
}

template<typename T>
inline bool CLuaThread::PushArray(const CEasyArray<T>& ArrayList, UINT StartIndex, UINT PushCount)
{
	if (m_pLuaState == NULL)
	{
		return false;
	}
	lua_newtable(m_pLuaState);
	if (StartIndex < ArrayList.GetCount())
	{
		if (PushCount + StartIndex > (UINT)ArrayList.GetCount())
			PushCount = (UINT)ArrayList.GetCount() - StartIndex;
		if (PushCount)
		{
			for (UINT i = StartIndex; i < StartIndex + PushCount; i++)
			{
				lua_pushinteger(m_pLuaState, i + 1);
				PushValue(ArrayList[i]);
				lua_settable(m_pLuaState, -3);
			}	
		}
	}	
	return true;
}

template<typename T>
inline bool CLuaThread::PushArray(const T* pArray, UINT ArrayLen, UINT StartIndex, UINT PushCount)
{
	if (m_pLuaState == NULL)
	{
		return false;
	}

	if (pArray)
	{
		lua_newtable(m_pLuaState);
		if (StartIndex < ArrayLen)
		{
			if (PushCount + StartIndex > ArrayLen)
				PushCount = ArrayLen - StartIndex;
			if (PushCount)
			{
				
				for (UINT i = StartIndex; i < StartIndex + PushCount; i++)
				{
					lua_pushinteger(m_pLuaState, i + 1);
					PushValue(pArray[i]);
					lua_settable(m_pLuaState, -3);
				}
			}
		}
	}
	else
	{
		lua_pushnil(m_pLuaState);
	}
	return true;
}

template<typename T>
inline T CLuaBaseMetaClass::GetMember(LPCTSTR MemberName)  throw(LUA_EXCEPTION)
{
	try 
	{
		if (m_pRegisterVM)
		{
			CLuaThread* pLuaThread = m_pRegisterVM->GetMainThread();
			PushToLua(pLuaThread);
			if (LUA_SCRIPT_CODE_PAGE == CEasyString::SYSTEM_STRING_CODE_PAGE)
			{
				//内码一致，直接压栈
				pLuaThread->PushString(MemberName);
			}
			else
			{
				//内码不同，转换后压栈
				CEasyStringA Name;
				SystemStrToLuaStr(MemberName, Name);
				pLuaThread->PushString(Name);
			}
			lua_gettable(pLuaThread->GetLuaState(), -2);
			T Value = pLuaThread->GetValue<T>(-1);
			pLuaThread->Pop(2);
			return Value;
		}
		else
		{
			LogLua("对象还未注册进Lua");
			return T();
		}
	}
	catch (LUA_EXCEPTION& e)
	{
		return T();
	}	
}

template<typename T>
inline void CLuaBaseMetaClass::SetMember(LPCTSTR MemberName, const T& Value)  throw(LUA_EXCEPTION)
{
	try
	{
		if (m_pRegisterVM)
		{
			CLuaThread* pLuaThread = m_pRegisterVM->GetMainThread();
			PushToLua(pLuaThread);
			if (LUA_SCRIPT_CODE_PAGE == CEasyString::SYSTEM_STRING_CODE_PAGE)
			{
				//内码一致，直接压栈
				pLuaThread->PushString(MemberName);
			}
			else
			{
				//内码不同，转换后压栈
				CEasyStringA Name;
				SystemStrToLuaStr(MemberName, Name);
				pLuaThread->PushString(Name);
			}
			pLuaThread->PushValue(Value);
			lua_settable(pLuaThread->GetLuaState(), -3);
			pLuaThread->Pop(1);
		}
		else
		{
			LogLua("对象还未注册进Lua");
		}
	}
	catch (LUA_EXCEPTION& e)
	{

	}
}

template <typename Callee, typename Func>
inline void CLuaBaseMetaClass::RegisterMetaFunction(lua_State* pLuaState, LPCTSTR szFuncName, Func func) const
{
	RegisterMetaCFun(pLuaState, szFuncName, LuaWrap::DirectCallMetaClassMemberDispatcherHelper<Callee, Func, 1>::DirectCallMemberDispatcher, &func, sizeof(func));
}

template <typename Callee, typename Func>
inline void CLuaBaseMetaClass::RegisterMetaPropertyGet(lua_State* pLuaState, LPCTSTR szPropertyName, Func func) const
{
	RegisterMetaProperty(pLuaState, szPropertyName, true, LuaWrap::DirectCallMetaClassMemberDispatcherHelper<Callee, Func, 1>::DirectCallMemberDispatcher, &func, sizeof(func));
}

template <typename Callee, typename Func>
inline void CLuaBaseMetaClass::RegisterMetaPropertySet(lua_State* pLuaState, LPCTSTR szPropertyName, Func func) const
{
	RegisterMetaProperty(pLuaState, szPropertyName, false, LuaWrap::DirectCallMetaClassMemberDispatcherHelper<Callee, Func, 1>::DirectCallMemberDispatcher, &func, sizeof(func));
}

template <typename Func>
inline void CLuaBaseMetaClass::RegisterMetaStaticFunction(lua_State* pLuaState, LPCTSTR szFuncName, Func func) const
{
	RegisterMetaCFun(pLuaState, szFuncName, LuaWrap::DirectCallFunctionDispatchHelper<Func, 1>::DirectCallFunctionDispatcher, &func, sizeof(func));
}

#include "LuaLoader.h"
#include "BaseLuaVM.h"
#include "LuaScript.h"
#include "LuaVM.h"

#include "BaseLuaLib.h"
#include "LuaLibCommon.h"

#include "LuaScriptPool.h"
