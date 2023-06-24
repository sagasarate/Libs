/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWOWM2ItemModel.h                                      */
/*      创建日期:  2009年10月20日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace D3DLib{

#pragma comment(linker,"/include:?m_CD3DWOWM2ItemModelClassInfoRegister@CD3DWOWM2ItemModel@D3DLib@@1VCClassInfoRegister@@A")

class CD3DWOWM2ItemModel :
	public CD3DWOWM2Model
{
public:
	enum ITEM_HAND_TYPE
	{
		IHT_LEFT,
		IHT_RIGHT,
	};
protected:
	//struct STORAGE_STRUCT:CD3DWOWM2Model::STORAGE_STRUCT
	//{
	//	UINT			ItemID;
	//	int				ItemHandType;
	//};

	enum SST_MEMBER_ID
	{
		SST_D3DWMIM_ITEM_DISPLAY_ID=SST_D3DWMM_MAX,
		SST_D3DWMIM_ITEM_HAND_TYPE,
		SST_D3DWMIM_MAX=SST_D3DWMM_MAX+50,
	};

	UINT			m_ItemDisplayID;
	int				m_ItemHandType;	

	DECLARE_CLASS_INFO(CD3DWOWM2ItemModel)
public:
	CD3DWOWM2ItemModel(void);
	~CD3DWOWM2ItemModel(void);

	void SetItemDisplayID(UINT ItemDisplayID);
	UINT GetItemDisplayID();

	void SetItemHandType(int Type);
	int GetItemHandType();

	bool BuildModel();

	virtual bool CloneFrom(CNameObject * pObject,UINT Param=0);

	virtual bool ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual UINT GetSmartStructSize(UINT Param=0);

	virtual void Update(FLOAT Time);

protected:
	//virtual CNameObject::STORAGE_STRUCT * USOCreateHead(UINT Param=0);
	//virtual int USOWriteHead(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param=0);		
	//virtual int USOReadHead(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param=0);
	
};


inline void CD3DWOWM2ItemModel::SetItemDisplayID(UINT ItemDisplayID)
{
	m_ItemDisplayID=ItemDisplayID;
}

inline UINT CD3DWOWM2ItemModel::GetItemDisplayID()
{
	return m_ItemDisplayID;
}

inline void CD3DWOWM2ItemModel::SetItemHandType(int Type)
{
	m_ItemHandType=Type;
}
inline int CD3DWOWM2ItemModel::GetItemHandType()
{
	return m_ItemHandType;
}

}