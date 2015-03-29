/****************************************************************************/
/*                                                                          */
/*      文件名:    ResourceFileObjectCreator.cpp                            */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

IMPLEMENT_CLASS_INFO_STATIC(CResourceFileObjectCreator,IFileObjectCreator);

CResourceFileObjectCreator::CResourceFileObjectCreator(void)
{
}

CResourceFileObjectCreator::~CResourceFileObjectCreator(void)
{
}

IFileAccessor * CResourceFileObjectCreator::CreateFileAccessor()
{
	return new CResourceFileAccessor();
}