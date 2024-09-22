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
	m_GroupCount = 0;
	m_ObjectGroups.SetTag(_T("CDOSObjectManager"));
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
	DOS_OBJECT_CONFIG& Config = m_pServer->GetConfig().ObjectConfig;
	if (Config.ObjectGroupCount > Config.MaxObjectGroupCount)
	{
		Config.ObjectGroupCount = Config.MaxObjectGroupCount;
		PrintDOSLog(_T("异常对象组数量大于最大对象组数量"));
	}		
	if (Config.ObjectGroupCount <= 0)
	{
		PrintDOSLog(_T("服务器没有正确配置对象组数量，对象管理器无法初始化！"));
		return false;
	}
	CAutoLock Lock(m_GroupListLock);

	m_ObjectGroups.Resize(Config.MaxObjectGroupCount);
	m_GroupCount = Config.ObjectGroupCount;
	for (UINT i = 0; i< m_GroupCount; i++)
	{
		CDOSObjectGroup * pGroup = MONITORED_NEW(_T("CDOSProxyManager"), CDOSObjectGroup);
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
	for (UINT i = 0; i < m_ObjectGroups.GetCount(); i++)
	{
		if (m_ObjectGroups[i])
			m_ObjectGroups[i]->SafeTerminate();
	}
	for (UINT i = 0; i < m_ObjectGroups.GetCount(); i++)
	{
		SAFE_RELEASE(m_ObjectGroups[i]);
	}
	m_GroupCount = 0;
	m_ObjectGroups.Clear();
	FUNCTION_END;
}

void CDOSObjectManager::SuspendAllGroup()
{
	for (UINT i = 0; i < m_GroupCount; i++)
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
		for (UINT i = 0; i < m_GroupCount; i++)
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

bool CDOSObjectManager::RegisterObject(DOS_OBJECT_REGISTER_INFO& ObjectRegisterInfo)
{
	FUNCTION_BEGIN;



	if(ObjectRegisterInfo.pObject==NULL)
	{
		PrintDOSLog(_T("空对象无法注册！"));
		return false;
	}

	if(ObjectRegisterInfo.Weight<=0)
	{
		PrintDOSLog(_T("对象权重必须大于零！"));
		return false;
	}

	if (ObjectRegisterInfo.ObjectID.ObjectTypeID < DOT_NORMAL_OBJECT)
	{
		PrintDOSLog( _T("对象类型不能使用系统保留类型！"));
		return false;
	}


	ObjectRegisterInfo.pObject->SetManager(this);
	ObjectRegisterInfo.pObject->SetRouter(GetServer()->GetRouter());

	CDOSObjectGroup * pGroup = NULL;
	if (ObjectRegisterInfo.Flag&DOS_OBJECT_REGISTER_FLAG_USE_PRIVATE_OBJECT_GROUP)
	{
		if (m_GroupCount < m_ObjectGroups.GetCount())
		{
			//创建一个私有对象组
			CAutoLock Lock(m_GroupListLock);
			pGroup = MONITORED_NEW(_T("CDOSProxyManager"), CDOSObjectGroup);
			if (pGroup->Initialize(this, m_GroupCount, OBJECT_GROUP_TYPE_PRIVATE))
			{
				if (pGroup->Start())
				{
					m_ObjectGroups[m_GroupCount] = pGroup;
					m_GroupCount++;
				}
				else
				{
					PrintDOSLog(_T("无法启动私有对象组！"));
					SAFE_DELETE(pGroup);
					return false;
				}
			}
			else
			{
				PrintDOSLog(_T("无法初始化私有对象组！"));
				SAFE_DELETE(pGroup);
				return false;
			}
		}
		else
		{
			PrintDOSLog(_T("对象组数量已经达到最大！"));
			return false;
		}
	}
	else
	{
		pGroup = SelectGroup(ObjectRegisterInfo.ObjectGroupIndex);
	}
	

	if(pGroup==NULL)
	{
		PrintDOSLog(_T("无法分配合适的对象组！"));
		return false;
	}

	ObjectRegisterInfo.ObjectGroupIndex=pGroup->GetIndex();

	ObjectRegisterInfo.ObjectID.RouterID=GetServer()->GetRouter()->GetRouterID();


	if(!pGroup->RegisterObject(ObjectRegisterInfo))
	{
		PrintDOSLog(_T("无法将对象添加到对象组！"));
		return false;
	}

	return true;
	FUNCTION_END;
	return false;

}

bool CDOSObjectManager::UnregisterObject(OBJECT_ID ObjectID)
{
	FUNCTION_BEGIN;


	UINT GroupIndex = ObjectID.GroupIndex;
	if (GroupIndex >= m_GroupCount)
	{
		PrintDOSLog(_T("对象所在组%u无效"), GroupIndex);
		return false;
	}

	CDOSObjectGroup * pGroup=m_ObjectGroups[GroupIndex];

	if(pGroup)
	{
		if(!pGroup->UnregisterObject(ObjectID))
		{
			PrintDOSLog(_T("向对象组请求注销对象失败"));
			return false;
		}
	}
	else
	{
		PrintDOSLog(_T("无法找到对象所在的对象组"));
		return false;
	}

	return true;
	FUNCTION_END;
	return false;

}

bool CDOSObjectManager::PushMessage(OBJECT_ID ObjectID, CDOSMessagePacket * pPacket)
{
	FUNCTION_BEGIN;
	UINT GroupIndex = ObjectID.GroupIndex;
	if (GroupIndex == BROAD_CAST_GROUP_INDEX)
	{
		for (UINT i = 0; i < m_GroupCount; i++)
		{
			m_ObjectGroups[i]->PushMessage(ObjectID, pPacket);
		}
		return true;
	}
	else
	{
		if (GroupIndex >= m_GroupCount)
		{
			PrintDOSLog(_T("对象所在组%u无效"), GroupIndex);
			return false;
		}
		return m_ObjectGroups[GroupIndex]->PushMessage(ObjectID, pPacket);
	}

	FUNCTION_END;
	return false;
}


CDOSObjectGroup * CDOSObjectManager::SelectGroup(int GroupIndex)
{
	FUNCTION_BEGIN;
	if (GroupIndex >= 0 && (UINT)GroupIndex < m_GroupCount)
	{
		return m_ObjectGroups[GroupIndex];
	}
	else
	{
		int Weight = 0x7fffffff;
		CDOSObjectGroup * pGroup = NULL;
		for (UINT i = 0; i < m_GroupCount; i++)
		{
			if ((m_ObjectGroups[i]->GetType() == OBJECT_GROUP_TYPE_NORMAL) && (m_ObjectGroups[i]->GetWeight() < Weight))
			{
				Weight = m_ObjectGroups[i]->GetWeight();
				pGroup = m_ObjectGroups[i];
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
	for (UINT i = 0; i < m_GroupCount; i++)
	{

		CLogManager::GetInstance()->PrintLog(LogChannel, ILogPrinter::LOG_LEVEL_NORMAL, NULL,
			_T("对象组[%u](%d):对象数[%u],权重[%u],CPU占用率[%0.2f%%],循环次数[%u],循环时间[%gMS],单循环CPU时间[%lluNS]"),
			i,
			m_ObjectGroups[i]->GetType(),
			m_ObjectGroups[i]->GetObjectCount(),
			m_ObjectGroups[i]->GetWeight(),
			m_ObjectGroups[i]->GetCPUUsedRate() * 100,
			m_ObjectGroups[i]->GetCycleCount(),
			m_ObjectGroups[i]->GetCycleTime(),
			m_ObjectGroups[i]->GetCPUUsedTime());

		m_ObjectGroups[i]->PrintObjectStat(LogChannel);
		CLogManager::GetInstance()->PrintLog(LogChannel, ILogPrinter::LOG_LEVEL_NORMAL, NULL, _T(""));
	}
	FUNCTION_END;
}
