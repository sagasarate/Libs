/****************************************************************************/
/*                                                                          */
/*      文件名:    SmartStruct.h                                            */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


#define CHECK_SMART_STRUCT_ADD(Opt,FailCount)		{if(!(Opt)) FailCount++;}
#define CHECK_SMART_STRUCT_ADD_AND_RETURN(Opt)		{if(!(Opt)) return false;}
//#define SMART_STRUCT_FIX_MEMBER_SIZE(DataSize)		(sizeof(WORD)+sizeof(BYTE)+DataSize)
//#define SMART_STRUCT_STRING_MEMBER_SIZE(StrLen)		(sizeof(WORD)+sizeof(BYTE)+sizeof(UINT)+sizeof(char)*(StrLen+1))
//#define SMART_STRUCT_WSTRING_MEMBER_SIZE(StrLen)	(sizeof(WORD)+sizeof(BYTE)+sizeof(UINT)+sizeof(WCHAR)*(StrLen+1))
//#define SMART_STRUCT_STRUCT_MEMBER_SIZE(StructLen)	(sizeof(WORD)+sizeof(BYTE)+sizeof(UINT)+StructLen)

template < typename LENGTH_TYPE,typename ID_TYPE >
class CSmartStructBase 
{
protected:
	BYTE *			m_pData;
	LENGTH_TYPE		m_DataLen;
	bool			m_IsSelfData;
	bool			m_AllowChange;

public:
	enum
	{
		HEAD_SIZE=(sizeof(BYTE)+sizeof(LENGTH_TYPE)),
		INVALID_MEMBER_ID=((ID_TYPE)-1),
	};
	CSmartStructBase(void)
	{
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
	}
	CSmartStructBase(int BufferLen)
	{
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
		Create(BufferLen);
	}
	CSmartStructBase(LPVOID pData,LENGTH_TYPE DataLen,bool IsEmpty)
	{
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
		Attach(pData,DataLen,IsEmpty);
	}
	CSmartStructBase(const CSmartStructBase& Struct)
	{
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
		Attach(Struct.GetData(),Struct.GetBufferLen(),false);
	}
	CSmartStructBase(const CSmartValue& Value)
	{
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
		Attach(Value.GetData(),Value.GetDataLen(),false);
	}

	bool Create(int BufferLen)
	{
		Destory();
		m_DataLen=sizeof(BYTE)+sizeof(LENGTH_TYPE)+BufferLen;
		m_pData=new BYTE[m_DataLen];
		ZeroMemory(m_pData,m_DataLen);
		*m_pData=CSmartValue::VT_STRUCT;
		*((LENGTH_TYPE *)(m_pData+1))=0;		
		m_IsSelfData=true;
		m_AllowChange=true;
		return true;
	}

	bool Attach(LPVOID pData,LENGTH_TYPE DataLen,bool IsEmpty);	

	bool CloneFrom(const CSmartStructBase& Value)
	{
		Destory();
		if(Value.GetData()&&Value.m_DataLen)
		{
			m_IsSelfData=true;
			m_AllowChange=false;
			m_DataLen=Value.m_DataLen;
			m_pData=new BYTE[m_DataLen];
			memcpy(m_pData,Value.GetData(),Value.GetDataLen());
		}
		return true;
	}
	
	virtual ~CSmartStructBase(void)
	{
		Destory();
	}


	void Destory()
	{
		if(m_IsSelfData)
		{
			SAFE_DELETE_ARRAY(m_pData);
		}
		else
		{
			m_pData=NULL;
		}
	}

	void Clear()
	{
		if(m_pData)
			*((LENGTH_TYPE *)(m_pData+1))=0;
	}	

	operator CSmartValue() const
	{
		return CSmartValue(GetData(),GetDataLen());
	}

	void operator=(const CSmartStructBase& Struct)
	{
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
		Attach(Struct.GetData(),Struct.GetBufferLen(),false);
	}


	LENGTH_TYPE GetLength() const
	{
		if(m_pData==NULL)
			return 0;
		return *((LENGTH_TYPE *)(m_pData+1));
	}

	LENGTH_TYPE GetDataLen() const
	{
		if(m_pData==NULL)
			return 0;
		LENGTH_TYPE DataLen=GetLength()+sizeof(BYTE)+sizeof(LENGTH_TYPE);
		if(DataLen>m_DataLen)
			DataLen=m_DataLen;
		return DataLen;
	}
	LENGTH_TYPE GetBufferLen() const
	{		
		return m_DataLen;
	}
	LPVOID GetData() const
	{
		return m_pData;
	}
	bool AddMember(ID_TYPE ID,char Value)
	{
		LENGTH_TYPE BufferSize;
		void * pBuffer=PrepareMember(BufferSize);
		if(pBuffer)
		{
			CSmartValue SmartValue;
			if(SmartValue.Attach(pBuffer,BufferSize,CSmartValue::VT_CHAR))
			{
				SmartValue=Value;
				return FinishMember(ID,SmartValue.GetDataLen());
			}			
		}
		return false;
	}
	bool AddMember(ID_TYPE ID,unsigned char Value)
	{
		LENGTH_TYPE BufferSize;
		void * pBuffer=PrepareMember(BufferSize);
		if(pBuffer)
		{
			CSmartValue SmartValue;
			if(SmartValue.Attach(pBuffer,BufferSize,CSmartValue::VT_UCHAR))
			{
				SmartValue=Value;
				return FinishMember(ID,SmartValue.GetDataLen());
			}			
		}
		return false;
	}
	bool AddMember(ID_TYPE ID,short Value)
	{
		LENGTH_TYPE BufferSize;
		void * pBuffer=PrepareMember(BufferSize);
		if(pBuffer)
		{
			CSmartValue SmartValue;
			if(SmartValue.Attach(pBuffer,BufferSize,CSmartValue::VT_SHORT))
			{
				SmartValue=Value;
				return FinishMember(ID,SmartValue.GetDataLen());
			}			
		}
		return false;
	}
	bool AddMember(ID_TYPE ID,unsigned short Value)
	{
		LENGTH_TYPE BufferSize;
		void * pBuffer=PrepareMember(BufferSize);
		if(pBuffer)
		{
			CSmartValue SmartValue;
			if(SmartValue.Attach(pBuffer,BufferSize,CSmartValue::VT_USHORT))
			{
				SmartValue=Value;
				return FinishMember(ID,SmartValue.GetDataLen());
			}			
		}
		return false;
	}
	//bool AddMember(ID_TYPE ID,long Value)
	//{
	//	LENGTH_TYPE BufferSize;
	//	void * pBuffer=PrepareMember(BufferSize);
	//	if(pBuffer)
	//	{
	//		CSmartValue SmartValue;
	//		if(SmartValue.Attach(pBuffer,BufferSize,CSmartValue::VT_INT))
	//		{
	//			SmartValue=Value;
	//			return FinishMember(ID,SmartValue.GetDataLen());
	//		}			
	//	}
	//	return false;
	//}
	//bool AddMember(ID_TYPE ID,unsigned long Value)
	//{
	//	LENGTH_TYPE BufferSize;
	//	void * pBuffer=PrepareMember(BufferSize);
	//	if(pBuffer)
	//	{
	//		CSmartValue SmartValue;
	//		if(SmartValue.Attach(pBuffer,BufferSize,CSmartValue::VT_UINT))
	//		{
	//			SmartValue=Value;
	//			return FinishMember(ID,SmartValue.GetDataLen());
	//		}			
	//	}
	//	return false;
	//}
	bool AddMember(ID_TYPE ID,int Value)
	{
		LENGTH_TYPE BufferSize;
		void * pBuffer=PrepareMember(BufferSize);
		if(pBuffer)
		{
			CSmartValue SmartValue;
			if(SmartValue.Attach(pBuffer,BufferSize,CSmartValue::VT_INT))
			{
				SmartValue=Value;
				return FinishMember(ID,SmartValue.GetDataLen());
			}			
		}
		return false;
	}
	bool AddMember(ID_TYPE ID,unsigned int Value)
	{
		LENGTH_TYPE BufferSize;
		void * pBuffer=PrepareMember(BufferSize);
		if(pBuffer)
		{
			CSmartValue SmartValue;
			if(SmartValue.Attach(pBuffer,BufferSize,CSmartValue::VT_UINT))
			{
				SmartValue=Value;
				return FinishMember(ID,SmartValue.GetDataLen());
			}			
		}
		return false;
	}
	bool AddMember(ID_TYPE ID,__int64 Value)
	{
		LENGTH_TYPE BufferSize;
		void * pBuffer=PrepareMember(BufferSize);
		if(pBuffer)
		{
			CSmartValue SmartValue;
			if(SmartValue.Attach(pBuffer,BufferSize,CSmartValue::VT_BIGINT))
			{
				SmartValue=Value;
				return FinishMember(ID,SmartValue.GetDataLen());
			}			
		}
		return false;
	}
	bool AddMember(ID_TYPE ID,unsigned __int64 Value)
	{
		LENGTH_TYPE BufferSize;
		void * pBuffer=PrepareMember(BufferSize);
		if(pBuffer)
		{
			CSmartValue SmartValue;
			if(SmartValue.Attach(pBuffer,BufferSize,CSmartValue::VT_UBIGINT))
			{
				SmartValue=Value;
				return FinishMember(ID,SmartValue.GetDataLen());
			}			
		}
		return false;
	}
	bool AddMember(ID_TYPE ID,float Value)
	{
		LENGTH_TYPE BufferSize;
		void * pBuffer=PrepareMember(BufferSize);
		if(pBuffer)
		{
			CSmartValue SmartValue;
			if(SmartValue.Attach(pBuffer,BufferSize,CSmartValue::VT_FLOAT))
			{
				SmartValue=Value;
				return FinishMember(ID,SmartValue.GetDataLen());
			}			
		}
		return false;
	}
	bool AddMember(ID_TYPE ID,double Value)
	{
		LENGTH_TYPE BufferSize;
		void * pBuffer=PrepareMember(BufferSize);
		if(pBuffer)
		{
			CSmartValue SmartValue;
			if(SmartValue.Attach(pBuffer,BufferSize,CSmartValue::VT_DOUBLE))
			{
				SmartValue=Value;
				return FinishMember(ID,SmartValue.GetDataLen());
			}			
		}
		return false;
	}
	bool AddMember(ID_TYPE ID,bool Value)
	{
		LENGTH_TYPE BufferSize;
		void * pBuffer=PrepareMember(BufferSize);
		if(pBuffer)
		{
			CSmartValue SmartValue;
			if(SmartValue.Attach(pBuffer,BufferSize,CSmartValue::VT_BOOL))
			{
				SmartValue=Value;
				return FinishMember(ID,SmartValue.GetDataLen());
			}			
		}
		return false;
	}
	bool AddMember(ID_TYPE ID,const char * pszStr)
	{
		if(pszStr==NULL)
			return true;
		return AddMember(ID,pszStr,(LENGTH_TYPE)strlen(pszStr));
	}
	bool AddMember(ID_TYPE ID,const WCHAR * pszStr)
	{
		if(pszStr==NULL)
			return true;
		return AddMember(ID,pszStr,(LENGTH_TYPE)wcslen(pszStr));
	}
	bool AddMember(ID_TYPE ID,const CSmartValue& Value)
	{		
		if(m_pData==NULL)
			return false;
		if(ID==0)
			return false;
		if(Value.GetDataLen()<=0)
		{
			//如果是空数据，插入一个0字节占个位子
			return AddMember(ID,(BYTE)0);
		}
		LENGTH_TYPE NeedSize=sizeof(ID_TYPE)+Value.GetDataLen();
		if(GetFreeLen()<NeedSize)
		{
			return false;
		}
		BYTE * pFreeBuffer=m_pData+sizeof(BYTE)+sizeof(LENGTH_TYPE)+GetLength();
		*((ID_TYPE *)pFreeBuffer)=ID;
		pFreeBuffer+=sizeof(ID_TYPE);
		memcpy(pFreeBuffer,Value.GetData(),Value.GetDataLen());
		*((LENGTH_TYPE *)(m_pData+1))+=NeedSize;
		return true;
	}
	bool AddMember(ID_TYPE ID,const char * pszStr,LENGTH_TYPE nStrLen);
	bool AddMember(ID_TYPE ID,const WCHAR * pszStr,LENGTH_TYPE nStrLen);
	bool AddMember(ID_TYPE ID,const CEasyString& Str)
	{
		return AddMember(ID,Str,(UINT)Str.GetLength());
	}
	void * PrepareMember(LENGTH_TYPE& nUsableSize)
	{
		if(m_pData==NULL)
			return NULL;
		if(GetFreeLen()<sizeof(ID_TYPE))
		{
			nUsableSize=0;
			return NULL;
		}
		nUsableSize=GetFreeLen()-sizeof(ID_TYPE);
		BYTE * pFreeBuffer=m_pData+sizeof(BYTE)+sizeof(LENGTH_TYPE)+GetLength()+sizeof(ID_TYPE);
		return pFreeBuffer;
	}
	CSmartStructBase PrepareSubStruct()
	{
		CSmartStructBase SubStruct;
		if(m_pData)			
		{
			if(GetFreeLen()>=GetStructMemberSize(0))
			{
				LENGTH_TYPE nUsableSize=GetFreeLen()-sizeof(ID_TYPE);
				BYTE * pFreeBuffer=m_pData+sizeof(BYTE)+sizeof(LENGTH_TYPE)+GetLength()+sizeof(ID_TYPE);
				SubStruct.Attach(pFreeBuffer,nUsableSize,true);
			}			
		}
		return SubStruct;
	}
	bool FinishMember(ID_TYPE ID,LENGTH_TYPE MemberSize)
	{
		if(m_pData==NULL)
			return false;
		if(ID==0)
			return false;
		LENGTH_TYPE NeedSize=sizeof(ID_TYPE)+MemberSize;
		if(GetFreeLen()<NeedSize)
		{
			return false;
		}
		BYTE * pFreeBuffer=m_pData+sizeof(BYTE)+sizeof(LENGTH_TYPE)+GetLength();
		*((ID_TYPE *)pFreeBuffer)=ID;
		*((LENGTH_TYPE *)(m_pData+1))+=NeedSize;
		return true;
	}
	
	bool Append(const CSmartStructBase& Struct)
	{
		if(m_pData==NULL)
			return false;
		if(Struct.m_pData==NULL)
			return false;
		if(Struct.m_pData[0]!=CSmartValue::VT_STRUCT)
			return false;	
		LENGTH_TYPE NeedSize=Struct.GetLength();
		if(GetFreeLen()<NeedSize)
		{
			return false;
		}
		BYTE * pFreeBuffer=m_pData+sizeof(BYTE)+sizeof(LENGTH_TYPE)+GetLength();

		memcpy(pFreeBuffer,Struct.m_pData+sizeof(BYTE)+sizeof(LENGTH_TYPE),NeedSize);
		*((LENGTH_TYPE *)(m_pData+1))+=NeedSize;
		return true;
	}
	int GetMemberCount() const
	{
		if(m_pData==NULL)
			return 0;
		BYTE * pHead=m_pData;
		BYTE * pTail=m_pData+GetDataLen();
		int MemberCount=0;
		pHead+=sizeof(BYTE)+sizeof(LENGTH_TYPE);
		while(pHead+sizeof(ID_TYPE)<pTail)
		{
			pHead+=sizeof(ID_TYPE);

			CSmartValue Value(pHead,(UINT)(pTail-pHead));

			pHead+=Value.GetDataLen();
			MemberCount++;
		}
		return MemberCount;
	}
	CSmartValue GetMemberByIndex(LENGTH_TYPE Index) const
	{
		CSmartValue Value;

		if(m_pData==NULL)
			return Value;

		BYTE * pHead=m_pData;
		BYTE * pTail=m_pData+GetDataLen();

		pHead+=sizeof(BYTE)+sizeof(LENGTH_TYPE);
		while(pHead+sizeof(ID_TYPE)<pTail)
		{
			pHead+=sizeof(ID_TYPE);
			if(!Value.Attach(pHead,(LENGTH_TYPE)(pTail-pHead),CSmartValue::VT_UNKNOWN))
				break;
			pHead+=Value.GetDataLen();
			if(Index==0)
				return Value;
			Index--;
		}
		Value.Destory();
		return Value;

	}
	CSmartValue GetMember(ID_TYPE ID) const
	{
		CSmartValue Value;

		if(m_pData==NULL)
			return Value;

		BYTE * pHead=m_pData;
		BYTE * pTail=m_pData+GetDataLen();

		pHead+=sizeof(BYTE)+sizeof(LENGTH_TYPE);
		while(pHead+sizeof(ID_TYPE)<pTail)
		{
			ID_TYPE MemberID=*((ID_TYPE *)pHead);
			pHead+=sizeof(ID_TYPE);
			if(!Value.Attach(pHead,(LENGTH_TYPE)(pTail-pHead),CSmartValue::VT_UNKNOWN))
				break;
			if(MemberID==ID)
				return Value;
			pHead+=Value.GetDataLen();
		}
		Value.Destory();
		return Value;
	}
	bool IsMemberExist(ID_TYPE ID) const
	{
		if(IDToIndex(ID)==INVALID_MEMBER_ID)
		{
			return false;
		}
		return true;
	}
	LENGTH_TYPE IDToIndex(ID_TYPE ID) const
	{
		CSmartValue Value;

		if(m_pData==NULL)
			return INVALID_MEMBER_ID;

		BYTE * pHead=m_pData;
		BYTE * pTail=m_pData+GetDataLen();

		LENGTH_TYPE Index=0;

		pHead+=sizeof(BYTE)+sizeof(LENGTH_TYPE);
		while(pHead+sizeof(ID_TYPE)<pTail)
		{
			ID_TYPE MemberID=*((ID_TYPE *)pHead);
			pHead+=sizeof(ID_TYPE);
			if(!Value.Attach(pHead,(LENGTH_TYPE)(pTail-pHead),CSmartValue::VT_UNKNOWN))
				break;
			if(MemberID==ID)
				return Index;
			pHead+=Value.GetDataLen();
			Index++;
		}
		return INVALID_MEMBER_ID;
	}
	ID_TYPE IndexToID(LENGTH_TYPE Index) const
	{
		CSmartValue Value;

		if(m_pData==NULL)
			return 0;

		BYTE * pHead=m_pData;
		BYTE * pTail=m_pData+GetDataLen();
		ID_TYPE MemberID=0;

		pHead+=sizeof(BYTE)+sizeof(LENGTH_TYPE);
		while(pHead+sizeof(ID_TYPE)<pTail)
		{
			MemberID=*((ID_TYPE *)pHead);
			if(Index==0)
				return MemberID;
			pHead+=sizeof(ID_TYPE);
			if(!Value.Attach(pHead,(LENGTH_TYPE)(pTail-pHead),CSmartValue::VT_UNKNOWN))
				break;
			pHead+=Value.GetDataLen();
			Index--;
		}
		return 0;
	}
	LPVOID GetFirstMemberPosition() const
	{
		if(m_pData==NULL)
			return NULL;


		BYTE * pHead=m_pData;
		BYTE * pTail=m_pData+GetDataLen();
		pHead+=sizeof(BYTE)+sizeof(LENGTH_TYPE);

		if(pHead<pTail)
			return pHead;
		else
			return NULL;
	}
	CSmartValue GetNextMember(void *& Pos,ID_TYPE& MemberID) const
	{
		CSmartValue Value;

		if(m_pData==NULL)
		{
			Pos=NULL;
			return Value;
		}


		if(Pos)
		{
			BYTE * pHead=(BYTE *)Pos;
			BYTE * pTail=m_pData+GetDataLen();
			MemberID=*((ID_TYPE *)pHead);

			pHead+=sizeof(ID_TYPE);

			if(pHead>=pTail)
			{
				Pos=NULL;
			}
			else
			{
				if(!Value.Attach(pHead,(LENGTH_TYPE)(pTail-pHead),CSmartValue::VT_UNKNOWN))
				{
					Pos=NULL;
					return Value;
				}
				Pos=pHead+Value.GetDataLen();
				if(Pos>=pTail)
				{
					Pos=NULL;
				}
			}

		}
		return Value;
	}

	static LENGTH_TYPE GetDataLenFromData(LPCVOID pData,LENGTH_TYPE DataSize)
	{
		if(pData==NULL||DataSize<HEAD_SIZE)
			return 0;
		return *((LENGTH_TYPE *)((BYTE *)pData+1))+HEAD_SIZE;
	}

	static LENGTH_TYPE GetEmptyStructSize()
	{
		return sizeof(BYTE)+sizeof(LENGTH_TYPE);
	}

	static LENGTH_TYPE GetFixMemberSize(LENGTH_TYPE TypeLen)
	{		
		return sizeof(ID_TYPE)+sizeof(BYTE)+TypeLen;		
	}
	static LENGTH_TYPE GetStructMemberSize(LENGTH_TYPE StructSize)
	{		
		return sizeof(ID_TYPE)+sizeof(BYTE)+sizeof(LENGTH_TYPE)+StructSize;		
	}
	static LENGTH_TYPE GetStringMemberSize(LENGTH_TYPE StrLen)
	{		
		return sizeof(ID_TYPE)+sizeof(BYTE)+sizeof(LENGTH_TYPE)+sizeof(char)*(StrLen+1);
	}
	static LENGTH_TYPE GetWStringMemberSize(LENGTH_TYPE StrLen)
	{
		return sizeof(ID_TYPE)+sizeof(BYTE)+sizeof(LENGTH_TYPE)+sizeof(WCHAR)*(StrLen+1);
	}

	LENGTH_TYPE GetFreeLen()
	{
		return m_DataLen-GetDataLen();
	}
	
};

template < >
inline bool CSmartStructBase<UINT,WORD>::Attach(LPVOID pData,UINT DataLen,bool IsEmpty)
{
	if(pData==NULL||DataLen==0)
		return false;
	Destory();
	m_IsSelfData=false;
	m_AllowChange=false;
	m_pData=(BYTE *)pData;
	if(IsEmpty)
	{
		m_pData[0]=CSmartValue::VT_STRUCT;
		*((UINT *)(m_pData+1))=0;			
	}
	else
	{
		if(m_pData[0]!=CSmartValue::VT_STRUCT)
		{
			Destory();
			return false;
		}
	}
	m_DataLen=DataLen;		
	return true;
}

template < >
inline bool CSmartStructBase<WORD,BYTE>::Attach(LPVOID pData,WORD DataLen,bool IsEmpty)
{
	if(pData==NULL||DataLen==0)
		return false;
	Destory();
	m_IsSelfData=false;
	m_AllowChange=false;
	m_pData=(BYTE *)pData;
	if(IsEmpty)
	{
		m_pData[0]=CSmartValue::VT_STRUCT_TINY;
		*((WORD *)(m_pData+1))=0;			
	}
	else
	{
		if(m_pData[0]!=CSmartValue::VT_STRUCT_TINY)
		{
			Destory();
			return false;
		}
	}
	m_DataLen=DataLen;		
	return true;
}

template < >
inline bool CSmartStructBase<UINT,WORD>::AddMember(WORD ID,const char * pszStr,UINT nStrLen)
{
	if(m_pData==NULL)
		return false;
	if(ID==0)
		return false;
	
	UINT NeedSize=sizeof(WORD)+sizeof(BYTE)+sizeof(UINT)+sizeof(char)*(nStrLen+1);
	if(GetFreeLen()<NeedSize)
	{
		return false;
	}
	BYTE * pFreeBuffer=m_pData+sizeof(BYTE)+sizeof(UINT)+GetLength();
	*((WORD *)pFreeBuffer)=ID;
	pFreeBuffer+=sizeof(WORD);
	*((BYTE *)pFreeBuffer)=CSmartValue::VT_STRING;
	pFreeBuffer+=sizeof(BYTE);
	*((UINT *)pFreeBuffer)=nStrLen;
	pFreeBuffer+=sizeof(UINT);
	if(pszStr&&nStrLen)
		memcpy(pFreeBuffer,pszStr,sizeof(char)*nStrLen);
	pFreeBuffer+=sizeof(char)*nStrLen;
	*((char *)pFreeBuffer)=0;
	*((UINT *)(m_pData+1))+=NeedSize;
	return true;
}

template < >
inline bool CSmartStructBase<WORD,BYTE>::AddMember(BYTE ID,const char * pszStr,WORD nStrLen)
{
	if(m_pData==NULL)
		return false;
	if(ID==0)
		return false;	
	WORD NeedSize=sizeof(BYTE)+sizeof(BYTE)+sizeof(WORD)+sizeof(char)*(nStrLen+1);
	if(GetFreeLen()<NeedSize)
	{
		return false;
	}
	BYTE * pFreeBuffer=m_pData+sizeof(BYTE)+sizeof(WORD)+GetLength();
	*((BYTE *)pFreeBuffer)=ID;
	pFreeBuffer+=sizeof(BYTE);
	*((BYTE *)pFreeBuffer)=CSmartValue::VT_STRING_TINY;
	pFreeBuffer+=sizeof(BYTE);
	*((WORD *)pFreeBuffer)=nStrLen;
	pFreeBuffer+=sizeof(WORD);
	if(pszStr&&nStrLen)
		memcpy(pFreeBuffer,pszStr,sizeof(char)*nStrLen);
	pFreeBuffer+=sizeof(char)*nStrLen;
	*((char *)pFreeBuffer)=0;
	*((WORD *)(m_pData+1))+=NeedSize;
	return true;
}

template < >
inline bool CSmartStructBase<UINT,WORD>::AddMember(WORD ID,const WCHAR * pszStr,UINT nStrLen)
{
	if(m_pData==NULL)
		return false;
	if(ID==0)
		return false;	

	
	UINT NeedSize=sizeof(WORD)+sizeof(BYTE)+sizeof(UINT)+sizeof(WCHAR)*(nStrLen+1);
	if(GetFreeLen()<NeedSize)
	{
		return false;
	}
	BYTE * pFreeBuffer=m_pData+sizeof(BYTE)+sizeof(UINT)+GetLength();
	*((WORD *)pFreeBuffer)=ID;
	pFreeBuffer+=sizeof(WORD);
	*((BYTE *)pFreeBuffer)=CSmartValue::VT_USTRING;
	pFreeBuffer+=sizeof(BYTE);
	*((UINT *)pFreeBuffer)=nStrLen;
	pFreeBuffer+=sizeof(UINT);
	if(pszStr&&nStrLen)
		memcpy(pFreeBuffer,pszStr,sizeof(WCHAR)*nStrLen);
	pFreeBuffer+=sizeof(WCHAR)*nStrLen;
	*((WCHAR *)pFreeBuffer)=0;
	*((UINT *)(m_pData+1))+=NeedSize;
	
	return true;
}

template < >
inline bool CSmartStructBase<WORD,BYTE>::AddMember(BYTE ID,const WCHAR * pszStr,WORD nStrLen)
{
	if(m_pData==NULL)
		return false;
	if(ID==0)
		return false;	


	WORD NeedSize = sizeof(BYTE) + sizeof(BYTE) + sizeof(WORD) + sizeof(WCHAR)*(nStrLen + 1);
	if (GetFreeLen() < NeedSize)
	{
		return false;
	}
	BYTE * pFreeBuffer = m_pData + sizeof(BYTE) + sizeof(WORD) + GetLength();
	*((BYTE *)pFreeBuffer) = ID;
	pFreeBuffer += sizeof(BYTE);
	*((BYTE *)pFreeBuffer) = CSmartValue::VT_USTRING_TINY;
	pFreeBuffer += sizeof(BYTE);
	*((WORD *)pFreeBuffer) = nStrLen;
	pFreeBuffer += sizeof(WORD);
	if (pszStr&&nStrLen)
		memcpy(pFreeBuffer, pszStr, sizeof(WCHAR)*nStrLen);
	pFreeBuffer += sizeof(WCHAR)*nStrLen;
	*((WCHAR *)pFreeBuffer) = 0;
	*((WORD *)(m_pData + 1)) += NeedSize;

	return true;
}

typedef CSmartStructBase<UINT,WORD>	CSmartStruct;
typedef CSmartStructBase<WORD,BYTE>	CSmartStructTiny;