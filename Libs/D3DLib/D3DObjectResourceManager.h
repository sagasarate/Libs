/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DObjectResourceManager.h                               */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace D3DLib{

class CD3DObjectResourceManager :
	public CNameObject
{
protected:
	CD3DDevice *									m_pD3DDevice;
	CStaticMap<CEasyString,CD3DObjectResource *>	m_ObjectStorage;

	DECLARE_CLASS_INFO_STATIC(CD3DObjectResourceManager)
public:
	CD3DObjectResourceManager(CD3DDevice * pD3DDevice,int StorageSize);
	virtual ~CD3DObjectResourceManager(void);

	bool AddResource(CD3DObjectResource * pResource,LPCTSTR ResourceName);

	CD3DObjectResource * DeleteResource(UINT ID);
	CD3DObjectResource * DeleteResource(LPCTSTR ResourceName);


	CD3DObjectResource * GetResource(UINT ID);

	CD3DObjectResource * GetResource(LPCTSTR ResourceName);

	CD3DDevice * GetDevice();

	int GetCount();
	LPVOID GetFirstPos();
	LPVOID GetLastPos();
	CD3DObjectResource * GetNext(LPVOID& Pos);
	CD3DObjectResource * GetPrev(LPVOID& Pos);
};

inline CD3DObjectResource * CD3DObjectResourceManager::GetResource(UINT ID)
{
	CD3DObjectResource ** ppResource=m_ObjectStorage.GetObject(ID);
	if(ppResource)
		return *ppResource;
	else
		return NULL;
}

inline CD3DObjectResource * CD3DObjectResourceManager::GetResource(LPCTSTR ResourceName)
{
	CEasyString Key=ResourceName;
	Key.MakeUpper();
	CD3DObjectResource ** ppResource=m_ObjectStorage.Find(Key);
	if(ppResource)
		return *ppResource;
	else
		return NULL;
}

inline CD3DDevice * CD3DObjectResourceManager::GetDevice()
{
	return m_pD3DDevice;
}

}