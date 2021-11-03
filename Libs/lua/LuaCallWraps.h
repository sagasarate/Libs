#pragma once

class CLuaBaseMetaClass;
namespace LuaWrap
{

	template<class RT>
	struct ReturnSpecialization
	{
		static int Call(RT(*func)(CLuaThread *), lua_State* L, int /*index*/, CLuaThread * pThreadInfo)
		{
			return 0;
		}


		template <typename P1>
		static int Call(RT(*func)(CLuaThread *, P1), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			return 0;
		}


		template <typename P1, typename P2>
		static int Call(RT(*func)(CLuaThread *, P1, P2), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			return 0;
		}


		template <typename P1, typename P2, typename P3>
		static int Call(RT(*func)(CLuaThread *, P1, P2, P3), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			return 0;
		}


		template <typename P1, typename P2, typename P3, typename P4>
		static int Call(RT(*func)(CLuaThread *, P1, P2, P3, P4), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			return 0;
		}


		template <typename P1, typename P2, typename P3, typename P4,
			typename P5>
			static int Call(RT(*func)(CLuaThread *, P1, P2, P3, P4, P5), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			return 0;
		}


		template <typename P1, typename P2, typename P3, typename P4,
			typename P5, typename P6>
			static int Call(RT(*func)(CLuaThread *, P1, P2, P3, P4, P5, P6), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			return 0;
		}


		template <typename P1, typename P2, typename P3, typename P4,
			typename P5, typename P6, typename P7>
			static int Call(RT(*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			return 0;
		}

		template <typename P1, typename P2, typename P3, typename P4,
			typename P5, typename P6, typename P7, typename P8>
			static int Call(RT(*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7, P8), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			return 0;
		}

		template <typename P1, typename P2, typename P3, typename P4,
			typename P5, typename P6, typename P7, typename P8, typename P9>
			static int Call(RT(*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7, P8, P9), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			return 0;
		}

		template <typename P1, typename P2, typename P3, typename P4,
			typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
			static int Call(RT(*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			return 0;
		}

		template <typename P1, typename P2, typename P3, typename P4,
			typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11>
			static int Call(RT(*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			return 0;
		}

		//This Call Wrap
		//////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////
		template <typename Callee>
		static int Call(Callee& callee, RT(Callee::*func)(CLuaThread *), lua_State* L, int /*index*/, CLuaThread * pThreadInfo)
		{
			return 0;
		}


		template <typename Callee, typename P1>
		static int Call(Callee& callee, RT(Callee::*func)(CLuaThread *, P1), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			return 0;
		}


		template <typename Callee, typename P1, typename P2>
		static int Call(Callee& callee, RT(Callee::*func)(CLuaThread *, P1, P2), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3>
		static int Call(Callee& callee, RT(Callee::*func)(CLuaThread *, P1, P2, P3), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4>
			static int Call(Callee& callee, RT(Callee::*func)(CLuaThread *, P1, P2, P3, P4), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5>
			static int Call(Callee& callee, RT(Callee::*func)(CLuaThread *, P1, P2, P3, P4, P5), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6>
			static int Call(Callee& callee, RT(Callee::*func)(CLuaThread *, P1, P2, P3, P4, P5, P6), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7>
			static int Call(Callee& callee, RT(Callee::*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8>
			static int Call(Callee& callee, RT(Callee::*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7, P8), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
			static int Call(Callee& callee, RT(Callee::*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7, P8, P9), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
			static int Call(Callee& callee, RT(Callee::*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11>
			static int Call(Callee& callee, RT(Callee::*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			return 0;
		}
	};



	template <typename RT>
	int Call(RT(*func)(CLuaThread *), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(func, L, index, pThreadInfo);
	}


	template <typename RT, typename P1>
	int Call(RT(*func)(CLuaThread *, P1), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(func, L, index, pThreadInfo);
	}


	template <typename RT, typename P1, typename P2>
	int Call(RT(*func)(CLuaThread *, P1, P2), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(func, L, index, pThreadInfo);
	}


	template <typename RT, typename P1, typename P2, typename P3>
	int Call(RT(*func)(CLuaThread *, P1, P2, P3), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(func, L, index, pThreadInfo);
	}


	template <typename RT, typename P1, typename P2, typename P3, typename P4>
	int Call(RT(*func)(CLuaThread *, P1, P2, P3, P4), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(func, L, index, pThreadInfo);
	}


	template <typename RT, typename P1, typename P2, typename P3, typename P4,
		typename P5>
		int Call(RT(*func)(CLuaThread *, P1, P2, P3, P4, P5), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(func, L, index, pThreadInfo);
	}


	template <typename RT, typename P1, typename P2, typename P3, typename P4,
		typename P5, typename P6>
		int Call(RT(*func)(CLuaThread *, P1, P2, P3, P4, P5, P6), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(func, L, index, pThreadInfo);
	}


	template <typename RT, typename P1, typename P2, typename P3, typename P4,
		typename P5, typename P6, typename P7>
		int Call(RT(*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(func, L, index, pThreadInfo);
	}

	template <typename RT, typename P1, typename P2, typename P3, typename P4,
		typename P5, typename P6, typename P7, typename P8>
		int Call(RT(*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7, P8), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(func, L, index, pThreadInfo);
	}

	template <typename RT, typename P1, typename P2, typename P3, typename P4,
		typename P5, typename P6, typename P7, typename P8, typename P9>
		int Call(RT(*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7, P8, P9), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(func, L, index, pThreadInfo);
	}

	template <typename RT, typename P1, typename P2, typename P3, typename P4,
		typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
		int Call(RT(*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(func, L, index, pThreadInfo);
	}

	template <typename RT, typename P1, typename P2, typename P3, typename P4,
		typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11>
		int Call(RT(*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(func, L, index, pThreadInfo);
	}


	//This Call
	template <typename Callee, typename RT>
	int Call(Callee& callee, RT(Callee::*func)(CLuaThread *), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(callee, func, L, index, pThreadInfo);
	}


	template <typename Callee, typename RT, typename P1>
	int Call(Callee& callee, RT(Callee::*func)(CLuaThread *, P1), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(callee, func, L, index, pThreadInfo);
	}


	template <typename Callee, typename RT, typename P1, typename P2>
	int Call(Callee& callee, RT(Callee::*func)(CLuaThread *, P1, P2), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(callee, func, L, index, pThreadInfo);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3>
	int Call(Callee& callee, RT(Callee::*func)(CLuaThread *, P1, P2, P3), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(callee, func, L, index, pThreadInfo);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3,
		typename P4>
		int Call(Callee& callee, RT(Callee::*func)(CLuaThread *, P1, P2, P3, P4), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(callee, func, L, index, pThreadInfo);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3,
		typename P4, typename P5>
		int Call(Callee& callee, RT(Callee::*func)(CLuaThread *, P1, P2, P3, P4, P5), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(callee, func, L, index, pThreadInfo);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3,
		typename P4, typename P5, typename P6>
		int Call(Callee& callee, RT(Callee::*func)(CLuaThread *, P1, P2, P3, P4, P5, P6), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(callee, func, L, index, pThreadInfo);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3,
		typename P4, typename P5, typename P6, typename P7>
		int Call(Callee& callee, RT(Callee::*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(callee, func, L, index, pThreadInfo);
	}

	template <typename Callee, typename RT, typename P1, typename P2, typename P3,
		typename P4, typename P5, typename P6, typename P7, typename P8>
		int Call(Callee& callee, RT(Callee::*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7, P8), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(callee, func, L, index, pThreadInfo);
	}

	template <typename Callee, typename RT, typename P1, typename P2, typename P3,
		typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		int Call(Callee& callee, RT(Callee::*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7, P8, P9), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(callee, func, L, index, pThreadInfo);
	}

	template <typename Callee, typename RT, typename P1, typename P2, typename P3,
		typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
		int Call(Callee& callee, RT(Callee::*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(callee, func, L, index, pThreadInfo);
	}

	template <typename Callee, typename RT, typename P1, typename P2, typename P3,
		typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11>
		int Call(Callee& callee, RT(Callee::*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11), lua_State* L, int index, CLuaThread * pThreadInfo)
	{
		return ReturnSpecialization<RT>::Call(callee, func, L, index, pThreadInfo);
	}

	inline unsigned char* GetFirstUpValueAsUserData(lua_State* L)
	{
		void* buffer;

#ifndef FAST_DISPATCH
		buffer = lua_touserdata(L, lua_upvalueindex(1));
#else // FAST_DISPATCH
#endif // FAST_DISPATCH

		return (unsigned char*)buffer;
	}

	//inline void* GetObjectUserData1(lua_State* L)
	//{
	//	StkId o = restorestack(L, sizeof(TValue));

	//	if (ttype(o) == LUA_TUSERDATA)
	//		return *(void**)(rawuvalue(o) + 1);
	//	else
	//	{
	//		luaL_argerror(L, 1, "must be userdata or a table with a userdata member called __object");
	//	}

	//	return NULL;
	//}

	//inline void* GetObjectUserData2(lua_State* L)
	//{
	//	StkId o = restorestack(L, sizeof(TValue) * 2);

	//	if (ttype(o) == LUA_TUSERDATA)
	//		return *(void**)(rawuvalue(o) + 1);
	//	else
	//	{
	//		luaL_argerror(L, 1, "must be userdata or a table with a userdata member called __object");
	//	}

	//	return NULL;
	//}

	inline void* GetObjectIndexData1(lua_State* L)
	{
		StkId o = restorestack(L, sizeof(TValue));

		if (ttype(o) == LUA_TLIGHTUSERDATA)
			return pvalue(o);
		else
		{
			luaL_argerror(L, 1, "must be userdata or a table with a userdata member called __object");
		}

		return NULL;
	}
	inline void* GetObjectIndexData2(lua_State* L)
	{
		StkId o = restorestack(L, sizeof(TValue) * 2);

		if (ttype(o) == LUA_TLIGHTUSERDATA)
			return pvalue(o);
		else
		{
			luaL_argerror(L, 1, "must be userdata or a table with a userdata member called __object");
		}

		return NULL;
	}
	
	template <typename Func, int startIndex>
	class DirectCallFunctionDispatchHelper
	{
	public:
		static inline int DirectCallFunctionDispatcher(lua_State* L)
		{
			unsigned char* buffer = GetFirstUpValueAsUserData(L);
			CLuaThread * pThreadInfo = (CLuaThread*)GetObjectIndexData2(L);

			return Call(*(Func*)(buffer), L, 1, pThreadInfo);
		}
	};

	template <typename Callee, typename Func, int startIndex>
	class DirectCallObjectMemberDispatcherHelper
	{
	public:
		static inline int DirectCallMemberDispatcher(lua_State* L)
		{
			unsigned char* buffer = GetFirstUpValueAsUserData(L);
			Callee *callee = (Callee *)GetObjectIndexData1(L);
			CLuaThread * pThreadInfo = (CLuaThread*)GetObjectIndexData2(L);

			if (callee)
				return Call(*callee, *(Func*)buffer, L, startIndex, pThreadInfo);
			else
			{
				luaL_error(L, "host object is null on call");
				return 0;
			}
		}
	};

	template <typename Callee, typename Func, int startIndex>
	class DirectCallMetaClassMemberDispatcherHelper
	{
	public:
		static inline int DirectCallMemberDispatcher(lua_State* L)
		{
			unsigned char* buffer = GetFirstUpValueAsUserData(L);
			Callee *callee = dynamic_cast<Callee *>(*((CLuaBaseMetaClass **)lua_touserdata(L, startIndex)));			
			CLuaThread * pThreadInfo = (CLuaThread*)GetObjectIndexData2(L);

			if (callee)
			{
				return Call(*callee, *(Func*)buffer, L, startIndex + 1, pThreadInfo);
			}
			else
			{
				luaL_argerror(L, 1, "this is null in MetaClassCall");
				return 0;
			}
				
		}
	};

	template <typename Callee, typename Func, int startIndex>
	class DirectCallStaticMetaClassMemberDispatcherHelper
	{
	public:
		static inline int DirectCallMemberDispatcher(lua_State* L)
		{
			
			unsigned char* buffer = GetFirstUpValueAsUserData(L);
			BYTE * pData = (BYTE *)lua_touserdata(L, startIndex);
			UINT DataLen = (UINT)lua_rawlen(L, startIndex);
			if (pData&&DataLen)
			{
				Callee * pCallee = (Callee *)pData;
				UINT ClassSize = (UINT)pCallee->GetMetaClassSize();
				if(ClassSize&&ClassSize<= DataLen)
				{
					//pCallee->Attach(pData + ClassSize, DataLen - ClassSize);
					CLuaThread * pThreadInfo = (CLuaThread*)GetObjectIndexData2(L);

					return Call(*pCallee, *(Func*)buffer, L, startIndex + 1, pThreadInfo);
				}
				else
				{
					luaL_argerror(L, 1, "Invalid StaticMetaClassSize");
					return 0;
				}
			}
			else
			{
				luaL_argerror(L, 1, "this is null in StaticMetaClassCall");
				return 0;
			}
		}
	};

}

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE void
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Ret;if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 0;
#include "LuaCallWrapTemplate.h"

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE bool
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(L, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE char
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(L, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE unsigned char
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(L, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE short
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(L, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE unsigned short
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(L, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE int
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(L, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE unsigned int
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(L, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE __int64
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(L, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE unsigned __int64
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(L, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"




#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE float
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(L, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE double
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(L, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE const char *
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(L, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE LuaValue
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(L, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE CLuaTable
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(L, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE CEasyArray<LuaValue>
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(L, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return (int)Ret.GetCount();
#include "LuaCallWrapTemplate.h"

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE LUA_EMPTY_VALUE
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Ret; if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION

#define LUA_WRAP_CALL_RETURN_TYPE LUA_CUSTOM_RETURN
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) LUA_CUSTOM_RETURN r=Ret; if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return r.ReturnCount;
#include "LuaCallWrapTemplate.h"