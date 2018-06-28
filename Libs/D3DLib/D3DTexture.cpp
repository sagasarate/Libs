/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DTexture.cpp                                           */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"


namespace D3DLib{

#define BLP_FILE_TAG				((DWORD)'2PLB')
#define MAX_BLP_MIP_MAP_LEVELS		16
#define DXT_BLOCK_SIZE				4
#define DXT1_BLOCK_BYTES			8
#define DXT3_BLOCK_BYTES			16
#define DXT5_BLOCK_BYTES			16

struct BLP_FILE_HEADER
{
	DWORD	Tag;
	UINT	Version;
	BYTE	CompressType;
	BYTE	AplhaDepth;
	BYTE	AlphaEncoding;
	BYTE	HasMips;
	UINT	Width;
	UINT	Height;
	UINT	MipMapOffsets[MAX_BLP_MIP_MAP_LEVELS];
	UINT	MipMapSizes[MAX_BLP_MIP_MAP_LEVELS];
};

enum BLP_COMPRESS_TYPE
{
	BLP_CT_UNCOMPRESS=1,
	BLP_CT_DXTC=2,
};

struct BLP_PALETTE
{
	BYTE	Blue;	
	BYTE	Green;
	BYTE	Red;
	BYTE	Alpha;
};

IMPLEMENT_FILE_PATH_MANAGER(CD3DTexture)
IMPLEMENT_FILE_CHANNEL_MANAGER(CD3DTexture)

IMPLEMENT_CLASS_INFO(CD3DTexture,CNameObject);

bool CD3DTexture::m_IsSaveOrgInfo=false;

void CD3DTexture::EnableSaveOrgInfo(bool Enable)
{
	m_IsSaveOrgInfo=Enable;
}

CD3DTexture::CD3DTexture()
{
	m_pManager=NULL;
	m_pTexture=NULL;
	m_pTextureSaveData=NULL;
	ZeroMemory(&m_TextureInfo,sizeof(m_TextureInfo));	
	ZeroMemory(&m_TextureOrgInfo,sizeof(m_TextureOrgInfo));	
}

CD3DTexture::CD3DTexture(CD3DTextureManager * pManager)
{
	m_pManager=pManager;
	m_pTexture=NULL;
	m_pTextureSaveData=NULL;
	ZeroMemory(&m_TextureInfo,sizeof(m_TextureInfo));	
	ZeroMemory(&m_TextureOrgInfo,sizeof(m_TextureOrgInfo));	
}

CD3DTexture::~CD3DTexture()
{
	if(m_pManager&&GetID())
	{
		m_pManager->DeleteTexture(GetID());
	}
	Destory();		
}

void CD3DTexture::Destory()
{
	m_TextureOrgInfo.DataSize=0;
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pTextureSaveData);	
	CNameObject::Destory();
}

bool CD3DTexture::Reset()
{
	if(m_TextureOrgInfo.WantRestore)
	{
		SAFE_RELEASE(m_pTexture);
	}

	return true;
}

bool CD3DTexture::Restore()
{
	if(m_TextureOrgInfo.WantRestore)
	{
		return CreateTexture(m_TextureOrgInfo.Width,m_TextureOrgInfo.Height,
			m_TextureOrgInfo.PixelFormat,m_TextureOrgInfo.Usage,
			m_TextureOrgInfo.Pool,m_TextureOrgInfo.MipLevels);
	}
	return true;
}

bool CD3DTexture::CreateTexture(int Width,int Height,DWORD PixelFormat,DWORD Usage,DWORD Pool,UINT MipLevels)
{
	HRESULT hr;

	CD3DTexture::Destory();

	hr=D3DXCreateTexture(m_pManager->GetDevice()->GetD3DDevice(),
		Width,
		Height,
		MipLevels,
		Usage,
		(D3DFORMAT)PixelFormat,
		(D3DPOOL)Pool,
		&m_pTexture);

	if(FAILED(hr))

	{
		return false;
	}
	m_TextureInfo.Width=Width;
	m_TextureInfo.Height=Height;
	m_TextureInfo.Format=(D3DFORMAT)PixelFormat;
	m_TextureInfo.MipLevels=MipLevels;
	m_pTexture->GetLevelDesc(0,&m_TextureSurfaceInfo);	

	if(m_IsSaveOrgInfo||Pool==D3DPOOL_DEFAULT)
	{	
		m_TextureOrgInfo.Width=Width;
		m_TextureOrgInfo.Height=Height;
		m_TextureOrgInfo.PixelFormat=PixelFormat;
		m_TextureOrgInfo.Usage=Usage;
		m_TextureOrgInfo.Pool=Pool;
		m_TextureOrgInfo.MipLevels=MipLevels;
		m_TextureOrgInfo.UseFilter=false;
		m_TextureOrgInfo.IsManaged=(Pool==D3DPOOL_MANAGED);
		m_TextureOrgInfo.WantRestore=(Pool==D3DPOOL_DEFAULT);
		m_TextureOrgInfo.KeyColor=0;
	}


	return true;
}

bool CD3DTexture::LoadTexture(LPCTSTR TextureFileName,UINT MipLevels,bool UseFilter,bool IsManaged,D3DCOLOR KeyColor)
{
	

	

	CEasyString FileName=FindFileOne(TextureFileName);


	//PrintD3DLog("装载纹理<%s>...\r\n",FileName);


	IFileAccessor * pFile;

	pFile=CreateFileAccessor();
	if(pFile==NULL)
		return false;
	bool Ret=false;
	if(pFile->Open(FileName,IFileAccessor::modeRead))
	{
		Ret=LoadTexture(pFile,MipLevels,UseFilter,IsManaged,KeyColor);
		if(!Ret)
		{
			PrintD3DLog(_T("装载纹理<%s>失败\r\n"),TextureFileName);	
		}
	}
	else
	{
		PrintD3DLog(_T("打开纹理<%s>失败\r\n"),TextureFileName);			
	}

	pFile->Release();
	return Ret;
}

bool CD3DTexture::LoadTexture(IFileAccessor * pFileAccessor,UINT MipLevels,bool UseFilter,bool IsManaged,D3DCOLOR KeyColor)
{
	int FileSize=(int)pFileAccessor->GetSize();	
	BYTE * pData=new BYTE[FileSize];
	pFileAccessor->Read(pData,FileSize);	

	bool ret=LoadTextureFromMemory(pData,FileSize,MipLevels,UseFilter,IsManaged,KeyColor);
	delete[] pData;
	return ret;	
}


bool CD3DTexture::LoadTextureFromMemory(LPVOID pData,int DataSize,UINT MipLevels,bool UseFilter,bool IsManaged,D3DCOLOR KeyColor)
{
	HRESULT hr;

	CD3DTexture::Destory();

	if(*((DWORD *)pData)==BLP_FILE_TAG)
	{
		if(!LoadBLPTexture(pData,DataSize,0,IsManaged))
		{
			return false;
		}
	}
	else
	{
		hr=D3DXCreateTextureFromFileInMemoryEx(m_pManager->GetDevice()->GetD3DDevice(),
			pData,DataSize,
			D3DX_DEFAULT,D3DX_DEFAULT,
			MipLevels,
			0,
			D3DFMT_UNKNOWN,
			IsManaged?D3DPOOL_MANAGED:D3DPOOL_DEFAULT,
			UseFilter?D3DX_DEFAULT:D3DX_FILTER_NONE,
			UseFilter?D3DX_DEFAULT:D3DX_FILTER_NONE,
			KeyColor,
			&m_TextureInfo,NULL,&m_pTexture);	

		if(FAILED(hr))
		{
			return false;
		}
		m_pTexture->GetLevelDesc(0,&m_TextureSurfaceInfo);	

		if(m_IsSaveOrgInfo)
		{	

			m_TextureOrgInfo.DataSize=DataSize;

			m_TextureOrgInfo.MipLevels=MipLevels;
			m_TextureOrgInfo.UseFilter=UseFilter;
			m_TextureOrgInfo.IsManaged=IsManaged;
			m_TextureOrgInfo.WantRestore=!IsManaged;
			m_TextureOrgInfo.KeyColor=0;
		}
		
	}
	


	


	return true;
}

void CD3DTexture::Update(FLOAT Time)
{
	if(!CD3DDevice::IsUseMultiThreadRender())
	{
		OnPrepareRenderData();
	}
}

void CD3DTexture::OnPrepareRenderData()
{

}


bool CD3DTexture::LockBits(UINT Level,D3DLOCKED_RECT* pLockedRect,const RECT* pRect,DWORD Flags)
{
	if(m_pTexture)
	{
		if(m_pTexture->LockRect(Level,pLockedRect,pRect,Flags)==D3D_OK)
		{
			return true;
		}
	}
	return false;
}
bool CD3DTexture::UnlockBits(UINT Level)
{
	if(m_pTexture)
	{
		if(m_pTexture->UnlockRect(Level)==D3D_OK)
		{
			return true;
		}
	}
	return false;
}





bool CD3DTexture::ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CNameObject::ToSmartStruct(Packet,pResourceManager,Param))
		return false;	
	if(m_pTexture)
	{
		if(m_pTextureSaveData==NULL)
		{
			if(D3DXSaveTextureToFileInMemory(&m_pTextureSaveData,D3DXIFF_PNG,m_pTexture,NULL)!=D3D_OK)
				return false;
		}
		bool Ret=Packet.AddMember(SST_D3DTEX_TESTURE_DATA,(char *)m_pTextureSaveData->GetBufferPointer(),m_pTextureSaveData->GetBufferSize());
		SAFE_RELEASE(m_pTextureSaveData);
		if(!Ret)
			return false;

		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DTEX_MIP_LEVEL,m_TextureOrgInfo.MipLevels));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DTEX_USE_FILTER,(BYTE)m_TextureOrgInfo.UseFilter));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DTEX_IS_MANAGED,(BYTE)m_TextureOrgInfo.IsManaged));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DTEX_KEY_COLOR,(UINT)m_TextureOrgInfo.KeyColor));

	}

	return true;
}
bool CD3DTexture::FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CNameObject::FromSmartStruct(Packet,pResourceManager,Param))
		return false;

	m_TextureOrgInfo.MipLevels=Packet.GetMember(SST_D3DTEX_MIP_LEVEL);
	m_TextureOrgInfo.UseFilter=(BYTE)Packet.GetMember(SST_D3DTEX_USE_FILTER)!=0;
	m_TextureOrgInfo.IsManaged=(BYTE)Packet.GetMember(SST_D3DTEX_IS_MANAGED)!=0;
	m_TextureOrgInfo.WantRestore=!m_TextureOrgInfo.IsManaged;
	m_TextureOrgInfo.KeyColor = (UINT)Packet.GetMember(SST_D3DTEX_KEY_COLOR);

	CSmartValue TextureData=Packet.GetMember(SST_D3DTEX_TESTURE_DATA);

	if(!LoadTextureFromMemory((LPVOID)(LPCTSTR)TextureData,TextureData.GetLength(),
		m_TextureOrgInfo.MipLevels,m_TextureOrgInfo.UseFilter,m_TextureOrgInfo.IsManaged,m_TextureOrgInfo.KeyColor))
	{
		return false;
	}
	

	if(m_pManager)
		m_pManager->AddTexture(this,GetName());

	return true;
}

UINT CD3DTexture::GetSmartStructSize(UINT Param)
{
	UINT Size=CNameObject::GetSmartStructSize(Param);

	if(m_pTexture)
	{		
		SAFE_RELEASE(m_pTextureSaveData);
		
		if(D3DXSaveTextureToFileInMemory(&m_pTextureSaveData,D3DXIFF_PNG,m_pTexture,NULL)!=D3D_OK)
			return false;
		Size += CSmartStruct::GetStringMemberSizeA(m_pTextureSaveData->GetBufferSize());
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_TextureOrgInfo.MipLevels));
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_TextureOrgInfo.UseFilter));
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_TextureOrgInfo.IsManaged));
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_TextureOrgInfo.KeyColor));
	}
	
	return Size;
}


bool CD3DTexture::LoadBLPTexture(LPVOID pData,int DataSize,DWORD Usage,bool IsManaged)
{
	static BYTE s_AlphaMasks[9]={0,1,3,7,0xF,0x1F,0x3F,0x7F,0xFF};

	int MipMapLevels=0;
	
	if(DataSize<sizeof(BLP_FILE_HEADER))
	{
		return false;
	}
	BLP_FILE_HEADER * pBLPHeader=(BLP_FILE_HEADER *)pData;	
	if(pBLPHeader->CompressType==BLP_CT_UNCOMPRESS)
	{
		BLP_PALETTE * pPalettes=(BLP_PALETTE *)((BYTE *)pData+sizeof(BLP_FILE_HEADER));		
		while(pBLPHeader->MipMapOffsets[MipMapLevels])
		{
			MipMapLevels++;
		}
		if(MipMapLevels==0)
		{
			return false;
		}
		if(!CreateTexture(pBLPHeader->Width,pBLPHeader->Height,
			pBLPHeader->AplhaDepth==0?D3DFMT_X8B8G8R8:D3DFMT_A8B8G8R8,
			Usage,
			IsManaged?D3DPOOL_MANAGED:D3DPOOL_DEFAULT,
			MipMapLevels))
		{
			return false;
		}	
		for(int i=0;i<MipMapLevels;i++)
		{
			UINT Width=(pBLPHeader->Width)>>i;
			UINT Height=(pBLPHeader->Height)>>i;
			if(Width==0)
				Width=1;
			if(Height==0)
				Height=1;
			UINT AlphaBitsSize=(Width*Height*pBLPHeader->AplhaDepth)/8;
			if(AlphaBitsSize==0&&pBLPHeader->AplhaDepth)
				AlphaBitsSize=1;
			UINT BitsSize=Width*Height+AlphaBitsSize;
			if(BitsSize!=pBLPHeader->MipMapSizes[i])
			{
				return false;
			}
			BYTE * pSrcBits=((BYTE *)pData)+pBLPHeader->MipMapOffsets[i];
			BYTE * pSrcAlphaBits;
			if(pBLPHeader->AplhaDepth==0)
				pSrcAlphaBits=NULL;
			else
				pSrcAlphaBits=((BYTE *)pData)+pBLPHeader->MipMapOffsets[i]+Width*Height;
			BYTE AlphaMask=s_AlphaMasks[pBLPHeader->AplhaDepth];

			D3DLOCKED_RECT LockRect;
			if(m_pTexture->LockRect(i,&LockRect,NULL,0)!=D3D_OK)
			{
				return false;
			}
			BYTE * pDestBits=(BYTE *)LockRect.pBits;
			for(UINT h=0;h<Height;h++)
			{
				D3D_A8B8G8R8_PIXEL * pDestLine=(D3D_A8B8G8R8_PIXEL *)pDestBits;
				BYTE * pSrcLine=pSrcBits;
				BYTE * pSrcAlphaLine;
				if(pBLPHeader->AplhaDepth==0)
					pSrcAlphaLine=NULL;
				else
					pSrcAlphaLine=pSrcAlphaBits;
				for(UINT w=0;w<Width;w++)
				{
					pDestLine[w].Red=pPalettes[pSrcBits[w]].Red;
					pDestLine[w].Green=pPalettes[pSrcBits[w]].Green;
					pDestLine[w].Blue=pPalettes[pSrcBits[w]].Blue;
					if(pBLPHeader->AplhaDepth)
					{
						int ByteIndex=(w*pBLPHeader->AplhaDepth)/8;
						int BitIndex=(w*pBLPHeader->AplhaDepth)%8;
						pDestLine[w].Alpha=((pSrcAlphaLine[ByteIndex]>>BitIndex)&(AlphaMask))<<(8-pBLPHeader->AplhaDepth);
					}

				}
				pDestBits+=LockRect.Pitch;
				pSrcBits+=Width;
				if(pBLPHeader->AplhaDepth)
				{
					pSrcAlphaBits+=(Width*pBLPHeader->AplhaDepth)/8;
				}

			}
			m_pTexture->UnlockRect(i);

		}

	}
	else if(pBLPHeader->CompressType==BLP_CT_DXTC)
	{
		DWORD PixelFormat;
		UINT DXTBlockSize=DXT_BLOCK_SIZE;
		UINT DXTBlockBytes;
		switch(pBLPHeader->AlphaEncoding)
		{
		case 0:
			PixelFormat=D3DFMT_DXT1;			
			DXTBlockBytes=DXT1_BLOCK_BYTES;
			break;
		case 1:
			PixelFormat=D3DFMT_DXT3;
			DXTBlockBytes=DXT3_BLOCK_BYTES;
			break;
		case 7:
			PixelFormat=D3DFMT_DXT5;
			DXTBlockBytes=DXT5_BLOCK_BYTES;
			break;
		default:
			PrintD3DLog(_T("CD3DTexture::LoadBLPTexture:无法识别的压缩格式"));
			return false;
		}		
		
		while(pBLPHeader->MipMapOffsets[MipMapLevels])
		{
			MipMapLevels++;
		}
		if(MipMapLevels==0)
		{
			return false;
		}
		
		if(!CreateTexture(pBLPHeader->Width,pBLPHeader->Height,
			PixelFormat,
			Usage,
			IsManaged?D3DPOOL_MANAGED:D3DPOOL_DEFAULT,
			MipMapLevels))
		{
			return false;
		}
		for(int i=0;i<MipMapLevels;i++)
		{
			//D3DSURFACE_DESC MipDesc;
			//
			//m_pTexture->GetLevelDesc(i,&MipDesc);

			//UINT Width=(pBLPHeader->Width)>>i;
			//UINT Height=(pBLPHeader->Height)>>i;
			//if(Width<DXTBlockSize)
			//	Width=DXTBlockSize;
			//if(Height<DXTBlockSize)
			//	Height=DXTBlockSize;
			//UINT Blocks=(Width/DXTBlockSize)*(Height/DXTBlockSize);
			//if(Blocks==0)
			//	Blocks=1;
			//UINT BitsSize=Blocks*DXTBlockBytes;
			//UINT SrcPitch=(Width/DXTBlockSize)*DXTBlockBytes;
			//if(SrcPitch<DXTBlockBytes)
			//	SrcPitch=DXTBlockBytes;
			//
			//if(BitsSize!=pBLPHeader->MipMapSizes[i])
			//{
			//	return false;
			//}
			BYTE * pSrcBits=((BYTE *)pData)+pBLPHeader->MipMapOffsets[i];
			

			D3DLOCKED_RECT LockRect;
			if(m_pTexture->LockRect(i,&LockRect,NULL,0)!=D3D_OK)
			{
				return false;
			}
			BYTE * pDestBits=(BYTE *)LockRect.pBits;

			memcpy(pDestBits,pSrcBits,pBLPHeader->MipMapSizes[i]);
			//for(UINT h=0;h<Height;h+=DXTBlockSize)
			//{
			//	BYTE * pDestLine=pDestBits;
			//	BYTE * pSrcLine=pSrcBits;
			//	
			//	memcpy(pDestLine,pSrcLine,SrcPitch);
			//	pDestBits+=LockRect.Pitch;
			//	pSrcBits+=SrcPitch;
			//}
			m_pTexture->UnlockRect(i);

		}
	}
	else
	{
		return false;
	}

	m_TextureOrgInfo.MipLevels=0;
	m_TextureOrgInfo.UseFilter=true;
	m_TextureOrgInfo.IsManaged=IsManaged;
	m_TextureOrgInfo.WantRestore=!IsManaged;
	m_TextureOrgInfo.KeyColor=0;

	
	return true;
}



}