/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DProgressBar.h                                         */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once
#include "d3dwnd.h"

namespace D3DGUI{

enum PROGRESS_BAR_WORK_MODE
{
	PBW_STRETCH,
	PBW_CUT,
};

enum PROGRESS_BAR_STYLE
{
	PROGRESS_BAR_VERTICAL,
	PROGRESS_BAR_HORIZONTAL,
};

class CD3DProgressBar :
	public CD3DWnd
{
protected:
	CD3DWnd *			m_pBarWnd;
	unsigned __int64	m_MaxPos;
	unsigned __int64	m_CurPos;
	int					m_WorkMode;
	int					m_Style;

	CEasyRect				m_BarWndOrgTextureRect;
	CEasyString				m_BarWndOrgTextureName;

	DECLARE_CLASS_INFO(CD3DProgressBar)
public:
	CD3DProgressBar(void);
	CD3DProgressBar(CD3DGUI * pGUI);
	virtual ~CD3DProgressBar(void);
	virtual void InitWnd(CD3DGUI *  pGUI);

	CD3DWnd * GetBarWnd()
	{
		return m_pBarWnd;
	}

	void SetWorkMode(int Mode);

	int GetWorkMode()
	{
		return m_WorkMode;
	}

	void SetProgressStyle(int Style);

	int GetProgressStyle()
	{
		return m_Style;
	}

	void SetPos(unsigned __int64 Pos);

	unsigned __int64 GetPos()
	{
		return m_CurPos;
	}

	void SetMaxPos(unsigned __int64 MaxPos);

	unsigned __int64 GetMaxPos()
	{
		return m_MaxPos;
	}

	unsigned __int64 GetPosByXY(int x,int y);

	virtual void UpdateRects();

	virtual void SaveToXml(xml_node * pXMLNode);
	virtual bool LoadFromXml(xml_node * pXMLNode);

protected:
	void UpdateProgessBar();
};

}