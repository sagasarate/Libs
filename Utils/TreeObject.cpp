/****************************************************************************/
/*                                                                          */
/*      文件名:    TreeObject.cpp                                           */
/*      创建日期:  2009年09月25日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

IMPLEMENT_CLASS_INFO(CTreeObject,CNameObject);

CTreeObject::CTreeObject(void)
{
	m_pParent=NULL;
	m_StorageID=(UINT)((UINT64)this);
	m_ChildList.SetTag(_T("CTreeObject"));
	m_ChildList.Create(0,5);
}

CTreeObject::~CTreeObject(void)
{
	Destory();
}

void CTreeObject::Destory()
{
	SetParent(NULL);
	ClearAllChild();
	CNameObject::Destory();
}

void CTreeObject::SetParent(CTreeObject* pParent)
{
	if(m_pParent)
	{
		m_pParent->DeleteChild(this);
	}
	m_pParent=pParent;
	if(m_pParent)
	{
		m_pParent->AddChild(this);
	}

}

CTreeObject* CTreeObject::GetParent()
{
	return m_pParent;
}

UINT CTreeObject::GetChildCount()
{
	return (UINT)m_ChildList.GetCount();
}

CTreeObject * CTreeObject::GetChildByIndex(UINT Index)
{
	if(Index<m_ChildList.GetCount())
		return m_ChildList[Index];
	else
		return NULL;
}

CTreeObject * CTreeObject::GetChild(LPCTSTR ObjName,bool NoCase)
{
	for(UINT i=0;i<m_ChildList.GetCount();i++)
	{
		if(NoCase)
		{
			if(_tcsicmp(m_ChildList[i]->GetName(),ObjName)==0)
				return m_ChildList[i];
		}
		else
		{
			if(_tcscmp(m_ChildList[i]->GetName(),ObjName)==0)
				return m_ChildList[i];
		}
	}
	return NULL;
}

CTreeObject * CTreeObject::GetChild(UINT ObjID)
{
	for(UINT i=0;i<m_ChildList.GetCount();i++)
	{
		if(m_ChildList[i]->GetID()==ObjID)
			return m_ChildList[i];
	}
	return NULL;
}

CTreeObject * CTreeObject::GetChildByStorageID(UINT StorageID)
{
	for(UINT i=0;i<m_ChildList.GetCount();i++)
	{
		if(m_ChildList[i]->GetStorageID()==StorageID)
			return m_ChildList[i];
	}
	return NULL;
}

CTreeObject * CTreeObject::GetChildRecursive(LPCTSTR ObjName,bool NoCase)
{
	CTreeObject * pObject=GetChild(ObjName,NoCase);
	if(pObject)
		return pObject;
	for(UINT i=0;i<m_ChildList.GetCount();i++)
	{
		pObject=m_ChildList[i]->GetChildRecursive(ObjName,NoCase);
		if(pObject)
			return pObject;
	}
	return NULL;
}

CTreeObject * CTreeObject::GetChildRecursive(UINT ObjID)
{
	CTreeObject * pObject=GetChild(ObjID);
	if(pObject)
		return pObject;
	for(UINT i=0;i<m_ChildList.GetCount();i++)
	{
		pObject=m_ChildList[i]->GetChildRecursive(ObjID);
		if(pObject)
			return pObject;
	}
	return NULL;
}

CTreeObject * CTreeObject::GetChildByStorageIDRecursive(UINT StorageID)
{
	CTreeObject * pObject=GetChildByStorageID(StorageID);
	if(pObject)
		return pObject;
	for(UINT i=0;i<m_ChildList.GetCount();i++)
	{
		pObject=m_ChildList[i]->GetChildByStorageIDRecursive(StorageID);
		if(pObject)
			return pObject;
	}
	return NULL;
}

void CTreeObject::ClearAllChild()
{
	for(int i=(int)m_ChildList.GetCount()-1;i>=0;i--)
	{
		CTreeObject * pClient=m_ChildList[i];
		pClient->SetParent(NULL);
		SAFE_RELEASE(pClient);
	}
	m_ChildList.Clear();
}

bool CTreeObject::CloneFrom(CNameObject * pObject,UINT Param)
{
	if(!CNameObject::CloneFrom(pObject,Param))
		return false;

	CTreeObject * pSource=(CTreeObject *)pObject;

	if((Param&OBJECT_CLONE_WITHOUT_CHILD)==0)
	{
		for(int i=0;i<(int)pSource->m_ChildList.GetCount();i++)
		{
			CTreeObject * pNewChild=dynamic_cast<CTreeObject *>(pSource->m_ChildList[i]->GetClassInfo().CreateObject());
			if(pNewChild)
			{
				if(pNewChild->CloneFrom(pSource->m_ChildList[i]))
				{
					pNewChild->SetParent(this);
				}
				else
				{
					pNewChild->Release();
				}
			}
		}
	}
	return true;
}

bool CTreeObject::ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CNameObject::ToSmartStruct(Packet,pResourceManager,Param))
		return false;
	if((Param&OPP_WITHOUT_CHILD)==0)
	{
		for(UINT i=0;i<m_ChildList.GetCount();i++)
		{
			UINT BufferSize;
			void * pBuffer=Packet.PrepareMember(BufferSize);
			CSmartStruct ChildPacket(pBuffer,BufferSize,true);
			if(!m_ChildList[i]->ToSmartStruct(ChildPacket,pResourceManager,Param))
				return false;
			if(!Packet.FinishMember(SST_TO_CHILD,ChildPacket.GetDataLen()))
				return false;
		}
	}
	return true;
}
bool CTreeObject::FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CNameObject::FromSmartStruct(Packet,pResourceManager,Param))
		return false;

	if((Param&OPP_WITHOUT_CHILD)==0)
	{
		void * Pos=Packet.GetFirstMemberPosition();
		while(Pos)
		{
			WORD MemberID;
			CSmartStruct ChildPacket=Packet.GetNextMember(Pos,MemberID);
			if(MemberID==SST_TO_CHILD)
			{
				LPCTSTR ClassName=ChildPacket.GetMember(SST_NO_CLASS_NAME);
				LPCTSTR ObjectName=ChildPacket.GetMember(SST_NO_OBJECT_NAME);
				CNameObject * pObject=pResourceManager->CreateObject(ClassName,ObjectName);
				if(pObject==NULL)
					return false;
				if(!pObject->IsKindOf(GET_CLASS_INFO(CTreeObject)))
				{
					SAFE_RELEASE(pObject);
					return false;
				}
				if(!pObject->FromSmartStruct(ChildPacket,pResourceManager,Param))
					return false;
				((CTreeObject *)pObject)->SetParent(this);
			}
		}
	}
	return true;
}
UINT CTreeObject::GetSmartStructSize(UINT Param)
{
	UINT Size=CNameObject::GetSmartStructSize(Param);
	if((Param&OPP_WITHOUT_CHILD)==0)
	{
		for(UINT i=0;i<m_ChildList.GetCount();i++)
		{
			Size+=CSmartStruct::GetFixMemberSize(m_ChildList[i]->GetSmartStructSize(Param));
		}
	}
	return Size;
}


void CTreeObject::RefreshStorageIDRecursive()
{
	RefreshStorageID();
	for(UINT i=0;i<m_ChildList.GetCount();i++)
	{
		m_ChildList[i]->RefreshStorageID();
	}
}

bool CTreeObject::AddChild(CTreeObject* pChild,bool CheckExist)
{
	if(CheckExist)
	{
		for(UINT i=0;i<m_ChildList.GetCount();i++)
		{
			if(m_ChildList[i]==pChild)
				return false;
		}
	}
	m_ChildList.Add(pChild);
	return true;
}

bool CTreeObject::DeleteChild(CTreeObject* pChild)
{
	for(UINT i=0;i<m_ChildList.GetCount();i++)
	{
		if(m_ChildList[i]==pChild)
		{
			m_ChildList.Delete(i);
			return true;;
		}
	}
	return false;
}
