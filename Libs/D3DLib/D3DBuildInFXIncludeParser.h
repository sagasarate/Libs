#pragma once

class CD3DBuildInFXIncludeParser :
	public ID3DXInclude
{
public:
	CD3DBuildInFXIncludeParser(void);
	virtual ~CD3DBuildInFXIncludeParser(void);

	STDMETHOD(Open)(D3DXINCLUDE_TYPE IncludeType,LPCSTR pFileName,LPCVOID pParentData,LPCVOID * ppData,UINT * pBytes);
	STDMETHOD(Close)(LPCVOID pData);
};
