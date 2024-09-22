#pragma once

namespace JSVM
{
	class CCustomTypeManager :CStaticObject4<CCustomTypeManager>
	{

	};

	template<typename T>
	struct is_easy_array : std::false_type {};

	template<typename T>
	struct is_easy_array<CEasyArray<T>> : std::true_type {};

	template<typename T>
	struct easy_array_content_type;

	template<typename T>
	struct easy_array_content_type<CEasyArray<T>> {
		using type = T;
	};


	template <typename T, typename Enable = void>
	struct TypeConvertor;

	template<>
	struct TypeConvertor<jsvm_value*>
	{
		static bool IsMatch(jsvm_context* context, jsvm_value* Value)
		{
			return true;
		}
		static jsvm_value* FromJS(jsvm_context* context, jsvm_value* Value)
		{
			return Value;
		}
		static jsvm_value* ToJS(jsvm_context* context, jsvm_value* Value)
		{
			return Value;
		}
	};

	template<>
	struct TypeConvertor<bool>
	{
		static bool IsMatch(jsvm_context* context, jsvm_value* Value)
		{
			return jsvm_is_boolean(Value);
		}
		static bool FromJS(jsvm_context* context, jsvm_value* Value)
		{
			return jsvm_get_value_bool(context, Value);
		}
		static jsvm_value* ToJS(jsvm_context* context, bool Value)
		{
			return jsvm_create_boolean(context, Value);
		}
	};
	//char,short,int and enum with these type
	template<typename T>
	struct TypeConvertor < T, typename std::enable_if<(std::is_integral<T>::value || std::is_enum<T>::value) && sizeof(T) < 8 && std::is_signed<T>::value > ::type>
	{
		static bool IsMatch(jsvm_context* context, jsvm_value* Value)
		{
			return jsvm_is_int32(Value);
		}
		static T FromJS(jsvm_context* context, jsvm_value* Value)
		{
			return jsvm_get_value_int32(context, Value);
		}
		static jsvm_value* ToJS(jsvm_context* context, T Value)
		{
			return jsvm_create_int32(context, Value);
		}
	};
	//unsigned char,short,int and enum with these type
	template<typename T>
	struct TypeConvertor < T, typename std::enable_if<(std::is_integral<T>::value || std::is_enum<T>::value) && sizeof(T) < 8 && !std::is_signed<T>::value > ::type>
	{
		static bool IsMatch(jsvm_context* context, jsvm_value* Value)
		{
			return jsvm_is_uint32(Value);
		}
		static T FromJS(jsvm_context* context, jsvm_value* Value)
		{
			return jsvm_get_value_uint32(context, Value);
		}
		static jsvm_value* ToJS(jsvm_context* context, T Value)
		{
			return jsvm_create_uint32(context, Value);
		}
	};
	//int64 and enum with this type
	template<typename T>
	struct TypeConvertor<T, typename std::enable_if<(std::is_integral<T>::value || std::is_enum<T>::value) && sizeof(T) == 8 && std::is_signed<T>::value>::type>
	{
		static bool IsMatch(jsvm_context* context, jsvm_value* Value)
		{
			return jsvm_is_int64(Value);
		}
		static T FromJS(jsvm_context* context, jsvm_value* Value)
		{
			return jsvm_get_value_int64(context, Value);
		}
		static jsvm_value* ToJS(jsvm_context* context, T Value)
		{
			return jsvm_create_int64(context, Value);
		}
	};
	//unsigned int64 and enum with this type
	template<typename T>
	struct TypeConvertor<T, typename std::enable_if<(std::is_integral<T>::value || std::is_enum<T>::value) && sizeof(T) == 8 && !std::is_signed<T>::value>::type>
	{
		static bool IsMatch(jsvm_context* context, jsvm_value* Value)
		{
			return jsvm_is_uint64(Value);
		}
		static T FromJS(jsvm_context* context, jsvm_value* Value)
		{
			return jsvm_get_value_uint64(context, Value);
		}
		static jsvm_value* ToJS(jsvm_context* context, T Value)
		{
			return jsvm_create_uint64(context, Value);
		}
	};
	//float,double
	template<typename T>
	struct TypeConvertor<T, typename std::enable_if<std::is_floating_point<T>::value>::type>
	{
		static bool IsMatch(jsvm_context* context, jsvm_value* Value)
		{
			return jsvm_is_double(Value);
		}
		static T FromJS(jsvm_context* context, jsvm_value* Value)
		{
			return jsvm_get_value_double(context, Value);
		}
		static jsvm_value* ToJS(jsvm_context* context, T Value)
		{
			return jsvm_create_double(context, Value);
		}
	};

	template<>
	struct TypeConvertor<const char*>
	{
		static bool IsMatch(jsvm_context* context, jsvm_value* Value)
		{
			return jsvm_is_string(Value);
		}
		static const char* FromJS(jsvm_context* context, jsvm_value* Value)
		{
			if (CEasyStringA::SYSTEM_STRING_CODE_PAGE == CEasyStringA::STRING_CODE_PAGE_UTF8)
				return jsvm_get_value_string_utf8_no_buff(context, Value);
			else
				return jsvm_get_value_string_gbk_no_buff(context, Value);
		}
		static jsvm_value* ToJS(jsvm_context* context, const char* Value)
		{
			if (CEasyStringA::SYSTEM_STRING_CODE_PAGE == CEasyStringA::STRING_CODE_PAGE_UTF8)
				return jsvm_create_string_utf8(context, Value);
			else
				return jsvm_create_string_gbk(context, Value);
		}
	};

	template<>
	struct TypeConvertor<const WCHAR*>
	{
		static bool IsMatch(jsvm_context* context, jsvm_value* Value)
		{
			return jsvm_is_string(Value);
		}
		static const WCHAR* FromJS(jsvm_context* context, jsvm_value* Value)
		{
			return jsvm_get_value_string_utf16_no_buff(context, Value);
		}
		static jsvm_value* ToJS(jsvm_context* context, const WCHAR* Value)
		{
			return jsvm_create_string_utf16(context, Value);
		}
	};

	template<typename T>
	struct TypeConvertor<T, typename std::enable_if<std::is_same<std::decay_t<T>, CEasyStringA>::value>::type>
	{
		static bool IsMatch(jsvm_context* context, jsvm_value* Value)
		{
			return jsvm_is_string(Value);
		}
		static CEasyStringA FromJS(jsvm_context* context, jsvm_value* Value)
		{
			CEasyStringA Str;
			if (CEasyStringA::SYSTEM_STRING_CODE_PAGE == CEasyStringA::STRING_CODE_PAGE_UTF8)
			{
				size_t Len = 0;
				Len = jsvm_get_value_string_utf8(context, Value, NULL, 0);
				Str.Resize(Len);
				jsvm_get_value_string_utf8(context, Value, Str.GetBuffer(), Len + 1);
			}
			else
			{
				size_t Len = 0;
				Len = jsvm_get_value_string_gbk(context, Value, NULL, 0);
				Str.Resize(Len);
				jsvm_get_value_string_gbk(context, Value, Str.GetBuffer(), Len + 1);
			}
			return Str;
		}
		static jsvm_value* ToJS(jsvm_context* context, const CEasyStringA& Value)
		{
			if (CEasyStringA::SYSTEM_STRING_CODE_PAGE == CEasyStringA::STRING_CODE_PAGE_UTF8)
				return jsvm_create_string_utf8(context, Value, Value.GetLength());
			else
				return jsvm_create_string_gbk(context, Value, Value.GetLength());
		}
	};

	template<typename T>
	struct TypeConvertor<T, typename std::enable_if<std::is_same<std::decay_t<T>, CEasyStringW>::value>::type>
	{
		static bool IsMatch(jsvm_context* context, jsvm_value* Value)
		{
			return jsvm_is_string(Value);
		}
		static CEasyStringW FromJS(jsvm_context* context, jsvm_value* Value)
		{
			CEasyStringW Str;
			size_t Len = 0;
			Len = jsvm_get_value_string_utf16(context, Value, NULL, 0);
			Str.Resize(Len);
			jsvm_get_value_string_utf16(context, Value, Str.GetBuffer(), Len + 1);
			return Str;
		}
		static jsvm_value* ToJS(jsvm_context* context, const CEasyStringW& Value)
		{
			return jsvm_create_string_utf16(context, Value, Value.GetLength());
		}
	};

	template<typename T>
	struct TypeConvertor<T, typename std::enable_if<std::is_same<std::decay_t<T>, CEasyBuffer>::value>::type>
	{
		static bool IsMatch(jsvm_context* context, jsvm_value* Value)
		{
			return jsvm_is_binary(Value);
		}
		static CEasyBuffer FromJS(jsvm_context* context, jsvm_value* Value)
		{
			size_t Len = 0;
			void* pData = jsvm_get_value_binary(context, Value, &Len);
			return CEasyBuffer((LPCVOID)pData, Len);
		}
		static jsvm_value* ToJS(jsvm_context* context, const CEasyBuffer& Value)
		{
			return jsvm_create_binary(context, Value.GetBuffer(), Value.GetUsedSize());
		}
	};

	template<typename T>
	struct TypeConvertor<T, typename std::enable_if<is_easy_array<T>::value>::type>
	{
		using U = typename easy_array_content_type<T>::type;
		static bool IsMatch(jsvm_context* context, jsvm_value* Value)
		{
			return jsvm_is_array(Value);
		}
		static std::decay_t<T> FromJS(jsvm_context* context, jsvm_value* Value)
		{
			UINT Len = jsvm_get_array_length(context, Value);
			std::decay_t<T> Array(Len);
			for (UINT i = 0; i < Len; i++)
			{
				jsvm_value* elm = jsvm_obj_get_by_index(context, Value, i);
				if (elm)
					Array.Add(TypeConvertor<U>::FromJS(context, elm));
				else
					Array.AddEmpty();
			}
			return Array;
		}
		static jsvm_value* ToJS(jsvm_context* context, const std::decay_t<T>& Value)
		{
			jsvm_value* array = jsvm_create_array(context);
			for (size_t i = 0; i < Value.GetCount(); i++)
			{
				jsvm_obj_set_by_index(context, array, i, TypeConvertor<U>::ToJS(context, Value[i]));
			}
			return array;
		}
	};
}