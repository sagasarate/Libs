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
public:
	enum TIME_MODE
	{
		TIME_MODE_LOCAL,
		TIME_MODE_UTC,
	};
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
	WORD m_TimeMode;
public:
	CEasyTime()
	{
		Clear();
	}
	CEasyTime(const CEasyTime& Time)
	{
		SetTime(Time);
	}
	CEasyTime(LPCTSTR szTime)
	{
		Clear();
		FromString(szTime);
	}
#ifdef WIN32
	CEasyTime(const SYSTEMTIME& Time)
	{
		SetTime(Time);
	}
	CEasyTime(const FILETIME& Time)
	{
		SetTime(Time);
	}
#endif
	CEasyTime(const tm& Time)
	{
		SetTime(Time);
	}
	CEasyTime(const time_t Time)
	{
		SetTime(Time);
	}
	~CEasyTime()
	{

	}	
	void Clear()
	{
		m_wYear = 0;
		m_wDayOfYear = 0;
		m_wMonth = 0;
		m_wDayOfWeek = 0;
		m_wDay = 0;
		m_wHour = 0;
		m_wMinute = 0;
		m_wSecond = 0;
		m_wMilliseconds = 0;
		m_TimeMode = (WORD)TIME_MODE_LOCAL;
	}
	TIME_MODE GetTimeMode()
	{
		return (TIME_MODE)m_TimeMode;
	}
#ifdef WIN32
	void SetTime(const SYSTEMTIME& Time, TIME_MODE TargetMode = TIME_MODE_LOCAL)
	{
		m_wYear = Time.wYear;
		m_wDayOfYear = 0;
		m_wMonth = Time.wMonth;
		m_wDayOfWeek = Time.wDayOfWeek;
		m_wDay = Time.wDay;
		m_wHour = Time.wHour;
		m_wMinute = Time.wMinute;
		m_wSecond = Time.wSecond;
		m_wMilliseconds = Time.wMilliseconds;
		m_TimeMode = (WORD)TargetMode;
	}
	CEasyTime& operator=(const SYSTEMTIME& Time)
	{
		SetTime(Time);
		return *this;
	}
	bool GetTime(SYSTEMTIME& Time, TIME_MODE TargetMode = TIME_MODE_LOCAL) const
	{
		if (m_TimeMode == TIME_MODE_UTC && TargetMode == TIME_MODE_LOCAL)
		{
			SYSTEMTIME SysTime;
			FILETIME UTCTime, LocalTime;
			SysTime.wYear = m_wYear;
			SysTime.wMonth = m_wMonth;
			SysTime.wDayOfWeek = m_wDayOfWeek;
			SysTime.wDay = m_wDay;
			SysTime.wHour = m_wHour;
			SysTime.wMinute = m_wMinute;
			SysTime.wSecond = m_wSecond;
			SysTime.wMilliseconds = m_wMilliseconds;
			if (!SystemTimeToFileTime(&SysTime, &UTCTime))
				return false;
			if (!FileTimeToLocalFileTime(&UTCTime, &LocalTime))
				return false;

			return FileTimeToSystemTime(&LocalTime, &Time) != FALSE;
		}
		else if (m_TimeMode == TIME_MODE_LOCAL && TargetMode == TIME_MODE_UTC)
		{
			SYSTEMTIME SysTime;
			FILETIME UTCTime, LocalTime;
			SysTime.wYear = m_wYear;
			SysTime.wMonth = m_wMonth;
			SysTime.wDayOfWeek = m_wDayOfWeek;
			SysTime.wDay = m_wDay;
			SysTime.wHour = m_wHour;
			SysTime.wMinute = m_wMinute;
			SysTime.wSecond = m_wSecond;
			SysTime.wMilliseconds = m_wMilliseconds;
			if (!SystemTimeToFileTime(&SysTime, &LocalTime))
				return false;
			if (!LocalFileTimeToFileTime(&LocalTime, &UTCTime))
				return false;

			return FileTimeToSystemTime(&UTCTime, &Time) != FALSE;
		}
		else
		{
			Time.wYear = m_wYear;
			Time.wMonth = m_wMonth;
			Time.wDayOfWeek = m_wDayOfWeek;
			Time.wDay = m_wDay;
			Time.wHour = m_wHour;
			Time.wMinute = m_wMinute;
			Time.wSecond = m_wSecond;
			Time.wMilliseconds = m_wMilliseconds;
			return true;
		}
	}
	operator SYSTEMTIME() const
	{
		SYSTEMTIME Time;
		GetTime(Time);
		return Time;
	}

	bool SetTime(const FILETIME& Time, TIME_MODE SourceMode = TIME_MODE_UTC, TIME_MODE TargetMode = TIME_MODE_LOCAL)
	{
		SYSTEMTIME SysTime;
		if (SourceMode == TIME_MODE_UTC && TargetMode == TIME_MODE_LOCAL)
		{
			FILETIME InputTime;
			if (!FileTimeToLocalFileTime(&Time, &InputTime))
				return false;
			if (!FileTimeToSystemTime(&InputTime, &SysTime))
				return false;
		}
		else if (SourceMode == TIME_MODE_LOCAL && TargetMode == TIME_MODE_UTC)
		{
			FILETIME InputTime;
			if (!LocalFileTimeToFileTime(&Time, &InputTime))
				return false;
			if (!FileTimeToSystemTime(&InputTime, &SysTime))
				return false;
		}
		else
		{
			if (!FileTimeToSystemTime(&Time, &SysTime))
				return false;
		}
		
		m_wYear = SysTime.wYear;
		m_wDayOfYear = 0;
		m_wMonth = SysTime.wMonth;
		m_wDayOfWeek = SysTime.wDayOfWeek;
		m_wDay = SysTime.wDay;
		m_wHour = SysTime.wHour;
		m_wMinute = SysTime.wMinute;
		m_wSecond = SysTime.wSecond;
		m_wMilliseconds = SysTime.wMilliseconds;
		m_TimeMode = (WORD)TargetMode;
		return true;
	}	
	CEasyTime& operator=(const FILETIME& Time)
	{
		SetTime(Time);
		return *this;
	}	
	bool GetTime(FILETIME& Time, TIME_MODE TargetMode = TIME_MODE_UTC) const
	{
		SYSTEMTIME SysTime;
		FILETIME FileTime;
		SysTime.wYear=m_wYear;
		SysTime.wMonth=m_wMonth;
		SysTime.wDayOfWeek=m_wDayOfWeek;
		SysTime.wDay=m_wDay;
		SysTime.wHour=m_wHour;
		SysTime.wMinute=m_wMinute;
		SysTime.wSecond=m_wSecond;
		SysTime.wMilliseconds=m_wMilliseconds;
		if (SystemTimeToFileTime(&SysTime, &FileTime))
		{
			if (m_TimeMode == TIME_MODE_UTC && TargetMode == TIME_MODE_LOCAL)
				return FileTimeToLocalFileTime(&FileTime, &Time) != FALSE;
			else if (m_TimeMode == TIME_MODE_LOCAL && TargetMode == TIME_MODE_UTC)
				return LocalFileTimeToFileTime(&FileTime, &Time) != FALSE;
			else
			{
				Time = FileTime;
				return true;
			}
		}
		return false;
	}	
	operator FILETIME() const
	{
		FILETIME Time;
		GetTime(Time);
		return Time;
	}
#endif
	void SetTime(const tm& Time, TIME_MODE TargetMode = TIME_MODE_LOCAL)
	{
		m_wYear = 1900 + Time.tm_year;
		m_wDayOfYear = Time.tm_yday;
		m_wMonth = Time.tm_mon + 1;
		m_wDayOfWeek = Time.tm_wday;
		m_wDay = Time.tm_mday;
		m_wHour = Time.tm_hour;
		m_wMinute = Time.tm_min;
		m_wSecond = Time.tm_sec;
		m_wMilliseconds = 0;
		m_TimeMode = TargetMode;
	}
	CEasyTime& operator=(const tm& Time)
	{
		SetTime(Time);
		return *this;
	}
	bool GetTime(tm& Time, TIME_MODE TargetMode = TIME_MODE_LOCAL) const
	{
		if (m_TimeMode == TargetMode)
		{
			Time.tm_year = m_wYear - 1900;
			Time.tm_yday = m_wDayOfYear;
			Time.tm_mon = m_wMonth - 1;
			Time.tm_wday = m_wDayOfWeek;
			Time.tm_mday = m_wDay;
			Time.tm_hour = m_wHour;
			Time.tm_min = m_wMinute;
			Time.tm_sec = m_wSecond;
			return true;
		}
		else if (TargetMode == TIME_MODE_LOCAL)
		{
			time_t UTCTime;
			GetTime(UTCTime);
			return localtime_s(&Time, &UTCTime) == 0;
		}		
		else
		{
			time_t UTCTime;
			GetTime(UTCTime);
			return gmtime_s(&Time, &UTCTime) == 0;
		}
	}
	operator tm() const
	{
		tm Time;
		GetTime(Time);
		return Time;
	}


	void SetTime(const time_t Time, TIME_MODE TargetMode = TIME_MODE_LOCAL)
	{
		if (Time)
		{
			tm Tm;
			if (TargetMode == TIME_MODE_UTC)
				gmtime_s(&Tm, &Time);
			else
				localtime_s(&Tm, &Time);
			m_wYear = 1900 + Tm.tm_year;
			m_wDayOfYear = Tm.tm_yday;
			m_wMonth = Tm.tm_mon + 1;
			m_wDayOfWeek = Tm.tm_wday;
			m_wDay = Tm.tm_mday;
			m_wHour = Tm.tm_hour;
			m_wMinute = Tm.tm_min;
			m_wSecond = Tm.tm_sec;
			m_wMilliseconds = 0;
			m_TimeMode = TargetMode;
		}
		else
		{
			Clear();
		}
	}
	CEasyTime& operator=(const time_t Time)
	{
		SetTime(Time);
		return *this;
	}
	void GetTime(time_t& Time) const
	{
		if (IsValidate())
		{
			tm Tm;
			
			Tm.tm_year = m_wYear - 1900;
			Tm.tm_yday = m_wDayOfYear;
			Tm.tm_mon = m_wMonth - 1;
			Tm.tm_wday = m_wDayOfWeek;
			Tm.tm_mday = m_wDay;
			Tm.tm_hour = m_wHour;
			Tm.tm_min = m_wMinute;
			Tm.tm_sec = m_wSecond;
			Tm.tm_isdst = 0;

			if (m_TimeMode == TIME_MODE_LOCAL)
				Time = mktime(&Tm);
			else
				Time = _mkgmtime(&Tm);
		}
		else
		{
			Time = 0;
		}
	}
	operator time_t() const
	{
		time_t Time;
		GetTime(Time);
		return Time;
	}


	void SetTime(const CEasyTime& Time)
	{
		m_wYear = Time.m_wYear;
		m_wDayOfYear = Time.m_wDayOfYear;
		m_wMonth = Time.m_wMonth;
		m_wDayOfWeek = Time.m_wDayOfWeek;
		m_wDay = Time.m_wDay;
		m_wHour = Time.m_wHour;
		m_wMinute = Time.m_wMinute;
		m_wSecond = Time.m_wSecond;
		m_wMilliseconds = Time.m_wMilliseconds;
		m_TimeMode = Time.m_TimeMode;
	}
	CEasyTime& operator=(const CEasyTime& Time)
	{
		SetTime(Time);
		return *this;
	}
	
#ifdef WIN32
	void FetchLocalTime()
	{
		SYSTEMTIME SysTime;
		::GetLocalTime(&SysTime);
		SetTime(SysTime, TIME_MODE_LOCAL);
	}
	void FetchUTCTime()
	{
		SYSTEMTIME SysTime;
		::GetSystemTime(&SysTime);
		SetTime(SysTime, TIME_MODE_UTC);
	}
	void ToUTCTime()
	{
		if (m_TimeMode == TIME_MODE_LOCAL)
		{
			SetTime(*this, TIME_MODE_LOCAL, TIME_MODE_UTC);
		}
	}
	void ToLocalTime()
	{
		if (m_TimeMode == TIME_MODE_UTC)
		{
			SetTime(*this, TIME_MODE_UTC, TIME_MODE_LOCAL);
		}
	}
#else
	void FetchLocalTime()
	{		
		SetTime(time(NULL), TIME_MODE_LOCAL);
	}
	void FetchUTCTime()
	{
		SetTime(time(NULL), TIME_MODE_UTC);
	}
	void ToUTCTime()
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
	bool FromString(LPCTSTR szTime, TIME_MODE TargetMode = TIME_MODE_LOCAL)
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
			m_TimeMode = TargetMode;
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
		if (IsValidate())
		{
			tm Time;
			GetTime(Time);
			size_t Len = _tcsftime(szFormatBuffer, BufferLen - 1, szFormat, &Time);
			if (szFormatBuffer)
				szFormatBuffer[BufferLen - 1] = 0;
			return Len;
		}
		else
		{
			_tcscpy_s(szFormatBuffer, BufferLen, _T("Invalid Time"));
			return 12;
		}
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

	bool IsValidate() const
	{
		return m_wYear >= 1 && m_wYear <= 6199 &&
			m_wMonth >= 1 && m_wMonth <= 12 &&
			m_wDay >= 1 && m_wDay <= 31 &&
			m_wHour <= 23 && m_wMinute <= 59 && m_wSecond <= 59;
	}

	static bool IsSameDate(const CEasyTime& Time1,const CEasyTime& Time2)
	{
		return Time1.m_wYear==Time2.m_wYear&&Time1.m_wMonth==Time2.m_wMonth&&Time1.m_wDay==Time2.m_wDay;
	}

	static time_t GetCurTime()
	{
		return time(NULL);
	}

	static int GetTimeZone()
	{
#ifdef WIN32
		TIME_ZONE_INFORMATION TimeZoneInfo;
		if (GetTimeZoneInformation(&TimeZoneInfo) != TIME_ZONE_ID_INVALID)
		{
			return -TimeZoneInfo.Bias * 60;
		}
		return 0;
#else
		tzset();
		return -timezone;
#endif
		//time_t t1, t2;     
		//struct tm *tm_local, *tm_utc; 	
		//t1 = time(NULL);
		//t2 = t1;	
		//tm_local = localtime(&t1);	
		//t1 = mktime(tm_local);	
		//tm_utc = gmtime(&t2);	
		//t2 = mktime(tm_utc);
		//return (t1 - t2);
	}
};

