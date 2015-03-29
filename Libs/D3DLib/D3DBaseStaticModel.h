/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DBaseStaticModel.h                                     */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace D3DLib{

class CD3DBaseStaticModel :
	public CD3DObject
{
protected:
protected:

	DECLARE_CLASS_INFO(CD3DBaseStaticModel)
public:
	CD3DBaseStaticModel(void);
	~CD3DBaseStaticModel(void);

	virtual void Destory();

	virtual bool Reset();
	virtual bool Restore();
	

};

}