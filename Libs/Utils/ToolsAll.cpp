#include "stdafx.h"

UINT WalkCallStack(void * pFrame, LPVOID * AddressList, UINT MaxStep)
{
	LPVOID * pCallStack = (LPVOID *)pFrame;
	UINT i = 0;
	while (MaxStep)
	{
		AddressList[i] = *(pCallStack + 1);
		pCallStack = (LPVOID *)(*pCallStack);
		i++;
		MaxStep--;
	}
	return i;
}