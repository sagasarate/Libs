#pragma once


// CPropertyGrid

#include "Inputer.h"
#include "PropertyConfigDialog.h"


#define WM_PROPERTY_GRID_EDIT_START		WM_USER+452
#define WM_PROPERTY_GRID_EDIT_END		WM_USER+453
#define WM_PROPERTY_GRID_VALUE_CHANGED	WM_USER+454

enum PROPERTY_TYPE
{	
	PROT_NONE,
	PROT_EDIT,
	PROT_NUMBER,
	PROT_SPIN_NUMBER,	
	PROT_COMBO,
	PROT_BOOL,
	PROT_COLOR,
	PROT_FONT,
	PROT_COLOREX,
	PROT_DIRECT,
	PROT_PATH,
	PROT_CUSTOM,
};

namespace PropertyGrid
{

class CPropertyValue
{
protected:
	CString		m_Value;
public:
	CPropertyValue(){}
	CPropertyValue(CPropertyValue& val)
	{
		m_Value=val.m_Value;
	}
	CPropertyValue(CString& val)
	{
		m_Value=val;
	}
	CPropertyValue(LPCTSTR val)
	{
		m_Value=val;
	}
	CPropertyValue(int val)
	{
		m_Value.Format(_T("%d"),val);
	}
	CPropertyValue(UINT val)
	{
		m_Value.Format(_T("%d"),val);
	}
	CPropertyValue(long val)
	{
		m_Value.Format(_T("%d"),val);
	}
	CPropertyValue(double val)
	{
		m_Value.Format(_T("%g"),val);
	}
	CPropertyValue(float val)
	{
		m_Value.Format(_T("%g"),val);
	}
	CPropertyValue(bool val)
	{
		val?m_Value=_T("ÊÇ"):m_Value=_T("·ñ");
	}

	CString& GetString()
	{
		return m_Value;
	}
	operator CString()
	{
		return m_Value;
	}
	operator LPCTSTR()
	{
		return (LPCTSTR)m_Value;
	}
	operator int()
	{
		return _tstoi((LPCTSTR)m_Value);
	}
	operator UINT()
	{
		return _tstoi((LPCTSTR)m_Value);
	}
	operator long()
	{
		return _tstoi((LPCTSTR)m_Value);
	}
	operator __int64()
	{
		return _tstoi64((LPCTSTR)m_Value);		
	}
	operator unsigned __int64()
	{
		return _tstoi64((LPCTSTR)m_Value);		
	}
	operator double()
	{
		return _tstof((LPCTSTR)m_Value);
	}
	operator float()
	{
		return (float)_tstof((LPCTSTR)m_Value);
	}
	operator bool()
	{
		return m_Value==_T("ÊÇ");
	}
	CPropertyValue& operator=(const CString& val)
	{
		m_Value=val;
		return *this;
	}
	CPropertyValue& operator=(const LPCTSTR val)
	{
		m_Value=val;
		return *this;
	}
	CPropertyValue& operator=(const int val)
	{
		m_Value.Format(_T("%d"),val);
		return *this;
	}
	CPropertyValue& operator=(const UINT val)
	{
		m_Value.Format(_T("%d"),val);
		return *this;
	}
	CPropertyValue& operator=(const long val)
	{
		m_Value.Format(_T("%d"),val);
		return *this;
	}
	CPropertyValue& operator=(const __int64 val)
	{
		m_Value.Format(_T("%I64d"),val);
		return *this;
	}
	CPropertyValue& operator=(const unsigned __int64 val)
	{
		m_Value.Format(_T("%I64u"),val);
		return *this;
	}
	CPropertyValue& operator=(const double val)
	{
		m_Value.Format(_T("%g"),val);
		return *this;
	}

	CPropertyValue& operator=(const float val)
	{
		m_Value.Format(_T("%g"),val);
		return *this;
	}
	CPropertyValue& operator=(const bool val)
	{
		val?m_Value=_T("ÊÇ"):m_Value=_T("·ñ");
		return *this;
	}

};

struct PROPERTY_INFO
{	
	int							PropertyType;	
	CString						Define;
	CPropertyConfigDialog *		pConfigDialog;
	CString						Name;
	int							ID;
	CPropertyValue				Value;
	int							BindRow;
	PROPERTY_INFO()
	{
		PropertyType=0;
		pConfigDialog=NULL;
		ID=0;
		BindRow=0;
	}
	PROPERTY_INFO(const PROPERTY_INFO& Info)
	{
		PropertyType=Info.PropertyType;
		Define=Info.Define;
		pConfigDialog=Info.pConfigDialog;
		Name=Info.Name;
		ID=Info.ID;
		Value=Info.Value;
		BindRow=Info.BindRow;
	}
};

struct PROPERTY_CHANGE_INFO
{
	CPropertyValue	OldValue;
	CPropertyValue	NewValue;
	PROPERTY_INFO * pInfo;
};

class CPropertyGrid : public CListCtrl
{
	DECLARE_DYNAMIC(CPropertyGrid)
protected:
	CEasyArray<PROPERTY_INFO>	m_PropertyList;
	CInputer					m_Inputer;	
	int							m_CurEditItem;

public:
	CPropertyGrid();
	virtual ~CPropertyGrid();

	BOOL Create(const RECT& Rect, CWnd * pParentWnd, UINT NameColWidth = 80, UINT ValueColWidth = 100);
	BOOL Init(UINT NameColWidth = 80, UINT ValueColWidth = 100);

	void Clear();

	void AddProperty(PROPERTY_INFO * pInfo);
	void SetProperty(PROPERTY_INFO * pInfo,bool AddWhileNotExist=true);
	void SetPropertyValue(int Index,CPropertyValue Value);
	void SetPropertyValueByID(int ID,CPropertyValue Value);
	void SetPropertyValue(LPCTSTR Name,CPropertyValue Value);
	PROPERTY_INFO * GetProperty(int index)
	{
		if(index>=0&&index<m_PropertyList.GetCount())
		{
			return &(m_PropertyList[index]);
		}
		return NULL;
	}
	PROPERTY_INFO * GetPropertyByName(LPCTSTR Name);
	PROPERTY_INFO * GetPropertyByID(int ID);	
	void OnFinishEdit(bool IsChanged);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
};


}

#include "PropertyView.h"

using namespace PropertyGrid;