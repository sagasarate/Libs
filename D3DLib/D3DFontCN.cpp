/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DFontCN.cpp                                            */
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






IMPLEMENT_CLASS_INFO(CD3DFontCN,CD3DBaseFont);

CD3DFontCN::CD3DFontCN():CD3DBaseFont()
{
	ZeroMemory(&m_LogFont,sizeof(m_LogFont));
	m_MipLevels=1;
	m_FontDC=NULL;
	m_GDIFont=NULL;
	m_FontFlag=0;
	ZeroMemory(m_CharMap,sizeof(m_CharMap));
	m_pCharInfoBuff=NULL;
	m_pCharTextureBuff=NULL;
	m_TextureFormat=D3DFMT_A8;
	m_CharBuffSize=0;
	m_CharBuffWritePtr=0;
	m_CharSpace=0;
	m_LineSpace=0;		
}

CD3DFontCN::CD3DFontCN(CD3DFontManager * pManager):CD3DBaseFont(pManager)
{
	ZeroMemory(&m_LogFont,sizeof(m_LogFont));
	m_MipLevels=1;
	m_FontDC=NULL;
	m_GDIFont=NULL;
	m_FontFlag=0;
	ZeroMemory(m_CharMap,sizeof(m_CharMap));
	m_pCharInfoBuff=NULL;
	m_pCharTextureBuff=NULL;
	m_TextureFormat=D3DFMT_A8;
	m_CharBuffSize=0;
	m_CharBuffWritePtr=0;
	m_CharSpace=0;
	m_LineSpace=0;		
}


CD3DFontCN::~CD3DFontCN()
{
	DeleteObject(m_CharBmp);
	DeleteObject(m_GDIFont);
	DeleteDC(m_FontDC);
	SAFE_DELETE_ARRAY(m_pCharInfoBuff);
	SAFE_RELEASE(m_pCharTextureBuff);	
}

bool CD3DFontCN::Reset()
{
	//DeleteObject(m_CharBmp);
	//DeleteObject(m_GDIFont);
	//DeleteDC(m_FontDC);
	//SAFE_DELETE_ARRAY(m_pCharInfoBuff);
	//SAFE_RELEASE(m_pCharTextureBuff);	
	return m_Render.Reset();	
}
bool CD3DFontCN::Restore()
{
	//return Create(&m_LogFont,m_MipLevels,m_CharBuffSize);
	return m_Render.Restore();	
}


bool CD3DFontCN::Create(const LOGFONT * pLogFont,int MipLevels,int BufferSize)
{
	HRESULT hr=S_OK;
	if(pLogFont==NULL)
		return false;

	m_LogFont=*pLogFont;
	m_MipLevels=MipLevels;
	m_CharBuffSize=BufferSize;

	//初始化映射表
	memset(m_CharMap,0xff,sizeof(m_CharMap));
	
	//获取设备支持的最大纹理大小
	D3DCAPS9 D3DCaps;
	hr=m_pManager->GetDevice()->GetD3DDevice()->GetDeviceCaps( &D3DCaps );
	if(FAILED(hr))
		return false;

	//决定缓冲纹理大小
	int HCount,VCount,Width,Height;
	HCount=(int)sqrt((FLOAT)m_CharBuffSize);
	m_CharHeight=m_LogFont.lfHeight;
	if(m_LogFont.lfWidth>m_LogFont.lfHeight)
		m_CharWidth=m_LogFont.lfWidth;
	else
		m_CharWidth=m_LogFont.lfHeight;
	Width=HCount*m_CharWidth;
	int Bits=0;
	while(Width)
	{
		Width=Width>>1;
		Bits++;
	}
	Width=1<<(Bits-1);
	if(Width>(int)D3DCaps.MaxTextureWidth)
		Width=(int)D3DCaps.MaxTextureWidth;
	HCount=(int)floor((FLOAT)Width/m_CharWidth);
	VCount=(int)ceil((FLOAT)m_CharBuffSize/HCount);
	Height=VCount*m_CharHeight;
	Bits=0;
	while(Height)
	{
		Height=Height>>1;
		Bits++;
	}
	Height=1<<(Bits-1);
	if(Height>(int)D3DCaps.MaxTextureHeight)
		Height=(int)D3DCaps.MaxTextureHeight;
	VCount=(int)floor((FLOAT)Height/m_CharHeight);

	m_CharBuffSize=HCount*VCount;

	//创建信息缓冲	
	m_pCharInfoBuff=new FONT_CHAR_INFO[m_CharBuffSize];
	m_CharBuffWritePtr=0;

	//检查支持的纹理格式
	if(m_pManager->GetDevice()->CheckDeviceFormat(D3DFMT_A8))
		m_TextureFormat=D3DFMT_A8;
	else
		m_TextureFormat=D3DFMT_A8R8G8B8;
	
	//创建纹理

	m_pCharTextureBuff=new CD3DTexture(m_pManager->GetDevice()->GetTextureManager());
	if(m_pCharTextureBuff==NULL)
	{
		SAFE_DELETE_ARRAY(m_pCharInfoBuff);
		return false;
	}
	if(!m_pCharTextureBuff->CreateTexture(Width,Height,m_TextureFormat,0,D3DPOOL_MANAGED,MipLevels))
	{
		SAFE_DELETE_ARRAY(m_pCharInfoBuff);
		SAFE_RELEASE(m_pCharTextureBuff);
		return false;
	}

	//初始化纹理坐标
	for(int i=0;i<m_CharBuffSize;i++)
	{
		m_pCharInfoBuff[i].Pos.x=(i%HCount)*m_CharWidth;
		m_pCharInfoBuff[i].Pos.y=(i/HCount)*m_CharHeight;
		m_pCharInfoBuff[i].UV.left=(FLOAT)(m_pCharInfoBuff[i].Pos.x)/m_pCharTextureBuff->GetWidth();
		m_pCharInfoBuff[i].UV.top=(FLOAT)(m_pCharInfoBuff[i].Pos.y)/m_pCharTextureBuff->GetHeight();
	}

	//创建DC
	m_FontDC=CreateCompatibleDC(NULL);
	if(m_FontDC==NULL)
	{
		SAFE_DELETE_ARRAY(m_pCharInfoBuff);
		SAFE_RELEASE(m_pCharTextureBuff);
		return false;
	}
	SetMapMode( m_FontDC, MM_TEXT );
	//创建GDI字体
	m_GDIFont=CreateFontIndirect(&m_LogFont);
	if(m_GDIFont==NULL)
	{
		DeleteDC(m_FontDC);
		m_FontDC=NULL;
		SAFE_DELETE_ARRAY(m_pCharInfoBuff);
		SAFE_RELEASE(m_pCharTextureBuff);
		return false;
	}

	HGDIOBJ OldFont=SelectObject(m_FontDC,m_GDIFont);
	DeleteObject(OldFont);

	//创建容纳字符的BMP	
	BITMAPINFO bmi;
	ZeroMemory( &bmi.bmiHeader, sizeof(BITMAPINFOHEADER) );
	bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth       =  m_CharWidth;
	bmi.bmiHeader.biHeight      =  m_CharHeight;
	bmi.bmiHeader.biPlanes      = 1;
	bmi.bmiHeader.biCompression = 0;
	bmi.bmiHeader.biBitCount    = 32;


	m_CharBmp = CreateDIBSection( GetDC(), &bmi, DIB_RGB_COLORS,
		(void**)&m_pBitmapBits, NULL, 0 );

	if(m_CharBmp==NULL)
	{
		DeleteObject(m_GDIFont);
		m_GDIFont=NULL;
		DeleteDC(m_FontDC);
		m_FontDC=NULL;
		SAFE_DELETE_ARRAY(m_pCharInfoBuff);
		SAFE_RELEASE(m_pCharTextureBuff);
		return false;
	}

	HGDIOBJ hbmOld = SelectObject( m_FontDC, m_CharBmp );
	DeleteObject(hbmOld);
	SetTextColor(m_FontDC,RGB(255,255,255));
	SetBkColor(m_FontDC,0x00000000);
	SetTextAlign( m_FontDC,TA_TOP);

	//建立渲染器
	if(!m_Render.Create(m_pManager->GetDevice()))
	{
		DeleteObject(m_CharBmp);
		DeleteObject(m_GDIFont);
		m_GDIFont=NULL;
		DeleteDC(m_FontDC);
		m_FontDC=NULL;
		SAFE_DELETE_ARRAY(m_pCharInfoBuff);
		SAFE_RELEASE(m_pCharTextureBuff);
		return false;
	}
	m_DrawClip.SetRender(&m_Render);
	m_DrawClip.SetFX(m_pManager->GetDevice()->GetFXManager()->LoadFXFromMemory(_T("FontFX"),(void *)FONT_FX,(int)strlen(FONT_FX)));
	
	return true;
}

bool CD3DFontCN::DrawTextA(LPCSTR pText,int TextLen,RECT * pRect,DWORD Align,D3DCOLOR Color)
{
	CEasyStringW Text(pText,TextLen);	
	return DrawTextW((LPCWSTR)Text,(int)Text.GetLength(),pRect,Align,Color);
}

bool CD3DFontCN::DrawTextW(LPCWSTR pText,int TextLen,RECT * pRect,DWORD Align,D3DCOLOR Color)
{
	int x,y;
	CTextLineList LineInfos;

	if(pRect==NULL)
		return false;

	ParseLineInfo(pText,TextLen,pRect,Align,LineInfos);


	x=pRect->left;
	

	if(Align&D3DFONT_VCENTER)
	{
		y=pRect->top+
			(
			(pRect->bottom-pRect->top)-
			((int)LineInfos.GetCount()*m_CharHeight+((int)LineInfos.GetCount()-1)*m_LineSpace)
			)/2;
	}
	else if(Align&D3DFONT_BOTTOM)
	{
		y=pRect->bottom-((int)LineInfos.GetCount()*m_CharHeight+((int)LineInfos.GetCount()-1)*m_LineSpace);
	}
	else
		y=pRect->top;

	
	for(int i=0;i<(int)LineInfos.GetCount();i++)
	{
		DrawLine(x,y,pText+LineInfos[i].StartPos,LineInfos[i].Len,LineInfos[i].Width,pRect,LineInfos[i].Align,Color);
		y+=m_CharHeight+m_LineSpace;
	}
	
	return true;
}

bool CD3DFontCN::GetTextSizeA(LPCSTR pText,int TextLen,LPSIZE pSize,LPINT pCharWidths)
{
	CEasyStringW Text(pText,TextLen);	
	return GetTextSizeW((LPCWSTR)Text,(int)Text.GetLength(),pSize,pCharWidths);
}

bool CD3DFontCN::GetTextSizeW(LPCWSTR pText,int TextLen,LPSIZE pSize,LPINT pCharWidths)
{
	int Width=0;
	
	bool CanOut=true;
	int CommandStart;
	pSize->cy=m_CharHeight;
	pSize->cx=0;
	for(int i=0;i<TextLen;i++)
	{
		FONT_CHAR_INFO * pCharInfo=NULL;
		if(pText[i]=='<')
		{
			//处理命令
			CanOut=false;
		}
		else if(pText[i]=='>')
		{
			CanOut=true;
		}
		else if(pText[i]=='&')
		{
			CanOut=false;
			CommandStart=i;
		}
		else if((!CanOut)&&pText[i]==';')
		{
			CanOut=true;
			pCharInfo=GetCharInfo(StrToChar(pText+CommandStart,i-CommandStart+1));
		}
		else if(CanOut)
		{	
			pCharInfo=GetCharInfo(pText[i]);
		}		
		if(pCharInfo)
		{
			Width+=pCharInfo->Size.cx;
			if(pCharWidths)
				pCharWidths[i]=Width;		
			pSize->cx=Width;
			Width+=m_CharSpace;				
		}		
		else
		{
			if(pCharWidths)
			{
				if(i)
					pCharWidths[i]=pCharWidths[i-1];
				else
					pCharWidths[i]=0;
			}
		}
	}
	return true;
}

bool CD3DFontCN::TranslateTextA(LPCSTR pSrcText,int StrLen,LPTSTR pDestText,int& BufferSize)
{
	if(pDestText)
	{
		CEasyStringA Translator(pSrcText,StrLen);
		Translator.Replace("&","&amp;");
		Translator.Replace("<","&lt;");
		Translator.Replace(">","&gt;");
		int OutLen=(int)Translator.GetLength();
		if(OutLen>=BufferSize)
			OutLen=BufferSize-1;
		memcpy(pDestText,(LPCSTR)Translator,OutLen);
		pDestText[OutLen]=0;
		return true;

	}
	else
	{
		BufferSize=StrLen+1;
		for(int i=0;i<StrLen;i++)
		{
			if(pSrcText[i]=='&')
				BufferSize+=4;
			if(pSrcText[i]=='<')
				BufferSize+=3;
			if(pSrcText[i]=='>')
				BufferSize+=3;
		}
		return true;
	}
}

bool CD3DFontCN::TranslateTextW(LPCWSTR pSrcText,int StrLen,LPWSTR pDestText,int& BufferSize)
{
	if(pDestText)
	{
		CEasyStringW Translator(pSrcText,StrLen);
		Translator.Replace(CEasyStringW("&"),CEasyStringW("&amp;"));
		Translator.Replace(CEasyStringW("<"),CEasyStringW("&lt;"));
		Translator.Replace(CEasyStringW(">"),CEasyStringW("&gt;"));
		int OutLen=(int)Translator.GetLength();
		if(OutLen>=BufferSize)
			OutLen=BufferSize-1;
		memcpy(pDestText,(LPCWSTR)Translator,OutLen*sizeof(WCHAR));
		pDestText[OutLen]=0;
		return true;

	}
	else
	{
		BufferSize=StrLen+1;
		for(int i=0;i<StrLen;i++)
		{
			if(pSrcText[i]==CEasyStringW('&')[0])
				BufferSize+=4;
			if(pSrcText[i]==CEasyStringW('<')[0])
				BufferSize+=3;
			if(pSrcText[i]==CEasyStringW('>')[0])
				BufferSize+=3;
		}
		return true;
	}
}

////////////////////////////////////////////////////////

bool CD3DFontCN::AddChar(WCHAR Char)
{	
	SIZE CharSize;	
	

	//获取字符的大小
	GetTextExtentPoint32W( GetDC(), &Char, 1, &CharSize );
	//输出字符到BMP
	if(ExtTextOutW( GetDC(), 0, 0, ETO_OPAQUE, NULL, &Char, 1, NULL )==0)
	{		
		return false;
	}
	if(CharSize.cx>m_CharWidth)
		CharSize.cx=m_CharWidth;
	if(CharSize.cy>m_CharHeight)
		CharSize.cy=m_CharHeight;

	FONT_CHAR_INFO * pCharInfo=AllocCharInfo(Char);

	pCharInfo->Size=CharSize;
	pCharInfo->UV.right=(FLOAT)(pCharInfo->Pos.x+CharSize.cx)/m_pCharTextureBuff->GetWidth();
	pCharInfo->UV.bottom=(FLOAT)(pCharInfo->Pos.y+CharSize.cy)/m_pCharTextureBuff->GetHeight();
	

	LPDIRECT3DTEXTURE9 pTexture=m_pCharTextureBuff->GetD3DTexture();
	D3DLOCKED_RECT D3DLock;

	pTexture->LockRect( 0, &D3DLock, 0, 0 );

	BYTE* pDstRow;
	BYTE* pDst;		

	pDstRow = (BYTE*)D3DLock.pBits+pCharInfo->Pos.y*D3DLock.Pitch;	

	for(int y=0;y<(int)pCharInfo->Size.cy;y++)
	{
		if(m_TextureFormat==D3DFMT_A8)
			pDst = pDstRow+pCharInfo->Pos.x;
		else
			pDst = pDstRow+pCharInfo->Pos.x*4;

		for(int x=0;x<(int)pCharInfo->Size.cx;x++)
		{
			if(m_TextureFormat==D3DFMT_A8)
			{
				*pDst=(BYTE)(m_pBitmapBits[(m_CharHeight-y-1)*m_CharWidth+x]&0xff);
				pDst++;
			}
			else
			{
				pDst+=3;
				*pDst=(BYTE)(m_pBitmapBits[(m_CharHeight-y-1)*m_CharWidth+x]&0xff);
				pDst++;
			}
		}
		pDstRow += D3DLock.Pitch;
	}	
	pTexture->UnlockRect(0);
	
	return true;
}

CD3DFontCN::FONT_CHAR_INFO * CD3DFontCN::AllocCharInfo(WCHAR Char)
{
	FONT_CHAR_INFO * pInfo;
	if(m_pCharInfoBuff[m_CharBuffWritePtr].IsUsed)
	{
		m_CharMap[m_pCharInfoBuff[m_CharBuffWritePtr].CharCode]=0xffff;
	}
	m_pCharInfoBuff[m_CharBuffWritePtr].IsUsed=true;
	m_pCharInfoBuff[m_CharBuffWritePtr].CharCode=Char;
	m_CharMap[Char]=m_CharBuffWritePtr;
	pInfo=m_pCharInfoBuff+m_CharBuffWritePtr;
	m_CharBuffWritePtr++;
	if(m_CharBuffWritePtr>=m_CharBuffSize)
		m_CharBuffWritePtr=0;
	return pInfo;
}

CD3DFontCN::FONT_CHAR_INFO * CD3DFontCN::GetCharInfo(WCHAR Char)
{
	if(m_CharMap[Char]==0xffff)
	{
		if(!AddChar(Char))
			return NULL;
	}

	return m_pCharInfoBuff+m_CharMap[Char];
}

void CD3DFontCN::ParseLineInfo(LPCWSTR pText,int TextLen,RECT * pRect,DWORD Align,CTextLineList& LineInfos)
{
	int x,y;
	int LineX,LineY;
	int LineStart;
	int	LineLen;
	int LineWidth;
	bool CanOut=true;
	bool ForceReturn=false;
	int CommandStart;



	LineInfos.Clear();

	x=pRect->left;
	y=pRect->top;

	LineStart=0;
	LineLen=0;
	LineWidth=0;
	LineX=x;
	LineY=y;
	for(int i=0;i<TextLen;i++)
	{
		FONT_CHAR_INFO * pCharInfo=NULL;
		if(pText[i]=='<')
		{
			//处理命令
			CanOut=false;
		}
		else if(pText[i]=='>')
		{
			CanOut=true;
		}
		else if(pText[i]=='&')
		{
			CanOut=false;
			CommandStart=i;
		}
		else if((!CanOut)&&pText[i]==';')
		{
			CanOut=true;
			pCharInfo=GetCharInfo(StrToChar(pText+CommandStart,i-CommandStart+1));
		}
		else if(CanOut)
		{	
			pCharInfo=GetCharInfo(pText[i]);
		}		
		if(pCharInfo)
		{
			x+=pCharInfo->Size.cx;
			//超过宽度或者遇到换行符
			if((x>pRect->right||ForceReturn)&&(Align&D3DFONT_WORDBREAK))
			{
				TEXT_LINE_INFO LineInfo;
			
				if(LineWidth)
					LineWidth-=m_CharSpace;

				LineInfo.StartPos=LineStart;
				LineInfo.Len=LineLen;
				LineInfo.Width=LineWidth;
				LineInfo.Align=Align;
				LineInfos.Add(LineInfo);


				y+=m_CharHeight+m_LineSpace;
				//超出范围，不用继续画
				//if(y>=pRect->bottom)
				//	break;

				x=pRect->left;				
				LineStart=i;
				LineLen=0;
				LineWidth=0;
				LineX=x;
				LineY=y;
				x+=pCharInfo->Size.cx;
				ForceReturn=false;
			}			
			x+=m_CharSpace;			
			if(pText[i]=='\n')
				ForceReturn=true;

			LineWidth+=pCharInfo->Size.cx+m_CharSpace;
		}		
		LineLen++;
	}
	if(LineLen)
	{
		TEXT_LINE_INFO LineInfo;

		if(LineWidth)
			LineWidth-=m_CharSpace;

		LineInfo.StartPos=LineStart;
		LineInfo.Len=LineLen;
		LineInfo.Width=LineWidth;
		LineInfo.Align=Align;
		LineInfos.Add(LineInfo);
	}
}

void CD3DFontCN::DrawLine(int x,int y,LPCWSTR pText,int TextLen,int TextWidth,RECT * pRange,DWORD Align,D3DCOLOR& Color)
{	
	bool CanOut=true;
	int CommandStart;	

	//超出绘制范围，不绘制
	if(y>pRange->bottom)
		return;

	if(Align& D3DFONT_CENTER)
	{
		x+=((pRange->right-pRange->left)-TextWidth)/2;
	}
	else if(Align&D3DFONT_RIGHT)
	{
		x=pRange->right-TextWidth;	
	}
	for(int i=0;i<TextLen;i++)
	{
		FONT_CHAR_INFO * pCharInfo=NULL;
		if(pText[i]=='<')
		{
			//处理命令
			CanOut=false;
			CommandStart=i+1;
		}
		else if((!CanOut)&&pText[i]=='>')
		{
			CanOut=true;
			CEasyStringW Cmd;
			Cmd="Color=";
			if(_wcsnicmp(pText+CommandStart,(LPCWSTR)Cmd,6)==0)
			{
				Color=(DWORD)wcstoul(pText+CommandStart+6,NULL,16);
			}				
		}
		else if(pText[i]=='&')
		{
			CanOut=false;
			CommandStart=i;
		}
		else if((!CanOut)&&pText[i]==';')
		{
			CanOut=true;
			pCharInfo=GetCharInfo(StrToChar(pText+CommandStart,i-CommandStart+1));
		}
		else if(CanOut)
		{	
			pCharInfo=GetCharInfo(pText[i]);
		}
		if(pCharInfo)
		{
			RECT CharRect;
			int XCut=0;
			int YCut=0;

			//超出绘制范围，停止绘制
			if(x>pRange->right)
				break;
			CharRect.left=x;
			CharRect.top=y;
			CharRect.right=x+pCharInfo->Size.cx;
			if(CharRect.right>pRange->right)
			{
				XCut=CharRect.right-pRange->right;
				CharRect.right=pRange->right;
			}
			CharRect.bottom=y+pCharInfo->Size.cy;
			if(CharRect.bottom>pRange->bottom)
			{
				YCut=CharRect.bottom-pRange->bottom;
				CharRect.bottom=pRange->bottom;
			}
			m_DrawClip.SetRect(CharRect);				
			CharRect.left=pCharInfo->Pos.x;
			CharRect.top=pCharInfo->Pos.y;
			CharRect.right=pCharInfo->Pos.x+pCharInfo->Size.cx-XCut;
			CharRect.bottom=pCharInfo->Pos.y+pCharInfo->Size.cy-YCut;			
			m_pCharTextureBuff->AddUseRef();
			m_DrawClip.SetTexture(m_pCharTextureBuff,&CharRect);			
			m_DrawClip.SetVertexColor(Color);
			m_DrawClip.CreateVertex();
			m_Render.RenderDirectly(&m_DrawClip);
			x+=pCharInfo->Size.cx+m_CharSpace;
		}		
	}
}

WCHAR CD3DFontCN::StrToChar(LPCWSTR pStr,int StrLen)
{
	CEasyStringW Cmd;

	if(StrLen<4)
		return 0;
	Cmd="&lt;";
	if(_wcsnicmp(pStr,(LPCWSTR)Cmd,4)==0)
		return '<';
	Cmd="&gt;";
	if(_wcsnicmp(pStr,(LPCWSTR)Cmd,4)==0)
		return '>';
	if(StrLen<5)
		return 0;
	Cmd="&amp;";
	if(_wcsnicmp(pStr,(LPCWSTR)Cmd,5)==0)
		return '&';
	return 0;
}

}