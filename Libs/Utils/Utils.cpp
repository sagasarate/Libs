/****************************************************************************/
/*                                                                          */
/*      文件名:    Utils.cpp                                                */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"


void Test()
{
	CSmartValue Value1;
	CSmartValue Value2;

	Value1=(char)0;
	Value1=(unsigned char)0;
	Value1=(short)0;
	Value1=(unsigned short)0;
	Value1=(int)0;
	Value1=(unsigned int)0;
	Value1=(__int64)0;
	Value1=(unsigned __int64)0;

	Value2=Value1;

}




