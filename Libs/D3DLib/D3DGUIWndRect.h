/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DGUIWndRect.h                                          */
/*      创建日期:  2009年10月20日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace D3DGUI{

class CD3DGUIWndRect :
	public CD3DObject,public IUIWndRect
{
protected:
	CD3DSubMesh		m_SubMesh;	
	FLOAT_RECT		m_Rect;
	FLOAT_RECT		m_TextureUV;
	RECTVERTEX 		m_Vertexs[4];


	DWORD			m_VertexColor;

	DECLARE_CLASS_INFO(CD3DGUIWndRect)

public:
	CD3DGUIWndRect();
	CD3DGUIWndRect(FLOAT_RECT& Rect);
	virtual ~CD3DGUIWndRect(void);

	virtual void SetRender(CD3DBaseRender * pRender);

	virtual void SetPos(FLOAT X,FLOAT Y);
	virtual FLOAT GetXPos();
	virtual FLOAT GetYPos();

	virtual void SetSize(FLOAT Width,FLOAT Height);
	virtual FLOAT GetWidth();
	virtual FLOAT GetHeight();

	virtual void SetVisible(bool IsVisible);
	virtual bool IsVisible();

	virtual void SetRect(FLOAT_RECT * pRect);
	virtual FLOAT_RECT GetRect();

	virtual void SetColor(DWORD Color);
	virtual DWORD GetColor();

	virtual void SetTexture(IUITexture * pTexture);
	virtual IUITexture * GetTexture();

	virtual void SetUV(FLOAT_RECT * pUVRect);
	virtual FLOAT_RECT GetUV();

	virtual bool SetEffectMode(int Mode);

	virtual bool SetFX(LPCTSTR FXFileName);
	virtual bool SetFXFromMemory(LPCTSTR FXName,LPVOID FXContent,int Size);
	virtual CEasyString GetFX();

	virtual void TopTo(IUIBaseRect* pBeforeRect);
	virtual void TopTo(IUIBaseRect** pRects,UINT RectCount,IUIBaseRect* pBeforeRect);

	virtual void Release();
	

public:

	virtual void OnPrepareRender(CD3DBaseRender * pRender,CD3DFX * pFX,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera);
	virtual void OnPrepareRenderSubMesh(CD3DBaseRender * pRender,CD3DFX * pFX,CD3DSubMesh * pSubMesh,CD3DSubMeshMaterial * pMaterial,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera);
	virtual void Update(FLOAT Time);
	virtual int GetSubMeshCount()
	{
		return 1;
	}
	virtual CD3DSubMesh * GetSubMesh(UINT index)
	{
		return &m_SubMesh;
	}	
	


protected:
	void CreateVertex();

};

}