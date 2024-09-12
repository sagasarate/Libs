/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DTextTexture.h                                         */
/*      创建日期:  2009年09月25日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace D3DLib{

enum FONT_SHADOW_MODE
{
	FONT_SHADOW_NONE=0,	
	FONT_SHADOW_TOP=1<<0,
	FONT_SHADOW_BOTTOM=1<<1,
	FONT_SHADOW_LEFT=1<<2,
	FONT_SHADOW_RIGHT=1<<3,	
	FONT_SHADOW_TOP_LEFT=1<<4,
	FONT_SHADOW_BOTTOM_LEFT=1<<5,
	FONT_SHADOW_TOP_RIGHT=1<<6,
	FONT_SHADOW_BOTTOM_RIGHT=1<<7,
};

#define TEXT_TEXTURE_MAX_TEXT_LEN	256

class CD3DFX;
class CD3DBaseFont;

class CD3DTextTexture :
	public CD3DTexture
{
protected:
	
	//struct STORAGE_STRUCT:public CNameObject::STORAGE_STRUCT
	//{		
	//	LOGFONT				LogFont;
	//	D3DCOLOR			Color;
	//	D3DCOLOR			BKColor;
	//	DWORD				ShadowMode;
	//	int					ShadowWidth;
	//	D3DCOLOR			ShadowColor;
	//	DWORD				Align;	
	//	int					CharSpace;
	//	int					LineSpace;
	//	WCHAR				Text[TEXT_TEXTURE_MAX_TEXT_LEN];
	//	int					MipLevels;
	//	int					Width;
	//	int					Height;
	//};
	enum SST_MEMBER_ID
	{
		SST_D3DTTEX_LOG_FONT=SST_NO_MAX,
		SST_D3DTTEX_COLOR,
		SST_D3DTTEX_BACK_COLOR,
		SST_D3DTTEX_SHADOW_MODE,
		SST_D3DTTEX_SHADOW_WIDTH,
		SST_D3DTTEX_SHADOWCOLOR,
		SST_D3DTTEX_ALIGN,
		SST_D3DTTEX_CHARSPACE,
		SST_D3DTTEX_LINESPACE,
		SST_D3DTTEX_TEXT,
		SST_D3DTTEX_MIPLEVELS,
		SST_D3DTTEX_WIDTH,
		SST_D3DTTEX_HEIGHT,
		SST_D3DTTEX_MAX=SST_NO_MAX+50,
	};

	CEasyStringW		m_Text;
	int					m_MipLevels;
	int					m_Width;
	int					m_Height;
	CD3DBaseFont*		m_pD3DFont;
	

	LOGFONT				m_LogFont;

	D3DCOLOR			m_Color;
	D3DCOLOR			m_BKColor;
	DWORD				m_ShadowMode;
	int					m_ShadowWidth;
	D3DCOLOR			m_ShadowColor;
	DWORD				m_Align;	
	int					m_CharSpace;
	int					m_LineSpace;

	bool				m_AllowUpdate;
	bool				m_WantUpdate;

	CD3DFX *			m_pFX;

	

	DECLARE_CLASS_INFO(CD3DTextTexture)
public:
	CD3DTextTexture();
	CD3DTextTexture(CD3DTextureManager * pManager);
	virtual ~CD3DTextTexture(void);

	virtual void Destory();

	virtual bool Reset();
	virtual bool Restore();	

	bool Create(LOGFONT * pLogFont,int Width,int Height,int MipLevels,D3DCOLOR FontColor);

	bool SetSize(int Width,int Height);
	bool SetFont(const LOGFONT* pLogFont);	
	void SetColor(D3DCOLOR Color);	
	void SetText(LPCTSTR szText,int StrLen);
	void SetTextW(LPCWSTR szText,int StrLen);
	void SetAlign(DWORD dwAlign);//对齐模式，值同ID3DXFont的DrawText中的参数
	void SetShadowMode(DWORD ShadowMode);
	void SetShadowColor(D3DCOLOR ShadowColor);
	void SetShadowWidth(int Width);
	void SetBKColor(D3DCOLOR Color);
	void SetCharSpace(int Space);
	void SetLineSpace(int Space);

	CD3DBaseFont* GetD3DFont()
	{
		return m_pD3DFont;
	}

	LOGFONT* GetFont()
	{
		return &m_LogFont;
	}
	D3DCOLOR GetColor()
	{
		return m_Color;
	}
	void GetText(CEasyString& Text)
	{
		Text=m_Text;
	}
	DWORD GetAlign()
	{
		return m_Align;
	}
	DWORD GetShadowMode()
	{
		return m_ShadowMode;
	}
	D3DCOLOR GetShadowColor()
	{
		return m_ShadowColor;
	}
	int GetShadowWidth()
	{
		return m_ShadowWidth;
	}
	D3DCOLOR GetBKColor()
	{
		return m_BKColor;
	}
	int GetCharSpace()
	{
		return m_CharSpace;
	}
	int GetLineSpace()
	{
		return m_LineSpace;
	}

	void EnableUpdate(bool AllowUpdate);

	
	virtual int GetWidth()
	{
		return m_Width;
	}
	virtual int GetHeight()
	{
		return m_Height;
	}	

	virtual FLOAT GetUS()
	{return (FLOAT)(m_Width-0.5f)/m_TextureSurfaceInfo.Width;}

	virtual FLOAT GetVS()
	{return (FLOAT)(m_Height-0.5f)/m_TextureSurfaceInfo.Height;}
	
	HDC GetDC();

	virtual void OnPrepareRenderData();
	
	virtual bool ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual UINT GetSmartStructSize(UINT Param=0);


protected:
	void UpdateTexture();

	//virtual CNameObject::STORAGE_STRUCT * USOCreateHead(UINT Param=0);
	//virtual int USOWriteHead(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param=0);	

	//virtual int USOReadHead(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param=0);
	//virtual bool USOReadFinish(CNameObject::STORAGE_STRUCT * pHead,UINT Param=0);

};

}