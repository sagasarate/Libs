/****************************************************************************/
/*                                                                          */
/*      文件名:    stdafx.h                                                 */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
//#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CEasyString 构造函数将为显式的

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 从 Windows 头中排除极少使用的资料
#endif


#define USE_CRT_DETAIL_NEW
// TODO: 在此处引用程序要求的附加头文件
//#define D3D_DEBUG_INFO

#include "..\Utils\Utils.h"
#include "..\D3DGUI\D3DGUIHeaders.h"
#include "D3DLib.h"


//#define FUNCTION_BEGIN				static int s_FunctionIndex=0;static CPerformanceStatistician * s_pPS=NULL;if(!s_FunctionIndex) {s_pPS=CPerformanceStatistician::GetInstance();s_FunctionIndex=s_pPS->GetFunctionIndex(s_FunctionIndex,__FUNCTION__);} CAutoPerformanceCounter tmpPerformanceCounter(s_pPS,s_FunctionIndex);
//#define FUNCTION_END
//
//#define BLOCK_BEGIN(Name)			{static int s_BlockIndex=0;static CPerformanceStatistician * s_pPS=NULL;if(!s_BlockIndex) {s_pPS=CPerformanceStatistician::GetInstance();s_BlockIndex=s_pPS->GetFunctionIndex(s_BlockIndex,Name);} CAutoPerformanceCounter tmpPerformanceCounter(s_pPS,s_BlockIndex);
//#define BLOCK_END					}