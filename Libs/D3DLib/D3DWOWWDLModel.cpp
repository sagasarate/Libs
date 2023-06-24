/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWOWWDLModel.cpp				                        */
/*      创建日期:  2011年06月08日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/

#include "StdAfx.h"

namespace D3DLib{

IMPLEMENT_FILE_CHANNEL_MANAGER(CD3DWOWWDLModel)

IMPLEMENT_CLASS_INFO(CD3DWOWWDLModel,CD3DBaseStaticModel);

CD3DWOWWDLModel::CD3DWOWWDLModel(void)
{
	m_pModelResource=NULL;
}

CD3DWOWWDLModel::~CD3DWOWWDLModel(void)
{
	Destory();
}

void CD3DWOWWDLModel::Destory()
{	
	CD3DBaseStaticModel::Destory();
	
	SAFE_RELEASE(m_pModelResource);

}

bool CD3DWOWWDLModel::Reset()
{
	return true;
}

bool CD3DWOWWDLModel::Restore()
{
	return true;
}

bool CD3DWOWWDLModel::LoadFromFile(LPCTSTR ModelFileName)
{

	CD3DWOWWDLModelResource* pResource=
		dynamic_cast<CD3DWOWWDLModelResource*>(GetDevice()->GetObjectResourceManager()->GetResource(ModelFileName));
	if(!pResource)
	{
		pResource=new CD3DWOWWDLModelResource(GetDevice()->GetObjectResourceManager());
		if(!pResource->LoadFromFile(ModelFileName))
		{
			pResource->Release();
			return false;
		}
		if(!GetDevice()->GetObjectResourceManager()->AddResource(pResource,ModelFileName))
		{
			pResource->Release();
			return false;
		}
	}
	else
	{
		pResource->AddUseRef();
	}	


	SetName(ModelFileName);

	bool Ret=LoadFromResource(pResource);
	SAFE_RELEASE(pResource);
	return Ret;
}

bool CD3DWOWWDLModel::LoadFromResource(CD3DWOWWDLModelResource * pModelResource)
{
	if(GetDevice()==NULL)
		return false;

	Destory();

	m_pModelResource=pModelResource;	
	pModelResource->AddUseRef();
	
	return true;
}

void CD3DWOWWDLModel::SetAreaVisible(UINT AreaX,UINT AreaY,bool IsVisible)
{
	UINT SubMeshID=AreaY*100+AreaX;
	for(int i=0;i<GetSubMeshCount();i++)
	{
		CD3DSubMesh * pSubMesh=GetSubMesh(i);
		if(pSubMesh->GetID()==SubMeshID)
		{
			pSubMesh->SetVisible(IsVisible);
			break;
		}
	}
}

void CD3DWOWWDLModel::PickResource(CUSOResourceManager * pResourceManager,UINT Param)
{
	CD3DBaseStaticModel::PickResource(pResourceManager,Param);
	if(m_pModelResource)
	{
		m_pModelResource->PickResource(pResourceManager);
		pResourceManager->AddResource(m_pModelResource);
	}
}

bool CD3DWOWWDLModel::ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	return false;
}
bool CD3DWOWWDLModel::FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	return false;
}

UINT CD3DWOWWDLModel::GetSmartStructSize(UINT Param)
{
	return 0;
}

void CD3DWOWWDLModel::OnPrepareRender(CD3DBaseRender * pRender,CD3DFX * pFX,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera)
{
	CD3DMatrix Mat=GetWorldMatrixR()*pCamera->GetViewMatR();
	pFX->SetMatrix("WorldViewMatrix",Mat);
}
void CD3DWOWWDLModel::OnPrepareRenderSubMesh(CD3DBaseRender * pRender,CD3DFX * pFX,CD3DSubMesh * pSubMesh,CD3DSubMeshMaterial * pMaterial,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera)
{
	if(pSubMesh->IsSelected())
	{
		pFX->SetColor("GlobalColor",0xFFFF0000);
	}
	else
	{
		pFX->SetColor("GlobalColor",0xFFFFFFFF);
	}	
}

void CD3DWOWWDLModel::Update(FLOAT Time)
{
	CD3DBaseStaticModel::Update(Time);	
}

int CD3DWOWWDLModel::GetSubMeshCount()
{
	if(m_pModelResource)
		return m_pModelResource->GetSubMeshCount();
	return 0;
}
CD3DSubMesh * CD3DWOWWDLModel::GetSubMesh(UINT index)
{
	if(m_pModelResource)
		return m_pModelResource->GetSubMesh(index);
	return NULL;
}
CD3DSubMeshMaterial * CD3DWOWWDLModel::GetSubMeshMaterial(UINT index)
{
	if(m_pModelResource)
		return &(m_pModelResource->GetSubMesh(index)->GetMaterial());
	return NULL;
}

CD3DBoundingBox * CD3DWOWWDLModel::GetBoundingBox()
{	
	if(m_pModelResource)
		return m_pModelResource->GetBoundingBox();
	return NULL;
}
CD3DBoundingSphere * CD3DWOWWDLModel::GetBoundingSphere()
{
	if(m_pModelResource)
		return m_pModelResource->GetBoundingSphere();
	return NULL;
}
}