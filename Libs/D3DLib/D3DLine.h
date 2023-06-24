#pragma once

namespace D3DLib{

class CD3DLine :
	public CD3DObject
{
protected:
	struct VERTEX
	{
		CD3DVector3 Pos;
		D3DCOLOR	Color;
	};

	CD3DSubMesh *	m_pSubMesh;
	CD3DBoundingBox	m_BoundingBox;

	DECLARE_CLASS_INFO(CD3DLine)
public:
	CD3DLine(void);
	~CD3DLine(void);

	virtual void Destory();
	virtual bool Reset();
	virtual bool Restore();

	bool Create(CD3DVector3 P1,CD3DVector3 P2,D3DCOLOR Color1,D3DCOLOR Color2);
	void Modify(CD3DVector3 P1,CD3DVector3 P2,D3DCOLOR Color1,D3DCOLOR Color2);

	virtual int GetSubMeshCount();
	virtual CD3DSubMesh * GetSubMesh(UINT index);
	virtual CD3DBoundingBox * GetBoundingBox();

	virtual bool ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual UINT GetSmartStructSize(UINT Param=0);
};


}