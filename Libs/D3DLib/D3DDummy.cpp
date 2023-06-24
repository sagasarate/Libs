/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DDummy.cpp                                             */
/*      创建日期:  2009年09月25日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

namespace D3DLib{

IMPLEMENT_CLASS_INFO(CD3DDummy,CD3DObject);

CD3DDummy::CD3DDummy(void):CD3DObject()
{
	SetVisible(false);
	m_pBoundingFrame=NULL;	
}

CD3DDummy::~CD3DDummy(void)
{
	Destory();
}

void CD3DDummy::Destory()
{
	CD3DObject::Destory();
}

bool CD3DDummy::CanRender()
{
	return false;
}

void CD3DDummy::ShowBoundingFrame(int Operator)
{
	switch(Operator)
	{
	case DBFO_HIDE:
		{
			if(m_pBoundingFrame)
				m_pBoundingFrame->SetVisible(false);
		}
		break;
	case DBFO_SHOW:
		{
			if(m_pBoundingFrame)
				m_pBoundingFrame->SetVisible(true);
			else
			{
				if(GetRender()==NULL)
					return;
				m_pBoundingFrame=new CD3DBoundingFrame();
				m_pBoundingFrame->SetRender(GetRender());
				m_pBoundingFrame->CreateFromBBox(m_BoundingBox);
				m_pBoundingFrame->SetParent(this);
				GetRender()->AddObject(m_pBoundingFrame);
			}
		}
		break;
	case DBFO_RELEASE:
		SAFE_RELEASE(m_pBoundingFrame);
		break;
	}
}

bool CD3DDummy::ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	SAFE_RELEASE(m_pBoundingFrame);
	return CD3DObject::ToSmartStruct(Packet,pResourceManager,Param);
}

}