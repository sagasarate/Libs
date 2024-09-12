#pragma once
class CWordFilter
{
protected:
	struct CHAR_NODE
	{
		TCHAR						Char;
		UINT						WordTypeMask;
		CEasyMap<TCHAR, CHAR_NODE>	NextCharMap;
		CHAR_NODE()
		{
			Char = 0;
			WordTypeMask = 0;
		}
	};

	CHAR_NODE		m_WordTree;	
public:
	CWordFilter();
	~CWordFilter();

	void Clear();
	bool AddWord(LPCTSTR szWord, UINT WordTypeMask);
	int Check(LPCTSTR szText, UINT WordTypeMask, size_t Start = 0, size_t Len = 0);

	

protected:
	
};

