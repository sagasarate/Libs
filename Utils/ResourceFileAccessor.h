/****************************************************************************/
/*                                                                          */
/*      文件名:    ResourceFileAccessor.h                                   */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


class CResourceFileAccessor :
	public IFileAccessor
{
protected:
	HRSRC			m_hResource;
	HGLOBAL			m_hResourceData;
	ULONG64			m_ResourceDataSize;
	LONG64			m_ReadPtr;
	bool			m_HaveError;

	DECLARE_CLASS_INFO_STATIC(CResourceFileAccessor)
public:
	CResourceFileAccessor(void);
	virtual ~CResourceFileAccessor(void);

	virtual bool Open(LPCTSTR FileName,int OpenMode);
	virtual void Close();

	virtual ULONG64 GetSize();

	virtual ULONG64 Read(LPVOID pBuff,ULONG64 Size);
	virtual ULONG64 Write(LPCVOID pBuff,ULONG64 Size);


	virtual bool IsEOF();

	virtual bool Seek(LONG64 Offset,int SeekMode);
	virtual ULONG64 GetCurPos();

	virtual bool SetCreateTime(const CEasyTime& Time);
	virtual bool GetCreateTime(CEasyTime& Time);

	virtual bool SetLastAccessTime(const CEasyTime& Time);
	virtual bool GetLastAccessTime(CEasyTime& Time);

	virtual bool SetLastWriteTime(const CEasyTime& Time);
	virtual bool GetLastWriteTime(CEasyTime& Time);
	virtual bool HaveError() override
	{
		return m_HaveError;
	}
};

