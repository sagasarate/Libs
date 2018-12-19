/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSObjectManager.cpp                                     */
/*      创建日期:  2009年10月23日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

IMPLEMENT_CLASS_INFO_STATIC(CDOSObjectManager, CNameObject);

CDOSObjectManager::CDOSObjectManager(void)
{
	FUNCTION_BEGIN;
	m_pServer=NULL;
	FUNCTION_END;
}

CDOSObjectManager::~CDOSObjectManager(void)
{
	FUNCTION_BEGIN;
	Destory();
	FUNCTION_END;
}

bool CDOSObjectManager::Initialize()
{
	FUNCTION_BEGIN;
	if(m_pServer==NULL)
	{
		PrintDOSLog(_T("没有初始化服务器，对象管理器无法初始化！"));
		return false;
	}
	if(m_pServer->GetConfig().ObjectConfig.ObjectGroupCount<=0)
	{
		PrintDOSLog(_T("服务器没有正确配置对象组数量，对象管理器无法初始化！"));
		return false;
	}
	m_ObjectGroups.Resize(m_pServer->GetConfig().ObjectConfig.ObjectGroupCount);
	for (UINT i = 0; i<m_pServer->GetConfig().ObjectConfig.ObjectGroupCount; i++)
	{
		CDOSObjectGroup * pGroup=new CDOSObjectGroup();
		m_ObjectGroups[i]=pGroup;
		if(pGroup->Initialize(this, i, OBJECT_GROUP_TYPE_NORMAL))
		{
			if (!pGroup->Start())
			{
				PrintDOSLog(_T("无法启动对象组(%u)！"), i);
				return false;
			}
		}
		else
		{
			PrintDOSLog(_T("无法初始化对象组(%u)！"), i);
		}
	}

	PrintDOSLog(_T("对象管理器创建了%d个对象组！"), m_pServer->GetConfig().ObjectConfig.ObjectGroupCount);
	return true;
	FUNCTION_END;
	return false;
}

void CDOSObjectManager::Destory()
{
	FUNCTION_BEGIN;
	for(int i=0;i<(int)m_ObjectGroups.GetCount();i++)
	{
		m_ObjectGroups[i]->SafeTerminate();
	}
	for(int i=0;i<(int)m_ObjectGroups.GetCount();i++)
	{
		SAFE_RELEASE(m_ObjectGroups[i]);
	}
	m_ObjectGroups.Clear();
	FUNCTION_END;
}

void CDOSObjectManager::SuspendAllGroup()
{
	for (UINT i = 0; i < m_ObjectGroups.GetCount(); i++)
	{
		m_ObjectGroups[i]->Suspend();
	}
}

bool CDOSObjectManager::WaitForSuspend(UINT TimeOut)
{
	CEasyTimer Timer;
	Timer.SetTimeOut(TimeOut);
	while (true)
	{
		if (Timer.IsTimeOut())
			return false;

		DoSleep(DEFAULT_IDLE_SLEEP_TIME);

		bool IsAllSuspended = true;
		for (UINT i = 0; i < m_ObjectGroups.GetCount(); i++)
		{
			if (m_ObjectGroups[i]->IsWorking())
			{
				IsAllSuspended = false;
				break;
			}
		}
		if (IsAllSuspended)
			break;
	}
	return true;
}

BOOL CDOSObjectManager::RegisterObject(DOS_OBJECT_REGISTER_INFO& ObjectRegisterInfo)
{
	FUNCTION_BEGIN;



	if(ObjectRegisterInfo.pObject==NULL)
	{
		PrintDOSLog(_T("空对象无法注册！"));
		return FALSE;
	}

	if(ObjectRegisterInfo.Weight<=0)
	{
		PrintDOSLog(_T("对象权重必须大于零！"));
		return FALSE;
	}

	if (ObjectRegisterInfo.ObjectID.ObjectTypeID < DOT_NORMAL_OBJECT)
	{
		PrintDOSLog( _T("对象类型不能使用系统保留类型！"));
		return FALSE;
	}


	ObjectRegisterInfo.pObject->SetManager(this);
	ObjectRegisterInfo.pObject->SetRouter(GetServer()->GetRouter());

	CDOSObjectGroup * pGroup = NULL;
	if (ObjectRegisterInfo.Flag&DOS_OBJECT_REGISTER_FLAG_USE_PRIVATE_OBJECT_GROUP)
	{
		//创建一个私有对象组
		pGroup = new CDOSObjectGroup();
		if(pGroup->Initialize(this, m_ObjectGroups.GetCount(), OBJECT_GROUP_TYPE_PRIVATE))
		{
			if (pGroup->Start())
			{
				m_ObjectGroups.Add(pGroup);
			}
			else
			{
				PrintDOSLog(_T("无法启动私有对象组！"));
				SAFE_DELETE(pGroup);
				return FALSE;
			}
		}
		else
		{
			PrintDOSLog(_T("无法初始化私有对象组！"));
			SAFE_DELETE(pGroup);
			return FALSE;
		}
	}
	else
	{
		pGroup = SelectGroup(ObjectRegisterInfo.ObjectGroupIndex);
	}
	

	if(pGroup==NULL)
	{
		PrintDOSLog(_T("无法分配合适的对象组！"));
		return FALSE;
	}

	ObjectRegisterInfo.ObjectGroupIndex=pGroup->GetIndex();

	ObjectRegisterInfo.ObjectID.RouterID=GetServer()->GetRouter()->GetRouterID();


	if(!pGroup->RegisterObject(ObjectRegisterInfo))
	{
		PrintDOSLog(_T("无法将对象添加到对象组！"));
		return FALSE;
	}

	return TRUE;
	FUNCTION_END;
	return FALSE;

}

BOOL CDOSObjectManager::UnregisterObject(OBJECT_ID ObjectID)
{
	FUNCTION_BEGIN;


	UINT GroupIndex=ObjectID.GroupIndex;
	if(GroupIndex>=m_ObjectGroups.GetCount())
	{
		PrintDOSLog(_T("对象所在组%u无效"),GroupIndex);
		return FALSE;
	}

	CDOSObjectGroup * pGroup=m_ObjectGroups[GroupIndex];

	if(pGroup)
	{
		if(!pGroup->UnregisterObject(ObjectID))
		{
			PrintDOSLog(_T("向对象组请求注销对象失败"));
			return FALSE;
		}
	}
	else
	{
		PrintDOSLog(_T("无法找到对象所在的对象组"));
		return FALSE;
	}

	return TRUE;
	FUNCTION_END;
	return FALSE;

}

BOOL CDOSObjectManager::PushMessage(OBJECT_ID ObjectID,CDOSMessagePacket * pPacket)
{
	FUNCTION_BEGIN;
	UINT GroupIndex=ObjectID.GroupIndex;
	if(GroupIndex==BROAD_CAST_GROUP_INDEX)
	{
		for(UINT i=0;i<m_ObjectGroups.GetCount();i++)
		{
			m_ObjectGroups[i]->PushMessage(ObjectID,pPacket);
		}
		return TRUE;
	}
	else
	{
		if(GroupIndex>=m_ObjectGroups.GetCount())
		{
			PrintDOSLog(_T("对象所在组%u无效"),GroupIndex);
			return FALSE;
		}
		return m_ObjectGroups[GroupIndex]->PushMessage(ObjectID,pPacket);
	}

	FUNCTION_END;
	return FALSE;
}


CDOSObjectGroup * CDOSObjectManager::SelectGroup(int GroupIndex)
{
	FUNCTION_BEGIN;
	if(GroupIndex>=0&&(UINT)GroupIndex<m_ObjectGroups.GetCount())
	{
		return m_ObjectGroups[GroupIndex];
	}
	else
	{
		int Weight=0x7fffffff;
		CDOSObjectGroup * pGroup=NULL;
		for(UINT i=0;i<m_ObjectGroups.GetCount();i++)
		{
			if ((m_ObjectGroups[i]->GetType() == OBJECT_GROUP_TYPE_NORMAL) && (m_ObjectGroups[i]->GetWeight()<Weight))
			{
				Weight=m_ObjectGroups[i]->GetWeight();
				pGroup=m_ObjectGroups[i];
			}
		}
		return pGroup;
	}
	FUNCTION_END;
	return NULL;
}


void CDOSObjectManager::PrintGroupInfo(UINT LogChannel)
{
	FUNCTION_BEGIN;
	for(UINT i=0;i<m_ObjectGroups.GetCount();i++)
	{

		CLogManager::GetInstance()->PrintLog(LogChannel,ILogPrinter::LOG_LEVEL_NORMAL,0,
			_T("对象组[%u](%d):对象数[%u],权重[%u],CPU占用率[%0.2f%%],循环次数[%u],循环时间[%gMS],单循环CPU时间[%lluNS]"),
			i,
			m_ObjectGroups[i]->GetType(),
			m_ObjectGroups[i]->GetObjectCount(),
			m_ObjectGroups[i]->GetWeight(),
			m_ObjectGroups[i]->GetCPUUsedRate()*100,
			m_ObjectGroups[i]->GetCycleCount(),
			m_ObjectGroups[i]->GetCycleTime(),
			m_ObjectGroups[i]->GetCPUUsedTime());

		m_ObjectGroups[i]->PrintObjectStat(LogChannel);
	}
	FUNCTION_END;
}
