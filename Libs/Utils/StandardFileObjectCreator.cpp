/****************************************************************************/
/*                                                                          */
/*      文件名:    StandardFileObjectCreator.cpp                            */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"


IMPLEMENT_CLASS_INFO_STATIC(CStandardFileObjectCreator,IFileObjectCreator);

CStandardFileObjectCreator::CStandardFileObjectCreator(void):IFileObjectCreator()
{
}

CStandardFileObjectCreator::~CStandardFileObjectCreator(void)
{
}

IFileAccessor * CStandardFileObjectCreator::CreateFileAccessor()
{
	return new CStandardFileAccessor();
}

