/****************************************************************************/
/*                                                                          */
/*      文件名:    FilterRenderOnTexture.cpp                                */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"
#include ".\filterrenderontexture.h"
#include "Dvdmedia.h"

namespace DShowLib{

#ifndef OUR_GUID_ENTRY
#define OUR_GUID_ENTRY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	const GUID  name \
	= { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
#endif

// 30323449-0000-0010-8000-00AA00389B71  'I420' == MEDIASUBTYPE_I420
OUR_GUID_ENTRY(MEDIASUBTYPE_I420,
	0x30323449, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);



#define YUV2RGB(y,u,v,r,g,b)\
{\
	float R=(y)+1.13983f*(v);\
	float G=(y)-0.39465f*(u)-0.58060f*(v);\
	float B=(y)+2.03211f*(u);\
	(r)=(BYTE)(R>255.0f?255:(R<0?0:(BYTE)R));\
	(g)=(BYTE)(G>255.0f?255:(G<0?0:(BYTE)G));\
	(b)=(BYTE)(B>255.0f?255:(B<0?0:(BYTE)B));\
}


CEasyString GetMediaTypeName(const CLSID& MainType,const CLSID& SubType)
{
	HRESULT   hr = E_FAIL;

	IFilterMapper2 *pMapper = NULL;
	IEnumMoniker *pEnum = NULL;

	hr = CoCreateInstance(CLSID_FilterMapper2, 
		NULL, CLSCTX_INPROC, IID_IFilterMapper2, 
		(void **) &pMapper);

	CEasyString TypeName="未知类型";

	if (FAILED(hr))
	{
		return TypeName;
	}

	

	GUID arrayInTypes[2];
	arrayInTypes[0] = MainType;
	arrayInTypes[1] = SubType;

	hr = pMapper->EnumMatchingFilters(
		&pEnum,
		0,                  // Reserved.
		TRUE,               // Use exact match?
		MERIT_DO_NOT_USE+1, // Minimum merit.
		TRUE,               // At least one input pin?
		1,                  // Number of major type/subtype pairs for input.
		arrayInTypes,       // Array of major type/subtype pairs for input.
		NULL,               // Input medium.
		NULL,               // Input pin category.
		FALSE,              // Must be a renderer?
		TRUE,               // At least one output pin?
		0,                  // Number of major type/subtype pairs for output.
		NULL,               // Array of major type/subtype pairs for output.
		NULL,               // Output medium.
		NULL);              // Output pin category.

	// Enumerate the monikers.
	IMoniker *pMoniker;
	ULONG cFetched;
	while (pEnum->Next(1, &pMoniker, &cFetched) == S_OK)
	{

		


		IPropertyBag *pPropBag = NULL;
		hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, 
			(void **)&pPropBag);

		if (SUCCEEDED(hr))
		{
			// To retrieve the friendly name of the filter, do the following:
			VARIANT varName;
			VariantInit(&varName);
			hr = pPropBag->Read(L"FriendlyName", &varName, 0);
			if (SUCCEEDED(hr))
			{
				// Display the name in your UI somehow.
				TypeName=varName.bstrVal;
			}
			VariantClear(&varName);

			//// To create an instance of the filter, do the following:
			//IBaseFilter *pFilter;
			//hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pFilter);
			//// Now add the filter to the graph. Remember to release pFilter later.

			// Clean up.
			pPropBag->Release();
		}
		pMoniker->Release();
	}

	// Clean up.
	pMapper->Release();
	pEnum->Release();

	return TypeName;

}

LPCTSTR GetVideoFormatName(const GUID& TypeID)
{
	if(IsEqualGUID( TypeID, MEDIASUBTYPE_CLPL))
	{
		return "MEDIASUBTYPE_CLPL";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_YUYV))
	{
		return "MEDIASUBTYPE_YUYV";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_IYUV))
	{
		return "MEDIASUBTYPE_IYUV";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_YVU9))
	{
		return "MEDIASUBTYPE_YVU9";

	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_Y411))
	{
		return "MEDIASUBTYPE_Y411";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_Y41P))
	{
		return "MEDIASUBTYPE_Y41P";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_YUY2))
	{
		return "MEDIASUBTYPE_YUY2";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_YVYU))
	{
		return "MEDIASUBTYPE_YVYU";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_UYVY))
	{
		return "MEDIASUBTYPE_UYVY";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_Y211))
	{
		return "MEDIASUBTYPE_Y211";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_YV12))
	{
		return "MEDIASUBTYPE_YV12";
	}	
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_I420))
	{
		return "MEDIASUBTYPE_I420";
	}	
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_CLJR))
	{
		return "MEDIASUBTYPE_CLJR";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_IF09))
	{
		return "MEDIASUBTYPE_IF09";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_CPLA))
	{
		return "MEDIASUBTYPE_CPLA";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_MJPG))
	{
		return "MEDIASUBTYPE_MJPG";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_TVMJ))
	{
		return "MEDIASUBTYPE_TVMJ";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_WAKE))
	{
		return "MEDIASUBTYPE_WAKE";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_CFCC))
	{
		return "MEDIASUBTYPE_CFCC";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_IJPG))
	{
		return "MEDIASUBTYPE_IJPG";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_Plum))
	{
		return "MEDIASUBTYPE_Plum";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_DVCS))
	{
		return "MEDIASUBTYPE_DVCS";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_DVSD))
	{
		return "MEDIASUBTYPE_DVSD";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_MDVF))
	{
		return "MEDIASUBTYPE_MDVF";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_RGB1))
	{
		return "MEDIASUBTYPE_RGB1";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_RGB4))
	{
		return "MEDIASUBTYPE_RGB4";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_RGB8))
	{
		return "MEDIASUBTYPE_RGB8";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_RGB565))
	{
		return "MEDIASUBTYPE_RGB565";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_RGB555))
	{
		return "MEDIASUBTYPE_RGB555";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_RGB24))
	{
		return "MEDIASUBTYPE_RGB24";
	}
	else if(IsEqualGUID( TypeID, MEDIASUBTYPE_RGB32))
	{
		return "MEDIASUBTYPE_RGB32";
	}
	else
	{
		static char buff[128];
		sprintf_s(buff,128,"未知类型{%08X,%04X,%04X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X}",
			TypeID.Data1,TypeID.Data2,TypeID.Data3,
			TypeID.Data4[0],TypeID.Data4[1],TypeID.Data4[2],TypeID.Data4[3],
			TypeID.Data4[4],TypeID.Data4[5],TypeID.Data4[6],TypeID.Data4[7]);
		return buff;
	}
}

CFilterRenderOnTexture::CFilterRenderOnTexture(CD3DDevice* pD3DDevice,HRESULT *phr,BOOL UseInCube):
	CBaseVideoRenderer(__uuidof(CLSID_FilterRenderOnTexture), 
				   NAME("Texture Renderer"), NULL, phr)
{	
	m_IsUseDynamicTextures=FALSE;
	m_MustTexturePOW2=FALSE;
	m_IsUseInCube=UseInCube;
	m_pTargetTexture=NULL;
	m_fTU=1.0f;
	m_fTV=1.0f;
	m_Width=0;
	m_Height=0;
	m_OrgWidth=0;
	m_OrgHeight=0;	
	m_lVidPitch=0;
	m_SourceFormat=0;
	m_TextureFormat=0;
	m_SupportTextureFormat=0;
	m_pPixelBuffer=NULL;
	m_IsVideoUpdate=false;

	D3DCAPS9 caps=pD3DDevice->GetDeviceCaps();

	if( caps.Caps2 & D3DCAPS2_DYNAMICTEXTURES )
	{
		m_IsUseDynamicTextures = TRUE;
	}

	if( caps.TextureCaps & D3DPTEXTURECAPS_POW2 )
	{
		m_MustTexturePOW2=TRUE;
	}
	if( (caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL)&& (!m_IsUseInCube))
	{
		m_MustTexturePOW2=FALSE;
	}	

	if(pD3DDevice->CheckDeviceFormat(D3DFMT_YUY2,
		m_IsUseDynamicTextures?D3DUSAGE_DYNAMIC:0,D3DRTYPE_TEXTURE))
	{
		m_SupportTextureFormat|=SUPPORT_D3DFMT_YUY2;
	}

	if(pD3DDevice->CheckDeviceFormat(D3DFMT_UYVY,
		m_IsUseDynamicTextures?D3DUSAGE_DYNAMIC:0,D3DRTYPE_TEXTURE))
	{
		m_SupportTextureFormat|=SUPPORT_D3DFMT_UYVY;
	}

	if(pD3DDevice->CheckDeviceFormat(D3DFMT_X8R8G8B8,
		m_IsUseDynamicTextures?D3DUSAGE_DYNAMIC:0,D3DRTYPE_TEXTURE))
	{
		m_SupportTextureFormat|=SUPPORT_D3DFMT_X8R8G8B8;
	}

	if(pD3DDevice->CheckDeviceFormat(D3DFMT_R5G6B5,
		m_IsUseDynamicTextures?D3DUSAGE_DYNAMIC:0,D3DRTYPE_TEXTURE))
	{
		m_SupportTextureFormat|=SUPPORT_D3DFMT_R5G6B5;
	}
	

}

CFilterRenderOnTexture::~CFilterRenderOnTexture(void)
{
	SAFE_DELETE(m_pPixelBuffer);
}

//-----------------------------------------------------------------------------
// CheckMediaType: This method forces the graph to give us an R8G8B8 video
// type, making our copy to texture memory trivial.
//-----------------------------------------------------------------------------
HRESULT CFilterRenderOnTexture::CheckMediaType(const CMediaType *pmt)
{
	HRESULT   hr = E_FAIL;
	VIDEOINFO *pvi=0;

	

	CheckPointer(pmt,E_POINTER);

	//CEasyString MediaTypeName=GetMediaTypeName(*pmt->Type(),*pmt->Subtype());
	//PrintSystemLog(0,"MediaType=%s",
	//	(LPCTSTR)MediaTypeName);


	// Reject the connection if this is not a video type
	if( *pmt->FormatType() != FORMAT_VideoInfo && *pmt->FormatType() != FORMAT_VideoInfo2) 
	{
		return E_INVALIDARG;
	}

	PrintSystemLog(0,"收到媒体类型%s",GetVideoFormatName(*pmt->Subtype()));


	if((m_SupportTextureFormat&SUPPORT_D3DFMT_YUY2)||(m_SupportTextureFormat&SUPPORT_D3DFMT_YUY2))
	{	
		if(IsEqualGUID( *pmt->Subtype(), MEDIASUBTYPE_YUY2)&&
			(m_SupportTextureFormat&SUPPORT_D3DFMT_YUY2))
		{
			m_SourceFormat=MSPF_YUY2;		
			m_TextureFormat=D3DFMT_YUY2;			
			hr = S_OK;
		}
		else if(IsEqualGUID( *pmt->Subtype(), MEDIASUBTYPE_UYVY)&&
			(m_SupportTextureFormat&SUPPORT_D3DFMT_YUY2))
		{
			m_SourceFormat=MSPF_UYVY;			
			m_TextureFormat=D3DFMT_UYVY;			
			hr = S_OK;
		}
	}	
	else if(m_SupportTextureFormat&SUPPORT_D3DFMT_X8R8G8B8)
	{
		if(IsEqualGUID( *pmt->Subtype(), MEDIASUBTYPE_RGB32))
		{
			m_SourceFormat=MSPF_X8R8G8B8;		
			m_TextureFormat=D3DFMT_X8R8G8B8;				
			hr = S_OK;
		}
		else if(IsEqualGUID( *pmt->Subtype(), MEDIASUBTYPE_RGB24))
		{
			m_SourceFormat=MSPF_R8G8B8;		
			m_TextureFormat=D3DFMT_X8R8G8B8;				
			hr = S_OK;
		}
		//else if(IsEqualGUID( *pmt->Subtype(), MEDIASUBTYPE_YUY2))
		//{
		//	m_SourceFormat=MSPF_YUY2;		
		//	m_TextureFormat=D3DFMT_X8R8G8B8;				
		//	hr = S_OK;
		//}
	}
	else if(m_SupportTextureFormat&SUPPORT_D3DFMT_R5G6B5)
	{
		if(IsEqualGUID( *pmt->Subtype(), MEDIASUBTYPE_RGB565))
		{
			m_SourceFormat=MSPF_R5G6B5;		
			m_TextureFormat=D3DFMT_R5G6B5;				
			hr = S_OK;
		}
	}
	return hr;
}

//-----------------------------------------------------------------------------
// SetMediaType: Graph connection has been made. 
//-----------------------------------------------------------------------------
HRESULT CFilterRenderOnTexture::SetMediaType(const CMediaType *pmt)
{
	CAutoLock Lock(m_ObjectLock);

	//HRESULT hr;

	int BitCount=0;

	// Retrive the size of this media type
	if( *pmt->FormatType() == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER *pviBmp;                      // Bitmap info header
		pviBmp = (VIDEOINFOHEADER *)pmt->Format();

		m_OrgWidth  = pviBmp->bmiHeader.biWidth;
		m_OrgHeight = abs(pviBmp->bmiHeader.biHeight);
		BitCount = pviBmp->bmiHeader.biBitCount;
	}
	else if( *pmt->FormatType() == FORMAT_VideoInfo2)
	{
		VIDEOINFOHEADER2 *pviBmp;                      // Bitmap info header
		pviBmp = (VIDEOINFOHEADER2 *)pmt->Format();

		m_OrgWidth  = pviBmp->bmiHeader.biWidth;
		m_OrgHeight = abs(pviBmp->bmiHeader.biHeight);
		BitCount = pviBmp->bmiHeader.biBitCount;
	}	
	else
	{
		return E_FAIL;
	}

	m_lVidPitch  = (m_OrgWidth * (BitCount/8) + 3) & ~(3); // We are forcing RGB24
		

	m_Width = 2;
	m_Height = 2;

	if( m_MustTexturePOW2)
	{
		while( m_Width < m_OrgWidth )
		{
			m_Width = m_Width << 1;
		}
		while( m_Height < m_OrgHeight )
		{
			m_Height = m_Height << 1;
		}
		m_fTU = (float)(m_OrgWidth) / (float)(m_Width);
		m_fTV = (float)(m_OrgHeight) / (float)(m_Height);
	}
	else
	{
		m_Width = m_OrgWidth;
		m_Height = m_OrgHeight;
	}
	SAFE_DELETE(m_pPixelBuffer);

	m_pPixelBuffer=new BYTE[m_lVidPitch*m_OrgHeight];
	ZeroMemory(m_pPixelBuffer,m_lVidPitch*m_OrgHeight);

	return S_OK;
}

bool CFilterRenderOnTexture::SetTargetTexture(LPDIRECT3DTEXTURE9 pTargetTexture)
{
	CAutoLock Lock(m_ObjectLock);

	HRESULT hr;

	m_pTargetTexture=pTargetTexture;	

	if(pTargetTexture==NULL)
		return true;

	D3DSURFACE_DESC ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));

	CSmartPtr<IDirect3DSurface9> pSurf; 

	hr = m_pTargetTexture->GetSurfaceLevel(0, &pSurf);
	CHECK_HRESULT(hr);
	hr = pSurf->GetDesc(&ddsd);
	CHECK_HRESULT(hr);

	// Save format info
	m_TextureFormat = ddsd.Format;

	if (m_TextureFormat != D3DFMT_X8R8G8B8&&		
		m_TextureFormat != D3DFMT_R5G6B5&&
		m_TextureFormat != D3DFMT_UYVY&&
		m_TextureFormat != D3DFMT_YUY2) 
	{
		PrintSystemLog(0xff00ff,"Texture is format we can't handle! Format = 0x%x", m_TextureFormat);
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// DoRenderSample: A sample has been delivered. Copy it to the texture.
//-----------------------------------------------------------------------------
HRESULT CFilterRenderOnTexture::DoRenderSample( IMediaSample * pSample )
{
	CAutoLock Lock(m_ObjectLock);

	BYTE  *pBmpBuffer; 

	CheckPointer(pSample,E_POINTER);
	CheckPointer(m_pTargetTexture,E_UNEXPECTED);

	// Get the video bitmap buffer
	pSample->GetPointer( &pBmpBuffer );

	long Size=pSample->GetSize();

	memcpy(m_pPixelBuffer,pBmpBuffer,m_lVidPitch*m_OrgHeight);

	m_IsVideoUpdate=true;

	return S_OK;
}

void CFilterRenderOnTexture::RenderToTexture()
{
	CAutoLock Lock(m_ObjectLock);	

	if(m_pPixelBuffer==NULL)
		return;

	if(!m_IsVideoUpdate)
		return;

	m_IsVideoUpdate=false;

	// Lock the Texture
	D3DLOCKED_RECT d3dlr;
	if( m_IsUseDynamicTextures )
	{
		if( FAILED(m_pTargetTexture->LockRect(0, &d3dlr, 0, D3DLOCK_DISCARD)))
			return ;
	}
	else
	{
		if (FAILED(m_pTargetTexture->LockRect(0, &d3dlr, 0, 0)))
			return ;
	}
	// Get the texture buffer & pitch
	
	TranslatePixelFormat(m_SourceFormat,m_OrgWidth,m_OrgHeight,m_lVidPitch,m_pPixelBuffer,
		m_TextureFormat,d3dlr.Pitch,static_cast<byte *>(d3dlr.pBits));
	

	// Unlock the Texture
	if (FAILED(m_pTargetTexture->UnlockRect(0)))
		return ;
}

void CFilterRenderOnTexture::TranslatePixelFormat(DWORD SourcePixelFormat,int SourceWidth,int SourceHeight,UINT SourcePitch,BYTE * pSourceBuffer,DWORD TargetPixelFormat,UINT TargetPitch,BYTE * pTargetBuffer)
{
	if(pSourceBuffer==NULL||pTargetBuffer==NULL)
		return;

	if(SourcePixelFormat==MSPF_YUY2&&TargetPixelFormat==D3DFMT_YUY2)
	{
		for(int y = 0; y < SourceHeight; y++ ) 
		{
			memcpy(pTargetBuffer,pSourceBuffer,SourcePitch);
			pSourceBuffer = pSourceBuffer + SourcePitch;
			pTargetBuffer = pTargetBuffer + TargetPitch;
		}
	}
	else if(SourcePixelFormat==MSPF_UYVY&&TargetPixelFormat==D3DFMT_UYVY)
	{
		for(int y = 0; y < SourceHeight; y++ ) 
		{
			memcpy(pTargetBuffer,pSourceBuffer,SourcePitch);
			pSourceBuffer = pSourceBuffer + SourcePitch;
			pTargetBuffer = pTargetBuffer + TargetPitch;
		}
	}
	else if(SourcePixelFormat==MSPF_R5G6B5&&TargetPixelFormat==D3DFMT_R5G6B5)
	{
		for(int y = 0; y < SourceHeight; y++ ) 
		{
			memcpy(pTargetBuffer+y*TargetPitch,pSourceBuffer+(SourceHeight-y-1)*SourcePitch,SourcePitch);
		}
	}
	else if(SourcePixelFormat==MSPF_X8R8G8B8&&TargetPixelFormat==D3DFMT_X8R8G8B8)
	{
		for(int y = 0; y < SourceHeight; y++ ) 
		{
			memcpy(pTargetBuffer+y*TargetPitch,pSourceBuffer+(SourceHeight-y-1)*SourcePitch,SourcePitch);
		}
	}
	else if(SourcePixelFormat==MSPF_R8G8B8&&TargetPixelFormat==D3DFMT_X8R8G8B8)
	{
		// Instead of copying data bytewise, we use DWORD alignment here.
		// We also unroll loop by copying 4 pixels at once.
		//
		// original BYTE array is [b0][g0][r0][b1][g1][r1][b2][g2][r2][b3][g3][r3]
		//
		// aligned DWORD array is     [b1 r0 g0 b0][g2 b2 r1 g1][r3 g3 b3 r2]
		//
		// We want to transform it to [ff r0 g0 b0][ff r1 g1 b1][ff r2 g2 b2][ff r3 b3 g3]
		// below, bitwise operations do exactly this.

		UINT dwordWidth = SourceWidth / 4; // aligned width of the row, in DWORDS
		// (pixel by 3 bytes over sizeof(DWORD))

		for(UINT row = 0; row< (UINT)SourceHeight; row++)
		{
			DWORD * pdwS = ( DWORD*)(pSourceBuffer+SourcePitch*(SourceHeight-row-1));
			DWORD * pdwD = ( DWORD*)(pTargetBuffer+TargetPitch*row);

			for(UINT col = 0; col < dwordWidth; col ++ )
			{
				pdwD[0] =  pdwS[0] | 0xFF000000;
				pdwD[1] = ((pdwS[1]<<8)  | 0xFF000000) | (pdwS[0]>>24);
				pdwD[2] = ((pdwS[2]<<16) | 0xFF000000) | (pdwS[1]>>16);
				pdwD[3] = 0xFF000000 | (pdwS[2]>>8);
				pdwD +=4;
				pdwS +=3;
			}

			// we might have remaining (misaligned) bytes here
			BYTE * pbS = (BYTE*) pdwS;
			for(UINT col = 0; col < (UINT)SourceWidth % 4; col++)
			{
				*pdwD = 0xFF000000     |
					(pbS[2] << 16) |
					(pbS[1] <<  8) |
					(pbS[0]);
				pdwD++;
				pbS += 3;           
			}

			//pBmpBuffer  += m_lVidPitch;
			//pTxtBuffer += lTxtPitch;
		}// for rows
	}
	else if(SourcePixelFormat==MSPF_YUY2&&TargetPixelFormat==D3DFMT_X8R8G8B8)
	{
		for(int y = 0; y < SourceHeight; y++ ) 
		{
			BYTE * pYData=pSourceBuffer;
			BYTE * pUData=pSourceBuffer+1;
			BYTE * pVData=pSourceBuffer+3;
			BYTE * pBData=pTargetBuffer;
			BYTE * pGData=pTargetBuffer+1;
			BYTE * pRData=pTargetBuffer+2;
			BYTE * pXData=pTargetBuffer+3;

			for(int x =0 ; x < SourceWidth; x++ )
			{
				YUV2RGB(*pYData,*pUData,*pVData,*pRData,*pGData,*pBData);

				//*pRData=*pYData;
				//*pGData=*pYData;
				//*pBData=*pYData;

				*pXData=0xff;
				pYData+=2;
				pUData+=4*(x%2);
				pVData+=4*(x%2);
				pBData+=4;
				pGData+=4;
				pRData+=4;
				pXData+=4;
			}
			pSourceBuffer = pSourceBuffer + SourcePitch;
			pTargetBuffer = pTargetBuffer + TargetPitch;
		}
	}
}

}