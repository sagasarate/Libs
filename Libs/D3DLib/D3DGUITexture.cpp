/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DGUITexture.cpp                                        */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"
#include "D3DGUITexture.h"

namespace D3DGUI{

IMPLEMENT_CLASS_INFO(CD3DGUITexture,CD3DTexture);

CD3DGUITexture::CD3DGUITexture():CD3DTexture()
{
}

CD3DGUITexture::CD3DGUITexture(CD3DTextureManager * pManager):CD3DTexture(pManager)
{
}

CD3DGUITexture::~CD3DGUITexture(void)
{
}

}