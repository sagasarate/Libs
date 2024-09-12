/****************************************************************************/
/*                                                                          */
/*      文件名:    LinuxFileAccessor.h                                      */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CLinuxFileAccessor :
	public IFileAccessor
{
protected:
	int			m_FileDescriptor;
	bool		m_IsWriteFlush;
	CFileInfo	m_FileInfo;
	bool		m_HaveError;

	DECLARE_CLASS_INFO_STATIC(CLinuxFileAccessor)
public:
	CLinuxFileAccessor(void);
	~CLinuxFileAccessor(void);

	virtual bool Open(LPCTSTR FileName,int OpenMode) override;
	virtual void Close() override;

	virtual ULONG64 GetSize() override;

	virtual ULONG64 Read(LPVOID pBuff,ULONG64 Size) override;
	virtual ULONG64 Write(LPCVOID pBuff,ULONG64 Size) override;


	virtual bool IsEOF() override;

	virtual bool Seek(LONG64 Offset,int SeekMode) override;
	virtual ULONG64 GetCurPos() override;

	virtual bool SetCreateTime(const CEasyTime& Time) override;
	virtual bool GetCreateTime(CEasyTime& Time) override;

	virtual bool SetLastAccessTime(const CEasyTime& Time) override;
	virtual bool GetLastAccessTime(CEasyTime& Time) override;

	virtual bool SetLastWriteTime(const CEasyTime& Time) override;
	virtual bool GetLastWriteTime(CEasyTime& Time) override;
	virtual bool HaveError() override
	{
		return m_HaveError;
	}

	int GetFileDescriptor()
	{
		return m_FileDescriptor;
	}
};
