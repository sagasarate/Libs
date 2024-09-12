/****************************************************************************/
/*                                                                          */
/*      文件名:    BLZBLSImporter.cpp                                       */
/*      创建日期:  2010年07月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/

#include "StdAfx.h"

namespace D3DLib{

IMPLEMENT_FILE_CHANNEL_MANAGER(CBLZBLSImporter)

CBLZBLSImporter::CBLZBLSImporter(void)
{
	m_pData=NULL;
	m_DataSize=0;
	m_IsSelfRelease=false;
	m_ShaderType=0;
}

CBLZBLSImporter::~CBLZBLSImporter(void)
{
	Close();
}

bool CBLZBLSImporter::Load(LPCTSTR FileName)
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

	bool Ret=Load(pFile);
	pFile->Release();
	return Ret;
}

bool CBLZBLSImporter::Load(IFileAccessor * pFile)
{
	UINT FileSize=(UINT)pFile->GetSize();	
	BYTE * pData=new BYTE[FileSize];
	pFile->Read(pData,FileSize);
	return Load(pData,FileSize,true);
}

bool CBLZBLSImporter::Load(BYTE * pData,UINT DataSize,bool IsSelfRelease)
{
	Close();
	m_pData=pData;
	m_DataSize=DataSize;
	m_IsSelfRelease=IsSelfRelease;

	BLZ_BLS_HEADER * pHeader=(BLZ_BLS_HEADER *)m_pData;
	if(pHeader->Tag==BLZ_BLS_VS_HEADER_TAG)
	{
		m_ShaderType=SHADER_TYPE_VS;
	}
	else if(pHeader->Tag==BLZ_BLS_PS_HEADER_TAG)
	{
		m_ShaderType=SHADER_TYPE_PS;
	}
	else
	{
		return false;
	}

	UINT BlockCount=pHeader->DataBlockCount;

	BYTE * pBlockData=m_pData+sizeof(BLZ_BLS_HEADER);
	while(BlockCount&&(pBlockData-m_pData<m_DataSize))
	{
		BLZ_BLS_DATA_BLOCK * pBlock=(BLZ_BLS_DATA_BLOCK *)pBlockData;

		m_Blocks.Add(pBlock);

		pBlockData+=sizeof(BLZ_BLS_DATA_BLOCK_HEADER)+pBlock->Header.DataSize;
		BlockCount--;
	}

	return true;
}

void CBLZBLSImporter::Close()
{
	if(m_IsSelfRelease)
	{
		SAFE_DELETE_ARRAY(m_pData);
	}
	m_pData=NULL;
	m_DataSize=0;
	m_IsSelfRelease=false;
	m_ShaderType=0;
	m_Blocks.Clear();
}

UINT CBLZBLSImporter::GetDataBlockCount()
{
	return m_Blocks.GetCount();
}
BLZ_BLS_DATA_BLOCK * CBLZBLSImporter::GetDataBlock(UINT Index)
{
	if(Index<m_Blocks.GetCount())
	{
		return m_Blocks[Index];
	}
	return NULL;
}

CEasyString CBLZBLSImporter::GetD3DXDisassembledShader(UINT Index)
{
	CEasyString ShaderText;
	BLZ_BLS_DATA_BLOCK * pBlock=GetDataBlock(Index);
	if(pBlock)
	{
		LPD3DXBUFFER pD3DBuffer;
		if(D3DXDisassembleShader((DWORD *)pBlock->Data,FALSE,NULL,&pD3DBuffer)==D3D_OK)
		{
			ShaderText.SetString((LPCTSTR)pD3DBuffer->GetBufferPointer(),pD3DBuffer->GetBufferSize());
			ShaderText.TrimBuffer();
			pD3DBuffer->Release();
		}
	}
	return ShaderText;
}

}