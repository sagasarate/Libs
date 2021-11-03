#pragma once

template<typename T>
class CTimerQueue 
{
protected:	
	struct TIMER_INFO
	{
		UINT	ID;
		UINT	TimeOutTime;
		bool	IsRepeat;
		UINT	StartTime;
		UINT	EndTime;
		T		UserData;
	};
	CIDStorage<TIMER_INFO>		m_TimerQueue;
	CEasyArray<TIMER_INFO*>		m_Tree;
public:
	CTimerQueue()
	{
		m_TimerQueue.SetTag(_T("CTimerQueue"));
		m_Tree.SetTag(_T("CTimerQueue"));
	}
	CTimerQueue(UINT Size, UINT GrowSize = 0, UINT GrowLimit = 0)
	{
		m_TimerQueue.SetTag(_T("CTimerQueue"));
		m_Tree.SetTag(_T("CTimerQueue"));
		Create(Size, GrowSize, GrowLimit);
	}
	~CTimerQueue()
	{

	}

	bool Create(UINT Size, UINT GrowSize = 0, UINT GrowLimit = 0)
	{
		if (m_TimerQueue.Create(Size, GrowSize, GrowLimit))
		{
			if (GrowSize < 32)
				GrowSize = 32;
			m_Tree.Create(GrowSize, GrowSize);
			return true;
		}
		return false;
	}
	bool Create(const STORAGE_POOL_SETTING& PoolSetting)
	{
		return Create(PoolSetting.StartSize, PoolSetting.GrowSize, PoolSetting.GrowLimit);
	}
	void Clear()
	{
		m_TimerQueue.Clear();
		m_Tree.Clear();
	}
	UINT AddTimer(UINT CurTime, UINT TimeOut, T& UserData, bool IsRepeat)
	{
		UINT EndTime = CurTime + TimeOut;
		TIMER_INFO * pTimerInfo = NULL;
		UINT ID = m_TimerQueue.NewObject(&pTimerInfo);
		if (ID)
		{
			pTimerInfo->ID = ID;
			pTimerInfo->StartTime = CurTime;
			pTimerInfo->EndTime = EndTime;
			pTimerInfo->TimeOutTime = TimeOut;
			pTimerInfo->UserData = UserData;
			pTimerInfo->IsRepeat = IsRepeat;

			InsertTimer(pTimerInfo);
			return ID;
		}
		return 0;
	}
	UINT AddTimer(UINT TimeOut, T& UserData, bool IsRepeat)
	{
		return AddTimer(CEasyTimer::GetTime(), TimeOut, UserData, IsRepeat);
	}

	bool DeleteTimer(UINT ID, T * pUserData)
	{
		TIMER_INFO * pTimerInfo = m_TimerQueue.GetObject(ID);
		if (pTimerInfo)
		{
			if (pUserData)
				*pUserData = pTimerInfo->UserData;
			int Index = SearchTimer(pTimerInfo->EndTime, false);
			if (Index >= 0)
			{
				int Ptr = Index;
				for (; Ptr >= 0; Ptr--)
				{
					if (m_Tree[Ptr] == pTimerInfo)
					{
						m_Tree.Delete(Ptr);
						m_TimerQueue.DeleteObject(ID);
						return true;
					}
					else if (m_Tree[Ptr]->EndTime != pTimerInfo->EndTime)
					{
						break;
					}
				}
				Ptr = Index + 1;
				for (; Ptr < (int)m_Tree.GetCount(); Ptr++)
				{
					if (m_Tree[Ptr] == pTimerInfo)
					{
						m_Tree.Delete(Ptr);
						m_TimerQueue.DeleteObject(ID);
						return true;
					}
					else if (m_Tree[Ptr]->EndTime != pTimerInfo->EndTime)
					{
						break;
					}
				}
			}
		}
		return false;
	}
	UINT UpdateTimer(T * pUserData, bool * pIsRepeat)
	{
		return UpdateTimer(CEasyTimer::GetTime(), pUserData, pIsRepeat);
	}
	UINT UpdateTimer(UINT CurTime, T * pUserData, bool * pIsRepeat)
	{
		int Ptr = (int)m_Tree.GetCount() - 1;
		if (Ptr >= 0)
		{
			TIMER_INFO * pTimerInfo = m_Tree[Ptr];
			if (pTimerInfo->EndTime <= CurTime)
			{
				UINT ID = pTimerInfo->ID;
				if (pUserData)
					*pUserData = pTimerInfo->UserData;
				if (pIsRepeat)
					*pIsRepeat = pTimerInfo->IsRepeat;
				m_Tree.Delete(Ptr);
				if (pTimerInfo->IsRepeat)
				{
					UINT EndTime = CurTime + pTimerInfo->TimeOutTime;
					pTimerInfo->StartTime = CurTime;
					pTimerInfo->EndTime = EndTime;
					InsertTimer(pTimerInfo);
				}
				else
				{
					m_TimerQueue.DeleteObject(pTimerInfo->ID);
				}
				return ID;
			}
		}
		return 0;
	}
	void * GetFirstTimerPos()
	{
		return m_TimerQueue.GetFirstObjectPos();
	}
	UINT GetNextTimer(LPVOID& Pos, T * pUserData)
	{
		TIMER_INFO * pTimerInfo = m_TimerQueue.GetNextObject(Pos);
		if (pTimerInfo)
		{
			if (pUserData)
				*pUserData = pTimerInfo->UserData;
			return pTimerInfo->ID;
		}
		return 0;
	}
protected:
	int SearchTimer(UINT Time, bool ForInsert)
	{
		int Start = (int)m_Tree.GetCount() - 1;
		int End = 0;
		while (Start >= End)
		{
			int Mid = (Start + End) / 2;
			if (Time == m_Tree[Mid]->EndTime)
			{
				return Mid;
			}
			else if (Time > m_Tree[Mid]->EndTime)
			{
				Start = Mid - 1;
			}
			else
			{
				End = Mid + 1;
			}
		}
		if (ForInsert)
		{
			return Start + 1;
		}

		return -1;
	}
	void InsertTimer(TIMER_INFO * pTimerInfo)
	{
		int Index = SearchTimer(pTimerInfo->EndTime, true);
		if (Index >= (int)m_Tree.GetCount())
			m_Tree.Add(pTimerInfo);
		else
			m_Tree.Insert(Index, pTimerInfo);
	}
};

