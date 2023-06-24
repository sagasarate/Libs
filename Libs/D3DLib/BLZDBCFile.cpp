#include "StdAfx.h"


namespace D3DLib{

IMPLEMENT_FILE_CHANNEL_MANAGER(CBLZDBCFile)

CBLZDBCFile::CBLZDBCFile(void)
{
	m_pData=NULL;
	m_DataSize=0;
	m_IsSelfRelease=false;
	m_RecordCount=0;
	m_FieldCount=0;
	m_RecordSize=0;
	m_StringTableSize=0;
	m_pRecordData=NULL;
	m_pStringTable=NULL;


}

CBLZDBCFile::~CBLZDBCFile(void)
{
	Close();
}

bool CBLZDBCFile::Load(LPCTSTR FileName,UINT RecordSize)
{
	IFileAccessor * pFile;

	pFile=CreateFileAccessor();
	if(pFile==NULL)
		return false;
	if(!pFile->Open(FileName,IFileAccessor::modeRead))
	{
		pFile->Release();
		return false;	
	}

	bool Ret=Load(pFile,RecordSize);
	pFile->Release();
	return Ret;
}

bool CBLZDBCFile::Load(IFileAccessor * pFile,UINT RecordSize)
{
	UINT FileSize=(UINT)pFile->GetSize();	
	BYTE * pData=new BYTE[FileSize];
	pFile->Read(pData,FileSize);
	return Load(pData,FileSize,RecordSize,true);
}

bool CBLZDBCFile::Load(BYTE * pData,UINT DataSize,UINT RecordSize,bool IsSelfRelease)
{
	Close();
	m_pData=pData;
	m_DataSize=DataSize;
	m_IsSelfRelease=IsSelfRelease;

	DWORD Tag=*((DWORD *)m_pData);
	if(Tag==BLZ_DBC_HEADER_TAG)
	{
		BLZ_DBC_HEADER * pHeader=(BLZ_DBC_HEADER *)m_pData;

		if(RecordSize)
		{
			if(pHeader->RecordSize!=RecordSize)
				return false;
		}

		m_RecordCount=pHeader->RecordCount;
		m_FieldCount=pHeader->FieldCount;
		m_RecordSize=pHeader->RecordSize;
		m_StringTableSize=pHeader->StringBlockSize;
		m_pRecordData=m_pData+sizeof(BLZ_DBC_HEADER);
		m_pStringTable=(char *)(m_pData+sizeof(BLZ_DBC_HEADER)+pHeader->RecordSize*pHeader->RecordCount);

		return true;
	}
	else if(Tag==BLZ_DB2_HEADER_TAG)
	{
		BLZ_DB2_HEADER * pHeader=(BLZ_DB2_HEADER *)m_pData;

		m_RecordCount=pHeader->RecordCount;
		m_FieldCount=pHeader->FieldCount;
		m_RecordSize=pHeader->RecordSize;
		m_StringTableSize=pHeader->StringBlockSize;
		if(pHeader->LastRowIndex)
		{
			m_pRecordData=m_pData+sizeof(BLZ_DB2_HEADER)+(pHeader->LastRowIndex-pHeader->FirstRowIndex+1)*6;
			m_pStringTable=(char *)(m_pData+sizeof(BLZ_DBC_HEADER)+(pHeader->LastRowIndex-pHeader->FirstRowIndex+1)*6+
				pHeader->RecordSize*pHeader->RecordCount);
		}
		else
		{
			m_pRecordData=m_pData+sizeof(BLZ_DB2_HEADER);
			m_pStringTable=(char *)(m_pData+sizeof(BLZ_DBC_HEADER)+pHeader->RecordSize*pHeader->RecordCount);
		}
		

		return true;
	}
	return false;

	
}

void CBLZDBCFile::Close()
{
	if(m_IsSelfRelease)
	{
		SAFE_DELETE_ARRAY(m_pData);
	}
	m_pData=NULL;
	m_DataSize=0;
	m_IsSelfRelease=false;
	m_RecordCount=0;
	m_FieldCount=0;
	m_RecordSize=0;
	m_StringTableSize=0;
	m_pRecordData=NULL;
	m_pStringTable=NULL;
}

UINT CBLZDBCFile::GetRecordSize()
{
	return m_RecordSize;
}

UINT CBLZDBCFile::GetRecordCount()
{
	return m_RecordCount;
}
UINT CBLZDBCFile::GetFieldCount()
{
	return m_FieldCount;
}
int CBLZDBCFile::GetDataInt(UINT Record,UINT Field)
{
	if(m_pRecordData&&Record<m_RecordCount&&Field<m_FieldCount)
	{
		int * pRecord=(int *)(m_pRecordData+m_RecordSize*Record);
		return pRecord[Field];
	}
	return 0;
}
UINT CBLZDBCFile::GetDataUint(UINT Record,UINT Field)
{
	if(m_pRecordData&&Record<m_RecordCount&&Field<m_FieldCount)
	{
		UINT * pRecord=(UINT *)(m_pRecordData+m_RecordSize*Record);
		return pRecord[Field];
	}
	return 0;
}
float CBLZDBCFile::GetDataFloat(UINT Record,UINT Field)
{
	if(m_pRecordData&&Record<m_RecordCount&&Field<m_FieldCount)
	{
		float * pRecord=(float *)(m_pRecordData+m_RecordSize*Record);
		return pRecord[Field];
	}
	return 0;
}
LPCSTR CBLZDBCFile::GetDataString(UINT Record,UINT Field)
{
	if(m_pRecordData&&Record<m_RecordCount&&Field<m_FieldCount)
	{
		UINT * pRecord=(UINT *)(m_pRecordData+m_RecordSize*Record);
		if(pRecord[Field]<m_StringTableSize)
			return m_pStringTable+pRecord[Field];
	}
	return NULL;
}
CEasyString CBLZDBCFile::GetDataLocalString(UINT Record,UINT Field)
{
	if(m_pRecordData&&Record<m_RecordCount&&Field<m_FieldCount)
	{
		UINT * pRecord=(UINT *)(m_pRecordData+m_RecordSize*Record);

		if(pRecord[Field]<m_StringTableSize)
		{
			char * pString=m_pStringTable+pRecord[Field];

			WCHAR	Buffer[1024];

			int len=MultiByteToWideChar(CP_UTF8,0,pString,strlen(pString),Buffer,1024);
			Buffer[len]=0;
			return Buffer;
		}
	}
	return "";
}

}