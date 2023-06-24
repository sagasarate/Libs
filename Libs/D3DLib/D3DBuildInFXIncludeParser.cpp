#include "StdAfx.h"


CD3DBuildInFXIncludeParser::CD3DBuildInFXIncludeParser(void)
{
}

CD3DBuildInFXIncludeParser::~CD3DBuildInFXIncludeParser(void)
{
}


HRESULT CD3DBuildInFXIncludeParser::Open(D3DXINCLUDE_TYPE IncludeType,LPCSTR pFileName,LPCVOID pParentData,LPCVOID * ppData,UINT * pBytes)
{
	if(_stricmp(pFileName,"BuildIn\\SharedLib.fx")==0)
	{
		*ppData=SHARED_LIB_FX;
		*pBytes=strlen(SHARED_LIB_FX);
		return D3D_OK;
	}
	return D3DXERR_INVALIDDATA;
}
HRESULT CD3DBuildInFXIncludeParser::Close(LPCVOID pData)
{
	return D3D_OK;
}