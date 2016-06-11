#pragma once
#include "NameObject.h"
class CTimerQueue 
{
protected:	
	struct TIMER_INFO
	{
		UINT	ID;
		UINT	StartTime;
		UINT	EndTime;
		UINT	TimeOutTime;
		void *	Param;
	};

	CIDStorage<TIMER_INFO>		m_TimerQueue;
	CEasyArray<TIMER_INFO*>		m_Tree;
public:
	CTimerQueue();
	CTimerQueue(UINT Size, UINT GrowSize = 0, UINT GrowLimit = 0);
	~CTimerQueue();

	bool Create(UINT Size, UINT GrowSize = 0, UINT GrowLimit = 0);
	void Clear();
	UINT AddTimer(UINT TimeOut,void * Param);

	bool DeleteTimer(UINT ID);
	void * UpdateTimer();
	void * UpdateTimer(UINT CurTime);

protected:
	int SearchTimer(UINT Time, bool ForInsert);
};

