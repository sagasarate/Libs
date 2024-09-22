#pragma once
#include <type_traits>

namespace JSVM
{
	enum class JSVMInternalFieldIndex
	{
		This = 0,
		Max,
	};

	template <typename T, T>
	struct CallWrap;

	template <typename RT, typename... ArgTypes, RT(*func)(ArgTypes...)>
	struct CallWrap<RT(*)(ArgTypes...), func>
	{
		static constexpr auto ArgsLength = sizeof...(ArgTypes);

		using Func = RT(*)(ArgTypes...);

		static void CallbackFunc(jsvm_callback_info* callback_info)
		{
			jsvm_context* context = jsvm_callback_get_cur_context(callback_info);
			int ParamCount = jsvm_callback_get_arg_len(callback_info);
			if (ParamCount >= ArgsLength)
			{
				int Index = (int)ArgsLength - 1;
				auto Ret = func(TypeConvertor<ArgTypes>::FromJS(context, jsvm_callback_get_arg(callback_info, Index--))...);
				jsvm_callback_set_return(callback_info, TypeConvertor<RT>::ToJS(context, Ret));
			}
			else
			{
				jsvm_throw_exception_with_format(context, _T("not enough param:%d/%d"), ParamCount, (int)ArgsLength);
			}
		}
	};

	template <typename... ArgTypes, void(*func)(ArgTypes...)>
	struct CallWrap<void(*)(ArgTypes...), func>
	{
		static constexpr auto ArgsLength = sizeof...(ArgTypes);

		using Func = void(*)(ArgTypes...);

		static void CallbackFunc(jsvm_callback_info* callback_info)
		{
			jsvm_context* context = jsvm_callback_get_cur_context(callback_info);
			int ParamCount = jsvm_callback_get_arg_len(callback_info);
			if (ParamCount >= ArgsLength)
			{
				int Index = (int)ArgsLength - 1;
				func(TypeConvertor<ArgTypes>::FromJS(context, jsvm_callback_get_arg(callback_info, Index--))...);
			}
			else
			{
				jsvm_throw_exception_with_format(context, _T("not enough param:%d/%d"), ParamCount, (int)ArgsLength);
			}
		}
	};

	template <typename RT, typename This, typename... ArgTypes, RT(This::* func)(ArgTypes...)>
	struct CallWrap<RT(This::*)(ArgTypes...), func>
	{
		static constexpr auto ArgsLength = sizeof...(ArgTypes);

		using Func = RT(This::*)(ArgTypes...);

		static void CallbackFunc(jsvm_callback_info* callback_info)
		{
			jsvm_context* context = jsvm_callback_get_cur_context(callback_info);
			int ParamCount = jsvm_callback_get_arg_len(callback_info);
			if (ParamCount >= ArgsLength)
			{
				jsvm_value* ThisObj = jsvm_callback_get_this(callback_info);
				if (jsvm_is_object(ThisObj) && !jsvm_is_null(ThisObj))
				{
					This* pThis = (This*)jsvm_obj_get_internal_field(context, ThisObj, (int)JSVMInternalFieldIndex::This);
					if (pThis)
					{
						int Index = (int)ArgsLength - 1;
						auto Ret = (pThis->*func)(TypeConvertor<ArgTypes>::FromJS(context, jsvm_callback_get_arg(callback_info, Index--))...);
						jsvm_callback_set_return(callback_info, TypeConvertor<RT>::ToJS(context, Ret));
					}
					else
					{
						jsvm_throw_exception(context, _T("c obj this is null"));
					}
				}
				else
				{
					jsvm_throw_exception(context, _T("call with invalid this"));
				}
			}
			else
			{
				jsvm_throw_exception_with_format(context, _T("not enough param:%d/%d"), ParamCount, (int)ArgsLength);
			}
		}
	};

	template <typename RT, typename This, typename... ArgTypes, RT(This::* func)(ArgTypes...) const>
	struct CallWrap<RT(This::*)(ArgTypes...) const, func>
	{
		static constexpr auto ArgsLength = sizeof...(ArgTypes);

		using Func = RT(This::*)(ArgTypes...) const;

		static void CallbackFunc(jsvm_callback_info* callback_info)
		{
			jsvm_context* context = jsvm_callback_get_cur_context(callback_info);
			int ParamCount = jsvm_callback_get_arg_len(callback_info);
			if (ParamCount >= ArgsLength)
			{
				jsvm_value* ThisObj = jsvm_callback_get_this(callback_info);
				if (jsvm_is_object(ThisObj) && !jsvm_is_null(ThisObj))
				{
					This* pThis = (This*)jsvm_obj_get_internal_field(context, ThisObj, (int)JSVMInternalFieldIndex::This);
					if (pThis)
					{
						int Index = (int)ArgsLength - 1;
						auto Ret = (pThis->*func)(TypeConvertor<ArgTypes>::FromJS(context, jsvm_callback_get_arg(callback_info, Index--))...);
						jsvm_callback_set_return(callback_info, TypeConvertor<RT>::ToJS(context, Ret));
					}
					else
					{
						jsvm_throw_exception(context, _T("c obj this is null"));
					}
				}
				else
				{
					jsvm_throw_exception(context, _T("call with invalid this"));
				}
			}
			else
			{
				jsvm_throw_exception_with_format(context, _T("not enough param:%d/%d"), ParamCount, (int)ArgsLength);
			}
		}
	};

	template <typename This, typename... ArgTypes, void(This::* func)(ArgTypes...)>
	struct CallWrap<void(This::*)(ArgTypes...), func>
	{
		static constexpr auto ArgsLength = sizeof...(ArgTypes);

		using Func = void(This::*)(ArgTypes...);

		static void CallbackFunc(jsvm_callback_info* callback_info)
		{
			jsvm_context* context = jsvm_callback_get_cur_context(callback_info);
			int ParamCount = jsvm_callback_get_arg_len(callback_info);
			if (ParamCount >= ArgsLength)
			{
				jsvm_value* ThisObj = jsvm_callback_get_this(callback_info);
				if (jsvm_is_object(ThisObj) && !jsvm_is_null(ThisObj))
				{
					This* pThis = (This*)jsvm_obj_get_internal_field(context, ThisObj, (int)JSVMInternalFieldIndex::This);
					if (pThis)
					{
						int Index = (int)ArgsLength - 1;
						(pThis->*func)(TypeConvertor<ArgTypes>::FromJS(context, jsvm_callback_get_arg(callback_info, Index--))...);
					}
					else
					{
						jsvm_throw_exception(context, _T("c obj this is null"));
					}
				}
				else
				{
					jsvm_throw_exception(context, _T("call with invalid this"));
				}
			}
			else
			{
				jsvm_throw_exception_with_format(context, _T("not enough param:%d/%d"), ParamCount, (int)ArgsLength);
			}
		}
	};

	template <typename This, typename... ArgTypes, void(This::* func)(ArgTypes...) const>
	struct CallWrap<void(This::*)(ArgTypes...) const, func>
	{
		static constexpr auto ArgsLength = sizeof...(ArgTypes);

		using Func = void(This::*)(ArgTypes...) const;

		static void CallbackFunc(jsvm_callback_info* callback_info)
		{
			jsvm_context* context = jsvm_callback_get_cur_context(callback_info);
			int ParamCount = jsvm_callback_get_arg_len(callback_info);
			if (ParamCount >= ArgsLength)
			{
				jsvm_value* ThisObj = jsvm_callback_get_this(callback_info);
				if (jsvm_is_object(ThisObj) && !jsvm_is_null(ThisObj))
				{
					This* pThis = (This*)jsvm_obj_get_internal_field(context, ThisObj, (int)JSVMInternalFieldIndex::This);
					if (pThis)
					{
						int Index = (int)ArgsLength - 1;
						(pThis->*func)(TypeConvertor<ArgTypes>::FromJS(context, jsvm_callback_get_arg(callback_info, Index--))...);
					}
					else
					{
						jsvm_throw_exception(context, _T("c obj this is null"));
					}
				}
				else
				{
					jsvm_throw_exception(context, _T("call with invalid this"));
				}
			}
			else
			{
				jsvm_throw_exception_with_format(context, _T("not enough param:%d/%d"), ParamCount, (int)ArgsLength);
			}
		}
	};

	template <typename Func, Func func>
	jsvm_value* RegisterJSMemberFunction(jsvm_context* context, jsvm_value* Obj, LPCTSTR FuncName)
	{
		jsvm_tmpl* tmpl = jsvm_create_func_tmpl(context, CallWrap<Func, func>::CallbackFunc, jsvm_func_method, NULL, FuncName);
		jsvm_value* pFunc = jsvm_create_func(context, tmpl);
		if (jsvm_obj_set(context, Obj, FuncName, pFunc))
			return pFunc;
		return NULL;
	}

	template <typename Func, Func func>
	bool RegisterJSClassMethod(jsvm_context* context, jsvm_tmpl* class_tmpl, LPCTSTR MethodName, bool IsStatic)
	{
		jsvm_tmpl* tmpl = jsvm_create_func_tmpl(context, CallWrap<Func, func>::CallbackFunc, jsvm_func_method, NULL, MethodName);
		if (jsvm_class_set_method(context, class_tmpl, MethodName, tmpl, IsStatic))
			return true;
		return false;
	}

	template <typename Getter, Getter getter, typename Setter, Setter setter>
	bool RegisterJSClassProperty(jsvm_context* context, jsvm_tmpl* class_tmpl, LPCTSTR PropertyName, bool IsStatic)
	{
		jsvm_tmpl* getter_tmpl = nullptr;
		if constexpr (!std::is_same<Getter, std::nullptr_t>::value)
		{
			CEasyString MethodName;
			MethodName.Format(_T("get_%s"), PropertyName);
			getter_tmpl = jsvm_create_func_tmpl(context, CallWrap<Getter, getter>::CallbackFunc, jsvm_func_method, NULL, (LPCTSTR)MethodName);
		}
		jsvm_tmpl* setter_tmpl = nullptr;
		if constexpr (!std::is_same<Setter, std::nullptr_t>::value)
		{
			CEasyString MethodName;
			MethodName.Format(_T("set_%s"), PropertyName);
			setter_tmpl = jsvm_create_func_tmpl(context, CallWrap<Setter, setter>::CallbackFunc, jsvm_func_method, NULL, (LPCTSTR)MethodName);
		}
		if (jsvm_class_set_property(context, class_tmpl, PropertyName, getter_tmpl, setter_tmpl, IsStatic))
			return true;
		else
			LogJSVM("register property(%s) failed", PropertyName);
		return false;
	}

	template <typename Func, Func func>
	jsvm_value* RegisterJSGlobalFunction(jsvm_context* context, LPCTSTR FuncName)
	{
		jsvm_tmpl* tmpl = jsvm_create_func_tmpl(context, CallWrap<Func, func>::CallbackFunc, jsvm_func_standalone, NULL, FuncName);
		jsvm_value* pFunc = jsvm_create_func(context, tmpl);
		if (jsvm_global_set(context, FuncName, pFunc))
			return pFunc;
		return NULL;
	}

	template <typename T, T>
	struct CallWrapWithContext;

	template <typename RT, typename... ArgTypes, RT(*func)(jsvm_context*, ArgTypes...)>
	struct CallWrapWithContext<RT(*)(jsvm_context*, ArgTypes...), func>
	{
		static constexpr auto ArgsLength = sizeof...(ArgTypes);

		using Func = RT(*)(jsvm_context*, ArgTypes...);

		static void CallbackFunc(jsvm_callback_info* callback_info)
		{
			jsvm_context* context = jsvm_callback_get_cur_context(callback_info);
			int ParamCount = jsvm_callback_get_arg_len(callback_info);
			if (ParamCount >= ArgsLength)
			{
				int Index = (int)ArgsLength - 1;
				auto Ret = func(context, TypeConvertor<ArgTypes>::FromJS(context, jsvm_callback_get_arg(callback_info, Index--))...);
				jsvm_callback_set_return(callback_info, TypeConvertor<RT>::ToJS(context, Ret));
			}
			else
			{
				jsvm_throw_exception_with_format(context, _T("not enough param:%d/%d"), ParamCount, (int)ArgsLength);
			}
		}
	};

	template <typename... ArgTypes, void(*func)(jsvm_context*, ArgTypes...)>
	struct CallWrapWithContext<void(*)(jsvm_context*, ArgTypes...), func>
	{
		static constexpr auto ArgsLength = sizeof...(ArgTypes);

		using Func = void(*)(jsvm_context*, ArgTypes...);

		static void CallbackFunc(jsvm_callback_info* callback_info)
		{
			jsvm_context* context = jsvm_callback_get_cur_context(callback_info);
			int ParamCount = jsvm_callback_get_arg_len(callback_info);
			if (ParamCount >= ArgsLength)
			{
				int Index = (int)ArgsLength - 1;
				func(context, TypeConvertor<ArgTypes>::FromJS(context, jsvm_callback_get_arg(callback_info, Index--))...);
			}
			else
			{
				jsvm_throw_exception_with_format(context, _T("not enough param:%d/%d"), ParamCount, (int)ArgsLength);
			}
		}
	};

	template <typename RT, typename This, typename... ArgTypes, RT(This::* func)(jsvm_context*, ArgTypes...)>
	struct CallWrapWithContext<RT(This::*)(jsvm_context*, ArgTypes...), func>
	{
		static constexpr auto ArgsLength = sizeof...(ArgTypes);

		using Func = RT(This::*)(jsvm_context*, ArgTypes...);

		static void CallbackFunc(jsvm_callback_info* callback_info)
		{
			jsvm_context* context = jsvm_callback_get_cur_context(callback_info);
			int ParamCount = jsvm_callback_get_arg_len(callback_info);
			if (ParamCount >= ArgsLength)
			{
				jsvm_value* ThisObj = jsvm_callback_get_this(callback_info);
				if (jsvm_is_object(ThisObj) && !jsvm_is_null(ThisObj))
				{
					This* pThis = (This*)jsvm_obj_get_internal_field(context, ThisObj, (int)JSVMInternalFieldIndex::This);
					if (pThis)
					{
						int Index = (int)ArgsLength - 1;
						auto Ret = (pThis->*func)(context, TypeConvertor<ArgTypes>::FromJS(context, jsvm_callback_get_arg(callback_info, Index--))...);
						jsvm_callback_set_return(callback_info, TypeConvertor<RT>::ToJS(context, Ret));
					}
					else
					{
						jsvm_throw_exception(context, _T("c obj this is null"));
					}
				}
				else
				{
					jsvm_throw_exception(context, _T("call with invalid this"));
				}
			}
			else
			{
				jsvm_throw_exception_with_format(context, _T("not enough param:%d/%d"), ParamCount, (int)ArgsLength);
			}
		}
	};
	template <typename RT, typename This, typename... ArgTypes, RT(This::* func)(jsvm_context*, ArgTypes...) const>
	struct CallWrapWithContext<RT(This::*)(jsvm_context*, ArgTypes...) const, func>
	{
		static constexpr auto ArgsLength = sizeof...(ArgTypes);

		using Func = RT(This::*)(jsvm_context*, ArgTypes...) const;

		static void CallbackFunc(jsvm_callback_info* callback_info)
		{
			jsvm_context* context = jsvm_callback_get_cur_context(callback_info);
			int ParamCount = jsvm_callback_get_arg_len(callback_info);
			if (ParamCount >= ArgsLength)
			{
				jsvm_value* ThisObj = jsvm_callback_get_this(callback_info);
				if (jsvm_is_object(ThisObj) && !jsvm_is_null(ThisObj))
				{
					This* pThis = (This*)jsvm_obj_get_internal_field(context, ThisObj, (int)JSVMInternalFieldIndex::This);
					if (pThis)
					{
						int Index = (int)ArgsLength - 1;
						auto Ret = (pThis->*func)(context, TypeConvertor<ArgTypes>::FromJS(context, jsvm_callback_get_arg(callback_info, Index--))...);
						jsvm_callback_set_return(callback_info, TypeConvertor<RT>::ToJS(context, Ret));
					}
					else
					{
						jsvm_throw_exception(context, _T("c obj this is null"));
					}
				}
				else
				{
					jsvm_throw_exception(context, _T("call with invalid this"));
				}
			}
			else
			{
				jsvm_throw_exception_with_format(context, _T("not enough param:%d/%d"), ParamCount, (int)ArgsLength);
			}
		}
	};

	template <typename This, typename... ArgTypes, void(This::* func)(jsvm_context*, ArgTypes...)>
	struct CallWrapWithContext<void(This::*)(jsvm_context*, ArgTypes...), func>
	{
		static constexpr auto ArgsLength = sizeof...(ArgTypes);

		using Func = void(This::*)(jsvm_context*, ArgTypes...);

		static void CallbackFunc(jsvm_callback_info* callback_info)
		{
			jsvm_context* context = jsvm_callback_get_cur_context(callback_info);
			int ParamCount = jsvm_callback_get_arg_len(callback_info);
			if (ParamCount >= ArgsLength)
			{
				jsvm_value* ThisObj = jsvm_callback_get_this(callback_info);
				if (jsvm_is_object(ThisObj) && !jsvm_is_null(ThisObj))
				{
					This* pThis = (This*)jsvm_obj_get_internal_field(context, ThisObj, (int)JSVMInternalFieldIndex::This);
					if (pThis)
					{
						int Index = (int)ArgsLength - 1;
						(pThis->*func)(context, TypeConvertor<ArgTypes>::FromJS(context, jsvm_callback_get_arg(callback_info, Index--))...);
					}
					else
					{
						jsvm_throw_exception(context, _T("c obj this is null"));
					}
				}
				else
				{
					jsvm_throw_exception(context, _T("call with invalid this"));
				}
			}
			else
			{
				jsvm_throw_exception_with_format(context, _T("not enough param:%d/%d"), ParamCount, (int)ArgsLength);
			}
		}
	};
	template <typename This, typename... ArgTypes, void(This::* func)(jsvm_context*, ArgTypes...) const>
	struct CallWrapWithContext<void(This::*)(jsvm_context*, ArgTypes...) const, func>
	{
		static constexpr auto ArgsLength = sizeof...(ArgTypes);

		using Func = void(This::*)(jsvm_context*, ArgTypes...) const;

		static void CallbackFunc(jsvm_callback_info* callback_info)
		{
			jsvm_context* context = jsvm_callback_get_cur_context(callback_info);
			int ParamCount = jsvm_callback_get_arg_len(callback_info);
			if (ParamCount >= ArgsLength)
			{
				jsvm_value* ThisObj = jsvm_callback_get_this(callback_info);
				if (jsvm_is_object(ThisObj) && !jsvm_is_null(ThisObj))
				{
					This* pThis = (This*)jsvm_obj_get_internal_field(context, ThisObj, (int)JSVMInternalFieldIndex::This);
					if (pThis)
					{
						int Index = (int)ArgsLength - 1;
						(pThis->*func)(context, TypeConvertor<ArgTypes>::FromJS(context, jsvm_callback_get_arg(callback_info, Index--))...);
					}
					else
					{
						jsvm_throw_exception(context, _T("c obj this is null"));
					}
				}
				else
				{
					jsvm_throw_exception(context, _T("call with invalid this"));
				}
			}
			else
			{
				jsvm_throw_exception_with_format(context, _T("not enough param:%d/%d"), ParamCount, (int)ArgsLength);
			}
		}
	};

	template <typename Func, Func func>
	jsvm_value* RegisterJSMemberFunctionWithContext(jsvm_context* context, jsvm_value* Obj, LPCTSTR FuncName)
	{
		jsvm_tmpl* tmpl = jsvm_create_func_tmpl(context, CallWrapWithContext<Func, func>::CallbackFunc, jsvm_func_method, NULL, FuncName);
		jsvm_value* pFunc = jsvm_create_func(context, tmpl);
		if (jsvm_obj_set(context, Obj, FuncName, pFunc))
			return pFunc;
		return NULL;
	}

	template <typename Func, Func func>
	bool RegisterJSClassMethodWithContext(jsvm_context* context, jsvm_tmpl* class_tmpl, LPCTSTR MethodName, bool IsStatic)
	{
		jsvm_tmpl* tmpl = jsvm_create_func_tmpl(context, CallWrapWithContext<Func, func>::CallbackFunc, jsvm_func_method, NULL, MethodName);
		if (jsvm_class_set_method(context, class_tmpl, MethodName, tmpl, IsStatic))
			return true;
		return false;
	}

	template <typename Getter, Getter getter, typename Setter, Setter setter>
	bool RegisterJSClassPropertyWithContext(jsvm_context* context, jsvm_tmpl* class_tmpl, LPCTSTR PropertyName, bool IsStatic)
	{
		jsvm_tmpl* getter_tmpl = nullptr;
		if constexpr (!std::is_same<Getter, std::nullptr_t>::value)
		{
			CEasyString MethodName;
			MethodName.Format(_T("get_%s"), PropertyName);
			getter_tmpl = jsvm_create_func_tmpl(context, CallWrapWithContext<Getter, getter>::CallbackFunc, jsvm_func_method, NULL, (LPCTSTR)MethodName);
		}
		jsvm_tmpl* setter_tmpl = nullptr;
		if constexpr (!std::is_same<Setter, std::nullptr_t>::value)
		{
			CEasyString MethodName;
			MethodName.Format(_T("set_%s"), PropertyName);
			setter_tmpl = jsvm_create_func_tmpl(context, CallWrapWithContext<Setter, setter>::CallbackFunc, jsvm_func_method, NULL, (LPCTSTR)MethodName);
		}
		if (jsvm_class_set_property(context, class_tmpl, PropertyName, getter_tmpl, setter_tmpl, IsStatic))
			return true;
		else
			LogJSVM("register property(%s) failed", PropertyName);
		return false;
	}

	template <typename Func, Func func>
	jsvm_value* RegisterJSGlobalFunctionWithContext(jsvm_context* context, LPCTSTR FuncName)
	{
		jsvm_tmpl* tmpl = jsvm_create_func_tmpl(context, CallWrapWithContext<Func, func>::CallbackFunc, jsvm_func_standalone, NULL, FuncName);
		jsvm_value* pFunc = jsvm_create_func(context, tmpl);
		if (jsvm_global_set(context, FuncName, pFunc))
			return pFunc;
		return NULL;
	}

	//template<typename RT, typename... ArgTypes>
	//RT JSCallGlobal(jsvm_context* context, LPCTSTR FuncName, ArgTypes... Args);

	template<typename RT, typename... ArgTypes>
	typename std::enable_if<!std::is_void<RT>::value, RT>::type JSCallGlobal(jsvm_context* context, LPCTSTR FuncName, ArgTypes... Args)
	{
		jsvm_value* func = jsvm_global_get(context, FuncName);
		if (func && jsvm_is_function(func))
		{
			if constexpr (sizeof...(ArgTypes) > 0)
			{
				jsvm_value* args[] = { TypeConvertor<ArgTypes>::ToJS(context, Args)... };
				int argc = sizeof...(ArgTypes);
				jsvm_value* ret = jsvm_call(context, func, NULL, args, argc);
				if (ret)
					return TypeConvertor<RT>::FromJS(context, ret);
			}
			else
			{
				jsvm_value* ret = jsvm_call(context, func, NULL, NULL, 0);
				if (ret)
					return TypeConvertor<RT>::FromJS(context, ret);
			}
		}
		else
		{
			LogJSVM(_T("[%s] is not function"), FuncName);
		}
		return RT();
	};
	template<typename RT = void, typename... ArgTypes>
	typename std::enable_if<std::is_void<RT>::value, void>::type JSCallGlobal(jsvm_context* context, LPCTSTR FuncName, ArgTypes... Args)
	{
		jsvm_value* func = jsvm_global_get(context, FuncName);
		if (func && jsvm_is_function(func))
		{
			if constexpr (sizeof...(ArgTypes) > 0)
			{
				jsvm_value* args[] = { TypeConvertor<ArgTypes>::ToJS(context, Args)... };
				int argc = sizeof...(ArgTypes);
				jsvm_call(context, func, NULL, args, argc);
			}
			else
			{
				jsvm_call(context, func, NULL, NULL, 0);
			}
		}
		else
		{
			LogJSVM(_T("[%s] is not function"), FuncName);
		}
	};

	template<typename RT, typename... ArgTypes>
	typename std::enable_if<!std::is_void<RT>::value, RT>::type JSCallGlobal(jsvm_context* context, jsvm_value* func, ArgTypes... Args)
	{
		if constexpr (sizeof...(ArgTypes) > 0)
		{
			jsvm_value* args[] = { TypeConvertor<ArgTypes>::ToJS(context, Args)... };
			int argc = sizeof...(ArgTypes);
			jsvm_value* ret = jsvm_call(context, func, NULL, args, argc);
			if (ret)
				return TypeConvertor<RT>::FromJS(context, ret);
		}
		else
		{
			jsvm_value* ret = jsvm_call(context, func, NULL, NULL, 0);
			if (ret)
				return TypeConvertor<RT>::FromJS(context, ret);
		}
		return RT();
	};
	template<typename RT = void, typename... ArgTypes>
	typename std::enable_if<std::is_void<RT>::value, void>::type JSCallGlobal(jsvm_context* context, jsvm_value* func, ArgTypes... Args)
	{
		if constexpr (sizeof...(ArgTypes) > 0)
		{
			jsvm_value* args[] = { TypeConvertor<ArgTypes>::ToJS(context, Args)... };
			int argc = sizeof...(ArgTypes);
			jsvm_call(context, func, NULL, args, argc);
		}
		else
		{
			jsvm_call(context, func, NULL, NULL, 0);
		}
	};

	template<typename RT, typename... ArgTypes>
	typename std::enable_if<!std::is_void<RT>::value, RT>::type JSCallThis(jsvm_context* context, jsvm_value* ThisObj, LPCTSTR FuncName, ArgTypes... Args)
	{
		jsvm_value* func = jsvm_global_get(context, FuncName);
		if (func && jsvm_is_function(func))
		{
			if constexpr (sizeof...(ArgTypes) > 0)
			{
				jsvm_value* args[] = { TypeConvertor<ArgTypes>::ToJS(context, Args)... };
				int argc = sizeof...(ArgTypes);
				jsvm_value* ret = jsvm_call(context, func, ThisObj, args, argc);
				if (ret)
					return TypeConvertor<RT>::FromJS(context, ret);
			}
			else
			{
				jsvm_value* ret = jsvm_call(context, func, ThisObj, NULL, 0);
				if (ret)
					return TypeConvertor<RT>::FromJS(context, ret);
			}
		}
		else
		{
			LogJSVM(_T("[%s] is not function"), FuncName);
		}
		return RT();
	};

	//template<typename RT, typename... ArgTypes>
	//RT JSCallThis(jsvm_context* context, jsvm_value* ThisObj, jsvm_value* func, ArgTypes... Args);

	template<typename RT, typename... ArgTypes>
	typename std::enable_if<!std::is_void<RT>::value, RT>::type JSCallThis(jsvm_context* context, jsvm_value* ThisObj, jsvm_value* func, ArgTypes... Args)
	{
		if constexpr (sizeof...(ArgTypes) > 0)
		{
			jsvm_value* args[] = { TypeConvertor<ArgTypes>::ToJS(context, Args)... };
			int argc = sizeof...(ArgTypes);
			jsvm_value* ret = jsvm_call(context, func, ThisObj, args, argc);
			if (ret)
				return TypeConvertor<RT>::FromJS(context, ret);
		}
		else
		{
			jsvm_value* ret = jsvm_call(context, func, ThisObj, NULL, 0);
			if (ret)
				return TypeConvertor<RT>::FromJS(context, ret);
		}
		return RT();
	};
	template<typename RT = void, typename... ArgTypes>
	typename std::enable_if<std::is_void<RT>::value, void>::type JSCallThis(jsvm_context* context, jsvm_value* ThisObj, jsvm_value* func, ArgTypes... Args)
	{
		if constexpr (sizeof...(ArgTypes) > 0)
		{
			jsvm_value* args[] = { TypeConvertor<ArgTypes>::ToJS(context, Args)... };
			int argc = sizeof...(ArgTypes);
			jsvm_call(context, func, ThisObj, args, argc);
		}
		else
		{
			jsvm_call(context, func, ThisObj, NULL, 0);
		}
	};

	template<typename RT, typename... ArgTypes>
	RT JSClassNew(jsvm_context* context, jsvm_value* Class, ArgTypes... Args)
	{
		if (Class && jsvm_is_function(Class))
		{
			if constexpr (sizeof...(ArgTypes) > 0)
			{
				jsvm_value* args[] = { TypeConvertor<ArgTypes>::ToJS(context, Args)... };
				int argc = sizeof...(ArgTypes);
				jsvm_value* ret = jsvm_new_obj(context, Class, args, argc);
				if (ret)
					return TypeConvertor<RT>::FromJS(context, ret);
			}
			else
			{
				jsvm_value* ret = jsvm_new_obj(context, Class, NULL, 0);
				if (ret)
					return TypeConvertor<RT>::FromJS(context, ret);
			}
		}
		else
		{
			LogJSVM(_T("Class is not function"));
		}
		return RT();
	};

	template<typename T, bool (*DisposeFn)(T*)>
	class CAutoDisposer
	{
	protected:
		T* m_Handle;
	public:
		CAutoDisposer(T* handle)
		{
			m_Handle = handle;
		}
		~CAutoDisposer()
		{
			DisposeFn(m_Handle);
		}
	};



}

#define JSVM_REGISTER_JS_MEMBER_FUNC(context, Obj, func, FuncName) JSVM::RegisterJSMemberFunction<decltype(func), func>(context, Obj, FuncName)
#define JSVM_REGISTER_JS_CLASS_METHOD(context, class_tmpl, func, MethodName, IsStatic) JSVM::RegisterJSClassMethod<decltype(func), func>(context, class_tmpl, MethodName, IsStatic)
#define JSVM_REGISTER_JS_CLASS_PROPERTY(context, class_tmpl, PropertyName, getter, setter, IsStatic) JSVM::RegisterJSClassProperty<decltype(getter), getter,decltype(setter), setter>(context, class_tmpl, PropertyName, IsStatic)
#define JSVM_REGISTER_JS_GLOBAL_FUNC(context, func, FuncName) JSVM::RegisterJSMemberFunction<decltype(func), func>(context, FuncName)


#define JSVM_REGISTER_JS_MEMBER_FUNC_WITH_CONTEXT(context, Obj, func, FuncName) JSVM::RegisterJSMemberFunctionWithContext<decltype(func), func>(context, Obj, FuncName)
#define JSVM_REGISTER_JS_CLASS_METHOD_WITH_CONTEXT(context, class_tmpl, func, MethodName, IsStatic) JSVM::RegisterJSClassMethodWithContext<decltype(func), func>(context, class_tmpl, MethodName, IsStatic)
#define JSVM_REGISTER_JS_CLASS_PROPERTY_WITH_CONTEXT(context, class_tmpl, PropertyName, getter, setter, IsStatic) JSVM::RegisterJSClassPropertyWithContext<decltype(getter), getter,decltype(setter), setter>(context, class_tmpl, PropertyName, IsStatic)
#define JSVM_REGISTER_JS_GLOBAL_FUNC_WITH_CONTEXT(context, func, FuncName) JSVM::RegisterJSMemberFunctionWithContext<decltype(func), func>(context, FuncName)

