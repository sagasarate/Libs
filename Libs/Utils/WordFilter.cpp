#include "stdafx.h"



CWordFilter::CWordFilter()
{
	
}


CWordFilter::~CWordFilter()
{
}


void CWordFilter::Clear()
{
	m_WordTree.Char = 0;
	m_WordTree.NextCharMap.Clear();
}
bool CWordFilter::AddWord(LPCTSTR szWord, UINT WordTypeMask)
{
	if (szWord)
	{
		CHAR_NODE * pRoot = &m_WordTree;
		size_t Len = _tcslen(szWord);
		if(Len)
		{
			for (size_t i = 0;i < Len;i++)
			{
				TCHAR Char = szWord[i];
				if (((UINT)Char) <= 0x7f)
					Char = toupper(Char);
				CHAR_NODE * pNext = pRoot->NextCharMap.Find(Char);
				if (pNext == NULL)
				{
					pNext = pRoot->NextCharMap.New(Char);
					pNext->Char = Char;
				}
				pRoot = pNext;
			}
			pRoot->WordTypeMask = WordTypeMask;
		}
		return true;
	}
	return false;
}

int CWordFilter::Check(LPCTSTR szText, UINT WordTypeMask, size_t Start, size_t Len)
{
	if (m_WordTree.NextCharMap.GetObjectCount() == 0)
		return -1;
	if (Len == 0)
		Len = _tcslen(szText);
	for (size_t i = Start;i < Len;i++)
	{
		CHAR_NODE * pNode = &m_WordTree;
		size_t Pos = i;
		do
		{
			TCHAR Char = szText[Pos];
			if (((UINT)Char) <= 0x7f)
				Char = toupper(Char);
			pNode = pNode->NextCharMap.Find(Char);
			if(pNode)
			{
				if (pNode->WordTypeMask & WordTypeMask)
					return i;
			}
			Pos++;
			
		} while (pNode);
	}
	return -1;
}