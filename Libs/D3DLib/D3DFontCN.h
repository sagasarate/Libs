/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DFontCN.h                                              */
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


#define D3DFONT_TOP                      0x00000000
#define D3DFONT_LEFT                     0x00000000
#define D3DFONT_CENTER                   0x00000001
#define D3DFONT_RIGHT                    0x00000002
#define D3DFONT_VCENTER                  0x00000004
#define D3DFONT_BOTTOM                   0x00000008
#define D3DFONT_WORDBREAK                0x00000010


#pragma comment(linker,"/include:?m_CD3DFontCNClassInfoRegister@CD3DFontCN@D3DLib@@1VCClassInfoRegister@@A")

class CD3DFontCN:
	public CD3DBaseFont
{
protected:	
	struct FONT_CHAR_INFO
	{	
		FLOAT_RECT	UV;
		POINT		Pos;
		SIZE		Size;
		WORD		CharCode;
		bool		IsUsed;
		FONT_CHAR_INFO()
		{
			CharCode=0;
			IsUsed=false;
		}		
	};
	struct TEXT_LINE_INFO
	{
		int StartPos;
		int Len;
		int Width;
		DWORD Align;
		TEXT_LINE_INFO()
		{
			StartPos=0;
			Len=0;
			Width=0;
			Align=0;
		}
	};

	typedef CEasyArray<TEXT_LINE_INFO> CTextLineList;


	LOGFONT							m_LogFont;
	int								m_MipLevels;
	HDC								m_FontDC;
	HFONT							m_GDIFont;
	DWORD							m_FontFlag;
	WORD							m_CharMap[0x10000];
	FONT_CHAR_INFO *				m_pCharInfoBuff;
	CD3DTexture *					m_pCharTextureBuff;
	DWORD							m_TextureFormat;
	int								m_CharBuffSize;
	int								m_CharBuffWritePtr;
	int								m_CharSpace;
	int								m_LineSpace;	

	int								m_CharWidth;
	int								m_CharHeight;
	HBITMAP							m_CharBmp;
	DWORD *							m_pBitmapBits;

	CD3DUIRender					m_Render;
	CD3DFontCharClip				m_DrawClip;

	DECLARE_CLASS_INFO(CD3DFontCN)

public:

	CD3DFontCN();
    CD3DFontCN(CD3DFontManager * pManager);
    ~CD3DFontCN();

	virtual bool Reset();
	virtual bool Restore();


	bool Create(const LOGFONT * pLogFont,int MipLevels=1,int BufferSize=4096);
	
	virtual bool DrawTextA(LPCSTR pText,int TextLen,RECT * pRect,DWORD Align,D3DCOLOR Color);
	virtual bool DrawTextW(LPCWSTR pText,int TextLen,RECT * pRect,DWORD Align,D3DCOLOR Color);

	virtual bool GetTextSizeA(LPCSTR pText,int TextLen,LPSIZE pSize,LPINT pCharWidths=NULL);
	virtual bool GetTextSizeW(LPCWSTR pText,int TextLen,LPSIZE pSize,LPINT pCharWidths=NULL);

	virtual bool TranslateTextA(LPCSTR pSrcText,int StrLen,LPTSTR pDestText,int& BufferSize);
	virtual bool TranslateTextW(LPCWSTR pSrcText,int StrLen,LPWSTR pDestText,int& BufferSize);

	virtual bool GetLogFont(LOGFONT * pLogFont)
	{
		if(pLogFont)
		{
			*pLogFont=m_LogFont;
			return true;
		}
		return false;
	}

	virtual HDC GetDC()
	{
		return m_FontDC;
	}

	virtual void SetCharSpace(int Space)
	{
		m_CharSpace=Space;
	}
	virtual int GetCharSpace()
	{
		return m_CharSpace;
	}

	virtual void SetLineSpace(int Space)
	{
		m_LineSpace=Space;
	}
	virtual int GetLineSpace()
	{
		return m_LineSpace;
	}
	
protected:
	bool AddChar(WCHAR Char);
	CD3DFontCN::FONT_CHAR_INFO * AllocCharInfo(WCHAR Char);
	CD3DFontCN::FONT_CHAR_INFO * GetCharInfo(WCHAR Char);

	void ParseLineInfo(LPCWSTR pText,int TextLen,RECT * pRect,DWORD Align,CTextLineList& LineInfos);

	void DrawLine(int x,int y,LPCWSTR pText,int TextLen,int TextWidth,RECT * pRange,DWORD Align,D3DCOLOR& Color);

	WCHAR StrToChar(LPCWSTR pStr,int StrLen);
	
};

}


//#endif



