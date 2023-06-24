/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DGUIWndRect.cpp                                        */
/*      创建日期:  2009年10月20日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"
#include "D3DGUIWndRect.h"

namespace D3DGUI{



IMPLEMENT_CLASS_INFO_STATIC(CD3DGUIWndRect,CD3DObject);

CD3DGUIWndRect::CD3DGUIWndRect():
	CD3DObject(),	
	m_TextureUV(0.0f,0.0f,100.0f,100.0f)
{	
	m_SubMesh.GetVertexFormat().FVF=D3DFVF_RECTVERTEX;
	m_SubMesh.GetVertexFormat().VertexSize=sizeof(RECTVERTEX);
	m_SubMesh.GetVertexFormat().IndexSize=sizeof(WORD);
	m_SubMesh.SetVertexCount(4);
	m_SubMesh.SetPrimitiveType(D3DPT_TRIANGLESTRIP);
	m_SubMesh.SetPrimitiveCount(2);	
	m_SubMesh.SetVertices((BYTE *)m_Vertexs);
	m_SubMesh.AllocVertexBufferR();
	m_SubMesh.SetRenderBufferUsed(CD3DSubMesh::BUFFER_USE_CUSTOM);
	
	m_VertexColor=D3DCOLOR_XRGB(255, 255, 255);
	Restore();
	CreateVertex();
}

CD3DGUIWndRect::CD3DGUIWndRect(FLOAT_RECT& Rect):
	CD3DObject(),
	m_Rect(Rect),
	m_TextureUV(0.0f,0.0f,100.0f,100.0f)
{	
	m_SubMesh.GetVertexFormat().FVF=D3DFVF_RECTVERTEX;
	m_SubMesh.GetVertexFormat().VertexSize=sizeof(RECTVERTEX);
	m_SubMesh.GetVertexFormat().IndexSize=sizeof(WORD);
	m_SubMesh.SetVertexCount(4);
	m_SubMesh.SetPrimitiveType(D3DPT_TRIANGLESTRIP);
	m_SubMesh.SetPrimitiveCount(2);	
	m_SubMesh.SetVertices((BYTE *)m_Vertexs);
	m_SubMesh.AllocVertexBufferR();
	m_SubMesh.SetRenderBufferUsed(CD3DSubMesh::BUFFER_USE_CUSTOM);
	


	m_VertexColor=D3DCOLOR_XRGB(255, 255, 255);
	Restore();
	CreateVertex();
}

CD3DGUIWndRect::~CD3DGUIWndRect(void)
{
	//m_SubMesh.Destory();
}

void CD3DGUIWndRect::SetRender(CD3DBaseRender * pRender)
{
	m_pRender=pRender;
	if(m_pRender)
	{
		SetDevice(m_pRender->GetDevice());
		if(m_SubMesh.GetMaterial().GetFX()==NULL)
			SetFXFromMemory(_T("DEFAULT_UI_WIN_RECT_FX"),(LPVOID)DEFAULT_UI_WIN_RECT_FX,(int)strlen(DEFAULT_UI_WIN_RECT_FX));
	}
}

void CD3DGUIWndRect::SetPos(FLOAT X,FLOAT Y)
{
	if(m_Rect.left!=X||m_Rect.top!=Y)
	{
		m_Rect.SetPos(X,Y);
		CreateVertex();
	}

}
FLOAT CD3DGUIWndRect::GetXPos()
{
	return m_Rect.left;
}
FLOAT CD3DGUIWndRect::GetYPos()
{
	return m_Rect.top;
}

void CD3DGUIWndRect::SetSize(FLOAT Width,FLOAT Height)
{
	if(m_Rect.Width()!=Width||m_Rect.Height()!=Height)
	{
		m_Rect.SetSize(Width,Height);
		CreateVertex();
	}		
}

FLOAT CD3DGUIWndRect::GetWidth()
{
	return m_Rect.Width();
}

FLOAT CD3DGUIWndRect::GetHeight()
{
	return m_Rect.Height();
}

void CD3DGUIWndRect::SetVisible(bool IsVisible)
{
	CD3DObject::SetVisible(IsVisible);
}

bool CD3DGUIWndRect::IsVisible()
{
	return CD3DObject::IsVisible();
}

void CD3DGUIWndRect::SetRect(FLOAT_RECT * pRect)
{
	if(m_Rect!=*pRect)
	{	
		m_Rect=*pRect;
		CreateVertex();
	}
}
FLOAT_RECT CD3DGUIWndRect::GetRect()
{
	return m_Rect;
}

void CD3DGUIWndRect::SetColor(DWORD Color)
{
	if(m_VertexColor!=Color)
	{	
		m_VertexColor=Color;
		CreateVertex();
	}
}

DWORD CD3DGUIWndRect::GetColor()
{
	return m_VertexColor;
}

void CD3DGUIWndRect::SetTexture(IUITexture * pTexture)
{	
	D3DLOCK_FOR_OBJECT_MODIFY
	CD3DTexture * pTex=dynamic_cast<CD3DTexture *>(pTexture);
	if(!m_SubMesh.GetMaterial().SetTexture(0,pTex))
		m_SubMesh.GetMaterial().AddTexture(pTex,0,"","");
	if(pTex)
		pTex->AddUseRef();
	if(m_SubMesh.GetMaterial().GetFX()==NULL)
		SetFXFromMemory(_T("DEFAULT_UI_WIN_RECT_FX"),(LPVOID)DEFAULT_UI_WIN_RECT_FX,(int)strlen(DEFAULT_UI_WIN_RECT_FX));
}

IUITexture * CD3DGUIWndRect::GetTexture()
{
	return dynamic_cast<IUITexture *>(m_SubMesh.GetMaterial().GetTexture(0));
}

void CD3DGUIWndRect::SetUV(FLOAT_RECT * pUVRect)
{
	if(m_TextureUV!=*pUVRect)
	{
		m_TextureUV=*pUVRect;
		CreateVertex();
	}
}
FLOAT_RECT CD3DGUIWndRect::GetUV()
{
	return m_TextureUV;
}

bool CD3DGUIWndRect::SetEffectMode(int Mode)
{
	switch(Mode)
	{
	case D3DGUI_EFFECT_INVERSE_COLOR:
		return SetFXFromMemory(_T("INVERSE_COLOR_FX"),(LPVOID)INVERSE_COLOR_FX,(int)strlen(INVERSE_COLOR_FX));
	case D3DGUI_EFFECT_TEXTURE_FILTER:
		return SetFXFromMemory(_T("DEFAULT_UI_WIN_RECT_FX_TF"),(LPVOID)DEFAULT_UI_WIN_RECT_FX_TF,(int)strlen(DEFAULT_UI_WIN_RECT_FX_TF));
	}
	
	return false;
}

bool CD3DGUIWndRect::SetFX(LPCTSTR FXFileName)
{
	D3DLOCK_FOR_OBJECT_MODIFY
	CD3DFX * pFX=GetDevice()->GetFXManager()->LoadFX(FXFileName);
	if(pFX)
	{		
		m_SubMesh.GetMaterial().SetFX(pFX);
		return true;
	}
	return false;
}

bool CD3DGUIWndRect::SetFXFromMemory(LPCTSTR FXName,LPVOID FXContent,int Size)
{
	D3DLOCK_FOR_OBJECT_MODIFY
	CD3DFX * pFX=GetDevice()->GetFXManager()->LoadFXFromMemory(FXName,(void *)FXContent,Size);
	if(pFX)
	{
		m_SubMesh.GetMaterial().SetFX(pFX);
		return true;
	}
	return false;
}

CEasyString CD3DGUIWndRect::GetFX()
{
	if(m_SubMesh.GetMaterial().GetFX())
		return m_SubMesh.GetMaterial().GetFX()->GetName();
	return "";
}

void CD3DGUIWndRect::TopTo(IUIBaseRect* pBeforeRect)
{
	((CD3DUIRender *)GetRender())->MoveToTop(this,dynamic_cast<CD3DObject *>(pBeforeRect));
}

void CD3DGUIWndRect::TopTo(IUIBaseRect** pRects,UINT RectCount,IUIBaseRect* pBeforeRect)
{
	CEasyArray<CD3DObject *> ObjectList(64,64);
	for(UINT i=0;i<RectCount;i++)
	{
		ObjectList.Add(dynamic_cast<CD3DObject *>(pRects[i]));
	}
	((CD3DUIRender *)GetRender())->MoveToTop(ObjectList.GetBuffer(),RectCount,dynamic_cast<CD3DObject *>(pBeforeRect));
}

void  CD3DGUIWndRect::Release()
{
	CD3DObject::Release();
}

void CD3DGUIWndRect::OnPrepareRender(CD3DBaseRender * pRender,CD3DFX * pFX,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera)
{

}
void CD3DGUIWndRect::OnPrepareRenderSubMesh(CD3DBaseRender * pRender,CD3DFX * pFX,CD3DSubMesh * pSubMesh,CD3DSubMeshMaterial * pMaterial,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera)
{
	pFX->SetTexture("TexLay0",pMaterial->GetTexture(0));
	//pFX->SetTexture("TexLay1",pMaterial->GetTexture(1));
}


void CD3DGUIWndRect::Update(FLOAT Time)
{
	////更新子对象
	//for(UINT i=0;i<GetChildCount();i++)
	//	GetChildByIndex(i)->Update(Time);
}

void CD3DGUIWndRect::CreateVertex()
{
	//D3DLOCK_FOR_OBJECT_MODIFY

	FLOAT x1,y1,tx1,ty1;
	FLOAT x2,y2,tx2,ty2;		

	x1=(FLOAT)m_Rect.left;
	y1=(FLOAT)m_Rect.top;	
	x2=(FLOAT)m_Rect.right;
	y2=(FLOAT)m_Rect.bottom;
	if(m_SubMesh.GetMaterial().GetTexture(0))
	{
		tx1=(FLOAT)m_TextureUV.left;
		ty1=(FLOAT)m_TextureUV.top;
		tx2=(FLOAT)m_TextureUV.right;
		ty2=(FLOAT)m_TextureUV.bottom;
	}
	else
	{
		tx1=0;tx2=0;
		ty1=0;ty2=0;
	}
	m_Vertexs[0] = InitRECTVertex( D3DXVECTOR4(x1,y1,0.9f,1.0f), m_VertexColor, tx1, ty1 );
	m_Vertexs[1] = InitRECTVertex( D3DXVECTOR4(x2,y1,0.9f,1.0f), m_VertexColor, tx2, ty1 );
	m_Vertexs[2] = InitRECTVertex( D3DXVECTOR4(x1,y2,0.9f,1.0f), m_VertexColor, tx1, ty2 );
	m_Vertexs[3] = InitRECTVertex( D3DXVECTOR4(x2,y2,0.9f,1.0f), m_VertexColor, tx2, ty2 );

	m_IsRenderDataChanged=true;
}

}