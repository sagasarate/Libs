#include "StdAfx.h"


namespace D3DLib{

IMPLEMENT_FILE_CHANNEL_MANAGER(CBLZWDTFile)

CBLZWDTFile::CBLZWDTFile(void)
{
	m_IsBigAlphaMask=false;
}

CBLZWDTFile::~CBLZWDTFile(void)
{
	
}

bool CBLZWDTFile::Load(LPCTSTR FileName)
{
	PrintD3DDebugLog(_T("Load %s"),FileName);
	IFileAccessor * pFile;

	pFile=CreateFileAccessor();
	if(pFile==NULL)
		return false;
	if(!pFile->Open(FileName,IFileAccessor::modeRead))
	{
		pFile->Release();
		return false;	
	}

	bool Ret=Load(pFile);
	pFile->Release();
	return Ret;
}

bool CBLZWDTFile::Load(IFileAccessor * pFile)
{
	CBLZChunkFile WDTFile;

	if(!WDTFile.Load(pFile))
	{
		return false;
	}

	BLZ_CHUNK_MPHD * pMPHD=(BLZ_CHUNK_MPHD *)WDTFile.GetFirstChunk(CHUNK_ID_WDT_MPHD);
	BLZ_CHUNK_MAIN * pMAIN=(BLZ_CHUNK_MAIN *)WDTFile.GetFirstChunk(CHUNK_ID_WDT_MAIN);
	BLZ_CHUNK_MWMO * pMWMO=(BLZ_CHUNK_MWMO *)WDTFile.GetFirstChunk(CHUNK_ID_WDT_MWMO);
	BLZ_CHUNK_MODF * pMODF=(BLZ_CHUNK_MODF *)WDTFile.GetFirstChunk(CHUNK_ID_WDT_MODF);
	m_IsBigAlphaMask=(pMPHD->Flags&WDT_MPHD_FLAG_BIG_ALPHA_MASK)!=0;
	if(pMAIN)
	{
		m_MapAreaInfo.Resize(64*64);
		memcpy(m_MapAreaInfo.GetBuffer(),pMAIN->MapAreaInfo,sizeof(pMAIN->MapAreaInfo));
	}
	if(pMODF)
	{		
		UINT WMOCount=pMODF->ChunkSize/sizeof(ADT_WMO_OBJECT_INFO);		
		m_MapWMOInfo.Resize(WMOCount);
		for(UINT i=0;i<WMOCount;i++)
		{
			m_MapWMOInfo[i].ID=pMODF->WMOObjects[i].ID;
			m_MapWMOInfo[i].Name=pMWMO->WMOFileNames+pMODF->WMOObjects[i].NameSet;
			m_MapWMOInfo[i].Position.x=pMODF->WMOObjects[i].Position.x;
			m_MapWMOInfo[i].Position.y=pMODF->WMOObjects[i].Position.y;
			m_MapWMOInfo[i].Position.z=-pMODF->WMOObjects[i].Position.z;		

			m_MapWMOInfo[i].Orientation=CD3DQuaternion::FromRotationYawPitchRoll(
				-pMODF->WMOObjects[i].Orientation.y*PI/180.0f,
				-pMODF->WMOObjects[i].Orientation.x*PI/180.0f,
				pMODF->WMOObjects[i].Orientation.z*PI/180.0f);
			m_MapWMOInfo[i].Orientation.Normalize();
			m_MapWMOInfo[i].DoodadSet=pMODF->WMOObjects[i].DoodadSet;
		}
	}

	return true;
}

}