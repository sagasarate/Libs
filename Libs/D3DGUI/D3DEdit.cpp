/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DEdit.cpp                                              */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"
#include ".\d3dedit.h"
#include "mmsystem.h"

namespace D3DGUI{

#define LINE_GROW	8
#define CARET_TIME	500
#define CARET_WIDTH 2
#define MAX_COMPSTRING_SIZE	256




IMPLEMENT_CLASS_INFO(CD3DEdit,CD3DWnd);

CD3DEdit::CD3DEdit():CD3DWnd()
{
	m_pScrollBar=NULL;
	m_pIMEReadingWnd=NULL;
	m_pIMECandidateWnd=NULL;
	InitWnd(NULL);	
}

CD3DEdit::CD3DEdit(CD3DGUI * pGUI):CD3DWnd(pGUI)
{
	m_pScrollBar=NULL;
	m_pIMEReadingWnd=NULL;
	m_pIMECandidateWnd=NULL;
	m_pCharSizes=NULL;
	InitWnd(pGUI);
	
}

CD3DEdit::~CD3DEdit(void)
{
	Destory();	
}

void CD3DEdit::InitWnd(CD3DGUI *  pGUI)
{
	Destory();

	CD3DWnd::InitWnd(pGUI);

	m_IsMultiLine=false;
	m_IsShowCaret=true;
	m_IsCaretValidate=true;
	m_AutoWrap=true;
	m_IsReadyOnly=false;
	m_IsEncryption=false;

	m_LineHeight=16;
	m_FirstVisibleLine=0;	
	m_FirstVisibleCol=0;
	m_CurVisibleLineCount=0;


	m_CaretColor=0xffffffff;

	SAFE_DELETE_ARRAY(m_pWndRects);

	m_WndRectCount=RECT_TEXT+LINE_GROW*2+2;
	m_pWndRects=new LPIBASERECT[m_WndRectCount];
	ZeroMemory(m_pWndRects,sizeof(LPIBASERECT)*m_WndRectCount);
	m_TextRectIndex=RECT_TEXT;	
	m_CaretRectIndex=m_TextRectIndex+1;
	m_SelectRectStart=m_CaretRectIndex+1;
	m_SelectRectCount=LINE_GROW*2;


	m_CurCaretRow=0;
	m_CurCaretCol=0;
	m_InputBuff[0]=0;
	m_InputBuff[1]=0;
	m_InputBuff[2]=0;

	m_SelectStartRow=0;
	m_SelectStartCol=0;
	m_SelectEndRow=0;
	m_SelectEndCol=0;
	m_IsSelecting=false;

	m_ClientCurWidth=0;
	m_ClientCurHeight=0;

	m_IsEnabledScrollBar=false;
	m_ScrollBarWidth=16;

	
	if(pGUI)
	{	
		m_pScrollBar=pGUI->CreateScrollBar(CEasyRect(0,0,0,0));
		m_pScrollBar->SetParent(this);
		m_pScrollBar->SetVisible(false);
		m_pScrollBar->SetName(_T("ED_ScrollBar"));
		m_pScrollBar->SetInternal(true);
		m_pScrollBar->EnableFocus(false);
	}	

	m_FontAlign=DT_LEFT|DT_VCENTER;
	m_CanDrag=false;
	m_CanResize=false;

	m_EnableIME=false;	
	//m_IsSupportTans=false;		
}

void CD3DEdit::Destory()
{
	SAFE_DELETE_ARRAY(m_pCharSizes);
	//SAFE_RELEASE(m_pScrollBar);
	//SAFE_RELEASE(m_pIMEReadingWnd);
	//SAFE_RELEASE(m_pIMECandidateList);
	CD3DWnd::Destory();
}

BOOL CD3DEdit::OnMessage(CD3DWnd * pWnd,UINT msg, WPARAM wParam, LPARAM lParam)
{
	static bool IsControlPress=false;
	static bool IsShiftPress=false;

	if(ProcessIME(msg,wParam,lParam))
		return true;

	if(CD3DWnd::OnMessage(pWnd,msg,wParam,lParam))	
		return true;


	switch(msg)
	{
	case WM_KEYDOWN:
		{			
			switch(wParam)
			{
			case VK_LEFT:
				{				
					m_CurCaretCol--;
					UpdateCaretPos(false);					
					UpdateCaret();
					if(IsShiftPress)
					{
						m_SelectStartRow=m_SelectingStartRow;
						m_SelectStartCol=m_SelectingStartCol;
						m_SelectEndRow=m_CurCaretRow;
						m_SelectEndCol=m_CurCaretCol;
						UpdateSelect();
					}
					else
						ClearSelection();
					if(!m_IsMultiLine)
						UpdateText();

				}
				return true;
			case VK_RIGHT:
				{					
					m_CurCaretCol++;
					UpdateCaretPos(true);					
					UpdateCaret();
					if(IsShiftPress)
					{
						m_SelectStartRow=m_SelectingStartRow;
						m_SelectStartCol=m_SelectingStartCol;
						m_SelectEndRow=m_CurCaretRow;
						m_SelectEndCol=m_CurCaretCol;
						UpdateSelect();
					}
					else
						ClearSelection();
					if(!m_IsMultiLine)
						UpdateText();
				}
				return true;
			case VK_UP:
				{
					if(m_CurCaretRow>0)
					{
						if(m_CurCaretRow-1<m_FirstVisibleLine)
							SetFirstVisibleLine(m_CurCaretRow-1);

						int x,y1,y2;
						CaretRowColToXY(x,y1,y2,m_CurCaretRow,m_CurCaretCol);
						y1-=m_LineHeight+m_FontLineSpace;
						XYToCaretRowCol(x,y1,m_CurCaretRow,m_CurCaretCol);
						
						UpdateCaret();
						if(IsShiftPress)
						{
							m_SelectStartRow=m_SelectingStartRow;
							m_SelectStartCol=m_SelectingStartCol;
							m_SelectEndRow=m_CurCaretRow;
							m_SelectEndCol=m_CurCaretCol;
							UpdateSelect();
						}
						else
							ClearSelection();
					}
				}
				return true;
			case VK_DOWN:
				{
					if(m_CurCaretRow<(int)m_LineInfos.size()-1)
					{
						if(m_CurCaretRow+1-m_FirstVisibleLine>=m_CurVisibleLineCount)
							SetFirstVisibleLine(m_CurCaretRow-m_CurVisibleLineCount+2);

						int x,y1,y2;
						CaretRowColToXY(x,y1,y2,m_CurCaretRow,m_CurCaretCol);
						y1+=m_LineHeight+m_FontLineSpace;
						XYToCaretRowCol(x,y1,m_CurCaretRow,m_CurCaretCol);
							
						UpdateCaret();
						if(IsShiftPress)
						{
							m_SelectStartRow=m_SelectingStartRow;
							m_SelectStartCol=m_SelectingStartCol;
							m_SelectEndRow=m_CurCaretRow;
							m_SelectEndCol=m_CurCaretCol;
							UpdateSelect();
						}
						else
							ClearSelection();
					}
				}
				return true;
			case VK_END:
				m_CurCaretCol=GetEndCol(m_CurCaretRow);
				UpdateCaret();
				if(!m_IsMultiLine)
				{
					UpdateCaretPos();
					UpdateText();
				}
				return true;
			case VK_HOME:
				m_CurCaretCol=0;
				UpdateCaret();
				if(!m_IsMultiLine)
				{
					UpdateCaretPos();
					UpdateText();
				}
				return true;
			
			case VK_DELETE:
				{
					DeleteTextAtCaret(false);
				}
				return true;		
			case VK_CONTROL:
				IsControlPress=true;
				return true;
			case VK_SHIFT:
				m_SelectingStartRow=m_CurCaretRow;
				m_SelectingStartCol=m_CurCaretCol;
				IsShiftPress=true;
				return true;
			case 'C':			
				{
					if(IsControlPress)
					{
						Copy();
						return true;
					}
				}
				break;
			case 'V':			
				{
					if(IsControlPress)
					{
						Paste();
						return true;
					}
				}
				break;
			case 'X':			
				{
					if(IsControlPress)
					{
						Cut();
						return true;
					}
				}
				break;
			case 'A':
				if(IsControlPress)
				{
					m_SelectStartRow=0;
					m_SelectStartCol=0;
					IndexToRowCol((int)m_WndText.GetLength(),m_SelectEndRow,m_SelectEndCol);
					m_CurCaretRow=m_SelectEndRow;
					m_CurCaretCol=m_SelectEndCol;
					UpdateCaretPos();
					UpdateCaret();
					UpdateSelect();
					return true;
				}
				break;
			}
		}
		break;
	case WM_KEYUP:
		{			
			switch(wParam)
			{
			case VK_CONTROL:
				IsControlPress=false;
				return true;
			case VK_SHIFT:
				IsShiftPress=false;
				return true;
			}
		}
	case WM_CHAR:
		{			
			if(lParam&0x81000000)
			{
			}		
			else if(wParam==VK_BACK)
			{
				DeleteTextAtCaret(true);	
				return true;
			}
			else
			{
				int Index;
				RowColToIndex(Index,m_CurCaretRow,m_CurCaretCol);
				if(Index>=0)
				{
					if(wParam==VK_RETURN&&m_IsMultiLine)
					{
						InsertChar('\n',false);
					}
					else
					{
						if(m_InputBuff[0])
						{
							m_InputBuff[1]=(CHAR)wParam;
							InsertChar(CEasyStringW(m_InputBuff)[0],true);
							m_InputBuff[0]=0;			
						}
						else if((BYTE)wParam<0x20)
						{
						}
						else if((BYTE)wParam>=0x80&&(WORD)wParam<0x100)
						{
							m_InputBuff[0]=(CHAR)wParam;
						}
						else
						{
							InsertChar((WCHAR)wParam,true);
							
						}						
					}	
					return true;
				}
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{		
			int xPos=LOWORD(lParam);
			int yPos=HIWORD(lParam);
			CEasyRect Rect=GetClientRect();
			ClientToScreen(&Rect);
			
			if(Rect.PtInRect(CEasyPoint(xPos,yPos)))
			{
				XYToCaretRowCol(xPos,yPos,m_CurCaretRow,m_CurCaretCol);
				UpdateCaretPos();
				UpdateCaret();
				m_SelectingStartRow=m_CurCaretRow;
				m_SelectingStartCol=m_CurCaretCol;

				m_SelectStartRow=m_SelectingStartRow;
				m_SelectStartCol=m_SelectingStartCol;
				m_SelectEndRow=m_CurCaretRow;
				m_SelectEndCol=m_CurCaretCol;
				UpdateSelect();
				m_IsSelecting=true;
				return true;
			}			
		}
		break;
	case WM_LBUTTONUP:	
		if(m_IsSelecting)
		{
			m_IsSelecting=false;
			return true;
		}
		break;
	case WM_MOUSEMOVE:
		{
			if(m_IsSelecting)
			{
				int xPos=LOWORD(lParam);
				int yPos=HIWORD(lParam);
				XYToCaretRowCol(xPos,yPos,m_CurCaretRow,m_CurCaretCol);
				UpdateCaretPos();
				UpdateCaret();
				m_SelectStartRow=m_SelectingStartRow;
				m_SelectStartCol=m_SelectingStartCol;
				m_SelectEndRow=m_CurCaretRow;
				m_SelectEndCol=m_CurCaretCol;
				UpdateSelect();
				return true;
			}
		}
		break;
	case WM_MOUSEWHEEL:
		{
			int Delta = -GET_WHEEL_DELTA_WPARAM(wParam)/WHEEL_DELTA;
			SetFirstVisibleLine(m_FirstVisibleLine+Delta);			
		}
		return true;
	case WM_D3DGUI_SCROLL_BAR_SCROLL:
		SetFirstVisibleLine((int)lParam);		
		return true;
	}
	return false;
}

CEasyRect CD3DEdit::GetCenterRect()
{
	CEasyRect rect=m_WndRect;

	rect.top+=m_Borders[RECT_TOP];
	rect.bottom-=m_Borders[RECT_BOTTOM];
	rect.left+=m_Borders[RECT_LEFT];
	rect.right-=m_Borders[RECT_RIGHT];

	if(m_IsEnabledScrollBar&&m_IsMultiLine)
		rect.right-=m_ScrollBarWidth;
	return rect;
}

void CD3DEdit::ActiveWnd(bool bActive,bool SendNotify)
{
	CD3DWnd::ActiveWnd(bActive,SendNotify);	
	m_IsCaretValidate=bActive;
	UpdateCaret();
}

void CD3DEdit::SetFont(LOGFONT * pLogFont)
{
	if(pLogFont)
	{
		m_LogFont=*pLogFont;
		m_LineHeight=m_LogFont.lfHeight*m_FontScale;//m_FontShadowWidth*2+1;
		m_WantUpdateFont=true;	
		UpdateFont();
	}
}

void CD3DEdit::SetFontAlign(DWORD Align)
{
	if(!m_IsMultiLine)
	{	
		m_FontAlign=Align&(~DT_WORDBREAK);		
		m_WantUpdateFont=true;
		UpdateFont();
	}
}

void CD3DEdit::SetFontSahdowWidth(DWORD ShadowWidth)
{
	m_FontShadowWidth=ShadowWidth;
	//m_LineHeight=m_LogFont.lfHeight*m_FontScale;
	m_WantUpdateFont=true;
	UpdateFont();
}

void CD3DEdit::SetFontScale(FLOAT Scale)
{
	m_FontScale=Scale;
	m_LineHeight=m_LogFont.lfHeight*m_FontScale;
	m_WantUpdateFont=true;
	UpdateFont();
}

void CD3DEdit::SetVisible(bool IsVisible)
{
	m_IsVisible=IsVisible;
	for(int i=0;i<m_WndRectCount;i++)
	{
		if(m_pWndRects[i])
			m_pWndRects[i]->SetVisible(IsVisible);
	}
	if(m_IsEnabledScrollBar)
		m_pScrollBar->SetVisible(IsVisible);
	UpdateText();
}


void CD3DEdit::SetText(LPCTSTR Text)
{	
	m_WndText=Text;
	FormatText(m_WndText);
	CaculateLineInfo(m_WndText,m_LineInfos);
	UpdateCaretPos();
	UpdateText();

}

void CD3DEdit::UpdateRects()
{
	if(!m_IsVisible)
		return;

	CD3DWnd::UpdateRects();


	CEasyRect ClientRect=GetClientRect();	
	ClientToScreen(&ClientRect);		

	//计算需要的可视行数
	m_CurVisibleLineCount=(int)floor((FLOAT)ClientRect.Height()/(m_LineHeight+m_FontLineSpace));

	//根据需要扩充矩形数
	if(m_CurVisibleLineCount>GetSelectRectCount())
		GrowTextRect(m_CurVisibleLineCount-GetSelectRectCount()+LINE_GROW);

	//按需要重新断行
	if(m_AutoWrap&&m_IsMultiLine)
	{
		if(m_ClientCurWidth!=ClientRect.Width())
		{
			CaculateLineInfo(m_WndText,m_LineInfos);
			UpdateCaretPos();
		}		
	}

	m_ClientCurWidth=ClientRect.Width();
	m_ClientCurHeight=ClientRect.Height();


	if(m_IsEnabledScrollBar&&m_IsMultiLine)
	{
		CEasyRect Rect=GetClientRect();
		//ClientToScreen(&Rect);
		Rect.left=Rect.right;
		Rect.right+=m_ScrollBarWidth;
		m_pScrollBar->SetRect(Rect);
	}
}

void CD3DEdit::UpdateText()
{
	if(!m_IsVisible)
		return;	
	CEasyRect Rect=GetClientRect();
	ClientToScreen(&Rect);

	
	if(TextRect()==NULL)		
	{
		TextRect()=CreateTextRect();
		RebuildOrder();
	}
	if(TextRect())
	{
		TextRect()->SetRect(&FLOAT_RECT(Rect));
		int Row,TextStart=0,TextEnd=0;
		EDIT_TEXT_LINE_INFO * pLineInfo;

		Row=m_FirstVisibleLine;
		pLineInfo=GetLineInfo(Row);
		if(pLineInfo)
		{			
			TextStart=pLineInfo->StartPos;
			Row+=m_CurVisibleLineCount-1;
			if(Row>=(int)m_LineInfos.size())
				Row=(int)m_LineInfos.size()-1;
			if(!m_IsMultiLine)
			{
				TextStart+=m_FirstVisibleCol;
			}
		}
		pLineInfo=GetLineInfo(Row);
		if(pLineInfo)
			TextEnd=pLineInfo->StartPos+pLineInfo->Len;
		if(m_IsEncryption)
		{
			CEasyStringW EncryptionText;
			EncryptionText.Resize(TextEnd-TextStart);
			EncryptionText.SetLength(TextEnd-TextStart);
			for(UINT i=0;i<EncryptionText.GetLength();i++)
			{
				EncryptionText.SetChar(i,'*');
			}
			TextRect()->SetTextW(EncryptionText,EncryptionText.GetLength());
		}
		else
		{
			TextRect()->SetTextW((LPCWSTR)m_WndText+TextStart,TextEnd-TextStart);
		}		
	}
	
	UpdateSelect();
	UpdateCaret();
	
}

void CD3DEdit::UpdateCaret()
{
	m_IsCaretValidate=false;

	if(!m_IsVisible)
		return;

	if(!IsActive())	
		return;
	//更新光标
	if(m_IsShowCaret)
	{		
		//计算光标位置
		FLOAT_RECT CaretRect;

		CEasyRect ClientRect=GetClientRect();	
		ClientToScreen(&ClientRect);
		FLOAT_RECT Rect(ClientRect);

		int Row=m_CurCaretRow;
		int Col=m_CurCaretCol;	

		if(Row>=(int)m_LineInfos.size())
			Row=(int)m_LineInfos.size()-1;
		

		if(Row>=0)
		{			
			if(CaretWndRect()==NULL)
			{
				CaretWndRect()=CreateRect();
				RebuildOrder();
				CaretWndRect()->SetColor(m_CaretColor);
			}

			
			int x,y1,y2;
			if(CaretRowColToXY(x,y1,y2,Row,Col))
			{
				CaretRect.left=(FLOAT)x;
				CaretRect.top=(FLOAT)y1;
				CaretRect.bottom=(FLOAT)y2;

				CaretRect.right=CaretRect.left+CARET_WIDTH;
				if(CaretRect.left<ClientRect.left||CaretRect.right>ClientRect.right||
					CaretRect.top<ClientRect.top||CaretRect.bottom>ClientRect.bottom)
					return;
				CaretWndRect()->SetRect(&CaretRect);
				m_IsCaretValidate=true;
			}
							
		}		
	}
}

void CD3DEdit::UpdateSelect()
{
	if(!m_IsVisible)
		return;

	for(int i=0;i<GetSelectRectCount();i++)
	{
		if(SelectRect(i))
			SelectRect(i)->SetVisible(false);
	}

	

	if(m_SelectStartRow==m_SelectEndRow&&m_SelectStartCol==m_SelectEndCol)
		return;

	if(m_SelectStartRow>m_SelectEndRow)
	{
		int temp=m_SelectEndRow;
		m_SelectEndRow=m_SelectStartRow;
		m_SelectStartRow=temp;

		temp=m_SelectEndCol;
		m_SelectEndCol=m_SelectStartCol;
		m_SelectStartCol=temp;
	}
	if(m_SelectStartRow==m_SelectEndRow&&m_SelectStartCol>m_SelectEndCol)
	{
		int temp=m_SelectEndCol;
		m_SelectEndCol=m_SelectStartCol;
		m_SelectStartCol=temp;
	}

	CEasyRect ClientRect=GetClientRect();	
	ClientToScreen(&ClientRect);

	for(int Row=m_SelectStartRow;Row<=m_SelectEndRow;Row++)
	{
		int StartCol,EndCol;
		EDIT_TEXT_LINE_INFO * pLineInfo;
		int x1,x2,y1,y2;
		
		if(Row<m_FirstVisibleLine||Row-m_FirstVisibleLine>GetSelectRectCount())
			continue;

		pLineInfo=GetLineInfo(Row);
		if(pLineInfo)
		{
			if(Row==m_SelectStartRow)
				StartCol=m_SelectStartCol;
			else
				StartCol=0;
			if(Row==m_SelectEndRow)
				EndCol=m_SelectEndCol;
			else
				EndCol=pLineInfo->Len;
			CaretRowColToXY(x1,y1,y2,Row,StartCol);
			CaretRowColToXY(x2,y1,y2,Row,EndCol);
			if(x1<ClientRect.left)
				x1=ClientRect.left;
			if(x2>ClientRect.right)
				x2=ClientRect.right;
			FLOAT_RECT Rect(x1,y1,x2,y2);
			int Index=Row-m_FirstVisibleLine;
			if(SelectRect(Index)==NULL)
			{
				SelectRect(Index)=CreateRect();
				RebuildOrder();
				SelectRect(Index)->SetEffectMode(D3DGUI_EFFECT_INVERSE_COLOR);
			}
			if(SelectRect(Index))
			{
				SelectRect(Index)->SetRect(&Rect);
				SelectRect(Index)->SetVisible(true);
			}
		}
	}
}

void CD3DEdit::Update()
{
	CD3DWnd::Update();
	
	if(CaretWndRect())
	{
		if(m_IsShowCaret&&m_IsCaretValidate&&IsEnable())
		{
			if((CEasyTimer::GetTime()/CARET_TIME)%2)
				CaretWndRect()->SetVisible(true);
			else
				CaretWndRect()->SetVisible(false);
		}
		else
			CaretWndRect()->SetVisible(false);
	}
}

CEasyStringW CD3DEdit::GetLineText(int Row)
{
	EDIT_TEXT_LINE_INFO * pLineInfo=GetLineInfo(Row);
	if(pLineInfo)
	{
		return m_WndText.SubStr(pLineInfo->StartPos,pLineInfo->Len);
	}
	return CEasyStringW("");
}

void CD3DEdit::SetMultiLine(bool IsMultiLine)
{
	m_IsMultiLine=IsMultiLine;
	if(!m_WndText.IsEmpty())
	{
		FormatText(m_WndText);
		CaculateLineInfo(m_WndText,m_LineInfos);
		UpdateText();
	}
	if(m_IsMultiLine)
	{
		m_FontAlign=DT_WORDBREAK;				
		EnableScrollBar(true);
	}
	else
	{		
		m_FontAlign=DT_LEFT|DT_VCENTER;
		EnableScrollBar(false);
	}
	m_WantUpdateFont=true;
	UpdateFont();
}

void CD3DEdit::EnableAutoWrap(bool IsAutoWrap)
{
	m_AutoWrap=IsAutoWrap;
	FormatText(m_WndText);
	CaculateLineInfo(m_WndText,m_LineInfos);	
	UpdateCaretPos();
	UpdateText();
}

void CD3DEdit::SetEncryption(bool IsEncryption)
{
	m_IsEncryption=IsEncryption;
	UpdateText();
}

void CD3DEdit::SetFirstVisibleLine(int LineIndex)
{
	m_FirstVisibleLine=LineIndex;
	if(m_FirstVisibleLine<0)
		m_FirstVisibleLine=0;
	if(m_FirstVisibleLine>=(int)m_LineInfos.size())
		m_FirstVisibleLine=(int)m_LineInfos.size()-1;	
	if(m_IsEnabledScrollBar&&m_IsMultiLine)
	{
		if(m_pScrollBar->GetScrollPos()!=m_FirstVisibleLine)
			m_pScrollBar->SetScrollPos(m_FirstVisibleLine);
	}
	UpdateText();
	
}

void CD3DEdit::SetCaretColor(DWORD Color)
{
	m_CaretColor=Color;
	if(CaretWndRect())
		CaretWndRect()->SetColor(m_CaretColor);
}

void CD3DEdit::SetCaretPos(int Pos)
{
	if(Pos>=0&&Pos<=(int)m_WndText.GetLength())
	{
		IndexToRowCol(Pos,m_CurCaretRow,m_CurCaretCol);
		m_CurLineInfo=*GetLineInfo(m_CurCaretRow);
		UpdateCaret();
	}
}

int CD3DEdit::GetCaretPos()
{
	int Index;
	RowColToIndex(Index,m_CurCaretRow,m_CurCaretCol);
	return Index;
}

void CD3DEdit::SetSel(int Start,int End)
{
	if(Start>=0&&Start<=(int)m_WndText.GetLength()&&
		End>=0&&End<=(int)m_WndText.GetLength())
	{
		IndexToRowCol(Start,m_SelectStartRow,m_SelectStartCol);
		IndexToRowCol(End,m_SelectEndRow,m_SelectEndCol);
		UpdateSelect();
	}
}

void CD3DEdit::GetSel(int& Start,int& End)
{
	RowColToIndex(Start,m_SelectStartRow,m_SelectStartCol);
	RowColToIndex(End,m_SelectEndRow,m_SelectEndCol);
}

void CD3DEdit::SetScrollBarWidth(int Width)
{
	m_ScrollBarWidth=Width;
	UpdateRects();
}

void CD3DEdit::EnableScrollBar(bool Enable)
{
	m_IsEnabledScrollBar=Enable;
	if(m_IsEnabledScrollBar&&m_IsMultiLine)
	{
		m_pScrollBar->SetVisible(true);
		m_pScrollBar->SetMaxScrollPos((int)m_LineInfos.size()-1);
		m_pScrollBar->SetScrollPos(m_FirstVisibleLine);
	}
	else
	{
		m_pScrollBar->SetVisible(false);
	}
	UpdateRects();
}

void CD3DEdit::Copy()
{
	if(m_SelectStartRow!=m_SelectEndRow||m_SelectStartCol!=m_SelectEndCol)
	{
		int Index;
		int Len;
		RowColToIndex(Index,m_SelectStartRow,m_SelectStartCol);
		RowColToIndex(Len,m_SelectEndRow,m_SelectEndCol);
		Len=Len-Index;
		CEasyString Text;
		Text=m_WndText.SubStr(Index,Len);
		Text.Replace(_T("\n"),_T("\r\n"));
		//if(m_IsSupportTans)
		//	Text="<TransFont>"+Text;

		HGLOBAL hClip = GlobalAlloc(GMEM_ZEROINIT|GMEM_MOVEABLE|GMEM_DDESHARE,(Text.GetLength()+1)*sizeof(TCHAR));
		if(!hClip)
			return;

		TCHAR *Buff = (TCHAR*)GlobalLock(hClip);
		if(!Buff)
		{
			GlobalFree(hClip);
			return;
		}
		_tcscpy_s(Buff,Text.GetLength(),(LPCTSTR)Text);		

		GlobalUnlock(hClip);

		if(OpenClipboard(NULL))
		{
			EmptyClipboard();
			SetClipboardData( CF_TEXT,hClip);
			CloseClipboard();
		}
		else
			GlobalFree(hClip);
	}
}

void CD3DEdit::Paste()
{	
	if(OpenClipboard(NULL))
	{
		HGLOBAL hClip=NULL;

		if( hClip=GetClipboardData(CF_TEXT) )
		{
			CEasyStringW Text;
			Text = (LPCTSTR)GlobalLock(hClip);				
			if(Text.GetLength())
			{				
				//if(Text.Left(11)==CEasyStringW("<TransFont>"))
				//{
				//	Text=Text.Right(Text.GetLength()-11);
				//	InsertStr((LPCWSTR)Text,false);
				//}
				//else
					InsertStr((LPCWSTR)Text,true);
			}
			GlobalUnlock(hClip);				
		}
		CloseClipboard();		
	}
}

void CD3DEdit::Cut()
{
	Copy();
	DeleteSelection();
}


void CD3DEdit::SaveToXml(xml_node * pXMLNode)
{
	xml_node Wnd=pXMLNode->append_child(node_element,_T("Edit"));
	Wnd.append_attribute(_T("Name"),(LPCTSTR)GetName());
	Wnd.append_attribute(_T("ID"),(long)GetID());
	Wnd.append_attribute(_T("IsInternal"),IsInternal());

	xml_node Behavior=Wnd.append_child(node_element,_T("Behavior"));
	SaveBehaviorToXML(Behavior);
	Behavior.append_attribute(_T("IsMultiLine"),m_IsMultiLine);
	Behavior.append_attribute(_T("ShowCaret"),m_IsShowCaret);
	Behavior.append_attribute(_T("CaretColor"),(long)m_CaretColor);
	Behavior.append_attribute(_T("AutoWrap"),m_AutoWrap);
	Behavior.append_attribute(_T("ReadOnly"),m_IsReadyOnly);
	Behavior.append_attribute(_T("Encryption"),m_IsEncryption);
	Behavior.append_attribute(_T("EnableScrollBar"),m_IsEnabledScrollBar);
	Behavior.append_attribute(_T("ScrollBarWidth"),(long)m_ScrollBarWidth);

	xml_node Frame=Wnd.append_child(node_element,_T("Frame"));
	SaveFrameToXML(Frame);
	xml_node Borders=Wnd.append_child(node_element,_T("Borders"));
	SaveBorderToXML(Borders);
	xml_node Text=Wnd.append_child(node_element,_T("Text"));
	SaveTextToXML(Text);
	xml_node Font=Wnd.append_child(node_element,_T("Font"));
	SaveFontToXML(Font);
	if(m_pTexture)
	{	
		xml_node Texture=Wnd.append_child(node_element,_T("Texture"));
		SaveTextureToXML(Texture);
	}
	
	if(m_ChildWndList.GetCount())
	{
		xml_node Childs=Wnd.append_child(node_element,_T("Childs"));
		SaveChildsToXml(Childs);
	}
}

bool CD3DEdit::LoadFromXml(xml_node * pXMLNode)
{
	if(_tcsnicmp(pXMLNode->name(),_T("Edit"),5)!=0)
		return false;
	if(pXMLNode->has_attribute(_T("Name")))
		SetName(pXMLNode->attribute(_T("Name")).getvalue());

	if(pXMLNode->has_attribute(_T("ID")))
		SetID(pXMLNode->attribute(_T("ID")));

	if(pXMLNode->has_attribute(_T("IsInternal")))
		SetInternal(pXMLNode->attribute(_T("IsInternal")));

	for(int i=0;i<(int)pXMLNode->children();i++)
	{
		if(_tcsnicmp(pXMLNode->child(i).name(),_T("Behavior"),9)==0)
		{
			LoadBehaviorFromXML(pXMLNode->child(i));
			if(pXMLNode->child(i).has_attribute(_T("IsMultiLine")))
				SetMultiLine(pXMLNode->child(i).attribute(_T("IsMultiLine")));
			if(pXMLNode->child(i).has_attribute(_T("ShowCaret")))
				EnableCaret(pXMLNode->child(i).attribute(_T("ShowCaret")));
			if(pXMLNode->child(i).has_attribute(_T("CaretColor")))
				SetCaretColor((UINT)pXMLNode->child(i).attribute(_T("CaretColor")));
			if(pXMLNode->child(i).has_attribute(_T("AutoWrap")))
				EnableAutoWrap(pXMLNode->child(i).attribute(_T("AutoWrap")));
			if(pXMLNode->child(i).has_attribute(_T("ReadOnly")))
				SetReadOnly(pXMLNode->child(i).attribute(_T("ReadOnly")));
			if(pXMLNode->child(i).has_attribute(_T("Encryption")))
				SetEncryption(pXMLNode->child(i).attribute(_T("Encryption")));
			if(pXMLNode->child(i).has_attribute(_T("EnableScrollBar")))
				EnableScrollBar(pXMLNode->child(i).attribute(_T("EnableScrollBar")));
			if(pXMLNode->child(i).has_attribute(_T("ScrollBarWidth")))
				SetScrollBarWidth(pXMLNode->child(i).attribute(_T("ScrollBarWidth")));
		}
		else if(_tcsnicmp(pXMLNode->child(i).name(),_T("Frame"),6)==0)
		{
			LoadFrameFromXML(pXMLNode->child(i));
		}
		else if(_tcsnicmp(pXMLNode->child(i).name(),_T("Borders"),8)==0)
		{
			LoadBorderFromXML(pXMLNode->child(i));
		}
		else if(_tcsnicmp(pXMLNode->child(i).name(),_T("Text"),5)==0)
		{
			LoadTextFromXML(pXMLNode->child(i));
		}
		else if(_tcsnicmp(pXMLNode->child(i).name(),_T("Font"),5)==0)
		{
			LoadFontFromXML(pXMLNode->child(i));
		}
		else if(_tcsnicmp(pXMLNode->child(i).name(),_T("Texture"),8)==0)
		{
			LoadTextureFromXML(pXMLNode->child(i));
		}
		

	}
	HandleMessage(this,WM_D3DGUI_WND_LOADED,GetID(),(LPARAM)this);

	//装载子窗口
	for(int i=(int)pXMLNode->children()-1;i>=0;i--)
	{
		if(_tcsnicmp(pXMLNode->child(i).name(),_T("Childs"),7)==0)
		{
			LoadChildsFromXml(pXMLNode->child(i));
			break;
		}
	}

	//识别内部对象
	for(int i=(int)m_ChildWndList.GetCount()-1;i>=0;i--)
	{
		CD3DWnd * pWnd=m_ChildWndList[i];
		if(m_ChildWndList[i]->IsInternal()&&
			m_ChildWndList[i]->IsKindOf(GET_CLASS_INFO(CD3DScrollBar))&&
			(_tcscmp(m_ChildWndList[i]->GetName(),_T("ED_ScrollBar"))==0)&&
			m_ChildWndList[i]!=m_pScrollBar)
		{
			CD3DScrollBar * pScrollBar=(CD3DScrollBar *)m_ChildWndList[i];
			SAFE_RELEASE(m_pScrollBar);
			m_pScrollBar=pScrollBar;
		}
	}
	TopChild(true);
	HandleMessage(this,WM_D3DGUI_CHILD_LOADED,GetID(),(LPARAM)this);
	return true;
}


void CD3DEdit::SetIMEReadingWnd(CD3DWnd * pWnd)
{
	SAFE_RELEASE(m_pIMEReadingWnd);
	m_pIMEReadingWnd=pWnd;
	m_pIMEReadingWnd->SetName(_T("ED_IMEReadingWnd"));
	m_pIMEReadingWnd->SetFont(GetFont());
	m_pIMEReadingWnd->SetVisible(false);
	m_pIMEReadingWnd->SetParent(this);
	m_pIMEReadingWnd->SetInternal(true);
	m_pIMEReadingWnd->EnableFocus(false);
}
void CD3DEdit::SetIMECandidateWnd(CD3DWnd * pWnd)
{
	SAFE_RELEASE(m_pIMECandidateWnd);
	m_pIMECandidateWnd=pWnd;
	m_pIMECandidateWnd->SetName(_T("ED_IMECandidateWnd"));
	m_pIMECandidateWnd->SetFont(GetFont());
	m_pIMECandidateWnd->SetVisible(false);
	m_pIMECandidateWnd->SetParent(this);
	m_pIMECandidateWnd->SetInternal(true);
	m_pIMECandidateWnd->EnableFocus(false);
}
void CD3DEdit::EnableIME(bool Enable)
{
	m_EnableIME=Enable&&(m_pIMEReadingWnd!=NULL)&&(m_pIMECandidateWnd!=NULL);
}


//////////////////////////////////////////////////////

bool CD3DEdit::UpdateFont()
{
	if(m_WantUpdateFont)
	{
				
		if(TextRect())
		{
			TextRect()->EnableUpdate(false);
			TextRect()->SetFont(&m_LogFont);
			TextRect()->SetColor(m_FontColor);
			TextRect()->SetAlign(m_FontAlign);
			TextRect()->SetShadowMode(m_FontShadowMode);
			TextRect()->SetShadowColor(m_FontShadowColor);
			TextRect()->SetShadowWidth(m_FontShadowWidth);
			TextRect()->SetCharSpace(m_FontCharSpace);
			TextRect()->SetLineSpace(m_FontLineSpace);
			TextRect()->SetScale(m_FontScale);
			m_FontCharSpace=TextRect()->GetCharSpace();
			m_FontLineSpace=TextRect()->GetLineSpace();
			TextRect()->EnableUpdate(true);
			CaculateLineInfo(m_WndText,m_LineInfos);
			UpdateCaretPos();
			UpdateText();

			//CEasyStringW Test;
			//FLOAT Width,Height;
			//Test="<Test>";
			//TextRect()->GetTextSizeW((LPCWSTR)Test,(int)Test.GetLength(),Width,Height);
			//if(Width==0)
			//	m_IsSupportTans=true;
			//else
			//	m_IsSupportTans=false;
		}
		
		m_WantUpdateFont=false;
		return true;
	}
	return false;
}

void CD3DEdit::GrowTextRect(int Grow)
{
	
	int NewCount=m_WndRectCount+Grow;
	

	LPIBASERECT *pNewRects=new LPIBASERECT[NewCount];
	ZeroMemory(pNewRects,sizeof(LPIBASERECT)*NewCount);
	memcpy(pNewRects,m_pWndRects,sizeof(LPIBASERECT)*m_WndRectCount);
	

	SAFE_DELETE_ARRAY(m_pWndRects);
	m_pWndRects=pNewRects;
	m_WndRectCount=NewCount;	
	m_SelectRectCount+=Grow;
	
}

void CD3DEdit::FormatText(CEasyStringW& Text)
{
	if(m_IsMultiLine)
	{
		Text.Remove('\r');		
	}
	else
	{
		Text.Remove('\r');
		Text.Remove('\n');
	}
	Text.Replace(CEasyStringW("	"),CEasyStringW("    "));
}

void CD3DEdit::CaculateLineInfo(CEasyStringW& Text,CEditLineList& LineInfos)
{
	EDIT_TEXT_LINE_INFO LineInfo;
	FLOAT Width,Height;
	
	CEasyRect ClientRect=GetClientRect();	
	ClientToScreen(&ClientRect);

	LineInfos.clear();	
	LineInfo.StartPos=0;
	LineInfo.Len=0;

	SAFE_DELETE_ARRAY(m_pCharSizes);
	m_pCharSizes=new int[Text.GetLength()+1];
	m_pCharSizes[0]=0;

	if(TextRect()==NULL)
	{
		TextRect()=CreateTextRect();
		RebuildOrder();
	}

	LPCWSTR pp=(LPCWSTR)Text;

	TextRect()->GetTextSizeW((LPCWSTR)Text,(int)Text.GetLength(),Width,Height,m_pCharSizes+1);

	int *pCharSizes=new int[Text.GetLength()+1];
	memcpy(pCharSizes,m_pCharSizes,sizeof(int)*(Text.GetLength()+1));
	
	for(int i=0;i<(int)Text.GetLength();i++)
	{		
		if(Text[i]=='\n')
		{			
			LineInfo.Len=i-LineInfo.StartPos+1;
			LineInfo.CharSizes.clear();
			for(int j=0;j<=LineInfo.Len;j++)
				LineInfo.CharSizes.push_back(pCharSizes[j]);
			LineInfos.push_back(LineInfo);
		

			LineInfo.StartPos=i+1;
			LineInfo.Len=0;		
			TextRect()->GetTextSizeW((LPCWSTR)Text+LineInfo.StartPos,
				(int)Text.GetLength()-LineInfo.StartPos,Width,Height,pCharSizes+1);
		}		

		if(m_AutoWrap&&m_IsMultiLine&&i<((int)Text.GetLength()-1))
		{			
			if(pCharSizes[i+2-LineInfo.StartPos]>(FLOAT)ClientRect.Width())
			{
				while(pCharSizes[i-LineInfo.StartPos]==pCharSizes[i+1-LineInfo.StartPos]&&i)
					i--;
				LineInfo.Len=i-LineInfo.StartPos+1;
				LineInfo.CharSizes.clear();
				for(int j=0;j<=LineInfo.Len;j++)
					LineInfo.CharSizes.push_back(pCharSizes[j]);
				LineInfos.push_back(LineInfo);
				
				LineInfo.StartPos=i+1;
				LineInfo.Len=0;	

				TextRect()->GetTextSizeW((LPCWSTR)Text+LineInfo.StartPos,
					(int)Text.GetLength()-LineInfo.StartPos,Width,Height,pCharSizes+1);
			}
		}
		
	}	
	if(LineInfo.StartPos<=(int)Text.GetLength())
	{
		LineInfo.Len=(int)Text.GetLength()-LineInfo.StartPos;
		LineInfo.CharSizes.clear();
		for(int j=0;j<=LineInfo.Len;j++)
			LineInfo.CharSizes.push_back(pCharSizes[j]);
		LineInfos.push_back(LineInfo);
	}
	if(m_IsEnabledScrollBar&&m_IsMultiLine)
	{
		m_pScrollBar->SetMaxScrollPos((int)LineInfos.size()-1);
		m_pScrollBar->SetScrollPos(m_FirstVisibleLine);
	}	
	UpdateCurLineInfo();

	delete[] pCharSizes;
	
}

void CD3DEdit::IndexToRowCol(int Index,int& Row,int& Col)
{	
	EDIT_TEXT_LINE_INFO * pInfo=NULL;
	Row=0;
	Col=Index;

	if(m_LineInfos.size()<=0)
	{	
		Col=0;
		return;
	}
	
	for(int i=0;i<(int)m_LineInfos.size();i++)
	{
		pInfo=&(m_LineInfos[i]);
		if(Col<pInfo->Len)
			break;
		else if(Col==pInfo->Len)
		{
			if(Row==m_LineInfos.size()-1)
				break;
			if(pInfo->StartPos+pInfo->Len-2<0)
				break;
			if(m_WndText[pInfo->StartPos+pInfo->Len-2]=='\n')
				break;
		}
		Col-=pInfo->Len;
		Row++;
	}
	if(Col>pInfo->Len)
		Col=pInfo->Len;
	if(Col<0)
		Col=0;		
}

void CD3DEdit::RowColToIndex(int& Index,int Row,int Col,bool IsForward)
{	
	Index=0;	
	
	if(Row>=(int)m_LineInfos.size())
		Row=(int)m_LineInfos.size()-1;
	for(int i=0;i<Row;i++)
	{				
		Index+=m_LineInfos[i].Len;		
	}	
	Index+=Col;
	if(Index>(int)m_WndText.GetLength())
		Index=(int)m_WndText.GetLength();
	CheckIndex(Index,IsForward);
}

void CD3DEdit::CheckIndex(int& Index,bool IsForward)
{
	//略过不可见字符,不包括换行符
	LPCWSTR pText=(LPCWSTR)m_WndText;
	if(IsForward)
	{
		if(Index)
		{
			while(m_pCharSizes[Index-1]==m_pCharSizes[Index]&&m_WndText[Index-1]!='\n')
				Index++;
		}
	}
	else
	{
		if(Index)
		{
			while(m_pCharSizes[Index-1]==m_pCharSizes[Index]&&m_WndText[Index-1]!='\n')
				Index--;
		}
	}
}

EDIT_TEXT_LINE_INFO * CD3DEdit::GetLineInfo(int Row)
{
	if(Row>=0&&Row<(int)m_LineInfos.size())
	{
		return &(m_LineInfos[Row]);
	}
	return NULL;
}

void CD3DEdit::UpdateCurLineInfo()
{
	if(m_CurCaretRow<0)
		m_CurCaretRow=0;
	if(m_CurCaretRow>=(int)m_LineInfos.size())
		m_CurCaretRow=(int)m_LineInfos.size()-1;

	EDIT_TEXT_LINE_INFO * pLineInfo=GetLineInfo(m_CurCaretRow);
	if(pLineInfo)
	{
		m_CurLineInfo=*pLineInfo;
	}
	else
	{		
		m_CurLineInfo.StartPos=0;
		m_CurLineInfo.Len=0;		
	}	
}

void CD3DEdit::UpdateCaretPos(bool IsForward)
{	
	int Index;	
	RowColToIndex(Index,m_CurCaretRow,m_CurCaretCol,IsForward);	
	IndexToRowCol(Index,m_CurCaretRow,m_CurCaretCol);
	UpdateCurLineInfo();	
	MakeCaretVisible();
	
}
int CD3DEdit::GetEndCol(int Row)
{
	EDIT_TEXT_LINE_INFO * pLineInfo=GetLineInfo(Row);
	if(pLineInfo)
	{	
		if(Row<(int)m_LineInfos.size()-1)
			return pLineInfo->Len-1;
		else
			return pLineInfo->Len;
	}
	return 0;
}	

bool CD3DEdit::XYToCaretRowCol(int x,int y,int& Row,int& Col)
{		 
	Row=0;
	Col=0;

	for(int i=0;i<m_CurVisibleLineCount;i++)
	{		
		CEasyRect Rect=GetLineRect(i);
		if(Rect.PtInRect(CEasyPoint(x,y)))
		{
			int Len=x-(int)Rect.left;
			int PrevWidth=0;
			int	PrevCol=0;

			Row=i+m_FirstVisibleLine;
			//UpdateCurLineInfo();
			Col=0;
			EDIT_TEXT_LINE_INFO * pLineInfo=GetLineInfo(Row);
			if(pLineInfo==NULL)
				return false;
			int LineStart=0;
			int LineLen=pLineInfo->Len;
			int StartWidth;
			if(!m_IsMultiLine)
			{
				LineStart+=m_FirstVisibleCol;
				LineLen-=m_FirstVisibleCol;
			}
			if(LineStart)
				StartWidth=pLineInfo->CharSizes[LineStart];
			else
				StartWidth=0;
			for(int j=0;j<LineLen;j++)
			{
				int Width;						
				Width=pLineInfo->CharSizes[LineStart+j+1]-StartWidth;						
				if(Len<=Width)
				{
					if((Width-Len)<=(Len-PrevWidth-m_FontCharSpace))
						Col=j+1;
					else
						Col=j;
					
					
					while((LineStart+Col)&&pLineInfo->CharSizes[LineStart+Col-1]==pLineInfo->CharSizes[LineStart+Col]&&
						*((LPCWSTR)m_WndText+pLineInfo->StartPos+LineStart+Col-1)!='\n')
						Col--;
					
					if(!m_IsMultiLine)
					{
						Col+=m_FirstVisibleCol;
					}				
					return true;
				}
				PrevWidth=Width;
				PrevCol=j;						
			}				
			Col=GetEndCol(Row);	
			return true;
		}
		
	}
	return false;
}

bool CD3DEdit::CaretRowColToXY(int& x,int& y1,int& y2,int Row,int Col)
{
	x=0;
	y1=0;
	y2=0;

	int i=Row-m_FirstVisibleLine;
	if(i<0||i>=m_CurVisibleLineCount)
		return false;

	int Width;

	EDIT_TEXT_LINE_INFO * pLineInfo=GetLineInfo(Row);
	if(pLineInfo==NULL)
		return false;
	if(Col>pLineInfo->Len)
		Col=pLineInfo->Len;

	int TestStart,TextEnd;
	TestStart=0;
	TextEnd=TestStart+Col;
	if(!m_IsMultiLine)
	{
		TestStart+=m_FirstVisibleCol;				
	}			
	if(TestStart)
		Width=pLineInfo->CharSizes[TextEnd]-pLineInfo->CharSizes[TestStart];
	else if((TestStart==TextEnd)&&(TextEnd==0))
		Width=0;
	else
		Width=pLineInfo->CharSizes[TextEnd];	

	RECT Rect=GetLineRect(i);
	

	x=(int)(Rect.left+Width);
	y1=(int)Rect.top;
	y2=(int)Rect.bottom;
	
	return true;		
	
}

bool CD3DEdit::DeleteTextAtCaret(bool IsBack)
{	
	if(m_IsReadyOnly)
		return false;
	if(!DeleteSelection())
	{
		int Index,Len;
		RowColToIndex(Index,m_CurCaretRow,m_CurCaretCol);

		if(IsBack)
		{
			Index--;
		}
		
		Len=1;
			
		if(Index>=0&&Index<(int)m_WndText.GetLength())
		{
			CheckIndex(Index,false);
			while(m_pCharSizes[Index]==m_pCharSizes[Index+Len]&&
				Index+Len<(int)m_WndText.GetLength()&&
				m_WndText[Index+Len-1]!='\n')
				Len++;
			m_WndText.Delete(Index,Len);							
			FormatText(m_WndText);
			CaculateLineInfo(m_WndText,m_LineInfos);
			if(IsBack)
			{
				m_CurCaretCol-=Len;
				UpdateCaretPos(false);
			}
			UpdateText();
			HandleMessage(this,WM_D3DGUI_EDIT_TEXT_CHANGED,(WPARAM)GetID(),0);
			return true;
		}	
		
	}
	return false;
}

void CD3DEdit::ClearSelection()
{
	m_SelectStartRow=0;
	m_SelectStartCol=0;
	m_SelectEndRow=0;
	m_SelectEndCol=0;
	UpdateSelect();
}

bool CD3DEdit::DeleteSelection()
{
	if(m_IsReadyOnly)
		return false;
	if(m_SelectStartRow!=m_SelectEndRow||m_SelectStartCol!=m_SelectEndCol)
	{
		int Index;
		int Len;
		RowColToIndex(Index,m_SelectStartRow,m_SelectStartCol);
		RowColToIndex(Len,m_SelectEndRow,m_SelectEndCol);
		Len=Len-Index;
		m_WndText.Delete(Index,Len);
		m_CurCaretRow=m_SelectStartRow;
		m_CurCaretCol=m_SelectStartCol;	
		m_SelectStartRow=0;
		m_SelectStartCol=0;
		m_SelectEndRow=0;
		m_SelectEndCol=0;
		UpdateCaretPos();
		FormatText(m_WndText);
		CaculateLineInfo(m_WndText,m_LineInfos);
		UpdateText();
		HandleMessage(this,WM_D3DGUI_EDIT_TEXT_CHANGED,(WPARAM)GetID(),0);
		return true;
	}
	return false;
}

void CD3DEdit::InsertChar(WCHAR Char,bool WantTrans)
{

	if(m_IsReadyOnly)
		return;
	WCHAR buff[2];
	buff[0]=Char;
	buff[1]=0;
	InsertStr(buff,WantTrans);
}

void CD3DEdit::InsertStr(LPCWSTR Str,bool WantTrans)
{
	int Index;

	if(m_IsReadyOnly)
		return;
	CEasyStringW Text;
	if(WantTrans)//&&m_IsSupportTans)
	{	
		if(TextRect()==NULL)
		{
			TextRect()=CreateTextRect();
			RebuildOrder();
		}
		int TextLen=0;
		TextRect()->TranslateTextW(Str,(int)wcslen(Str),NULL,TextLen);
		Text.Resize(TextLen-1);
		TextRect()->TranslateTextW(Str,(int)wcslen(Str),(LPWSTR)Text.GetBuffer(),TextLen);		
		Text.SetLength(TextLen-1);
	}
	else
	{
		Text=Str;
	}
	DeleteSelection();
	RowColToIndex(Index,m_CurCaretRow,m_CurCaretCol);
	m_WndText.Insert(Index,Text);
	FormatText(m_WndText);
	CaculateLineInfo(m_WndText,m_LineInfos);
	Index+=(int)Text.GetLength();
	IndexToRowCol(Index,m_CurCaretRow,m_CurCaretCol);
	UpdateCaretPos();
	UpdateText();
	HandleMessage(this,WM_D3DGUI_EDIT_TEXT_CHANGED,(WPARAM)GetID(),0);
}

void CD3DEdit::GetMiniSize(int& Width,int& Height)
{
	//检测窗口大小
	int LMinWidth,RMinWidth;

	LMinWidth=m_Borders[RECT_LEFT];
	if(LMinWidth<m_Borders[RECT_TOP_LEFT])
		LMinWidth=m_Borders[RECT_TOP_LEFT];
	if(LMinWidth<m_Borders[RECT_BOTTOM_LEFT])
		LMinWidth=m_Borders[RECT_BOTTOM_LEFT];

	RMinWidth=m_Borders[RECT_RIGHT];
	if(RMinWidth<m_Borders[RECT_TOP_RIGHT])
		RMinWidth=m_Borders[RECT_TOP_RIGHT];
	if(RMinWidth<m_Borders[RECT_BOTTOM_RIGHT])
		RMinWidth=m_Borders[RECT_BOTTOM_RIGHT];

	Width=LMinWidth+RMinWidth+m_ScrollBarWidth;
	if(m_IsEnabledScrollBar&&m_IsMultiLine)
	{
		int MinWidth,MinHeight;
		m_pScrollBar->GetMiniSize(MinWidth,MinHeight);
		Height=m_Borders[RECT_TOP]+m_Borders[RECT_BOTTOM]+MinHeight;
	}
	else
	{
		Height=m_Borders[RECT_TOP]+m_Borders[RECT_BOTTOM];
	}
	
}

void CD3DEdit::MakeCaretVisible()
{
	//保持光标可见

	if(m_IsMultiLine)
	{	
		if(m_CurCaretRow-m_FirstVisibleLine>=m_CurVisibleLineCount)
			SetFirstVisibleLine(m_CurCaretRow-m_CurVisibleLineCount+1);
		if(m_CurCaretRow-m_FirstVisibleLine<0)
			SetFirstVisibleLine(m_CurCaretRow);		
	}
	else
	{
			
		//FLOAT Width=0,Height;
		int x,y1,y2;
		CEasyRect ClientRect=GetClientRect();	
		ClientToScreen(&ClientRect);
		
		if(ClientRect.Width()<=0)
			return;

		if(m_FirstVisibleCol>m_CurCaretCol)
			m_FirstVisibleCol=m_CurCaretCol;


		CaretRowColToXY(x,y1,y2,m_CurCaretRow,m_CurCaretCol);
		
		while(x+CARET_WIDTH>ClientRect.right)
		{
			m_FirstVisibleCol++;
			CaretRowColToXY(x,y1,y2,m_CurCaretRow,m_CurCaretCol);
		}
	
	}
}

RECT CD3DEdit::GetLineRect(int Index)
{
	
	CEasyRect Rect=GetClientRect();	
	ClientToScreen(&Rect);	

	if(m_IsMultiLine)
	{
		Rect.top+=Index*(m_LineHeight+m_FontLineSpace);
		//if(Index)
		//	Rect.top+=(Index-1)*m_FontLineSpace;
		Rect.bottom=Rect.top+m_LineHeight;
	}
	else
	{
		if(m_FontAlign&DT_VCENTER)
		{
			Rect.top=Rect.top+(Rect.Height()-m_LineHeight)/2;
			Rect.bottom=Rect.top+m_LineHeight;
		}
		else if(m_FontAlign&DT_BOTTOM)
		{
			Rect.top=Rect.bottom-m_LineHeight;
			Rect.bottom=Rect.top+m_LineHeight;
		}
	}
	return Rect;
}

bool CD3DEdit::ProcessIME(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(!m_EnableIME)
		return false;

	switch(msg)
	{
	case WM_IME_SETCONTEXT:
		return true;
	case WM_IME_COMPOSITION: 
		{
			m_pIMEReadingWnd->SetVisible(true);
			HIMC hImc;

			hImc=ImmGetContext(m_pGUI->GetHWND());
			if(hImc)
			{	
				CEasyStringW CompStr;	

				CompStr.Resize(MAX_COMPSTRING_SIZE);

				if(lParam&GCS_RESULTSTR)
				{
					int Len=ImmGetCompositionStringW(hImc,GCS_RESULTSTR,CompStr,MAX_COMPSTRING_SIZE);
					Len=Len/sizeof(WCHAR);
					CompStr.TrimBuffer(Len);

					InsertStr(CompStr,false);
					m_pIMEReadingWnd->SetVisible(false);
				}
				else
				{									
					int Len=ImmGetCompositionStringW(hImc,GCS_COMPSTR,CompStr,MAX_COMPSTRING_SIZE);
					Len=Len/sizeof(WCHAR);
					CompStr.TrimBuffer(Len);

					int CursorPos=ImmGetCompositionStringW(hImc,GCS_CURSORPOS,NULL,0);

					CompStr.Insert(CursorPos,'|');
					
					CEasyRect Rect;
					int Width=m_pIMECandidateWnd->GetRect().Width()-
						m_pIMECandidateWnd->GetClientRect().Width();
					int Height=m_pIMECandidateWnd->GetRect().Height()-
						m_pIMECandidateWnd->GetClientRect().Height();

					POINT StartPoint;
					int x,y1,y2;
					CaretRowColToXY(x,y1,y2,m_CurCaretRow,m_CurCaretCol);
					StartPoint.x=x+CARET_WIDTH;
					StartPoint.y=y1;
					ScreenToClient(&StartPoint);

					Rect.top=StartPoint.y;
					Rect.left=StartPoint.x;

					FLOAT StrWidth=100.0f;
					FLOAT StrHeight=20.0f;

					IUITextRect * pTextRect=m_pIMEReadingWnd->GetTextRect();				

					if(pTextRect==NULL)
					{
						pTextRect=TextRect();
					}

					if(pTextRect)
					{
						TextRect()->GetTextSizeW(CompStr,CompStr.GetLength(),StrWidth,StrHeight);
					}					

					Rect.right=Rect.left+StrWidth;
					Rect.bottom=Rect.top+StrHeight;
					m_pIMEReadingWnd->SetRect(Rect);

					m_pIMEReadingWnd->SetTextW(CompStr);
				}			
				
				ImmReleaseContext(m_pGUI->GetHWND(),hImc);
			}
			
		}		
		return true;
	case WM_IME_ENDCOMPOSITION:
		{		
			m_pIMEReadingWnd->SetVisible(false);			
		}
		return true;
	case WM_IME_NOTIFY:
		switch( wParam )
		{
		case IMN_SETCONVERSIONMODE:
			break;
		case IMN_SETOPENSTATUS:
			break;
		case IMN_OPENCANDIDATE:
			m_pIMECandidateWnd->SetVisible(true);			
		case IMN_CHANGECANDIDATE:
			{
				HIMC hImc;

				hImc=ImmGetContext(m_pGUI->GetHWND());
				if(hImc)
				{					
					int BufferSize=ImmGetCandidateListW(hImc,0,NULL,0);
					char * pBuffer=new char[BufferSize];
					LPCANDIDATELIST pCandidateList=(LPCANDIDATELIST)pBuffer;
					ImmGetCandidateListW(hImc,0,pCandidateList,BufferSize);
					CEasyStringW CandidateStrings;
					for(UINT i=pCandidateList->dwPageStart;
						i<pCandidateList->dwPageStart+pCandidateList->dwPageSize;
						i++)
					{
						LPCWSTR pStr=(LPCWSTR)(pBuffer+pCandidateList->dwOffset[i]);
						CandidateStrings+=(char)('1'+i-pCandidateList->dwPageStart);
						CandidateStrings+="、";
						CandidateStrings+=pStr;
						CandidateStrings+="\r\n";
					}
					LPLOGFONT pFont=m_pIMECandidateWnd->GetFont();
					CEasyRect Rect;
					int Width=m_pIMECandidateWnd->GetRect().Width()-
						m_pIMECandidateWnd->GetClientRect().Width();
					int Height=m_pIMECandidateWnd->GetRect().Height()-
						m_pIMECandidateWnd->GetClientRect().Height();

					POINT StartPoint;
					int x,y1,y2;
					CaretRowColToXY(x,y1,y2,m_CurCaretRow,m_CurCaretCol);
					StartPoint.x=x;
					StartPoint.y=y2+5;
					ScreenToClient(&StartPoint);
					
					Rect.top=StartPoint.y;
					Rect.left=StartPoint.x;

					Rect.right=Rect.left+Width+100;
					Rect.bottom=Rect.top+Height+pCandidateList->dwPageSize*(pFont->lfHeight+m_pIMECandidateWnd->GetFontLineSpace());
					m_pIMECandidateWnd->SetRect(Rect);

					m_pIMECandidateWnd->SetTextW(CandidateStrings);
					delete[] pBuffer;
					ImmReleaseContext(m_pGUI->GetHWND(),hImc);

				}				
			}
			break;
		case IMN_CLOSECANDIDATE:
			m_pIMECandidateWnd->SetVisible(false);
			break;
		case IMN_PRIVATE:
			break;

		}
		return true;
	}
	return false;
}

}



