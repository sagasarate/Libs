/****************************************************************************/
/*                                                                          */
/*      文件名:    ClassifiedID.h                                           */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once



class CClassifiedID
{
protected:

	union CLASSIFIED_ID
	{
		UINT	ID;
		struct CLASSIFIED_4
		{
			BYTE	Index;			//索引编号
			BYTE	Group;			//分组号
			BYTE	SubType;		//子类型
			BYTE	MainType;		//主类型
		}ClassifiedID_4;
		struct CLASSIFIED_2
		{
			WORD	Index;		//索引编号		
			WORD	Type;		//类型			
		}ClassifiedID_2;
	};

	CLASSIFIED_ID		m_ID;

public:
	CClassifiedID()
	{
		m_ID.ID=0;
	}
	CClassifiedID(unsigned int ID)
	{
		m_ID.ID=ID;
	}
	CClassifiedID(int ID)
	{
		m_ID.ID=(UINT)ID;
	}
	CClassifiedID(LPCTSTR IDStr)
	{
		FromStr(IDStr);
	}
	CClassifiedID& operator=(CClassifiedID& ID)
	{
		m_ID.ID=ID.m_ID.ID;
		return *this;
	}
	CClassifiedID& operator=(unsigned int ID)
	{
		m_ID.ID=ID;
		return *this;
	}
	CClassifiedID& operator=(int ID)
	{
		m_ID.ID=(UINT)ID;
		return *this;
	}
	CClassifiedID& operator=(LPCTSTR IDStr)
	{
		FromStr(IDStr);
		return *this;
	}
	operator int() const
	{
		return (int)m_ID.ID;
	}
	operator unsigned int() const
	{
		return m_ID.ID;
	}
	operator LPCTSTR() const
	{
		return ToStr();
	}	
	void FromStr(LPCTSTR IDStr)
	{
		CStringSplitter Split(IDStr,_T('-'));

		if(Split.GetCount()>=4)
		{
			m_ID.ClassifiedID_4.MainType=_ttoi(Split[0]);
			m_ID.ClassifiedID_4.SubType=_ttoi(Split[1]);
			m_ID.ClassifiedID_4.Group=_ttoi(Split[2]);
			m_ID.ClassifiedID_4.Index=_ttoi(Split[3]);
		}
	}
	LPCTSTR ToStr() const
	{
		static TCHAR Buffer[32];

		_stprintf_s(Buffer,32,_T("%u-%u-%u-%u"),
			m_ID.ClassifiedID_4.MainType,
			m_ID.ClassifiedID_4.SubType,
			m_ID.ClassifiedID_4.Group,
			m_ID.ClassifiedID_4.Index);
		return Buffer;
	}
	BYTE& MainType()
	{
		return m_ID.ClassifiedID_4.MainType;
	}
	BYTE& SubType()
	{
		return m_ID.ClassifiedID_4.SubType;
	}
	BYTE& Group()
	{
		return m_ID.ClassifiedID_4.Group;
	}
	BYTE& Index()
	{
		return m_ID.ClassifiedID_4.Index;
	}
	WORD& BigType()
	{
		return m_ID.ClassifiedID_2.Type;
	}
	WORD& BigIndex()
	{
		return m_ID.ClassifiedID_2.Index;
	}
};