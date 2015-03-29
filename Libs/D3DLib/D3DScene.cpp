/****************************************************************************/
/*                                                                          */
/*      �ļ���:    D3DScene.cpp                                             */
/*      ��������:  2009��10��20��                                           */
/*      ����:      Sagasarate                                               */
/*                                                                          */
/*      ��������Ȩ��Sagasarate(sagasarate@sina.com)����                     */
/*      ����Խ������������κ���ҵ�ͷ���ҵ������������                      */
/*      ���뱣���˰�Ȩ����                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

namespace D3DLib{


IMPLEMENT_CLASS_INFO(CD3DScene,CD3DObject);

CD3DScene::CD3DScene(void):CD3DObject()
{
	SetVisible(false);	
}

CD3DScene::~CD3DScene(void)
{
}


bool CD3DScene::GetHeightByXZ(const CD3DVector3 Pos,FLOAT MinHeight,FLOAT MaxHeight,FLOAT& Height,FLOAT& WaterHeight)
{	
	bool HaveHeight=false;
	FLOAT FinalHeight=-1E38,FinalWaterHeight=-1E38;
	for(UINT i=0;i<GetChildCount();i++)
	{
		if(GetChildByIndex(i)->IsKindOf(GET_CLASS_INFO(CD3DWOWADTModel)))
		{
			FLOAT ADTHeight,ADTWaterHeight;
			CD3DWOWADTModel * pModel=(CD3DWOWADTModel *)GetChildByIndex(i);
			if(pModel->GetHeightByXZ(Pos,MinHeight,MaxHeight,ADTHeight,ADTWaterHeight))
			{				
				HaveHeight=true;
				if(ADTHeight>FinalHeight)
					FinalHeight=ADTHeight;
				if(ADTWaterHeight>FinalWaterHeight)
					FinalWaterHeight=ADTWaterHeight;
			}
		}
	}	

	if(HaveHeight)
	{
		Height=FinalHeight;
		WaterHeight=FinalWaterHeight;
	}
	return HaveHeight;
}



bool CD3DScene::AddChild(CTreeObject* pChild,bool CheckExist)
{
	if(CD3DObject::AddChild(pChild,CheckExist))
	{		
		if(pChild->IsKindOf(GET_CLASS_INFO(CD3DObject)))
		{
			CD3DObject * pObject=(CD3DObject *)pChild;
			pObject->Update(0);
			if(GetChildCount()==1)
			{
				m_BoundingBox=(*pObject->GetBoundingBox())*pObject->GetWorldMatrix();
				CD3DMatrix Mat=GetWorldMatrix();
				Mat.Invert();
				m_BoundingBox*=Mat;
			}
			else
			{
				CD3DBoundingBox BBox=(*pObject->GetBoundingBox())*pObject->GetWorldMatrix();
				CD3DMatrix Mat=GetWorldMatrix();
				Mat.Invert();
				m_BoundingBox.Merge(BBox*Mat);
			}	
		}
		return true;
	}
	return false;
}

}