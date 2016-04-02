/****************************************************************************/
/*                                                                          */
/*      文件名:    SmartValue.h                                             */
/*      创建日期:  2009年09月25日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

	

class CSmartValue 
{
protected:
	BYTE *			m_pData;
	UINT			m_DataLen;
	bool			m_IsSelfData;
	bool			m_AllowChange;

	
public:
	enum SMART_VALUE_TYPE
	{
		VT_UNKNOWN,
		VT_CHAR,
		VT_UCHAR,
		VT_SHORT,
		VT_USHORT,
		VT_INT,
		VT_UINT,
		VT_BIGINT,
		VT_UBIGINT,
		VT_FLOAT,
		VT_DOUBLE,
		VT_STRING,
		VT_USTRING,
		VT_STRUCT,
		VT_STRING_TINY,
		VT_USTRING_TINY,
		VT_STRUCT_TINY,
		VT_BOOL,
	};

	CSmartValue(void)
	{	
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
	}
	CSmartValue(char Value)
	{
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
		Create(VT_CHAR,0);
		*this=Value;
	}
	CSmartValue(unsigned char Value)
	{
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
		Create(VT_UCHAR,0);
		*this=Value;
	}
	CSmartValue(short Value)
	{
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
		Create(VT_SHORT,0);
		*this=Value;
	}
	CSmartValue(unsigned short Value)
	{
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
		Create(VT_USHORT,0);
		*this=Value;
	}
	CSmartValue(int Value)
	{
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
		Create(VT_INT,0);
		*this=Value;
	}
	CSmartValue(unsigned int Value)
	{
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
		Create(VT_UINT,0);
		*this=Value;
	}
	//CSmartValue(long Value)
	//{
	//	m_pData=NULL;
	//	m_DataLen=0;
	//	m_IsSelfData=true;
	//	m_AllowChange=true;
	//	Create(VT_INT,0);
	//	*this=Value;
	//}
	//CSmartValue(unsigned long Value)
	//{
	//	m_pData=NULL;
	//	m_DataLen=0;
	//	m_IsSelfData=true;
	//	m_AllowChange=true;
	//	Create(VT_UINT,0);
	//	*this=Value;
	//}
	CSmartValue(__int64 Value)
	{
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
		Create(VT_BIGINT,0);
		*this=Value;
	}
	CSmartValue(unsigned __int64 Value)
	{
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
		Create(VT_UBIGINT,0);
		*this=Value;
	}	
	CSmartValue(float Value)
	{
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
		Create(VT_FLOAT,0);
		*this=Value;
	}	
	CSmartValue(double Value)
	{
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
		Create(VT_DOUBLE,0);
		*this=Value;
	}	

	CSmartValue(const char * Value)
	{
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
		if(Value)
		{		
			Create(VT_STRING,(UINT)strlen(Value));
			SetString(Value,(UINT)strlen(Value));
		}
	}

	CSmartValue(const WCHAR * Value)
	{
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
		if(Value)
		{
			Create(VT_USTRING,(UINT)wcslen(Value));
			SetString(Value,(UINT)wcslen(Value));
		}
	}
	CSmartValue(bool Value)
	{
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
		if(Value)
		{
			Create(VT_BOOL,0);
			*this=Value;
		}
	}	

	CSmartValue(const CSmartValue& Value)
	{
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
		Attach(Value.GetData(),Value.GetDataLen(),VT_UNKNOWN);
	}	
	CSmartValue(LPVOID pData,UINT DataLen,int ClearType=VT_UNKNOWN)
	{
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
		Attach(pData,DataLen,ClearType);
	}
	
	CSmartValue(int Type,UINT Len)
	{
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
		Create(Type,Len);
	}

	virtual ~CSmartValue(void)
	{
		Destory();
	}

	bool Create(int Type,UINT Len)
	{
		Destory();
		switch(Type)
		{		
		case VT_CHAR:
		case VT_UCHAR:
			m_DataLen=sizeof(char)+sizeof(BYTE);
			m_pData=new BYTE[m_DataLen];
			m_pData[0]=Type;
			m_pData[1]=0;
			break;
		case VT_SHORT:
		case VT_USHORT:
			m_DataLen=sizeof(short)+sizeof(BYTE);
			m_pData=new BYTE[m_DataLen];
			m_pData[0]=Type;
			*((short *)(m_pData+1))=0;
			break;
		case VT_INT:
		case VT_UINT:
			m_DataLen=sizeof(int)+sizeof(BYTE);
			m_pData=new BYTE[m_DataLen];
			m_pData[0]=Type;
			*((int *)(m_pData+1))=0;
			break;
		case VT_BIGINT:
		case VT_UBIGINT:
			m_DataLen=sizeof(__int64)+sizeof(BYTE);
			m_pData=new BYTE[m_DataLen];
			m_pData[0]=Type;
			*((__int64 *)(m_pData+1))=0;
			break;
		case VT_FLOAT:
			m_DataLen=sizeof(float)+sizeof(BYTE);
			m_pData=new BYTE[m_DataLen];
			m_pData[0]=Type;
			*((float *)(m_pData+1))=0;
			break;
		case VT_DOUBLE:
			m_DataLen=sizeof(double)+sizeof(BYTE);
			m_pData=new BYTE[m_DataLen];
			m_pData[0]=Type;
			*((double *)(m_pData+1))=0;
			break;
		case VT_STRING:
			m_DataLen=sizeof(char)*(Len+1)+sizeof(BYTE)+sizeof(UINT);
			m_pData=new BYTE[m_DataLen];
			m_pData[0]=Type;
			*((UINT *)(m_pData+sizeof(BYTE)))=0;
			*((char *)(m_pData+sizeof(BYTE)+sizeof(UINT)))=0;
			break;
		case VT_USTRING:
			m_DataLen=sizeof(WCHAR)*(Len/sizeof(WCHAR)+1)+sizeof(BYTE)+sizeof(UINT);
			m_pData=new BYTE[m_DataLen];
			m_pData[0]=Type;
			*((UINT *)(m_pData+sizeof(BYTE)))=0;
			*((WCHAR *)(m_pData+sizeof(BYTE)+sizeof(UINT)))=0;
			break;
		case VT_STRUCT:
			m_DataLen=Len+sizeof(BYTE)+sizeof(UINT);
			m_pData=new BYTE[m_DataLen];
			m_pData[0]=Type;
			*((UINT *)(m_pData+sizeof(BYTE)))=0;
			break;
		case VT_STRING_TINY:
			m_DataLen=sizeof(char)*(Len+1)+sizeof(BYTE)+sizeof(WORD);
			m_pData=new BYTE[m_DataLen];
			m_pData[0]=Type;
			*((WORD *)(m_pData+sizeof(BYTE)))=0;
			*((char *)(m_pData+sizeof(BYTE)+sizeof(WORD)))=0;
			break;
		case VT_USTRING_TINY:
			m_DataLen=sizeof(WCHAR)*(Len/sizeof(WCHAR)+1)+sizeof(BYTE)+sizeof(WORD);
			m_pData=new BYTE[m_DataLen];
			m_pData[0]=Type;
			*((WORD *)(m_pData+sizeof(BYTE)))=0;
			*((WCHAR *)(m_pData+sizeof(BYTE)+sizeof(WORD)))=0;
			break;
		case VT_STRUCT_TINY:
			m_DataLen=Len+sizeof(BYTE)+sizeof(WORD);
			m_pData=new BYTE[m_DataLen];
			m_pData[0]=Type;
			*((WORD *)(m_pData+sizeof(BYTE)))=0;
			break;
		case VT_BOOL:
			m_DataLen=sizeof(bool)+sizeof(BYTE);
			m_pData=new BYTE[m_DataLen];
			m_pData[0]=Type;
			*((bool *)(m_pData+1))=false;
			break;
		default:
			return false;
		}

		m_IsSelfData=true;
		return true;
	}

	bool Attach(void * pData,UINT DataLen,int ClearType=VT_UNKNOWN)
	{
		Destory();
		m_IsSelfData=false;
		m_pData=(BYTE *)pData;

		int DataType=ClearType;
		UINT BinaryDataLen=0;

		if(ClearType==VT_UNKNOWN)
		{
			DataType=GetType();
			BinaryDataLen=GetLength();			
		}		

		switch(DataType)
		{
		case VT_CHAR:
		case VT_UCHAR:
			m_DataLen=sizeof(char)+sizeof(BYTE);
			break;
		case VT_SHORT:
		case VT_USHORT:
			m_DataLen=sizeof(short)+sizeof(BYTE);
			break;
		case VT_INT:
		case VT_UINT:
			m_DataLen=sizeof(int)+sizeof(BYTE);
			break;
		case VT_BIGINT:
		case VT_UBIGINT:
			m_DataLen=sizeof(__int64)+sizeof(BYTE);
			break;
		case VT_FLOAT:
			m_DataLen=sizeof(float)+sizeof(BYTE);
			break;
		case VT_DOUBLE:
			m_DataLen=sizeof(double)+sizeof(BYTE);
			break;
		case VT_STRING:
			m_DataLen=BinaryDataLen+sizeof(BYTE)+sizeof(UINT)+sizeof(char);
			break;
		case VT_USTRING:
			m_DataLen=BinaryDataLen+sizeof(BYTE)+sizeof(UINT)+sizeof(WCHAR);
			break;
		case VT_STRUCT:
			m_DataLen=BinaryDataLen+sizeof(BYTE)+sizeof(UINT);
			break;
		case VT_STRING_TINY:
			m_DataLen=BinaryDataLen+sizeof(BYTE)+sizeof(WORD)+sizeof(char);
			break;
		case VT_USTRING_TINY:
			m_DataLen=BinaryDataLen+sizeof(BYTE)+sizeof(WORD)+sizeof(WCHAR);
			break;
		case VT_STRUCT_TINY:
			m_DataLen=BinaryDataLen+sizeof(BYTE)+sizeof(WORD);
			break;
		case VT_BOOL:
			m_DataLen=sizeof(bool)+sizeof(BYTE);
			break;
		default:
			return false;
		}

		if(DataLen<m_DataLen)
		{
			Destory();
			return false;
		}

		m_DataLen=DataLen;

		if(ClearType==VT_UNKNOWN)
		{
			switch(DataType)
			{			
			case VT_STRING:
				*((char *)(m_pData+BinaryDataLen+sizeof(BYTE)+sizeof(UINT)))=0;
				break;
			case VT_USTRING:
				*((WCHAR *)(m_pData+BinaryDataLen+sizeof(BYTE)+sizeof(UINT)))=0;
				break;		
			case VT_STRING_TINY:
				*((char *)(m_pData+BinaryDataLen+sizeof(BYTE)+sizeof(WORD)))=0;
				break;
			case VT_USTRING_TINY:
				*((WCHAR *)(m_pData+BinaryDataLen+sizeof(BYTE)+sizeof(WORD)))=0;
				break;
			}
		}
		else
		{
			switch(ClearType)
			{
			case VT_CHAR:
			case VT_UCHAR:
				m_pData[0]=ClearType;
				m_pData[1]=0;
				break;
			case VT_SHORT:
			case VT_USHORT:
				m_pData[0]=ClearType;
				*((short *)(m_pData+1))=0;
				break;
			case VT_INT:
			case VT_UINT:
				m_pData[0]=ClearType;
				*((int *)(m_pData+1))=0;
				break;
			case VT_BIGINT:
			case VT_UBIGINT:
				m_pData[0]=ClearType;
				*((__int64 *)(m_pData+1))=0;
				break;
			case VT_FLOAT:
				m_pData[0]=ClearType;
				*((float *)(m_pData+1))=0;
				break;
			case VT_DOUBLE:
				m_pData[0]=ClearType;
				*((double *)(m_pData+1))=0;
				break;
			case VT_STRING:
				m_pData[0]=ClearType;
				*((UINT *)(m_pData+sizeof(BYTE)))=0;
				*((char *)(m_pData+sizeof(BYTE)+sizeof(UINT)))=0;
				break;
			case VT_USTRING:
				m_pData[0]=ClearType;
				*((UINT *)(m_pData+sizeof(BYTE)))=0;
				*((WCHAR *)(m_pData+sizeof(BYTE)+sizeof(UINT)))=0;
				break;	
			case VT_STRUCT:
				m_pData[0]=ClearType;
				*((UINT *)(m_pData+sizeof(BYTE)))=0;
				break;	
			case VT_STRING_TINY:
				m_pData[0]=ClearType;
				*((WORD *)(m_pData+sizeof(BYTE)))=0;
				*((char *)(m_pData+sizeof(BYTE)+sizeof(WORD)))=0;
				break;
			case VT_USTRING_TINY:
				m_pData[0]=ClearType;
				*((WORD *)(m_pData+sizeof(BYTE)))=0;
				*((WCHAR *)(m_pData+sizeof(BYTE)+sizeof(WORD)))=0;
				break;
			case VT_STRUCT_TINY:
				m_pData[0]=ClearType;
				*((WORD *)(m_pData+sizeof(BYTE)))=0;
				break;	
			case VT_BOOL:
				m_pData[0]=ClearType;
				*((bool *)(m_pData+1))=false;
				break;
			}
		}


		return true;
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
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
	}

	int GetType() const
	{
		if(m_pData==NULL)
			return VT_UNKNOWN;
		return m_pData[0];
	}
	UINT GetLength() const
	{
		switch(GetType())
		{		
		case VT_CHAR:
		case VT_UCHAR:
			return sizeof(char);
		case VT_SHORT:
		case VT_USHORT:
			return sizeof(short);
		case VT_INT:
		case VT_UINT:
			return sizeof(int);
		case VT_BIGINT:
		case VT_UBIGINT:
			return sizeof(__int64);
		case VT_FLOAT:
			return sizeof(float);
		case VT_DOUBLE:
			return sizeof(double);
		case VT_STRING:
			return *((UINT *)(m_pData+sizeof(BYTE)));
		case VT_USTRING:
			return *((UINT *)(m_pData+sizeof(BYTE)));
		case VT_STRUCT:
			return *((UINT *)(m_pData+sizeof(BYTE)));
		case VT_STRING_TINY:
			return *((WORD *)(m_pData+sizeof(BYTE)));
		case VT_USTRING_TINY:
			return *((WORD *)(m_pData+sizeof(BYTE)));
		case VT_STRUCT_TINY:
			return *((WORD *)(m_pData+sizeof(BYTE)));
		case VT_BOOL:
			return sizeof(bool);
		}
		return 0;
	}
	UINT GetDataLen()  const
	{		
		switch(GetType())
		{		
		case VT_CHAR:
		case VT_UCHAR:
			return sizeof(char)+sizeof(BYTE);
		case VT_SHORT:
		case VT_USHORT:
			return sizeof(short)+sizeof(BYTE);
		case VT_INT:
		case VT_UINT:
			return sizeof(int)+sizeof(BYTE);
		case VT_BIGINT:
		case VT_UBIGINT:
			return sizeof(__int64)+sizeof(BYTE);
		case VT_FLOAT:
			return sizeof(float)+sizeof(BYTE);
		case VT_DOUBLE:
			return sizeof(double)+sizeof(BYTE);
		case VT_STRING:
			return *((UINT *)(m_pData+sizeof(BYTE)))+sizeof(BYTE)+sizeof(UINT)+sizeof(char);
		case VT_USTRING:
			return *((UINT *)(m_pData+sizeof(BYTE)))+sizeof(BYTE)+sizeof(UINT)+sizeof(WCHAR);
		case VT_STRUCT:
			return *((UINT *)(m_pData+sizeof(BYTE)))+sizeof(BYTE)+sizeof(UINT);
		case VT_STRING_TINY:
			return *((WORD *)(m_pData+sizeof(BYTE)))+sizeof(BYTE)+sizeof(WORD)+sizeof(char);
		case VT_USTRING_TINY:
			return *((WORD *)(m_pData+sizeof(BYTE)))+sizeof(BYTE)+sizeof(WORD)+sizeof(WCHAR);
		case VT_STRUCT_TINY:
			return *((WORD *)(m_pData+sizeof(BYTE)))+sizeof(BYTE)+sizeof(WORD);
		case VT_BOOL:
			return sizeof(bool)+sizeof(BYTE);
		}
		return 0;
	}
	LPVOID GetData() const
	{
		return m_pData;
	}

	operator char() const
	{
		switch(GetType())
		{		
		case VT_CHAR:
			return (char)(*((char *)(m_pData+1)));
		case VT_UCHAR:
			return (char)(*((unsigned char *)(m_pData+1)));
		case VT_SHORT:
			return (char)(*((short *)(m_pData+1)));
		case VT_USHORT:
			return (char)(*((unsigned short *)(m_pData+1)));
		case VT_INT:
			return (char)(*((int *)(m_pData+1)));
		case VT_UINT:
			return (char)(*((unsigned int *)(m_pData+1)));
		case VT_BIGINT:
			return (char)(*((__int64 *)(m_pData+1)));
		case VT_UBIGINT:
			return (char)(*((unsigned __int64 *)(m_pData+1)));
		case VT_FLOAT:
			return (char)(*((float *)(m_pData+1)));
		case VT_DOUBLE:
			return (char)(*((double *)(m_pData+1)));
		case VT_BOOL:
			return (char)(*((bool *)(m_pData+1)));
		}
		return 0;
	}
	operator unsigned char() const
	{
		switch(GetType())
		{		
		case VT_CHAR:
			return (unsigned char)(*((char *)(m_pData+1)));
		case VT_UCHAR:
			return (unsigned char)(*((unsigned char *)(m_pData+1)));
		case VT_SHORT:
			return (unsigned char)(*((short *)(m_pData+1)));
		case VT_USHORT:
			return (unsigned char)(*((unsigned short *)(m_pData+1)));
		case VT_INT:
			return (unsigned char)(*((int *)(m_pData+1)));
		case VT_UINT:
			return (unsigned char)(*((unsigned int *)(m_pData+1)));
		case VT_BIGINT:
			return (unsigned char)(*((__int64 *)(m_pData+1)));
		case VT_UBIGINT:
			return (unsigned char)(*((unsigned __int64 *)(m_pData+1)));
		case VT_FLOAT:
			return (unsigned char)(*((float *)(m_pData+1)));
		case VT_DOUBLE:
			return (unsigned char)(*((double *)(m_pData+1)));
		case VT_BOOL:
			return (unsigned char)(*((bool *)(m_pData+1)));
		}
		return 0;
	}
	operator short() const
	{
		switch(GetType())
		{		
		case VT_CHAR:
			return (short)(*((char *)(m_pData+1)));
		case VT_UCHAR:
			return (short)(*((unsigned char *)(m_pData+1)));
		case VT_SHORT:
			return (short)(*((short *)(m_pData+1)));
		case VT_USHORT:
			return (short)(*((unsigned short *)(m_pData+1)));
		case VT_INT:
			return (short)(*((int *)(m_pData+1)));
		case VT_UINT:
			return (short)(*((unsigned int *)(m_pData+1)));
		case VT_BIGINT:
			return (short)(*((__int64 *)(m_pData+1)));
		case VT_UBIGINT:
			return (short)(*((unsigned __int64 *)(m_pData+1)));
		case VT_FLOAT:
			return (short)(*((float *)(m_pData+1)));
		case VT_DOUBLE:
			return (short)(*((double *)(m_pData+1)));
		case VT_BOOL:
			return (short)(*((bool *)(m_pData+1)));
		}
		return 0;
	}
	operator unsigned short() const
	{
		switch(GetType())
		{		
		case VT_CHAR:
			return (unsigned short)(*((char *)(m_pData+1)));
		case VT_UCHAR:
			return (unsigned short)(*((unsigned char *)(m_pData+1)));
		case VT_SHORT:
			return (unsigned short)(*((short *)(m_pData+1)));
		case VT_USHORT:
			return (unsigned short)(*((unsigned short *)(m_pData+1)));
		case VT_INT:
			return (unsigned short)(*((int *)(m_pData+1)));
		case VT_UINT:
			return (unsigned short)(*((unsigned int *)(m_pData+1)));
		case VT_BIGINT:
			return (unsigned short)(*((__int64 *)(m_pData+1)));
		case VT_UBIGINT:
			return (unsigned short)(*((unsigned __int64 *)(m_pData+1)));
		case VT_FLOAT:
			return (unsigned short)(*((float *)(m_pData+1)));
		case VT_DOUBLE:
			return (unsigned short)(*((double *)(m_pData+1)));
		case VT_BOOL:
			return (unsigned short)(*((bool *)(m_pData+1)));
		}
		return 0;
	}
	operator int() const
	{
		switch(GetType())
		{		
		case VT_CHAR:
			return (int)(*((char *)(m_pData+1)));
		case VT_UCHAR:
			return (int)(*((unsigned char *)(m_pData+1)));
		case VT_SHORT:
			return (int)(*((short *)(m_pData+1)));
		case VT_USHORT:
			return (int)(*((unsigned short *)(m_pData+1)));
		case VT_INT:
			return (int)(*((int *)(m_pData+1)));
		case VT_UINT:
			return (int)(*((unsigned int *)(m_pData+1)));
		case VT_BIGINT:
			return (int)(*((__int64 *)(m_pData+1)));
		case VT_UBIGINT:
			return (int)(*((unsigned __int64 *)(m_pData+1)));
		case VT_FLOAT:
			return (int)(*((float *)(m_pData+1)));
		case VT_DOUBLE:
			return (int)(*((double *)(m_pData+1)));
		case VT_BOOL:
			return (int)(*((bool *)(m_pData+1)));
		}
		return 0;
	}
	operator unsigned int() const
	{
		switch(GetType())
		{		
		case VT_CHAR:
			return (unsigned int)(*((char *)(m_pData+1)));
		case VT_UCHAR:
			return (unsigned int)(*((unsigned char *)(m_pData+1)));
		case VT_SHORT:
			return (unsigned int)(*((short *)(m_pData+1)));
		case VT_USHORT:
			return (unsigned int)(*((unsigned short *)(m_pData+1)));
		case VT_INT:
			return (unsigned int)(*((int *)(m_pData+1)));
		case VT_UINT:
			return (unsigned int)(*((unsigned int *)(m_pData+1)));
		case VT_BIGINT:
			return (unsigned int)(*((__int64 *)(m_pData+1)));
		case VT_UBIGINT:
			return (unsigned int)(*((unsigned __int64 *)(m_pData+1)));
		case VT_FLOAT:
			return (unsigned int)(*((float *)(m_pData+1)));
		case VT_DOUBLE:
			return (unsigned int)(*((double *)(m_pData+1)));
		case VT_BOOL:
			return (unsigned int)(*((bool *)(m_pData+1)));
		}
		return 0;
	}
	//operator long() const
	//{
	//	switch(GetType())
	//	{		
	//	case VT_CHAR:
	//		return (long)(*((char *)(m_pData+1)));
	//	case VT_UCHAR:
	//		return (long)(*((unsigned char *)(m_pData+1)));
	//	case VT_SHORT:
	//		return (long)(*((short *)(m_pData+1)));
	//	case VT_USHORT:
	//		return (long)(*((unsigned short *)(m_pData+1)));
	//	case VT_INT:
	//		return (long)(*((int *)(m_pData+1)));
	//	case VT_UINT:
	//		return (long)(*((unsigned int *)(m_pData+1)));
	//	case VT_BIGINT:
	//		return (long)(*((__int64 *)(m_pData+1)));
	//	case VT_UBIGINT:
	//		return (long)(*((unsigned __int64 *)(m_pData+1)));
	//	case VT_FLOAT:
	//		return (long)(*((float *)(m_pData+1)));
	//	case VT_DOUBLE:
	//		return (long)(*((double *)(m_pData+1)));
	//	case VT_BOOL:
	//		return (long)(*((bool *)(m_pData+1)));
	//	}
	//	return 0;
	//}
	//operator unsigned long() const
	//{
	//	switch(GetType())
	//	{		
	//	case VT_CHAR:
	//		return (unsigned long)(*((char *)(m_pData+1)));
	//	case VT_UCHAR:
	//		return (unsigned long)(*((unsigned char *)(m_pData+1)));
	//	case VT_SHORT:
	//		return (unsigned long)(*((short *)(m_pData+1)));
	//	case VT_USHORT:
	//		return (unsigned long)(*((unsigned short *)(m_pData+1)));
	//	case VT_INT:
	//		return (unsigned long)(*((int *)(m_pData+1)));
	//	case VT_UINT:
	//		return (unsigned long)(*((unsigned int *)(m_pData+1)));
	//	case VT_BIGINT:
	//		return (unsigned long)(*((__int64 *)(m_pData+1)));
	//	case VT_UBIGINT:
	//		return (unsigned long)(*((unsigned __int64 *)(m_pData+1)));
	//	case VT_FLOAT:
	//		return (unsigned long)(*((float *)(m_pData+1)));
	//	case VT_DOUBLE:
	//		return (unsigned long)(*((double *)(m_pData+1)));
	//	case VT_BOOL:
	//		return (unsigned long)(*((bool *)(m_pData+1)));
	//	}
	//	return 0;
	//}
	operator __int64() const
	{
		switch(GetType())
		{		
		case VT_CHAR:
			return (__int64)(*((char *)(m_pData+1)));
		case VT_UCHAR:
			return (__int64)(*((unsigned char *)(m_pData+1)));
		case VT_SHORT:
			return (__int64)(*((short *)(m_pData+1)));
		case VT_USHORT:
			return (__int64)(*((unsigned short *)(m_pData+1)));
		case VT_INT:
			return (__int64)(*((int *)(m_pData+1)));
		case VT_UINT:
			return (__int64)(*((unsigned int *)(m_pData+1)));
		case VT_BIGINT:
			return (__int64)(*((__int64 *)(m_pData+1)));
		case VT_UBIGINT:
			return (__int64)(*((unsigned __int64 *)(m_pData+1)));
		case VT_FLOAT:
			return (__int64)(*((float *)(m_pData+1)));
		case VT_DOUBLE:
			return (__int64)(*((double *)(m_pData+1)));
		case VT_BOOL:
			return (__int64)(*((bool *)(m_pData+1)));
		}
		return 0;
	}
	operator unsigned __int64() const
	{
		switch(GetType())
		{		
		case VT_CHAR:
			return (unsigned __int64)(*((char *)(m_pData+1)));
		case VT_UCHAR:
			return (unsigned __int64)(*((unsigned char *)(m_pData+1)));
		case VT_SHORT:
			return (unsigned __int64)(*((short *)(m_pData+1)));
		case VT_USHORT:
			return (unsigned __int64)(*((unsigned short *)(m_pData+1)));
		case VT_INT:
			return (unsigned __int64)(*((int *)(m_pData+1)));
		case VT_UINT:
			return (unsigned __int64)(*((unsigned int *)(m_pData+1)));
		case VT_BIGINT:
			return (unsigned __int64)(*((__int64 *)(m_pData+1)));
		case VT_UBIGINT:
			return (unsigned __int64)(*((unsigned __int64 *)(m_pData+1)));
		case VT_FLOAT:
			return (unsigned __int64)(*((float *)(m_pData+1)));
		case VT_DOUBLE:
			return (unsigned __int64)(*((double *)(m_pData+1)));
		case VT_BOOL:
			return (unsigned __int64)(*((bool *)(m_pData+1)));
		}
		return 0;
	}

	operator float() const
	{
		switch(GetType())
		{		
		case VT_CHAR:
			return (float)(*((char *)(m_pData+1)));
		case VT_UCHAR:
			return (float)(*((unsigned char *)(m_pData+1)));
		case VT_SHORT:
			return (float)(*((short *)(m_pData+1)));
		case VT_USHORT:
			return (float)(*((unsigned short *)(m_pData+1)));
		case VT_INT:
			return (float)(*((int *)(m_pData+1)));
		case VT_UINT:
			return (float)(*((unsigned int *)(m_pData+1)));
		case VT_BIGINT:
			return (float)(*((__int64 *)(m_pData+1)));
		case VT_UBIGINT:
			return (float)(*((unsigned __int64 *)(m_pData+1)));
		case VT_FLOAT:
			return (float)(*((float *)(m_pData+1)));
		case VT_DOUBLE:
			return (float)(*((double *)(m_pData+1)));
		case VT_BOOL:
			return (float)(*((bool *)(m_pData+1)));
		}
		return 0;
	}

	operator double() const
	{
		switch(GetType())
		{		
		case VT_CHAR:
			return (double)(*((char *)(m_pData+1)));
		case VT_UCHAR:
			return (double)(*((unsigned char *)(m_pData+1)));
		case VT_SHORT:
			return (double)(*((short *)(m_pData+1)));
		case VT_USHORT:
			return (double)(*((unsigned short *)(m_pData+1)));
		case VT_INT:
			return (double)(*((int *)(m_pData+1)));
		case VT_UINT:
			return (double)(*((unsigned int *)(m_pData+1)));
		case VT_BIGINT:
			return (double)(*((__int64 *)(m_pData+1)));
		case VT_UBIGINT:
			return (double)(*((unsigned __int64 *)(m_pData+1)));
		case VT_FLOAT:
			return (double)(*((float *)(m_pData+1)));
		case VT_DOUBLE:
			return (double)(*((double *)(m_pData+1)));
		case VT_BOOL:
			return (double)(*((bool *)(m_pData+1)));
		}
		return 0;
	}

	operator const char *() const
	{
		switch(GetType())
		{	
		case VT_STRING:
			return (const char *)(m_pData+sizeof(BYTE)+sizeof(UINT));
		case VT_STRING_TINY:
			return (const char *)(m_pData+sizeof(BYTE)+sizeof(WORD));
		}		
		return "";
	}

	operator const WCHAR *() const
	{
		static WCHAR pEmptyStr[1]={0};
		switch(GetType())
		{			
		case VT_USTRING:
			return (WCHAR *)(m_pData+sizeof(BYTE)+sizeof(UINT));
		case VT_USTRING_TINY:
			return (WCHAR *)(m_pData+sizeof(BYTE)+sizeof(WORD));
		}		
		return pEmptyStr;
	}

	operator CEasyString() const
	{
		switch (GetType())
		{
		case VT_STRING:
			return CEasyString((const char *)(m_pData + sizeof(BYTE) + sizeof(UINT)));
		case VT_STRING_TINY:
			return CEasyString((const char *)(m_pData + sizeof(BYTE) + sizeof(WORD)));
		case VT_USTRING:
			return CEasyString((WCHAR *)(m_pData + sizeof(BYTE) + sizeof(UINT)));
		case VT_USTRING_TINY:
			return CEasyString((WCHAR *)(m_pData + sizeof(BYTE) + sizeof(WORD)));
		}
	}


	operator bool() const
	{
		switch(GetType())
		{		
		case VT_CHAR:
			return (*((char *)(m_pData+1)))!=0;
		case VT_UCHAR:
			return (*((unsigned char *)(m_pData+1)))!=0;
		case VT_SHORT:
			return (*((short *)(m_pData+1)))!=0;
		case VT_USHORT:
			return (*((unsigned short *)(m_pData+1)))!=0;
		case VT_INT:
			return (*((int *)(m_pData+1)))!=0;
		case VT_UINT:
			return (*((unsigned int *)(m_pData+1)))!=0;
		case VT_BIGINT:
			return (*((__int64 *)(m_pData+1)))!=0;
		case VT_UBIGINT:
			return (*((unsigned __int64 *)(m_pData+1)))!=0;
		case VT_FLOAT:
			return (*((float *)(m_pData+1)))!=0;
		case VT_DOUBLE:
			return (*((double *)(m_pData+1)))!=0;
		case VT_BOOL:
			return (*((bool *)(m_pData+1)));
		}
		return false;
	}

//	const CEasyString ToStr() const
//	{
//		CEasyString String;
//		GetStr(String);		
//		return String;
//	}
//
//	void GetStr(CEasyString& String) const
//	{
//		switch(GetType())
//		{	
//		case VT_STRING:
//#ifdef WIN32
//			if(CSmartValue::IsConvertWideCharToUTF8()&&String.IsUnicode())
//			{
//				const char * szSrc=(const char *)(m_pData+sizeof(BYTE)+sizeof(UINT));
//				int SrcLen=(int)strlen(szSrc);
//				int DestLen=0;
//				DestLen=MultiByteToWideChar(CP_UTF8,0,szSrc,SrcLen,NULL,0);
//				String.Resize(DestLen,false);
//				MultiByteToWideChar(CP_UTF8,0,szSrc,SrcLen,(WCHAR *)String.GetBuffer(),DestLen);
//				String.TrimBuffer();
//			}
//			else
//#endif
//			{
//				String=(const char *)(m_pData+sizeof(BYTE)+sizeof(UINT));
//			}			
//			break;
//		case VT_STRING_TINY:
//#ifdef WIN32
//			if(CSmartValue::IsConvertWideCharToUTF8()&&String.IsUnicode())
//			{
//				const char * szSrc=(const char *)(m_pData+sizeof(BYTE)+sizeof(WORD));
//				int SrcLen = (int)strlen(szSrc);
//				int DestLen=0;
//				DestLen=MultiByteToWideChar(CP_UTF8,0,szSrc,SrcLen,NULL,0);
//				String.Resize(DestLen,false);
//				MultiByteToWideChar(CP_UTF8,0,szSrc,SrcLen,(WCHAR *)String.GetBuffer(),DestLen);
//				String.TrimBuffer();
//			}
//			else
//#endif
//			{
//				String=(const char *)(m_pData+sizeof(BYTE)+sizeof(WORD));
//			}			
//			break;
//		case VT_USTRING:
//			String=(WCHAR *)(m_pData+sizeof(BYTE)+sizeof(UINT));
//			break;
//		case VT_USTRING_TINY:
//			String=(WCHAR *)(m_pData+sizeof(BYTE)+sizeof(WORD));
//			break;
//		}
//	}
	
	void operator=(const CSmartValue& Value)
	{
		m_pData=NULL;
		m_DataLen=0;
		m_IsSelfData=true;
		m_AllowChange=true;
		Attach(Value.GetData(),Value.GetDataLen(),VT_UNKNOWN);
	}
	void SetValue(const CSmartValue& Value)
	{
		if(m_pData==NULL)
			return;
		switch(Value.GetType())
		{		
		case VT_CHAR:
			*this=(char)Value;
			break;
		case VT_UCHAR:
			*this=(unsigned char)Value;
			break;
		case VT_SHORT:
			*this=(short)Value;
			break;
		case VT_USHORT:
			*this=(unsigned short)Value;
			break;
		case VT_INT:
			*this=(int)Value;
			break;
		case VT_UINT:
			*this=(unsigned int)Value;
			break;
		case VT_BIGINT:
			*this=(__int64)Value;
			break;
		case VT_UBIGINT:
			*this=(unsigned __int64)Value;
			break;
		case VT_FLOAT:
			*this=(float)Value;
			break;
		case VT_DOUBLE:
			*this=(double)Value;
			break;
		case VT_STRING:
			*this=(LPCTSTR)Value;
			break;
		case VT_USTRING:
			*this=(LPCWSTR)Value;
			break;
		case VT_BOOL:
			*this=(bool)Value;
		}
	}
	void operator=(char Value)
	{
		if(m_pData==NULL)
			return;
		switch(GetType())
		{		
		case VT_CHAR:
			*((char *)(m_pData+1))=(char)Value;
			break;
		case VT_UCHAR:
			*((unsigned char *)(m_pData+1))=(unsigned char)Value;
			break;
		case VT_SHORT:
			*((short *)(m_pData+1))=(short)Value;
			break;
		case VT_USHORT:
			*((unsigned short *)(m_pData+1))=(unsigned short)Value;
			break;
		case VT_INT:
			*((int *)(m_pData+1))=(int)Value;
			break;
		case VT_UINT:
			*((unsigned int *)(m_pData+1))=(unsigned int)Value;
			break;
		case VT_BIGINT:
			*((__int64 *)(m_pData+1))=(__int64)Value;
			break;
		case VT_UBIGINT:
			*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
			break;
		case VT_FLOAT:
			*((float *)(m_pData+1))=(float)Value;
			break;
		case VT_DOUBLE:
			*((double *)(m_pData+1))=(double)Value;
			break;
		case VT_BOOL:
			*((bool *)(m_pData+1))=(Value!=0);
			break;
		}
	}
	void operator=(unsigned char Value)
	{
		if(m_pData==NULL)
			return;
		switch(GetType())
		{		
		case VT_CHAR:
			*((char *)(m_pData+1))=(char)Value;
			break;
		case VT_UCHAR:
			*((unsigned char *)(m_pData+1))=(unsigned char)Value;
			break;
		case VT_SHORT:
			*((short *)(m_pData+1))=(short)Value;
			break;
		case VT_USHORT:
			*((unsigned short *)(m_pData+1))=(unsigned short)Value;
			break;
		case VT_INT:
			*((int *)(m_pData+1))=(int)Value;
			break;
		case VT_UINT:
			*((unsigned int *)(m_pData+1))=(unsigned int)Value;
			break;
		case VT_BIGINT:
			*((__int64 *)(m_pData+1))=(__int64)Value;
			break;
		case VT_UBIGINT:
			*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
			break;
		case VT_FLOAT:
			*((float *)(m_pData+1))=(float)Value;
			break;
		case VT_DOUBLE:
			*((double *)(m_pData+1))=(double)Value;
			break;
		case VT_BOOL:
			*((bool *)(m_pData+1))=(Value!=0);
			break;
		}
	}
	void operator=(short Value)
	{
		if(m_pData==NULL)
			return;
		switch(GetType())
		{		
		case VT_CHAR:
			*((char *)(m_pData+1))=(char)Value;
			break;
		case VT_UCHAR:
			*((unsigned char *)(m_pData+1))=(unsigned char)Value;
			break;
		case VT_SHORT:
			*((short *)(m_pData+1))=(short)Value;
			break;
		case VT_USHORT:
			*((unsigned short *)(m_pData+1))=(unsigned short)Value;
			break;
		case VT_INT:
			*((int *)(m_pData+1))=(int)Value;
			break;
		case VT_UINT:
			*((unsigned int *)(m_pData+1))=(unsigned int)Value;
			break;
		case VT_BIGINT:
			*((__int64 *)(m_pData+1))=(__int64)Value;
			break;
		case VT_UBIGINT:
			*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
			break;
		case VT_FLOAT:
			*((float *)(m_pData+1))=(float)Value;
			break;
		case VT_DOUBLE:
			*((double *)(m_pData+1))=(double)Value;
			break;
		case VT_BOOL:
			*((bool *)(m_pData+1))=(Value!=0);
			break;
		}
	}
	void operator=(unsigned short Value)
	{
		if(m_pData==NULL)
			return;
		switch(GetType())
		{		
		case VT_CHAR:
			*((char *)(m_pData+1))=(char)Value;
			break;
		case VT_UCHAR:
			*((unsigned char *)(m_pData+1))=(unsigned char)Value;
			break;
		case VT_SHORT:
			*((short *)(m_pData+1))=(short)Value;
			break;
		case VT_USHORT:
			*((unsigned short *)(m_pData+1))=(unsigned short)Value;
			break;
		case VT_INT:
			*((int *)(m_pData+1))=(int)Value;
			break;
		case VT_UINT:
			*((unsigned int *)(m_pData+1))=(unsigned int)Value;
			break;
		case VT_BIGINT:
			*((__int64 *)(m_pData+1))=(__int64)Value;
			break;
		case VT_UBIGINT:
			*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
			break;
		case VT_FLOAT:
			*((float *)(m_pData+1))=(float)Value;
			break;
		case VT_DOUBLE:
			*((double *)(m_pData+1))=(double)Value;
			break;
		case VT_BOOL:
			*((bool *)(m_pData+1))=(Value!=0);
			break;
		}
	}	
	void operator=(int Value)
	{
		if(m_pData==NULL)
			return;
		switch(GetType())
		{		
		case VT_CHAR:
			*((char *)(m_pData+1))=(char)Value;
			break;
		case VT_UCHAR:
			*((unsigned char *)(m_pData+1))=(unsigned char)Value;
			break;
		case VT_SHORT:
			*((short *)(m_pData+1))=(short)Value;
			break;
		case VT_USHORT:
			*((unsigned short *)(m_pData+1))=(unsigned short)Value;
			break;
		case VT_INT:
			*((int *)(m_pData+1))=(int)Value;
			break;
		case VT_UINT:
			*((unsigned int *)(m_pData+1))=(unsigned int)Value;
			break;
		case VT_BIGINT:
			*((__int64 *)(m_pData+1))=(__int64)Value;
			break;
		case VT_UBIGINT:
			*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
			break;
		case VT_FLOAT:
			*((float *)(m_pData+1))=(float)Value;
			break;
		case VT_DOUBLE:
			*((double *)(m_pData+1))=(double)Value;
			break;
		case VT_BOOL:
			*((bool *)(m_pData+1))=(Value!=0);
			break;
		}
	}
	void operator=(unsigned int Value)
	{
		if(m_pData==NULL)
			return;
		switch(GetType())
		{		
		case VT_CHAR:
			*((char *)(m_pData+1))=(char)Value;
			break;
		case VT_UCHAR:
			*((unsigned char *)(m_pData+1))=(unsigned char)Value;
			break;
		case VT_SHORT:
			*((short *)(m_pData+1))=(short)Value;
			break;
		case VT_USHORT:
			*((unsigned short *)(m_pData+1))=(unsigned short)Value;
			break;
		case VT_INT:
			*((int *)(m_pData+1))=(int)Value;
			break;
		case VT_UINT:
			*((unsigned int *)(m_pData+1))=(unsigned int)Value;
			break;
		case VT_BIGINT:
			*((__int64 *)(m_pData+1))=(__int64)Value;
			break;
		case VT_UBIGINT:
			*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
			break;
		case VT_FLOAT:
			*((float *)(m_pData+1))=(float)Value;
			break;
		case VT_DOUBLE:
			*((double *)(m_pData+1))=(double)Value;
			break;
		case VT_BOOL:
			*((bool *)(m_pData+1))=(Value!=0);
			break;
		}
	}
	//void operator=(long Value)
	//{
	//	if(m_pData==NULL)
	//		return;
	//	switch(GetType())
	//	{		
	//	case VT_CHAR:
	//		*((char *)(m_pData+1))=(char)Value;
	//		break;
	//	case VT_UCHAR:
	//		*((unsigned char *)(m_pData+1))=(unsigned char)Value;
	//		break;
	//	case VT_SHORT:
	//		*((short *)(m_pData+1))=(short)Value;
	//		break;
	//	case VT_USHORT:
	//		*((unsigned short *)(m_pData+1))=(unsigned short)Value;
	//		break;
	//	case VT_INT:
	//		*((int *)(m_pData+1))=(int)Value;
	//		break;
	//	case VT_UINT:
	//		*((unsigned int *)(m_pData+1))=(unsigned int)Value;
	//		break;
	//	case VT_BIGINT:
	//		*((__int64 *)(m_pData+1))=(__int64)Value;
	//		break;
	//	case VT_UBIGINT:
	//		*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
	//		break;
	//	case VT_FLOAT:
	//		*((float *)(m_pData+1))=(float)Value;
	//		break;
	//	case VT_DOUBLE:
	//		*((double *)(m_pData+1))=(double)Value;
	//		break;
	//	case VT_BOOL:
	//		*((bool *)(m_pData+1))=(Value!=0);
	//		break;
	//	}
	//}
	//void operator=(unsigned long Value)
	//{
	//	if(m_pData==NULL)
	//		return;
	//	switch(GetType())
	//	{		
	//	case VT_CHAR:
	//		*((char *)(m_pData+1))=(char)Value;
	//		break;
	//	case VT_UCHAR:
	//		*((unsigned char *)(m_pData+1))=(unsigned char)Value;
	//		break;
	//	case VT_SHORT:
	//		*((short *)(m_pData+1))=(short)Value;
	//		break;
	//	case VT_USHORT:
	//		*((unsigned short *)(m_pData+1))=(unsigned short)Value;
	//		break;
	//	case VT_INT:
	//		*((int *)(m_pData+1))=(int)Value;
	//		break;
	//	case VT_UINT:
	//		*((unsigned int *)(m_pData+1))=(unsigned int)Value;
	//		break;
	//	case VT_BIGINT:
	//		*((__int64 *)(m_pData+1))=(__int64)Value;
	//		break;
	//	case VT_UBIGINT:
	//		*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
	//		break;
	//	case VT_FLOAT:
	//		*((float *)(m_pData+1))=(float)Value;
	//		break;
	//	case VT_DOUBLE:
	//		*((double *)(m_pData+1))=(double)Value;
	//		break;
	//	case VT_BOOL:
	//		*((bool *)(m_pData+1))=(Value!=0);
	//		break;
	//	}
	//}
	void operator=(__int64 Value)
	{
		if(m_pData==NULL)
			return;
		switch(GetType())
		{		
		case VT_CHAR:
			*((char *)(m_pData+1))=(char)Value;
			break;
		case VT_UCHAR:
			*((unsigned char *)(m_pData+1))=(unsigned char)Value;
			break;
		case VT_SHORT:
			*((short *)(m_pData+1))=(short)Value;
			break;
		case VT_USHORT:
			*((unsigned short *)(m_pData+1))=(unsigned short)Value;
			break;
		case VT_INT:
			*((int *)(m_pData+1))=(int)Value;
			break;
		case VT_UINT:
			*((unsigned int *)(m_pData+1))=(unsigned int)Value;
			break;
		case VT_BIGINT:
			*((__int64 *)(m_pData+1))=(__int64)Value;
			break;
		case VT_UBIGINT:
			*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
			break;
		case VT_FLOAT:
			*((float *)(m_pData+1))=(float)Value;
			break;
		case VT_DOUBLE:
			*((double *)(m_pData+1))=(double)Value;
			break;
		case VT_BOOL:
			*((bool *)(m_pData+1))=(Value!=0);
			break;
		}
	}
	void operator=(unsigned __int64 Value)
	{
		if(m_pData==NULL)
			return;
		switch(GetType())
		{		
		case VT_CHAR:
			*((char *)(m_pData+1))=(char)Value;
			break;
		case VT_UCHAR:
			*((unsigned char *)(m_pData+1))=(unsigned char)Value;
			break;
		case VT_SHORT:
			*((short *)(m_pData+1))=(short)Value;
			break;
		case VT_USHORT:
			*((unsigned short *)(m_pData+1))=(unsigned short)Value;
			break;
		case VT_INT:
			*((int *)(m_pData+1))=(int)Value;
			break;
		case VT_UINT:
			*((unsigned int *)(m_pData+1))=(unsigned int)Value;
			break;
		case VT_BIGINT:
			*((__int64 *)(m_pData+1))=(__int64)Value;
			break;
		case VT_UBIGINT:
			*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
			break;
		case VT_FLOAT:
			*((float *)(m_pData+1))=(float)Value;
			break;
		case VT_DOUBLE:
			*((double *)(m_pData+1))=(double)Value;
			break;
		case VT_BOOL:
			*((bool *)(m_pData+1))=(Value!=0);
			break;
		}
	}
	void operator=(float Value)
	{
		if(m_pData==NULL)
			return;
		switch(GetType())
		{		
		case VT_CHAR:
			*((char *)(m_pData+1))=(char)Value;
			break;
		case VT_UCHAR:
			*((unsigned char *)(m_pData+1))=(unsigned char)Value;
			break;
		case VT_SHORT:
			*((short *)(m_pData+1))=(short)Value;
			break;
		case VT_USHORT:
			*((unsigned short *)(m_pData+1))=(unsigned short)Value;
			break;
		case VT_INT:
			*((int *)(m_pData+1))=(int)Value;
			break;
		case VT_UINT:
			*((unsigned int *)(m_pData+1))=(unsigned int)Value;
			break;
		case VT_BIGINT:
			*((__int64 *)(m_pData+1))=(__int64)Value;
			break;
		case VT_UBIGINT:
			*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
			break;
		case VT_FLOAT:
			*((float *)(m_pData+1))=(float)Value;
			break;
		case VT_DOUBLE:
			*((double *)(m_pData+1))=(double)Value;
			break;
		case VT_BOOL:
			*((bool *)(m_pData+1))=(Value!=0);
			break;
		}
	}

	void operator=(double Value)
	{
		if(m_pData==NULL)
			return;
		switch(GetType())
		{		
		case VT_CHAR:
			*((char *)(m_pData+1))=(char)Value;
			break;
		case VT_UCHAR:
			*((unsigned char *)(m_pData+1))=(unsigned char)Value;
			break;
		case VT_SHORT:
			*((short *)(m_pData+1))=(short)Value;
			break;
		case VT_USHORT:
			*((unsigned short *)(m_pData+1))=(unsigned short)Value;
			break;
		case VT_INT:
			*((int *)(m_pData+1))=(int)Value;
			break;
		case VT_UINT:
			*((unsigned int *)(m_pData+1))=(unsigned int)Value;
			break;
		case VT_BIGINT:
			*((__int64 *)(m_pData+1))=(__int64)Value;
			break;
		case VT_UBIGINT:
			*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
			break;
		case VT_FLOAT:
			*((float *)(m_pData+1))=(float)Value;
			break;
		case VT_DOUBLE:
			*((double *)(m_pData+1))=(double)Value;
			break;
		case VT_BOOL:
			*((bool *)(m_pData+1))=(Value!=0);
			break;
		}
	}

	void operator=(const char * Value)
	{
		Destory();
		if(Value)
			SetString(Value,(UINT)strlen(Value));
	}

	void operator=(const WCHAR * Value)
	{
		Destory();
		if(Value)
			SetString(Value,(UINT)wcslen(Value));
	}

	void operator=(const CEasyString& Value)
	{
		Destory();
		SetString(Value,(UINT)Value.GetLength());
	}	

	void SetString(const char * pStr,UINT Len)
	{
		if(GetType()==VT_STRING)			
		{
			UINT MaxLen=m_DataLen-sizeof(BYTE)-sizeof(UINT)-sizeof(char);
			if(Len>MaxLen)
				Len=MaxLen;
			*((UINT *)(m_pData+sizeof(BYTE)))=Len*sizeof(char);
			memcpy(m_pData+sizeof(BYTE)+sizeof(UINT),pStr,Len*sizeof(char));
			*((char *)(m_pData+sizeof(BYTE)+sizeof(UINT)+Len*sizeof(char)))=0;
		}
		else if(GetType()==VT_STRING_TINY)
		{
			WORD MaxLen=(WORD)(m_DataLen-sizeof(BYTE)-sizeof(WORD)-sizeof(char));
			if(Len>MaxLen)
				Len=MaxLen;
			*((WORD *)(m_pData+sizeof(BYTE)))=(WORD)(Len*sizeof(char));
			memcpy(m_pData+sizeof(BYTE)+sizeof(WORD),pStr,Len*sizeof(char));
			*((char *)(m_pData+sizeof(BYTE)+sizeof(WORD)+Len*sizeof(char)))=0;
		}
	}

	void SetString(const WCHAR * pStr,UINT Len)
	{
		if(GetType()==VT_USTRING)			
		{
			UINT MaxLen=m_DataLen-sizeof(BYTE)-sizeof(UINT)-sizeof(WCHAR);
			if(Len>MaxLen)
				Len=MaxLen;		
			*((UINT *)(m_pData+sizeof(BYTE)))=Len*sizeof(WCHAR);
			memcpy(m_pData+sizeof(BYTE)+sizeof(UINT),pStr,Len*sizeof(WCHAR));
			*((WCHAR *)(m_pData+sizeof(BYTE)+sizeof(UINT)+Len*sizeof(WCHAR)))=0;
		}
		else if(GetType()==VT_USTRING_TINY)
		{
			WORD MaxLen=(WORD)(m_DataLen-sizeof(BYTE)-sizeof(WORD)-sizeof(WCHAR));
			if(Len>MaxLen)
				Len=MaxLen;		
			*((WORD *)(m_pData+sizeof(BYTE)))=(WORD)(Len*sizeof(WCHAR));
			memcpy(m_pData+sizeof(BYTE)+sizeof(WORD),pStr,Len*sizeof(WCHAR));
			*((WCHAR *)(m_pData+sizeof(BYTE)+sizeof(WORD)+Len*sizeof(WCHAR)))=0;
		}
		//else if(GetType()==VT_STRING)			
		//{
		//	UINT UTF8Len=(UINT)UnicodeToUTF8(pStr,Len,NULL,0);
		//	UINT MaxLen=m_DataLen-sizeof(BYTE)-sizeof(UINT)-sizeof(char);
		//	if(UTF8Len>MaxLen)
		//		UTF8Len=MaxLen;
		//	*((UINT *)(m_pData+sizeof(BYTE)))=UTF8Len*sizeof(char);
		//	UnicodeToUTF8(pStr,Len,(char *)m_pData+sizeof(BYTE)+sizeof(UINT),UTF8Len);			
		//	*((char *)(m_pData+sizeof(BYTE)+sizeof(UINT)+UTF8Len*sizeof(char)))=0;
		//}
		//else if(GetType()==VT_STRING_TINY)
		//{
		//	UINT UTF8Len=(UINT)UnicodeToUTF8(pStr,Len,NULL,0);
		//	WORD MaxLen=(WORD)(m_DataLen-sizeof(BYTE)-sizeof(WORD)-sizeof(char));
		//	if(UTF8Len>MaxLen)
		//		UTF8Len=MaxLen;
		//	*((WORD *)(m_pData+sizeof(BYTE)))=(WORD)(UTF8Len*sizeof(char));
		//	UnicodeToUTF8(pStr,Len,(char *)m_pData+sizeof(BYTE)+sizeof(WORD),UTF8Len);
		//	*((char *)(m_pData+sizeof(BYTE)+sizeof(WORD)+UTF8Len*sizeof(char)))=0;
		//}
	}

	static int GetTypeFromData(LPCVOID pData,UINT DataSize);
	
};
