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
	enum LUA_THREAD_TYPE
	{
		LUA_THREAD_TYPE_NORMAL,
		LUA_THREAD_TYPE_MAIN,
	};
protected:
	UINT				m_ID;
	CBaseLuaVM *		m_pLuaVM;
	void *				m_Param1;
	void *				m_Param2;
	CBaseScriptHost *	m_pHostObject;
	lua_State *			m_pLuaState;
	int					m_ThreadType;
	UINT				m_StartStackLen;
	bool				m_IsNeedYield;
	int					m_LastLuaStatus;
	CEasyTimer			m_YieldTimer;
	int					m_YieldType;
	int					m_YieldReturnCount;
	CEasyTimer			m_SleepTimer;
	ITimeManager *		m_pTimeManager;
	float				m_SleepTime;
	float				m_SleepPastTime;
	int					m_StackBeforeCall;	
public:
	CLuaThread()
	{
		m_ID = 0;
		m_pLuaVM = NULL;
		m_Param1 = NULL;
		m_Param2 = NULL;
		m_pHostObject = NULL;
		m_pLuaState = NULL;
		m_StartStackLen = 0;
		m_ThreadType = LUA_THREAD_TYPE_NORMAL;
		m_IsNeedYield = false;
		m_LastLuaStatus = LUA_OK;		
		m_YieldType = LUA_RESERVE_YIELD_TYPE_NONE;
		m_YieldReturnCount = 0;		
		m_pTimeManager = NULL;
		m_SleepTime = 0;
		m_SleepPastTime = 0;
		m_StackBeforeCall = -1;
	}
	virtual ~CLuaThread()
	{
		//Destory();
	}
	bool IsInited()
	{
		return m_pLuaState != NULL;
	}
	bool Init(CBaseLuaVM * pLuaVM, int StackSize);
	bool CreateMainThread(CBaseLuaVM* pLuaVM, int StackSize);
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
		return m_pLuaState;
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
		m_YieldType = LUA_RESERVE_YIELD_TYPE_NONE;
		m_YieldReturnCount = 0;
	}
	bool IsYieldTimeOut()
	{
		return m_YieldTimer.IsTimeOut();
	}
	void DoSleep(UINT Time)
	{
		m_YieldType = LUA_RESERVE_YIELD_TYPE_SLEEP;
		m_SleepTimer.SetTimeOut(Time);
	}
	void DoSleep(ITimeManager * pTimeManager,float Time)
	{
		m_YieldType = LUA_RESERVE_YIELD_TYPE_SLEEP;
		m_pTimeManager = pTimeManager;
		m_SleepTime = Time;
		m_SleepPastTime = 0;
	}
	void ClearStack()
	{
		UINT CurLen = lua_gettop(m_pLuaState);
		if (CurLen > m_StartStackLen)
			lua_pop(m_pLuaState, (int)(CurLen - m_StartStackLen));
	}
	void PushNil()
	{
		lua_pushnil (m_pLuaState);
	}
	void PushBoolean(bool Value)
	{
		lua_pushboolean(m_pLuaState, Value ? 1 : 0);
	}
	void PushNumber(lua_Number Value)
	{
		lua_pushnumber(m_pLuaState, Value);
	}
	void PushInteger(lua_Integer Value)
	{
		lua_pushinteger(m_pLuaState, Value);
	}
	void PushString(LPCSTR Value);
	void PushString(LPCWSTR Value);
	void Pop(int Count)
	{
		lua_pop(m_pLuaState, Count);
	}
	template<typename T>
	void PushValue(T Value);
	bool PushPacketValue(const CSmartValue& Value);
	int PushPacket(const CSmartStruct& Packet);
	bool PushTablePacket(const CSmartArray& Packet);
	template<typename T>
	bool PushArray(const CEasyArray<T>& ArrayList, UINT StartIndex = 0, UINT PushCount = 0);
	bool PushJson(LPCTSTR JsonStr);
	bool GetJson(CEasyString& JsonStr, int Index, bool IsPretty);
	bool Prepare(CBaseScriptHost * pObject, LPCSTR szFunctionName);
	inline bool Prepare(CBaseScriptHost * pObject, LPCWSTR szFunctionName)
	{
		CEasyStringA StringA;
		StringA = szFunctionName;
		return Prepare(pObject, (LPCSTR)StringA);
	}
	int DoCall(int ParamCount);		
	int DoCall(LPCTSTR ScriptName, LPCTSTR ScriptContent);
	CEasyString GetErrorString(int Idx = -1)
	{
		CEasyString Msg;
		LuaStrToSystemStr(lua_tostring(m_pLuaState, Idx), Msg);
		return Msg;
	}	
	int GetValueType(int Index)
	{
		return GetLuaObjectType(m_pLuaState, Index);
	}
	bool PackResult(CSmartStruct& Packet, int SkillCount);
	bool PackTable(CSmartArray& Packet, int Index);
	template<typename T>
	T PopValue();
	template<typename T>
	T GetValue(int Index);
	int GetValueCount()
	{
		return lua_gettop(m_pLuaState);
	}	
	void StartCall()
	{
		m_StackBeforeCall = lua_gettop(m_pLuaState);
	}
	int GetResultCount()
	{
		if (m_StackBeforeCall >= 0)
			return lua_gettop(m_pLuaState) - m_StackBeforeCall;
		return 0;
	}
	//int MoveResultToLThread(CLuaThread * pLThread, LUA_META_CLASS_COPY_FUNCTION pExtendCopyFN = NULL);
	//int CopyStack(CLuaThread * pTargetLThread, int StartIndex, int Count, LUA_META_CLASS_COPY_FUNCTION pExtendCopyFN = NULL);
	int Update(int ProcessLimit = 32);
	static bool IsSiblingThread(CLuaThread * pThread1, CLuaThread * pThread2);
	static int LuaErrParser(lua_State* L);
};

