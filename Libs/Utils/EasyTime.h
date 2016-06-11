/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyTime.h                                               */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

#define TIME_FORMAT_BUFFER_LEN		512

class CEasyTime
{
protected:
	WORD m_wYear;
	WORD m_wDayOfYear;
	WORD m_wMonth;
	WORD m_wDayOfWeek;
	WORD m_wDay;
	WORD m_wHour;
	WORD m_wMinute;
	WORD m_wSecond;
	WORD m_wMilliseconds;
public:
	CEasyTime()
	{
		Clear();
	}
	CEasyTime(const CEasyTime& Time)
	{
		m_wYear=Time.m_wYear;
		m_wDayOfYear=Time.m_wDayOfYear;
		m_wMonth=Time.m_wMonth;
		m_wDayOfWeek=Time.m_wDayOfWeek;
		m_wDay=Time.m_wDay;
		m_wHour=Time.m_wHour;
		m_wMinute=Time.m_wMinute;
		m_wSecond=Time.m_wSecond;
		m_wMilliseconds=Time.m_wMilliseconds;
	}
	CEasyTime(LPCTSTR szTime)
	{
		Clear();
		FromString(szTime);
	}
#ifdef WIN32
	CEasyTime(const SYSTEMTIME& Time)
	{
		m_wYear=Time.wYear;
		m_wDayOfYear=0;
		m_wMonth=Time.wMonth;
		m_wDayOfWeek=Time.wDayOfWeek;
		m_wDay=Time.wDay;
		m_wHour=Time.wHour;
		m_wMinute=Time.wMinute;
		m_wSecond=Time.wSecond;
		m_wMilliseconds=Time.wMilliseconds;
	}
	CEasyTime(const FILETIME& Time)
	{
		SYSTEMTIME SysTime;
		if(FileTimeToSystemTime(&Time,&SysTime))
		{
			m_wYear = SysTime.wYear;
			m_wDayOfYear = 0;
			m_wMonth = SysTime.wMonth;
			m_wDayOfWeek = SysTime.wDayOfWeek;
			m_wDay = SysTime.wDay;
			m_wHour = SysTime.wHour;
			m_wMinute = SysTime.wMinute;
			m_wSecond = SysTime.wSecond;
			m_wMilliseconds = SysTime.wMilliseconds;
		}
	}
#endif
	CEasyTime(const tm& Time)
	{
		m_wYear=1900+Time.tm_year;
		m_wDayOfYear=Time.tm_yday;
		m_wMonth=Time.tm_mon+1;
		m_wDayOfWeek=Time.tm_wday;
		m_wDay=Time.tm_mday;
		m_wHour=Time.tm_hour;
		m_wMinute=Time.tm_min;
		m_wSecond=Time.tm_sec;
		m_wMilliseconds=0;
	}
	CEasyTime(const time_t Time)
	{
		tm Tm;
		localtime_s(&Tm,&Time);
		m_wYear=1900+Tm.tm_year;
		m_wDayOfYear=Tm.tm_yday;
		m_wMonth=Tm.tm_mon+1;
		m_wDayOfWeek=Tm.tm_wday;
		m_wDay=Tm.tm_mday;
		m_wHour=Tm.tm_hour;
		m_wMinute=Tm.tm_min;
		m_wSecond=Tm.tm_sec;
		m_wMilliseconds=0;
	}
	~CEasyTime()
	{

	}
	bool IsValidate() const
	{
		return m_wYear!=0||m_wDayOfYear!=0||m_wMonth!=0||m_wDayOfWeek!=0||
			m_wDay!=0||m_wHour!=0||m_wMinute!=0||m_wSecond!=0;
	}
	void Clear()
	{
		m_wYear=0;
		m_wDayOfYear=0;
		m_wMonth=0;
		m_wDayOfWeek=0;
		m_wDay=0;
		m_wHour=0;
		m_wMinute=0;
		m_wSecond=0;
		m_wMilliseconds=0;
	}
#ifdef WIN32
	CEasyTime& operator=(const SYSTEMTIME& Time)
	{
		m_wYear=Time.wYear;
		m_wDayOfYear=0;
		m_wMonth=Time.wMonth;
		m_wDayOfWeek=Time.wDayOfWeek;
		m_wDay=Time.wDay;
		m_wHour=Time.wHour;
		m_wMinute=Time.wMinute;
		m_wSecond=Time.wSecond;
		m_wMilliseconds=Time.wMilliseconds;

		return *this;
	}
	CEasyTime& operator=(const FILETIME& Time)
	{
		SYSTEMTIME SysTime;
		if(FileTimeToSystemTime(&Time,&SysTime))
		{
			m_wYear = SysTime.wYear;
			m_wDayOfYear = 0;
			m_wMonth = SysTime.wMonth;
			m_wDayOfWeek = SysTime.wDayOfWeek;
			m_wDay = SysTime.wDay;
			m_wHour = SysTime.wHour;
			m_wMinute = SysTime.wMinute;
			m_wSecond = SysTime.wSecond;
			m_wMilliseconds = SysTime.wMilliseconds;
		}

		return *this;
	}
	void GetTime(SYSTEMTIME& Time) const
	{
		Time.wYear=m_wYear;
		Time.wMonth=m_wMonth;
		Time.wDayOfWeek=m_wDayOfWeek;
		Time.wDay=m_wDay;
		Time.wHour=m_wHour;
		Time.wMinute=m_wMinute;
		Time.wSecond=m_wSecond;
		Time.wMilliseconds=m_wMilliseconds;
	}
	void GetTime(FILETIME& Time) const
	{
		SYSTEMTIME SysTime;
		SysTime.wYear=m_wYear;
		SysTime.wMonth=m_wMonth;
		SysTime.wDayOfWeek=m_wDayOfWeek;
		SysTime.wDay=m_wDay;
		SysTime.wHour=m_wHour;
		SysTime.wMinute=m_wMinute;
		SysTime.wSecond=m_wSecond;
		SysTime.wMilliseconds=m_wMilliseconds;
		SystemTimeToFileTime(&SysTime,&Time);
	}
	operator SYSTEMTIME() const
	{
		SYSTEMTIME Time;
		GetTime(Time);
		return Time;
	}
	operator FILETIME() const
	{
		FILETIME Time;
		GetTime(Time);
		return Time;
	}
#endif
	CEasyTime& operator=(const tm& Time)
	{
		m_wYear=1900+Time.tm_year;
		m_wDayOfYear=Time.tm_yday;
		m_wMonth=Time.tm_mon+1;
		m_wDayOfWeek=Time.tm_wday;
		m_wDay=Time.tm_mday;
		m_wHour=Time.tm_hour;
		m_wMinute=Time.tm_min;
		m_wSecond=Time.tm_sec;
		m_wMilliseconds=0;

		return *this;
	}
	CEasyTime& operator=(const time_t Time)
	{
		if(Time)
		{
			tm Tm;
			localtime_s(&Tm,&Time);
			m_wYear=1900+Tm.tm_year;
			m_wDayOfYear=Tm.tm_yday;
			m_wMonth=Tm.tm_mon+1;
			m_wDayOfWeek=Tm.tm_wday;
			m_wDay=Tm.tm_mday;
			m_wHour=Tm.tm_hour;
			m_wMinute=Tm.tm_min;
			m_wSecond=Tm.tm_sec;
			m_wMilliseconds=0;
		}
		else
		{
			Clear();
		}

		return *this;
	}
	CEasyTime& operator=(const CEasyTime& Time)
	{
		m_wYear=Time.m_wYear;
		m_wDayOfYear=Time.m_wDayOfYear;
		m_wMonth=Time.m_wMonth;
		m_wDayOfWeek=Time.m_wDayOfWeek;
		m_wDay=Time.m_wDay;
		m_wHour=Time.m_wHour;
		m_wMinute=Time.m_wMinute;
		m_wSecond=Time.m_wSecond;
		m_wMilliseconds=Time.m_wMilliseconds;

		return *this;
	}
	void GetTime(tm& Time) const
	{
		Time.tm_year=m_wYear-1900;
		Time.tm_yday=m_wDayOfYear;
		Time.tm_mon=m_wMonth-1;
		Time.tm_wday=m_wDayOfWeek;
		Time.tm_mday=m_wDay;
		Time.tm_hour=m_wHour;
		Time.tm_min=m_wMinute;
		Time.tm_sec=m_wSecond;
	}
	void GetTime(time_t& Time) const
	{
		if(IsValidate())
		{
			tm Tm;
			Tm.tm_year=m_wYear-1900;
			Tm.tm_yday=m_wDayOfYear;
			Tm.tm_mon=m_wMonth-1;
			Tm.tm_wday=m_wDayOfWeek;
			Tm.tm_mday=m_wDay;
			Tm.tm_hour=m_wHour;
			Tm.tm_min=m_wMinute;
			Tm.tm_sec=m_wSecond;

			Time=mktime(&Tm);
		}
		else
		{
			Time=0;
		}
	}
	operator tm() const
	{
		tm Time;
		GetTime(Time);
		return Time;
	}
	operator time_t() const
	{
		time_t Time;
		GetTime(Time);
		return Time;
	}
#ifdef WIN32
	void FetchLocalTime()
	{
		SYSTEMTIME SysTime;
		::GetLocalTime(&SysTime);
		*this=SysTime;
	}
	void FetchSystemTime()
	{
		SYSTEMTIME SysTime;
		::GetSystemTime(&SysTime);
		*this=SysTime;
	}
	void ToSystemTime()
	{
		FILETIME LocalTime=*this;
		FILETIME SystemTime;
		LocalFileTimeToFileTime(&LocalTime,&SystemTime);
		*this=SystemTime;
	}
	void ToLocalTime()
	{
		FILETIME LocalTime;
		FILETIME SystemTime=*this;
		FileTimeToLocalFileTime(&SystemTime,&LocalTime);
		*this=SystemTime;
	}
#else
	void FetchLocalTime()
	{
		tm Tm;
		time_t Time=time(NULL);
		localtime_s(&Tm,&Time);
		m_wYear=1900+Tm.tm_year;
		m_wDayOfYear=Tm.tm_yday;
		m_wMonth=Tm.tm_mon+1;
		m_wDayOfWeek=Tm.tm_wday;
		m_wDay=Tm.tm_mday;
		m_wHour=Tm.tm_hour;
		m_wMinute=Tm.tm_min;
		m_wSecond=Tm.tm_sec;
		m_wMilliseconds=0;

	}
	void FetchSystemTime()
	{
		tm Tm;
		time_t Time=time(NULL);
		gmtime_s(&Tm,&Time);
		m_wYear=1900+Tm.tm_year;
		m_wDayOfYear=Tm.tm_yday;
		m_wMonth=Tm.tm_mon+1;
		m_wDayOfWeek=Tm.tm_wday;
		m_wDay=Tm.tm_mday;
		m_wHour=Tm.tm_hour;
		m_wMinute=Tm.tm_min;
		m_wSecond=Tm.tm_sec;
		m_wMilliseconds=0;
	}
	void ToSystemTime()
	{
	}
	void ToLocalTime()
	{
	}
#endif
	bool operator==(const CEasyTime& Time) const
	{
		return m_wYear==Time.m_wYear&&
			m_wMonth==Time.m_wMonth&&
			m_wDay==Time.m_wDay&&
			m_wHour==Time.m_wHour&&
			m_wMinute==Time.m_wMinute&&
			m_wSecond==Time.m_wSecond&&
			m_wMilliseconds==Time.m_wMilliseconds;
	}
	bool operator!=(const CEasyTime& Time) const
	{
		return m_wYear!=Time.m_wYear||
			m_wMonth!=Time.m_wMonth||
			m_wDay!=Time.m_wDay||
			m_wHour!=Time.m_wHour||
			m_wMinute!=Time.m_wMinute||
			m_wSecond!=Time.m_wSecond||
			m_wMilliseconds!=Time.m_wMilliseconds;
	}
	bool operator>(const CEasyTime& Time) const
	{
		time_t Time1,Time2;
		UINT64 BigTime1,BigTime2;
		GetTime(Time1);
		Time.GetTime(Time2);
		BigTime1=Time1*1000+m_wMilliseconds;
		BigTime2=Time2*1000+Time.m_wMilliseconds;
		return BigTime1>BigTime2;

	}
	bool operator<(const CEasyTime& Time) const
	{
		time_t Time1,Time2;
		UINT64 BigTime1,BigTime2;
		GetTime(Time1);
		Time.GetTime(Time2);
		BigTime1=Time1*1000+m_wMilliseconds;
		BigTime2=Time2*1000+Time.m_wMilliseconds;
		return BigTime1<BigTime2;
	}
	bool operator>=(const CEasyTime& Time) const
	{
		time_t Time1,Time2;
		UINT64 BigTime1,BigTime2;
		GetTime(Time1);
		Time.GetTime(Time2);
		BigTime1=Time1*1000+m_wMilliseconds;
		BigTime2=Time2*1000+Time.m_wMilliseconds;
		return BigTime1>=BigTime2;
	}
	bool operator<=(const CEasyTime& Time) const
	{
		time_t Time1,Time2;
		UINT64 BigTime1,BigTime2;
		GetTime(Time1);
		Time.GetTime(Time2);
		BigTime1=Time1*1000+m_wMilliseconds;
		BigTime2=Time2*1000+Time.m_wMilliseconds;
		return BigTime1<=BigTime2;
	}
	WORD& Year()
	{
		return m_wYear;
	}
	WORD& Month()
	{
		return m_wMonth;
	}
	WORD& Day()
	{
		return m_wDay;
	}
	WORD& DayOfYear()
	{
		return m_wDayOfYear;
	}
	WORD& DayOfWeek()
	{
		return m_wDayOfWeek;
	}
	WORD& Hour()
	{
		return m_wHour;
	}
	WORD& Minute()
	{
		return m_wMinute;
	}
	WORD& Second()
	{
		return m_wSecond;
	}
	WORD& Milliseconds()
	{
		return m_wMilliseconds;
	}
	bool FromString(LPCTSTR szTime)
	{
		WORD Year;
		WORD Month;		
		WORD Day;
		WORD Hour;
		WORD Minute;
		WORD Second;
	
#pragma warning( push )
#pragma warning (disable : 4996)
		int Count=_stscanf(szTime,_T("%hu-%hu-%hu %hu:%hu:%hu"),&Year,&Month,&Day,&Hour,&Minute,&Second);
#pragma warning( pop )
		if(Count>=6)
		{
			m_wYear=Year;
			m_wDayOfYear=0;
			m_wMonth=Month;
			m_wDayOfWeek=0;
			m_wDay=Day;
			m_wHour=Hour;
			m_wMinute=Minute;
			m_wSecond=Second;
			m_wMilliseconds=0;
			return true;
		}
		return false;
	}
	LPCTSTR FormatStatic(LPCTSTR szFormat)
	{
		static TCHAR szFormatBuffer[TIME_FORMAT_BUFFER_LEN+1];

		Format(szFormatBuffer,TIME_FORMAT_BUFFER_LEN+1,szFormat);
		return szFormatBuffer;
	}
	size_t Format(LPTSTR szFormatBuffer,size_t BufferLen,LPCTSTR szFormat) const
	{
		tm Time;
		GetTime(Time);
		size_t Len=_tcsftime(szFormatBuffer,BufferLen-1,szFormat,&Time);
		if(szFormatBuffer)
			szFormatBuffer[BufferLen-1]=0;
		return Len;
	}
	void Format(CEasyString& FormatBuffer,LPCTSTR szFormat) const
	{
		FormatBuffer.Resize(TIME_FORMAT_BUFFER_LEN+1,false);
		size_t Len=Format((LPTSTR)FormatBuffer.GetBuffer(),TIME_FORMAT_BUFFER_LEN+1,szFormat);
		FormatBuffer.TrimBuffer(Len);
	}

	static int GetDayDiff(const CEasyTime& StartTime,const CEasyTime& EndTime)
	{
		time_t StartTime_t,EndTime_t;
		StartTime.GetTime(StartTime_t);
		EndTime.GetTime(EndTime_t);
		return (int)(EndTime_t/86400-StartTime_t/86400);
	}

	int GetDayToToday() const
	{
		CEasyTime Today;

		Today.FetchLocalTime();
		return GetDayDiff(*this,Today);
	}

	int GetDayFromToday() const
	{
		CEasyTime Today;

		Today.FetchLocalTime();
		return GetDayDiff(Today,*this);
	}

	static int GetMinuteDiff(const CEasyTime& StartTime,const CEasyTime& EndTime)
	{
		time_t StartTime_t,EndTime_t;
		StartTime.GetTime(StartTime_t);
		EndTime.GetTime(EndTime_t);
		return (int)(EndTime_t/60-StartTime_t/60);
	}

	int GetMinuteToNow() const
	{
		CEasyTime Today;

		Today.FetchLocalTime();
		return GetMinuteDiff(*this,Today);
	}

	int GetMinuteFromNow() const
	{
		CEasyTime Today;

		Today.FetchLocalTime();
		return GetMinuteDiff(Today,*this);
	}

	void AddDay(int Day)
	{
		time_t Time;
		GetTime(Time);
		Time+=Day*86400;
		*this=Time;
	}
	void AddMinute(int Minute)
	{
		time_t Time;
		GetTime(Time);
		Time+=Minute*60;
		*this=Time;
	}

	bool IsValidate()
	{
		return m_wYear<=6199&&m_wMonth>=1&&m_wMonth<=12&&m_wDay>=1&&m_wDay<=31&&m_wHour<=23&&m_wMinute<=59&&m_wSecond<=59;
	}

	static bool IsSameDate(const CEasyTime& Time1,const CEasyTime& Time2)
	{
		return Time1.m_wYear==Time2.m_wYear&&Time1.m_wMonth==Time2.m_wMonth&&Time1.m_wDay==Time2.m_wDay;
	}
};

