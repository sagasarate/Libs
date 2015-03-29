#pragma once

namespace D3DLib{

class CD3DBoard :
	public CD3DObject
{
protected:
	struct VERTEX
	{
		CD3DVector3 Pos;
		D3DCOLOR	Color;
	};

	CD3DSubMesh *	m_pSubMesh[2];
	CD3DBoundingBox	m_BoundingBox;

	DECLARE_CLASS_INFO(CD3DBoard)
public:
	CD3DBoard(void);
	~CD3DBoard(void);

	virtual void Destory();
	virtual bool Reset();
	virtual bool Restore();

	bool Create(CD3DVector3 V1,CD3DVector3 V2,CD3DVector3 V3,CD3DVector3 V4,CD3DVector3 Normal,D3DCOLOR Color);

	virtual int GetSubMeshCount();
	virtual CD3DSubMesh * GetSubMesh(UINT index);
	virtual CD3DBoundingBox * GetBoundingBox();

	virtual bool ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual UINT GetSmartStructSize(UINT Param=0);
};

}