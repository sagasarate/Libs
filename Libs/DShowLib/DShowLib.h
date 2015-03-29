/****************************************************************************/
/*                                                                          */
/*      文件名:    DShowLib.h                                               */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

#define DSHOWLIB_VERSION	"1.0.0.0"

#define USE_CRT_DETAIL_NEW

#include "dshow.h"
#include "vmr9.h"

#include ".\FilterBase\Streams.h"
#include "IDirectVobSub.h"

#include "D3DVideoTexture.h"

#pragma comment(lib,"strmiids.lib")
#pragma comment(lib,"quartz.lib")
//#pragma comment(lib,"wmvcore.lib")

using namespace DShowLib;