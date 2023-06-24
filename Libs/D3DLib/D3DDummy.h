/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DDummy.h                                               */
/*      创建日期:  2009年09月25日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace D3DLib{

class CD3DDummy :
	public CD3DObject
{
protected:
	CD3DBoundingFrame *			m_pBoundingFrame;
	CD3DBoundingBox				m_BoundingBox;

	DECLARE_CLASS_INFO(CD3DDummy)
public:
	CD3DDummy(void);
	virtual ~CD3DDummy(void);

	virtual void Destory();

	virtual bool CanRender();

	virtual void ShowBoundingFrame(int Operator);	

	void SetBoundingBox(CD3DBoundingBox& BoundingBox);

	virtual bool ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
};



inline void CD3DDummy::SetBoundingBox(CD3DBoundingBox& BoundingBox)
{
	m_BoundingBox=BoundingBox;
}

}