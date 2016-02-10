/****************************************************************************/
/*                                                                          */
/*      文件名:    TEA.h                                                */
/*      创建日期:  2014年1月2日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

extern bool TEAEncode(const BYTE * pIn, BYTE * pOut, UINT DataLen, const BYTE * pKey, unsigned int Cycle);
extern bool TEADecode(const BYTE * pIn, BYTE * pOut, UINT DataLen, const BYTE * pKey, unsigned int Cycle);