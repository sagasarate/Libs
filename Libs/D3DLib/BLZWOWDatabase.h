/****************************************************************************/
/*                                                                          */
/*      文件名:    BLZWOWDatabase.h                                         */
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

class CBLZWOWDatabase :
	public CStaticObject<CBLZWOWDatabase>
{
public:
	struct BLZ_DB_CHAR_SECTION
	{
		UINT32		ID;
		int			Race;
		BYTE		Sex;
		BYTE		GeneralType;
		BYTE		Type;
		BYTE		Variation;
		UINT32		Flags;
		CEasyString	Texture1;
		CEasyString	Texture2;
		CEasyString	Texture3;		
	};
	struct BLZ_DB_ANIMATION_DATA_RECORD
	{
		UINT32		ID;
		CEasyString	Name;
		UINT32		WeaponFlags;
		UINT32		BodyFlags;
		UINT32		Flags;
		UINT32		Fallback;		
	};

	struct BLZ_DB_CHAR_HAIR_SUBMESH_INFO
	{		
		BYTE	Race;
		BYTE	Sex;
		BYTE	HairType;
		BYTE	SubMeshID;
		BYTE	IsBald;
	};

	struct BLZ_DB_CHAR_WHISKER_SUBMESH_INFO
	{	
		BYTE	Race;
		BYTE	Sex;
		BYTE	WhiskerType;	
		BYTE	SubMeshID1;		
		BYTE	SubMeshID2;
		BYTE	SubMeshID3;
	};

	struct BLZ_DB_CHAR_RACE_INFO
	{	
		UINT			RaceID;
		UINT			Flags;				
		UINT			FactionID;			
		UINT			Exploration;		
		UINT			MaleModelID;			
		UINT			FemaleModelID;		
		CEasyString		Abbrev;				
		UINT			BaseLanguage;			
		UINT			CreatureType;		
		CEasyString		InternalName;		
		UINT			CinematicSequenceID;	
		UINT			Alliance;
		CEasyString		RaceName;
		CEasyString		FacialHairCustomization1;			
		CEasyString		FacialHairCustomization2;			
		CEasyString		HairCustomization;			
		UINT			Expansion;	
		UINT			UnalteredVisualRaceID;

	};
	struct BLZ_DB_CREATURE_DISPLAY_INFO
	{
		UINT32			ID;							
		UINT32			ModelID;						
		UINT32			SoundID;						
		UINT32			ExtraDisplayInformationID;	
		CEasyString		Name;
		FLOAT			Scale;						
		UINT32			Opacity;					
		CEasyString		Skin1;						
		CEasyString		Skin2;						
		CEasyString		Skin3;						
		CEasyString		Icon;	
		UINT32			SizeClass;
		UINT32			BloodLevelID;					
		UINT32			BloodID;						
		UINT32			NPCSoundID;					
		UINT32			ParticleID;					
		UINT32			CreatureGeosetDataID;			
		UINT32			ObjectEffectPackageID;		
	};

	struct BLZ_DB_CREATURE_EXTRA_DISPLAY_INFO
	{
		UINT32			ID;
		UINT32			Race;
		UINT32			Sex;
		UINT32			SkinColor;
		UINT32			FaceType;
		UINT32			HairType;
		UINT32			HairColor;
		UINT32			BeardType;
		UINT32			HeadEuipment;
		UINT32			ShoulderEuipment;
		UINT32			ShirtEuipment;
		UINT32			BustEuipment;
		UINT32			BeltEuipment;
		UINT32			LegEuipment;
		UINT32			BootEuipment;
		UINT32			RingEuipment;
		UINT32			HandEuipment;
		UINT32			WristEuipment;
		UINT32			CapeEuipment;


	};
	struct BLZ_DB_CREATURE_MODEL_INFO
	{
		UINT32			ID;							
		UINT32			Flags;						
		CEasyString		ModelPath;					
		CEasyString		AltermateModel;	
		FLOAT			Scale;						
		UINT32			BloodLevelID;
		UINT32			FootPrintID;			
		FLOAT			FootPrintTextureLength; 
		FLOAT			FootPrintTextureWidth; 	
		FLOAT			FootPrintParticleScale; 
		UINT32			FoleyMaterialID;		
		UINT32			FootStepShakeSizeID;	
		UINT32			DeathThudShakeSize; 	
		UINT			SoundDataID;					
		FLOAT			CollisionWidth; 			
		FLOAT			CollisionHeight;
		FLOAT			MountHeight;
		CD3DBoundingBox	BoundingBox; 		
		FLOAT			WorldEffectScale ; 		
		FLOAT			AttachedEffectScale; 	

		CEasyArray<BLZ_DB_CREATURE_DISPLAY_INFO *> CreatureDisplayInfos;
	};

	struct BLZ_DB_ITEM_DATA
	{
		UINT			ItemID;			
		UINT			ItemClass;		
		UINT			ItemSubClass;	
		int				MaterialID;		
		UINT			ItemDisplayInfo;
		UINT			InventorySlotID;
		UINT			SheathID;	
		//UINT 			Type;
		//UINT 			SheathType;
		UINT 			Quality;
		CEasyString 	Name;
	};

	struct BLZ_DB_ITEM_SUB_CLASS
	{		
		UINT			ClassID;
		UINT			SubClassID;		
		UINT			SubClassMark;
		UINT			HandsNumber;
		CEasyString		Name;
		CEasyString		UniquePlural;

		CEasyArray<BLZ_DB_ITEM_DATA *>	ItemInfo;
	};


	struct BLZ_DB_ITEM_CLASS
	{
		UINT			ClassID;
		bool			IsWeapon;
		CEasyString		Name;
		CEasyArray<BLZ_DB_ITEM_SUB_CLASS>	SubClassInfo;
	};

	

	struct BLZ_DB_ITEM_SUB_CLASS_MASK
	{
		UINT32			ClassID;
		UINT32			SubClassIDMask;
		CEasyString		Name;
	};

	

	struct BLZ_DB_ITEM_DISPLAY_INFO
	{
		UINT32			ID;								
		CEasyString		LeftModel;						
		CEasyString		RightModel;						
		CEasyString		LeftModelTexture;				
		CEasyString		RightModelTexture;				
		CEasyString		Icon;							
		CEasyString		Texture;						
		UINT32			GloveGeosetFlags;				
		UINT32			PantsGeosetFlags;				
		UINT32			RobeGeosetFlags;				
		UINT32			BootsGeosetFlags;
		UINT32			UnkownGeosetFlags;
		UINT32			ItemGroupSounds;				
		UINT32			HelmetGeosetVisData1;			
		UINT32			HelmetGeosetVisData2;			
		CEasyString		UpperArmTexture;				
		CEasyString		LowerArmTexture;				
		CEasyString		HandsTexture;					
		CEasyString		UpperTorsoTexture;				
		CEasyString		LowerTorsoTexture;				
		CEasyString		UpperLegTexture;				
		CEasyString		LowerLegTexture;				
		CEasyString		FootTexture;					
		UINT32			ItemVisuals;					

	};

	struct BLZ_DB_HELMET_GEOSET_VISIBLE_INFO
	{
		UINT	ID;						
		int		HairVisible;				
		int		Facial1Visible;			
		int		Facial2Visible;			
		int		Facial3Visible;			
		int		EarsVisible;
	};

	struct BLZ_DB_MAP_INFO
	{
		UINT		ID;
		CEasyString	InternalName;							
		UINT		AreaType;								
		BOOL		IsBattleground;							
		CEasyString	Name;			
		UINT		AreaTable;								
		CEasyString	AllianceDesc;	
		CEasyString	HordeDesc;		
		UINT		LoadingScreen;							
		FLOAT		BattlefieldMapIconScale;				
		UINT		ParentArea;								
		FLOAT		XCoord;									
		FLOAT		YCoord;			
		UINT		ExpansionID;							

	};

	struct BLZ_DB_LIQUID_TYPE
	{
		UINT		ID;
		CEasyString	Name;
		UINT		Flag;
		UINT		Type;
		UINT		SoundEntriesID;
		UINT		SpellID;
		CEasyString	TextureList[8];
	};

	struct LIGHT_COLOR_VALUE
	{
		UINT		Time;
		D3DCOLOR	Color;
	};

	struct BLZ_DB_LIGHT_INFO
	{
		UINT							ID;
		UINT							MapID;
		bool							IsGlobal;
		CD3DVector3						Pos;
		float							FallOffStart;
		float							FallOffEnd;
		UINT							ParamID;
		float							BlendWeight;

		bool							IsHighLightSky;
		UINT							SkyboxID;
		UINT							CloudTypeID;
		float							Glow;
		float							WaterShallowAlpha;
		float							WaterDeepAlpha;
		float							OceanShallowAlpha;
		float							OceanDeepAlpha;
		UINT							LightParamFlag;

		CEasyArray<LIGHT_COLOR_VALUE>	Colors[DBC_LICI_MAX];

		bool GetColor(UINT ColorIndex,UINT Time,D3DCOLOR& Color);

	};

	struct BLZ_DB_SKY_BOX_INFO
	{
		UINT			ID;
		CEasyString		ModelName;
		UINT			Flag;
	};


protected:
	

	CEasyArray<BLZ_DB_CHAR_SECTION>								m_CharSectionData;
	int															m_CharRaceMax;

	CEasyArray<BLZ_DB_ANIMATION_DATA_RECORD>					m_AnimationData;

	CEasyArray<BLZ_DB_CHAR_HAIR_SUBMESH_INFO>					m_CharHairSubMeshInfo;

	CEasyArray<BLZ_DB_CHAR_WHISKER_SUBMESH_INFO>				m_CharWhiskerSubMeshInfo;

	CEasyArray<BLZ_DB_CHAR_RACE_INFO>							m_CharRaceInfo;
	CEasyMap<UINT,BLZ_DB_CREATURE_DISPLAY_INFO>					m_CreatureDisplayInfo;
	CEasyMap<UINT,BLZ_DB_CREATURE_EXTRA_DISPLAY_INFO>			m_CreatureExtraDisplayInfo;
	CEasyMap<UINT,BLZ_DB_CREATURE_MODEL_INFO>					m_CreatureModelInfo;

	CEasyArray<BLZ_DB_ITEM_CLASS>								m_ItemClass;
	
	CEasyArray<BLZ_DB_ITEM_SUB_CLASS_MASK>						m_ItemSubClassMask;


	CEasyMap<UINT,BLZ_DB_ITEM_DATA>								m_ItemData;
	CEasyMap<UINT,BLZ_DB_ITEM_DISPLAY_INFO>						m_ItemDisplayInfo;

	CEasyMap<UINT,BLZ_DB_HELMET_GEOSET_VISIBLE_INFO>			m_HelmetGeosetVisibleInfo;

	CEasyArray<BLZ_DB_MAP_INFO>									m_MapInfo;

	CBLZDBCFile													m_SpellVisualEffectNameInfo;

	CEasyArray<BLZ_DB_LIQUID_TYPE>								m_LiquidTypeInfo;

	//CEasyMap<CEasyString,CEasyString>							m_MiniMapFileInfos;


	CEasyArray<BLZ_DB_LIGHT_INFO>								m_LightInfoList;
	CEasyMap<UINT,CEasyArray<BLZ_DB_LIGHT_INFO *> >				m_LightInfoMapByParamID;
	CEasyMap<UINT,CEasyArray<BLZ_DB_LIGHT_INFO *> >				m_MapLightInfos;

	DECLARE_FILE_CHANNEL_MANAGER

public:
	CBLZWOWDatabase(void);
	~CBLZWOWDatabase(void);

	virtual void Destory();

	bool LoadDBCs(CEasyString& ErrorMsg);
	bool Save(LPCTSTR szFileName);
	bool Load(LPCTSTR szFileName);

	bool LoadCharSectionData(LPCTSTR FileName);
	int GetMaxCharRace();
	int GetMaxCharSex(int Race);
	void GetCharMaxInfo(int Race,int Sex,int& SkinColor,int& HairColor,int& FaceType,int& HairType,int& WhiskerType);

	BLZ_DB_CHAR_SECTION * FindCharSection(UINT GeneralType,UINT Race,UINT Sex,UINT Type,UINT Color);


	bool LoadAnimationData(LPCTSTR FileName);

	BLZ_DB_ANIMATION_DATA_RECORD * FindAnimationData(UINT AnimationID);

	bool LoadCharHairSubMeshInfo(LPCTSTR FileName);

	int FindCharHairSubMesh(UINT Race,UINT Sex,UINT HairType,bool IsCharBald);

	bool LoadCharWhiskerSubMeshInfo(LPCTSTR FileName);

	void FindCharWhiskerSubMesh(UINT Race,UINT Sex,UINT WhiskerType,int& SubMeshID1,int& SubMeshID2,int& SubMeshID3);

	bool LoadCharRaceInfo(LPCTSTR FileName);

	BLZ_DB_CHAR_RACE_INFO * FindRaceInfo(UINT Race);

	bool LoadCreatureDisplayInfo(LPCTSTR FileName);
	bool LoadCreatureExtraDisplayInfo(LPCTSTR FileName);

	BLZ_DB_CREATURE_DISPLAY_INFO * FindCreatureDisplayInfo(UINT ID);
	BLZ_DB_CREATURE_EXTRA_DISPLAY_INFO * FindCreatureExtraDisplayInfo(UINT ID);
	LPVOID GetFirstCreatureDisplayInfoPos();
	BLZ_DB_CREATURE_DISPLAY_INFO * GetNextCreatureDisplayInfo(LPVOID& Pos);




	bool LoadCreatureModelInfo(LPCTSTR FileName);

	bool LoadNPCData(LPCTSTR FileName);
	

	BLZ_DB_CREATURE_MODEL_INFO * FindCreatureModelInfo(UINT ID);
	LPVOID GetFirstCreatureModelInfoPos();
	BLZ_DB_CREATURE_MODEL_INFO * GetNextCreatureModelInfo(LPVOID& Pos);

	bool FindCharModelInfo(UINT Race,UINT Sex,CEasyString& ModelFileName,CEasyString& SkinFileName);

	bool LoadItemClass(LPCTSTR FileName);
	bool LoadItemSubClass(LPCTSTR FileName);
	bool LoadItemSubClassMask(LPCTSTR FileName);

	UINT GetItemClassCount();
	BLZ_DB_ITEM_CLASS * GetItemClassInfo(UINT ClassID);
	BLZ_DB_ITEM_CLASS * GetItemClassInfoByIndex(UINT Index);
	BLZ_DB_ITEM_SUB_CLASS * GetItemSubClassInfo(UINT ClassID,UINT SubClassID);
	UINT GetItemSubClassMaskCount();

	bool LoadItemData(LPCTSTR FileName);

	bool LoadItemCacheData(LPCTSTR FileName);

	bool LoadItemDisplayInfo(LPCTSTR FileName);
	

	BLZ_DB_ITEM_DATA * GetItemData(UINT ItemID);

	BLZ_DB_ITEM_DISPLAY_INFO * GetItemDisplayInfo(UINT ItemDisplayID);

	bool LoadHelmetGeosetVisibleInfo(LPCTSTR FileName);

	BLZ_DB_HELMET_GEOSET_VISIBLE_INFO * GetHelmetGeosetVisibleInfo(UINT ID);

	bool LoadMapInfo(LPCTSTR FileName);

	UINT GetMapInfoCount();
	BLZ_DB_MAP_INFO * GetMapInfo(UINT Index);
	BLZ_DB_MAP_INFO * FindMapInfo(UINT MapID);

	bool LoadLiquidTypeInfo(LPCTSTR FileName);
	BLZ_DB_LIQUID_TYPE * GetLiquidTypeInfo(UINT TypeID);

	//bool LoadMiniMapInfo(LPCTSTR FileName);

	//LPCTSTR GetMiniMapRealFileName(LPCTSTR FileName);

	bool LoadMapLightInfos(LPCTSTR FileName);
	bool LoadLightParams(LPCTSTR FileName);
	bool LoadLightColors(LPCTSTR FileName);

	CEasyArray<BLZ_DB_LIGHT_INFO *> * GetMapLightInfos(UINT MapID);
	bool GetMapLightColor(UINT MapID,CD3DVector3 Pos,UINT ColorIndex,UINT Time,D3DCOLOR& Color);

protected:
	CEasyString UTF8ToLocal(LPCSTR szStr,int StrLen);
	void BuildCreatureModelInfo();

	void PrintColors(BLZ_DB_LIGHT_INFO * pInfo);

	void AddLightInfoSorted(CEasyArray<BLZ_DB_LIGHT_INFO *>& LightList,BLZ_DB_LIGHT_INFO * pLightInfo);
	
};

}