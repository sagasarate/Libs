#include "stdafx.h"



CTimerQueue::CTimerQueue()
{
}

CTimerQueue::CTimerQueue(UINT Size, UINT GrowSize, UINT GrowLimit)
{
	Create(Size, GrowSize, GrowLimit);
}

CTimerQueue::~CTimerQueue()
{
}


bool CTimerQueue::Create(UINT Size, UINT GrowSize, UINT GrowLimit)
{
	if (m_TimerQueue.Create(Size, GrowSize, GrowLimit))
		return true;
	m_Tree.Create(Size, GrowSize);
	return false;
}

void CTimerQueue::Clear()
{
	m_TimerQueue.Clear();
}

UINT CTimerQueue::AddTimer(UINT TimeOut, void * Param)
{
	UINT CurTime = CEasyTimer::GetTime();
	UINT EndTime = CurTime + TimeOut;
	TIMER_INFO * pTimerInfo = NULL;
	UINT ID = m_TimerQueue.NewObject(&pTimerInfo);
	if (ID)
	{
		pTimerInfo->ID = ID;
		pTimerInfo->StartTime = CurTime;
		pTimerInfo->EndTime = EndTime;
		pTimerInfo->TimeOutTime = TimeOut;
		pTimerInfo->Param = Param;

		int Index = SearchTimer(EndTime, true);
		m_Tree.Insert(Index, pTimerInfo);
		return ID;
	}
	return 0;
}

bool CTimerQueue::DeleteTimer(UINT ID)
{
	TIMER_INFO * pTimerInfo = m_TimerQueue.GetObject(ID);
	if (pTimerInfo)
	{
		int Index = SearchTimer(pTimerInfo->EndTime, false);
		if (Index >= 0)
		{
			for (; Index >= 0; Index--)
			{
				if (m_Tree[Index] == pTimerInfo)
				{
					m_Tree.Delete(Index);
					m_TimerQueue.DeleteObject(ID);
					return true;
				}
			}
		}
	}	
	return false;
}
void * CTimerQueue::UpdateTimer()
{
	return UpdateTimer(CEasyTimer::GetTime());
}
void * CTimerQueue::UpdateTimer(UINT CurTime)
{
	int Ptr = (int)m_Tree.GetCount() - 1;
	if (Ptr >= 0)
	{
		TIMER_INFO * pTimerInfo = m_Tree[Ptr];
		if (pTimerInfo->EndTime <= CurTime)
		{
			void * Param = pTimerInfo->Param;
			m_Tree.Delete(Ptr);
			m_TimerQueue.DeleteObject(pTimerInfo->ID);
			return Param;
		}
	}
	return NULL;
}

int CTimerQueue::SearchTimer(UINT Time,bool ForInsert)
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
		if (Start >= 0)
			return Start;
		else
			return 0;
	}
		
	return -1;
}