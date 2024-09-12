/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DDX9Font.cpp                                           */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

namespace D3DLib{

IMPLEMENT_CLASS_INFO(CD3DDX9Font,CD3DBaseFont);

CD3DDX9Font::CD3DDX9Font():CD3DBaseFont()
{
	m_pD3DFont=NULL;	
	ZeroMemory(&m_LogFont,sizeof(m_LogFont));
}

CD3DDX9Font::CD3DDX9Font(CD3DFontManager * pManager):CD3DBaseFont(pManager)
{
	m_pD3DFont=NULL;	
	ZeroMemory(&m_LogFont,sizeof(m_LogFont));
}

CD3DDX9Font::~CD3DDX9Font(void)
{
	SAFE_RELEASE(m_pD3DFont);
}

bool CD3DDX9Font::Reset()
{
	SAFE_RELEASE(m_pD3DFont);
	return true;
}

bool CD3DDX9Font::Restore()
{
	return Create(&m_LogFont,m_MipLevels);
}

bool CD3DDX9Font::Create(const LOGFONT * pLogFont,int MipLevels)
{
	D3DXFONT_DESC FontDesc;

	m_LogFont=*pLogFont;
	m_MipLevels=MipLevels;

	SAFE_RELEASE(m_pD3DFont);
	strncpy_0(FontDesc.FaceName,LF_FACESIZE,pLogFont->lfFaceName,LF_FACESIZE);
	FontDesc.Width=pLogFont->lfWidth;
	FontDesc.Height=pLogFont->lfHeight;
	FontDesc.MipLevels=MipLevels;
	FontDesc.Weight=pLogFont->lfWeight;
	FontDesc.CharSet=pLogFont->lfCharSet;
	FontDesc.Italic=pLogFont->lfItalic;
	FontDesc.OutputPrecision=pLogFont->lfOutPrecision;
	FontDesc.PitchAndFamily=pLogFont->lfPitchAndFamily;
	FontDesc.Quality=pLogFont->lfQuality;

	HRESULT hr=D3DXCreateFontIndirect(m_pManager->GetDevice()->GetD3DDevice(),
		&FontDesc,
		&m_pD3DFont);
	if(FAILED(hr))
		return false;

	return true;
}

bool CD3DDX9Font::DrawTextA(LPCSTR pText,int TextLen,RECT * pRect,DWORD Align,D3DCOLOR Color)
{
	return SUCCEEDED(m_pD3DFont->DrawTextA(NULL,pText,TextLen,pRect,Align,Color));
}

bool CD3DDX9Font::DrawTextW(LPCWSTR pText,int TextLen,RECT * pRect,DWORD Align,D3DCOLOR Color)
{
	return SUCCEEDED(m_pD3DFont->DrawTextW(NULL,pText,TextLen,pRect,Align,Color));
}

bool CD3DDX9Font::GetTextSizeA(LPCSTR pText,int TextLen,LPSIZE pSize,LPINT pCharWidths)
{
	HDC hdc=GetDC();
	if(hdc)
	{		
		GetTextExtentExPointA(hdc,pText,TextLen,NULL,NULL,pCharWidths,pSize);
		return true;
	}
	return false;
}

bool CD3DDX9Font::GetTextSizeW(LPCWSTR pText,int TextLen,LPSIZE pSize,LPINT pCharWidths)
{
	HDC hdc=GetDC();
	if(hdc)
	{		
		GetTextExtentExPointW(hdc,pText,TextLen,NULL,NULL,pCharWidths,pSize);
		return true;
	}
	return false;
}

bool CD3DDX9Font::TranslateTextA(LPCSTR pSrcText,int StrLen,LPTSTR pDestText,int& BufferSize)
{
	if(pDestText)
	{
		int OutLen=StrLen;
		if(OutLen>=BufferSize)
			OutLen=BufferSize-1;
		memcpy(pDestText,pSrcText,OutLen);
		pDestText[OutLen]=0;
		return true;
	}
	else
	{
		BufferSize=StrLen+1;
		return true;
	}
}
bool CD3DDX9Font::TranslateTextW(LPCWSTR pSrcText,int StrLen,LPWSTR pDestText,int& BufferSize)
{
	if(pDestText)
	{
		int OutLen=StrLen;
		if(OutLen>=BufferSize)
			OutLen=BufferSize-1;
		memcpy(pDestText,pSrcText,OutLen*sizeof(WCHAR));
		pDestText[OutLen]=0;
		return true;
	}
	else
	{
		BufferSize=StrLen+1;
		return true;
	}
}

bool CD3DDX9Font::GetLogFont(LOGFONT * pLogFont)
{
	if(m_pD3DFont&&pLogFont)
	{
		D3DXFONT_DESC FontDesc;
		m_pD3DFont->GetDesc(&FontDesc);
		strncpy_0(pLogFont->lfFaceName,LF_FACESIZE,FontDesc.FaceName,LF_FACESIZE);
		pLogFont->lfWidth			=FontDesc.Width				;
		pLogFont->lfHeight			=FontDesc.Height			;
		pLogFont->lfWeight			=FontDesc.Weight			;
		pLogFont->lfCharSet			=FontDesc.CharSet			;
		pLogFont->lfItalic			=FontDesc.Italic			;
		pLogFont->lfOutPrecision	=FontDesc.OutputPrecision	;
		pLogFont->lfPitchAndFamily	=FontDesc.PitchAndFamily	;
		pLogFont->lfQuality			=FontDesc.Quality			;
		return true;
	}
	return false;
}

HDC CD3DDX9Font::GetDC()
{
	if(m_pD3DFont)
		return m_pD3DFont->GetDC();
	else
		return NULL;
}

}