/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DObjectResource.h                                      */
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

class CD3DObjectResourceManager;

class CD3DObjectResource :
	public CNameObject
{
public:
	struct MODEL_FX_INFO
	{
		int			Type;
		CD3DFX *	pFX;
	};
protected:
	//struct STORAGE_STRUCT:public CNameObject::STORAGE_STRUCT
	//{		
	//	CD3DBoundingBox		BoundingBox;
	//	CD3DBoundingSphere	BoundingSphere;
	//	int					SubMeshCount;		
	//};

	enum SST_MEMBER_ID
	{
		SST_D3DOR_BOUDING_BOX=SST_NO_MAX,
		SST_D3DOR_BOUDING_SPHERE,
		SST_D3DOR_SUB_MESH,
		SST_D3DOR_MAX=SST_NO_MAX+50,
	};

	CD3DObjectResourceManager*		m_pManager;
	CEasyArray<CD3DSubMesh *>		m_SubMeshList;
	CD3DBoundingBox					m_BoundingBox;
	CD3DBoundingSphere				m_BoundingSphere;

	DECLARE_CLASS_INFO(CD3DObjectResource)
public:
	CD3DObjectResource();
	CD3DObjectResource(CD3DObjectResourceManager* pManager);
	virtual ~CD3DObjectResource(void);

	void SetManager(CD3DObjectResourceManager* pManager);

	virtual void Destory();

	virtual bool Reset();
	virtual bool Restore();

	virtual int GetSubMeshCount();
	virtual CD3DSubMesh * GetSubMesh(UINT index);

	

	virtual CD3DBoundingBox * GetBoundingBox();
	virtual CD3DBoundingSphere * GetBoundingSphere();
	virtual void PickResource(CUSOResourceManager * pResourceManager,UINT Param=0);

	virtual bool ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual UINT GetSmartStructSize(UINT Param=0);

protected:

	

	void CreateBounding();

	//virtual CNameObject::STORAGE_STRUCT * USOCreateHead(UINT Param=0);
	//virtual int USOWriteHead(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param=0);	
	//virtual bool USOWriteChild(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param=0);
	//

	//virtual int USOReadHead(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param=0);
	//virtual bool USOReadChild(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param=0);
	//virtual bool USOReadFinish(CNameObject::STORAGE_STRUCT * pHead,UINT Param=0);
};

inline void CD3DObjectResource::SetManager(CD3DObjectResourceManager* pManager)
{
	m_pManager=pManager;
}

}