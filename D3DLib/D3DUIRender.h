/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DUIRender.h                                            */
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




class CD3DUIRender :
	public CD3DBaseRender
{
protected:

	CEasyArray<CD3DObject *>		m_ObjectList;	


	DECLARE_CLASS_INFO(CD3DUIRender)
public:
	CD3DUIRender();
	virtual ~CD3DUIRender(void);

	bool Create(CD3DDevice * pDevice);
	virtual void Destory();
	virtual bool Reset();
	virtual bool Restore();	
		
	virtual void Render();
	virtual void RenderDirectly(CD3DObject * pObject);	

	virtual bool AddObject(CD3DObject * pObj,bool IsRecursive=true);
	virtual bool DelObject(CD3DObject * pObj,bool IsRecursive=true);

	virtual void RemoveAllObject();

	bool MoveToTop(CD3DObject * pObj,CD3DObject *pBefore);
	bool MoveToTop(CD3DObject ** ppObj,UINT ObjectCount,CD3DObject *pBefore);

protected:
	
	
};



}