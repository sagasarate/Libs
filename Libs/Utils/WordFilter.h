#pragma once
class CWordFilter
{
protected:
	struct CHAR_NODE
	{
		TCHAR						Char;
		bool						HavwWord;
		CEasyMap<TCHAR, CHAR_NODE>	NextCharMap;
		CHAR_NODE()
		{
			Char = 0;
			HavwWord = false;
		}
	};

	CHAR_NODE		m_WordTree;	
public:
	CWordFilter();
	~CWordFilter();

	void Clear();
	bool AddWord(LPCTSTR szWord);
	bool Check(LPCTSTR szText, size_t Start = 0, size_t Len = 0);

	

protected:
	
};

