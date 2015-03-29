/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DVideoTexture.cpp                                      */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"
#include ".\d3dvideotexture.h"

#include "VMRAllocator.h"
#include "VMRImageCompositor.h"
#include "FilterRenderOnTexture.h"

namespace DShowLib{

IMPLEMENT_CLASS_INFO(CD3DVideoTexture,CD3DTexture);




CD3DVideoTexture::CD3DVideoTexture(void):CD3DTexture()
{
	m_pFilterGraph=NULL;
	m_pGraphBuilder=NULL;
	m_pMediaControl=NULL;
	m_pMediaSeeking=NULL;
	m_pMediaEvent=NULL;	
	m_pDirectVobSub=NULL;
	m_pFilterRenderOnTexture=NULL;
	m_TimeUnit=VTU_ORIGINAL;
	m_ForceLoadVobSub=false;

}

CD3DVideoTexture::CD3DVideoTexture(CD3DTextureManager * pManager):CD3DTexture(pManager)
{
	m_pFilterGraph=NULL;
	m_pGraphBuilder=NULL;
	m_pMediaControl=NULL;
	m_pMediaSeeking=NULL;
	m_pMediaEvent=NULL;		
	m_pFilterRenderOnTexture=NULL;
	m_TimeUnit=VTU_ORIGINAL;
}

CD3DVideoTexture::~CD3DVideoTexture(void)
{
	Destory();
}


void CD3DVideoTexture::Destory()
{
	ULONG Ref=0;
	
	
	SAFE_RELEASE(m_pMediaControl);
	SAFE_RELEASE(m_pMediaSeeking);
	SAFE_RELEASE(m_pMediaEvent);
	SAFE_RELEASE(m_pDirectVobSub);	
	SAFE_RELEASE(m_pFilterRenderOnTexture);
	SAFE_RELEASE(m_pTexture);

	SAFE_RELEASE(m_pGraphBuilder);

	SAFE_RELEASE(m_pFilterGraph);
}

bool CD3DVideoTexture::Reset()
{
	m_pFilterRenderOnTexture->SetTargetTexture(NULL);
	bool Ret=CD3DTexture::Reset();	
	return Ret;
}

bool CD3DVideoTexture::Restore()
{
	bool Ret=CD3DTexture::Restore();
	m_pFilterRenderOnTexture->SetTargetTexture(m_pTexture);
	return Ret;

}

LPDIRECT3DTEXTURE9  CD3DVideoTexture::GetD3DTexture()
{
	if(m_pFilterRenderOnTexture)
		m_pFilterRenderOnTexture->RenderToTexture();
	return m_pTexture;
}

bool CD3DVideoTexture::Create(LPCTSTR FileName)
{
	USES_CONVERSION;

	HRESULT							hr = S_OK;

	Destory();	

	SetName(FileName);

	strncpy_0(m_VideoFile,MAX_PATH,FileName,MAX_PATH);

	hr=CoCreateInstance(CLSID_FilterGraph,NULL, CLSCTX_INPROC,IID_IFilterGraph, 
		(LPVOID *)&m_pFilterGraph);
	CHECK_HRESULT(hr);	

	if(m_ForceLoadVobSub)
	{
		CoCreateInstance(CLSID_DirectVobSub,NULL, CLSCTX_INPROC,__uuidof(IBaseFilter), 
			(LPVOID *)&m_pDirectVobSub);
	}

	hr = S_OK;

	m_pFilterRenderOnTexture=new CFilterRenderOnTexture(m_pManager->GetDevice(),&hr);
	CHECK_HRESULT(hr);

	m_pFilterRenderOnTexture->AddRef();

	hr=m_pFilterGraph->AddFilter(m_pFilterRenderOnTexture, L"FilterRenderOnTexture");
	CHECK_HRESULT(hr);
	
	if(m_pDirectVobSub)
	{
		hr=m_pFilterGraph->AddFilter(m_pDirectVobSub, L"DirectVobSub");
		CHECK_HRESULT(hr);
	}

	hr=m_pFilterGraph->QueryInterface(IID_IGraphBuilder, (LPVOID *)&m_pGraphBuilder);
	CHECK_HRESULT(hr);

	hr=m_pGraphBuilder->RenderFile(T2W(FileName),NULL);
	CHECK_HRESULT(hr);

	

	int Width,Height;
	DWORD TextureFormat=0;
	DWORD Usage=0;
	m_pFilterRenderOnTexture->GetSize(Width,Height);
	TextureFormat=m_pFilterRenderOnTexture->GetTextureFormat();
	if(m_pFilterRenderOnTexture->IsUseDynamicTextures())
		Usage=D3DUSAGE_DYNAMIC;

	if(m_pFilterRenderOnTexture->GetTextureFormat()==0)
		return false;

	if(!CreateTexture((int)Width,(int)Height,TextureFormat,Usage,D3DPOOL_DEFAULT))
	{
		return false;
	}
	
	if(!m_pFilterRenderOnTexture->SetTargetTexture(m_pTexture))
		return false;

	hr=m_pFilterGraph->QueryInterface(IID_IMediaControl, (LPVOID *)&m_pMediaControl);
	CHECK_HRESULT(hr);

	hr=m_pGraphBuilder->QueryInterface(IID_IMediaSeeking, (LPVOID *)&m_pMediaSeeking);
	CHECK_HRESULT(hr);

	hr=m_pGraphBuilder->QueryInterface(IID_IMediaEvent, (LPVOID *)&m_pMediaEvent);
	CHECK_HRESULT(hr);

	m_pMediaSeeking->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);	

	IDirectVobSub * pDirectVobSub=NULL;

	if(SUCCEEDED(FindFilter(m_pFilterGraph,__uuidof(IDirectVobSub),(void **)&pDirectVobSub)))
	{
		pDirectVobSub->put_HideSubtitles(true);
		pDirectVobSub->put_HideSubtitles(false);
		int LanguageCount=0;
		pDirectVobSub->get_LanguageCount(&LanguageCount);
		PrintSystemLog(0,"LanguageCount=%d",
			LanguageCount);
		for(int i=0;i<LanguageCount;i++)
		{
			LPWSTR pLanguageName=NULL;
			pDirectVobSub->get_LanguageName(i,&pLanguageName);
			CEasyString LanguageName=pLanguageName;
			PrintSystemLog(0,"Language%d=%s",
				i,(LPCTSTR)LanguageName);
			CoTaskMemFree(pLanguageName);
		}
		pDirectVobSub->Release();
	}

	IAMStreamSelect * pAMStreamSelect;

	if(SUCCEEDED(FindFilter(m_pFilterGraph,__uuidof(pAMStreamSelect),(void **)&pAMStreamSelect)))
	{
		DWORD StreamCount=0;
		pAMStreamSelect->Count(&StreamCount);			
		DWORD Flag=0;
		LCID lcID=0;
		DWORD Group=0;
		LPWSTR pName;
		for(long i=0;i<StreamCount;i++)
		{
			pAMStreamSelect->Info(i,NULL,&Flag,&lcID,&Group,&pName,NULL,NULL);
			CEasyString StreamName=pName;
			PrintSystemLog(0,"        Stream:(Flag=%u,LCID=%u,Group=%u)%s",Flag,lcID,Group,(LPCTSTR)StreamName);
			CoTaskMemFree(pName);
		}

		pAMStreamSelect->Release();
	}

	EnumFilters(m_pFilterGraph);
	
	
	return true;
}

FLOAT CD3DVideoTexture::GetUS()
{
	if(m_pFilterRenderOnTexture)
		return m_pFilterRenderOnTexture->GetUS();
	else
		return 1.0f;
}
FLOAT CD3DVideoTexture::GetVS()
{
	if(m_pFilterRenderOnTexture)
		return m_pFilterRenderOnTexture->GetVS();
	else
		return 1.0f;
}

void CD3DVideoTexture::GetVideoSize(int& Width,int& Height)
{
	if(m_pFilterRenderOnTexture)
		m_pFilterRenderOnTexture->GetOrgSize(Width,Height);
}

LONGLONG CD3DVideoTexture::GetTimeLength()
{
	LONGLONG Len=0;
	if(m_pMediaSeeking)
		m_pMediaSeeking->GetDuration(&Len);
	Len=Len/m_TimeUnit;
	return Len;
}

LONGLONG CD3DVideoTexture::GetMaxPos()
{
	LONGLONG Pos=0;
	if(m_pMediaSeeking)
		m_pMediaSeeking->GetStopPosition(&Pos);
	Pos=Pos/m_TimeUnit;
	return Pos;
}

LONGLONG CD3DVideoTexture::GetCurPos()
{
	LONGLONG Pos=0;
	if(m_pMediaSeeking)
		m_pMediaSeeking->GetCurrentPosition(&Pos);
	Pos=Pos/m_TimeUnit;
	return Pos;
}

void CD3DVideoTexture::SetCurPos(LONGLONG Pos)
{		
	if(m_pMediaSeeking)
	{
		Pos=Pos*m_TimeUnit;				
		
		m_pMediaSeeking->SetPositions(&Pos,AM_SEEKING_AbsolutePositioning,NULL,AM_SEEKING_NoPositioning);				
		
	}
	
}

HRESULT CD3DVideoTexture::GetPlayEvent(long *lEventCode,LONG_PTR *lParam1,LONG_PTR *lParam2,long msTimeout)
{
	if(m_pMediaEvent)
		return m_pMediaEvent->GetEvent(lEventCode,lParam1,lParam2,msTimeout);
	return E_ABORT;
}

BOOL CD3DVideoTexture::Run()
{
	if(m_pMediaControl)
		return SUCCEEDED(m_pMediaControl->Run());
	return FALSE;
}
BOOL CD3DVideoTexture::Stop(BOOL IsFlush)
{
	
	if(m_pMediaControl)
	{
		SetCurPos(0);
		if(IsFlush)
		{			
			Pause();		
			GetPlayStatus(INFINITE);
			Run();
			GetPlayStatus(INFINITE);
		}

		if(SUCCEEDED(m_pMediaControl->Stop()))
		{			
			return TRUE;
		}
	}
	return FALSE;
}
BOOL CD3DVideoTexture::Pause()
{
	if(m_pMediaControl)
	{
		if(SUCCEEDED(m_pMediaControl->Pause()))
		{
			return TRUE;
		}
	}
	return FALSE;
}

int CD3DVideoTexture::GetPlayStatus(int WaitTime)
{
	int Status=VPS_NONE;
	if(m_pMediaControl)
		m_pMediaControl->GetState(WaitTime,(OAFilterState *)&Status);
	return Status;
}


HRESULT CD3DVideoTexture::FindFilter(IFilterGraph *pGraph,REFIID riid,__RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject) 
{
	IEnumFilters *pEnum = NULL;
	IBaseFilter *pFilter;
	ULONG cFetched;
	HRESULT hr = E_FAIL;

	if(SUCCEEDED(pGraph->EnumFilters(&pEnum)))
	{	
		while(pEnum->Next(1, &pFilter, &cFetched) == S_OK && hr != S_OK)
		{			
			if(SUCCEEDED(pFilter->QueryInterface(riid,ppvObject)))
			{
				hr = S_OK;
			}	
			pFilter->Release();
		}

		pEnum->Release();
	}
	return hr;
}

}


HRESULT CD3DVideoTexture::EnumFilters (IFilterGraph *pGraph) 
{
	IEnumFilters *pEnum = NULL;
	IBaseFilter *pFilter;
	ULONG cFetched;

	HRESULT hr = pGraph->EnumFilters(&pEnum);
	if (FAILED(hr)) return hr;

	while(pEnum->Next(1, &pFilter, &cFetched) == S_OK)
	{
		FILTER_INFO FilterInfo;
		hr = pFilter->QueryFilterInfo(&FilterInfo);
		if (FAILED(hr))
		{
			MessageBox(NULL, TEXT("Could not get the filter info"),
				TEXT("Error"), MB_OK | MB_ICONERROR);
			continue;  // Maybe the next one will work.
		}

		CLSID ClassID;

		pFilter->GetClassID(&ClassID);

		CEasyString FilterName=FilterInfo.achName;

		PrintSystemLog(0,"Filter:%s",(LPCTSTR)FilterName);

		IAMStreamSelect * pAMStreamSelect;

		if(SUCCEEDED(pFilter->QueryInterface(__uuidof(pAMStreamSelect),(void **)&pAMStreamSelect)))
		{
			DWORD StreamCount=0;
			pAMStreamSelect->Count(&StreamCount);			
			DWORD Flag=0;
			LCID lcID=0;
			DWORD Group=0;
			LPWSTR pName;
			for(long i=0;i<StreamCount;i++)
			{
				pAMStreamSelect->Info(i,NULL,&Flag,&lcID,&Group,&pName,NULL,NULL);
				CEasyString StreamName=pName;
				PrintSystemLog(0,"        Stream:(Flag=%u,LCID=%u,Group=%u)%s",Flag,lcID,Group,(LPCTSTR)StreamName);
				CoTaskMemFree(pName);
			}

			pAMStreamSelect->Release();
		}

		EnumPin(pFilter);
		

		// The FILTER_INFO structure holds a pointer to the Filter Graph
		// Manager, with a reference count that must be released.
		if (FilterInfo.pGraph != NULL)
		{
			FilterInfo.pGraph->Release();
		}
		pFilter->Release();
	}

	pEnum->Release();
	return S_OK;
}


HRESULT CD3DVideoTexture::EnumPin(IBaseFilter *pFilter)
{
	IEnumPins  *pEnum = NULL;
	IPin       *pPin = NULL;
	HRESULT    hr;
	

	hr = pFilter->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		return hr;
	}
	while(pEnum->Next(1, &pPin, 0) == S_OK)
	{
		PIN_INFO PinInfo;

		if(!FAILED(pPin->QueryPinInfo(&PinInfo)))
		{
			
			CEasyString PinName=PinInfo.achName;


			IPin *pTmp = 0;
			hr = pPin->ConnectedTo(&pTmp);
			if (SUCCEEDED(hr))  // Already connected, not the pin we want.
			{
				pTmp->Release();
				PrintSystemLog(0,"    Pin:%s,Connected",(LPCTSTR)PinName);
			}
			else  // Unconnected, this is the pin we want.
			{
				PrintSystemLog(0,"    Pin:%s,Not Connect",(LPCTSTR)PinName);
			}
			
			EnumMediaType(pPin);
			if(PinInfo.pFilter)
				PinInfo.pFilter->Release();

			//MessageBox(NULL, (LPCTSTR)PinName, TEXT("Filter Name"), MB_OK);
		}

		// Release the pin for the next time through the loop.
		pPin->Release();
	}
	// No more pins. We did not find a match.
	pEnum->Release();
	return E_FAIL;  
}

HRESULT CD3DVideoTexture::EnumMediaType(IPin *pPin)
{
	IEnumMediaTypes  *pEnum = NULL;
	AM_MEDIA_TYPE    *pMediaType = NULL;
	HRESULT    hr;


	hr = pPin->EnumMediaTypes(&pEnum);
	if (FAILED(hr))
	{
		return hr;
	}
	while(pEnum->Next(1, &pMediaType, 0) == S_OK)
	{
		// Release the pin for the next time through the loop.		
		DeleteMediaType(pMediaType);
	}
	// No more pins. We did not find a match.
	pEnum->Release();
	return E_FAIL;  
}