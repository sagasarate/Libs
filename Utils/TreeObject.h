/****************************************************************************/
/*                                                                          */
/*      文件名:    TreeObject.h                                             */
/*      创建日期:  2009年09月25日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


class CTreeObject :
	public CNameObject
{
protected:

	enum SST_MEMBER_ID
	{
		SST_TO_CHILD=SST_NO_MAX,
		SST_TO_MAX=SST_NO_MAX+50,
	};
	
	CTreeObject *				m_pParent;
	CEasyArray<CTreeObject *>	m_ChildList;

	DECLARE_CLASS_INFO(CTreeObject)
public:
	CTreeObject(void);
	virtual ~CTreeObject(void);

	

	virtual void Destory();
	virtual void SetParent(CTreeObject* pParent);
	virtual CTreeObject* GetParent();
	virtual UINT GetChildCount();
	virtual CTreeObject * GetChildByIndex(UINT Index);
	virtual CTreeObject * GetChild(LPCTSTR ObjName,bool NoCase=false);
	virtual CTreeObject * GetChild(UINT ObjID);
	virtual CTreeObject * GetChildByStorageID(UINT StorageID);
	virtual CTreeObject * GetChildRecursive(LPCTSTR ObjName,bool NoCase=false);
	virtual CTreeObject * GetChildRecursive(UINT ObjID);
	virtual CTreeObject * GetChildByStorageIDRecursive(UINT StorageID);
	virtual void ClearAllChild();

	virtual bool CloneFrom(CNameObject * pObject,UINT Param=0);

	virtual bool ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual UINT GetSmartStructSize(UINT Param=0);
	
	void RefreshStorageIDRecursive();
protected:

	
	virtual bool AddChild(CTreeObject* pChild,bool CheckExist=false);
	virtual bool DeleteChild(CTreeObject* pChild);
};

