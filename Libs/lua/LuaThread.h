#pragma once

class CLuaThread;
class CBaseLuaVM;


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
	UINT				m_ID;
	CBaseLuaVM *		m_pLuaVM;
	void *				m_Param1;
	void *				m_Param2;
	CBaseScriptHost *	m_pHostObject;
	lua_State *			m_pLuaThread;
	int					m_ThreadType;
	bool				m_IsInExec;
	bool				m_IsNeedYield;
	int					m_LastLuaStatus;
	CEasyTimer			m_YieldTimer;
	int					m_YieldType;
	int					m_YieldReturnCount;
	CEasyTimer			m_SleepTimer;
	ITimeManager *		m_pTimeManager;
	float				m_SleepTime;
	float				m_SleepPastTime;
public:
	CLuaThread()
	{
		m_ID = 0;
		m_pLuaVM = NULL;
		m_Param1 = NULL;
		m_Param2 = NULL;
		m_pHostObject = NULL;
		m_pLuaThread = NULL;
		m_ThreadType = 0;
		m_IsInExec = false;
		m_IsNeedYield = false;
		m_LastLuaStatus = LUA_OK;		
		m_YieldType = LUA_YIELD_TYPE_NONE;
		m_YieldReturnCount = 0;		
		m_pTimeManager = NULL;
		m_SleepTime = 0;
		m_SleepPastTime = 0;
	}
	~CLuaThread()
	{
		//Destory();
	}
	bool IsInited()
	{
		return m_pLuaThread != NULL;
	}
	bool Init(CBaseLuaVM * pLuaVM, int StackSize);
	void Destory();
	//void Release();
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
	CBaseLuaVM * GetLuaVM()
	{
		return m_pLuaVM;
	}
	int GetLastLuaStatus()
	{
		return m_LastLuaStatus;
	}
	int GetYeildType()
	{
		return m_YieldType;
	}
	int GetYieldReturnCount()
	{
		return m_YieldReturnCount;
	}
	lua_State * GetLuaState()
	{
		return m_pLuaThread;
	}
	void SetParam1(void * Param)
	{
		m_Param1 = Param;
	}
	void * GetParam1()
	{
		return m_Param1;
	}
	void SetParam2(void * Param)
	{
		m_Param2 = Param;
	}
	void * GetParam2()
	{
		return m_Param2;
	}
	void SetThreadType(int Type)
	{
		m_ThreadType = Type;
	}
	int GetThreadType()
	{
		return m_ThreadType;
	}
	void NeedYield(int YeildType, int YeildReturnCount, UINT Timeout = LUA_YIELD_TIME_OUT)
	{
		m_IsNeedYield = true;
		m_YieldType = YeildType;
		m_YieldReturnCount = YeildReturnCount;
		m_YieldTimer.SetTimeOut(Timeout);
	}	
	bool IsNeedYield()
	{
		return m_IsNeedYield;
	}
	void ClearYield()
	{
		m_IsNeedYield = false;
		m_LastLuaStatus = LUA_OK;
		m_YieldType = LUA_YIELD_TYPE_NONE;
		m_YieldReturnCount = 0;
	}
	bool IsYieldTimeOut()
	{
		return m_YieldTimer.IsTimeOut();
	}
	void DoSleep(UINT Time)
	{
		m_YieldType = LUA_YIELD_TYPE_SLEEP;
		m_SleepTimer.SetTimeOut(Time);
	}
	void DoSleep(ITimeManager * pTimeManager,float Time)
	{
		m_YieldType = LUA_YIELD_TYPE_SLEEP;
		m_pTimeManager = pTimeManager;
		m_SleepTime = Time;
		m_SleepPastTime = 0;
	}
	void PushNil()
	{
		lua_pushnil (m_pLuaThread);
	}
	void PushBoolean(bool Value)
	{
		lua_pushboolean(m_pLuaThread, Value ? 1 : 0);
	}
	void PushNumber(lua_Number Value)
	{
		lua_pushnumber(m_pLuaThread, Value);
	}
	void PushInteger(lua_Integer Value)
	{
		lua_pushinteger(m_pLuaThread, Value);
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
	void PushValue(T Value);
	bool PushPacketValue(const CSmartValue& Value);
	int PushPacket(const CSmartStruct& Packet);
	bool PushTablePacket(const CSmartStruct& Packet);
	//void Clear()
	//{
	//	m_pLuaScript = NULL;
	//	m_IsNeedYield = false;
	//	m_YieldType = LUA_YIELD_TYPE_NONE;
	//	m_YeildReturnCount = 0;
	//	m_LastLuaStatus = LUA_OK;
	//	m_Param1 = NULL;
	//	m_Param2 = NULL;
	//	m_pTimeManager = NULL;
	//	m_SleepTime = 0;
	//	m_SleepPastTime = 0;
	//	if (m_pLuaThread)
	//		lua_pop(m_pLuaThread, lua_gettop(m_pLuaThread));
	//}

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
		return lua_gettop(m_pLuaThread) - 2;
	}
	int GetValueType(int Index)
	{
		return GetLuaObjectType(m_pLuaThread, Index);
	}
	bool PackResult(CSmartStruct& Packet, int SkillCount);
	bool PackTable(CSmartStruct& Packet, int Index);
	template<typename T>
	T PopValue();
	template<typename T>
	T GetValue(int Index);
	int GetValueCount()
	{
		return lua_gettop(m_pLuaThread);
	}
	int MoveResultToLThread(CLuaThread * pLThread, LUA_META_CLASS_COPY_FUNCTION pExtendCopyFN = NULL);
	//int CopyStack(CLuaThread * pTargetLThread, int StartIndex, int Count, LUA_META_CLASS_COPY_FUNCTION pExtendCopyFN = NULL);
	void Update();
	static bool IsSiblingThread(CLuaThread * pThread1, CLuaThread * pThread2);
};

