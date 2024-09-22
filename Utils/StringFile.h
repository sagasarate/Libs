/****************************************************************************/
/*                                                                          */
/*      文件名:    StringFile.h                                             */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


class CStringFile
{
protected:
	TCHAR *	m_pData;
	size_t	m_iDataSize;
	UINT	m_iLineCount;
	TCHAR**	m_pLines;
	int		m_FileChannel;
	TCHAR	m_LineDelimiter;
	int		m_LocalCodePage;
	int		m_SaveCodePage;
public:
	CStringFile( LPCTSTR pszTextFile ,bool bSplitLine=true,int FileChannel=0);
	CStringFile(int FileChannel=0);
	~CStringFile();
	void Destroy();
	void MakeDeflate();
	bool LoadFile( LPCTSTR pszTextFile ,bool bSplitLine=true);
	bool LoadFile( IFileAccessor * pFile ,bool bSplitLine=true);
	bool LoadFromString(LPCTSTR pStr,int Len=-1,bool bSplitLine=true);
	bool SaveToFile(LPCTSTR pszTextFile);
	bool SaveToFile(LPCTSTR pData, LPCTSTR pszTextFile);
	bool SaveToFile(IFileAccessor * pFile);
	bool SaveToFile(LPCTSTR, IFileAccessor * pFile);
	TCHAR * operator[]( UINT line );
	TCHAR * GetData()
	{
		return m_pData;
	}
	size_t GetDataLen()
	{
		return m_iDataSize;
	}
	UINT	GetLineCount()
	{
		return m_iLineCount;
	}
	void SetLineDelimiter(TCHAR Delimiter)
	{
		m_LineDelimiter=Delimiter;
	}
	void SetLocalCodePage(int CodePage)
	{
		m_LocalCodePage=CodePage;
	}
	int GetLocalCodePage()
	{
		return m_LocalCodePage;
	}
	void SetSaveCodePage(int CodePage)
	{
		m_SaveCodePage=CodePage;
	}
	int GetSaveCodePage()
	{
		return m_SaveCodePage;
	}
protected:
	bool BuildLines( );
	UINT	ProcData();
	
};