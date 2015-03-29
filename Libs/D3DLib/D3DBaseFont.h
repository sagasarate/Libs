/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DBaseFont.h                                            */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


namespace D3DLib{

class CD3DFontManager;

class CD3DBaseFont :
	public CNameObject
{
protected:
	CD3DFontManager *	m_pManager;

	DECLARE_CLASS_INFO_STATIC(CD3DBaseFont)
public:
	CD3DBaseFont();
	CD3DBaseFont(CD3DFontManager * pManager);
	virtual ~CD3DBaseFont(void);

	virtual bool Reset()=0;
	virtual bool Restore()=0;

	void SetManager(CD3DFontManager * pManager);

	virtual bool DrawTextA(LPCSTR pText,int TextLen,RECT * pRect,DWORD Align,D3DCOLOR Color)=0;
	virtual bool DrawTextW(LPCWSTR pText,int TextLen,RECT * pRect,DWORD Align,D3DCOLOR Color)=0;

	virtual bool GetTextSizeA(LPCSTR pText,int TextLen,LPSIZE pSize,LPINT pCharWidths=NULL)=0;
	virtual bool GetTextSizeW(LPCWSTR pText,int TextLen,LPSIZE pSize,LPINT pCharWidths=NULL)=0;

	virtual bool TranslateTextA(LPCSTR pSrcText,int StrLen,LPTSTR pDestText,int& BufferSize)=0;
	virtual bool TranslateTextW(LPCWSTR pSrcText,int StrLen,LPWSTR pDestText,int& BufferSize)=0;

	virtual bool GetLogFont(LOGFONT * pLogFont)=0;

	virtual HDC GetDC()=0;

	//并非所有字体都支持
	virtual void SetCharSpace(int Space)=0;
	virtual int GetCharSpace()=0;

	//并非所有字体都支持
	virtual void SetLineSpace(int Space)=0;
	virtual int GetLineSpace()=0;

};

inline void CD3DBaseFont::SetManager(CD3DFontManager * pManager)
{
	m_pManager=pManager;	
}

}