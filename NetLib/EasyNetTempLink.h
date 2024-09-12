#pragma once

class CEasyNetTempLink :
	public CEasyNetLink
{
protected:

	DECLARE_CLASS_INFO_STATIC(CEasyNetTempLink);
public:
	CEasyNetTempLink();
	~CEasyNetTempLink();

	virtual void OnData(const BYTE * pData, UINT DataSize)
	{

	}
};

