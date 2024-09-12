/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWnd.h                                                 */
/*      创建日期:  2009年09月25日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once
//#include "D3DGUI.h"


namespace D3DGUI{

enum WND_RECT_INDEX
{
	RECT_TOP,
	RECT_BOTTOM,
	RECT_LEFT,
	RECT_RIGHT,
	RECT_TOP_LEFT,
	RECT_TOP_RIGHT,
	RECT_BOTTOM_LEFT,
	RECT_BOTTOM_RIGHT,
////////////////Borders///////////////////////
	RECT_CENTER,
	RECT_TEXT,
	RECT_MAX,
	RECT_BORDER_COUNT=RECT_CENTER,
};

typedef int WIN_BORDERS[RECT_BORDER_COUNT];


class CD3DGUI;
class CD3DWnd :public CNameObject
{
public:
	

protected:
	//struct STORAGE_STRUCT:public CNameObject::STORAGE_STRUCT
	//{
	//	UINT				ScriptStrLen;
	//};
	enum SST_MEMEBR_ID
	{
		SST_GUI_SCRIPT=SST_NO_MAX,
		SST_GUI_MAX=SST_NO_MAX+50,
	};

	CD3DGUI *				m_pGUI;
	LPIBASERECT*			m_pWndRects;
	int						m_WndRectCount;
	

	CEasyRect				m_WndRect;
	WIN_BORDERS				m_Borders;

	IUITexture *			m_pTexture;
	CEasyRect				m_TextureRect;	

	bool					m_IsVisible;

	bool					m_IsDragging;
	CEasyPoint				m_DraggingStartPoint;
	CEasySize				m_DraggingStartSize;	
	CEasyPoint				m_DraggingStartMousePoint;

	bool					m_CanDrag;
	bool					m_CanResize;
	int						m_Resizing;	

	UINT					m_TabIndex;
	bool					m_IsTabContainer;
	bool					m_IsEnable;
	bool					m_CanGetFocus;
	bool					m_IsTopmost;

	CD3DWnd	*				m_pParentWnd;
	CEasyArray<CD3DWnd *>	m_ChildWndList;
	CEasyArray<CD3DWnd *>	m_TabWndList;
	
	bool					m_IsActive;
	bool					m_IsCaptureAll;

	CEasyStringW			m_WndText;	
	LOGFONT					m_LogFont;
	DWORD					m_FontColor;
	DWORD					m_FontAlign;
	DWORD					m_FontShadowMode;
	DWORD					m_FontShadowColor;
	DWORD					m_FontShadowWidth;
	int						m_FontCharSpace;
	int						m_FontLineSpace;
	FLOAT					m_FontScale;

	DWORD					m_BKColor;

	bool					m_WantUpdateFont;
	bool					m_IsInternal;

	LPVOID					m_WndData;

	CEasyString				m_TempScript;

	DECLARE_CLASS_INFO(CD3DWnd)
	
public:
	CD3DWnd();
	CD3DWnd(CD3DGUI *  pGUI);
	virtual ~CD3DWnd(void);	
	virtual void InitWnd(CD3DGUI *  pGUI);
	virtual void Destory();
	

	virtual BOOL OnMessage(CD3DWnd * pWnd,UINT msg, WPARAM wParam, LPARAM lParam);
	virtual BOOL HandleMessage(CD3DWnd * pWnd,UINT msg, WPARAM wParam, LPARAM lParam);


	virtual bool SetTexture(LPCTSTR TextureFileName,RECT& TextureRect);
	virtual void GetTexture(CEasyString& TextureFileName,RECT& TextureRect);
	
	virtual void Move(int x, int y);
	virtual void Resize(int width, int height);


	virtual void SetRect(RECT& rect);	
	virtual CEasyRect GetRect();	
	virtual CEasyRect GetCenterRect();

	virtual CEasyRect GetWndRect();	
	virtual CEasyRect GetClientRect();


	virtual void SetBorders(WIN_BORDERS& Borders);
	virtual void GetBorders(WIN_BORDERS& Borders);
	virtual int GetBorder(UINT Index);

	virtual bool SetEffectMode(UINT Index,int EffectMode);
	
		
	virtual void SetParent(CD3DWnd * parent,bool IsUpdateParent=true);
	CD3DWnd * GetParent(void);

	virtual void SetFont(LOGFONT * pLogFont);
	virtual void SetFontColor(DWORD Color);
	virtual void SetFontAlign(DWORD Align);
	virtual void SetFontShadowMode(DWORD ShadowMode);
	virtual void SetFontShadowColor(DWORD ShadowColor);
	virtual void SetFontShadowWidth(DWORD ShadowWidth);
	virtual void SetFontCharSpace(int Space);
	virtual void SetFontLineSpace(int Space);
	virtual void SetFontScale(FLOAT Scale);

	virtual LOGFONT * GetFont();
	virtual DWORD GetFontColor();
	virtual DWORD GetFontAlign();
	virtual DWORD GetFontShadowMode();
	virtual DWORD GetFontShadowColor();
	virtual DWORD GetFontShadowWidth();
	virtual int GetFontCharSpace();
	virtual int GetFontLineSpace();
	virtual FLOAT GetFontScale();
	
	

	virtual void SetTextW(LPCWSTR Text);
	virtual void SetText(LPCTSTR Text);	
	virtual void GetText(CEasyString& Text)
	{
		Text=m_WndText;
	}

	virtual void SetBKColor(DWORD BKColor);
	DWORD GetBKColor()
	{
		return m_BKColor;
	}


	void ClientToScreen(RECT * rect);	
	void ScreenToClient(RECT * rect);
	void ClientToScreen(POINT * point);
	void ScreenToClient(POINT * point);	

	virtual void ActiveWnd(bool bActive,bool SendNotify=true);
	virtual void ActiveNextChildWnd(CD3DWnd * pCurWnd);


	virtual void EnableDrag(bool bCanDrag)
	{
		m_CanDrag=bCanDrag;
	}

	bool CanDrag()
	{
		return m_CanDrag;
	}

	virtual void EnableResize(bool bCanResize)
	{
		m_CanResize=bCanResize;
	}

	bool CanResize()
	{
		return m_CanResize;
	}

	virtual void TopTo(bool bRedraw,CD3DWnd * Before=NULL);	

	virtual void SetVisible(bool IsVisible);
	virtual bool IsVisible()
	{return m_IsVisible;}

	virtual void EnableWindow(bool IsEnable)
	{
		m_IsEnable=IsEnable;
	}
	virtual bool IsEnable()
	{
		return m_IsEnable;
	}

	UINT GetTabIndex()
	{
		return m_TabIndex;
	}

	void SetTabIndex(UINT Index);

	bool IsTabContainer()
	{
		return m_IsTabContainer;
	}

	void EnableTabContainer(bool Enbale);

	void EnableFocus(bool Enable)
	{
		m_CanGetFocus=Enable;
	}

	bool CanGetFocus()
	{
		return m_CanGetFocus&&m_IsEnable;
	}

	void SetTopmost(bool IsTopmost)
	{
		m_IsTopmost=IsTopmost;
	}

	bool IsTopmost()
	{
		return m_IsTopmost;
	}

	virtual void SetCaptureAll(bool IsCaptureAll);

	bool IsCaptureAll()
	{
		return m_IsCaptureAll;
	}

	bool IsActive()
	{
		return m_IsActive;
	}

	void SetInternal(bool IsInternal)
	{
		m_IsInternal=IsInternal;
	}

	bool IsInternal()
	{
		return m_IsInternal;
	}

	void SetFocus();

	virtual void GetMiniSize(int& Width,int& Height);	

	virtual void UpdateRects();
	virtual bool UpdateTextureRect();
	virtual void UpdateText();

	virtual void Update();	

	virtual void SaveToXml(xml_node * pXMLNode);
	virtual bool LoadFromXml(xml_node * pXMLNode);
	virtual bool LoadFromFile(LPCTSTR FileName,LPCTSTR WndName=NULL);

	
	virtual void PickResource(CUSOResourceManager * pResourceManager,UINT Param=0);

	virtual void OnLoaded();

	UINT GetChildWndCount()
	{
		return m_ChildWndList.GetCount();
	}

	CD3DWnd * GetChildWnd(UINT Index)
	{
		if(Index<m_ChildWndList.GetCount())
			return m_ChildWndList[Index];
		else
			return NULL;
	}

	CD3DWnd * GetChildWndByID(int ID);
	CD3DWnd * GetChildWndByName(LPCTSTR Name);

	bool IsChild(CD3DWnd * pWnd);
	bool IsChildRecursion(CD3DWnd * pWnd);

	IUITextRect * GetTextRect();

	void SetWndData(LPVOID Data)
	{
		m_WndData=Data;
	}

	LPVOID GetWndData()
	{
		return m_WndData;
	}

	virtual bool ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual UINT GetSmartStructSize(UINT Param=0);

protected:
	void SaveChildsToXml(xml_node& Childs);
	void SaveFrameToXML(xml_node& Frame);
	void SaveBorderToXML(xml_node& Borders);
	void SaveTextToXML(xml_node& Text);
	void SaveFontToXML(xml_node& Font);
	void SaveTextureToXML(xml_node& Texture);
	void SaveBehaviorToXML(xml_node& Behavior);

	void LoadChildsFromXml(xml_node& Childs);
	void LoadFrameFromXML(xml_node& Frame);
	void LoadBorderFromXML(xml_node& Borders);
	void LoadTextFromXML(xml_node& Text);
	void LoadFontFromXML(xml_node& Font);
	void LoadTextureFromXML(xml_node& Texture);
	void LoadBehaviorFromXML(xml_node& Behavior);

	bool AddChildWnd(CD3DWnd * child);
	bool DelChildWnd(CD3DWnd * child);

	bool AddTabWnd(CD3DWnd * child);
	bool DelTabWnd(CD3DWnd * child);	

	virtual bool UpdateFont();
	void UpdateChildRects();

	IUIBaseRect * GetTopRect();
	void GetRectIncludeChild(CEasyArray<IUIBaseRect *>& RectList);
	void TopChild(bool Redraw);

	virtual IUIWndRect * CreateRect();
	virtual IUITextRect * CreateTextRect();

	virtual void RebuildOrder();

	void TopParent();

	//virtual CNameObject::STORAGE_STRUCT * USOCreateHead(UINT Param=0);
	//virtual int USOWriteHead(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param=0);
	//virtual bool USOWriteData(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param=0);

	//virtual int USOReadHead(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param=0);
	//virtual int USOReadData(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,BYTE * pData,int DataSize,UINT Param=0);
	
	

};



}