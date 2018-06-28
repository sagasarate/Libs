/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DGUITextRect.cpp                                       */
/*      创建日期:  2009年10月20日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"
#include "D3DGUITextRect.h"
namespace D3DGUI{



	

IMPLEMENT_CLASS_INFO(CD3DGUITextRect,CD3DObject);

CD3DGUITextRect::CD3DGUITextRect():
	CD3DObject()
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
		
	
	m_pTexture=NULL;
	m_Scale=1.0f;

	Restore();
	CreateVertex();
}

CD3DGUITextRect::CD3DGUITextRect(FLOAT_RECT& Rect):
	CD3DObject(),
	m_Rect(Rect)
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
	


	m_pTexture=NULL;
	m_Scale=1.0f;
	
	
	Restore();
	CreateVertex();
}

CD3DGUITextRect::~CD3DGUITextRect(void)
{
	m_pTexture=NULL;	
}

void CD3DGUITextRect::SetRender(CD3DBaseRender * pRender)
{
	m_pRender=pRender;
	if(m_pRender)
	{
		SetDevice(m_pRender->GetDevice());
		m_pTexture=new CD3DTextTexture(GetDevice()->GetTextureManager());//->CreateTextTexture("[TextTexture]",NULL,(int)m_Rect.Width(),(int)m_Rect.Height(),1,0xffffffff);
		if(!m_pTexture->Create(NULL,(int)m_Rect.Width(),(int)m_Rect.Height(),1,0xffffffff))
		{
			PrintD3DLog(_T("CD3DGUITextRect::SetRender:创建文字纹理失败"));
		}
		if(!m_SubMesh.GetMaterial().SetTexture(0,m_pTexture))
			m_SubMesh.GetMaterial().AddTexture(m_pTexture,0,"","");
		if(m_SubMesh.GetMaterial().GetFX()==NULL)
			SetFXFromMemory(_T("TEXT_RECT_FX"),(LPVOID)TEXT_RECT_FX,(int)strlen(TEXT_RECT_FX));
		CreateVertex();
	}
}

void CD3DGUITextRect::SetPos(FLOAT X,FLOAT Y)
{
	if(m_Rect.left!=X||m_Rect.top!=Y)
	{
		m_Rect.SetPos(X,Y);
		CreateVertex();
	}

}
FLOAT CD3DGUITextRect::GetXPos()
{
	return m_Rect.left;
}
FLOAT CD3DGUITextRect::GetYPos()
{
	return m_Rect.top;
}

void CD3DGUITextRect::SetSize(FLOAT Width,FLOAT Height)
{
	if(m_Rect.Width()!=Width||m_Rect.Height()!=Height)
	{
		m_Rect.SetSize(Width,Height);		
		m_pTexture->SetSize((int)(Width/m_Scale),(int)(Height/m_Scale));
		CreateVertex();
	}		
}

FLOAT CD3DGUITextRect::GetWidth()
{
	return m_Rect.Width();
}

FLOAT CD3DGUITextRect::GetHeight()
{
	return m_Rect.Height();
}

void CD3DGUITextRect::SetVisible(bool IsVisible)
{
	CD3DObject::SetVisible(IsVisible);
}

bool CD3DGUITextRect::IsVisible()
{
	return CD3DObject::IsVisible();
}

void CD3DGUITextRect::SetRect(FLOAT_RECT * pRect)
{
	if(m_Rect!=*pRect)
	{	
		m_Rect=*pRect;
		m_pTexture->SetSize((int)(m_Rect.Width()/m_Scale),(int)(m_Rect.Height()/m_Scale));
		CreateVertex();
	}
}
FLOAT_RECT CD3DGUITextRect::GetRect()
{
	return m_Rect;
}

bool CD3DGUITextRect::SetFont(const LOGFONT* pLogFont)
{
	return m_pTexture->SetFont(pLogFont);
}
void CD3DGUITextRect::SetColor(DWORD Color)
{
	m_pTexture->SetColor(Color);
}
void CD3DGUITextRect::SetTextW(LPCWSTR szText,int StrLen)
{
	m_pTexture->SetTextW(szText,StrLen);	
}
void CD3DGUITextRect::SetAlign(DWORD dwAlign)
{
	m_pTexture->SetAlign(dwAlign);
}
void CD3DGUITextRect::SetShadowMode(DWORD ShadowMode)
{
	m_pTexture->SetShadowMode(ShadowMode);
}
void CD3DGUITextRect::SetShadowColor(DWORD ShadowColor)
{
	m_pTexture->SetShadowColor(ShadowColor);
}
void CD3DGUITextRect::SetShadowWidth(int Width)
{
	m_pTexture->SetShadowWidth(Width);
}
void CD3DGUITextRect::SetBKColor(DWORD Color)
{
	m_pTexture->SetBKColor(Color);
}
void CD3DGUITextRect::SetCharSpace(int Space)
{
	m_pTexture->SetCharSpace(Space);
}
void CD3DGUITextRect::SetLineSpace(int Space)
{
	m_pTexture->SetLineSpace(Space);
}
void CD3DGUITextRect::SetScale(FLOAT Scale)
{
	if(m_Scale!=Scale)
	{
		m_Scale=Scale;
		m_pTexture->SetSize((int)(m_Rect.Width()/m_Scale),(int)(m_Rect.Height()/m_Scale));
	}
}

void CD3DGUITextRect::EnableUpdate(bool AllowUpdate)
{
	m_pTexture->EnableUpdate(AllowUpdate);
}

LOGFONT* CD3DGUITextRect::GetFont()
{
	return m_pTexture->GetFont();
}
DWORD CD3DGUITextRect::GetColor()
{
	return m_pTexture->GetColor();
}
void CD3DGUITextRect::GetText(CEasyString& Text)
{
	m_pTexture->GetText(Text);
}
DWORD CD3DGUITextRect::GetAlign()
{
	return m_pTexture->GetAlign();
}
DWORD CD3DGUITextRect::GetShadowMode()
{
	return m_pTexture->GetShadowMode();
}
DWORD CD3DGUITextRect::GetShadowColor()
{
	return m_pTexture->GetShadowColor();
}
int CD3DGUITextRect::GetShadowWidth()
{
	return m_pTexture->GetShadowWidth();
}
DWORD CD3DGUITextRect::GetBKColor()
{
	return m_pTexture->GetBKColor();
}
int CD3DGUITextRect::GetCharSpace()
{
	return m_pTexture->GetCharSpace();
}
int CD3DGUITextRect::GetLineSpace()
{
	return m_pTexture->GetLineSpace();
}

FLOAT CD3DGUITextRect::GetScale()
{
	return m_Scale;
}

bool CD3DGUITextRect::GetTextSizeW(LPCWSTR pText,int StrLen,FLOAT& Width,FLOAT& Height,LPINT pCharWidths)
{
	if(m_pTexture)
	{
		if(m_pTexture->GetD3DFont())
		{
			SIZE Size;
			m_pTexture->GetD3DFont()->GetTextSizeW(pText,StrLen,&Size,pCharWidths);
			Width=(FLOAT)Size.cx*m_Scale;
			Height=(FLOAT)Size.cy*m_Scale;
			for(UINT i=0;i<StrLen;i++)
			{
				pCharWidths[i]=(int)(pCharWidths[i]*m_Scale);
			}
		}
	}
	return false;
}

bool CD3DGUITextRect::TranslateTextW(LPCWSTR pSrcText,int StrLen,LPWSTR pDestText,int& BufferSize)
{
	if(m_pTexture)
	{
		if(m_pTexture->GetD3DFont())
		{
			m_pTexture->GetD3DFont()->TranslateTextW(pSrcText,StrLen,pDestText,BufferSize);
		}
	}
	return false;
}

bool CD3DGUITextRect::SetEffectMode(int Mode)
{
	return false;
}

bool CD3DGUITextRect::SetFX(LPCTSTR FXFileName)
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

bool CD3DGUITextRect::SetFXFromMemory(LPCTSTR FXName,LPVOID FXContent,int Size)
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

CEasyString CD3DGUITextRect::GetFX()
{
	if(m_SubMesh.GetMaterial().GetFX())
		return m_SubMesh.GetMaterial().GetFX()->GetName();
	return "";
}

void CD3DGUITextRect::TopTo(IUIBaseRect* pBeforeRect)
{
	((CD3DUIRender *)GetRender())->MoveToTop(this,dynamic_cast<CD3DObject *>(pBeforeRect));
}

void CD3DGUITextRect::TopTo(IUIBaseRect** pRects,UINT RectCount,IUIBaseRect* pBeforeRect)
{
	((CD3DUIRender *)GetRender())->MoveToTop((CD3DObject **)pRects,RectCount,dynamic_cast<CD3DObject *>(pBeforeRect));
}

void  CD3DGUITextRect::Release()
{
	CD3DObject::Release();
}

void CD3DGUITextRect::OnPrepareRender(CD3DBaseRender * pRender,CD3DFX * pFX,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera)
{

}
void CD3DGUITextRect::OnPrepareRenderSubMesh(CD3DBaseRender * pRender,CD3DFX * pFX,CD3DSubMesh * pSubMesh,CD3DSubMeshMaterial * pMaterial,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera)
{
	//设置纹理
				
	pFX->SetTexture("TexLay0",pMaterial->GetTexture(0));
	//pFX->SetTexture("TexLay1",pMaterial->GetTexture(1));		
}



void CD3DGUITextRect::Update(FLOAT Time)
{
	////更新子对象
	//for(UINT i=0;i<GetChildCount();i++)
	//	GetChildByIndex(i)->Update(Time);
}

void CD3DGUITextRect::CreateVertex()
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
		tx1=0;
		ty1=0;
		tx2=m_SubMesh.GetMaterial().GetTexture(0)->GetUS();
		ty2=m_SubMesh.GetMaterial().GetTexture(0)->GetVS();
	}
	else
	{
		tx1=0;tx2=0;
		ty1=0;ty2=0;
	}	
	m_Vertexs[0] = InitRECTVertex( D3DXVECTOR4(x1,y1,0.9f,1.0f), 0xffffffff, tx1, ty1 );
	m_Vertexs[1] = InitRECTVertex( D3DXVECTOR4(x2,y1,0.9f,1.0f), 0xffffffff, tx2, ty1 );
	m_Vertexs[2] = InitRECTVertex( D3DXVECTOR4(x1,y2,0.9f,1.0f), 0xffffffff, tx1, ty2 );
	m_Vertexs[3] = InitRECTVertex( D3DXVECTOR4(x2,y2,0.9f,1.0f), 0xffffffff, tx2, ty2 );
	m_IsRenderDataChanged=true;

}

}