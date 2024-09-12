class CLuaBaseMetaClass;
namespace LuaWrap
{

	template<class RT>
	struct ReturnSpecialization
	{
		static int Call(RT(*func)(), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}


		template <typename P1>
		static int Call(RT(*func)(P1), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}


		template <typename P1, typename P2>
		static int Call(RT(*func)(P1, P2), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}


		template <typename P1, typename P2, typename P3>
		static int Call(RT(*func)(P1, P2, P3), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}


		template <typename P1, typename P2, typename P3, typename P4>
		static int Call(RT(*func)(P1, P2, P3, P4), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}


		template <typename P1, typename P2, typename P3, typename P4,
			typename P5>
			static int Call(RT(*func)(P1, P2, P3, P4, P5), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}


		template <typename P1, typename P2, typename P3, typename P4,
			typename P5, typename P6>
			static int Call(RT(*func)(P1, P2, P3, P4, P5, P6), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}


		template <typename P1, typename P2, typename P3, typename P4,
			typename P5, typename P6, typename P7>
			static int Call(RT(*func)(P1, P2, P3, P4, P5, P6, P7), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename P1, typename P2, typename P3, typename P4,
			typename P5, typename P6, typename P7, typename P8>
			static int Call(RT(*func)(P1, P2, P3, P4, P5, P6, P7, P8), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename P1, typename P2, typename P3, typename P4,
			typename P5, typename P6, typename P7, typename P8, typename P9>
			static int Call(RT(*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename P1, typename P2, typename P3, typename P4,
			typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
			static int Call(RT(*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename P1, typename P2, typename P3, typename P4,
			typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11>
			static int Call(RT(*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		//////////////////////////////////////////////////////////////////////////////

		template <typename Callee>
		static int Call(Callee& callee, RT(Callee::* func)(), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename Callee>
		static int Call(Callee& callee, RT(Callee::* func)() const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}


		template <typename Callee, typename P1>
		static int Call(Callee& callee, RT(Callee::* func)(P1), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}


		template <typename Callee, typename P1, typename P2>
		static int Call(Callee& callee, RT(Callee::* func)(P1, P2), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3>
		static int Call(Callee& callee, RT(Callee::* func)(P1, P2, P3), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4>
			static int Call(Callee& callee, RT(Callee::* func)(P1, P2, P3, P4), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5>
			static int Call(Callee& callee, RT(Callee::* func)(P1, P2, P3, P4, P5), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6>
			static int Call(Callee& callee, RT(Callee::* func)(P1, P2, P3, P4, P5, P6), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7>
			static int Call(Callee& callee, RT(Callee::* func)(P1, P2, P3, P4, P5, P6, P7), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8>
			static int Call(Callee& callee, RT(Callee::* func)(P1, P2, P3, P4, P5, P6, P7, P8), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
			static int Call(Callee& callee, RT(Callee::* func)(P1, P2, P3, P4, P5, P6, P7, P8, P9), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
			static int Call(Callee& callee, RT(Callee::* func)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11>
			static int Call(Callee& callee, RT(Callee::* func)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		static int CallWithThread(RT(*func)(CLuaThread*), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}


		template <typename P1>
		static int CallWithThread(RT(*func)(CLuaThread*, P1), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}


		template <typename P1, typename P2>
		static int CallWithThread(RT(*func)(CLuaThread*, P1, P2), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}


		template <typename P1, typename P2, typename P3>
		static int CallWithThread(RT(*func)(CLuaThread*, P1, P2, P3), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}


		template <typename P1, typename P2, typename P3, typename P4>
		static int CallWithThread(RT(*func)(CLuaThread*, P1, P2, P3, P4), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}


		template <typename P1, typename P2, typename P3, typename P4,
			typename P5>
			static int CallWithThread(RT(*func)(CLuaThread*, P1, P2, P3, P4, P5), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}


		template <typename P1, typename P2, typename P3, typename P4,
			typename P5, typename P6>
			static int CallWithThread(RT(*func)(CLuaThread*, P1, P2, P3, P4, P5, P6), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}


		template <typename P1, typename P2, typename P3, typename P4,
			typename P5, typename P6, typename P7>
			static int CallWithThread(RT(*func)(CLuaThread*, P1, P2, P3, P4, P5, P6, P7), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename P1, typename P2, typename P3, typename P4,
			typename P5, typename P6, typename P7, typename P8>
			static int CallWithThread(RT(*func)(CLuaThread*, P1, P2, P3, P4, P5, P6, P7, P8), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename P1, typename P2, typename P3, typename P4,
			typename P5, typename P6, typename P7, typename P8, typename P9>
			static int CallWithThread(RT(*func)(CLuaThread*, P1, P2, P3, P4, P5, P6, P7, P8, P9), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename P1, typename P2, typename P3, typename P4,
			typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
			static int CallWithThread(RT(*func)(CLuaThread*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename P1, typename P2, typename P3, typename P4,
			typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11>
			static int CallWithThread(RT(*func)(CLuaThread*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		//////////////////////////////////////////////////////////////////////////////

		template <typename Callee>
		static int CallWithThread(Callee& callee, RT(Callee::* func)(CLuaThread*), lua_State* L, int /*index*/, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}


		template <typename Callee, typename P1>
		static int CallWithThread(Callee& callee, RT(Callee::* func)(CLuaThread*, P1), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}


		template <typename Callee, typename P1, typename P2>
		static int CallWithThread(Callee& callee, RT(Callee::* func)(CLuaThread*, P1, P2), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3>
		static int CallWithThread(Callee& callee, RT(Callee::* func)(CLuaThread*, P1, P2, P3), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4>
			static int CallWithThread(Callee& callee, RT(Callee::* func)(CLuaThread*, P1, P2, P3, P4), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5>
			static int CallWithThread(Callee& callee, RT(Callee::* func)(CLuaThread*, P1, P2, P3, P4, P5), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6>
			static int CallWithThread(Callee& callee, RT(Callee::* func)(CLuaThread*, P1, P2, P3, P4, P5, P6), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7>
			static int CallWithThread(Callee& callee, RT(Callee::* func)(CLuaThread*, P1, P2, P3, P4, P5, P6, P7), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8>
			static int CallWithThread(Callee& callee, RT(Callee::* func)(CLuaThread*, P1, P2, P3, P4, P5, P6, P7, P8), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
			static int CallWithThread(Callee& callee, RT(Callee::* func)(CLuaThread*, P1, P2, P3, P4, P5, P6, P7, P8, P9), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
			static int CallWithThread(Callee& callee, RT(Callee::* func)(CLuaThread*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11>
			static int CallWithThread(Callee& callee, RT(Callee::* func)(CLuaThread*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_error(L, "unsupported return type");
			return 0;
		}
	};
};