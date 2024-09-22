#pragma once

#if defined _WIN32 || defined __CYGWIN__
#define JSVM_DLL_IMPORT __declspec(dllimport)
#define JSVM_DLL_EXPORT __declspec(dllexport)
#else
#if __GNUC__ >= 4
#define JSVM_DLL_IMPORT __attribute__ ((visibility("default")))
#define JSVM_DLL_EXPORT __attribute__ ((visibility("default")))
#else
#define JSVM_DLL_IMPORT
#define JSVM_DLL_EXPORT
#endif
#endif

namespace JSVM
{
	enum jsvm_api
	{
		jsvm_api_platform_init,
		jsvm_api_platform_dispose,

		jsvm_api_create_vm,
		jsvm_api_vm_dispose,

		jsvm_api_create_handle_scope,
		jsvm_api_get_context,
		jsvm_api_release_handle_scope,

		jsvm_api_run_script,
		jsvm_api_load_module,
		jsvm_api_call,
		jsvm_api_event_loop,

		jsvm_api_create_persistent_value,
		jsvm_api_get_value_from_persistent_value,
		jsvm_api_release_persistent_value,

		jsvm_api_create_null,
		jsvm_api_create_undefined,
		jsvm_api_create_boolean,
		jsvm_api_create_int32,
		jsvm_api_create_uint32,
		jsvm_api_create_int64,
		jsvm_api_create_uint64,
		jsvm_api_create_double,
		jsvm_api_create_string_utf8,
		jsvm_api_create_string_gbk,		
		jsvm_api_create_string_utf16,
		jsvm_api_create_binary_empty,
		jsvm_api_create_binary,
		jsvm_api_create_binary_copy_data,
		jsvm_api_create_array,
		jsvm_api_create_func,
		jsvm_api_create_obj,
		jsvm_api_new_obj,

		jsvm_api_get_value_bool,
		jsvm_api_get_value_int32,
		jsvm_api_get_value_uint32,
		jsvm_api_get_value_int64,
		jsvm_api_get_value_uint64,
		jsvm_api_get_value_double,
		jsvm_api_get_value_string_utf8,
		jsvm_api_get_value_string_utf8_no_buff,
		jsvm_api_get_value_string_gbk,
		jsvm_api_get_value_string_gbk_no_buff,		
		jsvm_api_get_value_string_utf16,
		jsvm_api_get_value_string_utf16_no_buff,
		jsvm_api_get_value_binary,
		jsvm_api_get_array_length,

		jsvm_api_get_type,
		jsvm_api_get_type_no_buff,
		jsvm_api_is_null,
		jsvm_api_is_undefined,
		jsvm_api_is_boolean,
		jsvm_api_is_int32,
		jsvm_api_is_uint32,
		jsvm_api_is_int64,
		jsvm_api_is_uint64,
		jsvm_api_is_double,
		jsvm_api_is_string,
		jsvm_api_is_object,
		jsvm_api_is_function,
		jsvm_api_is_binary,
		jsvm_api_is_array,
		jsvm_api_is_promise,
		jsvm_api_is_class,
		jsvm_api_of_class,

		jsvm_api_value_serialize,
		jsvm_api_value_deserialize,

		jsvm_api_create_func_tmpl,

		jsvm_api_create_persistent_tmpl,
		jsvm_api_get_tmpl_from_persistent_tmpl,
		jsvm_api_release_persistent_tmpl,


		jsvm_api_class_set_internal_field_count,
		jsvm_api_class_set_super_class,
		jsvm_api_class_set_method,
		jsvm_api_class_set_property,
		jsvm_api_class_set_member,

		jsvm_api_global,
		jsvm_api_global_set,
		jsvm_api_global_get,
		jsvm_api_obj_set,
		jsvm_api_obj_get,
		jsvm_api_obj_set_by_index,
		jsvm_api_obj_get_by_index,
		jsvm_api_obj_set_internal_field,
		jsvm_api_obj_get_internal_field,
		jsvm_api_get_obj_keys,
		jsvm_api_get_class_name,
		jsvm_api_class_get_name,

		jsvm_api_callback_get_this,
		jsvm_api_callback_get_arg_len,
		jsvm_api_callback_get_arg,
		jsvm_api_callback_set_return,
		jsvm_api_callback_get_cur_context,
		jsvm_api_callback_get_pass_data,

		jsvm_api_create_resolver,
		jsvm_api_get_promise,
		jsvm_api_resolver_resolve,
		jsvm_api_resolver_reject,
		jsvm_api_set_promise_then,

		jsvm_api_process_exception,
		jsvm_api_get_exception,
		jsvm_api_throw_exception,
		jsvm_api_throw_exception_with_format,
		jsvm_api_throw_exception_with_format_vl,

		jsvm_api_heap_stat,
		jsvm_api_heap_dump,
		jsvm_api_gc,

		jsvm_api_set_console_log_channel,

		jsvm_api_max
	};

	enum jsvm_create_flag
	{
		jsvm_not_redirect_console = 1,//不重定向console输出到日志系统
		jsvm_keep_event_loop_active = (1 << 1),//保持消息循环的活跃，这在需要调试的时候可能是必须的
	};

	enum jsvm_func_type
	{
		jsvm_func_standalone,
		jsvm_func_constructor,
		jsvm_func_method,
	};

	struct jsvm_vm
	{

	};

	struct jsvm_context
	{

	};

	struct jsvm_handle_scope
	{

	};

	struct jsvm_value
	{

	};

	struct jsvm_persistent_value
	{

	};

	struct jsvm_tmpl
	{

	};

	struct jsvm_persistent_tmpl
	{

	};

	struct jsvm_callback_info
	{

	};

	struct vm_heap_stat
	{
		size_t total_heap_size;
		size_t total_heap_size_executable;
		size_t total_physical_size;
		size_t total_available_size;
		size_t total_global_handles_size;
		size_t used_global_handles_size;
		size_t used_heap_size;
		size_t heap_size_limit;
		size_t malloced_memory;
		size_t external_memory;
		size_t peak_malloced_memory;
		size_t number_of_native_contexts;
		size_t number_of_detached_contexts;
	};

	struct child_vm_setting
	{
		jsvm_vm* parent_vm;
		UINT64		child_thread_id;
		LPCSTR		child_url;
		LPCSTR		child_name;
		child_vm_setting()
		{
			parent_vm = NULL;
			child_thread_id = 0;
			child_url = "";
			child_name = "";
		}
	};

	struct JSVMApiInfo {
		const char* name;
		void* pFunc;
	};

	template<class T>
	class BaseJSVMApiManager
	{
	protected:
		static HMODULE	m_hModule;
	public:
		static JSVMApiInfo m_APIList[jsvm_api_max];
	};
	template<class T>
	HMODULE BaseJSVMApiManager<T>::m_hModule = NULL;

	template<class T>
	JSVMApiInfo BaseJSVMApiManager<T>::m_APIList[jsvm_api_max] = {
		{"jsvm_platform_init",NULL},
		{"jsvm_platform_dispose",NULL},

		{"jsvm_create_vm",NULL},
		{"jsvm_vm_dispose",NULL},

		{"jsvm_create_handle_scope",NULL},
		{"jsvm_get_context",NULL},
		{"jsvm_release_handle_scope",NULL},

		{"jsvm_run_script",NULL},
		{"jsvm_load_module",NULL},
		{"jsvm_call",NULL},
		{"jsvm_event_loop",NULL},

		{"jsvm_create_persistent_value",NULL},
		{"jsvm_get_value_from_persistent_value",NULL},
		{"jsvm_release_persistent_value",NULL},

		{"jsvm_create_null",NULL},
		{"jsvm_create_undefined",NULL},
		{"jsvm_create_boolean",NULL},
		{"jsvm_create_int32",NULL},
		{"jsvm_create_uint32",NULL},
		{"jsvm_create_int64",NULL},
		{"jsvm_create_uint64",NULL},
		{"jsvm_create_double",NULL},
		{"jsvm_create_string_utf8",NULL},
		{"jsvm_create_string_gbk",NULL},		
		{"jsvm_create_string_utf16",NULL},
		{"jsvm_create_binary_empty",NULL},
		{"jsvm_create_binary",NULL},
		{"jsvm_create_binary_copy_data",NULL},
		{"jsvm_create_array",NULL},
		{"jsvm_create_func",NULL},
		{"jsvm_create_obj",NULL},
		{"jsvm_new_obj",NULL},

		{"jsvm_get_value_bool",NULL},
		{"jsvm_get_value_int32",NULL},
		{"jsvm_get_value_uint32",NULL},
		{"jsvm_get_value_int64",NULL},
		{"jsvm_get_value_uint64",NULL},
		{"jsvm_get_value_double",NULL},
		{"jsvm_get_value_string_utf8",NULL},
		{"jsvm_get_value_string_utf8_no_buff",NULL},
		{"jsvm_get_value_string_gbk",NULL},
		{"jsvm_get_value_string_gbk_no_buff",NULL},		
		{"jsvm_get_value_string_utf16",NULL},
		{"jsvm_get_value_string_utf16_no_buff",NULL},
		{"jsvm_get_value_binary",NULL},
		{"jsvm_get_array_length",NULL},

		{"jsvm_get_type", NULL},
		{"jsvm_get_type_no_buff",NULL },
		{"jsvm_is_null",NULL},
		{"jsvm_is_undefined",NULL},
		{"jsvm_is_boolean",NULL},
		{"jsvm_is_int32",NULL},
		{"jsvm_is_uint32",NULL},
		{"jsvm_is_int64",NULL},
		{"jsvm_is_uint64",NULL},
		{"jsvm_is_double",NULL},
		{"jsvm_is_string",NULL},
		{"jsvm_is_object",NULL},
		{"jsvm_is_function",NULL},
		{"jsvm_is_binary",NULL},
		{"jsvm_is_array",NULL},
		{"jsvm_is_promise",NULL},
		{"jsvm_is_class",NULL},
		{"jsvm_of_class",NULL},

		{"jsvm_value_serialize",NULL},
		{"jsvm_value_deserialize",NULL},

		{"jsvm_create_func_tmpl",NULL},

		{"jsvm_create_persistent_tmpl",NULL},
		{"jsvm_get_tmpl_from_persistent_tmpl",NULL},
		{"jsvm_release_persistent_tmpl",NULL},

		{"jsvm_class_set_internal_field_count",NULL},
		{"jsvm_class_set_super_class",NULL},
		{"jsvm_class_set_method",NULL},
		{"jsvm_class_set_property",NULL},
		{"jsvm_class_set_member",NULL},

		{"jsvm_global", NULL },
		{"jsvm_global_set",NULL},
		{"jsvm_global_get",NULL},
		{"jsvm_obj_set",NULL},
		{"jsvm_obj_get",NULL},
		{"jsvm_obj_set_by_index",NULL},
		{"jsvm_obj_get_by_index",NULL},
		{"jsvm_obj_set_internal_field",NULL},
		{"jsvm_obj_get_internal_field",NULL},
		{"jsvm_get_obj_keys",NULL},
		{"jsvm_get_class_name",NULL},
		{"jsvm_class_get_name",NULL},

		{"jsvm_callback_get_this",NULL},
		{"jsvm_callback_get_arg_len",NULL},
		{"jsvm_callback_get_arg",NULL},
		{"jsvm_callback_set_return",NULL},
		{"jsvm_callback_get_cur_context",NULL},
		{"jsvm_callback_get_pass_data",NULL},

		{"jsvm_create_resolver",NULL},
		{"jsvm_get_promise",NULL},
		{"jsvm_resolver_resolve",NULL},
		{"jsvm_resolver_reject",NULL},
		{"jsvm_set_promise_then",NULL},

		{"jsvm_process_exception", NULL },
		{"jsvm_get_exception", NULL },
		{"jsvm_throw_exception",NULL},
		{"jsvm_throw_exception_with_format",NULL },
		{"jsvm_throw_exception_with_format_vl",NULL },

		{"jsvm_heap_stat",NULL},
		{"jsvm_heap_dump",NULL},
		{"jsvm_gc",NULL },

		{"jsvm_set_console_log_channel",NULL },

	};

	class JSVMApiManager :public BaseJSVMApiManager<JSVMApiManager>
	{
	public:
		static bool LoadApi(LPCTSTR szDllPath)
		{
			ReleaseApi();
#ifdef WIN32
			m_hModule = LoadLibrary(szDllPath);
#else
			m_hModule = dlopen(szDllPath, RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
#endif
			if (m_hModule)
			{
				for (int i = 0; i < jsvm_api_max; i++)
				{
#ifdef WIN32
					m_APIList[i].pFunc = GetProcAddress(m_hModule, m_APIList[i].name);
#else
					m_APIList[i].pFunc = dlsym(m_hModule, m_APIList[i].name);
#endif
					if (!m_APIList[i].pFunc)
					{
						PrintImportantLog("api[%s] not exist.", m_APIList[i].name);
						return false;
					}

				}
				return true;
			}
			else
			{
#ifdef WIN32
				PrintImportantLog("load module[%s] failed:%d", szDllPath, GetLastError());
#else
				PrintImportantLog("load module[%s] failed:%s", szDllPath, dlerror());
#endif
			}
			return false;
		}
		static void ReleaseApi()
		{
			if (m_hModule)
			{
#ifdef WIN32
				FreeLibrary(m_hModule);
#else
				dlclose(m_hModule);
#endif
			}
		}
	};


	extern "C"
	{
		typedef void (*jsvm_callback)(jsvm_callback_info* callback_info);

#ifdef DELAY_LOAD_JSVM_API
		inline bool jsvm_platform_init(LPCSTR params)
		{
			return ((bool (*)(LPCSTR))JSVMApiManager::m_APIList[jsvm_api_platform_init].pFunc)(params);
		}
		inline void jsvm_platform_dispose()
		{
			((void(*)())JSVMApiManager::m_APIList[jsvm_api_platform_dispose].pFunc)();
		}

		inline jsvm_vm* jsvm_create_vm(LPCSTR script_root_dir, LPCSTR params = NULL, UINT Flags = 0, child_vm_setting* child_setting = NULL, LPCSTR init_script = NULL, size_t script_len = 0)
		{
			return ((jsvm_vm * (*)(LPCSTR, LPCSTR, UINT, child_vm_setting*, LPCSTR, size_t))JSVMApiManager::m_APIList[jsvm_api_create_vm].pFunc)(script_root_dir, params, Flags, child_setting, init_script, script_len);
		}
		inline bool jsvm_vm_dispose(jsvm_vm* vm)
		{
			return ((bool (*)(jsvm_vm*))JSVMApiManager::m_APIList[jsvm_api_vm_dispose].pFunc)(vm);
		}

		inline jsvm_handle_scope* jsvm_create_handle_scope(jsvm_vm* vm)
		{
			return ((jsvm_handle_scope * (*)(jsvm_vm*))JSVMApiManager::m_APIList[jsvm_api_create_handle_scope].pFunc)(vm);
		}
		inline jsvm_context* jsvm_get_context(jsvm_handle_scope* scope)
		{
			return ((jsvm_context * (*)(jsvm_handle_scope*))JSVMApiManager::m_APIList[jsvm_api_get_context].pFunc)(scope);
		}
		inline bool jsvm_release_handle_scope(jsvm_handle_scope* scope)
		{
			return ((bool (*)(jsvm_handle_scope*))JSVMApiManager::m_APIList[jsvm_api_release_handle_scope].pFunc)(scope);
		}

		inline jsvm_value* jsvm_run_script(jsvm_handle_scope* scope, LPCSTR script, size_t script_len = 0, LPCSTR origin = NULL, LPCSTR source_map_url = NULL)
		{
			return ((jsvm_value * (*)(jsvm_handle_scope*, LPCSTR, size_t, LPCSTR, LPCSTR))JSVMApiManager::m_APIList[jsvm_api_run_script].pFunc)(scope, script, script_len, origin, source_map_url);
		}
		inline jsvm_value* jsvm_load_module(jsvm_handle_scope* scope, LPCSTR script, size_t script_len = 0, LPCSTR origin = NULL, LPCSTR source_map_url = NULL)
		{
			return ((jsvm_value * (*)(jsvm_handle_scope*, LPCSTR, size_t, LPCSTR, LPCSTR))JSVMApiManager::m_APIList[jsvm_api_load_module].pFunc)(scope, script, script_len, origin, source_map_url);
		}
		inline jsvm_value* jsvm_call(jsvm_context* context, jsvm_value* func, jsvm_value* this_obj = NULL, jsvm_value** args = NULL, int argc = 0)
		{
			return ((jsvm_value * (*)(jsvm_context*, jsvm_value*, jsvm_value*, jsvm_value**, int))JSVMApiManager::m_APIList[jsvm_api_call].pFunc)(context, func, this_obj, args, argc);
		}
		inline int jsvm_event_loop(jsvm_handle_scope* scope, int max_loop_count = 1)
		{
			return ((int (*)(jsvm_handle_scope*, int))JSVMApiManager::m_APIList[jsvm_api_event_loop].pFunc)(scope, max_loop_count);
		}

		inline jsvm_persistent_value* jsvm_create_persistent_value(jsvm_context* context, jsvm_value* value)
		{
			return ((jsvm_persistent_value * (*)(jsvm_context*, jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_create_persistent_value].pFunc)(context, value);
		}
		inline jsvm_value* jsvm_get_value_from_persistent_value(jsvm_context* context, jsvm_persistent_value* value)
		{
			return ((jsvm_value * (*)(jsvm_context*, jsvm_persistent_value*))JSVMApiManager::m_APIList[jsvm_api_get_value_from_persistent_value].pFunc)(context, value);
		}
		inline bool jsvm_release_persistent_value(jsvm_persistent_value* value)
		{
			return ((bool (*)(jsvm_persistent_value*))JSVMApiManager::m_APIList[jsvm_api_release_persistent_value].pFunc)(value);
		}

		inline jsvm_value* jsvm_create_null(jsvm_context* context)
		{
			return ((jsvm_value * (*)(jsvm_context*))JSVMApiManager::m_APIList[jsvm_api_create_null].pFunc)(context);
		}
		inline jsvm_value* jsvm_create_undefined(jsvm_context* context)
		{
			return ((jsvm_value * (*)(jsvm_context*))JSVMApiManager::m_APIList[jsvm_api_create_undefined].pFunc)(context);
		}
		inline jsvm_value* jsvm_create_boolean(jsvm_context* context, bool value)
		{
			return ((jsvm_value * (*)(jsvm_context*, bool))JSVMApiManager::m_APIList[jsvm_api_create_boolean].pFunc)(context, value);
		}
		inline jsvm_value* jsvm_create_int32(jsvm_context* context, int value)
		{
			return ((jsvm_value * (*)(jsvm_context*, int))JSVMApiManager::m_APIList[jsvm_api_create_int32].pFunc)(context, value);
		}
		inline jsvm_value* jsvm_create_uint32(jsvm_context* context, unsigned int value)
		{
			return ((jsvm_value * (*)(jsvm_context*, unsigned int))JSVMApiManager::m_APIList[jsvm_api_create_uint32].pFunc)(context, value);
		}
		inline jsvm_value* jsvm_create_int64(jsvm_context* context, INT64 value)
		{
			return ((jsvm_value * (*)(jsvm_context*, INT64))JSVMApiManager::m_APIList[jsvm_api_create_int64].pFunc)(context, value);
		}
		inline jsvm_value* jsvm_create_uint64(jsvm_context* context, UINT64 value)
		{
			return ((jsvm_value * (*)(jsvm_context*, UINT64))JSVMApiManager::m_APIList[jsvm_api_create_uint64].pFunc)(context, value);
		}
		inline jsvm_value* jsvm_create_double(jsvm_context* context, double value)
		{
			return ((jsvm_value * (*)(jsvm_context*, double))JSVMApiManager::m_APIList[jsvm_api_create_double].pFunc)(context, value);
		}
		inline jsvm_value* jsvm_create_string_utf8(jsvm_context* context, const char* str, size_t length = 0)
		{
			return ((jsvm_value * (*)(jsvm_context*, const char*, size_t))JSVMApiManager::m_APIList[jsvm_api_create_string_utf8].pFunc)(context, str, length);
		}
		inline jsvm_value* jsvm_create_string_gbk(jsvm_context* context, const char* str, size_t length = 0)
		{
			return ((jsvm_value * (*)(jsvm_context*, const char*, size_t))JSVMApiManager::m_APIList[jsvm_api_create_string_gbk].pFunc)(context, str, length);
		}
		inline jsvm_value* jsvm_create_string_utf16(jsvm_context* context, const WCHAR* str, size_t length = 0)
		{
			return ((jsvm_value * (*)(jsvm_context*, const WCHAR*, size_t))JSVMApiManager::m_APIList[jsvm_api_create_string_utf16].pFunc)(context, str, length);
		}
		inline jsvm_value* jsvm_create_binary_empty(jsvm_context* context, size_t length)
		{
			return ((jsvm_value * (*)(jsvm_context*, size_t))JSVMApiManager::m_APIList[jsvm_api_create_binary_empty].pFunc)(context, length);
		}
		inline jsvm_value* jsvm_create_binary(jsvm_context* context, void* data, size_t length)
		{
			return ((jsvm_value * (*)(jsvm_context*, void*, size_t))JSVMApiManager::m_APIList[jsvm_api_create_binary].pFunc)(context, data, length);
		}
		inline jsvm_value* jsvm_create_binary_copy_data(jsvm_context* context, const void* data, size_t length)
		{
			return ((jsvm_value * (*)(jsvm_context*, const void*, size_t))JSVMApiManager::m_APIList[jsvm_api_create_binary_copy_data].pFunc)(context, data, length);
		}
		inline jsvm_value* jsvm_create_array(jsvm_context* context)
		{
			return ((jsvm_value * (*)(jsvm_context*))JSVMApiManager::m_APIList[jsvm_api_create_array].pFunc)(context);
		}
		inline jsvm_value* jsvm_create_func(jsvm_context* context, jsvm_tmpl* tmpl)
		{
			return ((jsvm_value * (*)(jsvm_context*, jsvm_tmpl*))JSVMApiManager::m_APIList[jsvm_api_create_func].pFunc)(context, tmpl);
		}
		inline jsvm_value* jsvm_create_obj(jsvm_context* context)
		{
			return ((jsvm_value * (*)(jsvm_context*))JSVMApiManager::m_APIList[jsvm_api_create_obj].pFunc)(context);
		}
		inline jsvm_value* jsvm_new_obj(jsvm_context* context, jsvm_value* class_func, jsvm_value** args = NULL, int argc = 0)
		{
			return ((jsvm_value * (*)(jsvm_context*, jsvm_value*, jsvm_value**, int))JSVMApiManager::m_APIList[jsvm_api_new_obj].pFunc)(context, class_func, args, argc);
		}

		inline bool jsvm_get_value_bool(jsvm_context* context, jsvm_value* value)
		{
			return ((bool (*)(jsvm_context*, jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_get_value_bool].pFunc)(context, value);
		}
		inline int jsvm_get_value_int32(jsvm_context* context, jsvm_value* value)
		{
			return ((int (*)(jsvm_context*, jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_get_value_int32].pFunc)(context, value);
		}
		inline unsigned int jsvm_get_value_uint32(jsvm_context* context, jsvm_value* value)
		{
			return ((unsigned int (*)(jsvm_context*, jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_get_value_uint32].pFunc)(context, value);
		}
		inline INT64 jsvm_get_value_int64(jsvm_context* context, jsvm_value* value)
		{
			return ((INT64(*)(jsvm_context*, jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_get_value_int64].pFunc)(context, value);
		}
		inline UINT64 jsvm_get_value_uint64(jsvm_context* context, jsvm_value* value)
		{
			return ((UINT64(*)(jsvm_context*, jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_get_value_uint64].pFunc)(context, value);
		}
		inline double jsvm_get_value_double(jsvm_context* context, jsvm_value* value)
		{
			return ((double (*)(jsvm_context*, jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_get_value_double].pFunc)(context, value);
		}
		inline int jsvm_get_value_string_utf8(jsvm_context* context, jsvm_value* value, char* buf, int bufsize)
		{
			return ((int (*)(jsvm_context*, jsvm_value*, char*, int))JSVMApiManager::m_APIList[jsvm_api_get_value_string_utf8].pFunc)(context, value, buf, bufsize);
		}
		inline const char* jsvm_get_value_string_utf8_no_buff(jsvm_context* context, jsvm_value* value)
		{
			return ((const char* (*)(jsvm_context*, jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_get_value_string_utf8_no_buff].pFunc)(context, value);
		}
		inline int jsvm_get_value_string_gbk(jsvm_context* context, jsvm_value* value, char* buf, int bufsize)
		{
			return ((int (*)(jsvm_context*, jsvm_value*, char*, int))JSVMApiManager::m_APIList[jsvm_api_get_value_string_gbk].pFunc)(context, value, buf, bufsize);
		}
		inline const char* jsvm_get_value_string_gbk_no_buff(jsvm_context* context, jsvm_value* value)
		{
			return ((const char* (*)(jsvm_context*, jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_get_value_string_gbk_no_buff].pFunc)(context, value);
		}
		inline int jsvm_get_value_string_utf16(jsvm_context* context, jsvm_value* value, WCHAR* buf, int bufsize)
		{
			return ((int (*)(jsvm_context*, jsvm_value*, WCHAR*, int))JSVMApiManager::m_APIList[jsvm_api_get_value_string_utf16].pFunc)(context, value, buf, bufsize);
		}
		inline const WCHAR* jsvm_get_value_string_utf16_no_buff(jsvm_context* context, jsvm_value* value)
		{
			return ((const WCHAR * (*)(jsvm_context*, jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_get_value_string_utf16_no_buff].pFunc)(context, value);
		}
		inline void* jsvm_get_value_binary(jsvm_context* context, jsvm_value* value, size_t* len)
		{
			return ((void* (*)(jsvm_context*, jsvm_value*, size_t*))JSVMApiManager::m_APIList[jsvm_api_get_value_binary].pFunc)(context, value, len);
		}
		inline unsigned int jsvm_get_array_length(jsvm_context* context, jsvm_value* value)
		{
			return ((unsigned int (*)(jsvm_context*, jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_get_array_length].pFunc)(context, value);
		}

		inline int jsvm_get_type(jsvm_context* context, jsvm_value* value, char* buff, int bufsize)
		{
			return ((int (*)(jsvm_context*, jsvm_value*, char*, int))JSVMApiManager::m_APIList[jsvm_api_get_type].pFunc)(context, value, buff, bufsize);
		}
		inline const char* jsvm_get_type_no_buff(jsvm_context* context, jsvm_value* value)
		{
			return ((const char* (*)(jsvm_context*, jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_get_type_no_buff].pFunc)(context, value);
		}
		inline bool jsvm_is_null(jsvm_value* value)
		{
			return ((bool (*)(jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_is_null].pFunc)(value);
		}
		inline bool jsvm_is_undefined(jsvm_value* value)
		{
			return ((bool (*)(jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_is_undefined].pFunc)(value);
		}
		inline bool jsvm_is_boolean(jsvm_value* value)
		{
			return ((bool (*)(jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_is_boolean].pFunc)(value);
		}
		inline bool jsvm_is_int32(jsvm_value* value)
		{
			return ((bool (*)(jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_is_int32].pFunc)(value);
		}
		inline bool jsvm_is_uint32(jsvm_value* value)
		{
			return ((bool (*)(jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_is_uint32].pFunc)(value);
		}
		inline bool jsvm_is_int64(jsvm_value* value)
		{
			return ((bool (*)(jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_is_int64].pFunc)(value);
		}
		inline bool jsvm_is_uint64(jsvm_value* value)
		{
			return ((bool (*)(jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_is_uint64].pFunc)(value);
		}
		inline bool jsvm_is_double(jsvm_value* value)
		{
			return ((bool (*)(jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_is_double].pFunc)(value);
		}
		inline bool jsvm_is_string(jsvm_value* value)
		{
			return ((bool (*)(jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_is_string].pFunc)(value);
		}
		inline bool jsvm_is_object(jsvm_value* value)
		{
			return ((bool (*)(jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_is_object].pFunc)(value);
		}
		inline bool jsvm_is_function(jsvm_value* value)
		{
			return ((bool (*)(jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_is_function].pFunc)(value);
		}
		inline bool jsvm_is_binary(jsvm_value* value)
		{
			return ((bool (*)(jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_is_binary].pFunc)(value);
		}
		inline bool jsvm_is_array(jsvm_value* value)
		{
			return ((bool (*)(jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_is_array].pFunc)(value);
		}
		inline bool jsvm_is_promise(jsvm_value* value)
		{
			return ((bool (*)(jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_is_promise].pFunc)(value);
		}
		inline bool jsvm_is_class(jsvm_context* context, jsvm_value* value, const char* class_name)
		{
			return ((bool (*)(jsvm_context*, jsvm_value*, const char*))JSVMApiManager::m_APIList[jsvm_api_is_class].pFunc)(context, value, class_name);
		}
		inline bool jsvm_of_class(jsvm_context* context, jsvm_value* value, const char* class_name)
		{
			return ((bool (*)(jsvm_context*, jsvm_value*, const char*))JSVMApiManager::m_APIList[jsvm_api_of_class].pFunc)(context, value, class_name);
		}

		inline int jsvm_value_serialize(jsvm_context* context, jsvm_value** values, int value_count, CEasyBuffer& Buffer)
		{
			return ((int (*)(jsvm_context*, jsvm_value**, int, CEasyBuffer&))JSVMApiManager::m_APIList[jsvm_api_value_serialize].pFunc)(context, values, value_count, Buffer);
		}
		inline int jsvm_value_deserialize(jsvm_context* context, const void* data, size_t data_len, jsvm_value** values, int value_count)
		{
			return ((int (*)(jsvm_context*, const void* data, size_t, jsvm_value**, int))JSVMApiManager::m_APIList[jsvm_api_value_deserialize].pFunc)(context, data, data_len, values, value_count);
		}


		inline jsvm_tmpl* jsvm_create_func_tmpl(jsvm_context* context, jsvm_callback func, jsvm_func_type func_type, void* pass_data, const char* class_name)
		{
			return ((jsvm_tmpl * (*)(jsvm_context*, jsvm_callback, jsvm_func_type, void*, const char*))JSVMApiManager::m_APIList[jsvm_api_create_func_tmpl].pFunc)(context, func, func_type, pass_data, class_name);
		}
		inline jsvm_persistent_tmpl* jsvm_create_persistent_tmpl(jsvm_context* context, jsvm_tmpl* tmpl)
		{
			return ((jsvm_persistent_tmpl * (*)(jsvm_context*, jsvm_tmpl*))JSVMApiManager::m_APIList[jsvm_api_create_persistent_tmpl].pFunc)(context, tmpl);
		}
		inline jsvm_tmpl* jsvm_get_tmpl_from_persistent_tmpl(jsvm_context* context, jsvm_persistent_tmpl* tmpl)
		{
			return ((jsvm_tmpl * (*)(jsvm_context*, jsvm_persistent_tmpl*))JSVMApiManager::m_APIList[jsvm_api_get_tmpl_from_persistent_tmpl].pFunc)(context, tmpl);
		}
		inline bool jsvm_release_persistent_tmpl(jsvm_context* context, jsvm_persistent_tmpl* tmpl)
		{
			return ((bool (*)(jsvm_context*, jsvm_persistent_tmpl*))JSVMApiManager::m_APIList[jsvm_api_release_persistent_tmpl].pFunc)(context, tmpl);
		}

		inline bool jsvm_class_set_internal_field_count(jsvm_tmpl* constructor, int field_count)
		{
			return ((bool (*)(jsvm_tmpl*, int))JSVMApiManager::m_APIList[jsvm_api_class_set_internal_field_count].pFunc)(constructor, field_count);
		}
		inline bool jsvm_class_set_super_class(jsvm_tmpl* constructor, jsvm_tmpl* super_constructor)
		{
			return ((bool (*)(jsvm_tmpl*, jsvm_tmpl*))JSVMApiManager::m_APIList[jsvm_api_class_set_super_class].pFunc)(constructor, super_constructor);
		}
		inline bool jsvm_class_set_method(jsvm_context* context, jsvm_tmpl* constructor, const char* method_name, jsvm_tmpl* method, bool is_static)
		{
			return ((bool (*)(jsvm_context*, jsvm_tmpl*, const char*, jsvm_tmpl*, bool))JSVMApiManager::m_APIList[jsvm_api_class_set_method].pFunc)(context, constructor, method_name, method, is_static);
		}
		inline bool jsvm_class_set_property(jsvm_context* context, jsvm_tmpl* constructor, const char* prop_name, jsvm_tmpl* getter, jsvm_tmpl* setter, bool is_static)
		{
			return ((bool (*)(jsvm_context*, jsvm_tmpl*, const char*, jsvm_tmpl*, jsvm_tmpl*, bool))JSVMApiManager::m_APIList[jsvm_api_class_set_property].pFunc)(context, constructor, prop_name, getter, setter, is_static);
		}
		inline bool jsvm_class_set_member(jsvm_context* context, jsvm_tmpl* constructor, const char* member_name, jsvm_value* member, bool is_static)
		{
			return ((bool (*)(jsvm_context*, jsvm_tmpl*, const char*, jsvm_value*, bool))JSVMApiManager::m_APIList[jsvm_api_class_set_member].pFunc)(context, constructor, member_name, member, is_static);
		}

		inline jsvm_value* jsvm_global(jsvm_context* context)
		{
			return ((jsvm_value * (*)(jsvm_context*))JSVMApiManager::m_APIList[jsvm_api_global].pFunc)(context);
		}
		inline bool jsvm_global_set(jsvm_context* context, const char* member_name, jsvm_value* value)
		{
			return ((bool (*)(jsvm_context*, const char*, jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_global_set].pFunc)(context, member_name, value);
		}
		inline jsvm_value* jsvm_global_get(jsvm_context* context, const char* member_name)
		{
			return ((jsvm_value * (*)(jsvm_context*, const char*))JSVMApiManager::m_APIList[jsvm_api_global_get].pFunc)(context, member_name);
		}
		inline bool jsvm_obj_set(jsvm_context* context, jsvm_value* obj, const char* member_name, jsvm_value* value)
		{
			return ((bool (*)(jsvm_context*, jsvm_value*, const char*, jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_obj_set].pFunc)(context, obj, member_name, value);
		}
		inline jsvm_value* jsvm_obj_get(jsvm_context* context, jsvm_value* obj, const char* member_name)
		{
			return ((jsvm_value * (*)(jsvm_context*, jsvm_value*, const char*))JSVMApiManager::m_APIList[jsvm_api_obj_get].pFunc)(context, obj, member_name);
		}
		inline bool jsvm_obj_set_by_index(jsvm_context* context, jsvm_value* obj, UINT index, jsvm_value* value)
		{
			return ((bool (*)(jsvm_context*, jsvm_value*, UINT, jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_obj_set_by_index].pFunc)(context, obj, index, value);
		}
		inline jsvm_value* jsvm_obj_get_by_index(jsvm_context* context, jsvm_value* obj, UINT index)
		{
			return ((jsvm_value * (*)(jsvm_context*, jsvm_value*, UINT))JSVMApiManager::m_APIList[jsvm_api_obj_get_by_index].pFunc)(context, obj, index);
		}
		inline bool jsvm_obj_set_internal_field(jsvm_context* context, jsvm_value* obj, int field, const void* data)
		{
			return ((bool (*)(jsvm_context*, jsvm_value*, int, const void*))JSVMApiManager::m_APIList[jsvm_api_obj_set_internal_field].pFunc)(context, obj, field, data);
		}
		inline const void* jsvm_obj_get_internal_field(jsvm_context* context, jsvm_value* obj, int field)
		{
			return ((const void* (*)(jsvm_context*, jsvm_value*, int))JSVMApiManager::m_APIList[jsvm_api_obj_get_internal_field].pFunc)(context, obj, field);
		}
		inline jsvm_value* jsvm_get_obj_keys(jsvm_context* context, jsvm_value* obj, bool include_prototypes)
		{
			return ((jsvm_value * (*)(jsvm_context*, jsvm_value*, bool))JSVMApiManager::m_APIList[jsvm_api_get_obj_keys].pFunc)(context, obj, include_prototypes);
		}
		inline jsvm_value* jsvm_get_class_name(jsvm_context* context, jsvm_value* obj)
		{
			return ((jsvm_value * (*)(jsvm_context*, jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_get_class_name].pFunc)(context, obj);
		}
		inline jsvm_value* jsvm_class_get_name(jsvm_context* context, jsvm_value* constructor)
		{
			return ((jsvm_value * (*)(jsvm_context*, jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_class_get_name].pFunc)(context, constructor);
		}

		inline jsvm_value* jsvm_callback_get_this(jsvm_callback_info* callback_info)
		{
			return ((jsvm_value * (*)(jsvm_callback_info*))JSVMApiManager::m_APIList[jsvm_api_callback_get_this].pFunc)(callback_info);
		}
		inline int jsvm_callback_get_arg_len(jsvm_callback_info* callback_info)
		{
			return ((int (*)(jsvm_callback_info*))JSVMApiManager::m_APIList[jsvm_api_callback_get_arg_len].pFunc)(callback_info);
		}
		inline jsvm_value* jsvm_callback_get_arg(jsvm_callback_info* callback_info, int index)
		{
			return ((jsvm_value * (*)(jsvm_callback_info*, int))JSVMApiManager::m_APIList[jsvm_api_callback_get_arg].pFunc)(callback_info, index);
		}
		inline bool jsvm_callback_set_return(jsvm_callback_info* callback_info, jsvm_value* value)
		{
			return ((bool (*)(jsvm_callback_info*, jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_callback_set_return].pFunc)(callback_info, value);
		}
		inline jsvm_context* jsvm_callback_get_cur_context(jsvm_callback_info* callback_info)
		{
			return ((jsvm_context * (*)(jsvm_callback_info*))JSVMApiManager::m_APIList[jsvm_api_callback_get_cur_context].pFunc)(callback_info);
		}
		inline void* jsvm_callback_get_pass_data(jsvm_callback_info* callback_info)
		{
			return ((void* (*)(jsvm_callback_info*))JSVMApiManager::m_APIList[jsvm_api_callback_get_pass_data].pFunc)(callback_info);
		}

		inline jsvm_value* jsvm_create_resolver(jsvm_context* context)
		{
			return ((jsvm_value * (*)(jsvm_context*))JSVMApiManager::m_APIList[jsvm_api_create_resolver].pFunc)(context);
		}
		inline jsvm_value* jsvm_get_promise(jsvm_value* resolver)
		{
			return ((jsvm_value * (*)(jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_get_promise].pFunc)(resolver);
		}
		inline bool jsvm_resolver_resolve(jsvm_context* context, jsvm_value* resolver, jsvm_value* return_value)
		{
			return ((bool (*)(jsvm_context*, jsvm_value*, jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_resolver_resolve].pFunc)(context, resolver, return_value);
		}
		inline bool jsvm_resolver_reject(jsvm_context* context, jsvm_value* resolver, jsvm_value* return_value)
		{
			return ((bool (*)(jsvm_context*, jsvm_value*, jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_resolver_reject].pFunc)(context, resolver, return_value);
		}
		inline bool jsvm_set_promise_then(jsvm_context* context, jsvm_value* promise, jsvm_value* callback_func)
		{
			return ((bool (*)(jsvm_context*, jsvm_value*, jsvm_value*))JSVMApiManager::m_APIList[jsvm_api_set_promise_then].pFunc)(context, promise, callback_func);
		}

		inline bool jsvm_process_exception(jsvm_handle_scope* scope)
		{
			return ((bool (*)(jsvm_handle_scope*))JSVMApiManager::m_APIList[jsvm_api_process_exception].pFunc)(scope);
		}
		inline bool jsvm_get_exception(jsvm_handle_scope* scope, char* msg_buff, size_t buff_len)
		{
			return ((bool (*)(jsvm_handle_scope*, char*, size_t))JSVMApiManager::m_APIList[jsvm_api_get_exception].pFunc)(scope, msg_buff, buff_len);
		}
		inline void jsvm_throw_exception(jsvm_context* context, const char* error_msg)
		{
			((void (*)(jsvm_context*, const char*))JSVMApiManager::m_APIList[jsvm_api_throw_exception].pFunc)(context, error_msg);
		}
		inline void jsvm_throw_exception_with_format(jsvm_context* context, const char* error_msg, ...)
		{
			va_list	vl;
			va_start(vl, error_msg);
			((void (*)(jsvm_context*, const char*, va_list))JSVMApiManager::m_APIList[jsvm_api_throw_exception_with_format_vl].pFunc)(context, error_msg, vl);
			va_end(vl);
		}
		inline void jsvm_throw_exception_with_format_vl(jsvm_context* context, const char* error_msg, va_list vl)
		{
			((void (*)(jsvm_context*, const char*, va_list))JSVMApiManager::m_APIList[jsvm_api_throw_exception_with_format_vl].pFunc)(context, error_msg, vl);
		}

		inline void jsvm_heap_stat(jsvm_vm* vm, vm_heap_stat* stat_info)
		{
			((void (*)(jsvm_vm*, vm_heap_stat*))JSVMApiManager::m_APIList[jsvm_api_heap_stat].pFunc)(vm, stat_info);
		}
		inline bool jsvm_heap_dump(jsvm_vm* vm, const char* dump_file_name)
		{
			return ((bool (*)(jsvm_vm*, const char*))JSVMApiManager::m_APIList[jsvm_api_heap_dump].pFunc)(vm, dump_file_name);
		}
		inline void jsvm_gc(jsvm_vm* vm, int level, bool full_gc)
		{
			((void (*)(jsvm_vm*, int, bool))JSVMApiManager::m_APIList[jsvm_api_gc].pFunc)(vm, level, full_gc);
		}

		inline void jsvm_set_console_log_channel(jsvm_vm* vm, unsigned int LogChannel)
		{
			((void (*)(jsvm_vm*, unsigned int))JSVMApiManager::m_APIList[jsvm_api_set_console_log_channel].pFunc)(vm, LogChannel);
		}
#else



		JSVM_DLL_EXPORT bool jsvm_platform_init(LPCSTR params);
		JSVM_DLL_EXPORT void jsvm_platform_dispose();

		JSVM_DLL_EXPORT jsvm_vm* jsvm_create_vm(LPCSTR script_root_dir, LPCSTR params = NULL, UINT Flags = 0, child_vm_setting* child_setting = NULL, LPCSTR init_script = NULL, size_t script_len = 0);
		JSVM_DLL_EXPORT bool jsvm_vm_dispose(jsvm_vm* vm);

		JSVM_DLL_EXPORT jsvm_handle_scope* jsvm_create_handle_scope(jsvm_vm* vm);
		JSVM_DLL_EXPORT jsvm_context* jsvm_get_context(jsvm_handle_scope* scope);
		JSVM_DLL_EXPORT bool jsvm_release_handle_scope(jsvm_handle_scope* scope);

		JSVM_DLL_EXPORT jsvm_value* jsvm_run_script(jsvm_handle_scope* scope, LPCSTR script, size_t script_len = 0, LPCSTR origin = NULL, LPCSTR source_map_url = NULL);
		JSVM_DLL_EXPORT jsvm_value* jsvm_load_module(jsvm_handle_scope* scope, LPCSTR script, size_t script_len = 0, LPCSTR origin = NULL, LPCSTR source_map_url = NULL);
		JSVM_DLL_EXPORT jsvm_value* jsvm_call(jsvm_context* context, jsvm_value* func, jsvm_value* this_obj = NULL, jsvm_value** args = NULL, int argc = 0);
		JSVM_DLL_EXPORT int jsvm_event_loop(jsvm_handle_scope* scope, int max_loop_count = 1);

		JSVM_DLL_EXPORT jsvm_persistent_value* jsvm_create_persistent_value(jsvm_context* context, jsvm_value* value);
		JSVM_DLL_EXPORT jsvm_value* jsvm_get_value_from_persistent_value(jsvm_context* context, jsvm_persistent_value* value);
		JSVM_DLL_EXPORT bool jsvm_release_persistent_value(jsvm_persistent_value* value);

		JSVM_DLL_EXPORT jsvm_value* jsvm_create_null(jsvm_context* context);
		JSVM_DLL_EXPORT jsvm_value* jsvm_create_undefined(jsvm_context* context);
		JSVM_DLL_EXPORT jsvm_value* jsvm_create_boolean(jsvm_context* context, bool value);
		JSVM_DLL_EXPORT jsvm_value* jsvm_create_int32(jsvm_context* context, int value);
		JSVM_DLL_EXPORT jsvm_value* jsvm_create_uint32(jsvm_context* context, unsigned int value);
		JSVM_DLL_EXPORT jsvm_value* jsvm_create_int64(jsvm_context* context, INT64 value);
		JSVM_DLL_EXPORT jsvm_value* jsvm_create_uint64(jsvm_context* context, UINT64 value);
		JSVM_DLL_EXPORT jsvm_value* jsvm_create_double(jsvm_context* context, double value);
		JSVM_DLL_EXPORT jsvm_value* jsvm_create_string_utf8(jsvm_context* context, const char* str, size_t length = 0);
		JSVM_DLL_EXPORT jsvm_value* jsvm_create_string_gbk(jsvm_context* context, const char* str, size_t length = 0);
		JSVM_DLL_EXPORT jsvm_value* jsvm_create_string_utf16(jsvm_context* context, const WCHAR* str, size_t length = 0);
		JSVM_DLL_EXPORT jsvm_value* jsvm_create_binary_empty(jsvm_context* context, size_t length);
		JSVM_DLL_EXPORT jsvm_value* jsvm_create_binary(jsvm_context* context, void* data, size_t length);
		JSVM_DLL_EXPORT jsvm_value* jsvm_create_binary_copy_data(jsvm_context* context, const void* data, size_t length);
		JSVM_DLL_EXPORT jsvm_value* jsvm_create_array(jsvm_context* context);
		JSVM_DLL_EXPORT jsvm_value* jsvm_create_func(jsvm_context* context, jsvm_tmpl* tmpl);
		JSVM_DLL_EXPORT jsvm_value* jsvm_create_obj(jsvm_context* context);
		JSVM_DLL_EXPORT jsvm_value* jsvm_new_obj(jsvm_context* context, jsvm_value* class_func, jsvm_value** args = NULL, int argc = 0);

		JSVM_DLL_EXPORT bool jsvm_get_value_bool(jsvm_context* context, jsvm_value* value);
		JSVM_DLL_EXPORT int jsvm_get_value_int32(jsvm_context* context, jsvm_value* value);
		JSVM_DLL_EXPORT unsigned int jsvm_get_value_uint32(jsvm_context* context, jsvm_value* value);
		JSVM_DLL_EXPORT INT64 jsvm_get_value_int64(jsvm_context* context, jsvm_value* value);
		JSVM_DLL_EXPORT UINT64 jsvm_get_value_uint64(jsvm_context* context, jsvm_value* value);
		JSVM_DLL_EXPORT double jsvm_get_value_double(jsvm_context* context, jsvm_value* value);
		JSVM_DLL_EXPORT int jsvm_get_value_string_utf8(jsvm_context* context, jsvm_value* value, char* buf, int bufsize);
		JSVM_DLL_EXPORT const char* jsvm_get_value_string_utf8_no_buff(jsvm_context* context, jsvm_value* value);
		JSVM_DLL_EXPORT int jsvm_get_value_string_gbk(jsvm_context* context, jsvm_value* value, char* buf, int bufsize);
		JSVM_DLL_EXPORT const char* jsvm_get_value_string_gbk_no_buff(jsvm_context* context, jsvm_value* value);
		JSVM_DLL_EXPORT int jsvm_get_value_string_utf16(jsvm_context* context, jsvm_value* value, WCHAR* buf, int bufsize);
		JSVM_DLL_EXPORT const WCHAR* jsvm_get_value_string_utf16_no_buff(jsvm_context* context, jsvm_value* value);
		JSVM_DLL_EXPORT void* jsvm_get_value_binary(jsvm_context* context, jsvm_value* value, size_t* len);
		JSVM_DLL_EXPORT unsigned int jsvm_get_array_length(jsvm_context* context, jsvm_value* value);

		JSVM_DLL_EXPORT int jsvm_get_type(jsvm_context* context, jsvm_value* value, char* buff, int bufsize);
		JSVM_DLL_EXPORT const char* jsvm_get_type_no_buff(jsvm_context* context, jsvm_value* value);
		JSVM_DLL_EXPORT bool jsvm_is_null(jsvm_value* value);
		JSVM_DLL_EXPORT bool jsvm_is_undefined(jsvm_value* value);
		JSVM_DLL_EXPORT bool jsvm_is_boolean(jsvm_value* value);
		JSVM_DLL_EXPORT bool jsvm_is_int32(jsvm_value* value);
		JSVM_DLL_EXPORT bool jsvm_is_uint32(jsvm_value* value);
		JSVM_DLL_EXPORT bool jsvm_is_int64(jsvm_value* value);
		JSVM_DLL_EXPORT bool jsvm_is_uint64(jsvm_value* value);
		JSVM_DLL_EXPORT bool jsvm_is_double(jsvm_value* value);
		JSVM_DLL_EXPORT bool jsvm_is_string(jsvm_value* value);
		JSVM_DLL_EXPORT bool jsvm_is_object(jsvm_value* value);
		JSVM_DLL_EXPORT bool jsvm_is_function(jsvm_value* value);
		JSVM_DLL_EXPORT bool jsvm_is_binary(jsvm_value* value);
		JSVM_DLL_EXPORT bool jsvm_is_array(jsvm_value* value);
		JSVM_DLL_EXPORT bool jsvm_is_promise(jsvm_value* value);
		JSVM_DLL_EXPORT bool jsvm_is_class(jsvm_context* context, jsvm_value* value, const char* class_name);
		JSVM_DLL_EXPORT bool jsvm_of_class(jsvm_context* context, jsvm_value* value, const char* class_name);

		JSVM_DLL_EXPORT int jsvm_value_serialize(jsvm_context* context, jsvm_value** values, int value_count, CEasyBuffer& Buffer);
		JSVM_DLL_EXPORT int jsvm_value_deserialize(jsvm_context* context, const void* data, size_t data_len, jsvm_value** values, int value_count);


		JSVM_DLL_EXPORT jsvm_tmpl* jsvm_create_func_tmpl(jsvm_context* context, jsvm_callback func, jsvm_func_type func_type, void* pass_data, const char* class_name);

		JSVM_DLL_EXPORT jsvm_persistent_tmpl* jsvm_create_persistent_tmpl(jsvm_context* context, jsvm_tmpl* tmpl);
		JSVM_DLL_EXPORT jsvm_tmpl* jsvm_get_tmpl_from_persistent_tmpl(jsvm_context* context, jsvm_persistent_tmpl* tmpl);
		JSVM_DLL_EXPORT bool jsvm_release_persistent_tmpl(jsvm_context* context, jsvm_persistent_tmpl* tmpl);


		JSVM_DLL_EXPORT bool jsvm_class_set_internal_field_count(jsvm_tmpl* constructor, int field_count);
		JSVM_DLL_EXPORT bool jsvm_class_set_super_class(jsvm_tmpl* constructor, jsvm_tmpl* super_constructor);
		JSVM_DLL_EXPORT bool jsvm_class_set_method(jsvm_context* context, jsvm_tmpl* constructor, const char* method_name, jsvm_tmpl* method, bool is_static);
		JSVM_DLL_EXPORT bool jsvm_class_set_property(jsvm_context* context, jsvm_tmpl* constructor, const char* prop_name, jsvm_tmpl* getter, jsvm_tmpl* setter, bool is_static);
		JSVM_DLL_EXPORT bool jsvm_class_set_member(jsvm_context* context, jsvm_tmpl* constructor, const char* member_name, jsvm_value* member, bool is_static);


		JSVM_DLL_EXPORT jsvm_value* jsvm_global(jsvm_context* context);
		JSVM_DLL_EXPORT bool jsvm_global_set(jsvm_context* context, const char* member_name, jsvm_value* value);
		JSVM_DLL_EXPORT jsvm_value* jsvm_global_get(jsvm_context* context, const char* member_name);
		JSVM_DLL_EXPORT bool jsvm_obj_set(jsvm_context* context, jsvm_value* obj, const char* member_name, jsvm_value* value);
		JSVM_DLL_EXPORT jsvm_value* jsvm_obj_get(jsvm_context* context, jsvm_value* obj, const char* member_name);
		JSVM_DLL_EXPORT bool jsvm_obj_set_by_index(jsvm_context* context, jsvm_value* obj, UINT index, jsvm_value* value);
		JSVM_DLL_EXPORT jsvm_value* jsvm_obj_get_by_index(jsvm_context* context, jsvm_value* obj, UINT index);
		JSVM_DLL_EXPORT bool jsvm_obj_set_internal_field(jsvm_context* context, jsvm_value* obj, int field, const void* data);
		JSVM_DLL_EXPORT const void* jsvm_obj_get_internal_field(jsvm_context* context, jsvm_value* obj, int field);
		JSVM_DLL_EXPORT jsvm_value* jsvm_get_obj_keys(jsvm_context* context, jsvm_value* obj, bool include_prototypes);
		JSVM_DLL_EXPORT jsvm_value* jsvm_get_class_name(jsvm_context* context, jsvm_value* obj);
		JSVM_DLL_EXPORT jsvm_value* jsvm_class_get_name(jsvm_context* context, jsvm_value* constructor);

		JSVM_DLL_EXPORT jsvm_value* jsvm_callback_get_this(jsvm_callback_info* callback_info);
		JSVM_DLL_EXPORT int jsvm_callback_get_arg_len(jsvm_callback_info* callback_info);
		JSVM_DLL_EXPORT jsvm_value* jsvm_callback_get_arg(jsvm_callback_info* callback_info, int index);
		JSVM_DLL_EXPORT bool jsvm_callback_set_return(jsvm_callback_info* callback_info, jsvm_value* value);
		JSVM_DLL_EXPORT jsvm_context* jsvm_callback_get_cur_context(jsvm_callback_info* callback_info);
		JSVM_DLL_EXPORT void* jsvm_callback_get_pass_data(jsvm_callback_info* callback_info);

		JSVM_DLL_EXPORT jsvm_value* jsvm_create_resolver(jsvm_context* context);
		JSVM_DLL_EXPORT jsvm_value* jsvm_get_promise(jsvm_value* resolver);
		JSVM_DLL_EXPORT bool jsvm_resolver_resolve(jsvm_context* context, jsvm_value* resolver, jsvm_value* return_value);
		JSVM_DLL_EXPORT bool jsvm_resolver_reject(jsvm_context* context, jsvm_value* resolver, jsvm_value* return_value);
		JSVM_DLL_EXPORT bool jsvm_set_promise_then(jsvm_context* context, jsvm_value* promise, jsvm_value* callback_func);

		JSVM_DLL_EXPORT bool jsvm_process_exception(jsvm_handle_scope* scope);
		JSVM_DLL_EXPORT bool jsvm_get_exception(jsvm_handle_scope* scope, char* msg_buff, size_t buff_len);
		JSVM_DLL_EXPORT void jsvm_throw_exception(jsvm_context* context, const char* error_msg);
		JSVM_DLL_EXPORT void jsvm_throw_exception_with_format(jsvm_context* context, const char* error_msg, ...);
		JSVM_DLL_EXPORT void jsvm_throw_exception_with_format_vl(jsvm_context* context, const char* error_msg, va_list	vl);

		JSVM_DLL_EXPORT void jsvm_heap_stat(jsvm_vm* vm, vm_heap_stat* stat_info);
		JSVM_DLL_EXPORT bool jsvm_heap_dump(jsvm_vm* vm, const char* dump_file_name);
		JSVM_DLL_EXPORT void jsvm_gc(jsvm_vm* vm, int level, bool full_gc);

		JSVM_DLL_EXPORT void jsvm_set_console_log_channel(jsvm_vm* vm, unsigned int LogChannel);
#endif
	}
}
