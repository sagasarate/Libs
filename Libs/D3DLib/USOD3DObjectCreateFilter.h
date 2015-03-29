/****************************************************************************/
/*                                                                          */
/*      文件名:    USOD3DObjectCreateFilter.h                               */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace D3DLib{


class CUSOD3DObjectCreateFilter :
	public CUSOObjectCreateFilter
{
protected:
	CD3DDevice *	m_pD3DDevice;

	DECLARE_CLASS_INFO(CUSOD3DObjectCreateFilter)
public:
	CUSOD3DObjectCreateFilter();
	CUSOD3DObjectCreateFilter(CD3DDevice * pDevice);
	~CUSOD3DObjectCreateFilter(void);

	void SetDevice(CD3DDevice * pDevice);

	virtual bool OnObjectCreate(CNameObject * pObject);
	virtual CNameObject * FindObject(CLASS_INFO * pClassInfo,LPCTSTR ObjectName);
};


}