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

#include "stdafx.h"

void tea_encrypt(unsigned int *v, const unsigned int *k, unsigned int Cycle)
{
	unsigned int y=v[0], z=v[1], sum=0, i;         /* set up */
	unsigned int delta=0x9e3779b9;                 /* a key schedule constant */
	unsigned int a=k[0], b=k[1], c=k[2], d=k[3];   /* cache key */
	for (i=0; i < Cycle; i++) 
	{                        /* basic cycle start */
		sum += delta;
		y += ((z<<4) + a) ^ (z + sum) ^ ((z>>5) + b);
		z += ((y<<4) + c) ^ (y + sum) ^ ((y>>5) + d);/* end cycle */
	}
	v[0]=y;
	v[1]=z;
 }
 
void tea_decrypt(unsigned int *v, const unsigned int *k, unsigned int Cycle)
{
	unsigned int y=v[0], z=v[1], sum=0, i; /* set up */
	unsigned int delta=0x9e3779b9;                  /* a key schedule constant */
	unsigned int a=k[0], b=k[1], c=k[2], d=k[3];    /* cache key */

	if (Cycle == 32)
		sum = 0xC6EF3720; /* delta << 5*/
	else if (Cycle == 16)
		sum = 0xE3779B90; /* delta << 4*/
	else
		sum = delta * Cycle;


	for(i=0; i<Cycle; i++) 
	{                            /* basic cycle start */
		 z -= ((y<<4) + c) ^ (y + sum) ^ ((y>>5) + d);
		 y -= ((z<<4) + a) ^ (z + sum) ^ ((z>>5) + b);
		 sum -= delta;                                /* end cycle */
	}
	v[0]=y;
	v[1]=z;
}

