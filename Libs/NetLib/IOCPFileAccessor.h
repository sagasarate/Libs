/****************************************************************************/
/*                                                                          */
/*      文件名:    IOCPFileAccessor.h                                       */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

struct FIOCP_ERROR_INFO
{
	UINT	ErrorType;
	UINT	ErrorCode;
	int		QueryType;
	ULONG64	QueryPos;
	UINT	QuerySize;
};

enum FIOCP_QUERY_TYPE
{
	FQT_NONE,
	FQT_READ=IO_FILE_READ,
	FQT_WRITE=IO_FILE_WRITE,
};

enum FIOCP_ERROR_TYPE
{
	FET_UNKNOW,
	FET_SYSTEM_ERROR,
	FET_QUEUE_ERROR,	
};

enum FIOCP_QUEUE_ERROR_CODE
{
	FQEC_QUEUE_FULL,
};

class CIOCPFileAccessor :
	public CNameObject,public IIOCPEventHandler
{
protected:
	volatile HANDLE								m_FileHandle;
	CNetServer*									m_pServer;	
	volatile BOOL								m_WantClose;
	CThreadSafeIDStorage<COverLappedObject *>	m_DataQueue;
	CThreadSafeIDStorage<COverLappedObject *>	m_RequestQueue;
	CThreadSafeIDStorage<FIOCP_ERROR_INFO>		m_ErrorQueue;
	volatile UINT								m_Session;
	CIOCPEventRouter *							m_pIOCPEventRouter;
	ULONG64_CONVERTER							m_ReadPosition;
	ULONG64_CONVERTER							m_WritePosition;
	bool										m_UseAsyncQuery;

	DECLARE_CLASS_INFO_STATIC(CIOCPFileAccessor);
public:
	CIOCPFileAccessor(void);
	~CIOCPFileAccessor(void);

	virtual BOOL OnIOCPEvent(int EventID,COverLappedObject * pOverLappedObject);
	
	virtual void Destory();

	HANDLE GetFileHandle();
	void SetServer(CNetServer* pServer);
	CNetServer* GetServer();		

	BOOL Open(LPCTSTR lpFileName,
		DWORD dwDesiredAccess=GENERIC_READ|GENERIC_WRITE, 
		DWORD dwShareMode=FILE_SHARE_READ|FILE_SHARE_WRITE, 
		LPSECURITY_ATTRIBUTES lpSecurityAttributes=NULL, 
		DWORD dwCreationDisposition=OPEN_ALWAYS, 
		DWORD dwFlagsAndAttributes=0,
		int DataQueueSize=DEFAULT_FILE_ACCESSOR_DATA_QUEUE,
		int RequestQueuSize=DEFAULT_FILE_ACCESSOR_REQUEST_QUEUE,
		int ErrorQueuSize=DEFAULT_FILE_ACCESSOR_ERROR_QUEUE);
	BOOL OpenByHandle(HANDLE FileHandle,
		int DataQueueSize=DEFAULT_FILE_ACCESSOR_DATA_QUEUE,
		int RequestQueuSize=DEFAULT_FILE_ACCESSOR_REQUEST_QUEUE,
		int ErrorQueuSize=DEFAULT_FILE_ACCESSOR_ERROR_QUEUE);
	void Close();
	void QueryClose();

	virtual void OnOpen();
	virtual void OnClose();

	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);

	virtual void OnReadData(ULONG64 ReadPosition,const CEasyBuffer& DataBuffer);
	virtual void OnWriteData(ULONG64 WritePosition,const CEasyBuffer& DataBuffer);
	virtual void OnError(UINT ErrorType,UINT ErrorCode,int QueryType,ULONG64 QueryPos,ULONG64 QuerySize);

	BOOL QueryRead(ULONG64 ReadSize);
	BOOL QueryRead(ULONG64 StartPos,ULONG64 ReadSize);
	BOOL QueryWrite(LPVOID pData,ULONG64 WriteSize);
	BOOL QueryWrite(ULONG64 StartPos,LPVOID pData,ULONG64 WriteSize);

	ULONG64 GetFileSize();
	BOOL SeekRead(LONG64 Offset,int SeekMode);
	BOOL SeekWrite(LONG64 Offset,int SeekMode);
	ULONG64 GetCurReadPos();
	ULONG64 GetCurWritePos();
	int GetCurRequestLen();

	BOOL IsOpen();

protected:
	BOOL DoOverlappedOperation(COverLappedObject * pOverLappedObject);
	BOOL DoNextOverlappedOperation();
	void PushError(UINT ErrorType,UINT ErrorCode,int QueryType=FQT_NONE,UINT QueryPosHigh=0,UINT QueryPosLow=0,UINT QuerySize=0);
};

inline HANDLE CIOCPFileAccessor::GetFileHandle()
{
	return m_FileHandle;
}


inline void CIOCPFileAccessor::SetServer(CNetServer* pServer)
{
	m_pServer=pServer;
}

inline CNetServer* CIOCPFileAccessor::GetServer()
{
	return m_pServer;
}

inline int CIOCPFileAccessor::GetCurRequestLen()
{
	return m_RequestQueue.GetObjectCount();
}

inline BOOL CIOCPFileAccessor::IsOpen()
{
	return m_FileHandle!=INVALID_HANDLE_VALUE;
}