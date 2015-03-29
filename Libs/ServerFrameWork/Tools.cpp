/****************************************************************************/
/*                                                                          */
/*      文件名:    Tools.cpp                                                */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"


WORD g_ProgramVersion[4]={0,0,0,0};
#ifdef WIN32
int g_ServiceWorkMode=SWM_SERVICE;
#else
int g_ServiceWorkMode=SWM_APPLICATION;
#endif