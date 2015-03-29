/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DFXManager.cpp                                         */
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



IMPLEMENT_CLASS_INFO_STATIC(CD3DFXManager,CNameObject);

CD3DFXManager::CD3DFXManager(CD3DDevice * pD3DDevice,int StorageSize):CNameObject()
{
	m_pD3DDevice=pD3DDevice;
	m_pEffectPool=NULL;
	D3DXCreateEffectPool(&m_pEffectPool);
	m_FXStorage.Create(StorageSize);
	m_DelayReleaseTime=0;
}

CD3DFXManager::~CD3DFXManager(void)
{
#ifdef _DEBUG
	void * Pos;

	Pos=m_FXStorage.GetFirstObjectPos();
	while(Pos)
	{
		CEasyString Key;
		FX_INFO * pFXInfo=m_FXStorage.GetNextObject(Pos,Key);
		if(m_DelayReleaseTime==0)
			PrintSystemLog(0,_T("FX<%s>未释放！"),(LPCTSTR)pFXInfo->pFX->GetName());
		SAFE_RELEASE(pFXInfo->pFX);
	}
	m_FXStorage.Destory();
#endif
}

bool CD3DFXManager::Reset()
{
	void * Pos;

	Pos=m_FXStorage.GetFirstObjectPos();
	while(Pos)
	{		
		CEasyString Key;
		FX_INFO * pFXInfo=m_FXStorage.GetNextObject(Pos,Key);
		pFXInfo->pFX->Reset();
	}
	return true;
}

bool CD3DFXManager::Restore()
{
	void * Pos;

	Pos=m_FXStorage.GetFirstObjectPos();
	while(Pos)
	{		
		CEasyString Key;
		FX_INFO * pFXInfo=m_FXStorage.GetNextObject(Pos,Key);
		pFXInfo->pFX->Restore();
	}
	return true;
}

bool CD3DFXManager::AddFX(CD3DFX * pFX,LPCTSTR Name)
{
	CEasyString Key=Name;
	Key.MakeUpper();
	FX_INFO FXInfo;
	FXInfo.pFX=pFX;
	FXInfo.IsPrepareRelease=false;
	UINT ID=m_FXStorage.Insert(Key,FXInfo);
	if(ID)
	{
		FXInfo.pFX->SetID(ID);
		FXInfo.pFX->SetName(Name);
		if(m_DelayReleaseTime)
		{
			FXInfo.pFX->AddUseRef();
		}		
		return true;
	}
	else
	{
		PrintD3DLog(0,_T("将FX加入FX管理器失败(%u,%u)"),
			m_FXStorage.GetObjectCount(),m_FXStorage.GetBufferSize());
		return false;
	}
}

bool CD3DFXManager::DeleteFX(UINT ID)
{	
	if(m_FXStorage.DeleteByID(ID))
	{
		return true;
	}
	else
	{
		PrintD3DLog(0,_T("CD3DFXManager::DeleteFX:FX[%u]未找到"),
			ID);
		return false;
	}		
}

bool CD3DFXManager::DeleteFX(LPCTSTR Name)
{
	CEasyString Key=Name;
	Key.MakeUpper();
	FX_INFO * pFXInfo=m_FXStorage.Find(Key);
	if(pFXInfo)
	{
		m_FXStorage.DeleteByID(pFXInfo->pFX->GetID());		
		return true;
	}
	else
	{
		PrintD3DLog(0,_T("CD3DFXManager::DeleteFX:FX[%s]未找到"),
			Name);
		return false;
	}
}

CD3DFX * CD3DFXManager::LoadFX(LPCTSTR FileName)
{
	CEasyString FXFileName=CD3DFX::FindFileOne(FileName);

	CD3DFX * pFX=GetFX(FXFileName);
	if(pFX)
	{
		pFX->AddUseRef();
		return pFX;
	}

	pFX=new CD3DFX(this);

#ifdef D3D_DEBUG_INFO
	if(pFX->LoadFromFileDirect(FXFileName))
	{
		if(AddFX(pFX,FXFileName))
			return pFX;
	}
#else
	if(pFX->LoadFromFile(FXFileName))
	{
		if(AddFX(pFX,FXFileName))
			return pFX;
	}
#endif
	delete pFX;
	return NULL;
}

CD3DFX * CD3DFXManager::LoadFXFromMemory(LPCTSTR Name,const void * pData,int DataSize)
{	
	CD3DFX * pFX=GetFX(Name);
	if(pFX)
	{
		pFX->AddUseRef();
		return pFX;
	}

	pFX=new CD3DFX(this);

	if(pFX->LoadFromMemory(pData,DataSize))
	{
		if(AddFX(pFX,Name))
			return pFX;
	}
	delete pFX;
	return NULL;
}

int CD3DFXManager::GetCount()
{
	return m_FXStorage.GetObjectCount();
}

LPVOID CD3DFXManager::GetFirstPos()
{
	return m_FXStorage.GetFirstObjectPos();
}

LPVOID CD3DFXManager::GetLastPos()
{
	return m_FXStorage.GetLastObjectPos();
}

CD3DFX * CD3DFXManager::GetNext(LPVOID& Pos)
{
	CEasyString Key;
	return m_FXStorage.GetNextObject(Pos,Key)->pFX;
}

CD3DFX * CD3DFXManager::GetPrev(LPVOID& Pos)
{
	CEasyString Key;
	return m_FXStorage.GetPrevObject(Pos,Key)->pFX;
}

int CD3DFXManager::Update(FLOAT Time)
{
	if(m_DelayReleaseTime)
	{
		void * Pos;
		Pos=m_FXStorage.GetFirstObjectPos();
		while(Pos)
		{		
			CEasyString Key;
			FX_INFO * pFXInfo=m_FXStorage.GetNextObject(Pos,Key);
			if(pFXInfo->pFX->GetUseRef()==1)
			{
				if(pFXInfo->IsPrepareRelease)
				{
					if(pFXInfo->ReleaseTimer.IsTimeOut(m_DelayReleaseTime))
					{
						//PrintD3DDebugLog(0,"CD3DFXManager::Update:已延时删除FX:%s",pFXInfo->pFX->GetName());
						SAFE_RELEASE(pFXInfo->pFX);						
					}
				}
				else
				{
					pFXInfo->IsPrepareRelease=true;
					pFXInfo->ReleaseTimer.SaveTime();
				}
			}
		}
	}
	return 0;
}

}