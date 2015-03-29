/****************************************************************************/
/*                                                                          */
/*      文件名:    StandardFileAccessor.h                                   */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CStandardFileAccessor :
	public IFileAccessor
{
protected:
	FILE *	m_hFile;
	bool	m_IsWriteFlush;

	DECLARE_CLASS_INFO_STATIC(CStandardFileAccessor)
public:
	CStandardFileAccessor(void);
	virtual ~CStandardFileAccessor(void);

	virtual BOOL Open(LPCTSTR FileName,int OpenMode);
	virtual void Close();

	virtual ULONG64 GetSize();

	virtual ULONG64 Read(LPVOID pBuff,ULONG64 Size);
	virtual ULONG64 Write(LPCVOID pBuff,ULONG64 Size);


	virtual BOOL IsEOF();

	virtual BOOL Seek(LONG64 Offset,int SeekMode);
	virtual ULONG64 GetCurPos();

	virtual BOOL SetCreateTime(const CEasyTime& Time);
	virtual BOOL GetCreateTime(CEasyTime& Time);

	virtual BOOL SetLastAccessTime(const CEasyTime& Time);
	virtual BOOL GetLastAccessTime(CEasyTime& Time);

	virtual BOOL SetLastWriteTime(const CEasyTime& Time);
	virtual BOOL GetLastWriteTime(CEasyTime& Time);
	
};
