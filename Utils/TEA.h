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

#define TEA_KEY_SIZE		16
#define TEA_BLOCK_SIZE		8
#define DEFAULT_TEA_CYCLE	16

void tea_encrypt(unsigned int *v, const unsigned int *k, unsigned int Cycle);
void tea_decrypt(unsigned int *v, const unsigned int *k, unsigned int Cycle);

