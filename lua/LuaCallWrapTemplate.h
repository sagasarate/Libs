﻿namespace LuaWrap
{
	template<>
	struct ReturnSpecialization < LUA_WRAP_CALL_RETURN_TYPE >
	{
		static int Call(LUA_WRAP_CALL_RETURN_TYPE(*func)(), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			LUA_WRAP_RETURN_PUSH_OPERATION(func());
		}


		template <typename P1>
		static int Call(LUA_WRAP_CALL_RETURN_TYPE(*func)(P1), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);

			LUA_WRAP_RETURN_PUSH_OPERATION(func(
				Get(TypeWrapper<P1>(), L, index + 0)
			));
		}


		template <typename P1, typename P2>
		static int Call(LUA_WRAP_CALL_RETURN_TYPE(*func)(P1, P2), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);

			LUA_WRAP_RETURN_PUSH_OPERATION(func(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1)
			));
		}


		template <typename P1, typename P2, typename P3>
		static int Call(LUA_WRAP_CALL_RETURN_TYPE(*func)(P1, P2, P3), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);

			LUA_WRAP_RETURN_PUSH_OPERATION(func(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2)
			));
		}

		template <typename P1, typename P2, typename P3, typename P4>
		static int Call(LUA_WRAP_CALL_RETURN_TYPE(*func)(P1, P2, P3, P4), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);

			LUA_WRAP_RETURN_PUSH_OPERATION(func(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3)
			));
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5>
		static int Call(LUA_WRAP_CALL_RETURN_TYPE(*func)(P1, P2, P3, P4, P5), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);

			LUA_WRAP_RETURN_PUSH_OPERATION(func(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4)
			));
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		static int Call(LUA_WRAP_CALL_RETURN_TYPE(*func)(P1, P2, P3, P4, P5, P6), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);

			LUA_WRAP_RETURN_PUSH_OPERATION(func(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5)
			));
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		static int Call(LUA_WRAP_CALL_RETURN_TYPE(*func)(P1, P2, P3, P4, P5, P6, P7), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);

			LUA_WRAP_RETURN_PUSH_OPERATION(func(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6)
			));
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		static int Call(LUA_WRAP_CALL_RETURN_TYPE(*func)(P1, P2, P3, P4, P5, P6, P7, P8), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);

			LUA_WRAP_RETURN_PUSH_OPERATION(func(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7)
			));
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		static int Call(LUA_WRAP_CALL_RETURN_TYPE(*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);

			LUA_WRAP_RETURN_PUSH_OPERATION(func(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7),
				Get(TypeWrapper<P9>(), L, index + 8)
			));
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
		static int Call(LUA_WRAP_CALL_RETURN_TYPE(*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);
			luaL_argassert(10, index + 9);

			LUA_WRAP_RETURN_PUSH_OPERATION(func(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7),
				Get(TypeWrapper<P9>(), L, index + 8),
				Get(TypeWrapper<P10>(), L, index + 9)
			));
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11>
		static int Call(LUA_WRAP_CALL_RETURN_TYPE(*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);
			luaL_argassert(10, index + 9);
			luaL_argassert(11, index + 10);

			LUA_WRAP_RETURN_PUSH_OPERATION(func(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7),
				Get(TypeWrapper<P9>(), L, index + 8),
				Get(TypeWrapper<P10>(), L, index + 9),
				Get(TypeWrapper<P11>(), L, index + 10)
			));
		}

		//////////////////////////////////////////////////////////////////////////////
		template <typename Callee>
		static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)());
		}

		template <typename Callee, typename P1>
		static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(P1), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(
				Get(TypeWrapper<P1>(), L, index + 0)
				));
		}


		template <typename Callee, typename P1, typename P2>
		static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(P1, P2), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3>
		static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(P1, P2, P3), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4>
			static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(P1, P2, P3, P4), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5>
			static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(P1, P2, P3, P4, P5), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6>
			static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(P1, P2, P3, P4, P5, P6), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7>
			static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(P1, P2, P3, P4, P5, P6, P7), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8>
			static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(P1, P2, P3, P4, P5, P6, P7, P8), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
			static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(P1, P2, P3, P4, P5, P6, P7, P8, P9), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7),
				Get(TypeWrapper<P9>(), L, index + 8)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
			static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);
			luaL_argassert(10, index + 9);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7),
				Get(TypeWrapper<P9>(), L, index + 8),
				Get(TypeWrapper<P10>(), L, index + 9)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11>
			static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);
			luaL_argassert(10, index + 9);
			luaL_argassert(11, index + 10);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7),
				Get(TypeWrapper<P9>(), L, index + 8),
				Get(TypeWrapper<P10>(), L, index + 9),
				Get(TypeWrapper<P11>(), L, index + 10)
				));
		}
		//////////////////////////////////////////////////////////////////////////////
		template <typename Callee>
		static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)() const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)());
		}

		template <typename Callee, typename P1>
		static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(P1) const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(
				Get(TypeWrapper<P1>(), L, index + 0)
				));
		}


		template <typename Callee, typename P1, typename P2>
		static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(P1, P2) const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3>
		static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(P1, P2, P3) const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4>
			static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(P1, P2, P3, P4) const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5>
			static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(P1, P2, P3, P4, P5) const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6>
			static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(P1, P2, P3, P4, P5, P6) const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7>
			static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(P1, P2, P3, P4, P5, P6, P7) const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8>
			static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(P1, P2, P3, P4, P5, P6, P7, P8) const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
			static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(P1, P2, P3, P4, P5, P6, P7, P8, P9) const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7),
				Get(TypeWrapper<P9>(), L, index + 8)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
			static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10) const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);
			luaL_argassert(10, index + 9);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7),
				Get(TypeWrapper<P9>(), L, index + 8),
				Get(TypeWrapper<P10>(), L, index + 9)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11>
			static int Call(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11) const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);
			luaL_argassert(10, index + 9);
			luaL_argassert(11, index + 10);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7),
				Get(TypeWrapper<P9>(), L, index + 8),
				Get(TypeWrapper<P10>(), L, index + 9),
				Get(TypeWrapper<P11>(), L, index + 10)
				));
		}
		//////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////
		static int CallWithThread( LUA_WRAP_CALL_RETURN_TYPE (*func)(CLuaThread *), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			LUA_WRAP_RETURN_PUSH_OPERATION(func(pThreadInfo));
		}


		template <typename P1>
		static int CallWithThread( LUA_WRAP_CALL_RETURN_TYPE (*func)(CLuaThread *, P1), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			luaL_argassert(1, index + 0);

			LUA_WRAP_RETURN_PUSH_OPERATION(func(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0)
			));
		}


		template <typename P1, typename P2>
		static int CallWithThread( LUA_WRAP_CALL_RETURN_TYPE (*func)(CLuaThread *, P1, P2), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);

			LUA_WRAP_RETURN_PUSH_OPERATION(func(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1)
			));
		}


		template <typename P1, typename P2, typename P3>
		static int CallWithThread( LUA_WRAP_CALL_RETURN_TYPE (*func)(CLuaThread *, P1, P2, P3), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);

			LUA_WRAP_RETURN_PUSH_OPERATION(func(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2)
			));
		}

		template <typename P1, typename P2, typename P3, typename P4>
		static int CallWithThread( LUA_WRAP_CALL_RETURN_TYPE (*func)(CLuaThread *, P1, P2, P3, P4), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);

			LUA_WRAP_RETURN_PUSH_OPERATION(func(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3)
			));
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5>
		static int CallWithThread( LUA_WRAP_CALL_RETURN_TYPE (*func)(CLuaThread *, P1, P2, P3, P4, P5), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);

			LUA_WRAP_RETURN_PUSH_OPERATION(func(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4)
			));
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		static int CallWithThread( LUA_WRAP_CALL_RETURN_TYPE (*func)(CLuaThread *, P1, P2, P3, P4, P5, P6), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);

			LUA_WRAP_RETURN_PUSH_OPERATION(func(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5)
			));
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		static int CallWithThread( LUA_WRAP_CALL_RETURN_TYPE (*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);

			LUA_WRAP_RETURN_PUSH_OPERATION(func(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6)
			));
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		static int CallWithThread( LUA_WRAP_CALL_RETURN_TYPE (*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7, P8), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);

			LUA_WRAP_RETURN_PUSH_OPERATION(func(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7)
			));
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		static int CallWithThread( LUA_WRAP_CALL_RETURN_TYPE (*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7, P8, P9), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);

			LUA_WRAP_RETURN_PUSH_OPERATION(func(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7),
				Get(TypeWrapper<P9>(), L, index + 8)
			));
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
		static int CallWithThread( LUA_WRAP_CALL_RETURN_TYPE (*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);
			luaL_argassert(10, index + 9);

			LUA_WRAP_RETURN_PUSH_OPERATION(func(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7),
				Get(TypeWrapper<P9>(), L, index + 8),
				Get(TypeWrapper<P10>(), L, index + 9)
			));
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11>
		static int CallWithThread(LUA_WRAP_CALL_RETURN_TYPE(*func)(CLuaThread*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);
			luaL_argassert(10, index + 9);
			luaL_argassert(11, index + 10);

			LUA_WRAP_RETURN_PUSH_OPERATION(func(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7),
				Get(TypeWrapper<P9>(), L, index + 8),
				Get(TypeWrapper<P10>(), L, index + 9),
				Get(TypeWrapper<P11>(), L, index + 10)
			));
		}
		//////////////////////////////////////////////////////////////////////////////
		template <typename Callee>
		static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::*func)(CLuaThread *), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo));
		}


		template <typename Callee, typename P1>
		static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::*func)(CLuaThread *, P1), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			luaL_argassert(1, index + 0);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0)
				));
		}


		template <typename Callee, typename P1, typename P2>
		static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::*func)(CLuaThread *, P1, P2), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3>
		static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::*func)(CLuaThread *, P1, P2, P3), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4>
			static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::*func)(CLuaThread *, P1, P2, P3, P4), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5>
			static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::*func)(CLuaThread *, P1, P2, P3, P4, P5), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6>
			static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::*func)(CLuaThread *, P1, P2, P3, P4, P5, P6), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7>
			static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8>
			static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7, P8), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
			static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7, P8, P9), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7),
				Get(TypeWrapper<P9>(), L, index + 8)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
			static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::*func)(CLuaThread *, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10), lua_State* L, int index, CLuaThread * pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);
			luaL_argassert(10, index + 9);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7),
				Get(TypeWrapper<P9>(), L, index + 8),
				Get(TypeWrapper<P10>(), L, index + 9)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11>
			static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(CLuaThread*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11), lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);
			luaL_argassert(10, index + 9);
			luaL_argassert(11, index + 10);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7),
				Get(TypeWrapper<P9>(), L, index + 8),
				Get(TypeWrapper<P10>(), L, index + 9),
				Get(TypeWrapper<P11>(), L, index + 10)
				));
		}

		//////////////////////////////////////////////////////////////////////////////
		template <typename Callee>
		static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(CLuaThread*) const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo));
		}


		template <typename Callee, typename P1>
		static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(CLuaThread*, P1) const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0)
				));
		}


		template <typename Callee, typename P1, typename P2>
		static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(CLuaThread*, P1, P2) const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3>
		static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(CLuaThread*, P1, P2, P3) const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4>
			static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(CLuaThread*, P1, P2, P3, P4) const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5>
			static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(CLuaThread*, P1, P2, P3, P4, P5) const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6>
			static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(CLuaThread*, P1, P2, P3, P4, P5, P6) const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7>
			static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(CLuaThread*, P1, P2, P3, P4, P5, P6, P7) const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8>
			static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(CLuaThread*, P1, P2, P3, P4, P5, P6, P7, P8) const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
			static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(CLuaThread*, P1, P2, P3, P4, P5, P6, P7, P8, P9) const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7),
				Get(TypeWrapper<P9>(), L, index + 8)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
			static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(CLuaThread*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10) const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);
			luaL_argassert(10, index + 9);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7),
				Get(TypeWrapper<P9>(), L, index + 8),
				Get(TypeWrapper<P10>(), L, index + 9)
				));
		}

		template <typename Callee, typename P1, typename P2, typename P3,
			typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11>
			static int CallWithThread(Callee& callee, LUA_WRAP_CALL_RETURN_TYPE(Callee::* func)(CLuaThread*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11) const, lua_State* L, int index, CLuaThread* pThreadInfo)
		{
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);
			luaL_argassert(10, index + 9);
			luaL_argassert(11, index + 10);

			LUA_WRAP_RETURN_PUSH_OPERATION((callee.*func)(pThreadInfo,
				Get(TypeWrapper<P1>(), L, index + 0),
				Get(TypeWrapper<P2>(), L, index + 1),
				Get(TypeWrapper<P3>(), L, index + 2),
				Get(TypeWrapper<P4>(), L, index + 3),
				Get(TypeWrapper<P5>(), L, index + 4),
				Get(TypeWrapper<P6>(), L, index + 5),
				Get(TypeWrapper<P7>(), L, index + 6),
				Get(TypeWrapper<P8>(), L, index + 7),
				Get(TypeWrapper<P9>(), L, index + 8),
				Get(TypeWrapper<P10>(), L, index + 9),
				Get(TypeWrapper<P11>(), L, index + 10)
				));
		}
	};
}
