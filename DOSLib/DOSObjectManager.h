/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSObjectManager.h                                       */
/*      创建日期:  2009年10月23日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


class CDOSServer;



class CDOSObjectManager :
	public CNameObject
{
protected:
	CDOSServer *						m_pServer;	
	CEasyArray<CDOSObjectGroup *>		m_ObjectGroups;	
	volatile UINT						m_GroupCount;
	CEasyCriticalSection				m_GroupListLock;

	DECLARE_CLASS_INFO_STATIC(CDOSObjectManager);
public:
	CDOSObjectManager(void);
	virtual ~CDOSObjectManager(void);

	virtual bool Initialize();
	virtual void Destory();

	void SuspendAllGroup();
	bool WaitForSuspend(UINT TimeOut = -1);

	bool RegisterObject(DOS_OBJECT_REGISTER_INFO& ObjectRegisterInfo);
	bool UnregisterObject(OBJECT_ID ObjectID);

	void SetServer(CDOSServer * pServer);
	CDOSServer * GetServer();

	UINT GetObejctCount();
	void PrintGroupInfo(UINT LogChannel);
	UINT GetGroupCount();
	CDOSObjectGroup * GetGroup(UINT Index);
	
	bool PushMessage(OBJECT_ID ObjectID,CDOSMessagePacket * pPacket);

protected:

	CDOSObjectGroup * SelectGroup(int GroupIndex);

};

inline void CDOSObjectManager::SetServer(CDOSServer * pServer)
{
	m_pServer=pServer;
}
inline CDOSServer * CDOSObjectManager::GetServer()
{
	return m_pServer;
}

inline UINT CDOSObjectManager::GetObejctCount()
{
	UINT Count=0;
	for (UINT i = 0; i < m_GroupCount; i++)
	{
		Count += m_ObjectGroups[i]->GetObjectCount();
	}
	return Count;
}
inline UINT CDOSObjectManager::GetGroupCount()
{
	return m_GroupCount;
}
inline CDOSObjectGroup * CDOSObjectManager::GetGroup(UINT Index)
{
	if (Index < m_GroupCount)
		return m_ObjectGroups[Index];
	return NULL;
}

