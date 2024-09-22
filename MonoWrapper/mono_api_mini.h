#pragma once



#define METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK        0x0007
#define METHOD_ATTRIBUTE_COMPILER_CONTROLLED       0x0000
#define METHOD_ATTRIBUTE_PRIVATE                   0x0001
#define METHOD_ATTRIBUTE_FAM_AND_ASSEM             0x0002
#define METHOD_ATTRIBUTE_ASSEM                     0x0003
#define METHOD_ATTRIBUTE_FAMILY                    0x0004
#define METHOD_ATTRIBUTE_FAM_OR_ASSEM              0x0005
#define METHOD_ATTRIBUTE_PUBLIC                    0x0006

#define METHOD_ATTRIBUTE_STATIC                    0x0010
#define METHOD_ATTRIBUTE_FINAL                     0x0020
#define METHOD_ATTRIBUTE_VIRTUAL                   0x0040
#define METHOD_ATTRIBUTE_HIDE_BY_SIG               0x0080

#define METHOD_ATTRIBUTE_VTABLE_LAYOUT_MASK        0x0100
#define METHOD_ATTRIBUTE_REUSE_SLOT                0x0000
#define METHOD_ATTRIBUTE_NEW_SLOT                  0x0100

#define METHOD_ATTRIBUTE_STRICT                    0x0200
#define METHOD_ATTRIBUTE_ABSTRACT                  0x0400
#define METHOD_ATTRIBUTE_SPECIAL_NAME              0x0800

#define METHOD_ATTRIBUTE_PINVOKE_IMPL              0x2000
#define METHOD_ATTRIBUTE_UNMANAGED_EXPORT          0x0008

#define METHOD_ATTRIBUTE_REQSECOBJ                 0x8000

//#ifndef _UINTPTR_T_DEFINED
//#define _UINTPTR_T_DEFINED
//#ifdef _WIN64
//typedef unsigned __int64  uintptr_t;
//#else
//typedef unsigned int uintptr_t;
//#endif
//#endif

#define MONO_FALSE 0
#define MONO_TRUE 1

typedef int				mono_bool;
typedef unsigned char	mono_byte;
typedef mono_byte       MonoBoolean;
#ifdef _WIN32
typedef wchar_t 		mono_unichar2;
#else
typedef unsigned short	mono_unichar2;
#endif
typedef unsigned int	mono_unichar4;

typedef struct _MonoDomain MonoDomain;
typedef struct _MonoJitInfo MonoJitInfo;

typedef struct _MonoAssembly MonoAssembly;
typedef struct _MonoAssemblyName MonoAssemblyName;
typedef struct _MonoTableInfo MonoTableInfo;

typedef struct _MonoClass MonoClass;
typedef struct _MonoImage MonoImage;
typedef struct _MonoMethod MonoMethod;

typedef struct _MonoObject MonoObject;
typedef struct _MonoException MonoException;
typedef struct _MonoReflectionAssembly MonoReflectionAssembly;
typedef struct _MonoReflectionTypeBuilder MonoReflectionTypeBuilder;

typedef struct MonoVTable MonoVTable;

typedef struct _MonoClassField MonoClassField;
typedef struct _MonoProperty MonoProperty;
typedef struct _MonoEvent MonoEvent;

typedef struct _MonoString MonoString;
typedef struct _MonoArray MonoArray;

typedef union MonoErrorExternal MonoError;

typedef struct _MonoMethodSignature MonoMethodSignature;
typedef struct _MonoType MonoType;

typedef struct _MonoThread MonoThread;

typedef enum {
	MONO_IMAGE_OK,
	MONO_IMAGE_ERROR_ERRNO,
	MONO_IMAGE_MISSING_ASSEMBLYREF,
	MONO_IMAGE_IMAGE_INVALID
} MonoImageOpenStatus;

typedef struct {
	const char* name;
	const unsigned char* data;
	unsigned int size;
} MonoBundledAssembly;

typedef enum {
	MONO_DEBUG_FORMAT_NONE,
	MONO_DEBUG_FORMAT_MONO,
	/* Deprecated, the mdb debugger is not longer supported. */
	MONO_DEBUG_FORMAT_DEBUGGER
} MonoDebugFormat;

typedef enum {
	MONO_TYPE_NAME_FORMAT_IL,
	MONO_TYPE_NAME_FORMAT_REFLECTION,
	MONO_TYPE_NAME_FORMAT_FULL_NAME,
	MONO_TYPE_NAME_FORMAT_ASSEMBLY_QUALIFIED
} MonoTypeNameFormat;

typedef struct {
	const char* name;
	void* pFunc;
} MonoApiInfo;

typedef enum {
	mono_api_trace_set_log_handler,
	mono_api_trace_set_print_handler,
	mono_api_trace_set_printerr_handler,

	mono_api_set_assemblies_path,
	mono_api_assembly_setrootdir,	
	mono_api_set_config_dir,
	mono_api_config_parse,
	mono_api_debugger_agent_parse_options,
	mono_api_debug_init,
	mono_api_debug_open_image_from_memory,

	mono_api_jit_parse_options,
	mono_api_trace_set_level_string,	
	mono_api_jit_init_version,
	mono_api_jit_cleanup,

	mono_api_domain_create_appdomain,
	mono_api_domain_set,
	mono_api_domain_assembly_open,
	mono_api_assembly_get_image,
	mono_api_assembly_close,
	mono_api_assembly_foreach,
	mono_api_domain_unload,
	mono_api_domain_try_unload,

	mono_api_assembly_get_name,
	mono_api_stringify_assembly_name,
	mono_api_assembly_name_get_name,
	mono_api_assembly_name_get_culture,
	mono_api_assembly_name_get_version,

	mono_api_add_internal_call,

	mono_api_object_get_class,
	mono_api_class_get_element_class,
	mono_api_class_get_type_token,

	mono_api_class_from_name,
	mono_api_class_get_name,
	mono_api_class_get_namespace,
	mono_api_class_get_parent,

	mono_api_class_get_fields,
	mono_api_class_get_methods,
	mono_api_class_get_properties,
	mono_api_class_get_events,
	mono_api_class_num_fields,
	mono_api_class_num_methods,
	mono_api_class_num_properties,
	mono_api_class_num_events,
	mono_api_method_get_name,
	mono_api_field_get_name,
	mono_api_property_get_name,
	mono_api_event_get_name,

	mono_api_field_get_type,
	mono_api_field_get_value,

	mono_api_method_signature,
	mono_api_signature_get_return_type,
	mono_api_signature_get_params,
	mono_api_signature_get_param_count,

	mono_api_type_get_name_full,
	mono_api_type_get_name,

	mono_api_class_get_field_from_name,
	mono_api_class_get_method_from_name,
	mono_api_class_get_method_from_name_flags,

	mono_api_string_length,
	mono_api_string_to_utf8,
	mono_api_string_to_utf8_checked,
	mono_api_string_to_utf16,
	mono_api_string_chars,
	mono_api_string_new,
	mono_api_string_new_wrapper,
	mono_api_string_new_len,
	mono_api_string_new_utf16,
	mono_api_string_from_utf16,
	mono_api_string_empty,

	mono_api_object_to_string,

	mono_api_object_new,
	mono_api_object_get_size,

	mono_api_array_new,
	mono_api_array_new_full,
	mono_api_array_length,
	mono_api_array_addr_with_size,

	mono_api_free,

	mono_api_get_object_class,
	mono_api_get_byte_class,
	mono_api_get_void_class,
	mono_api_get_boolean_class,
	mono_api_get_sbyte_class,
	mono_api_get_int16_class,
	mono_api_get_uint16_class,
	mono_api_get_int32_class,
	mono_api_get_uint32_class,
	mono_api_get_intptr_class,
	mono_api_get_uintptr_class,
	mono_api_get_int64_class,
	mono_api_get_uint64_class,
	mono_api_get_single_class,
	mono_api_get_double_class,
	mono_api_get_char_class,
	mono_api_get_string_class,
	mono_api_get_enum_class,
	mono_api_get_array_class,
	mono_api_get_thread_class,
	mono_api_get_exception_class,

	mono_api_gc_collect,
	mono_api_gc_max_generation,
	mono_api_gc_get_used_size,
	mono_api_gc_get_heap_size,

	mono_api_runtime_invoke,

	mono_api_value_box,
	mono_api_object_unbox,

	mono_api_gchandle_new,
	mono_api_gchandle_new_weakref,
	mono_api_gchandle_get_target,
	mono_api_gchandle_free,

	mono_api_thread_attach,
	mono_api_thread_current,
	mono_api_thread_detach,

	mono_api_max
} MonoApi;


template<class T>
class BaseMonoApiManager
{
protected:
	static HMODULE	m_hModule;
public:
	static MonoApiInfo m_APIList[mono_api_max];
};
template<class T>
HMODULE BaseMonoApiManager<T>::m_hModule = NULL;

template<class T>
MonoApiInfo BaseMonoApiManager<T>::m_APIList[mono_api_max] = {
	{"mono_trace_set_log_handler",NULL},
	{"mono_trace_set_print_handler",NULL},
	{"mono_trace_set_printerr_handler",NULL},

	{"mono_set_assemblies_path",NULL},
	{"mono_assembly_setrootdir",NULL},	
	{"mono_set_config_dir",NULL},
	{"mono_config_parse",NULL},
	{"mono_debugger_agent_parse_options",NULL},
	{"mono_debug_init",NULL},
	{"mono_debug_open_image_from_memory",NULL},

	{"mono_jit_parse_options", NULL },
	{"mono_trace_set_level_string", NULL },	
	{"mono_jit_init_version",NULL},
	{"mono_jit_cleanup",NULL},

	{"mono_domain_create_appdomain",NULL},
	{"mono_domain_set",NULL},
	{"mono_domain_assembly_open",NULL},
	{"mono_assembly_get_image",NULL},
	{"mono_assembly_close",NULL},
	{"mono_assembly_foreach",NULL},
	{"mono_domain_unload",NULL},
	{"mono_domain_try_unload",NULL},

	{"mono_assembly_get_name",NULL},
	{"mono_stringify_assembly_name",NULL},
	{"mono_assembly_name_get_name",NULL},
	{"mono_assembly_name_get_culture",NULL},
	{"mono_assembly_name_get_version",NULL},

	{"mono_add_internal_call",NULL},

	{"mono_object_get_class",NULL},
	{"mono_class_get_element_class",NULL},
	{"mono_class_get_type_token",NULL},

	{"mono_class_from_name",NULL},
	{"mono_class_get_name",NULL},
	{"mono_class_get_namespace",NULL},
	{"mono_class_get_parent",NULL},

	{"mono_class_get_fields",NULL},
	{"mono_class_get_methods",NULL},
	{"mono_class_get_properties",NULL},
	{"mono_class_get_events",NULL},
	{"mono_class_num_fields",NULL},
	{"mono_class_num_methods",NULL},
	{"mono_class_num_properties",NULL},
	{"mono_class_num_events",NULL},
	{"mono_method_get_name",NULL},
	{"mono_field_get_name",NULL},
	{"mono_property_get_name",NULL},
	{"mono_event_get_name",NULL},

	{"mono_field_get_type",NULL},
	{"mono_field_get_value",NULL},

	{"mono_method_signature",NULL},
	{"mono_signature_get_return_type",NULL},
	{"mono_signature_get_params",NULL},
	{"mono_signature_get_param_count",NULL},

	{"mono_type_get_name_full",NULL},
	{"mono_type_get_name",NULL},

	{"mono_class_get_field_from_name",NULL},
	{"mono_class_get_method_from_name",NULL},
	{"mono_class_get_method_from_name_flags",NULL},

	{"mono_string_length",NULL},
	{"mono_string_to_utf8",NULL},
	{"mono_string_to_utf8_checked",NULL},
	{"mono_string_to_utf16",NULL},
	{"mono_string_to_utf16",NULL},
	{"mono_string_new",NULL},
	{"mono_string_new_wrapper",NULL},
	{"mono_string_new_len",NULL},
	{"mono_string_new_utf16",NULL},
	{"mono_string_from_utf16",NULL},
	{"mono_string_empty",NULL},

	{"mono_object_to_string",NULL},

	{"mono_object_new",NULL},
	{"mono_object_get_size",NULL},

	{"mono_array_new",NULL},
	{"mono_array_new_full",NULL},
	{"mono_array_length",NULL},
	{"mono_array_addr_with_size",NULL},

	{"mono_free",NULL},

	{"mono_get_object_class",NULL},
	{"mono_get_byte_class",NULL},
	{"mono_get_void_class",NULL},
	{"mono_get_boolean_class",NULL},
	{"mono_get_sbyte_class",NULL},
	{"mono_get_int16_class",NULL},
	{"mono_get_uint16_class",NULL},
	{"mono_get_int32_class",NULL},
	{"mono_get_uint32_class",NULL},
	{"mono_get_intptr_class",NULL},
	{"mono_get_uintptr_class",NULL},
	{"mono_get_int64_class",NULL},
	{"mono_get_uint64_class",NULL},
	{"mono_get_single_class",NULL},
	{"mono_get_double_class",NULL},
	{"mono_get_char_class",NULL},
	{"mono_get_string_class",NULL},
	{"mono_get_enum_class",NULL},
	{"mono_get_array_class",NULL},
	{"mono_get_thread_class",NULL},
	{"mono_get_exception_class",NULL},

	{"mono_gc_collect",NULL},
	{"mono_gc_max_generation",NULL},
	{"mono_gc_get_used_size",NULL},
	{"mono_gc_get_heap_size",NULL},

	{"mono_runtime_invoke",NULL},

	{"mono_value_box",NULL},
	{"mono_object_unbox",NULL},

	{"mono_gchandle_new",NULL},
	{"mono_gchandle_new_weakref",NULL},
	{"mono_gchandle_get_target",NULL},
	{"mono_gchandle_free",NULL},

	{"mono_thread_attach",NULL},
	{"mono_thread_current",NULL},
	{"mono_thread_detach",NULL},
};


class MonoApiManager :public BaseMonoApiManager<MonoApiManager>
{
public:
	static bool LoadMonoApi(LPCTSTR szDllPath)
	{
		ReleaseMonoApi();
#ifdef WIN32
		m_hModule = LoadLibrary(szDllPath);
#else
		m_hModule = dlopen(szDllPath, RTLD_NOW | RTLD_GLOBAL | RTLD_DEEPBIND);
#endif
		if (m_hModule)
		{
			for (int i = 0; i < mono_api_max; i++)
			{
#ifdef WIN32
				m_APIList[i].pFunc = GetProcAddress(m_hModule, m_APIList[i].name);
#else
				m_APIList[i].pFunc = dlsym(m_hModule, m_APIList[i].name);
#endif
				if (!m_APIList[i].pFunc)
				{
					LogMono("api[%s] not exist.", m_APIList[i].name);
					return false;
				}

			}
			return true;
		}
		else
		{
			LogMono("load module[%s] failed.", szDllPath);
		}
		return false;
	}
	static void ReleaseMonoApi()
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


extern "C" {

	typedef void (*MonoPrintCallback) (const char* string, mono_bool is_stdout);
	typedef void (*MonoLogCallback) (const char* log_domain, const char* log_level, const char* message, mono_bool fatal, void* user_data);
	typedef void (*MonoFunc)	(void* data, void* user_data);

#ifdef DELAY_LOAD_MONO_API
	inline void mono_trace_set_log_handler(MonoLogCallback callback, void* user_data)
	{
		((void(*)(MonoLogCallback, void*))MonoApiManager::m_APIList[mono_api_trace_set_log_handler].pFunc)(callback, user_data);
	}
	inline void mono_trace_set_print_handler(MonoPrintCallback callback)
	{
		((void(*)(MonoPrintCallback))MonoApiManager::m_APIList[mono_api_trace_set_print_handler].pFunc)(callback);
	}
	inline void mono_trace_set_printerr_handler(MonoPrintCallback callback)
	{
		((void(*)(MonoPrintCallback))MonoApiManager::m_APIList[mono_api_trace_set_printerr_handler].pFunc)(callback);
	}

	inline void mono_set_assemblies_path(const char* path)
	{
		((void(*)(const char*))MonoApiManager::m_APIList[mono_api_set_assemblies_path].pFunc)(path);
	}
	inline void mono_assembly_setrootdir(const char* root_dir)
	{
		((void(*)(const char*))MonoApiManager::m_APIList[mono_api_assembly_setrootdir].pFunc)(root_dir);
	}
	inline void mono_set_config_dir(const char* dir)
	{
		((void(*)(const char*))MonoApiManager::m_APIList[mono_api_set_config_dir].pFunc)(dir);
	}
	inline void mono_config_parse(const char* filename)
	{
		((void(*)(const char*))MonoApiManager::m_APIList[mono_api_config_parse].pFunc)(filename);
	}
	inline void mono_debugger_agent_parse_options(char* options)
	{
		((void(*)(char*))MonoApiManager::m_APIList[mono_api_debugger_agent_parse_options].pFunc)(options);
	}
	inline void mono_debug_init(MonoDebugFormat format)
	{
		((void(*)(MonoDebugFormat))MonoApiManager::m_APIList[mono_api_debug_init].pFunc)(format);
	}
	inline void mono_debug_open_image_from_memory(MonoImage* image, const mono_byte* raw_contents, int size)
	{
		((void(*)(MonoImage*, const mono_byte*, int))MonoApiManager::m_APIList[mono_api_debug_open_image_from_memory].pFunc)(image, raw_contents, size);
	}

	inline void mono_jit_parse_options(int argc, char* argv[])
	{
		((void(*)(int, char* []))MonoApiManager::m_APIList[mono_api_jit_parse_options].pFunc)(argc, argv);
	}
	inline void mono_trace_set_level_string(const char* value)
	{
		((void(*)(const char*))MonoApiManager::m_APIList[mono_api_trace_set_level_string].pFunc)(value);
	}
	inline MonoDomain* mono_jit_init_version(const char* root_domain_name, const char* runtime_version)
	{
		return ((MonoDomain * (*)(const char*, const char*))MonoApiManager::m_APIList[mono_api_jit_init_version].pFunc)(root_domain_name, runtime_version);
	}
	inline void mono_jit_cleanup(MonoDomain* domain)
	{
		((void(*)(MonoDomain*))MonoApiManager::m_APIList[mono_api_jit_cleanup].pFunc)(domain);
	}

	inline MonoDomain* mono_domain_create_appdomain(char* friendly_name, char* configuration_file)
	{
		return ((MonoDomain * (*)(char*, char*))MonoApiManager::m_APIList[mono_api_domain_create_appdomain].pFunc)(friendly_name, configuration_file);
	}
	inline mono_bool mono_domain_set(MonoDomain* domain, mono_bool force)
	{
		return ((mono_bool(*)(MonoDomain*, mono_bool))MonoApiManager::m_APIList[mono_api_domain_set].pFunc)(domain, force);
	}
	inline MonoAssembly* mono_domain_assembly_open(MonoDomain* domain, const char* name)
	{
		return ((MonoAssembly * (*)(MonoDomain*, const char*))MonoApiManager::m_APIList[mono_api_domain_assembly_open].pFunc)(domain, name);
	}
	inline MonoImage* mono_assembly_get_image(MonoAssembly* assembly)
	{
		return ((MonoImage * (*)(MonoAssembly*))MonoApiManager::m_APIList[mono_api_assembly_get_image].pFunc)(assembly);
	}
	inline void mono_assembly_close(MonoAssembly* assembly)
	{
		((void(*)(MonoAssembly*))MonoApiManager::m_APIList[mono_api_assembly_close].pFunc)(assembly);
	}
	inline void mono_assembly_foreach(MonoFunc func, void* user_data)
	{
		((void(*)(MonoFunc, void*))MonoApiManager::m_APIList[mono_api_assembly_foreach].pFunc)(func, user_data);
	}
	inline void mono_domain_unload(MonoDomain* domain)
	{
		((void(*)(MonoDomain*))MonoApiManager::m_APIList[mono_api_domain_unload].pFunc)(domain);
	}
	inline void mono_domain_try_unload(MonoDomain* domain, MonoObject** exc)
	{
		((void(*)(MonoDomain*, MonoObject**))MonoApiManager::m_APIList[mono_api_domain_try_unload].pFunc)(domain, exc);
	}

	inline MonoAssemblyName* mono_assembly_get_name(MonoAssembly* assembly)
	{
		return ((MonoAssemblyName * (*)(MonoAssembly*))MonoApiManager::m_APIList[mono_api_assembly_get_name].pFunc)(assembly);
	}
	inline char* mono_stringify_assembly_name(MonoAssemblyName* aname)
	{
		return ((char* (*)(MonoAssemblyName*))MonoApiManager::m_APIList[mono_api_stringify_assembly_name].pFunc)(aname);
	}
	inline const char* mono_assembly_name_get_name(MonoAssemblyName* aname)
	{
		return ((const char* (*)(MonoAssemblyName*))MonoApiManager::m_APIList[mono_api_assembly_name_get_name].pFunc)(aname);
	}
	inline const char* mono_assembly_name_get_culture(MonoAssemblyName* aname)
	{
		return ((const char* (*)(MonoAssemblyName*))MonoApiManager::m_APIList[mono_api_assembly_name_get_culture].pFunc)(aname);
	}
	inline unsigned short mono_assembly_name_get_version(MonoAssemblyName* aname, unsigned short* minor, unsigned short* build, unsigned short* revision)
	{
		return ((unsigned short (*)(MonoAssemblyName*, unsigned short*, unsigned short*, unsigned short*))MonoApiManager::m_APIList[mono_api_assembly_name_get_version].pFunc)(aname, minor, build, revision);
	}

	inline void  mono_add_internal_call(const char* name, const void* method)
	{
		((void(*)(const char*, const void*))MonoApiManager::m_APIList[mono_api_add_internal_call].pFunc)(name, method);
	}

	inline MonoClass* mono_object_get_class(MonoObject* obj)
	{
		return ((MonoClass * (*)(MonoObject*))MonoApiManager::m_APIList[mono_api_object_get_class].pFunc)(obj);
	}
	inline MonoClass* mono_class_get_element_class(MonoClass* klass)
	{
		return ((MonoClass * (*)(MonoClass*))MonoApiManager::m_APIList[mono_api_class_get_element_class].pFunc)(klass);
	}
	inline unsigned int mono_class_get_type_token(MonoClass* klass)
	{
		return ((unsigned int (*)(MonoClass*))MonoApiManager::m_APIList[mono_api_class_get_type_token].pFunc)(klass);
	}

	inline MonoClass* mono_class_from_name(MonoImage* image, const char* name_space, const char* name)
	{
		return ((MonoClass * (*)(MonoImage*, const char*, const char*))MonoApiManager::m_APIList[mono_api_class_from_name].pFunc)(image, name_space, name);
	}
	inline const char* mono_class_get_name(MonoClass* klass)
	{
		return ((const char* (*)(MonoClass*))MonoApiManager::m_APIList[mono_api_class_get_name].pFunc)(klass);
	}
	inline const char* mono_class_get_namespace(MonoClass* klass)
	{
		return ((const char* (*)(MonoClass*))MonoApiManager::m_APIList[mono_api_class_get_namespace].pFunc)(klass);
	}
	inline MonoClass* mono_class_get_parent(MonoClass* klass)
	{
		return ((MonoClass * (*)(MonoClass*))MonoApiManager::m_APIList[mono_api_class_get_parent].pFunc)(klass);
	}

	inline MonoClassField* mono_class_get_fields(MonoClass* klass, void** iter)
	{
		return ((MonoClassField * (*)(MonoClass*, void**))MonoApiManager::m_APIList[mono_api_class_get_fields].pFunc)(klass, iter);
	}
	inline MonoMethod* mono_class_get_methods(MonoClass* klass, void** iter)
	{
		return ((MonoMethod * (*)(MonoClass*, void**))MonoApiManager::m_APIList[mono_api_class_get_methods].pFunc)(klass, iter);
	}
	inline MonoProperty* mono_class_get_properties(MonoClass* klass, void** iter)
	{
		return ((MonoProperty * (*)(MonoClass*, void**))MonoApiManager::m_APIList[mono_api_class_get_properties].pFunc)(klass, iter);
	}
	inline MonoEvent* mono_class_get_events(MonoClass* klass, void** iter)
	{
		return ((MonoEvent * (*)(MonoClass*, void**))MonoApiManager::m_APIList[mono_api_class_get_events].pFunc)(klass, iter);
	}
	inline int	mono_class_num_fields(MonoClass* klass)
	{
		return ((int (*)(MonoClass*))MonoApiManager::m_APIList[mono_api_class_num_fields].pFunc)(klass);
	}
	inline int	mono_class_num_methods(MonoClass* klass)
	{
		return ((int (*)(MonoClass*))MonoApiManager::m_APIList[mono_api_class_num_methods].pFunc)(klass);
	}
	inline int	mono_class_num_properties(MonoClass* klass)
	{
		return ((int (*)(MonoClass*))MonoApiManager::m_APIList[mono_api_class_num_properties].pFunc)(klass);
	}
	inline int	mono_class_num_events(MonoClass* klass)
	{
		return ((int (*)(MonoClass*))MonoApiManager::m_APIList[mono_api_class_num_events].pFunc)(klass);
	}
	inline const char* mono_method_get_name(MonoMethod* method)
	{
		return ((const char* (*)(MonoMethod*))MonoApiManager::m_APIList[mono_api_method_get_name].pFunc)(method);
	}
	inline const char* mono_field_get_name(MonoClassField* field)
	{
		return ((const char* (*)(MonoClassField*))MonoApiManager::m_APIList[mono_api_field_get_name].pFunc)(field);
	}
	inline const char* mono_property_get_name(MonoProperty* prop)
	{
		return ((const char* (*)(MonoProperty*))MonoApiManager::m_APIList[mono_api_property_get_name].pFunc)(prop);
	}
	inline const char* mono_event_get_name(MonoEvent* event)
	{
		return ((const char* (*)(MonoEvent*))MonoApiManager::m_APIList[mono_api_event_get_name].pFunc)(event);
	}

	inline MonoType* mono_field_get_type(MonoClassField* field)
	{
		return ((MonoType * (*)(MonoClassField*))MonoApiManager::m_APIList[mono_api_field_get_type].pFunc)(field);
	}
	inline void mono_field_get_value(MonoObject* obj, MonoClassField* field, void* value)
	{
		((void(*)(MonoObject*, MonoClassField*, void*))MonoApiManager::m_APIList[mono_api_field_get_value].pFunc)(obj, field, value);
	}

	inline MonoMethodSignature* mono_method_signature(MonoMethod* method)
	{
		return ((MonoMethodSignature * (*)(MonoMethod*))MonoApiManager::m_APIList[mono_api_method_signature].pFunc)(method);
	}
	inline MonoType* mono_signature_get_return_type(MonoMethodSignature* sig)
	{
		return ((MonoType * (*)(MonoMethodSignature*))MonoApiManager::m_APIList[mono_api_signature_get_return_type].pFunc)(sig);
	}
	inline MonoType* mono_signature_get_params(MonoMethodSignature* sig, void** iter)
	{
		return ((MonoType * (*)(MonoMethodSignature*, void**))MonoApiManager::m_APIList[mono_api_signature_get_params].pFunc)(sig, iter);
	}
	inline unsigned int mono_signature_get_param_count(MonoMethodSignature* sig)
	{
		return ((unsigned int (*)(MonoMethodSignature*))MonoApiManager::m_APIList[mono_api_signature_get_param_count].pFunc)(sig);
	}

	inline char* mono_type_get_name_full(MonoType* type, MonoTypeNameFormat format)
	{
		return ((char* (*)(MonoType*, MonoTypeNameFormat))MonoApiManager::m_APIList[mono_api_type_get_name_full].pFunc)(type, format);
	}
	inline char* mono_type_get_name(MonoType* type)
	{
		return ((char* (*)(MonoType*))MonoApiManager::m_APIList[mono_api_type_get_name].pFunc)(type);
	}

	inline MonoClassField* mono_class_get_field_from_name(MonoClass* klass, const char* name)
	{
		return ((MonoClassField * (*)(MonoClass*, const char*))MonoApiManager::m_APIList[mono_api_class_get_field_from_name].pFunc)(klass, name);
	}
	inline MonoMethod* mono_class_get_method_from_name(MonoClass* klass, const char* name, int param_count)
	{
		return ((MonoMethod * (*)(MonoClass*, const char*, int))MonoApiManager::m_APIList[mono_api_class_get_method_from_name].pFunc)(klass, name, param_count);
	}
	inline MonoMethod* mono_class_get_method_from_name_flags(MonoClass* klass, const char* name, int param_count, int flags)
	{
		return ((MonoMethod * (*)(MonoClass*, const char*, int, int))MonoApiManager::m_APIList[mono_api_class_get_method_from_name_flags].pFunc)(klass, name, param_count, flags);
	}

	inline int mono_string_length(MonoString* string_obj)
	{
		return ((int (*)(MonoString*))MonoApiManager::m_APIList[mono_api_string_length].pFunc)(string_obj);
	}
	inline char* mono_string_to_utf8(MonoString* string_obj)
	{
		return ((char* (*)(MonoString*))MonoApiManager::m_APIList[mono_api_string_to_utf8].pFunc)(string_obj);
	}
	inline char* mono_string_to_utf8_checked(MonoString* string_obj, MonoError* error)
	{
		return ((char* (*)(MonoString*, MonoError*))MonoApiManager::m_APIList[mono_api_string_to_utf8_checked].pFunc)(string_obj, error);
	}
	inline mono_unichar2* mono_string_to_utf16(MonoString* string_obj)
	{
		return ((mono_unichar2 * (*)(MonoString*))MonoApiManager::m_APIList[mono_api_string_to_utf16].pFunc)(string_obj);
	}
	inline mono_unichar2* mono_string_chars(MonoString* string_obj)
	{
		return ((mono_unichar2 * (*)(MonoString*))MonoApiManager::m_APIList[mono_api_string_chars].pFunc)(string_obj);
	}
	inline MonoString* mono_string_new(MonoDomain* domain, const char* text)
	{
		return ((MonoString * (*)(MonoDomain*, const char*))MonoApiManager::m_APIList[mono_api_string_new].pFunc)(domain, text);
	}
	inline MonoString* mono_string_new_wrapper(const char* text)
	{
		return ((MonoString * (*)(const char*))MonoApiManager::m_APIList[mono_api_string_new_wrapper].pFunc)(text);
	}
	inline MonoString* mono_string_new_len(MonoDomain* domain, const char* text, unsigned int length)
	{
		return ((MonoString * (*)(MonoDomain*, const char*, unsigned int))MonoApiManager::m_APIList[mono_api_string_new_len].pFunc)(domain, text, length);
	}
	inline MonoString* mono_string_new_utf16(MonoDomain* domain, const mono_unichar2* text, int len)
	{
		return ((MonoString * (*)(MonoDomain*, const mono_unichar2*, int))MonoApiManager::m_APIList[mono_api_string_new_utf16].pFunc)(domain, text, len);
	}
	inline MonoString* mono_string_from_utf16(const mono_unichar2* data)
	{
		return ((MonoString * (*)(const mono_unichar2*))MonoApiManager::m_APIList[mono_api_string_from_utf16].pFunc)(data);
	}
	inline MonoString* mono_string_empty(MonoDomain* domain)
	{
		return ((MonoString * (*)(MonoDomain*))MonoApiManager::m_APIList[mono_api_string_empty].pFunc)(domain);
	}

	inline MonoString* mono_object_to_string(MonoObject* obj, MonoObject** exc)
	{
		return ((MonoString * (*)(MonoObject*, MonoObject**))MonoApiManager::m_APIList[mono_api_object_to_string].pFunc)(obj, exc);
	}

	inline MonoObject* mono_object_new(MonoDomain* domain, MonoClass* klass)
	{
		return ((MonoObject * (*)(MonoDomain*, MonoClass*))MonoApiManager::m_APIList[mono_api_object_new].pFunc)(domain, klass);
	}
	inline unsigned int mono_object_get_size(MonoObject* o)
	{
		return ((unsigned int (*)(MonoObject*))MonoApiManager::m_APIList[mono_api_object_get_size].pFunc)(o);
	}

	inline MonoArray* mono_array_new(MonoDomain* domain, MonoClass* eclass, uintptr_t n)
	{
		return ((MonoArray * (*)(MonoDomain*, MonoClass*, uintptr_t))MonoApiManager::m_APIList[mono_api_array_new].pFunc)(domain, eclass, n);
	}
	inline MonoArray* mono_array_new_full(MonoDomain* domain, MonoClass* array_class, uintptr_t* lengths, intptr_t* lower_bounds)
	{
		return ((MonoArray * (*)(MonoDomain*, MonoClass*, uintptr_t*, intptr_t*))MonoApiManager::m_APIList[mono_api_array_new_full].pFunc)(domain, array_class, lengths, lower_bounds);
	}
	inline uintptr_t mono_array_length(MonoArray* array)
	{
		return ((uintptr_t(*)(MonoArray*))MonoApiManager::m_APIList[mono_api_array_length].pFunc)(array);
	}
	inline char* mono_array_addr_with_size(MonoArray* array, int size, uintptr_t idx)
	{
		return ((char* (*)(MonoArray*, int, uintptr_t))MonoApiManager::m_APIList[mono_api_array_addr_with_size].pFunc)(array, size, idx);
	}

	inline void mono_free(void* p)
	{
		((void(*)(void*))MonoApiManager::m_APIList[mono_api_free].pFunc)(p);
	}

	inline MonoClass* mono_get_object_class(void)
	{
		return ((MonoClass * (*)(void))MonoApiManager::m_APIList[mono_api_get_object_class].pFunc)();
	}
	inline MonoClass* mono_get_byte_class(void)
	{
		return ((MonoClass * (*)(void))MonoApiManager::m_APIList[mono_api_get_byte_class].pFunc)();
	}
	inline MonoClass* mono_get_void_class(void)
	{
		return ((MonoClass * (*)(void))MonoApiManager::m_APIList[mono_api_get_void_class].pFunc)();
	}
	inline MonoClass* mono_get_boolean_class(void)
	{
		return ((MonoClass * (*)(void))MonoApiManager::m_APIList[mono_api_get_boolean_class].pFunc)();
	}
	inline MonoClass* mono_get_sbyte_class(void)
	{
		return ((MonoClass * (*)(void))MonoApiManager::m_APIList[mono_api_get_sbyte_class].pFunc)();
	}
	inline MonoClass* mono_get_int16_class(void)
	{
		return ((MonoClass * (*)(void))MonoApiManager::m_APIList[mono_api_get_int16_class].pFunc)();
	}
	inline MonoClass* mono_get_uint16_class(void)
	{
		return ((MonoClass * (*)(void))MonoApiManager::m_APIList[mono_api_get_uint16_class].pFunc)();
	}
	inline MonoClass* mono_get_int32_class(void)
	{
		return ((MonoClass * (*)(void))MonoApiManager::m_APIList[mono_api_get_int32_class].pFunc)();
	}
	inline MonoClass* mono_get_uint32_class(void)
	{
		return ((MonoClass * (*)(void))MonoApiManager::m_APIList[mono_api_get_uint32_class].pFunc)();
	}
	inline MonoClass* mono_get_intptr_class(void)
	{
		return ((MonoClass * (*)(void))MonoApiManager::m_APIList[mono_api_get_intptr_class].pFunc)();
	}
	inline MonoClass* mono_get_uintptr_class(void)
	{
		return ((MonoClass * (*)(void))MonoApiManager::m_APIList[mono_api_get_uintptr_class].pFunc)();
	}
	inline MonoClass* mono_get_int64_class(void)
	{
		return ((MonoClass * (*)(void))MonoApiManager::m_APIList[mono_api_get_int64_class].pFunc)();
	}
	inline MonoClass* mono_get_uint64_class(void)
	{
		return ((MonoClass * (*)(void))MonoApiManager::m_APIList[mono_api_get_uint64_class].pFunc)();
	}
	inline MonoClass* mono_get_single_class(void)
	{
		return ((MonoClass * (*)(void))MonoApiManager::m_APIList[mono_api_get_single_class].pFunc)();
	}
	inline MonoClass* mono_get_double_class(void)
	{
		return ((MonoClass * (*)(void))MonoApiManager::m_APIList[mono_api_get_double_class].pFunc)();
	}
	inline MonoClass* mono_get_char_class(void)
	{
		return ((MonoClass * (*)(void))MonoApiManager::m_APIList[mono_api_get_char_class].pFunc)();
	}
	inline MonoClass* mono_get_string_class(void)
	{
		return ((MonoClass * (*)(void))MonoApiManager::m_APIList[mono_api_get_string_class].pFunc)();
	}
	inline MonoClass* mono_get_enum_class(void)
	{
		return ((MonoClass * (*)(void))MonoApiManager::m_APIList[mono_api_get_enum_class].pFunc)();
	}
	inline MonoClass* mono_get_array_class(void)
	{
		return ((MonoClass * (*)(void))MonoApiManager::m_APIList[mono_api_get_array_class].pFunc)();
	}
	inline MonoClass* mono_get_thread_class(void)
	{
		return ((MonoClass * (*)(void))MonoApiManager::m_APIList[mono_api_get_thread_class].pFunc)();
	}
	inline MonoClass* mono_get_exception_class(void)
	{
		return ((MonoClass * (*)(void))MonoApiManager::m_APIList[mono_api_get_exception_class].pFunc)();
	}

	inline void mono_gc_collect(int generation)
	{
		((void(*)(int))MonoApiManager::m_APIList[mono_api_gc_collect].pFunc)(generation);
	}
	inline int mono_gc_max_generation(void)
	{
		return ((int (*)(void))MonoApiManager::m_APIList[mono_api_gc_max_generation].pFunc)();
	}
	inline long long mono_gc_get_used_size(void)
	{
		return ((long long (*)(void))MonoApiManager::m_APIList[mono_api_gc_get_used_size].pFunc)();
	}
	inline long long mono_gc_get_heap_size(void)
	{
		return ((long long (*)(void))MonoApiManager::m_APIList[mono_api_gc_get_heap_size].pFunc)();
	}

	inline MonoObject* mono_runtime_invoke(MonoMethod* method, void* obj, void** params, MonoObject** exc)
	{
		return ((MonoObject * (*)(MonoMethod*, void*, void**, MonoObject**))MonoApiManager::m_APIList[mono_api_runtime_invoke].pFunc)(method, obj, params, exc);
	}

	inline MonoObject* mono_value_box(MonoDomain* domain, MonoClass* klass, void* val)
	{
		return ((MonoObject * (*)(MonoDomain*, MonoClass*, void*))MonoApiManager::m_APIList[mono_api_value_box].pFunc)(domain, klass, val);
	}
	inline void* mono_object_unbox(MonoObject* obj)
	{
		return ((void* (*)(MonoObject*))MonoApiManager::m_APIList[mono_api_object_unbox].pFunc)(obj);
	}

	inline unsigned int mono_gchandle_new(MonoObject* obj, mono_bool pinned)
	{
		return ((unsigned int (*)(MonoObject*, mono_bool))MonoApiManager::m_APIList[mono_api_gchandle_new].pFunc)(obj, pinned);
	}
	inline unsigned int mono_gchandle_new_weakref(MonoObject* obj, mono_bool track_resurrection)
	{
		return ((unsigned int (*)(MonoObject*, mono_bool))MonoApiManager::m_APIList[mono_api_gchandle_new_weakref].pFunc)(obj, track_resurrection);
	}
	inline MonoObject* mono_gchandle_get_target(unsigned int gchandle)
	{
		return ((MonoObject * (*)(unsigned int))MonoApiManager::m_APIList[mono_api_gchandle_get_target].pFunc)(gchandle);
	}
	inline void mono_gchandle_free(unsigned int gchandle)
	{
		((void(*)(unsigned int))MonoApiManager::m_APIList[mono_api_gchandle_free].pFunc)(gchandle);
	}

	inline MonoThread* mono_thread_attach(MonoDomain* domain)
	{
		return ((MonoThread * (*)(MonoDomain*))MonoApiManager::m_APIList[mono_api_thread_attach].pFunc)(domain);
	}
	inline MonoThread* mono_thread_current(void)
	{
		return ((MonoThread * (*)(void))MonoApiManager::m_APIList[mono_api_thread_current].pFunc)();
	}
	inline void mono_thread_detach(MonoThread* thread)
	{
		((void(*)(MonoThread*))MonoApiManager::m_APIList[mono_api_thread_detach].pFunc)(thread);
	}
#else

	void mono_trace_set_log_handler(MonoLogCallback callback, void* user_data);
	void mono_trace_set_print_handler(MonoPrintCallback callback);
	void mono_trace_set_printerr_handler(MonoPrintCallback callback);

	void mono_set_assemblies_path(const char* path);
	void mono_assembly_setrootdir(const char* root_dir);
	void mono_set_config_dir(const char* dir);
	void mono_config_parse(const char* filename);
	void mono_debugger_agent_parse_options(char* options);
	void mono_debug_init(MonoDebugFormat format);
	void mono_debug_open_image_from_memory(MonoImage* image, const mono_byte* raw_contents, int size);

	void mono_jit_parse_options(int argc, char* argv[]);
	void mono_trace_set_level_string(const char* value);
	MonoDomain* mono_jit_init_version(const char* root_domain_name, const char* runtime_version);
	void mono_jit_cleanup(MonoDomain* domain);

	MonoDomain* mono_domain_create_appdomain(char* friendly_name, char* configuration_file);
	mono_bool mono_domain_set(MonoDomain* domain, mono_bool force);
	MonoAssembly* mono_domain_assembly_open(MonoDomain* domain, const char* name);
	MonoImage* mono_assembly_get_image(MonoAssembly* assembly);
	void mono_assembly_close(MonoAssembly* assembly);
	void mono_assembly_foreach(MonoFunc func, void* user_data);
	void mono_domain_unload(MonoDomain* domain);
	void mono_domain_try_unload(MonoDomain* domain, MonoObject** exc);

	MonoAssemblyName* mono_assembly_get_name(MonoAssembly* assembly);
	char* mono_stringify_assembly_name(MonoAssemblyName* aname);
	const char* mono_assembly_name_get_name(MonoAssemblyName* aname);
	const char* mono_assembly_name_get_culture(MonoAssemblyName* aname);
	unsigned short mono_assembly_name_get_version(MonoAssemblyName* aname, unsigned short* minor, unsigned short* build, unsigned short* revision);

	void  mono_add_internal_call(const char* name, const void* method);

	MonoClass* mono_object_get_class(MonoObject* obj);
	MonoClass* mono_class_get_element_class(MonoClass* klass);
	unsigned int mono_class_get_type_token(MonoClass* klass);

	MonoClass* mono_class_from_name(MonoImage* image, const char* name_space, const char* name);
	const char* mono_class_get_name(MonoClass* klass);
	const char* mono_class_get_namespace(MonoClass* klass);
	MonoClass* mono_class_get_parent(MonoClass* klass);

	MonoClassField* mono_class_get_fields(MonoClass* klass, void** iter);
	MonoMethod* mono_class_get_methods(MonoClass* klass, void** iter);
	MonoProperty* mono_class_get_properties(MonoClass* klass, void** iter);
	MonoEvent* mono_class_get_events(MonoClass* klass, void** iter);
	int	mono_class_num_fields(MonoClass* klass);
	int	mono_class_num_methods(MonoClass* klass);
	int	mono_class_num_properties(MonoClass* klass);
	int	mono_class_num_events(MonoClass* klass);
	const char* mono_method_get_name(MonoMethod* method);
	const char* mono_field_get_name(MonoClassField* field);
	const char* mono_property_get_name(MonoProperty* prop);
	const char* mono_event_get_name(MonoEvent* event);

	MonoType* mono_field_get_type(MonoClassField* field);
	void mono_field_get_value(MonoObject* obj, MonoClassField* field, void* value);

	MonoMethodSignature* mono_method_signature(MonoMethod* method);
	MonoType* mono_signature_get_return_type(MonoMethodSignature* sig);
	MonoType* mono_signature_get_params(MonoMethodSignature* sig, void** iter);
	unsigned int mono_signature_get_param_count(MonoMethodSignature* sig);

	char* mono_type_get_name_full(MonoType* type, MonoTypeNameFormat format);
	char* mono_type_get_name(MonoType* type);

	MonoClassField* mono_class_get_field_from_name(MonoClass* klass, const char* name);
	MonoMethod* mono_class_get_method_from_name(MonoClass* klass, const char* name, int param_count);
	MonoMethod* mono_class_get_method_from_name_flags(MonoClass* klass, const char* name, int param_count, int flags);

	int mono_string_length(MonoString* string_obj);
	char* mono_string_to_utf8(MonoString* string_obj);
	char* mono_string_to_utf8_checked(MonoString* string_obj, MonoError* error);
	mono_unichar2* mono_string_to_utf16(MonoString* string_obj);
	mono_unichar2* mono_string_chars(MonoString* string_obj);
	MonoString* mono_string_new(MonoDomain* domain, const char* text);
	MonoString* mono_string_new_wrapper(const char* text);
	MonoString* mono_string_new_len(MonoDomain* domain, const char* text, unsigned int length);
	MonoString* mono_string_new_utf16(MonoDomain* domain, const mono_unichar2* text, int len);
	MonoString* mono_string_from_utf16(const mono_unichar2* data);
	MonoString* mono_string_empty(MonoDomain* domain);

	MonoString* mono_object_to_string(MonoObject* obj, MonoObject** exc);

	MonoObject* mono_object_new(MonoDomain* domain, MonoClass* klass);
	unsigned int mono_object_get_size(MonoObject* o);

	MonoArray* mono_array_new(MonoDomain* domain, MonoClass* eclass, uintptr_t n);
	MonoArray* mono_array_new_full(MonoDomain* domain, MonoClass* array_class, uintptr_t* lengths, intptr_t* lower_bounds);
	uintptr_t mono_array_length(MonoArray* array);
	char* mono_array_addr_with_size(MonoArray* array, int size, uintptr_t idx);

	void mono_free(void* p);

	MonoClass* mono_get_object_class(void);
	MonoClass* mono_get_byte_class(void);
	MonoClass* mono_get_void_class(void);
	MonoClass* mono_get_boolean_class(void);
	MonoClass* mono_get_sbyte_class(void);
	MonoClass* mono_get_int16_class(void);
	MonoClass* mono_get_uint16_class(void);
	MonoClass* mono_get_int32_class(void);
	MonoClass* mono_get_uint32_class(void);
	MonoClass* mono_get_intptr_class(void);
	MonoClass* mono_get_uintptr_class(void);
	MonoClass* mono_get_int64_class(void);
	MonoClass* mono_get_uint64_class(void);
	MonoClass* mono_get_single_class(void);
	MonoClass* mono_get_double_class(void);
	MonoClass* mono_get_char_class(void);
	MonoClass* mono_get_string_class(void);
	MonoClass* mono_get_enum_class(void);
	MonoClass* mono_get_array_class(void);
	MonoClass* mono_get_thread_class(void);
	MonoClass* mono_get_exception_class(void);

	void   mono_gc_collect(int generation);
	int    mono_gc_max_generation(void);
	long long mono_gc_get_used_size(void);
	long long mono_gc_get_heap_size(void);

	MonoObject* mono_runtime_invoke(MonoMethod* method, void* obj, void** params, MonoObject** exc);

	MonoObject* mono_value_box(MonoDomain* domain, MonoClass* klass, void* val);
	void* mono_object_unbox(MonoObject* obj);

	unsigned int mono_gchandle_new(MonoObject* obj, mono_bool pinned);
	unsigned int mono_gchandle_new_weakref(MonoObject* obj, mono_bool track_resurrection);
	MonoObject* mono_gchandle_get_target(unsigned int gchandle);
	void mono_gchandle_free(unsigned int gchandle);

	MonoThread* mono_thread_attach(MonoDomain* domain);
	MonoThread* mono_thread_current(void);
	void mono_thread_detach(MonoThread* thread);


#endif
}
inline CEasyString mono_string_to_easy_string(MonoString* string_obj)
{
	CEasyString Str;
	int Len = mono_string_length(string_obj);
	if (Len)
		Str.SetString(mono_string_chars(string_obj), Len);
	return Str;
}