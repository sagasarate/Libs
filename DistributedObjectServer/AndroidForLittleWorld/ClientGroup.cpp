#include "StdAfx.h"


CClientGroup::CClientGroup(void)
{
	m_Index=0;
	m_pServer=NULL;
	m_pMainControlView=NULL;
}

CClientGroup::~CClientGroup(void)
{	
	m_ClientList.Destory();
}


bool CClientGroup::Init(CNetServer * pServer,CMainControlView * pMainControlView,CLIENT_GROUP_CONFIG& GroupConfig)
{
	if(pServer==NULL||pMainControlView==NULL)
		return false;
	m_pServer=pServer;
	m_pMainControlView=pMainControlView;
	m_GroupConfig=GroupConfig;

	if(GroupConfig.ClientCount>=0xFFFF)
		return false;

	if(!m_ClientList.Create(GroupConfig.ClientCount))
		return false;

	if(!m_AroundCharList.Create(MAX_GROUP_CHAR_LIST_COUNT,MAX_CHAR_LIST_COUNT))
		return false;

	for(UINT i=0;i<GroupConfig.ClientCount;i++)
	{
		CLIENT_CONFIG ClientConfig=GroupConfig;

		ClientConfig.AccountName.Format(GroupConfig.AccountName,GroupConfig.StartIndex+i);
		ClientConfig.FirstConnectDelay=GroupConfig.FirstConnectDelay*(i+1);

		CClient * pClient=m_ClientList.NewObject();
		if(pClient)
		{
			if(pClient->Init(this,pServer,pMainControlView,ClientConfig))
			{
				
			}
			else
			{
				SAFE_RELEASE(pClient);
			}
		}

	}
	return true;
}


BOOL CClientGroup::OnStart()
{
	void * Pos=m_ClientList.GetFirstObjectPos();
	while(Pos)
	{
		CClient * pClient=m_ClientList.GetNext(Pos);
		pClient->StartRun();
	}	
	return TRUE;
}

BOOL CClientGroup::OnRun()
{
	int ProcessCount=0;

	UINT CurTime=CEasyTimer::GetTime();
	if(m_CharMoveStatusUpdateTimer.IsTimeOut(CHAR_MOVE_STATUS_UPDATE_TIME))
	{
		m_CharMoveStatusUpdateTimer.SaveTime();

		void * Pos=m_AroundCharList.GetFirstObjectPos();
		while(Pos)
		{
			UINT64 Key;
			MAP_CHAR_INFO * pCharInfo=m_AroundCharList.GetNextObject(Pos,Key);
			if(pCharInfo)
			{
				pCharInfo->MoveStatus.Update(CurTime);
				ProcessCount++;				
			}
		}
	}

	void * Pos=m_ClientList.GetFirstObjectPos();
	while(Pos)
	{
		CClient * pClient=m_ClientList.GetNext(Pos);
		ProcessCount+=pClient->Update();
	}	
	if(ProcessCount==0)
	{
		Sleep(1);
	}
	return TRUE;
}
void CClientGroup::OnTerminate()
{
	m_ClientList.Destory();
}


MAP_CHAR_INFO * CClientGroup::AddChar(UINT64 CharID,CSmartStruct& MoveStatus)
{
	MAP_CHAR_INFO * pCharInfo=NULL;
	if(m_AroundCharList.New(CharID,&pCharInfo))
	{
		pCharInfo->CharID=CharID;
		pCharInfo->RefCount++;
		pCharInfo->MoveStatus.SetMoveStatus(CEasyTimer::GetTime(),MoveStatus);

		return pCharInfo;
	}

	return NULL;
}
bool CClientGroup::DeleteChar(UINT64 CharID)
{
	MAP_CHAR_INFO * pCharInfo=m_AroundCharList.Find(CharID);
	if(pCharInfo)
	{
		pCharInfo->RefCount--;
		if(pCharInfo->RefCount<=0)
		{
			m_AroundCharList.Delete(CharID);
		}
	}
	return false;
}
MAP_CHAR_INFO * CClientGroup::UpdateCharMoveStatus(UINT64 CharID,CSmartStruct& MoveStatus)
{
	MAP_CHAR_INFO * pCharInfo=m_AroundCharList.Find(CharID);
	if(pCharInfo)
	{
		pCharInfo->MoveStatus.SetMoveStatus(CEasyTimer::GetTime(),MoveStatus);

		return pCharInfo;
	}
	return NULL;
}

bool CClientGroup::UpdateCharInfo(UINT64 CharID,CSmartStruct& CharData)
{
	MAP_CHAR_INFO * pCharInfo=m_AroundCharList.Find(CharID);
	if(pCharInfo)
	{
		pCharInfo->BaseCharInfo.ParsePacket(CharData,MF_BASE_CHAR_INFO);

		return true;
	}
	return false;
}
MAP_CHAR_INFO * CClientGroup::GetCharInfo(UINT64 CharID)
{
	return m_AroundCharList.Find(CharID);
}