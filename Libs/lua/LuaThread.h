#pragma once

class CLuaThread;
class CLuaScript;
class CBaseScriptHost
{
public:
	virtual ~CBaseScriptHost()
	{

	}	
};

class CLuaThread
{
public:
	typedef bool(*LUA_META_CLASS_COPY_FUNCTION) (int Type, CLuaThread * pFromLThread, int Index, CLuaThread * pToLThread);
	enum LUA_VALUE_PACKET_SSID
	{
		SSID_LUA_VALUE_TABLE_KEY = 101,
		SSID_LUA_VALUE_TABLE_VALUE = 102,
	};
protected:
	CLuaScript *		m_pLuaScript;
	UINT				m_ID;
	UINT				m_ScriptID;
	void *				m_Param;
	CBaseScriptHost *	m_pHostObject;
	//lua_State *			m_pLuaStatus;
	lua_State *			m_pLuaThread;
	int					m_ThreadType;
	int					m_StackInitSize;
	bool				m_IsInExec;
	bool				m_IsNeedYield;
	int					m_LastLuaStatus;
	CEasyTimer			m_YieldTimer;
	int					m_YieldType;
	int					m_YeildReturnCount;
	CEasyTimer			m_SleepTimer;

public:
	CLuaThread()
	{
		m_pLuaScript = NULL;
		m_ID = 0;
		m_ScriptID = 0;
		m_pHostObject = NULL;
		//m_pLuaStatus = NULL;
		m_pLuaThread = NULL;
		m_ThreadType = 0;
		m_StackInitSize = 0;
		m_IsInExec = false;
		m_IsNeedYield = false;
		m_LastLuaStatus = LUA_OK;
		m_YieldType = LUA_YIELD_TYPE_NONE;
		m_YeildReturnCount = 0;
	}
	~CLuaThread()
	{
		Clear();
	}
	bool IsInit()
	{
		return m_pLuaThread != NULL;
	}
	bool Init(CLuaScript * pLuaScript, lua_State * pLuaState, UINT ScriptID);
	void Destory()
	{
		Clear();		
		m_ScriptID = 0;
		m_StackInitSize = 0;
		//m_pLuaStatus = NULL;
		m_pLuaThread = NULL;
	}
	void Release();
	void SetID(UINT ID)
	{
		m_ID = ID;
	}
	UINT GetID()
	{
		return m_ID;
	}
	CBaseScriptHost * GetHostObject()
	{
		return m_pHostObject;
	}
	int GetLastLuaStatus()
	{
		return m_LastLuaStatus;
	}
	int GetYeildType()
	{
		return m_YieldType;
	}
	int GetYeildReturnCount()
	{
		return m_YeildReturnCount;
	}
	lua_State * GetLuaState()
	{
		return m_pLuaThread;
	}
	void SetParam(void * Param)
	{
		m_Param = Param;
	}
	void * GetParam()
	{
		return m_Param;
	}
	void SetThreadType(int Type)
	{
		m_ThreadType = Type;
	}
	int GetThreadType()
	{
		return m_ThreadType;
	}
	void NeedYield(int YeildType,int YeildReturnCount)
	{
		m_IsNeedYield = true;
		m_YieldType = YeildType;
		m_YeildReturnCount = YeildReturnCount;
		m_YieldTimer.SaveTime();
	}
	void SetYeildReturnCount(int YeildReturnCount)
	{
		m_YeildReturnCount = YeildReturnCount;
	}
	bool IsNeedYield()
	{
		return m_IsNeedYield;
	}
	void DoSleep(UINT Time)
	{
		m_YieldType = LUA_YIELD_TYPE_SLEEP;
		m_SleepTimer.SetTimeOut(Time);
	}
	void PushNil()
	{
		lua_pushnil (m_pLuaThread);
	}
	void PushNumber(lua_Number Value)
	{
		lua_pushnumber(m_pLuaThread, Value);
	}
	void PushString(LPCSTR Value)
	{
		lua_pushstring(m_pLuaThread, Value);
	}
	void PushString(LPCWSTR Value)
	{
		CEasyStringA StringA;
		StringA = Value;
		lua_pushstring(m_pLuaThread, StringA);
	}
	template<typename T>
	void PushValue(T Value)
	{
		LuaWrap::Push(m_pLuaThread, Value);
	}
	int PushPacket(const CSmartStruct& Packet);
	bool PushTablePacket(const CSmartStruct& Packet);
	void Clear()
	{
		m_IsNeedYield = false;
		m_YieldType = LUA_YIELD_TYPE_NONE;
		m_YeildReturnCount = 0;
		m_LastLuaStatus = LUA_OK;
		m_Param = 0;
	}
	void Reset()
	{
		Clear();
		lua_pop(m_pLuaThread, lua_gettop(m_pLuaThread));		
	}

	bool Prepare(CBaseScriptHost * pObject, LPCSTR szFunctionName);
	inline bool Prepare(CBaseScriptHost * pObject, LPCWSTR szFunctionName)
	{
		CEasyStringA StringA;
		StringA = szFunctionName;
		return Prepare(pObject, (LPCSTR)StringA);
	}
	int DoCall(int ParamCount);	
	LPCSTR GetErrorStringA();
	inline LPCWSTR GetErrorStringW()
	{
		static CEasyStringW ErrorMsg;
		ErrorMsg = GetErrorStringA();
		return ErrorMsg;
	}
	inline LPCTSTR GetErrorString()
	{
#ifdef UNICODE
		return GetErrorStringW();
#else
		return GetErrorStringA();
#endif
	}
	int GetResultCount()
	{
		return lua_gettop(m_pLuaThread) - m_StackInitSize + 1;
	}
	int GetValueType(int Index)
	{
		return lua_type(m_pLuaThread, Index);
	}
	bool PackResult(CSmartStruct& Packet);
	bool PackTable(CSmartStruct& Packet, int Index);
	template<typename T>
	T PopValue()
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
	T GetValue(int Index)
	{
		if (LuaWrap::Match(TypeWrapper<T>(), m_pLuaThread, Index))
		{
			T Result = LuaWrap::Get(TypeWrapper<T>(), m_pLuaThread, Index);
			return Result;
		}
		return T();
	}
	int GetValueCount()
	{
		return lua_gettop(m_pLuaThread);
	}
	int MoveResultToLThread(CLuaThread * pLThread, LUA_META_CLASS_COPY_FUNCTION pExtendCopyFN = NULL);
	int CopyStack(CLuaThread * pTargetLThread, int StartIndex, int Count, LUA_META_CLASS_COPY_FUNCTION pExtendCopyFN = NULL);
	void Update();
	
};

