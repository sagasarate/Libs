/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DBaseStaticModel.cpp                                   */
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

IMPLEMENT_CLASS_INFO(CD3DBaseStaticModel,CD3DObject);

CD3DBaseStaticModel::CD3DBaseStaticModel(void)
{
}

CD3DBaseStaticModel::~CD3DBaseStaticModel(void)
{
	Destory();
}


void CD3DBaseStaticModel::Destory()
{
	CD3DObject::Destory();
}

bool CD3DBaseStaticModel::Reset()
{	
	return m_pBoundingFrame->Reset()&&CD3DObject::Reset();
}

bool CD3DBaseStaticModel::Restore()
{
	return m_pBoundingFrame->Restore()&&CD3DObject::Reset();
}



}