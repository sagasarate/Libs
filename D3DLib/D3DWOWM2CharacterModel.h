/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWOWM2CharacterModel.h                                 */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace D3DLib{

#pragma comment(linker,"/include:?m_CD3DWOWM2CharacterModelClassInfoRegister@CD3DWOWM2CharacterModel@D3DLib@@1VCClassInfoRegister@@A")

class CD3DWOWM2ItemModel;

class CD3DWOWM2CharacterModel :
	public CD3DWOWM2Model
{
public:
	enum CHAR_EQUIPMENT_SLOT
	{
		CES_HEAD,
		CES_SHOULDER,
		CES_LEFT_HAND,
		CES_RIGHT_HAND,
		CES_SHIELD,
		CES_CAPE,	
		CES_SHIRT,		
		CES_LEG,
		CES_BOOT,
		CES_BUST,
		CES_WRIST,
		CES_HAND,		
		CES_BELT,
		CES_ENSIGN,
		CES_MAX,
	};

	struct EQUIPMENT_INFO
	{
		UINT	ItemID;
		UINT	ItemDisplayID;
	};
protected:		

	enum SST_MEMBER_ID
	{
		SST_D3DWMCM_CHAR_ACITVE_SUB_MESH=SST_D3DWMM_MAX,
		SST_D3DWMCM_CHAR_MATERIAL,
		SST_D3DWMCM_CHAR_RACE,
		SST_D3DWMCM_CHAR_SEX,
		SST_D3DWMCM_CHAR_SKIN_COLOR,
		SST_D3DWMCM_CHAR_HAIR_COLOR,
		SST_D3DWMCM_CHAR_FACE_TYPE,
		SST_D3DWMCM_CHAR_HAIR_TYPE,
		SST_D3DWMCM_CHAR_WHISKER_TYPE,
		SST_D3DWMCM_CHAR_IS_BALD,
		SST_D3DWMCM_CHAR_EQUIPMENTS,
		SST_D3DWMCM_CHAR_HELMET_MODEL,
		SST_D3DWMCM_CHAR_LEFT_SHOULDER_MODEL,
		SST_D3DWMCM_CHAR_RIGHT_SHOULDER_MODEL,
		SST_D3DWMCM_CHAR_LEFT_WEAPON_MODEL,
		SST_D3DWMCM_CHAR_RIGHT_WEAPON_MODEL,
		SST_D3DWMCM_CHAR_SHIELD_MODEL,
		SST_D3DWMCM_CREATURE_DISPLAY_ID,
		SST_D3DWMCM_NEED_REBUILD_SUBMESH,
		SST_D3DWMCM_MAX=SST_D3DWMM_MAX+50,
	};

	CEasyArray<CD3DSubMesh *>			m_SubMeshList;
	CEasyArray<CD3DSubMeshMaterial>		m_SubMeshMaterialList;

	UINT								m_CreatureDisplayID;
	bool								m_NeedRebuildSubMesh;

	int									m_CharRace;
	int									m_CharSex;
	int									m_CharSexMax;
	int									m_CharSkinColor;
	int									m_CharSkinColorMax;
	int									m_CharHairColor;
	int									m_CharHairColorMax;
	int									m_CharFaceType;
	int									m_CharFaceTypeMax;
	int									m_CharHairType;
	int									m_CharHairTypeMax;
	int									m_CharBeardType;
	int									m_CharBeardTypeMax;
	bool								m_IsCharBald;

	EQUIPMENT_INFO						m_Equipments[CES_MAX];
	

	CD3DWOWM2ItemModel *				m_pHelmetModel;
	CD3DWOWM2ItemModel *				m_pLeftShoulderModel;
	CD3DWOWM2ItemModel *				m_pRightShoulderModel;
	CD3DWOWM2ItemModel *				m_pLeftWeaponModel;
	CD3DWOWM2ItemModel *				m_pRightWeaponModel;
	CD3DWOWM2ItemModel *				m_pRightShieldModel;

	int									m_CloseHandAnimationIndex;
	
	DECLARE_CLASS_INFO(CD3DWOWM2CharacterModel)
public:
	CD3DWOWM2CharacterModel(void);
	~CD3DWOWM2CharacterModel(void);

	virtual void Destory();
	virtual void DestoryModel();

	virtual bool Reset();
	virtual bool Restore();

	bool SetCreatureDisplayID(UINT ID);
	
	bool SetCharSkinColor(int Value);
	bool SetCharHairColor(int Value);
	bool SetCharFaceType(int Value);
	bool SetCharHairType(int Value);
	bool SetCharBeardType(int Value);
	void SetCharBald(bool IsCharBald);

	UINT GetCreatureDisplayID();
	int GetCharRace();
	int GetCharSex();
	int GetCharSkinColor();
	int GetCharHairColor();
	int GetCharFaceType();
	int GetCharHairType();
	int GetCharBeardType();
	bool IsCharBald();

	bool SetEquipment(UINT Slot,UINT ItemID);
	EQUIPMENT_INFO * GetEquipment(UINT Slot);

	bool LoadCharacter(UINT Race,UINT Sex);
	bool LoadCreature(UINT CreatureDisplayID);

	bool BuildModel();

	virtual int GetSubMeshCount();
	virtual CD3DSubMesh * GetOriginSubMesh(UINT index);
	virtual CD3DSubMeshMaterial * GetSubMeshMaterial(UINT index);

	virtual bool CloneFrom(CNameObject * pObject,UINT Param=0);
	virtual void PickResource(CUSOResourceManager * pResourceManager,UINT Param=0);

	virtual bool ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual UINT GetSmartStructSize(UINT Param=0);

protected:
	void CleanCharModel();
	bool FetchCreatureExtraInfo(UINT ExtraInfoID);
	bool BuildEquipmentModel(bool& HairVisible,bool& Facial1Visible,bool& Facial2Visible,bool& Facial3Visible,bool& EarsVisible);
	bool RebuildSubMesh(bool HairVisible,bool Facial1Visible,bool Facial2Visible,bool Facial3Visible,bool EarsVisible,bool& HaveSleeve);

	bool MakeCharSkinTexture(CD3DDevice * pD3DDevice,CD3DTexture *& pCharSkinTexture,CD3DTexture *& pCharSkinExtraTexture,
		CD3DTexture *& pCharHairTexture,CD3DTexture *& pCapeTexture,
		CD3DTexture *& pSkinTexture1,CD3DTexture *& pSkinTexture2,CD3DTexture *& pSkinTexture3,
		bool HaveSleeve);
	void AlphaMix(D3D_A8B8G8R8_PIXEL& DestPixel,D3D_A8B8G8R8_PIXEL& SrcPixel,bool UseAlphaBlend);
	bool AddTexture(CD3DTexture * pDestTexture,CD3DTexture * pSrcTexture,UINT SrcWifth,UINT SrcHeight,UINT DestOffsetX,UINT DestOffsetY,bool UseAlphaBlend);
	bool MixTexture(CD3DTexture * pSrcTexture,int MipLevel,UINT SrcWidth,UINT SrcHeight,BYTE * pDestPixels,UINT DestPitch,UINT DestOffsetX,UINT DestOffsetY,bool UseAlphaBlend);
	CD3DTexture * LoadTextureBySex(CD3DDevice * pD3DDevice,LPCTSTR TextureFileName,int Sex);
	virtual void FetchAnimationFrames(UINT Time);
};

inline void CD3DWOWM2CharacterModel::SetCharBald(bool IsCharBald)
{
	m_IsCharBald=IsCharBald;
}
inline UINT CD3DWOWM2CharacterModel::GetCreatureDisplayID()
{
	return m_CreatureDisplayID;
}
inline int CD3DWOWM2CharacterModel::GetCharRace()
{
	return m_CharRace;
}
inline int CD3DWOWM2CharacterModel::GetCharSex()
{
	return m_CharSex;
}
inline int CD3DWOWM2CharacterModel::GetCharSkinColor()
{
	return m_CharSkinColor;
}
inline int CD3DWOWM2CharacterModel::GetCharHairColor()
{
	return m_CharHairColor;
}
inline int CD3DWOWM2CharacterModel::GetCharFaceType()
{
	return m_CharFaceType;
}
inline int CD3DWOWM2CharacterModel::GetCharHairType()
{
	return m_CharHairType;
}
inline int CD3DWOWM2CharacterModel::GetCharBeardType()
{
	return m_CharBeardType;
}
inline bool CD3DWOWM2CharacterModel::IsCharBald()
{
	return m_IsCharBald;
}

inline CD3DWOWM2CharacterModel::EQUIPMENT_INFO * CD3DWOWM2CharacterModel::GetEquipment(UINT Slot)
{
	if(Slot<CES_MAX)
	{
		return &(m_Equipments[Slot]);
	}
	return NULL;
}

}