/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DBaseDynamicModel.cpp                                  */
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

IMPLEMENT_CLASS_INFO(CD3DBaseDynamicModel,CD3DObject);

CD3DBaseDynamicModel::CD3DBaseDynamicModel(void):CD3DObject()
{
	
}

CD3DBaseDynamicModel::~CD3DBaseDynamicModel(void)
{
	Destory();
}

void CD3DBaseDynamicModel::Destory()
{
	
	CD3DObject::Destory();
}

bool CD3DBaseDynamicModel::Reset()
{	
	return m_pBoundingFrame->Reset()&&CD3DObject::Reset();
}

bool CD3DBaseDynamicModel::Restore()
{
	return m_pBoundingFrame->Restore()&&CD3DObject::Reset();
}

bool CD3DBaseDynamicModel::Play(bool IsLoop)
{
	return false;
}
bool CD3DBaseDynamicModel::Stop()
{
	return false;
}
bool CD3DBaseDynamicModel::IsPlaying()
{
	return false;
}

void CD3DBaseDynamicModel::SetPlaySpeed(FLOAT Rate)
{

}



}