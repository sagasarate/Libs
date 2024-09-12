/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DBaseDynamicModel.h                                    */
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

class CD3DBaseDynamicModel :
	public CD3DObject
{
protected:
	

	DECLARE_CLASS_INFO(CD3DBaseDynamicModel)
public:
	CD3DBaseDynamicModel(void);
	virtual ~CD3DBaseDynamicModel(void);

	virtual void Destory();

	virtual bool Reset();
	virtual bool Restore();

	virtual bool Play(bool IsLoop);	
	virtual bool Stop();
	virtual bool IsPlaying();
	virtual void SetPlaySpeed(FLOAT Rate);

};

}