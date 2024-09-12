/****************************************************************************/
/*                                                                          */
/*      文件名:    NameObject.cpp                                           */
/*      创建日期:  2009年09月25日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

CLASS_INFO	CNameObject::m_CNameObjectClassInfo={_T("CNameObject"),NULL,CNameObject::CreateObject};



CLASS_INFO_MAP CNameObject::m_AllClassInfo;

CClassInfoRegister CNameObject::m_CNameObjectClassInfoRegister(_T("CNameObject"),&m_CNameObjectClassInfo);

CClassInfoRegister::CClassInfoRegister(LPCTSTR ClassName,CLASS_INFO * pClassInfo)
{
	if (CNameObject::m_AllClassInfo.GetBufferSize()==0)
	{
		CNameObject::m_AllClassInfo.Create(64, 128, 128);
	}

	CNameObject::m_AllClassInfo.Insert(ClassName, pClassInfo);
}
CClassInfoRegister::~CClassInfoRegister()
{
}

CNameObject::CNameObject()
{
	m_ID=0;
	m_UseRef=1;
	m_pParam=NULL;
}

CNameObject::~CNameObject()
{

}

bool CNameObject::Initialize()
{
	return true;
}

void CNameObject::Destory()
{
}

void CNameObject::Release()
{
	if (DecUseRef() <= 0)
	{
		MONITORED_DELETE(this);
	}
}

CNameObject * CNameObject::CreateObject()
{
	return MONITORED_NEW(_T("CNameObject"), CNameObject);
}


CLASS_INFO& CNameObject::GetThisClassInfo()
{
	return m_CNameObjectClassInfo;
}

CLASS_INFO * CNameObject::GetClassInfo(LPCTSTR ClassName)
{
	CLASS_INFO ** pInfo = m_AllClassInfo.Find(ClassName);
	if (pInfo)
		return *pInfo;
	return NULL;
}

int CNameObject::GetAllClassCount()
{
	return m_AllClassInfo.GetObjectCount();
}

void CNameObject::RefreshStorageID()
{
	m_StorageID=(UINT)((UINT64)this);
}



bool CNameObject::CloneFrom(CNameObject * pObject,UINT Param)
{
	Destory();

	m_Name=pObject->m_Name;
	//strncpy_0(m_Name,MAX_OBJECT_NAME,pObject->m_Name,MAX_OBJECT_NAME);
	m_StorageID=pObject->m_StorageID;
	m_pParam=pObject->m_pParam;
	return true;
}
bool CNameObject::StealFrom(CNameObject * pObject,UINT Param)
{
	m_Name=pObject->m_Name;
	//strncpy_0(m_Name,MAX_OBJECT_NAME,pObject->m_Name,MAX_OBJECT_NAME);
	m_ID=pObject->m_ID;
	m_pParam=pObject->m_pParam;
	pObject->m_ID=0;
	return true;
}

inline void CNameObject::PickResource(CUSOResourceManager * pResourceManager,UINT Param)
{
}

bool CNameObject::ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	RefreshStorageID();
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_NO_CLASS_NAME,GetClassInfo().ClassName));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_NO_OBJECT_NAME,(LPCTSTR)m_Name));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_NO_OBJECT_ID,m_ID));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_NO_STORAGE_ID,m_StorageID));
	return true;
}

bool CNameObject::FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	void * Pos=Packet.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartValue Value=Packet.GetNextMember(Pos,MemberID);
		switch(MemberID)
		{
		case SST_NO_CLASS_NAME:
			if(!IsKindOf(Value))
				return false;
			break;
		case SST_NO_OBJECT_NAME:
			m_Name=(LPCTSTR)Value;
			break;
		case SST_NO_OBJECT_ID:
			m_ID=Value;
			break;
		case SST_NO_STORAGE_ID:
			m_StorageID=Value;
			break;
		}
	}
	return true;
}

 UINT CNameObject::GetSmartStructSize(UINT Param)
{
	return CSmartStruct::GetStringMemberSize(GetClassInfo().ClassName) +
		CSmartStruct::GetStringMemberSize(m_Name) +
		CSmartStruct::GetFixMemberSize(sizeof(m_ID)) +
		CSmartStruct::GetFixMemberSize(sizeof(m_StorageID));
}


//CNameObject::STORAGE_STRUCT * CNameObject::USOCreateHead(UINT Param)
//{
//	STORAGE_STRUCT * pHead=new STORAGE_STRUCT;
//	ZeroMemory(pHead,sizeof(STORAGE_STRUCT));
//	pHead->Size=sizeof(STORAGE_STRUCT);
//	return pHead;
//}
//int CNameObject::USOWriteHead(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param)
//{
//	if(pHead==NULL)
//		return false;
//	strncpy_0(pHead->Type,MAX_TYPE_LEN,GetClassInfo().ClassName,MAX_TYPE_LEN);
//	strncpy_0(pHead->Name,MAX_OBJECT_NAME,GetName(),MAX_OBJECT_NAME);
//	pHead->StorageID=GetStorageID();
//	pHead->ID=GetID();
//	return sizeof(STORAGE_STRUCT);
//}
//bool CNameObject::USOWriteData(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param)
//{
//	return true;
//}
//bool CNameObject::USOWriteChild(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param)
//{
//	return true;
//}
//bool CNameObject::USOWriteFinish(CNameObject::STORAGE_STRUCT * pHead,UINT Param)
//{
//	return true;
//}
//
//int CNameObject::USOReadHead(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param)
//{
//	pHead->Name[MAX_OBJECT_NAME-1]=0;
//	SetName(pHead->Name);
//	SetStorageID(pHead->StorageID);
//	SetID(pHead->ID);
//	return sizeof(STORAGE_STRUCT);
//}
//int CNameObject::USOReadData(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,BYTE * pData,int DataSize,UINT Param)
//{
//	return 0;
//}
//bool CNameObject::USOReadChild(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param)
//{
//	return true;
//}
//bool CNameObject::USOReadFinish(CNameObject::STORAGE_STRUCT * pHead,UINT Param)
//{
//	return true;
//}
