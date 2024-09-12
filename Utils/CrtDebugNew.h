/****************************************************************************/
/*                                                                          */
/*      文件名:    CrtDebugNew.h                                            */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


extern void* operator new( size_t size, LPCSTR file, int line );
extern void operator delete( void* address, LPCSTR file, int line );
extern void* operator new[]( size_t size, LPCSTR file, int line );
extern void operator delete[]( void* address , LPCSTR file, int line );

#define NEWNEW new( __FILE__, __LINE__ )

#define new				NEWNEW