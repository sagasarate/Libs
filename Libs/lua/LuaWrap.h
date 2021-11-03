#pragma once

#include "LuaWrapBase.h"


template<typename T>
void CLuaThread::PushValue(T Value)
{
	LuaWrap::Push(m_pLuaThread, Value);
}

template<typename T>
T CLuaThread::PopValue()
{
	if (LuaWrap::Match(TypeWrapper<T>(), m_pLuaThread, -1))
	{
		T Result = LuaWrap::Get(TypeWrapper<T>(), m_pLuaThread, -1);
		lua_pop(m_pLuaThread, 1);
		return Result;
	}
	return T();
}
template<typename T>
T CLuaThread::GetValue(int Index)
{
	if (LuaWrap::Match(TypeWrapper<T>(), m_pLuaThread, Index))
	{
		T Result = LuaWrap::Get(TypeWrapper<T>(), m_pLuaThread, Index);
		return Result;
	}
	return T();
}

//#include "../../Libs/rapidjson/document.h"
//#include "../../Libs/rapidjson/stringbuffer.h"
//#include "../../Libs/rapidjson/prettywriter.h"

#include "LuaCallWraps.h"

#include "LuaBaseMetaClass.h"
#include "LuaBaseStaticMetaClass.h"

#include "LuaGrid.h"
#include "LuaByteArray.h"
#include "LuaSmartValue.h"
#include "LuaSmartStruct.h"
//#include "LuaJsonValue.h"

#include "BaseLuaLib.h"
#include "LuaLibCommon.h"
#include "LuaLibSmartStruct.h"

#include "LuaScriptPool.h"






