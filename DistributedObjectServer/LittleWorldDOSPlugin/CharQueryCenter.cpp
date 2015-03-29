#include "StdAfx.h"

START_DOS_MSG_MAP(CCharQueryCenter)
DOS_MSG_MAP(CCharQueryCenterMsgHandler)
END_DOS_MSG_MAP(CCharQueryCenter)

CCharQueryCenter::CCharQueryCenter(void)
{
	INIT_DOS_MSG_MAP;
	m_pOperator=NULL;
}

CCharQueryCenter::~CCharQueryCenter(void)
{
	
}

BOOL CCharQueryCenter::Initialize(IDistributedObjectOperator * pOperator)
{
	m_pOperator=pOperator;

	MSG_ID_TYPE MsgIDs[]={
		MAKE_MSG_ID(INTERFACE_LW_CHAR_QUERY_CENTER,METHOD_CQC_QUERY_CHAR_INFO,false)};

	m_pOperator->RegisterGlobalMsgMap(BROAD_CAST_ROUTER_ID, BROAD_CAST_PROXY_TYPE, MsgIDs, sizeof(MsgIDs) / sizeof(MSG_ID_TYPE));

	if(!m_QueryMap.Create(CMainConfig::GetInstance()->GetCharQueryCenterConfig().QueryPool.StartSize,
		CMainConfig::GetInstance()->GetCharQueryCenterConfig().QueryPool.GrowSize,
		CMainConfig::GetInstance()->GetCharQueryCenterConfig().QueryPool.GrowLimit))
	{
		Log("CCharQueryCenter::Initialize:Pool(%u,%u,%u)创建失败",
			CMainConfig::GetInstance()->GetCharQueryCenterConfig().QueryPool.StartSize,
			CMainConfig::GetInstance()->GetCharQueryCenterConfig().QueryPool.GrowSize,
			CMainConfig::GetInstance()->GetCharQueryCenterConfig().QueryPool.GrowLimit);
		return FALSE;
	}

	return TRUE;
}
void CCharQueryCenter::Destory()
{
	m_pOperator=NULL;
}

int CCharQueryCenter::RegisterChar(UINT64 CharID )
{
	LogDebug("CCharQueryCenter::RegisterChar:角色[0x%llX:0x%llX]注册",
		CharID,GetCurMsgSenderID().ID);
	m_QueryMap.Insert(CharID,GetCurMsgSenderID());
	return COMMON_RESULT_SUCCEED;
}
int CCharQueryCenter::UnregisterChar(UINT64 CharID )
{
	LogDebug("CCharQueryCenter::UnregisterChar:角色[0x%llX:0x%llX]注销",
		CharID,GetCurMsgSenderID().ID);
	m_QueryMap.Delete(CharID);
	return COMMON_RESULT_SUCCEED;
}
int CCharQueryCenter::QueryCharInfo(UINT64 CharID )
{
	OBJECT_ID * pObjectID=m_QueryMap.Find(CharID);
	if(pObjectID)
	{
		CCharacterMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),*pObjectID);
		MsgCaller.QueryCharInfo(GetCurMsgSenderID());
	}
	else
	{
		CCharacterAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),GetCurMsgSenderID());

		CSmartStruct Empty;
		MsgCaller.QueryCharInfoAck(QCIRT_CHAR_NOT_EXIST,Empty);
	}
	return COMMON_RESULT_SUCCEED;
}
