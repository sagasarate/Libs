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
	UINT	m_iDataSize;
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
	BOOL LoadFile( LPCTSTR pszTextFile ,bool bSplitLine=true);
	BOOL LoadFile( IFileAccessor * pFile ,bool bSplitLine=true);
	BOOL LoadFromString(LPCTSTR pStr,int Len=-1,bool bSplitLine=true);
	BOOL SaveToFile(LPCTSTR pszTextFile);
	BOOL SaveToFile(IFileAccessor * pFile);
	TCHAR * operator[]( UINT line );
	TCHAR * GetData()
	{
		return m_pData;
	}
	UINT GetDataLen()
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
	BOOL BuildLines( );
	UINT	ProcData();
	
};