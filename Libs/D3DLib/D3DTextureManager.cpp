/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DTextureManager.cpp                                    */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"


namespace D3DLib{

IMPLEMENT_CLASS_INFO_STATIC(CD3DTextureManager,CNameObject);

CD3DTextureManager::CD3DTextureManager(CD3DDevice * pD3DDevice,int StorageSize):CNameObject()
{
	m_pD3DDevice=pD3DDevice;
	m_TextureStorage.Create(StorageSize);
	m_DelayReleaseTime=0;
}

CD3DTextureManager::~CD3DTextureManager(void)
{
#ifdef _DEBUG
	CAutoLock Lock(m_CriticalSection);
	void * Pos;

	Pos=m_TextureStorage.GetFirstObjectPos();
	while(Pos)
	{
		CEasyString Key;
		TESTURE_INFO * pTextureInfo=m_TextureStorage.GetNextObject(Pos,Key);
		if(m_DelayReleaseTime==0)
			PrintD3DLog(_T("纹理<%s>未释放！"), (LPCTSTR)pTextureInfo->pTexture->GetName());
		SAFE_RELEASE(pTextureInfo->pTexture);
	}
	m_TextureStorage.Destory();
#endif
}

bool CD3DTextureManager::Reset()
{
	CAutoLock Lock(m_CriticalSection);

	bool Ret=true;
	void * Pos;

	Pos=m_TextureStorage.GetFirstObjectPos();
	while(Pos)
	{		
		CEasyString Key;
		TESTURE_INFO * pTextureInfo=m_TextureStorage.GetNextObject(Pos,Key);
		if(!pTextureInfo->pTexture->Reset())
			Ret=false;

	}
	for(UINT i=0;i<m_DynamicTextureList.GetCount();i++)
	{
		if(!m_DynamicTextureList[i]->Reset())
			Ret=false;
	}
	return Ret;
}

bool CD3DTextureManager::Restore()
{
	CAutoLock Lock(m_CriticalSection);

	bool Ret=true;
	void * Pos;

	Pos=m_TextureStorage.GetFirstObjectPos();
	while(Pos)
	{		
		CEasyString Key;
		TESTURE_INFO * pTextureInfo=m_TextureStorage.GetNextObject(Pos,Key);
		if(!pTextureInfo->pTexture->Restore())
			Ret=false;

	}
	for(UINT i=0;i<m_DynamicTextureList.GetCount();i++)
	{
		if(!m_DynamicTextureList[i]->Restore())
			Ret=false;
	}
	return Ret;
}


bool CD3DTextureManager::AddTexture(CD3DTexture * pTexture,LPCTSTR TextureName)
{
	CAutoLock Lock(m_CriticalSection);

	CEasyString Key=TextureName;
	Key.MakeUpper();
	TESTURE_INFO TextureInfo;
	TextureInfo.pTexture=pTexture;
	TextureInfo.IsPrepareRelease=false;
	UINT ID=m_TextureStorage.Insert(Key,TextureInfo);
	if(ID)
	{
		TextureInfo.pTexture->SetID(ID);
		TextureInfo.pTexture->SetName(TextureName);
		if(TextureInfo.pTexture->IsKindOf(GET_CLASS_INFO(CD3DIFLTexture))||
			TextureInfo.pTexture->IsKindOf(GET_CLASS_INFO(CD3DTextTexture)))
		{
			AddAniTexture(TextureInfo.pTexture);
		}
		if(m_DelayReleaseTime)
		{
			TextureInfo.pTexture->AddUseRef();
		}
		return true;
	}
	else
	{
		PrintD3DLog(_T("把纹理添加到纹理管理器失败(%u,%u)"),
			m_TextureStorage.GetObjectCount(),m_TextureStorage.GetBufferSize());
		return false;
	}
}

bool CD3DTextureManager::DeleteTexture(UINT ID)
{
	CAutoLock Lock(m_CriticalSection);

	TESTURE_INFO * pTextureInfo=m_TextureStorage.GetObject(ID);
	if(pTextureInfo)
	{		
		DelAniTexture(pTextureInfo->pTexture);
		m_TextureStorage.DeleteByID(ID);
		return true;
	}
	else
	{
		PrintD3DLog(_T("CD3DTextureManager::DeleteTexture:纹理[%u]未找到"),
			ID);
		return false;
	}
}

bool CD3DTextureManager::DeleteTexture(LPCTSTR TextureName)
{
	CAutoLock Lock(m_CriticalSection);

	CEasyString Key=TextureName;
	Key.MakeUpper();
	TESTURE_INFO * pTextureInfo=m_TextureStorage.Find(Key);
	if(pTextureInfo)
	{
		DelAniTexture(pTextureInfo->pTexture);
		m_TextureStorage.DeleteByID(pTextureInfo->pTexture->GetID());		
		return true;
	}
	else
	{
		PrintD3DLog(_T("CD3DTextureManager::DeleteTexture:纹理[%s]未找到"),
			TextureName);
		return false;
	}	
}

CD3DTexture * CD3DTextureManager::LoadTexture(LPCTSTR TextureFileName,UINT MipLevels,bool UseFilter,bool IsManaged,D3DCOLOR KeyColor)
{
	CEasyString FileName=CD3DTexture::FindFileOne(TextureFileName);
	CD3DTexture * pTexture=GetTextrue(FileName);
	if(pTexture)
	{
		pTexture->AddUseRef();
		return pTexture;
	}

	pTexture=new CD3DTexture(this);

	if(pTexture->LoadTexture(FileName,MipLevels,UseFilter,IsManaged,KeyColor))
	{
		if(AddTexture(pTexture,FileName))
			return pTexture;
	}	
	delete pTexture;
	return NULL;
}

CD3DIFLTexture * CD3DTextureManager::LoadIFLTexture(LPCTSTR TextureFileName,UINT MipLevels,bool UseFilter,bool IsManaged,D3DCOLOR KeyColor)
{
	CEasyString FileName=CD3DTexture::FindFileOne(TextureFileName);
	CD3DIFLTexture * pTexture=(CD3DIFLTexture *)GetTextrue(FileName);
	if(pTexture)
	{
		pTexture->AddUseRef();
		return pTexture;
	}

	
	pTexture=new CD3DIFLTexture(this);
	if(pTexture->LoadTexture(FileName,MipLevels,UseFilter,IsManaged,KeyColor))
	{
		if(AddTexture(pTexture,FileName))
			return pTexture;
	}
	
	delete pTexture;
	return NULL;
}

int CD3DTextureManager::GetCount()
{
	return m_TextureStorage.GetObjectCount();
}

LPVOID CD3DTextureManager::GetFirstPos()
{
	return m_TextureStorage.GetFirstObjectPos();
}

LPVOID CD3DTextureManager::GetLastPos()
{
	return m_TextureStorage.GetLastObjectPos();
}

CD3DTexture * CD3DTextureManager::GetNext(LPVOID& Pos)
{
	CEasyString Key;
	return m_TextureStorage.GetNextObject(Pos,Key)->pTexture;
}

CD3DTexture * CD3DTextureManager::GetPrev(LPVOID& Pos)
{
	CEasyString Key;
	return m_TextureStorage.GetPrevObject(Pos,Key)->pTexture;
}

int CD3DTextureManager::Update(FLOAT Time)
{
	CAutoLock Lock(m_CriticalSection);

	for(UINT i=0;i<m_DynamicTextureList.GetCount();i++)
	{
		m_DynamicTextureList[i]->Update(Time);
	}

	if(m_DelayReleaseTime)
	{
		void * Pos;
		Pos=m_TextureStorage.GetFirstObjectPos();
		while(Pos)
		{		
			CEasyString Key;
			TESTURE_INFO * pTestureInfo=m_TextureStorage.GetNextObject(Pos,Key);
			if(pTestureInfo->pTexture->GetUseRef()==1)
			{
				if(pTestureInfo->IsPrepareRelease)
				{
					if(pTestureInfo->ReleaseTimer.IsTimeOut(m_DelayReleaseTime))
					{
						//PrintD3DDebugLog("CD3DFXManager::Update:已延时删除纹理:%s",pTestureInfo->pTexture->GetName());
						SAFE_RELEASE(pTestureInfo->pTexture);
					}
				}
				else
				{
					pTestureInfo->IsPrepareRelease=true;
					pTestureInfo->ReleaseTimer.SaveTime();
				}
			}
		}
	}
	return 0;
}

void CD3DTextureManager::PrepareRenderData()
{
	for(UINT i=0;i<m_DynamicTextureList.GetCount();i++)
	{
		m_DynamicTextureList[i]->OnPrepareRenderData();
	}
}

void CD3DTextureManager::AddAniTexture(CD3DTexture * pTexture)
{
	CAutoLock Lock(m_CriticalSection);

	for(UINT i=0;i<m_DynamicTextureList.GetCount();i++)
	{
		if(m_DynamicTextureList[i]==pTexture)
		{
			return;
		}
	}
	m_DynamicTextureList.Add(pTexture);
}

void CD3DTextureManager::DelAniTexture(CD3DTexture * pTexture)
{
	CAutoLock Lock(m_CriticalSection);

	for(UINT i=0;i<m_DynamicTextureList.GetCount();i++)
	{
		if(m_DynamicTextureList[i]==pTexture)
		{
			m_DynamicTextureList.Delete(i);
			break;
		}
	}
}

}