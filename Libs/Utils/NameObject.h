/****************************************************************************/
/*                                                                          */
/*      文件名:    NameObject.h                                             */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

#define OBJECT_TYPE_ID_SEC	0xffff0000
#define OBJECT_TYPE_ID_ID	0x0000ffff


#define MAX_TYPE_LEN		32
#define MAX_OBJECT_NAME		128

class CNameObject;

typedef CNameObject * (*OBJECT_CREATE_FN)();


//typedef CIndexSet<CNameObject *> CNameObjectSet;

enum OBJECT_CLONE_PARAM
{
	OBJECT_CLONE_WITHOUT_CHILD=1,
};

enum OBJECT_PACK_PARAM
{
	OPP_WITHOUT_CHILD=1,
};


struct CLASS_INFO
{
	LPCTSTR				ClassName;
	CLASS_INFO *		pParentClassInfo;
	OBJECT_CREATE_FN	pObjectCreateFn;
	bool operator==(const CLASS_INFO& ClassInfo)
	{
		return _tcscmp(ClassName,ClassInfo.ClassName)==0;
	}
	bool operator!=(const CLASS_INFO& ClassInfo)
	{
		return _tcscmp(ClassName,ClassInfo.ClassName)!=0;
	}
	bool operator==(LPCTSTR Name)
	{
		return _tcscmp(ClassName,Name)==0;
	}
	bool operator!=(LPCTSTR Name)
	{
		return _tcscmp(ClassName,Name)!=0;
	}
	bool IsKindOf(const CLASS_INFO& ClassInfo)
	{
		if(*this==ClassInfo)
			return true;
		if(pParentClassInfo)
			return pParentClassInfo->IsKindOf(ClassInfo);
		return false;
	}
	bool IsKindOf(LPCTSTR Name)
	{
		if(*this==Name)
			return true;
		if(pParentClassInfo)
			return pParentClassInfo->IsKindOf(Name);
		return false;
	}
	bool IsKindOfFast(const CLASS_INFO& ClassInfo)
	{
		if(this==&ClassInfo)
			return true;
		if(pParentClassInfo)
			return pParentClassInfo->IsKindOfFast(ClassInfo);
		return false;
	}
	CNameObject * CreateObject()
	{
		if(pObjectCreateFn)
			return (*pObjectCreateFn)();
		else
			return NULL;
	}
};

class CClassInfoRegister
{
public:
	CClassInfoRegister(LPCTSTR ClassName,CLASS_INFO * pClassInfo);
	~CClassInfoRegister();
};

#define DECLARE_CLASS_INFO(ClassName) \
protected:\
	static CLASS_INFO	m_##ClassName##ClassInfo;\
	static CClassInfoRegister m_##ClassName##ClassInfoRegister;\
public:\
	virtual CLASS_INFO& GetClassInfo()\
	{\
		return GetThisClassInfo();\
	}\
	static CNameObject * CreateObject();\
	static CLASS_INFO& GetThisClassInfo();\



#define GET_CLASS_INFO(ClassName) (ClassName::GetThisClassInfo())
#define GET_CLASS_INFO_BY_NAME(ClassName) (*ClassName::GetClassInfo(ClassName))

#define IMPLEMENT_CLASS_INFO(ClassName,ParentClassName) \
	CLASS_INFO	ClassName::m_##ClassName##ClassInfo={_T(#ClassName),&GET_CLASS_INFO(ParentClassName),ClassName::CreateObject};\
	CClassInfoRegister	ClassName::m_##ClassName##ClassInfoRegister(_T(#ClassName),&m_##ClassName##ClassInfo);\
	CNameObject * ClassName::CreateObject()\
	{\
		return (ParentClassName *) new ClassName();\
	}\
	CLASS_INFO& ClassName::GetThisClassInfo()\
	{\
		return m_##ClassName##ClassInfo;\
	}

#define DECLARE_CLASS_INFO_STATIC(ClassName) \
protected:\
	static CLASS_INFO	m_##ClassName##ClassInfo;\
public:\
	virtual CLASS_INFO& GetClassInfo()\
	{\
		return GetThisClassInfo();\
	}\
	static CLASS_INFO& GetThisClassInfo();\


#define IMPLEMENT_CLASS_INFO_STATIC(ClassName,ParentClassName) \
	CLASS_INFO	ClassName::m_##ClassName##ClassInfo={_T(#ClassName),&GET_CLASS_INFO(ParentClassName),NULL};\
	CLASS_INFO& ClassName::GetThisClassInfo()\
	{\
		return m_##ClassName##ClassInfo;\
	}



class CUSOResourceManager;



class CNameObject //: public CObject
{
protected:	
	UINT			m_ID;
	UINT			m_StorageID;
	volatile UINT	m_UseRef;
	LPVOID			m_pParam;
	CEasyString		m_Name;

	static CLASS_INFO	m_CNameObjectClassInfo;
	static CEasyMap<CEasyString,CLASS_INFO *> * m_pAllClassInfo;
	static int m_AllClassCount;
	static CClassInfoRegister m_CNameObjectClassInfoRegister;

	friend class CClassInfoRegister;
public:
	enum SST_MEMBER_ID
	{
		SST_NO_CLASS_NAME=1,
		SST_NO_OBJECT_NAME,
		SST_NO_OBJECT_ID,
		SST_NO_STORAGE_ID,
		SST_NO_MAX=50,		
	};

	CNameObject();
	virtual ~CNameObject();

	static CLASS_INFO& GetThisClassInfo();
	static CNameObject * CreateObject();
	static CLASS_INFO * GetClassInfo(LPCTSTR ClassName);
	static int GetAllClassCount();
	virtual CLASS_INFO& GetClassInfo();
	bool IsKindOf(const CLASS_INFO& ClassInfo);
	bool IsKindOf(LPCTSTR Name);
	bool IsClass(const CLASS_INFO& ClassInfo);
	bool IsClass(LPCTSTR Name);
	bool IsKindOfFast(const CLASS_INFO& ClassInfo);
	LPCTSTR GetClassName();

	
	virtual bool Initialize();
	virtual void Destory();
	UINT AddUseRef();
	UINT DecUseRef();
	UINT GetUseRef();
	virtual void Release();

	void SetName(LPCTSTR Name);

	LPCTSTR GetName();
	UINT GetNameLength();

	void SetID(UINT ID);

	UINT GetID();

	UINT GetStorageID();
	void SetStorageID(UINT StorageID);
	void RefreshStorageID();

	void SetParam(LPVOID pParam);

	LPVOID GetParam();

	
	virtual bool CloneFrom(CNameObject * pObject,UINT Param=0);
	virtual bool StealFrom(CNameObject * pObject,UINT Param=0);
	virtual void PickResource(CUSOResourceManager * pResourceManager,UINT Param=0);

	virtual bool ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual UINT GetSmartStructSize(UINT Param=0);
	

};

inline CLASS_INFO& CNameObject::GetClassInfo()
{
	return GetThisClassInfo();;
}
inline bool CNameObject::IsKindOf(const CLASS_INFO& ClassInfo)
{
	return GetClassInfo().IsKindOf(ClassInfo);
}
inline bool CNameObject::IsKindOf(LPCTSTR Name)
{
	return GetClassInfo().IsKindOf(Name);
}
inline bool CNameObject::IsClass(const CLASS_INFO& ClassInfo)
{
	return GetClassInfo()==ClassInfo;
}
inline bool CNameObject::IsClass(LPCTSTR Name)
{
	return GetClassInfo()==Name;
}
inline bool CNameObject::IsKindOfFast(const CLASS_INFO& ClassInfo)
{
	return GetClassInfo().IsKindOfFast(ClassInfo);
}
inline LPCTSTR CNameObject::GetClassName()
{
	return GetClassInfo().ClassName;
}

inline bool CNameObject::Initialize()
{
	return true;
}
inline void CNameObject::Destory()
{
}
inline UINT CNameObject::AddUseRef()
{
	return AtomicInc(&m_UseRef);
}
inline UINT CNameObject::DecUseRef()
{
	return AtomicDec(&m_UseRef);
}
inline UINT CNameObject::GetUseRef()
{
	return m_UseRef;
}
inline void CNameObject::Release()
{
	if(DecUseRef()<=0)
		delete this;
}

inline void CNameObject::SetName(LPCTSTR Name)
{
	m_Name=Name;
	//strncpy_0(m_Name,MAX_OBJECT_NAME,Name,MAX_OBJECT_NAME);
}

inline LPCTSTR CNameObject::GetName()
{
	return m_Name;
}

inline UINT CNameObject::GetNameLength()
{
	return (UINT)m_Name.GetLength();
}

inline void CNameObject::SetID(UINT ID)
{
	m_ID=ID;
}

inline UINT CNameObject::GetID()
{
	return m_ID;
}

inline UINT CNameObject::GetStorageID()
{
	return m_StorageID;
}

inline void CNameObject::SetStorageID(UINT StorageID)
{
	m_StorageID=StorageID;
}

inline void CNameObject::SetParam(LPVOID pParam)
{
	m_pParam=pParam;
}

inline LPVOID CNameObject::GetParam()
{
	return m_pParam;
}

