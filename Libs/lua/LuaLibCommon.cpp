#include "stdafx.h"

#include "../../Libs/rapidjson/document.h"
#include "../../Libs/rapidjson/stringbuffer.h"
#include "../../Libs/rapidjson/prettywriter.h"
#include "../../Libs/rapidjson/writer.h"

CLuaLibCommon::CLuaLibCommon()
{
	RegisterFunction<CLuaLibCommon>("Log", &CLuaLibCommon::LuaLog);
	RegisterFunction<CLuaLibCommon>("LogDebug", &CLuaLibCommon::LuaLogDebug);
	RegisterFunction<CLuaLibCommon>("LogWithStack", &CLuaLibCommon::LuaLogWithStack);
	RegisterFunction<CLuaLibCommon>("NewByteArray", &CLuaLibCommon::LuaNewByteArray);
	RegisterFunction<CLuaLibCommon>("NewGrid", &CLuaLibCommon::LuaNewGrid);
	RegisterFunction<CLuaLibCommon>("TableToJsonStr", &CLuaLibCommon::LuaTableToJsonStr);
	RegisterFunction<CLuaLibCommon>("TableToJsonStrPretty", &CLuaLibCommon::LuaTableToJsonStrPretty);
	RegisterFunction<CLuaLibCommon>("JsonStrToTable", &CLuaLibCommon::LuaJsonStrToTable);
}


CLuaLibCommon::~CLuaLibCommon()
{
	
}

bool TableToJson(lua_State * pLuaState, int Index, rapidjson::Value& JsonObject, rapidjson::Document::AllocatorType &Alloc)
{
	if (!lua_istable(pLuaState, Index))
	{
		return false;
	}
	lua_pushnil(pLuaState);  /* 第一个 key */
	if (Index < 0)
		Index--;
	int Next = lua_next(pLuaState, Index);
	if (lua_isinteger(pLuaState, -2))
	{
		JsonObject.SetArray();
		while (Next != 0)
		{
			/* 用一下 'key' （在索引 -2 处） 和 'value' （在索引 -1 处） */
			int Type = GetLuaObjectType(pLuaState, -1);
			switch (Type)
			{
			case LUA_TNIL:
				JsonObject.PushBack(rapidjson::Value(rapidjson::kNullType), Alloc);
				break;
			case LUA_TBOOLEAN:
				JsonObject.PushBack((lua_toboolean(pLuaState, -1) != 0), Alloc);
				break;
			case LUA_TNUMBER:
				JsonObject.PushBack(lua_tonumber(pLuaState, -1), Alloc);
				break;
			case LUA_TSTRING:
				JsonObject.PushBack(rapidjson::Value(lua_tostring(pLuaState, -1), Alloc), Alloc);
				break;
			case LUA_TTABLE:
			{
				rapidjson::Value SubObject;
				TableToJson(pLuaState, -1, SubObject, Alloc);
				JsonObject.PushBack(SubObject, Alloc);
			}
			break;
			case LUA_TINTEGER:
				JsonObject.PushBack(lua_tointeger(pLuaState, -1), Alloc);
				break;
			}
			lua_pop(pLuaState, 1);

			Next = lua_next(pLuaState, Index);
		}
	}
	else
	{
		JsonObject.SetObject();
		while (Next != 0)
		{
			/* 用一下 'key' （在索引 -2 处） 和 'value' （在索引 -1 处） */
#ifdef UNICODE
			CEasyStringW Key = lua_tostring(pLuaState, -2);
#else
			LPCTSTR Key = lua_tostring(pLuaState, -2);
#endif
			int Type = GetLuaObjectType(pLuaState, -1);
			switch (Type)
			{
			case LUA_TNIL:
				JsonObject.AddMember(rapidjson::Value((LPCTSTR)Key, Alloc), rapidjson::Value(rapidjson::kNullType), Alloc);
				break;
			case LUA_TBOOLEAN:
				JsonObject.AddMember(rapidjson::Value((LPCTSTR)Key, Alloc), (lua_toboolean(pLuaState, -1) != 0), Alloc);
				break;
			case LUA_TNUMBER:
				JsonObject.AddMember(rapidjson::Value((LPCTSTR)Key, Alloc), lua_tonumber(pLuaState, -1), Alloc);
				break;
			case LUA_TSTRING:
				JsonObject.AddMember(rapidjson::Value((LPCTSTR)Key, Alloc), rapidjson::Value(lua_tostring(pLuaState, -1), Alloc), Alloc);
				break;
			case LUA_TTABLE:
			{
				rapidjson::Value SubObject;
				TableToJson(pLuaState, -1, SubObject, Alloc);
				JsonObject.AddMember(rapidjson::Value((LPCTSTR)Key, Alloc), SubObject, Alloc);
			}
			break;
			case LUA_TINTEGER:
				JsonObject.AddMember(rapidjson::Value((LPCTSTR)Key, Alloc), lua_tointeger(pLuaState, -1), Alloc);
				break;
			}
			lua_pop(pLuaState, 1);

			Next = lua_next(pLuaState, Index);
		}
	}
	
	return true;
}

void JsonToTable(CLuaThread * pLuaThread, rapidjson::Value& JsonObject)
{
	lua_newtable(pLuaThread->GetLuaState());
	if (JsonObject.IsArray())
	{
		for (size_t i = 0; i < JsonObject.Size(); i++)
		{
			rapidjson::Value& Element = JsonObject[(rapidjson::SizeType)i];

			pLuaThread->PushValue(i + 1);

			if (Element.IsBool())
			{
				pLuaThread->PushValue(Element.GetBool());
			}
			else if (Element.IsInt())
			{
				pLuaThread->PushValue(Element.GetInt());
			}
			else if (Element.IsUint())
			{
				pLuaThread->PushValue(Element.GetInt());
			}
			else if (Element.IsInt64())
			{
				pLuaThread->PushValue(Element.GetInt());
			}
			else if (Element.IsUint64())
			{
				pLuaThread->PushValue(Element.GetInt());
			}
			else if (Element.IsDouble())
			{
				pLuaThread->PushValue(Element.GetInt());
			}
			else if (Element.IsString())
			{
				pLuaThread->PushValue(Element.GetInt());
			}
			else
			{
				JsonToTable(pLuaThread, Element);
			}
			lua_settable(pLuaThread->GetLuaState(), -3);
		}
	}
	else
	{
		for (rapidjson::Value::MemberIterator Itr = JsonObject.MemberBegin(); Itr != JsonObject.MemberEnd(); Itr++)
		{
			pLuaThread->PushString(Itr->name.GetString());
			if (Itr->value.IsBool())
			{
				pLuaThread->PushValue(Itr->value.GetBool());
			}
			else if (Itr->value.IsInt())
			{
				pLuaThread->PushValue(Itr->value.GetInt());
			}
			else if (Itr->value.IsUint())
			{
				pLuaThread->PushValue(Itr->value.GetUint());
			}
			else if (Itr->value.IsInt64())
			{
				pLuaThread->PushValue(Itr->value.GetInt64());
			}
			else if (Itr->value.IsUint64())
			{
				pLuaThread->PushValue(Itr->value.GetUint64());
			}
			else if (Itr->value.IsDouble())
			{
				pLuaThread->PushValue(Itr->value.GetDouble());
			}
			else if (Itr->value.IsString())
			{
				pLuaThread->PushValue(Itr->value.GetString());
			}
			else
			{
				JsonToTable(pLuaThread, Itr->value);
			}
			lua_settable(pLuaThread->GetLuaState(), -3);
		}
	}
}

void CLuaLibCommon::LuaLog(CLuaThread * pLuaThread, LPCTSTR Text)
{
	LogLua(_T("%s"), Text);
}
void CLuaLibCommon::LuaLogDebug(CLuaThread * pLuaThread, LPCTSTR Text)
{
	LogLuaDebug(_T("%s"), Text);
}
void CLuaLibCommon::LuaLogWithStack(CLuaThread * pLuaThread, int Deepth, LPCTSTR Text)
{	
	LogLua(_T("%s"), Text);
	luaL_traceback(pLuaThread->GetLuaState(), pLuaThread->GetLuaState(), NULL, 0);
	LogLua(_T("%s"), lua_tolstring(pLuaThread->GetLuaState(), -1, NULL));
	lua_pop(pLuaThread->GetLuaState(), 1);
}
LUA_EMPTY_VALUE CLuaLibCommon::LuaNewByteArray(CLuaThread * pLuaThread, UINT Size)
{
	UINT ClassSize = sizeof(CLuaByteArray);
	if (Size)
	{
		BYTE * pBuff = (BYTE *)lua_newuserdata(pLuaThread->GetLuaState(), ClassSize + Size);
#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#undef new
#endif
		::new(pBuff) CLuaByteArray();
#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#define new NEWNEW
#endif
		((CLuaByteArray *)pBuff)->Attach(pBuff + ClassSize, Size);
		((CLuaByteArray *)pBuff)->SetMetaClass(pLuaThread->GetLuaState());
	}
	else
	{
		lua_pushnil(pLuaThread->GetLuaState());
	}
	return LUA_EMPTY_VALUE();
}
LUA_EMPTY_VALUE CLuaLibCommon::LuaNewGrid(CLuaThread * pLuaThread, UINT Col, UINT Row)
{
	UINT ClassSize = sizeof(CLuaGrid);
	UINT Size = Col * Row * sizeof(LuaValue) + sizeof(WORD) * 2;;
	if (Size)
	{
		BYTE * pBuff = (BYTE *)lua_newuserdata(pLuaThread->GetLuaState(), ClassSize + Size);
#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#undef new
#endif
		::new(pBuff) CLuaGrid();
#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#define new NEWNEW
#endif
		((CLuaGrid *)pBuff)->Attach(pBuff + ClassSize, Size, Col, Row);
		((CLuaGrid *)pBuff)->SetMetaClass(pLuaThread->GetLuaState());
	}
	else
	{
		lua_pushnil(pLuaThread->GetLuaState());
	}
	return LUA_EMPTY_VALUE();
}

LUA_EMPTY_VALUE CLuaLibCommon::LuaTableToJsonStr(CLuaThread * pLuaThread, LUA_EMPTY_VALUE)
{
	rapidjson::Document Root;
	rapidjson::Document::AllocatorType &Alloc = Root.GetAllocator();

	TableToJson(pLuaThread->GetLuaState(), -1, Root, Alloc);

#ifdef UNICODE
	rapidjson::GenericStringBuffer<rapidjson::UTF16<> > buffer;
	rapidjson::Writer<rapidjson::GenericStringBuffer<rapidjson::UTF16<> >, rapidjson::UTF16<>, rapidjson::UTF16<> > writer(buffer);
#else
	rapidjson::GenericStringBuffer<rapidjson::UTF8<> > buffer;
	rapidjson::Writer<rapidjson::GenericStringBuffer<rapidjson::UTF8<> >, rapidjson::UTF8<>, rapidjson::UTF8<> > writer(buffer);
#endif

	Root.Accept(writer);

	pLuaThread->PushString(buffer.GetString());
	return LUA_EMPTY_VALUE();
}

LUA_EMPTY_VALUE CLuaLibCommon::LuaTableToJsonStrPretty(CLuaThread * pLuaThread, LUA_EMPTY_VALUE)
{
	rapidjson::Document Root;
	rapidjson::Document::AllocatorType &Alloc = Root.GetAllocator();

	TableToJson(pLuaThread->GetLuaState(), -1, Root, Alloc);

#ifdef UNICODE
	rapidjson::GenericStringBuffer<rapidjson::UTF16<> > buffer;
	rapidjson::PrettyWriter<rapidjson::GenericStringBuffer<rapidjson::UTF16<> >, rapidjson::UTF16<>, rapidjson::UTF16<> > writer(buffer);
#else
	rapidjson::GenericStringBuffer<rapidjson::UTF8<> > buffer;
	rapidjson::PrettyWriter<rapidjson::GenericStringBuffer<rapidjson::UTF8<> >, rapidjson::UTF8<>, rapidjson::UTF8<> > writer(buffer);
#endif

	Root.Accept(writer);

	pLuaThread->PushString(buffer.GetString());
	return LUA_EMPTY_VALUE();
}

LUA_EMPTY_VALUE CLuaLibCommon::LuaJsonStrToTable(CLuaThread * pLuaThread, LPCTSTR JsonStr)
{
	rapidjson::Document Root;
	Root.Parse(JsonStr);
	if (!Root.HasParseError())
	{
		JsonToTable(pLuaThread, Root);
	}
	else
	{
		pLuaThread->PushNil();
	}
	return LUA_EMPTY_VALUE();
}