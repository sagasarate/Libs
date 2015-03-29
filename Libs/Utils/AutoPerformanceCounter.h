/****************************************************************************/
/*                                                                          */
/*      文件名:    AutoPerformanceCounter.h                                 */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

//inline __int64 QueryPerformanceCounterFast()
//{
//	__asm{RDTSC}
//}

class CAutoPerformanceCounter
{
protected:	
	CPerformanceStatistician *	m_pPS;
	int							m_Index;
	UINT64						m_PerformanceStart;
	UINT64						m_PerformanceEnd;
public:
	inline CAutoPerformanceCounter(CPerformanceStatistician * pPS,int Index)
	{		
		m_pPS=pPS;
		m_Index=Index;
		//m_PerformanceStart.QuadPart=QueryPerformanceCounterFast();
		//QueryPerformanceCounter(&m_PerformanceStart);		
		m_PerformanceStart=CEasyTimerEx::GetTime();
		m_pPS->StartPerformanceCount(m_Index);
	}
	inline ~CAutoPerformanceCounter(void)
	{
		//m_PerformanceEnd.QuadPart=QueryPerformanceCounterFast();
		//QueryPerformanceCounter(&m_PerformanceEnd);
		m_PerformanceEnd=CEasyTimerEx::GetTime();
		m_pPS->AddPerformanceCount(m_Index,(UINT)((m_PerformanceEnd-m_PerformanceStart)/PERFORMANCE_UNIT));
	}
};


#ifdef PERFORMANCE_STAT

#define BLOCK_BEGIN(Name)			{static int s_BlockIndex=0;static CPerformanceStatistician * s_pPS=NULL;if(!s_BlockIndex) {s_pPS=CPerformanceStatistician::GetInstance();s_BlockIndex=s_pPS->GetFunctionIndex(s_BlockIndex,Name);} CAutoPerformanceCounter tmpPerformanceCounter(s_pPS,s_BlockIndex);
#define BLOCK_END					}

#define FUNCTION_BEGIN				static int s_FunctionIndex=0;static CPerformanceStatistician * s_pPS=NULL;if(!s_FunctionIndex) {s_pPS=CPerformanceStatistician::GetInstance();s_FunctionIndex=s_pPS->GetFunctionIndex(s_FunctionIndex,__FUNCTION__);} CAutoPerformanceCounter tmpPerformanceCounter(s_pPS,s_FunctionIndex);
#define FUNCTION_END

#else

#define BLOCK_BEGIN(Name)
#define BLOCK_END

#define FUNCTION_BEGIN
#define FUNCTION_END

#endif