/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DFontCharClip.h                                        */
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

class CD3DFontCharClip :
	public CD3DObject
{
protected:
	CD3DSubMesh		m_SubMesh;	
	RECT			m_Rect;
	RECT			m_TextureRect;
	RECTVERTEX 		m_Vertexs[4];


	DWORD			m_VertexColor;

	DECLARE_CLASS_INFO(CD3DFontCharClip)

public:
	CD3DFontCharClip(void);
	virtual ~CD3DFontCharClip(void);	

	virtual void SetRect(RECT& rect);
	virtual const RECT& GetRect()
	{return m_Rect;}

	virtual void SetTexture(CD3DTexture * pTexture,RECT * pTextureRect);	
	virtual const RECT& GetTextureRect()
	{return m_TextureRect;}

	virtual CD3DTexture * GetTexture()
	{return m_SubMesh.GetMaterial().GetTexture(0);}

	virtual void SetVertexColor(DWORD Color);
	DWORD GetVertexColor()
	{return m_VertexColor;}

	virtual bool MoveToTop();	
	virtual bool MoveToTop(CD3DObject *before);

	inline void SetFX(CD3DFX * pFX)
	{		
		m_SubMesh.GetMaterial().SetFX(pFX);
	}
public:
	virtual int GetSubMeshCount()
	{
		return 1;
	}
	virtual CD3DSubMesh * GetSubMesh(UINT index)
	{
		return &m_SubMesh;
	}	

	void CreateVertex();
};

}