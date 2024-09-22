#include "stdafx.h"





IMPLEMENT_META_CLASS_WITHOUT_NEW(CLuaBaseMetaClass, _T("BaseMetaClass"))

CLuaBaseMetaClass::CLuaBaseMetaClass()
{
	m_IsInLuaHeap = false;
	m_pRegisterVM = NULL;
	m_pCurThread = NULL;
}


CLuaBaseMetaClass::~CLuaBaseMetaClass()
{
	UnregisterLuaObject();
}

void CLuaBaseMetaClass::SetMetaClass(lua_State * pLuaState)
{
#ifdef UNICODE
	CEasyStringA ClassName;
	SystemStrToLuaStr(GetMetaClassName(), ClassName);
#else
	LPCTSTR ClassName = GetMetaClassName();
#endif
	luaL_getmetatable(pLuaState, ClassName);
	if (lua_isnil(pLuaState, -1))
	{
		lua_pop(pLuaState, 1);
		RegisterMetaClass(pLuaState);
		luaL_getmetatable(pLuaState, ClassName);
	}
	lua_setmetatable(pLuaState, -2);
	InitClassMember(pLuaState);
}
void CLuaBaseMetaClass::InitClassMember(lua_State* L)
{
	lua_newtable(L);
	lua_pushstring(L, "__bindobj");
	lua_newtable(L);
	lua_settable(L, -3);
	RegisterMemberFunctions(L);
	lua_setiuservalue(L, -2, 1);
}
void CLuaBaseMetaClass::RegisterMetaClass(lua_State * pLuaState) const
{
#ifdef UNICODE
	CEasyStringA ClassName;
	SystemStrToLuaStr(GetMetaClassName(), ClassName);
#else
	LPCTSTR ClassName = GetMetaClassName();
#endif
	luaL_newmetatable(pLuaState, ClassName);
	lua_pushstring(pLuaState, "__gc");
	lua_pushcclosure(pLuaState, &CLuaBaseMetaClass::DoGarbageCollect, 0);
	lua_settable(pLuaState, -3);
	lua_pushstring(pLuaState, "__index");
	lua_pushcclosure(pLuaState, &CLuaBaseMetaClass::DoGetMember, 0);
	lua_settable(pLuaState, -3);
	lua_pushstring(pLuaState, "__newindex");
	lua_pushcclosure(pLuaState, &CLuaBaseMetaClass::DoSetMember, 0);
	lua_settable(pLuaState, -3);
	lua_pop(pLuaState, 1);
}



void CLuaBaseMetaClass::RegisterMemberFunctions(lua_State * pLuaState) const
{
	lua_pushstring(pLuaState, "_ClassID");
	lua_pushinteger(pLuaState, GetMetaClassID());
	lua_settable(pLuaState, -3);

	RegisterMetaFunction<CLuaBaseMetaClass>(pLuaState, _T("GetClassName"), &CLuaBaseMetaClass::LuaGetClassName);
	RegisterMetaFunction<CLuaBaseMetaClass>(pLuaState, _T("MergeObject"), &CLuaBaseMetaClass::LuaMergeObject);
	RegisterMetaFunction<CLuaBaseMetaClass>(pLuaState, _T("MemberExist"), &CLuaBaseMetaClass::LuaMemberExist);

}

//bool CLuaBaseMetaClass::GetProperty(lua_State* L, LPCTSTR szPropertyName)
//{
//	return false;
//}
//bool CLuaBaseMetaClass::SetProperty(lua_State* L, LPCTSTR szPropertyName)
//{
//	return false;
//}

void CLuaBaseMetaClass::Dump(CEasyString& Data) const
{
	Data = GetMetaClassName();
}
//void CLuaBaseMetaClass::CheckAPI(lua_State * pLuaState, CEasyArray<CEasyStringA>& Functions) const
//{
//#ifdef UNICODE
//	CEasyStringA ClassName;
//	CBaseLuaVM::SystemStrToLuaStr(GetMetaClassName(), ClassName);
//#else
//	LPCTSTR ClassName = GetMetaClassName();
//#endif
//	luaL_getmetatable(pLuaState, ClassName);
//	if (lua_isnil(pLuaState, -1))
//	{
//		lua_pop(pLuaState, 1);
//		RegisterMetaClass(pLuaState);
//		luaL_getmetatable(pLuaState, ClassName);
//	}
//	lua_pushstring(pLuaState, "__index");
//	lua_gettable(pLuaState, -2);
//	if (lua_istable(pLuaState, -1))
//	{
//		for (UINT i = 0; i < Functions.GetCount(); i++)
//		{
//			CEasyStringA& FuncName = Functions[i];
//			bool IsExist = false;
//			lua_pushstring(pLuaState, FuncName);
//			lua_gettable(pLuaState, -2);
//			if (lua_isnil(pLuaState, -1))
//			{
//				LogLua(_T("Lua %s API %s Not Exist"), GetMetaClassName(), (LPCSTR)FuncName);
//			}
//			lua_pop(pLuaState, 1);
//		}
//	}
//	else
//	{
//		LogLua(_T("Invalid MetaTable %s "), GetMetaClassName());
//	}
//
//	lua_pop(pLuaState, 1);
//
//	lua_pushnil(pLuaState);
//	while (lua_next(pLuaState, -2) != 0)
//	{
//		if (lua_iscfunction(pLuaState, -1))
//		{
//			CEasyStringA FuncName = lua_tostring(pLuaState, -2);
//			bool IsExist = false;
//			for (UINT i = 0; i < Functions.GetCount(); i++)
//			{
//				if (Functions[i].CompareNoCase(FuncName) == 0)
//				{
//					IsExist = true;
//					break;
//				}
//			}
//			if (!IsExist)
//			{
//				LogLua(_T("Lua %s API %s Is Obsolete"), GetMetaClassName(),(LPCSTR)FuncName);
//			}
//		}
//		lua_pop(pLuaState, 1);
//	}
//	lua_pop(pLuaState, 1);
//}
bool CLuaBaseMetaClass::RegisterLuaObject(CBaseLuaVM* pVM, int ObjIndex)
{
	if (m_pRegisterVM)
	{
		LogLua(_T("重复注册对象"));
		return false;
	}
	else
	{
		char* szTableName = "__OUTSIDE_OBJECTS";
		if (IsInLuaHeap())
		{
			//内部对象
			szTableName = "__INSIDE_OBJECTS";
		}
		if (lua_getglobal(pVM->GetLuaState(), szTableName) == LUA_TTABLE)
		{
			lua_pushlightuserdata(pVM->GetLuaState(), this);
			lua_pushvalue(pVM->GetLuaState(), ObjIndex);
			lua_settable(pVM->GetLuaState(), -3);
			lua_pop(pVM->GetLuaState(), 1);
		}
		else
		{
			LogLua(_T("未能找到全局表%s"), szTableName);
			lua_pop(pVM->GetLuaState(), 1);
			return false;
		}

		if (lua_getglobal(pVM->GetLuaState(), "__ALL_OBJECTS") == LUA_TTABLE)
		{
			lua_pushlightuserdata(pVM->GetLuaState(), this);
			lua_pushvalue(pVM->GetLuaState(), ObjIndex);
			lua_settable(pVM->GetLuaState(), -3);

			//LogLua(_T("0x%p成功注册进Lua"), this);
			//CStringBuilder Str;
			//CBaseLuaVM::DumpLuaValue(pVM->GetLuaState(), lua_gettop(pVM->GetLuaState()), Str, "", false, 0);
			//LogLua(_T("%s"), (LPCTSTR)Str.GetStr());

			lua_pop(pVM->GetLuaState(), 1);
			m_pRegisterVM = pVM;
			return true;
		}
		else
		{
			LogLua(_T("未能找到全局表__ALL_OBJECTS"));
			lua_pop(pVM->GetLuaState(), 1);
			return false;
		}
	}
}
bool CLuaBaseMetaClass::UnregisterLuaObject()
{
	if (m_pRegisterVM)
	{
		if (IsInLuaHeap())
		{
			//LogLua(_T("内部对象无需注销"));
			return true;
		}
		else
		{
			if (lua_getglobal(m_pRegisterVM->GetLuaState(), "__OUTSIDE_OBJECTS") == LUA_TTABLE)
			{
				//撤销元表，以免被调用时访问非法内存
				lua_pushlightuserdata(m_pRegisterVM->GetLuaState(), this);
				if (lua_gettable(m_pRegisterVM->GetLuaState(), -2) == LUA_TUSERDATA)
				{
					lua_newtable(m_pRegisterVM->GetLuaState());
					lua_setmetatable(m_pRegisterVM->GetLuaState(), -2);
				}
				else
				{
					LogLua(_T("异常,0x%p对应的不是对象"), this);
				}
				lua_pop(m_pRegisterVM->GetLuaState(), 1);

				lua_pushlightuserdata(m_pRegisterVM->GetLuaState(), this);
				lua_pushnil(m_pRegisterVM->GetLuaState());
				lua_settable(m_pRegisterVM->GetLuaState(), -3);
				lua_pop(m_pRegisterVM->GetLuaState(), 1);
			}
			else
			{
				LogLua(_T("未能找到全局表__OUTSIDE_OBJECTS"));
				lua_pop(m_pRegisterVM->GetLuaState(), 1);
				return false;
			}

			if (lua_getglobal(m_pRegisterVM->GetLuaState(), "__ALL_OBJECTS") == LUA_TTABLE)
			{
				lua_pushlightuserdata(m_pRegisterVM->GetLuaState(), this);
				lua_pushnil(m_pRegisterVM->GetLuaState());
				lua_settable(m_pRegisterVM->GetLuaState(), -3);
				lua_pop(m_pRegisterVM->GetLuaState(), 1);
				m_pRegisterVM = NULL;
				//LogLua(_T("0x%p成功从Lua注销"), this);
				return true;
			}
			else
			{
				LogLua(_T("未能找到全局表__ALL_OBJECTS"));
				lua_pop(m_pRegisterVM->GetLuaState(), 1);
				m_pRegisterVM = NULL;
				return false;
			}
		}
	}
	else
	{
		return false;
	}
}
bool CLuaBaseMetaClass::PushToLua(CLuaThread* pLuaThread)
{
	//失败也保证压栈nil
	if(m_pRegisterVM)
	{
		//已注册
		if (lua_getglobal(pLuaThread->GetLuaState(), "__ALL_OBJECTS") == LUA_TTABLE)
		{
			lua_pushlightuserdata(pLuaThread->GetLuaState(), this);
			int Type = lua_gettable(pLuaThread->GetLuaState(), -2);
			if (Type == LUA_TUSERDATA)
			{
				lua_remove(pLuaThread->GetLuaState(), -2);
				return true;
			}
			else
			{
				if (Type == LUA_TNIL)
				{
					LogLua(_T("0x%p映射值不存在"), this);

					//CStringBuilder Str;
					//CBaseLuaVM::DumpLuaValue(pLuaThread->GetLuaState(), lua_gettop(pLuaThread->GetLuaState()) - 1, Str, "", false, 0);
					//LogLua(_T("%s"), (LPCTSTR)Str.GetStr());
				}
				else
				{
					LogLua(_T("0x%p映射值不是对象%d"), this, Type);
				}



				lua_pop(pLuaThread->GetLuaState(), 2);
				lua_pushnil(pLuaThread->GetLuaState());
			}
		}
	}
	else
	{
		//未注册
		if (!IsInLuaHeap())
		{
			CBaseLuaVM* pVM = pLuaThread->GetLuaVM();
			UINT BuffSize = sizeof(CLuaBaseMetaClass*);
			BYTE* pBuffer = (BYTE*)lua_newuserdatauv(pVM->GetLuaState(), BuffSize, 1);
			CLuaBaseMetaClass* pObject = this;
			memcpy(pBuffer, &pObject, sizeof(CLuaBaseMetaClass*));
			pObject->SetMetaClass(pVM->GetLuaState());
			if(pObject->RegisterLuaObject(pVM, lua_gettop(pVM->GetLuaState())))
			{
				if (pVM->GetLuaState() != pLuaThread->GetLuaState())
				{
					//不是主线程，复制到目标线程
					lua_pop(pVM->GetLuaState(), 1);
					PushToLua(pLuaThread);
				}
				return true;
			}
			else
			{
				//注册失败
				lua_pop(pVM->GetLuaState(), 1);
				lua_pushnil(pLuaThread->GetLuaState());
			}

		}
		else
		{
			LogLua(_T("object alloc in lua not be push"));
			lua_pushnil(pLuaThread->GetLuaState());
		}
	}
	return false;
}

bool CLuaBaseMetaClass::MergeLuaObject(CLuaThread* pLuaThread, int Index, OBJECT_MERGE_TYPE MergeType)
{
	bool Ret = false;
	if(lua_istable(pLuaThread->GetLuaState(), Index))
	{
		if (PushToLua(pLuaThread))
		{
			if (lua_getiuservalue(pLuaThread->GetLuaState(), -1, 1))//获取成员表
			{
				lua_pushvalue(pLuaThread->GetLuaState(), Index);//将要合并的对象压栈
				pLuaThread->PushNil();
				while (lua_next(pLuaThread->GetLuaState(), -2) != 0)//遍历要合并的对象，把所有键值压入成员表
				{
					if (MergeType != OBJECT_MERGE_TYPE_OVERWRITE_ALL)
					{
						//不覆盖，需要检查是否已经存在
						lua_pushvalue(pLuaThread->GetLuaState(), -2);
						lua_gettable(pLuaThread->GetLuaState(), -5);
						if ((MergeType == OBJECT_MERGE_TYPE_OVERWRITE_FUNCTION) &&
							(!lua_isfunction(pLuaThread->GetLuaState(), -1)) && (!IsProperty(pLuaThread->GetLuaState())))
						{
							lua_pop(pLuaThread->GetLuaState(), 2);
							continue;
						}
						if ((MergeType == OBJECT_MERGE_TYPE_OVERWRITE_NONE) && (!lua_isnil(pLuaThread->GetLuaState(), -1)))
						{
							lua_pop(pLuaThread->GetLuaState(), 2);
							continue;
						}
						lua_pop(pLuaThread->GetLuaState(), 1);
					}
					lua_pushvalue(pLuaThread->GetLuaState(), -2);
					lua_insert(pLuaThread->GetLuaState(), lua_gettop(pLuaThread->GetLuaState()) - 2);
					lua_settable(pLuaThread->GetLuaState(), -5);
				}
				Ret = true;
				pLuaThread->Pop(1);
			}
		}
		pLuaThread->Pop(1);
	}
	else
	{
		LogLua(_T("object can not be merge"));
	}
	return Ret;
}

CLuaThread* CLuaBaseMetaClass::AllocLuaThread()
{
	if (m_pRegisterVM)
	{
		return m_pRegisterVM->AllocLuaThread();
	}
	else
	{
		LogLua(_T("对象%s还未注册进Lua"), GetClassName());
	}
	return NULL;
}

bool CLuaBaseMetaClass::PrepareCall(CLuaThread* pLuaThread, LPCTSTR szMemberName) throw(LUA_EXCEPTION)
{
	try
	{
		CEasyStringA MemberName;
		SystemStrToLuaStr(szMemberName, MemberName);
		pLuaThread->StartCall();
		if (PushToLua(pLuaThread))
		{
			pLuaThread->PushString(MemberName);
			lua_gettable(pLuaThread->GetLuaState(), -2);
		}
		lua_insert(pLuaThread->GetLuaState(), lua_gettop(pLuaThread->GetLuaState()) - 1);
		return lua_isfunction(pLuaThread->GetLuaState(), -2);
	}
	catch(LUA_EXCEPTION&)
	{
		return false;
	}
}

CLuaThread* CLuaBaseMetaClass::PrepareCall(LPCTSTR szMemberName)
{
	if (m_pRegisterVM)
	{
		return m_pRegisterVM->PrepareCallMember(this, szMemberName);
	}
	else
	{
		LogLua(_T("对象%s还未注册进Lua"), GetClassName());
	}
	return NULL;
}

bool CLuaBaseMetaClass::PushMember(CLuaThread* pLuaThread, LPCTSTR MemberName)
{
	int Top = lua_gettop(pLuaThread->GetLuaState());
	if (PushToLua(pLuaThread)) 
	{
		int ObjIndex = lua_gettop(pLuaThread->GetLuaState());
		//取出成员表
		if (lua_getiuservalue(pLuaThread->GetLuaState(), ObjIndex, 1) == LUA_TTABLE)
		{
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

			if (IsProperty(pLuaThread->GetLuaState()))
			{
				//是属性，执行__get
				lua_pushcclosure(pLuaThread->GetLuaState(), CLuaThread::LuaErrParser, 0);
				int nMsgFn = lua_gettop(pLuaThread->GetLuaState());
				lua_pushstring(pLuaThread->GetLuaState(), "__get");
				lua_gettable(pLuaThread->GetLuaState(), -3);
				if (lua_isfunction(pLuaThread->GetLuaState(), -1))
				{
					lua_pushvalue(pLuaThread->GetLuaState(), ObjIndex);
					if (lua_pcall(pLuaThread->GetLuaState(), 1, 1, nMsgFn) == LUA_OK)
					{
						lua_copy(pLuaThread->GetLuaState(), -1, Top + 1);
						lua_settop(pLuaThread->GetLuaState(), Top + 1);
						return true;
					}
				}				
			}
			else
			{
				lua_copy(pLuaThread->GetLuaState(), -1, Top + 1);
				lua_settop(pLuaThread->GetLuaState(), Top + 1);
				return true;
			}
			
		}		
	}
	lua_settop(pLuaThread->GetLuaState(), Top);
	pLuaThread->PushNil();
	return false;
}

bool CLuaBaseMetaClass::SetMember(CLuaThread* pLuaThread, LPCTSTR MemberName)
{
	int Top = lua_gettop(pLuaThread->GetLuaState());
	if (Top > 0)
	{
		if (PushToLua(pLuaThread))
		{
			int ObjIndex = lua_gettop(pLuaThread->GetLuaState());
			//取出成员表
			if (lua_getiuservalue(pLuaThread->GetLuaState(), ObjIndex, 1) == LUA_TTABLE)
			{
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
				lua_pushvalue(pLuaThread->GetLuaState(), -1);
				lua_gettable(pLuaThread->GetLuaState(), -3);

				if (IsProperty(pLuaThread->GetLuaState()))
				{
					//是属性，执行__get
					lua_pushcclosure(pLuaThread->GetLuaState(), CLuaThread::LuaErrParser, 0);
					int nMsgFn = lua_gettop(pLuaThread->GetLuaState());
					lua_pushstring(pLuaThread->GetLuaState(), "__set");
					lua_gettable(pLuaThread->GetLuaState(), -3);
					if (lua_isfunction(pLuaThread->GetLuaState(), -1))
					{
						lua_pushvalue(pLuaThread->GetLuaState(), ObjIndex);
						lua_pushvalue(pLuaThread->GetLuaState(), Top);
						if (lua_pcall(pLuaThread->GetLuaState(), 1, 1, nMsgFn) == LUA_OK)
						{
							lua_settop(pLuaThread->GetLuaState(), Top - 1);
							return true;
						}
					}
				}
				else
				{
					pLuaThread->Pop(1);
					lua_pushvalue(pLuaThread->GetLuaState(), Top);
					lua_settable(pLuaThread->GetLuaState(), -3);
					lua_settop(pLuaThread->GetLuaState(), Top - 1);
					return true;
				}

			}

		}
		lua_settop(pLuaThread->GetLuaState(), Top - 1);
	}
	return false;
}

int CLuaBaseMetaClass::AddPersistentObject(CLuaThread* pLuaThread, int Index)
{
	int Top = lua_gettop(pLuaThread->GetLuaState());
	if(Top>0)
	{
		Index = lua_absindex(pLuaThread->GetLuaState(), Index);
		PushMember(pLuaThread, "__PERSISTENT_OBJECTS");
		if (!lua_istable(pLuaThread->GetLuaState(), -1))
		{
			pLuaThread->Pop(1);
			lua_newtable(pLuaThread->GetLuaState());
			if (!SetMember(pLuaThread, "__PERSISTENT_OBJECTS"))
			{
				LogLua(_T("创建表__PERSISTENT_OBJECTS失败"));
				lua_settop(pLuaThread->GetLuaState(), Top - 1);
				return LUA_NOREF;
			}
			PushMember(pLuaThread, "__PERSISTENT_OBJECTS");
			//LogLuaDebug(_T("__PERSISTENT_OBJECTS Created"));
		}
		lua_pushvalue(pLuaThread->GetLuaState(), Index);
		int RefID = luaL_ref(pLuaThread->GetLuaState(), -2);

		//int Len = luaL_len(pLuaThread->GetLuaState(), -1);
		//LogLuaDebug(_T("__PERSISTENT_OBJECTS Ref=%d MaxRef=%d"), RefID, Len);

		lua_settop(pLuaThread->GetLuaState(), Top - 1);
		return RefID;
	}
	return LUA_NOREF;
}
int CLuaBaseMetaClass::PushPersistentObject(CLuaThread* pLuaThread, int RefID)
{
	PushMember(pLuaThread, "__PERSISTENT_OBJECTS");
	if (lua_istable(pLuaThread->GetLuaState(), -1))
	{
		lua_rawgeti(pLuaThread->GetLuaState(), -1, RefID);
		lua_remove(pLuaThread->GetLuaState(), -2);
		return lua_type(pLuaThread->GetLuaState(), -1);
	}
	else
	{
		LogLua(_T("表__PERSISTENT_OBJECTS不存在"));
	}
	pLuaThread->Pop(1);
	pLuaThread->PushNil();
	return LUA_TNIL;
}
bool CLuaBaseMetaClass::RemovePersistentObject(CLuaThread* pLuaThread, int RefID)
{
	PushMember(pLuaThread, "__PERSISTENT_OBJECTS");
	if (lua_istable(pLuaThread->GetLuaState(), -1))
	{
		luaL_unref(pLuaThread->GetLuaState(), -1, RefID);

		//int Len = luaL_len(pLuaThread->GetLuaState(), -1);
		//LogLuaDebug(_T("__CALLBACK_FN_TABLE MaxRef=%d"), Len);

		lua_pop(pLuaThread->GetLuaState(), 1);
		return true;
	}
	else
	{
		LogLua(_T("表__PERSISTENT_OBJECTS不存在"));
	}
	pLuaThread->Pop(1);
	return false;
}

int CLuaBaseMetaClass::DoGarbageCollect(lua_State* L)
{
	if (lua_type(L, 1) == LUA_TUSERDATA)
	{
		UINT Len = (UINT)lua_rawlen(L, 1);
		BYTE* pBuff = (BYTE*)lua_touserdata(L, 1);
		if (GetLuaObjectType(L, 1) >= CLuaBaseMetaClass::CLASS_ID)
		{
			if (pBuff && Len >= sizeof(CLuaBaseMetaClass*))
			{
				CLuaBaseMetaClass* pObject = dynamic_cast<CLuaBaseMetaClass*>(*((CLuaBaseMetaClass**)pBuff));

				if (pObject)
				{
					pObject->OnGarbageCollect();
				}
				else
				{
					LogLua(_T("对象不是CLuaBaseMetaClass"));
				}
			}
			else
			{
				LogLua(_T("userdata大小不符"));
			}
		}
		else
		{
			LogLua(_T("对象类型不符"));
		}
	}
	else
	{
		LogLua(_T("对象不是userdata"));
	}
	return 0;
}

int CLuaBaseMetaClass::DoGetMember(lua_State* L)
{
	if (lua_type(L, 1) == LUA_TUSERDATA)
	{
		UINT Len = (UINT)lua_rawlen(L, 1);
		BYTE* pBuff = (BYTE*)lua_touserdata(L, 1);
		if (GetLuaObjectType(L, 1) >= CLuaBaseMetaClass::CLASS_ID)
		{
			if (pBuff && Len >= sizeof(CLuaBaseMetaClass*))
			{
				CLuaBaseMetaClass* pObject = dynamic_cast<CLuaBaseMetaClass*>(*((CLuaBaseMetaClass**)pBuff));

				if (pObject)
				{
					if (lua_isstring(L, 2))
					{
						CEasyString Temp;
						const char* szPropertyName = lua_tostring(L, 2);

						if (CompareString(szPropertyName, "__members") == 0)
						{
							//特殊属性,返回成员表
							lua_getiuservalue(L, 1, 1);
							return 1;
						}
						else
						{
							//尝试获得成员
							if (lua_getiuservalue(L, 1, 1) == LUA_TTABLE)
							{
								lua_pushvalue(L, 2);
								lua_gettable(L, -2);
								if (lua_isnil(L, -1))
								{
									luaL_error(L, "member %s not exist", szPropertyName);
								}
								else if(IsProperty(L))
								{
									//是属性，执行__get
									lua_pushstring(L, "__get");
									lua_gettable(L, -2);
									if (lua_isfunction(L, -1))
									{
										lua_pushvalue(L, 1);
										lua_call(L, 1, 1);
									}
									else
									{
										lua_pop(L, 1);
										luaL_error(L, "member %s can not be get", szPropertyName);
									}
								}
								return 1;
							}
							else
							{
								lua_pop(L, 1);
								luaL_error(L, "target not have __members");
							}
						}
					}
					else
					{
						luaL_error(L, "member name must string");
					}
				}
				else
				{
					luaL_error(L, "target not CLuaBaseMetaClass");
				}
			}
			else
			{
				luaL_error(L, "userdata size error");
			}
		}
		else
		{
			luaL_error(L, "target not metaclass");
		}
	}
	else
	{
		luaL_error(L, "target not userdata");
	}
	return 0;
}

int CLuaBaseMetaClass::DoSetMember(lua_State* L)
{
	if (lua_type(L, 1) == LUA_TUSERDATA)
	{
		UINT Len = (UINT)lua_rawlen(L, 1);
		BYTE* pBuff = (BYTE*)lua_touserdata(L, 1);
		if (GetLuaObjectType(L, 1) >= CLuaBaseMetaClass::CLASS_ID)
		{
			if (pBuff && Len >= sizeof(CLuaBaseMetaClass*))
			{
				CLuaBaseMetaClass* pObject = dynamic_cast<CLuaBaseMetaClass*>(*((CLuaBaseMetaClass**)pBuff));

				if (pObject)
				{
					if(lua_isstring(L, 2))
					{
						if (lua_getiuservalue(L, 1, 1) == LUA_TTABLE)
						{
							//先检测下是不是属性
							lua_pushvalue(L, 2);
							lua_gettable(L, -2);
							if (IsProperty(L))
							{
								//是属性，调用__set
								lua_pushstring(L, "__set");
								lua_gettable(L, -2);
								//lua_remove(L, lua_gettop(L) - 1); //如果有可能不是在lua中被函数形式调用，就应该加这一句，保持堆栈原状
								if (lua_isfunction(L, -1))
								{
									lua_pushvalue(L, 1);
									lua_pushvalue(L, 3);
									lua_call(L, 2, 0);
								}
								else
								{
									const char* szPropertyName = lua_tostring(L, 2);

									luaL_error(L, "member %s can not be set", szPropertyName);
								}
							}
							else
							{
								//不是属性，走t[k]=v 流程
								lua_pop(L, 1);
								lua_pushvalue(L, 2);
								lua_pushvalue(L, 3);
								lua_settable(L, -3);
							}

						}
						else
						{
							lua_pop(L, 1);
							luaL_error(L, "target not have __members");
						}

					}
				}
				else
				{
					luaL_error(L, "target not CLuaBaseMetaClass");
				}
			}
			else
			{
				luaL_error(L, "userdata size error");
			}
		}
		else
		{
			luaL_error(L, "target not metaclass");
		}
	}
	else
	{
		luaL_error(L, "target not userdata");
	}
	return 0;
}

void CLuaBaseMetaClass::AddBindObject(lua_State* L, int Index)
{
	int ObjIndex = lua_gettop(L);
	if (lua_getiuservalue(L, Index, 1) == LUA_TTABLE)
	{
		lua_pushstring(L, "__bindobj");
		if (lua_gettable(L, -2) == LUA_TTABLE)
		{
			lua_len(L, -1);
			UINT Len = (UINT)lua_tointeger(L, -1);
			lua_pop(L, 1);
			lua_pushinteger(L, Len + 1);
			lua_pushvalue(L, ObjIndex);
			lua_settable(L, -3);
		}
		else
		{
			LogLua(_T("没有绑定对象表"));
		}
		lua_pop(L, 2);
	}
	else
	{
		lua_pop(L, 1);
		LogLua(_T("对象没有成员"));
	}
	lua_pop(L, 1);
}

bool CLuaBaseMetaClass::IsProperty(lua_State* L)
{
	if (lua_istable(L, -1))
	{
		lua_pushstring(L, "__get");
		lua_gettable(L, -2);
		if (lua_isfunction(L, -1))
		{
			lua_pop(L, 1);
			return true;
		}
		lua_pop(L, 1);
		lua_pushstring(L, "__set");
		lua_gettable(L, -2);
		if (lua_isfunction(L, -1))
		{
			lua_pop(L, 1);
			return true;
		}
		lua_pop(L, 1);
	}
	return false;
}

void CLuaBaseMetaClass::RegisterMetaCFun(lua_State * pLuaState, LPCTSTR szFuncName, lua_CFunction function, void* func, unsigned int sizeofFunc) const
{
	if (LUA_SCRIPT_CODE_PAGE == CEasyString::SYSTEM_STRING_CODE_PAGE)
	{
		lua_pushstring(pLuaState, (const char*)szFuncName);
	}
	else
	{
		CEasyStringA FuncName;
		SystemStrToLuaStr(szFuncName, FuncName);
		lua_pushstring(pLuaState, FuncName);
	}


	unsigned char* buffer = (unsigned char*)lua_newuserdatauv(pLuaState, sizeofFunc, 0);

	memcpy(buffer, func, sizeofFunc), buffer += sizeofFunc;

	lua_pushcclosure(pLuaState, function, 1);
	lua_settable(pLuaState, -3);
}

void CLuaBaseMetaClass::RegisterMetaProperty(lua_State* pLuaState, LPCTSTR szPropertyName, bool IsGet, lua_CFunction function, void* func, unsigned int sizeofFunc) const
{
	CEasyStringA PropertyName;
	SystemStrToLuaStr(szPropertyName, PropertyName);
	lua_pushstring(pLuaState, PropertyName);

	lua_gettable(pLuaState, -2);
	if (!lua_istable(pLuaState, -1))
	{
		lua_pop(pLuaState, 1);
		lua_pushstring(pLuaState, PropertyName);
		lua_newtable(pLuaState);
		lua_settable(pLuaState, -3);
		lua_pushstring(pLuaState, PropertyName);
		lua_gettable(pLuaState, -2);
	}
	if(IsGet)
		lua_pushstring(pLuaState, "__get");
	else
		lua_pushstring(pLuaState, "__set");
	unsigned char* buffer = (unsigned char*)lua_newuserdatauv(pLuaState, sizeofFunc, 0);

	memcpy(buffer, func, sizeofFunc), buffer += sizeofFunc;

	lua_pushcclosure(pLuaState, function, 1);
	lua_settable(pLuaState, -3);
	lua_pop(pLuaState, 1);
}

LPCTSTR CLuaBaseMetaClass::LuaGetClassName()
{
	return GetMetaClassName();
}

bool CLuaBaseMetaClass::LuaMergeObject(LUA_EMPTY_VALUE Obj, int MergeType)
{
	return MergeLuaObject(m_pCurThread, 2, (OBJECT_MERGE_TYPE)MergeType);
}

bool CLuaBaseMetaClass::LuaMemberExist(LPCSTR MemberName)
{
	bool Exist = false;
	if (PushToLua(m_pCurThread))
	{
		if (lua_getiuservalue(m_pCurThread->GetLuaState(), -1, 1) == LUA_TTABLE)
		{
			lua_pushstring(m_pCurThread->GetLuaState(), MemberName);
			lua_gettable(m_pCurThread->GetLuaState(), -2);
			Exist = !lua_isnil(m_pCurThread->GetLuaState(), -1);
			m_pCurThread->Pop(1);
		}
		m_pCurThread->Pop(1);
	}
	m_pCurThread->Pop(1);
	return Exist;
}
