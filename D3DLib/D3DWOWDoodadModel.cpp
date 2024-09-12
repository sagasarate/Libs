/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWOWDoodadModel.cpp	                                */
/*      创建日期:  2010年08月05日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"


namespace D3DLib{

IMPLEMENT_CLASS_INFO(CD3DWOWDoodadModel,CD3DWOWM2Model);

CD3DWOWDoodadModel::CD3DWOWDoodadModel(void)
{
	m_GroupIndex=0xffff;
}

CD3DWOWDoodadModel::~CD3DWOWDoodadModel(void)
{
	Destory();
}

void CD3DWOWDoodadModel::Destory()
{
	CD3DWOWM2Model::Destory();
	m_GroupIndex=0xffff;
}

}