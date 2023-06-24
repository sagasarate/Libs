/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWOWWDLModelResource.h                                 */
/*      创建日期:  2011年06月08日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace D3DLib{

class CD3DWOWWDLModelResource :
	public CD3DObjectResource
{
protected:
	struct MODEL_VERTEXT
	{
		CD3DVector3			Pos;
	};

	DECLARE_CLASS_INFO(CD3DWOWWDLModelResource)
public:
	CD3DWOWWDLModelResource(void);
	CD3DWOWWDLModelResource(CD3DObjectResourceManager* pManager);
	~CD3DWOWWDLModelResource(void);

	virtual void Destory();

	virtual bool Reset();
	virtual bool Restore();

	bool LoadFromFile(LPCTSTR ModelFileName);

public:	
	virtual void PickResource(CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual UINT GetSmartStructSize(UINT Param=0);
};

}