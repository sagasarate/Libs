/****************************************************************************/
/*                                                                          */
/*      文件名:    BLZFileDefines.h                                         */
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

#define BLZ_DBC_HEADER_TAG					((DWORD)'CBDW')
#define BLZ_DB2_HEADER_TAG					((DWORD)'2BDW')
#define BLZ_M2_HEADER_TAG					((DWORD)'02DM')
#define BLZ_SKIN_HEADER_TAG					((DWORD)'NIKS')
#define BLZ_BLS_VS_HEADER_TAG				((DWORD)'GXVS')
#define BLZ_BLS_PS_HEADER_TAG				((DWORD)'GXPS')

//#define M2_MODEL_FX_FILE_NAME				"M2ModelSoftBoneOnly.fx"
//#define M2_PARTICLE_FX_FILE_NAME			"M2Particle.fx"
//#define M2_RIBBON_FX_FILE_NAME				"M2Ribbon.fx"
//#define WMO_MODEL_FX_FILE_NAME				"WMOModel.fx"
//#define ADT_MODEL_20_FX_FILE_NAME			"ADTModel20.fx"
//#define ADT_MODEL_30_FX_FILE_NAME			"ADTModel30.fx"
//#define ADT_MODEL_LIQUID_FX_FILE_NAME		"ADTLiquid.fx"




#define BLZ_DBC_CHAR_SECTION_RECORD_SIZE				(40)
#define BLZ_DBC_ANIMATION_DATA_RECORD_SIZE				(24)
#define BLZ_DBC_CHAR_HAIR_GEOSET_RECORD_SIZE			(24)
#define BLZ_DBC_CHAR_WHISKER_GEOSET_RECORD_SIZE			(9*sizeof(UINT32))
#define BLZ_DBC_CHAR_RACE_RECORD_SIZE					(96)
#define BLZ_DBC_CREATURE_DISPLAY_INFO_RECORD_SIZE		(68)
#define BLZ_DBC_CREATURE_EXTRA_DISPLAY_INFO_RECORD_SIZE	(84)
#define BLZ_DBC_CREATURE_MODEL_INFO_RECORD_SIZE			(124)
#define BLZ_DBC_ITEM_CLASS_RECORD_SIZE					(20)
#define BLZ_DBC_ITEM_SUB_CLASS_RECORD_SIZE				(52)
#define BLZ_DBC_ITEM_SUB_CLASS_MASK_RECORD_SIZE			(16)
#define BLZ_DBC_ITEM_DATA_RECORD_SIZE					(32)
#define BLZ_DBC_ITEM_DISPLAY_INFO_RECORD_SIZE			(100)
#define BLZ_DBC_HELMET_GEOSET_VISIBLE_RECORD_SIZE		(32)
#define BLZ_DBC_MAP_RECORD_SIZE							(20*sizeof(UINT32))
#define BLZ_DBC_SPELL_VISUAL_EFFECT_NAME_RECORD_SIZE	(28)
#define BLZ_DBC_LIQUID_TYPE_RECORD_SIZE					(45*sizeof(UINT32))
#define BLZ_DBC_LIGHT_RECORD_SIZE						(15*sizeof(UINT32))
#define BLZ_DBC_LIGHT_PARAMS_RECORD_SIZE				(10*sizeof(UINT32))
#define BLZ_DBC_LIGHT_INT_BAND_RECORD_SIZE				(34*sizeof(UINT32))

#define BLZ_DBC_STR_LOCAL_ZH_CN							0//4
#define BLZ_DBC_STR_LOCAL_NUM							1//17

#define BLZ_DBC_CHAR_SECTION_FILE_NAME					"DBFilesClient\\CharSections.dbc"
#define BLZ_DBC_ANIMATION_DATA_FILE_NAME				"DBFilesClient\\AnimationData.dbc"
#define BLZ_DBC_CHAR_HAIR_GEOSET_FILE_NAME				"DBFilesClient\\CharHairGeosets.dbc"
#define BLZ_DBC_CHAR_WHISKER_GEOSET_FILE_NAME			"DBFilesClient\\CharacterFacialHairStyles.dbc"
#define BLZ_DBC_CHAR_RACE_FILE_NAME						"DBFilesClient\\ChrRaces.dbc"
#define BLZ_DBC_CREATURE_DISPLAY_INFO_FILE_NAME			"DBFilesClient\\CreatureDisplayInfo.dbc"
#define BLZ_DBC_CREATURE_EXTRA_DISPLAY_INFO_FILE_NAME	"DBFilesClient\\CreatureDisplayInfoExtra.dbc"
#define BLZ_DBC_CREATURE_MODEL_INFO_FILE_NAME			"DBFilesClient\\CreatureModelData.dbc"
#define BLZ_DBC_NPC_DATA_FILE_NAME						"NPCData.csv"
#define BLZ_DBC_ITEM_CLASS_FILE_NAME					"DBFilesClient\\ItemClass.dbc"
#define BLZ_DBC_ITEM_SUB_CLASS_FILE_NAME				"DBFilesClient\\ItemSubClass.dbc"
#define BLZ_DBC_ITEM_SUB_CLASS_MASK_FILE_NAME			"DBFilesClient\\ItemSubClassMask.dbc"
#define BLZ_DBC_ITEM_DATA_FILE_NAME						"DBFilesClient\\Item.db2"
#define BLZ_DBC_ITEM_CACHE_DATA_FILE_NAME				"ItemData.csv"
#define BLZ_DBC_ITEM_DISPLAY_INFO_FILE_NAME				"DBFilesClient\\ItemDisplayInfo.dbc"
#define BLZ_DBC_HELMET_GEOSET_VISIBLE_FILE_NAME			"DBFilesClient\\HelmetGeosetVisData.dbc"
#define BLZ_DBC_MAP_FILE_NAME							"DBFilesClient\\Map.dbc"
#define BLZ_DBC_SPELL_VISUAL_EFFECT_NAME_FILE_NAME		"DBFilesClient\\SpellVisualEffectName.dbc"
#define BLZ_DBC_LIQUID_TYPE_FILE_NAME					"DBFilesClient\\LiquidType.dbc"
#define BLZ_TRS_MINI_MAP_TEXTURE_FILES					"Textures\\MiniMap\\md5translate.trs"
#define BLZ_DBC_LIGHT_FILE_NAME							"DBFilesClient\\Light.dbc"
#define BLZ_DBC_LIGHT_PARAMS_FILE_NAME					"DBFilesClient\\LightParams.dbc"
#define BLZ_DBC_LIGHT_INT_BAND_FILE_NAME				"DBFilesClient\\LightIntBand.dbc"


#define EQUIPMENT_ARM_UPPER_TEXTURE_PATH		"ITEM\\TEXTURECOMPONENTS\\ARMUPPERTEXTURE"
#define EQUIPMENT_ARM_LOWER_TEXTURE_PATH		"ITEM\\TEXTURECOMPONENTS\\ARMLOWERTEXTURE"
#define EQUIPMENT_HAND_TEXTURE_PATH				"ITEM\\TEXTURECOMPONENTS\\HANDTEXTURE"
#define EQUIPMENT_TORSO_UPPER_TEXTURE_PATH		"ITEM\\TEXTURECOMPONENTS\\TorsoUpperTexture"
#define EQUIPMENT_TORSO_LOWER_TEXTURE_PATH		"ITEM\\TEXTURECOMPONENTS\\TorsoLowerTexture"
#define EQUIPMENT_LEG_UPPER_TEXTURE_PATH		"ITEM\\TEXTURECOMPONENTS\\LEGUPPERTEXTURE"
#define EQUIPMENT_LEG_LOWER_TEXTURE_PATH		"ITEM\\TEXTURECOMPONENTS\\LEGLOWERTEXTURE"
#define EQUIPMENT_FOOT_TEXTURE_PATH				"ITEM\\TEXTURECOMPONENTS\\FOOTTEXTURE"

#define WOW_MAP_ADT_FILE_DIR					"WORLD\\MAPS"
#define WOW_MINI_MAP_FILE_DIR					"World\\Minimaps"

#define BLZ_WOW_MAP_WIDTH						64
#define BLZ_ADT_MAP_TILE_SIZE					(100.0f/3.0f)
#define BLZ_ADT_MAP_TILE_WIDTH					(16)
#define BLZ_ADT_MAP_TILE_COUNT					(BLZ_ADT_MAP_TILE_WIDTH*BLZ_ADT_MAP_TILE_WIDTH)
#define BLZ_ADT_MAP_AREA_SIZE					(BLZ_ADT_MAP_TILE_SIZE*16.0f)
#define BLZ_ADT_MAP_TRANS_VALUE					(BLZ_ADT_MAP_AREA_SIZE*32.0f)
#define BLZ_ADT_TEXTURE_LAYER_COUNT				(4)
#define BLZ_ADT_MAP_TILE_BLOCK_SIZE				(BLZ_ADT_MAP_TILE_SIZE/8.0f)
#define BLZ_ADT_MAP_TILE_BLOCK_HALF_SIZE		(BLZ_ADT_MAP_TILE_SIZE/16.0f)

#define MAX_M2_SKIN								5

//WMO Root
#define CHUNK_ID_VERSION						((DWORD)'MVER')
#define CHUNK_ID_WMO_MOHD						((DWORD)'MOHD')
#define CHUNK_ID_WMO_MOTX						((DWORD)'MOTX')
#define CHUNK_ID_WMO_MOMT						((DWORD)'MOMT')
#define CHUNK_ID_WMO_MOGN						((DWORD)'MOGN')
#define CHUNK_ID_WMO_MOGI						((DWORD)'MOGI')
#define CHUNK_ID_WMO_MOPV						((DWORD)'MOPV')
#define CHUNK_ID_WMO_MOPT						((DWORD)'MOPT')
#define CHUNK_ID_WMO_MOPR						((DWORD)'MOPR')
#define CHUNK_ID_WMO_MOVV						((DWORD)'MOVV')
#define CHUNK_ID_WMO_MOVB						((DWORD)'MOVB')
#define CHUNK_ID_WMO_MOLT						((DWORD)'MOLT')
#define CHUNK_ID_WMO_MODS						((DWORD)'MODS')
#define CHUNK_ID_WMO_MODN						((DWORD)'MODN')
#define CHUNK_ID_WMO_MODD						((DWORD)'MODD')
#define CHUNK_ID_WMO_MFOG						((DWORD)'MFOG')
#define CHUNK_ID_WMO_MCVP						((DWORD)'MCVP')
//WMO Group
#define CHUNK_ID_WMO_MOGP						((DWORD)'MOGP')
#define CHUNK_ID_WMO_MOPY						((DWORD)'MOPY')
#define CHUNK_ID_WMO_MOVI						((DWORD)'MOVI')
#define CHUNK_ID_WMO_MOVT						((DWORD)'MOVT')
#define CHUNK_ID_WMO_MONR						((DWORD)'MONR')
#define CHUNK_ID_WMO_MOTV						((DWORD)'MOTV')
#define CHUNK_ID_WMO_MOBA						((DWORD)'MOBA')
#define CHUNK_ID_WMO_MOLR						((DWORD)'MOLR')
#define CHUNK_ID_WMO_MODR						((DWORD)'MODR')
#define CHUNK_ID_WMO_MOBN						((DWORD)'MOBN')
#define CHUNK_ID_WMO_MOBR						((DWORD)'MOBR')
#define CHUNK_ID_WMO_MOCV						((DWORD)'MOCV')
#define CHUNK_ID_WMO_MLIQ						((DWORD)'MLIQ')
//ADT
#define CHUNK_ID_ADT_MHDR						((DWORD)'MHDR')
#define CHUNK_ID_ADT_MCIN						((DWORD)'MCIN')
#define CHUNK_ID_ADT_MTEX						((DWORD)'MTEX')
#define CHUNK_ID_ADT_MMDX						((DWORD)'MMDX')
#define CHUNK_ID_ADT_MMID						((DWORD)'MMID')
#define CHUNK_ID_ADT_MWMO						((DWORD)'MWMO')
#define CHUNK_ID_ADT_MWID						((DWORD)'MWID')
#define CHUNK_ID_ADT_MDDF						((DWORD)'MDDF')
#define CHUNK_ID_ADT_MODF						((DWORD)'MODF')
#define CHUNK_ID_ADT_MH2O						((DWORD)'MH2O')
#define CHUNK_ID_ADT_MCNK						((DWORD)'MCNK')
#define CHUNK_ID_ADT_MCVT						((DWORD)'MCVT')
#define CHUNK_ID_ADT_MCCV						((DWORD)'MCCV')
#define CHUNK_ID_ADT_MCNR						((DWORD)'MCNR')
#define CHUNK_ID_ADT_MCLY						((DWORD)'MCLY')
#define CHUNK_ID_ADT_MCRF						((DWORD)'MCRF')
#define CHUNK_ID_ADT_MCSH						((DWORD)'MCSH')
#define CHUNK_ID_ADT_MCAL						((DWORD)'MCAL')
#define CHUNK_ID_ADT_MCLQ						((DWORD)'MCLQ')
#define CHUNK_ID_ADT_MCSE						((DWORD)'MCSE')
#define CHUNK_ID_ADT_MFBO						((DWORD)'MFBO')
#define CHUNK_ID_ADT_MTFX						((DWORD)'MTFX')

#define CHUNK_ID_WDT_MPHD						((DWORD)'MPHD')
#define CHUNK_ID_WDT_MAIN						((DWORD)'MAIN')
#define CHUNK_ID_WDT_MWMO						((DWORD)'MWMO')
#define CHUNK_ID_WDT_MODF						((DWORD)'MODF')

#define CHUNK_ID_WDL_MWMO						((DWORD)'MWMO')
#define CHUNK_ID_WDL_MWID						((DWORD)'MWID')
#define CHUNK_ID_WDL_MODF						((DWORD)'MODF')
#define CHUNK_ID_WDL_MAOF						((DWORD)'MAOF')
#define CHUNK_ID_WDL_MARE						((DWORD)'MARE')
#define CHUNK_ID_WDL_MAHO						((DWORD)'MAHO')

enum BLZ_M2_MODEL_FLAG
{
	BLZ_M2_MODEL_FLAG_USE_VERTEX_COLOR=0x8,
};

enum BLZ_M2_RENDER_FLAG
{
	BLZ_M2_RENDER_FLAG_NO_LIGHT=1,
	BLZ_M2_RENDER_FLAG_NO_FOG=(1<<1),
	BLZ_M2_RENDER_FLAG_NO_CULL=(1<<2),
	BLZ_M2_RENDER_FLAG_BILL_BOARD=(1<<3),
	BLZ_M2_RENDER_FLAG_DISABLE_ZBUFFER=(1<<4),
};

enum BLZ_M2_BLENDING_MODE
{
	BLZ_M2_BLENDING_MODE_OPAQUE,
	BLZ_M2_BLENDING_MODE_ALPHA_TEST,
	BLZ_M2_BLENDING_MODE_ALPHA_BLENDING,
	BLZ_M2_BLENDING_MODE_ADDITIVE,
	BLZ_M2_BLENDING_MODE_ADDITIVE_ALPHA,
	BLZ_M2_BLENDING_MODE_MODULATE,
	BLZ_M2_BLENDING_MODE_MUL,
};

enum BLZ_M2_TEXTURE_UNIT_FLAG
{
	BLZ_M2_TEXTURE_UNIT_FLAG_USE_VERTEX_ALPHA2=0x100,
	//BLZ_M2_TEXTURE_UNIT_FLAG_USE_VERTEX_ALPHA1=0x100
};

enum BLZ_M2_ANIMATION_SEQUENCE_FLAG
{
	BLZ_M2_ANIMATION_SEQUENCE_FLAG_HAVE_KEY_DATA=0x20,
};

enum BLZ_M2_INTERPOLATION_TYPE
{
	BLZ_M2_INTERPOLATE_NONE,
	BLZ_M2_INTERPOLATE_LINEAR,
	BLZ_M2_INTERPOLATE_HERMITE,
};

enum KEY_BONE_TYPE
{	
	BONE_LARM = 0,		// 0, Left upper arm
	BONE_RARM,			// 1, Right upper arm
	BONE_LSHOULDER,		// 2, Left Shoulder / deltoid area
	BONE_RSHOULDER,		// 3, Right Shoulder / deltoid area
	BONE_STOMACH,		// 4, (upper?) abdomen
	BONE_WAIST,			// 5, (lower abdomen?) waist
	BONE_HEAD,			// 6, head
	BONE_JAW,			// 7, jaw/mouth
	BONE_RFINGER1,		// 8, (Trolls have 3 "fingers", this points to the 2nd one.
	BONE_RFINGER2,		// 9, center finger - only used by dwarfs.. don't know why
	BONE_RFINGER3,		// 10, (Trolls have 3 "fingers", this points to the 3rd one.
	BONE_RFINGERS,		// 11, Right fingers -- this is -1 for trolls, they have no fingers, only the 3 thumb like thingys
	BONE_RTHUMB,		// 12, Right Thumb
	BONE_LFINGER1,		// 13, (Trolls have 3 "fingers", this points to the 2nd one.
	BONE_LFINGER2,		// 14, Center finger - only used by dwarfs.
	BONE_LFINGER3,		// 15, (Trolls have 3 "fingers", this points to the 3rd one.
	BONE_LFINGERS,		// 16, Left fingers
	BONE_LTHUMB,		// 17, Left Thumb
	UnK19,	// ?
	UnK20,	// ?
	UnK21,	// ?
	UnK22,	// ?
	UnK23,	// ?
	UnK24,	// ?
	UnK25,	// ?
	UnK26,	// ?
	BONE_ROOT			// 26, The "Root" bone,  this controls rotations, transformations, etc of the whole model and all subsequent bones.
};

enum CHAR_ATTACHMENT_ID
{
	CAI_LEFT_WRIST					=0,	
	CAI_RIGHT_PALM1					=1,	 
	CAI_LEFT_PALM1					=2,	 
	CAI_RIGHT_ELBOW					=3,	 
	CAI_LEFT_ELBOW					=4,	 
	CAI_RIGHT_SHOULDER				=5,	 
	CAI_LEFT_SHOULDER				=6,	 
	CAI_RIGHT_KNEE					=7,	 
	CAI_LEFT_KNEE					=8,	
	CAI_RIGHT_WAIST					=9,
	CAI_LEFT_FRONT_WAIST			=10,
	CAI_HELMET						=11,	 
	CAI_BACK						=12,
	CAI_IN_RIGHT_SHOULDER			=13,
	CAI_LEFT_UPPER_ARM				=14,
	CAI_BUST1						=15,	 
	CAI_BUST2						=16,	 
	CAI_FACE						=17,	 
	CAI_ABOVE_CHARACTER				=18,	 
	CAI_GROUND 						=19,	
	CAI_TOP_OF_HEAD 				=20,	
	CAI_LEFT_PALM2 					=21,	
	CAI_RIGHT_PALM2					=22,	
	CAI_RIGHT_BACK_SHEATH 			=26,	
	CAI_LEFT_BACK_SHEATH 			=27,	
	CAI_MIDDLE_BACK_SHEATH 			=28,	
	CAI_BELLY 						=29,	
	CAI_LEFT_BACK 					=30,	
	CAI_RIGHT_BACK 					=31,	
	CAI_LEFT_HIP_SHEATH 			=32,	
	CAI_RIGHT_HIP_SHEATH 			=33,	
	CAI_BUST3 						=34,	
	CAI_RIGHT_PALM3 				=35,
	CAI_RIGHT_PALM4 				=36,
	CAI_LEFT_PALM3 					=37,
	CAI_LEFT_PALM4					=38,
	CAI_MAX,
};

extern LPCTSTR CHAR_ATTACHMENT_NAME[CAI_MAX];


enum ITEM_TYPE_FLAGS
{
	ITEM_TYPE_UNKNOW1=1,				//Unknown 	Didn't find any items with this mask 
	ITEM_TYPE_CONJURED=(1<<2),			//Conjured item 	Warlock/Mage stones, water etc 
	ITEM_TYPE_OPENABLE=(1<<3),			//Openable item 	Lockboxes, clams etc 
	ITEM_TYPE_UNKNOW2=(1<<4),			//Unknown 	Didn't find any items with this mask 
	ITEM_TYPE_DEPRECATED=(1<<5),		//Deprecated item	ie 128, 16999, 18566, 21785, 21786 - sometimes contains non deprecated recipes 
	ITEM_TYPE_TOTEM=(1<<6),				//Totem 		Shaman totem tools 
	ITEM_TYPE_SPELL_TRIGGERER=(1<<7),	//Spelltriggerer 	Items bearing a spell (Equip/Use/Proc etc) 
	ITEM_TYPE_UNKNOW3=(1<<8),			//Unknown 	Torch of Retribution 
	ITEM_TYPE_WAND=(1<<9),				//Wand 		These items would appear to do ranged magical damage 
	ITEM_TYPE_WRAP=(1<<10),				//Wrap 		These items can wrap other items (wrapping paper) 
	ITEM_TYPE_PRODUCER=(1<<11),			//Producer 	These items produce other items when right clicked (motes, enchanting essences, darkmoon cards...) 
	ITEM_TYPE_MULTI_LOOT=(1<<12),		//Multi loot 	Everyone in the group/raid can loot a copy of the item. 
	ITEM_TYPE_BG=(1<<13),				//BG Item? 	Do any items still have this? --Jb55 02:19, 31 December 2006 (EST) ) 
	ITEM_TYPE_CHARTER=(1<<14),			//Charter 	Guild charters 
	ITEM_TYPE_READABLE=(1<<15),			//Readable item 	<Right Click to Read> 
	ITEM_TYPE_PVP=(1<<16),				//PvP item 	Item bought with Honor or Arena Points - also contains a lot of reputation rewards and random stuff like Egbert's Egg (?!) 
	ITEM_TYPE_DURATION=(1<<17),			//Duration 	Item expires after a certain amount of time 
	ITEM_TYPE_UNKNOW4=(1<<18),			//Unknown 	
	ITEM_TYPE_PROSPECTABLE=(1<<19),		//Prospectable 	Items a jewel crafter can prospect 
	ITEM_TYPE_UNIQUE=(1<<20),			//Unique-Equipped	Items you can only have one of equipped, but multiple in your inventory 
	ITEM_TYPE_UNKNOW5=(1<<21),			//Unknown 	
	ITEM_TYPE_UNKNOW6=(1<<22),			//Unknown 	Bandages, Poisons + Most conjured items (not all) 
	ITEM_TYPE_UNLIMIT_THROWING=(1<<23), //Throwing Weapon	Lowers durability on cast (also contains deprecated throwing weapons) 
	ITEM_TYPE_UNKNOW7=(1<<24), 			//Unknown 	Shiny Red Apple

};


enum ITEM_QUALITY 
{
	ITEM_QUALITY_Poor=0,				//(Gray) 
	ITEM_QUALITY_Common=1,				//(White)
	ITEM_QUALITY_Uncommon=2,			//(Green)
	ITEM_QUALITY_Rare=3,				//(Blue) 
	ITEM_QUALITY_Epic=4,				//(Purple) 
	ITEM_QUALITY_Legendary=5,			//(Orange) 
	ITEM_ARTIFACT=6,					//(Red) 
};

enum ITEM_SHEATH_TYPE
{
	ITEM_SHEATH_TYPE_UPPER_RIGHT_BACK=26,
	ITEM_SHEATH_TYPE_UPPER_LEFT_BACK=27, 
	ITEM_SHEATH_TYPE_CENTER_BACK=28, 
	ITEM_SHEATH_TYPE_UPSIDE_DOWN=30,
	ITEM_SHEATH_TYPE_LEFT_HIP=32,
	ITEM_SHEATH_TYPE_RIGHT_HIP=33,
};

enum ITEM_INVENTORY_SLOT_ID
{
	IISI_NONE				=0,
	IISI_HEAD				=1,	
	IISI_NECK				=2,	
	IISI_SHOULDERS			=3,	
	IISI_SHIRT				=4,	
	IISI_VEST				=5,	
	IISI_WAIST				=6,	
	IISI_LEGS				=7,	
	IISI_FEET				=8,	
	IISI_WRIST				=9,	
	IISI_HANDS				=10,  
	IISI_RING				=11,  
	IISI_TRINKET			=12,  
	IISI_ONE_HAND			=13,  
	IISI_SHIELD				=14,	
	IISI_BOW				=15,  
	IISI_BACK				=16,  
	IISI_TWO_HAND			=17,  
	IISI_BAG				=18,  
	IISI_TABARD				=19,	
	IISI_ROBE				=20,  
	IISI_MAIN_HAND			=21,  
	IISI_OFF_HAND			=22,  
	IISI_HELD				=23,  
	IISI_AMMO				=24,  
	IISI_THROWN				=25,  
	IISI_RANGED				=26,  
	IISI_RANGED2			=27,  
	IISI_RELIC				=28,  
	IISI_MAX,

};

extern LPCTSTR ITEM_INVENTORY_SLOT_NAME[IISI_MAX];

extern LPCTSTR ITEM_PATH_BY_SLOT[IISI_MAX];

//角色模型部件
enum CHAR_SUBMESH_PART
{
	CSP_HAIR=0,				//头发
	CSP_WHISKER1=1,			//胡须(下巴)
	CSP_WHISKER2=2,			//胡须(鬓角)
	CSP_WHISKER3=3,			//胡须(上唇)
	CSP_GLOVE=4,			//手腕(上臂)
	CSP_FOOT=5,				//脚(鞋子)
	CSP_EAR=7,				//耳朵
	CSP_SLEEVE=8,			//袖子
	CSP_PAINTS=9,			//裤子小腿
	CSP_LAP=10,				//衣摆？短裙？
	CSP_SKIRT=11,			//衣服下摆
	CSP_ENSIGN=12,			//工会旗帜
	CSP_ROBE=13,			//裤子大腿/长袍下摆
	CSP_BACK=15,			//披风
	CSP_SHINE_EYE=17,		//眼部发光遮片
	CSP_SASH=18,			//腰带
};

enum CHAR_RACES
{
	RACE_HUMAN              = 1,		//人类
	RACE_ORC                = 2,		//兽人
	RACE_DWARF              = 3,		//矮人
	RACE_NIGHTELF           = 4,		//暗夜精灵
	RACE_UNDEAD             = 5,		//亡灵
	RACE_TAUREN             = 6,		//牛头人
	RACE_GNOME              = 7,		//侏儒
	RACE_TROLL              = 8,		//巨魔
	RACE_GOBLIN             = 9,		//地精
	RACE_BLOODELF           = 10,		//血精灵
	RACE_DRAENEI            = 11,		//德莱尼
	RACE_FEL_ORC            = 12,		//邪兽人
	RACE_NAGA               = 13,		//纳迦
	RACE_BROKEN             = 14,		//破碎者
	RACE_SKELETON           = 15,		//骷髅
	RACE_VRYKUL             = 16,		//维库人
	RACE_TUSKARR            = 17,		//海象人
	RACE_FOREST_TROLL       = 18,		//森林巨魔
	RACE_TAUNKA             = 19,		//耗牛人
	RACE_NORTHREND_SKELETON = 20,		//诺森德骷髅
	RACE_ICE_TROLL          = 21,		//冰巨魔
	RACE_WORGEN             = 22,		//狼人
	RACE_WORGEN_HUMAN       = 23,		//吉尔尼斯人
};

enum PARTICLE_EMITTER_BLENDING_TYPE
{
	EBT_OPACITY=0,			//glDisable(GL_BLEND); glDisable(GL_ALPHA_TEST);  
	EBT_ADD=1,				//glBlendFunc(GL_SRC_COLOR, GL_ONE);  
	EBT_ALPHA_BLEND=2,		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
	EBT_ALPHA_TEST=3,		//glDisable(GL_BLEND); glEnable(GL_ALPHA_TEST);  
	EBT_ALPHA_ADD=4,		//glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
	EBT_ALPHA_MUL=5,
};

enum M2_LIGHT_TYPE
{
	M2LT_DIRECT,
	M2LT_POINT,
};

enum WMO_LIGHT_TYPE 
{
	WMOLT_OMNI_LGT,
	WMOLT_SPOT_LGT,
	WMOLT_DIRECT_LGT,
	WMOLT_AMBIENT_LGT,
};

enum WMO_GROUP_FLAG
{
	WMOGF_HAVE_BOUNDBOX=0x1,
	WMOGF_HAVE_VERTEX_COLOR=0x4,
	WMOGF_OUTDOOR=0x8,
	WMOGF_HAVE_LIGHT=0x200,
	WMOGF_HAVE_DOODADS=0x800,
	WMOGF_HAVE_WATER=0x1000,
	WMOGF_INDOOR=0x2000,
	WMOGF_SHOW_SKYBOX=0x40000,
	WMOGF_IS_NOT_OCEAN=0x80000,
};

enum WMO_TRIANGLE_MATERIAL_FLAG
{
	WMOTMF_NO_COLLISION=0x4,
};

enum WMO_MATERIAL_FLAG
{
	WMOMF_NO_CULL=0x04,
	WMOMF_EMISSIVE=0x10,
	WMOMF_TEXTURE_CLAMP=0x40,
	WMOMF_TEXTURE_WRAP=0x80,
};

enum WMO_BLENDING_MODE
{
	WMOBM_OPAQUE,
	WMOBM_ALPHA_TEST,
};

enum MCNK_FLAGS
{
	MCNK_FLAG_MCSH_AVAILABLE=0x01,
	MCNK_FLAG_IMPASSABLE=0x02,
	MCNK_FLAG_RIVER=0x04,
	MCNK_FLAG_OCEAN=0x08,
	MCNK_FLAG_MAGMA=0x10,
	MCNK_FLAG_SLIME=0x20,
	MCNK_FLAG_MCCV_AVAILABLE=0x40,
	MCNK_FLAG_UNKNOWN=0x8000,
};

enum WOW_LIQUID_TYPE
{
	WLT_RIVER=1,
	WLT_OCEAN=2,
	WLT_MAGMA=3,
	WLT_SLIMA=4,
};

enum MCLY_LAYER_INFO_FLAGS
{
	MLIF_ANI_45=0x01,
	MLIF_ANI_90=0x02,
	MLIF_ANI_180=0x04,
	MLIF_ANI_FAST=0x08,
	MLIF_ANI_FAST_2=0x10,
	MLIF_ANI_FAST_3=0x20,
	MLIF_ANI=0x40,
	MLIF_GLOW=0x80,
	MLIF_ALPHA_MAP=0x100,
	MLIF_COMPRESSED_ALPHA_MAP=0x200,
	MLIF_SHINY=0x400,
};

enum MH2O_FLAG
{
	MH2OF_NO_HEIGHT_MAP=0x02,
};

enum WDT_MPHD_FLAG
{
	WDT_MPHD_FLAG_NO_TERRAIN=0x01,
	WDT_MPHD_FLAG_USE_VERTEX_SHADING=0x02,
	WDT_MPHD_FLAG_BIG_ALPHA_MASK=0x04,
};

enum DBC_LIGHT_INFO_COLOR_ID
{
	DBC_LICI_LIGHT_DIFFUSE,
	DBC_LICI_LIGHT_AMBIENT,
	DBC_LICI_SKY_COLOR_0,
	DBC_LICI_SKY_COLOR_1,
	DBC_LICI_SKY_COLOR_2,
	DBC_LICI_SKY_COLOR_3,
	DBC_LICI_SKY_COLOR_4,
	DBC_LICI_FOG_COLOR,
	DBC_LICI_SKY_UNKNOWN_1,
	DBC_LICI_SUN_COLOR,
	DBC_LICI_SUN_HALO_COLOR,
	DBC_LICI_SKY_UNKNOWN_2,
	DBC_LICI_CLOUD_COLOR,
	DBC_LICI_SKY_UNKNOWN_3,
	DBC_LICI_SKY_UNKNOWN_4,
	DBC_LICI_WATER_COLOR_DARK,
	DBC_LICI_WATER_COLOR_LIGHT,	
	DBC_LICI_SHADOW_COLOR,
	DBC_LICI_MAX,
};


#pragma pack(push)
#pragma pack(1)

struct BLZ_DBC_HEADER
{
	DWORD	Tag;
	UINT32	RecordCount;
	UINT32	FieldCount;
	UINT32	RecordSize;
	UINT32	StringBlockSize;
};

struct BLZ_DB2_HEADER
{
	DWORD	Tag;
	UINT32	RecordCount;
	UINT32	FieldCount;
	UINT32	RecordSize;
	UINT32	StringBlockSize;
	UINT32	Unknow1;
	UINT32	ClientMinorVersion;
	UINT32	Unknow2;
	UINT32	FirstRowIndex;
	UINT32	LastRowIndex;
	UINT32	LocaleCode;
	UINT32	Unknow3;
};

struct DBC_CHAR_SECTION_RECORD
{
	UINT32	ID;
	UINT32	Race;
	UINT32	Sex;
	UINT32	GeneralType;
	UINT32	Texture1;
	UINT32	Texture2;
	UINT32	Texture3;
	UINT32	Flags;
	UINT32	Type;
	UINT32	Variation;
};

struct DBC_ANIMATION_DATA_RECORD
{
	UINT32	ID;
	UINT32	Name;
	UINT32	WeaponFlags;
	UINT32	BodyFlags;
	UINT32	Flags;
	UINT32	Fallback;
};

struct DBC_CHAR_HAIR_GEOSET_RECORD
{
	UINT32	ID;
	UINT32	Race;
	UINT32	Sex;
	UINT32	HairType;
	UINT32	SubMeshID;
	UINT32	IsBald;
};

struct DBC_CHAR_WHISKER_TYPE_RECORD
{	
	UINT32	ID;
	UINT32	Race;
	UINT32	Sex;
	UINT32	WhiskerType;	
	UINT32	SubMeshID1;
	UINT32	SubMeshID2;
	UINT32	SubMeshID3;
	UINT32	SubMeshID4;
	UINT32	SubMeshID5;
};

struct DBC_CHAR_RACE_RECORD
{	
	UINT32	RaceID;
	UINT32	Flags;					//Integer  &1: Not playable; This can remove or show boots. Set to 12 to show them. Known to be checked: 4 together with CMD 0x80. (?)  
	UINT32	FactionID;				//iRefID  facton template id. The order in the creation screen depends on this.  
	UINT32	Exploration;			//iRefID  Played on exploring zones with SMSG_EXPLORATION_EXPERIENCE.  
	UINT32	MaleModel;				//iRefID  only used for the char creation/selection screen. Ingame the server sets the model.  
	UINT32	FemaleModel;			//iRefID  only used for the char creation/selection screen. Ingame the server sets the model.  
	UINT32	Abbrev;					//String  A short form of the name. Used for helmet models.  
	UINT32	BaseLanguage;			//Integer  1 = Horde, 7 = Alliance & not playable  
	UINT32	CreatureType;			//iRefID  Always 7 (humanoid).  
	UINT32	ResSicknessSpellID;		//Integer  Always 15007.  
	UINT32	SplashSoundID ;			//Integer  1090 for dwarfs, 1096 for the others. Getting stored in CGUnit at CGUnit::PostInit.  
	UINT32	InternalName;			//String  Same as the one used in model-filepathes.  
	UINT32	CinematicSequenceID ;	//iRefID  Used for the opening cinematic.  
	UINT32	Alliance;
	UINT32	RaceNameNeutral[BLZ_DBC_STR_LOCAL_NUM];	//Loc  always is NULL for zhCN.  
	UINT32	RaceNameFemale[BLZ_DBC_STR_LOCAL_NUM];		//Loc  always is NULL for zhCN.  
	UINT32	RaceNameMale[BLZ_DBC_STR_LOCAL_NUM];		//Loc  A name to display.  
	UINT32	FacialHairCustomization1;				//String  Internal names for the facial features.  
	UINT32	FacialHairCustomization2;				//String  The localized ones are in luas.  
	UINT32	HairCustomization;				//String   
	UINT32	Expansion;				//Integer  0 for classic and non-playables, 1 for Burning Crusade  
	UINT32	UnalteredVisualRaceID;
	UINT32	Unknow1;
	UINT32	Unknow2;

};

struct DBC_CREATURE_DISPLAY_INFO_RECORD
{
	UINT32	ID;							//Integer   
	UINT32	ModelID;					//iRefID  A model to be used.  
	UINT32	SoundID;					//iRefID  Not set for that much models. Can also be set in CreatureModelData.  
	UINT32	ExtraDisplayInformationID;	//iRefID  If this display-id is a NPC wearing things that are described in there.  	
	FLOAT	Scale;						//Float  Default scale, if not set by server. 1 is the normal size.  
	UINT32	Opacity;					//Integer  0 (transparent) to 255 (opaque).  
	UINT32	Skin1;						//String  Skins that are used in the model.  
	UINT32	Skin2;						//String  See this for information when they are used.  
	UINT32	Skin3;						//String   
	UINT32	Icon;						//String  Holding an icon like INV_Misc_Food_59. Only on a few.  
	UINT32	SizeClass;
	UINT32	BloodLevelID;				//iRefID  If 0, this is read from CreatureModelData. (CGUnit::RefreshDataPointers)  
	UINT32	BloodID;					//iRefID   
	UINT32	NPCSoundID;					//iRefID  Sounds used when interacting with the NPC.  
	UINT32	ParticleID;					//iRefID  Values are 0 and >281. Wherever they are used ..  
	UINT32	CreatureGeosetDataID;		//Integer  Only set for IronDwarfs. 0x1111 to 0x1213. (WotLK)  
	UINT32	ObjectEffectPackageID;		//iRefID  Set for gyrocopters, catapults, rocketmounts and siegevehicles. (WotLK)  
};

struct DBC_CREATURE_MODEL_INFO_RECORD
{
	UINT32		ID;							//Integer 	
	UINT32		Flags;						//Integer 	Known to be checked: 0x80. Known: 4: Has death corpse.
	UINT32		ModelPath;					//String 		*.MDX!
	UINT32		AltermateModel;				//String 		This is always 0. It would be used, if something was in here. Its pushed into M2Scene::AddNewModel(GetM2Cache(), Modelpath, AlternateModel, 0).
	FLOAT		Scale;						//Float 		CMD.Scale * CDI.Scale is used in CUnit.
	UINT32		BloodLevelID;				//iRefID	
	UINT32		FootPrintID;				//iRefID 		Defines the footpritns you leave in snow. 
	FLOAT		FootPrintTextureLength; 	//Float		most time 18.0 
	FLOAT		FootPrintTextureWidth; 		//Float 	        mostly 12, others are 0.0 - 20.0
	FLOAT		FootPrintParticleScale; 	//Float 	        mostly 1.0, others are 0.0 - 5.0  
	UINT32		FoleyMaterialID;			//Integer* 	always 0.
	UINT32		FootStepShakeSizeID;		//Integer* 	ground shake? (Kunga) | 0 - 2, 10, 79, 82 | again bigger models got bigger number most time
	UINT32		DeathThudShakeSize; 		//Integer* 	0, 10 - 12, 38 | 0 most of the time.
	UINT32		SoundDataID;				//iRefID		
	FLOAT		CollisionWidth; 			//Float 		Size of collision for the model. Has to be bigger than 0.41670012920929, else "collision width is too small.".
	FLOAT		CollisionHeight;			//Float 		ZEROSCALEUNIT when 0-CollisionHeight < 0
	FLOAT		MountHeight; 				//Float 		<.. other collision data?
	CD3DVector3	BoundingBoxMin; 			//Float 		 ..
	CD3DVector3	BoundingBoxMax; 			//Float 		 ..	
	FLOAT		WorldEffectScale ; 			//Float 		 mostly 1.0, others are 0.03 - 0.9
	FLOAT		AttachedEffectScale; 		//Float 		 mostly 1.0, others are 0.5 - 2.9
	FLOAT		Unknow1;
	FLOAT		Unknow2;
	FLOAT		Unknow3;
	FLOAT		Unknow4;
	FLOAT		Unknow5;
	FLOAT		Unknow6;
};

struct DBC_ITEM_CLASS
{
	UINT32	ClassID;
	UINT32	ClassID2;
	UINT32	Unknow;
	UINT32	IsWeapon;	
	UINT32	Name[BLZ_DBC_STR_LOCAL_NUM];
};

struct DBC_ITEM_SUB_CLASS
{
	UINT32	ID;
	UINT32	ClassID;	
	UINT32	SubClassID;
	UINT32	Unknow2;
	UINT32	Unknow3;
	UINT32	Unknow4;
	UINT32	Unknow5;
	UINT32	Unknow6;
	UINT32	SubClassMark;
	UINT32	Unknow8;
	UINT32	HandsNumber;
	UINT32	Name[BLZ_DBC_STR_LOCAL_NUM];
	UINT32	UniquePlural[BLZ_DBC_STR_LOCAL_NUM];
};

struct DBC_ITEM_SUB_CLASS_MASK
{
	UINT32	ID;
	UINT32	Unknow;
	UINT32	Mask;
	UINT32	Name[BLZ_DBC_STR_LOCAL_NUM];
};

struct DBC_ITEM_DATA
{
	UINT32	ItemID;							//Integer   
	UINT32	ItemClass;						//iRefID  
	UINT32	ItemSubClass;					//iRefID  
	INT32	Unknown;						//Integer  -1 - 20, mostly -1, same as ItemCache.wdb column5  
	INT32	MaterialID;						//Integer  -1 - 8  
	UINT32	ItemDisplayInfo;				//iRefID  
	UINT32	InventorySlotID;				//Integer  These inventory slot ID's are used to determine how an item may be equipped, see below  
	UINT32	SheathID;						//Integer  0 -4, mostly 0  
};


struct DBC_ITEM_DISPLAY_INFO
{
	UINT32	ID;								//Integer   
	UINT32	LeftModel;						//String  For example at shoulders. Defineing the pairs.  
	UINT32	RightModel;						//String   
	UINT32	LeftModelTexture;				//String  And of course the textures, if not hardcoded.  
	UINT32	RightModelTexture;				//String   
	UINT32	Icon;							//  String  The icon displayed in the bags etc.  
	UINT32	Texture;						//  String  If another texture is needed.  
	UINT32	GloveGeosetFlags;				//  Integer  Maybe the gloves and bracers are swapped!  
	UINT32	PantsGeosetFlags;				//  Integer   
	UINT32	RobeGeosetFlags;				//  Integer   
	UINT32	BootsGeosetFlags;				//  Integer   
	UINT32	UnkownGeosetFlags;				//  Integer   
	UINT32	ItemGroupSounds;				//  iRefID   
	UINT32	HelmetGeosetVisData1;			//  iRefID   
	UINT32	HelmetGeosetVisData2;			//  iRefID   
	UINT32	UpperArmTexture;				//  String   
	UINT32	LowerArmTexture;				//  String   
	UINT32	HandsTexture;					//  String   
	UINT32	UpperTorsoTexture;				//  String   
	UINT32	LowerTorsoTexture;				//  String   
	UINT32	UpperLegTexture;				//  String   
	UINT32	LowerLegTexture;				//  String   
	UINT32	FootTexture;					//  String   
	UINT32	ItemVisuals;					//  iRefID  Static enchants.  
	UINT32	Unkown2;						//Integer  

};

struct DBC_HELMET_GEOSET_VISIBLE_INFO
{
	UINT32	ID;								//Integer   
	INT32	HairFlags;						//Integer  0 = show, anything else = don't show? eg: a value of 1020 won't hide night elf ears, but 999999 or -1 will.  
	INT32	Facial1Flags;					//Integer  (Beard, Tusks)  
	INT32	Facial2Flags;					//Integer  (Earrings)  
	INT32	Facial3Flags;					//Integer  See ChrRaces, column 24 to 26 for information on what is what.  
	INT32	EarsFlags;						//Integer  
	INT32	Unknow1;
	INT32	Unknow2;
};


struct DBC_MAP_INFO
{
	UINT32	ID;
	UINT32	InternalName;							//String reference to World\Map\ [...] \  
	UINT32	Unknown1;
	UINT32	Unknown2;
	UINT32	AreaType;								// Integer 0: none, 1: party, 2: raid, 3: pvp, 4: arena, >=5: none (official from "IsInInstance()")  
	UINT32	IsBattleground;							//  Boolean Simple flag for battleground maps  
	UINT32	Name[BLZ_DBC_STR_LOCAL_NUM];			// Loc Name (displayed on World Map for example)  
	UINT32	AreaTable;								// iRefID iRefID to AreaTable.dbc  
	UINT32	AllianceDesc[BLZ_DBC_STR_LOCAL_NUM];	// Loc Alliance  
	UINT32	HordeDesc[BLZ_DBC_STR_LOCAL_NUM];		// Loc Horde  
	UINT32	LoadingScreen;							// iRefID The LoadingScreen to Display  
	FLOAT	BattlefieldMapIconScale;				//Float  
	UINT32	ParentArea;								// iRefID Points to column 1, -1 if none  
	FLOAT	XCoord;									// Float The X-Coord of the instance entrance  
	FLOAT	YCoord;									// Float The Y-Coord of the instance entrance  	
	UINT32	Unknown3;								//  Integer All the time 0.  	
	UINT32	ExpansionID;							//  Integer Vanilla: 0, BC: 1, WotLK: 2  
	UINT32	Unknown4;								//  Integer  Another resettime? Is the same as the heroic one for some entries.  
	UINT32	Unknown5;
	UINT32	Unknown6;

};

struct DBC_SPELL_INFO
{
	UINT32      Id;                                             // 0        m_ID
	UINT32      Category;                                       // 1        m_category
	UINT32      Dispel;                                         // 2        m_dispelType
	UINT32      Mechanic;                                       // 3        m_mechanic
	UINT32      Attributes;                                     // 4        m_attribute
	UINT32      AttributesEx;                                   // 5        m_attributesEx
	UINT32      AttributesEx2;                                  // 6        m_attributesExB
	UINT32      AttributesEx3;                                  // 7        m_attributesExC
	UINT32      AttributesEx4;                                  // 8        m_attributesExD
	UINT32      AttributesEx5;                                  // 9        m_attributesExE
	UINT32      AttributesEx6;                                  // 10       m_attributesExF
	UINT32      unk_320_1;                                      // 11       3.2.0 (0x20 - totems, 0x4 - paladin auras, etc...)
	UINT32      Stances;                                        // 12       m_shapeshiftMask
	UINT32      unk_320_2;                                      // 13       3.2.0
	UINT32      StancesNot;                                     // 14       m_shapeshiftExclude
	UINT32      unk_320_3;                                      // 15       3.2.0
	UINT32      Targets;                                        // 16       m_targets
	UINT32      TargetCreatureType;                             // 17       m_targetCreatureType
	UINT32      RequiresSpellFocus;                             // 18       m_requiresSpellFocus
	UINT32      FacingCasterFlags;                              // 19       m_facingCasterFlags
	UINT32      CasterAuraState;                                // 20       m_casterAuraState
	UINT32      TargetAuraState;                                // 21       m_targetAuraState
	UINT32      CasterAuraStateNot;                             // 22       m_excludeCasterAuraState
	UINT32      TargetAuraStateNot;                             // 23       m_excludeTargetAuraState
	UINT32      casterAuraSpell;                                // 24       m_casterAuraSpell
	UINT32      targetAuraSpell;                                // 25       m_targetAuraSpell
	UINT32      excludeCasterAuraSpell;                         // 26       m_excludeCasterAuraSpell
	UINT32      excludeTargetAuraSpell;                         // 27       m_excludeTargetAuraSpell
	UINT32      CastingTimeIndex;                               // 28       m_castingTimeIndex
	UINT32      RecoveryTime;                                   // 29       m_recoveryTime
	UINT32      CategoryRecoveryTime;                           // 30       m_categoryRecoveryTime
	UINT32      InterruptFlags;                                 // 31       m_interruptFlags
	UINT32      AuraInterruptFlags;                             // 32       m_auraInterruptFlags
	UINT32      ChannelInterruptFlags;                          // 33       m_channelInterruptFlags
	UINT32      procFlags;                                      // 34       m_procTypeMask
	UINT32      procChance;                                     // 35       m_procChance
	UINT32      procCharges;                                    // 36       m_procCharges
	UINT32      maxLevel;                                       // 37       m_maxLevel
	UINT32      baseLevel;                                      // 38       m_baseLevel
	UINT32      spellLevel;                                     // 39       m_spellLevel
	UINT32      DurationIndex;                                  // 40       m_durationIndex
	UINT32      powerType;                                      // 41       m_powerType
	UINT32      manaCost;                                       // 42       m_manaCost
	UINT32      manaCostPerlevel;                               // 43       m_manaCostPerLevel
	UINT32      manaPerSecond;                                  // 44       m_manaPerSecond
	UINT32      manaPerSecondPerLevel;                          // 45       m_manaPerSecondPerLeve
	UINT32      rangeIndex;                                     // 46       m_rangeIndex
	FLOAT       speed;                                          // 47       m_speed
	UINT32      modalNextSpell;                                 // 48       m_modalNextSpell 
	UINT32      StackAmount;                                    // 49       m_cumulativeAura
	UINT32      Totem[2];                                       // 50-51    m_totem
	INT32       Reagent[8];                                     // 52-59    m_reagent
	UINT32      ReagentCount[8];                                // 60-67    m_reagentCount
	INT32       EquippedItemClass;                              // 68       m_equippedItemClass (value)
	INT32       EquippedItemSubClassMask;                       // 69       m_equippedItemSubclass (mask)
	INT32       EquippedItemInventoryTypeMask;                  // 70       m_equippedItemInvTypes (mask)
	UINT32      Effect[3];                                      // 71-73    m_effect
	INT32       EffectDieSides[3];                              // 74-76    m_effectDieSides
	FLOAT       EffectRealPointsPerLevel[3];                    // 77-79    m_effectRealPointsPerLevel
	INT32       EffectBasePoints[3];                            // 80-82    m_effectBasePoints (don't must be used in spell/auras explicitly, must be used cached Spell::m_currentBasePoints)
	UINT32      EffectMechanic[3];                              // 83-85    m_effectMechanic
	UINT32      EffectImplicitTargetA[3];                       // 86-88    m_implicitTargetA
	UINT32      EffectImplicitTargetB[3];                       // 89-91    m_implicitTargetB
	UINT32      EffectRadiusIndex[3];                           // 92-94    m_effectRadiusIndex - spellradius.dbc
	UINT32      EffectApplyAuraName[3];                         // 95-97    m_effectAura
	UINT32      EffectAmplitude[3];                             // 98-100   m_effectAuraPeriod
	FLOAT       EffectMultipleValue[3];                         // 101-103  m_effectAmplitude
	UINT32      EffectChainTarget[3];                           // 104-106  m_effectChainTargets
	UINT32      EffectItemType[3];                              // 107-109  m_effectItemType
	INT32       EffectMiscValue[3];                             // 110-112  m_effectMiscValue
	INT32       EffectMiscValueB[3];                            // 113-115  m_effectMiscValueB
	UINT32      EffectTriggerSpell[3];                          // 116-118  m_effectTriggerSpell
	FLOAT       EffectPointsPerComboPoint[3];                   // 119-121  m_effectPointsPerCombo
	UINT32      EffectSpellClassMaskA[3];                       // 122-124  m_effectSpellClassMaskA, effect 0
	UINT32      EffectSpellClassMaskB[3];                       // 125-127  m_effectSpellClassMaskB, effect 1
	UINT32      EffectSpellClassMaskC[3];                       // 128-130  m_effectSpellClassMaskC, effect 2
	UINT32      SpellVisual[2];                                 // 131-132  m_spellVisualID
	UINT32      SpellIconID;                                    // 133      m_spellIconID
	UINT32      activeIconID;                                   // 134      m_activeIconID
	UINT32      spellPriority;                                  // 135      m_spellPriority     
	UINT32      SpellName[BLZ_DBC_STR_LOCAL_NUM];               // 136-151  m_name_lang
	UINT32      SpellNameFlag;                                  // 152 
	UINT32      Rank[BLZ_DBC_STR_LOCAL_NUM];                    // 153-168  m_nameSubtext_lang
	UINT32      RankFlags;                                      // 169 
	UINT32      Description[BLZ_DBC_STR_LOCAL_NUM];             // 170-185  m_description_lang 
	UINT32      DescriptionFlags;                               // 186 
	UINT32      ToolTip[BLZ_DBC_STR_LOCAL_NUM];                 // 187-202  m_auraDescription_lang 
	UINT32      ToolTipFlags;                                   // 203 
	UINT32      ManaCostPercentage;                             // 204      m_manaCostPct
	UINT32      StartRecoveryCategory;                          // 205      m_startRecoveryCategory
	UINT32      StartRecoveryTime;                              // 206      m_startRecoveryTime
	UINT32      MaxTargetLevel;                                 // 207      m_maxTargetLevel
	UINT32      SpellFamilyName;                                // 208      m_spellClassSet
	UINT64      SpellFamilyFlags;                               // 209-210  m_spellClassMask NOTE: size is 12 bytes!!!
	UINT32      SpellFamilyFlags2;                              // 211      addition to m_spellClassMask
	UINT32      MaxAffectedTargets;                             // 212      m_maxTargets
	UINT32      DmgClass;                                       // 213      m_defenseType
	UINT32      PreventionType;                                 // 214      m_preventionType
	UINT32      StanceBarOrder;                                 // 215      m_stanceBarOrder 
	FLOAT       DmgMultiplier[3];                               // 216-218  m_effectChainAmplitude
	UINT32      MinFactionId;                                   // 219      m_minFactionID 
	UINT32      MinReputation;                                  // 220      m_minReputation 
	UINT32      RequiredAuraVision;                             // 221      m_requiredAuraVision 
	UINT32      TotemCategory[2];                               // 222-223  m_requiredTotemCategoryID
	INT32       AreaGroupId;                                    // 224      m_requiredAreaGroupId
	UINT32      SchoolMask;                                     // 225      m_schoolMask
	UINT32      runeCostID;                                     // 226      m_runeCostID
	UINT32      spellMissileID;                                 // 227      m_spellMissileID 
	UINT32      PowerDisplayId;                                 // 228      PowerDisplay.dbc, new in 3.1
	FLOAT       unk_320_4[3];                                   // 229-231  3.2.0
	UINT32      spellDescriptionVariableID;                     // 232      3.2.0
	UINT32      SpellDifficultyId;                              // 233      3.3.0
};

struct DBC_SPELL_VISUAL_EFFECT_NAME
{
	UINT32	ID;							//Integer 	
	UINT32	DisplayName; 				//String 		The name of the model effects used for some editor probably 
	UINT32	ModelLocation; 				//String 		Location of the model 
	FLOAT	Type; 						//Float 	        Probably flags or type of some sort but hard to tell 0.0 ~ 50.0, mostly 0.0 and 1.0
	FLOAT	Sizemodifier; 				//Float 	        another size modifier? only a few have not 1.0 or 0.0 "Acid Breath (45 yd)" has 45.0 ...0.15 ~ 5.0, mostly 1.0 
	FLOAT	Scale; 						//Float 		since 95 is "Missile: Meteor (Scale: x3) and this column is 3.00.1 and 0.01 
	FLOAT	Unknown; 					//Float           1.0 ~ 100.0, mostly 100.0
};

struct BLZ_M2_HEADER
{
	DWORD		Tag;
	DWORD		Version;
	UINT		ModelNameLength;
	UINT		ModelNameOffset;
	UINT		ModelFlag;
	UINT		GlobalSequencesCount;
	UINT		GlobalSequencesOffset;
	UINT		AnimationsCount;
	UINT		AnimationsOffset;
	UINT		AnimationLookupCount;
	UINT		AnimationLookupOffset;
	UINT		BonesCount;
	UINT		BonesOffset;
	UINT		KeyBoneLookupCount;
	UINT		KeyBoneLookupOffset;
	UINT		VerticesCount;
	UINT		VerticesOffset;	
	UINT		ViewsCount;	
	UINT		ColorsCount;
	UINT		ColorsOffset;
	UINT		TexturesCount;
	UINT		TexturesOffset;
	UINT		TransparencyCount;
	UINT		TransparencyOffset;
	UINT		TexAnimsCount;
	UINT		TexAnimsOffset;
	UINT		TexReplaceCount;
	UINT		TexReplaceOffset;
	UINT		RenderFlagsCount;
	UINT		RenderFlagsOffset;
	UINT		BoneLookupCount;
	UINT		BoneLookupOffset;
	UINT		TexLookupCount;
	UINT		TexLookupOffset;
	UINT		TexUnitsCount;
	UINT		TexUnitsOffset;
	UINT		TransLookupCount;
	UINT		TransLookupOffset;
	UINT		TexAnimLookupCount;
	UINT		TexAnimLookupOffset;
	UINT		UnknowFloats[14];
	UINT		BoundingTrianglesCount;
	UINT		BoundingTrianglesOffset;
	UINT		BoundingVerticesCount;
	UINT		BoundingVerticesOffsets;
	UINT		BoundingNormalsCount;
	UINT		BoundingNormalsOffset;
	UINT		AttachmentsCount;
	UINT		AttachmentsOffset;
	UINT		AttachLookupCount;
	UINT		AttachLookupOffset;
	UINT		Events2Count;
	UINT		EventsOffset;
	UINT		LightsCount;
	UINT		LightsOffset;
	UINT		CamerasCount;
	UINT		CamerasOffset;
	UINT		CameraLookupCount;
	UINT		CameraLookupOffset;
	UINT		RibbonEmittersCount;
	UINT		RibbonEmittersOffset;
	UINT		ParticleEmittersCount;
	UINT		ParticleEmittersOffset;
};

struct M2_MODEL_VERTEXT
{
	CD3DVector3		Pos;
	BYTE			BoneWeight[4];
	BYTE			BoneIndex[4];
	CD3DVector3		Normal;
	CD3DVector2		Tex;
	FLOAT			Unknow[2];
};

struct M2_MODEL_TEXTURE
{
	UINT32 Type;
	UINT32 Flags;
	UINT32 FileNameLength;
	UINT32 FileNameOffset;
};
struct M2_MODEL_ANIMATION_SEQUENCE
{
	UINT16		AnimationID;
	UINT16		SubAnimationID;
	UINT32		Length;
	FLOAT		MovingSpeed;
	UINT32		Flags;
	UINT32		Flags2;
	UINT32		Unknown1;
	UINT32		Unknown2;
	UINT32		PlaybackSpeed;
	CD3DVector3 BoundingBox[2];
	FLOAT		Radius;
	INT16		NextAnimation;
	UINT16		IndexId;
};

struct M2_MODEL_ANIMATION_BLOCK
{
	UINT16	InterpolationType;
	INT16	GlobalSequenceID;
	UINT32	TimestampsCount;
	UINT32	TimestampsOffset;
	UINT32	KeysCount;
	UINT32	KeysOffset;
};

struct M2_MODEL_FAKE_ANIMATION_BLOCK
{	
	UINT32	TimestampsCount;
	UINT32	TimestampsOffset;
	UINT32	KeysCount;
	UINT32	KeysOffset;
};

struct M2_MODEL_ANIMATION_PAIR
{
	UINT32	Count;
	UINT32	Offset;
};



struct M2_MODEL_BONE
{
	INT32						AnimationSeq;
	UINT32						Flags;
	INT16						ParentBone;
	UINT16						GeosetID;
	UINT32						Unknown;
	M2_MODEL_ANIMATION_BLOCK	Translation;
	M2_MODEL_ANIMATION_BLOCK	Rotation;
	M2_MODEL_ANIMATION_BLOCK	Scaling;
	CD3DVector3					PivotPoint;
};

struct M2_MODEL_RENDER_FLAG
{
	UINT16	Flags;
	UINT16	BlendingMode;
};

struct M2_MODEL_ATTACHMENT
{
	UINT32						ID;
	UINT32						Bone;
	CD3DVector3					Position;
	M2_MODEL_ANIMATION_BLOCK	Data;
};

struct M2_MODEL_ATTACHMENT2
{
	UINT32						Identifier;
	UINT32						ID;
	UINT32						Bone;
	CD3DVector3					Position;
	UINT16						InterpolationType;
	INT16						GlobalSequenceID;
	UINT32						TimestampsCount;
	UINT32						TimestampsOffset;
};

struct M2_COLOR_ANIMATION
{
	M2_MODEL_ANIMATION_BLOCK	ColorAni;
	M2_MODEL_ANIMATION_BLOCK	AlphaAni;
};

struct M2_TRANSPARENCY_ANIMATION
{
	M2_MODEL_ANIMATION_BLOCK	AlphaAni;
};


struct SKIN_HEADER
{
	UINT32	Tag;
	UINT32	IndicesCount;
	UINT32	IndicesOffset;
	UINT32	TrianglesCount;
	UINT32	TrianglesOffset;
	UINT32	PropertiesCount;
	UINT32	PropertiesOffset;
	UINT32	SubmeshesCount;
	UINT32	SubmeshesOffset;
	UINT32	TextureUnitsCount;
	UINT32	TextureUnitsOffset;
	UINT32	LOD;
};

struct SKIN_SUB_MESH
{
	UINT32		ID;
	UINT16		StartVertex;
	UINT16		VerticesCount;
	UINT16		StartTriangle;
	UINT16		TrianglesCount;
	UINT16		BonesCount;
	UINT16		StartBones;
	UINT16		Unknown;		
	UINT16		RootBone;
	CD3DVector3	CenterMass;
	CD3DVector3	CenterBoundingBox;
	FLOAT		Radius;
};

struct SKIN_TEXTURE_UNIT
{
	UINT16	Flags;			//Usually 16 for static textures, and 0 for animated textures.
	UINT16	Shading;		//If set to 0x8000: shaders. Used in skyboxes to ditch the need for depth buffering. See below.
	UINT16	SubmeshIndex;	//A duplicate entry of a submesh from the list above.
	UINT16	SubmeshIndex2;	
	INT16	ColorIndex;		//A Color out of the Colors-Block or -1 if none.
	UINT16	RenderFlags;	//The renderflags used on this texture-unit.
	UINT16	TexUnitNumber;	//Index into the texture unit lookup table.
	UINT16	Mode;			//Always 1.
	UINT16	Texture;		//Index into Texture lookup table
	UINT16	TexUnitNumber2;	//Duplicate of TexUnitNumber.
	UINT16	Transparency;	//Index into transparency lookup table.
	UINT16	TextureAnim;	//Was a index into the texture animation lookup table. 
};

struct SKIN_VERTEX_PROPERTIES
{
	BYTE	BoneLookupIndex[4];
};


struct BLZ_SHORT_QUATERNION
{
	short x,y,z,w;
};


struct M2_PARTICLE_EMITTER
{
	UINT32							Unknow1;					//Unknown Always (as I have seen): -1.  
	UINT32							Flags;						//See Below  
	CD3DVector3						Position;					//The position. Relative to the following bone.  
	UINT16							Bone;						//The bone its attached to.  
	UINT16							Texture;					//And the texture that is used.  
	UINT32							ModelFileNameLength;		//The lenght of the ModelFilename. Zeroterminated String!  
	UINT32							ModelFileNameOffset;		//And the matching offset. This is used for spawning Models. *.mdx  
	UINT32							ParticleFileNameLength;		//The lenght of the ParticleFilename. Zeroterminated String!  
	UINT32							ParticleFileNameOffset;		//And the matching offset again.This is used for spawning models of a model. *.mdx!  
	UINT8							BlendingType;				//A blending type for the particle. See Below  
	UINT8							EmitterType;				//1 - Plane (rectangle), 2 - Sphere, 3 - Spline? (can't be bothered to find one)  
	UINT16							ParticleColorRef;			//This one is used for ParticleColor.dbc. See below.  
	UINT8							ParticleType;				//? Found below.  
	UINT8							HeadOrTail;					//0 - Head, 1 - Tail, 2 - Both  
	UINT16							TextureTileRotation;		// Rotation for the texture tile. (Values: -1,0,1)  
	UINT16							TextureRows;				// How many different frames are on that texture? People should learn what rows and cols are.  
	UINT16							TextureCols;				// Its different everywhere. I just took it random.  
	M2_MODEL_ANIMATION_BLOCK		EmissionSpeed;				// All of the following blocks should be floats.  
	M2_MODEL_ANIMATION_BLOCK		SpeedVariation;				// Variation in the flying-speed. (range: 0 to 1)  
	M2_MODEL_ANIMATION_BLOCK		VerticalRange;				// Drifting away vertically. (range: 0 to pi)  
	M2_MODEL_ANIMATION_BLOCK		HorizontalRange;			// They can do it horizontally too! (range: 0 to 2*pi)  
	M2_MODEL_ANIMATION_BLOCK		Gravity;					// Fall down, apple!  
	M2_MODEL_ANIMATION_BLOCK		LifeSpan;					// Everyone has to die.  
	UINT32							UnknownPadding1;			//  I don't know what these two values should do..  
	M2_MODEL_ANIMATION_BLOCK		EmissionRate;				// Stread your particles, emitter.  
	UINT32							UnknownPadding2;				//It could have been an array without them..  
	M2_MODEL_ANIMATION_BLOCK		EmissionAreaLength;			// Well, you can do that in this area.  
	M2_MODEL_ANIMATION_BLOCK		EmissionAreaWidth;			//  
	M2_MODEL_ANIMATION_BLOCK		Decelerate;					// 减速度
	M2_MODEL_FAKE_ANIMATION_BLOCK	ParticleColor;				// (short, vec3f) This one points to 3 floats defining red, green and blue.  
	M2_MODEL_FAKE_ANIMATION_BLOCK	ParticleOpacity;			//? (short, short) Looks like opacity (short) --Igor; Most likely they all have 3 timestamps for {start, middle, end}.  
	M2_MODEL_FAKE_ANIMATION_BLOCK	ParticleSizes;				// (short, vec2f) It carries two floats per key. (x and y scale)  
	INT32							UnknownFields[2];			//No references. Padding?  
	M2_MODEL_FAKE_ANIMATION_BLOCK	Intensity;					//Some kind of intensity values seen: 0,16,17,32(if set to different it will have high intensity) (short, short)  
	M2_MODEL_FAKE_ANIMATION_BLOCK	UnknownReferences1;			//(short, short)  
	FLOAT							UnknownFloats1[3];			//They have something to do with the spread.  
	CD3DVector3						Scale;						// Wheey, its the scale!  
	FLOAT							Slowdown;					//Slowpoke is slow.  
	FLOAT							UnknownFloats2[2];			//More unknown fields.  
	FLOAT							Rotation;					// As a single value? Most likely only one axis then..  
	FLOAT							UnknownFloats3[2];			// More unknown fields.  
	FLOAT							Rotation1[3];				// Model Rotation 1  
	FLOAT							Rotation2[3];				// Model Rotation 2  
	CD3DVector3						Trans;						// Model Translation  
	FLOAT							UnknownFloats4[4];			// Unknown, unknown, unknown, unknown, unknown...  
	UINT32							UnknownReferenceCount;  
	UINT32							UnknownReferenceOffset;		// Vec3D array  
	M2_MODEL_ANIMATION_BLOCK		EnabledIn;					// (boolean) Has been in the earlier documentations. Enabled Anim Block.  
	UINT32							Unknows[4];
};


struct M2_RIBBON_EMITTER
{
	UINT32						Unknown1;						//  Always (as I have seen): -1.  
	UINT32						BoneID;							//  A bone to attach to.  
	CD3DVector3					Position;						// And a position, relative to that bone.  
	UINT32						TextureCount;					// Number of referenced textures.  
	UINT32						TextureOffset;					// Offset to the referenced textures.  
	UINT32						BlendRefCount;					//  Number of some referenced integers,which look like the blending for the texture  
	UINT32						BlendRefOffset;					// Offset to the blending-integers.  
	M2_MODEL_ANIMATION_BLOCK	Color;							// A color in three floats.  
	M2_MODEL_ANIMATION_BLOCK	Opacity;						// And an alpha value in a short, where: 0 - transparent, 0x7FFF - opaque.  
	M2_MODEL_ANIMATION_BLOCK	Above;							// The height above.  
	M2_MODEL_ANIMATION_BLOCK	Below;							// The height below. Do not set these to the same!  
	FLOAT						Resolution;						// This defines how smooth the ribbon is. A low value may produce a lot of edges.  
	FLOAT						Length;							//  The length aka Lifespan.  
	FLOAT						Emissionangle;					// use arcsin(val) to get the angle in degree  
	WORD						Renderflags[2];					// Perhaps the same as in renderflags  
	M2_MODEL_ANIMATION_BLOCK	UnknownABlock1;					// (short)  
	M2_MODEL_ANIMATION_BLOCK	UnknownABlock2;					// (boolean)  
	INT32						unknown2;						// This looks much like just some Padding to the fill up the 0x10 Bytes, always 0  
};

struct M2_TEXTURE_UV_ANIMATION
{
	M2_MODEL_ANIMATION_BLOCK	Translation;
	M2_MODEL_ANIMATION_BLOCK	Rotation;
	M2_MODEL_ANIMATION_BLOCK	Scaling;
};

struct M2_LIGHT_INFO
{
	UINT16						Type;					//Types are listed below.  
	INT16						Bone;					//  If its attached to a bone, this is the bone. Else here is a nice -1.  
	CD3DVector3					Position;				//  Where is this light?  
	M2_MODEL_ANIMATION_BLOCK	AmbientColor;			//  The ambient color. Three floats for RGB.  
	M2_MODEL_ANIMATION_BLOCK	AmbientIntensity;		// A float for the intensity.  
	M2_MODEL_ANIMATION_BLOCK	DiffuseColor;			//  The diffuse color. Three floats for RGB.  
	M2_MODEL_ANIMATION_BLOCK	DiffuseIntensity;		// A float for the intensity again.  
	M2_MODEL_ANIMATION_BLOCK	AttenuationStart;		// This defines, where the light starts to be.  
	M2_MODEL_ANIMATION_BLOCK	AttenuationEnd;			//  And where it stops.  
	M2_MODEL_ANIMATION_BLOCK	Unknown;				//  Its an integer and usually 1.  
};

struct M2_CAMERA_INFO
{
	UINT32  					Type;					//  0: potrait, 1: characterinfo; -1: else (flyby etc.); referenced backwards in the lookup table.  	
	FLOAT						FarClipping;			//  Where it stops to be drawn.  
	FLOAT						NearClipping;			//  Far and near. Both of them.  
	M2_MODEL_ANIMATION_BLOCK	TranslationPos;			//  How the camera's position moves. Should be 3*3 floats. (WoW parses 36 bytes = 3*3*sizeof(float))  
	CD3DVector3					Position;				//Where the camera is located.  
	M2_MODEL_ANIMATION_BLOCK	TranslationTar;			//  How the target moves. Should be 3*3 floats.  
	CD3DVector3					Target;					//			Where the camera points to.  
	M2_MODEL_ANIMATION_BLOCK	Scaling;				//  The camera can have some roll-effect. Its 0 to 2*Pi. 3 Floats!  
	M2_MODEL_ANIMATION_BLOCK	FOV;					//  No radians, no degrees. Multiply by 35 to get degrees.  

};

struct BLZ_CHUNK_HEADER
{
	union
	{
		DWORD	ChunkID;
		char	ChunkName[4];
	};
	
	UINT32	ChunkSize;
};

struct BLZ_CHUNK_VERSION:public BLZ_CHUNK_HEADER
{
	UINT32	Version;
};


struct BLZ_CHUNK_MOHD:public BLZ_CHUNK_HEADER
{
	UINT32 		MaterialCount;							// - number of materials
	UINT32 		GroupCount;								// - number of WMO groups
	UINT32 		PortalCount;							// - number of portals
	UINT32 		LightCount;								// - number of lights
	UINT32 		ModelCount;								// - number of M2 models imported
	UINT32 		DoodadCount;							// - number of dedicated files (*see below this table!) 
	UINT32 		DoodadSetCount;							// - number of doodad sets
	UINT32 		AmbientColor;
	UINT32 		WMOID;									// (column 2 in WMOAreaTable.dbc)
	CD3DVector3 BoundingBox[2];
	UINT32 		LiquidType;								// related, see below in the MLIQ chunk.

};

struct BLZ_CHUNK_MOTX:public BLZ_CHUNK_HEADER
{
	char TextureFileNames[1];
};


struct WMOMaterial
{
	UINT32	Flags;		
	UINT32	SpecularMode;		
	UINT32	BlendMode;		// Blending: 0 for opaque, 1 for transparent
	INT32	Texture1;		// Start position for the first texture filename in the MOTX data block	
	UINT32	TextureColor1; 
	UINT32	TextureFlags1;
	INT32	Texture2;		// Start position for the second texture filename in the MOTX data block	
	UINT32	TextureColor2; 	
	UINT32	TextureFlags2;
	UINT32	Color3; 	
	UINT32	Unknow[4];
	void*	pTexture1;		// this is the first texture object. of course only in RAM. leave this alone. :D
	void*	pTexture2;		// this is the second texture object.
};

struct BLZ_CHUNK_MOMT:public BLZ_CHUNK_HEADER
{
	WMOMaterial		Materials[1];
};


struct BLZ_CHUNK_MOGN:public BLZ_CHUNK_HEADER
{
	char GroupFileNames[1];
};

struct WMOGroup
{
	UINT32			Flags;  		
	CD3DVector3		BoundingBox[2];  		
	INT32			NameOffset;
};

struct BLZ_CHUNK_MOGI:public BLZ_CHUNK_HEADER
{
	WMOGroup Group[1];
};

struct BLZ_CHUNK_MOPV:public BLZ_CHUNK_HEADER
{
	CD3DVector3 PortalVertices[1];
};

struct WMOPortalInfo
{
	UINT16 			VertexIndex;
	UINT16 			VertexCount;			// (?), always 4 (?)
	CD3DVector3 	Normal;					// vector maybe? haven't checked.
	FLOAT 			Unknown;				//  - if this is NAN, the three floats will be (0,0,1)
};

struct BLZ_CHUNK_MOPT:public BLZ_CHUNK_HEADER
{
	WMOPortalInfo PortalInfo[1];
};

struct WMOPortalRelationship
{
	UINT16 		PortalIndex;
	UINT16 		GroupIndex;
	INT16 		Side;		//1 or -1
	UINT16 		Filler;		//always 0
};

struct BLZ_CHUNK_MOPR:public BLZ_CHUNK_HEADER
{
	WMOPortalRelationship PortalRelationship[1];
};

struct BLZ_CHUNK_MOVV:public BLZ_CHUNK_HEADER
{
	CD3DVector3 Vertices[1];
};

struct WMOVisibleBlockList
{
	UINT16	FirstVertex;
	UINT16	VertextCount;
};

struct BLZ_CHUNK_MOVB:public BLZ_CHUNK_HEADER
{
	WMOVisibleBlockList VisibleBlock[1];
};

struct WMOLightInfo
{
	UINT8		LightType; 	
	UINT8		Type;
	UINT8		UseAtten;
	UINT8		Pad;
	UINT32		Color;
	CD3DVector3 Position;
	float 		Intensity;
	float 		AttenStart;
	float 		AttenEnd;
	float 		Unknow1;
	float 		Unknow2;
	float 		Unknow3;
	float 		Unknow4;
};

struct BLZ_CHUNK_MOLT:public BLZ_CHUNK_HEADER
{
	WMOLightInfo Lights[1];
};

struct WMODoodadSet
{
	char   Name[20];
	UINT32 FirstInstanceIndex;
	UINT32 DoodadCount;
	UINT32 Nulls;
};

struct BLZ_CHUNK_MODS:public BLZ_CHUNK_HEADER
{
	WMODoodadSet DoodadSets[1];
};

struct BLZ_CHUNK_MODN:public BLZ_CHUNK_HEADER
{
	char DoodadFileNames[1];
};

struct SMODoodadInfo
{
	UINT32			NameIndex;
	CD3DVector3		Translation;
	CD3DQuaternion	Rotation;
	float			Scaling;
	UINT32			Color;
};

struct BLZ_CHUNK_MODD:public BLZ_CHUNK_HEADER
{
	SMODoodadInfo Doodads[1];
};

struct BLZ_CHUNK_MFOG:public BLZ_CHUNK_HEADER
{
	UINT32 		Flags;
	CD3DVector3	Position;
	FLOAT 		SmalleRadius;
	FLOAT 		LargeRadius;
	FLOAT 		FogEnd;
	FLOAT 		FogStart;						// multiplier (0..1)
	UINT32 		FogColor;						//The back buffer is also cleared to this colour 
	FLOAT 		Unknown1;						// (almost always 222.222)
	FLOAT 		Unknown2;						// (-1 or -0.5)
	UINT32 		Color2;
};

struct BLZ_CHUNK_MCVP:public BLZ_CHUNK_HEADER
{
	float ConvexVolumePlanes[1];
};


struct BLZ_CHUNK_MOGP:public BLZ_CHUNK_HEADER
{
	UINT32 		Name;							//Group name (offset into MOGN chunk)
	UINT32 		Descriptive;					// group name (offset into MOGN chunk)
	UINT32 		Flags;							//
	CD3DVector3 BoundingBox[2];
	UINT16 		PortalStart;					//Index into the MOPR chunk
	UINT16 		PortalCount;					//Number of items used from the MOPR chunk
	UINT16 		Batches[4];
	UINT8	 	Fogs[4];						//Up to four indices into the WMO fog list
	UINT32 		LiquidType;						// related, see below in the MLIQ chunk.
	UINT32 		GroupID;						//WMO group ID (column 4 in WMOAreaTable.dbc)
	UINT32 		Unknow1;
	UINT32 		Unknow2;
};

struct WMOTriangleMaterialInfo
{
	UINT8	Flags;
	INT8	MaterialIndex;
};

struct BLZ_CHUNK_MOPY:public BLZ_CHUNK_HEADER
{
	WMOTriangleMaterialInfo TriangleMaterialInfo[1];
};

struct BLZ_CHUNK_MOVI:public BLZ_CHUNK_HEADER
{
	UINT16 VertexIndices[1];
};

struct BLZ_CHUNK_MOVT:public BLZ_CHUNK_HEADER
{
	CD3DVector3 Vertices[1];
};

struct BLZ_CHUNK_MONR:public BLZ_CHUNK_HEADER
{
	CD3DVector3 Normals[1];
};

struct BLZ_CHUNK_MOTV:public BLZ_CHUNK_HEADER
{
	CD3DVector2 TextureCoordinates[1];
};

struct WMORenderBatch
{
	UINT32 		Color1;
	UINT32 		Color2;
	UINT32 		Color3;
	UINT32 		StartIndex;
	UINT16 		IndexCount;
	UINT16 		StartVertex;
	UINT16 		EndVertex;
	UINT8 		Unknow;
	UINT8 		Material;
};

struct BLZ_CHUNK_MOBA:public BLZ_CHUNK_HEADER
{
	WMORenderBatch RenderBatchs[1];
};

struct BLZ_CHUNK_MOLR:public BLZ_CHUNK_HEADER
{
	UINT16 LightIndices[1];
};

struct BLZ_CHUNK_MODR:public BLZ_CHUNK_HEADER
{
	UINT16 DoodadIndices[1];
};

struct WMOBSPNode
{	
	INT16	PlaneType;          // unsure
	INT16	RightChild;			// index of bsp child node(right in this array)   
	INT16	LeftChild;
	UINT16	FaceCount;			// num of triangle faces in  MOBR
	UINT16	FirstFace;			// index of the first triangle index(in  MOBR)
	INT16	Unknow;		        // 0
	float	Distance;    
};

struct BLZ_CHUNK_MOBN:public BLZ_CHUNK_HEADER
{
	WMOBSPNode BSPNodes[1];
};

struct BLZ_CHUNK_MOBR:public BLZ_CHUNK_HEADER
{
	UINT16 BSPFaceIndices[1];
};

struct BLZ_CHUNK_MOCV:public BLZ_CHUNK_HEADER
{
	UINT32 VertexColors[1];
};

struct WMOLiquidInfo
{
	UINT32 		XVertexCount;				//number of X vertices (xverts)
	UINT32 		YVertexCount;				//number of Y vertices (yverts)
	UINT32 		XTileCount;					//number of X tiles (xtiles = xverts-1)
	UINT32 		YTileCount;					//number of Y tiles (ytiles = yverts-1)
	CD3DVector3 BaseCoordinates;
	UINT16 		MaterialID;
};

struct BLZ_CHUNK_MLIQ:public BLZ_CHUNK_HEADER
{
	WMOLiquidInfo LiquidInfos[1];
};


struct BLZ_CHUNK_MHDR:public BLZ_CHUNK_HEADER
{
	UINT32 flags;		// &1: MFBO, &2: unknown. in some Northrend ones.
	UINT32 MCINChunkOffset;		
	UINT32 MTEXChunkOffset;		
	UINT32 MMDXChunkOffset;		
	UINT32 MMIDChunkOffset;		
	UINT32 MWMOChunkOffset;		
	UINT32 MWIDChunkOffset;		
	UINT32 MDDFChunkOffset;		
	UINT32 MODFChunkOffset;	
	UINT32 MFBOChunkOffset; 		// tbc, wotlk; only when flags&1
	UINT32 MH2OChunkOffset;		// wotlk
	UINT32 MTFXChunkOffset;		// wotlk
	UINT32 PAD4;		
	UINT32 PAD5;		
	UINT32 PAD6;		
	UINT32 PAD7;	
};

struct MCNKChunkInfo
{
	UINT32 ChunkOffset;				// absolute offset.
	UINT32 Size;					// the size of the MCNK chunk, this is refering to.
	UINT32 UnusedFlags;				// these two are always 0. only set in the client.
	UINT32 UnusedAsyncID;
};

struct BLZ_CHUNK_MCIN:public BLZ_CHUNK_HEADER
{
	MCNKChunkInfo MCNKChunkList[256];
};


struct BLZ_CHUNK_MTEX:public BLZ_CHUNK_HEADER
{
	char TextureFileNames[1];
};

struct BLZ_CHUNK_MMDX:public BLZ_CHUNK_HEADER
{
	char M2FileNames[1];
};

struct BLZ_CHUNK_MMID:public BLZ_CHUNK_HEADER
{
	UINT32 M2FileNameIndices[1];
};

struct BLZ_CHUNK_MWMO:public BLZ_CHUNK_HEADER
{
	char WMOFileNames[1];
};

struct BLZ_CHUNK_MWID:public BLZ_CHUNK_HEADER
{
	UINT32 WMOFileNameIndices[1];
};

struct ADT_M2_OBJECT_INFO
{
	UINT32 			Index;			//ID (index in the MMID list)
	UINT32 			ID;				//unique identifier for this instance
	CD3DVector3 	Position;		// (X,Y,Z)
	CD3DVector3 	Orientation;	//(A,B,C)
	UINT16 			Scale;			// factor * 1024 (it's scale / 1024 for the other way around)
	UINT16 			Flags;			//, known: &1 (sets the internal flags to 0x801 instead of 0x1. (WDOODADDEF.var0xC))
};

struct BLZ_CHUNK_MDDF:public BLZ_CHUNK_HEADER
{
	ADT_M2_OBJECT_INFO	M2Objects[1];
};

struct ADT_WMO_OBJECT_INFO
{
	UINT32 			Index;			//ID (index in the MWID list)
	UINT32 			ID;				//unique identifier for this instance
	CD3DVector3 	Position;		// (X,Y,Z)
	CD3DVector3 	Orientation;	// (A,B,C)
	CD3DVector3 	UpperExtents;
	CD3DVector3 	LowerExtents;
	UINT16 			Flags;
	UINT16 			DoodadSet;
	UINT16 			NameSet;
	UINT16 			Unknown;
};

struct BLZ_CHUNK_MODF:public BLZ_CHUNK_HEADER
{
	ADT_WMO_OBJECT_INFO	WMOObjects[1];
};

struct MH2OHeader
{
	UINT32	InformationOffset;			//An offset to the MH2O_Information struct(s) for this chunk.
	UINT32	LayerCount;					//0 if the chunk has no liquids. If > 1, the offsets will point to arrays.
	UINT32	RenderOffset;				//An offset to the 64 bit render mask.
};

struct MH2OInfo
{
	INT16	LiquidType;			//Points to LiquidType.dbc
	UINT16	Flags;			
	FLOAT	HeightLevel1;		//The global liquid-height of this chunk. Which is always in there twice. Blizzard knows why.
	FLOAT	HeightLevel2;		//(Actually these values are not always identical, I think they define the highest and lowest points in the heightmap)
	BYTE	XOffset;			//The X offset of the liquid square (0-7)
	BYTE	YOffset;			//The Y offset of the liquid square (0-7)
	BYTE	Width;				//The width of the liquid square (1-8)
	BYTE	Height;				//The height of the liquid square (1-8)
	UINT32	Mask2Offset;		//Offset to some data.
	UINT32	HeightMapOffset;	//Offset to MH2O_HeightmapData structure for this chunk.
};



struct BLZ_CHUNK_MH2O:public BLZ_CHUNK_HEADER
{
	MH2OHeader	Header[256];
};

struct BLZ_CHUNK_MFBO:public BLZ_CHUNK_HEADER
{
	short	Maximum[3][3];
	short	Minimum[3][3];
};

struct BLZ_CHUNK_MTFX:public BLZ_CHUNK_HEADER
{
	UINT32	Mode[1];
};

struct BLZ_CHUNK_MCNK:public BLZ_CHUNK_HEADER
{
	UINT32 		Flags;
	UINT32 		IndexX;
	UINT32 		IndexY;
	UINT32 		LayerCount;							// maximum 4
	UINT32 		DoodadRefCount;
	UINT32 		MCVTOffset;
	UINT32 		MCNROffset;
	UINT32 		MCLYOffset;
	UINT32 		MCRFOffset;
	UINT32 		MCALOffset;
	UINT32 		AlphaSize;
	UINT32 		MCSHOffset;							// only with flags&0x1
	UINT32 		ShadowSize;
	UINT32 		AreaID;
	UINT32 		MapObjRefCount;
	UINT32 		Holes;
	UINT16		ReallyLowQualityTextureingMap[8];	// the content is the layer being on top, I guess.
	UINT32 		PredTex;							// 03-29-2005 By ObscuR; TODO: Investigate
	UINT32 		NoEffectDoodad;						// 03-29-2005 By ObscuR; TODO: Investigate
	UINT32 		MCSEOffset;
	UINT32 		SndEmitterCount;					//will be set to 0 in the client if ofsSndEmitters doesn't point to MCSE!
	UINT32 		MCLQOffset;
	UINT32 		LiquidSize;  						// 8 when not used; only read if >8.
	CD3DVector3 Position;
	UINT32 		MCCVOffset; 						// only with flags&0x40, had UINT32 textureId; in ObscuR's structure.
	UINT32 		Unused1; 							// will most likely be ofsNewSubChunk in next expansion, currently 0.
	UINT32 		Unused2; 							// most likely nNewSubChunkEntries/sizeNewSubChunk in next Expansion or another offset --Tigurius
};



struct BLZ_CHUNK_MCVT:public BLZ_CHUNK_HEADER
{
	FLOAT	Heights[145];
};

struct MCCVColor
{
	BYTE	Red;
	BYTE	Green;
	BYTE	Blue;
	BYTE	Alpha;
};

struct BLZ_CHUNK_MCCV:public BLZ_CHUNK_HEADER
{
	MCCVColor	Colors[145];
};

struct BLZ_CHUNK_MCNR:public BLZ_CHUNK_HEADER
{
	char	Normals[145][3];
};

struct MCLYLayerInfo
{
	UINT32	TextureID; 
	UINT32	Flags;		
	UINT32	MCALOffset;
	INT16	EffectID;			// (actually int16 and padding)
	INT16	Padding;
};



struct BLZ_CHUNK_MCLY:public BLZ_CHUNK_HEADER
{
	MCLYLayerInfo	Layers[1];
};

struct BLZ_CHUNK_MCRF:public BLZ_CHUNK_HEADER
{
	UINT32	ObjectRefs[1];
};

struct BLZ_CHUNK_MCSH:public BLZ_CHUNK_HEADER
{
	UINT64	ShadowMap[64];
};

struct BLZ_CHUNK_MCAL:public BLZ_CHUNK_HEADER
{
	BYTE	AlphaMapData[1];
};

struct MCSESoundEmitter
{
	UINT32		SoundEntriesAdvancedId;
	CD3DVector3 Position;
	CD3DVector3 Size; 					// I'm not really sure with this. I'm far too lazy to analyze this. Seems like noone ever needed these anyway.
};

struct MCLQLiquidInfo
{
	UINT16		Color1;
	UINT16		Color2;
	FLOAT		Height;
};

struct BLZ_CHUNK_MCLQ:public BLZ_CHUNK_HEADER
{
	FLOAT			MinHeight;
	FLOAT			MaxHeight;
	MCLQLiquidInfo	LiquidInfo[9][9];
	BYTE			RenderFlag[8][8];
};

struct BLZ_CHUNK_MCSE:public BLZ_CHUNK_HEADER
{
	MCSESoundEmitter	SoundEmitters[1];
};

struct BLZ_BLS_HEADER
{
	UINT32	Tag;
	UINT32	Version;
	UINT32	DataBlockCount;
};

struct BLZ_BLS_DATA_BLOCK_HEADER
{
	UINT32	Flag1;
	UINT32	Flag2;
	UINT32	Unknow;
	UINT32	DataSize;
};
struct BLZ_BLS_DATA_BLOCK
{
	BLZ_BLS_DATA_BLOCK_HEADER	Header;
	char						Data[1];
};

struct BLZ_CHUNK_MPHD:public BLZ_CHUNK_HEADER
{
	UINT32 Flags;
	UINT32 Something;
	UINT32 Unused[6];
};

struct MAINMapAreaInfo
{
	UINT32	Flag;
	UINT32	pAsyncObject;
};

struct BLZ_CHUNK_MAIN:public BLZ_CHUNK_HEADER
{
	MAINMapAreaInfo	MapAreaInfo[64][64];
};

struct BLZ_CHUNK_MAOF:public BLZ_CHUNK_HEADER
{
	UINT32 AreaLowOffsets[64][64];
};

struct BLZ_CHUNK_MARE:public BLZ_CHUNK_HEADER
{
	INT16 HeightmapOuter[17*17];
	INT16 HeightmapInner[16*16];
};

struct BLZ_CHUNK_MAHO:public BLZ_CHUNK_HEADER
{
	UINT16  Holes[16];
};

#pragma pack(pop)

inline CD3DQuaternion ShortQuaternionToLongQuaternion(BLZ_SHORT_QUATERNION& Quaternion)
{
	return CD3DQuaternion(
		float(Quaternion.x < 0? Quaternion.x + 32768 : Quaternion.x - 32767)/ 32767.0f, 
		float(Quaternion.y < 0? Quaternion.y + 32768 : Quaternion.y - 32767)/ 32767.0f,
		float(Quaternion.z < 0? Quaternion.z + 32768 : Quaternion.z - 32767)/ 32767.0f,
		float(Quaternion.w < 0? Quaternion.w + 32768 : Quaternion.w - 32767)/ 32767.0f);
}


inline FLOAT BLZShortToFloat(short Value)
{
	return FLOAT(Value < 0? Value + 32768 : Value - 32767)/ 32767.0f;
}

inline CD3DVector3 BLZTranslationToD3D(CD3DVector3 Value)
{
	//return CD3DVector3(Value.x, Value.z, -Value.y);
	return CD3DVector3(Value.y, Value.z,-Value.x);
}

inline CD3DVector3 BLZScalingToD3D(CD3DVector3 Value)
{
	//return CD3DVector3(Value.x, Value.z, Value.y);
	return CD3DVector3(Value.y, Value.z, Value.x);
}

inline CD3DQuaternion BLZRotationToD3D(CD3DQuaternion Value)
{
	//return CD3DQuaternion(-Value.x, -Value.z, Value.y, Value.w);
	return CD3DQuaternion(-Value.y, -Value.z, Value.x, Value.w);//.GetInverse();
}

//void LoadTranslationFrames(BYTE * pData,UINT TimeStampCount,UINT TimeStampOffset,UINT KeyCount,UINT KeyOffset,ANIMATION_FRAME<CD3DVector3>& AniFrame);
//void LoadRotationFrames(BYTE * pData,UINT TimeStampCount,UINT TimeStampOffset,UINT KeyCount,UINT KeyOffset,ANIMATION_FRAME<CD3DQuaternion>& AniFrame);
//void LoadScalingFrames(BYTE * pData,UINT TimeStampCount,UINT TimeStampOffset,UINT KeyCount,UINT KeyOffset,ANIMATION_FRAME<CD3DVector3>& AniFrame);


//void LoadAniBlockFloat(BYTE * pData,M2_MODEL_ANIMATION_BLOCK& AniBlockInfo,ANIMATION_BLOCK<FLOAT>& AniBlock);
//void LoadAniBlockVector3(BYTE * pData,M2_MODEL_ANIMATION_BLOCK& AniBlockInfo,ANIMATION_BLOCK<CD3DVector3>& AniBlock);
//void LoadAniBlockQuaternion(BYTE * pData,M2_MODEL_ANIMATION_BLOCK& AniBlockInfo,ANIMATION_BLOCK<CD3DQuaternion>& AniBlock);
//void LoadAniBlockQuaternion2(BYTE * pData,M2_MODEL_ANIMATION_BLOCK& AniBlockInfo,ANIMATION_BLOCK<CD3DQuaternion>& AniBlock);
//void LoadAniBlockAlpha(BYTE * pData,M2_MODEL_ANIMATION_BLOCK& AniBlockInfo,ANIMATION_BLOCK<FLOAT>& AniBlock);
//void LoadAniBlockTranslation(BYTE * pData,M2_MODEL_ANIMATION_BLOCK& AniBlockInfo,ANIMATION_BLOCK<CD3DVector3>& AniBlock);
//void LoadAniBlockRotation(BYTE * pData,M2_MODEL_ANIMATION_BLOCK& AniBlockInfo,ANIMATION_BLOCK<CD3DQuaternion>& AniBlock);
//void LoadAniBlockScaling(BYTE * pData,M2_MODEL_ANIMATION_BLOCK& AniBlockInfo,ANIMATION_BLOCK<CD3DVector3>& AniBlock);
//void LoadAniBlockBool(BYTE * pData,M2_MODEL_ANIMATION_BLOCK& AniBlockInfo,ANIMATION_BLOCK<bool>& AniBlock);
//
//void LoadFakeAniBlockColor(BYTE * pData,M2_MODEL_FAKE_ANIMATION_BLOCK& AniBlockInfo,FAKE_ANIMATION_FRAME<CD3DVector3>& AniBlock);
//void LoadFakeAniBlockAlpha(BYTE * pData,M2_MODEL_FAKE_ANIMATION_BLOCK& AniBlockInfo,FAKE_ANIMATION_FRAME<FLOAT>& AniBlock);
//void LoadFakeAniBlockSize(BYTE * pData,M2_MODEL_FAKE_ANIMATION_BLOCK& AniBlockInfo,FAKE_ANIMATION_FRAME<CD3DVector2>& AniBlock);
//void LoadFakeAniBlockShort(BYTE * pData,M2_MODEL_FAKE_ANIMATION_BLOCK& AniBlockInfo,FAKE_ANIMATION_FRAME<short>& AniBlock);

inline void LoadTranslationFrames(BYTE * pData,UINT TimeStampCount,UINT TimeStampOffset,UINT KeyCount,UINT KeyOffset,ANIMATION_FRAME<CD3DVector3>& AniFrame)
{
	UINT32 * pTimeStamps=(UINT *)(pData+TimeStampOffset);
	AniFrame.TimeStamps.Resize(TimeStampCount);
	for(UINT k=0;k<TimeStampCount;k++)
	{				
		AniFrame.TimeStamps[k]=pTimeStamps[k];
	}
	CD3DVector3 * pKeys=(CD3DVector3 *)(pData+KeyOffset);
	AniFrame.Keys.Resize(KeyCount);
	for(UINT k=0;k<KeyCount;k++)
	{
		AniFrame.Keys[k]=BLZTranslationToD3D(pKeys[k]);
	}
}
inline void LoadRotationFrames(BYTE * pData,UINT TimeStampCount,UINT TimeStampOffset,UINT KeyCount,UINT KeyOffset,ANIMATION_FRAME<CD3DQuaternion>& AniFrame)
{
	UINT32 * pTimeStamps=(UINT *)(pData+TimeStampOffset);	
	AniFrame.TimeStamps.Resize(TimeStampCount);
	for(UINT k=0;k<TimeStampCount;k++)
	{				
		AniFrame.TimeStamps[k]=pTimeStamps[k];
	}
	BLZ_SHORT_QUATERNION * pRotations=(BLZ_SHORT_QUATERNION *)(pData+KeyOffset);
	AniFrame.Keys.Resize(KeyCount);
	for(UINT k=0;k<KeyCount;k++)
	{
		CD3DQuaternion Rotation=BLZRotationToD3D(ShortQuaternionToLongQuaternion(pRotations[k]));
		Rotation.Normalize();
		AniFrame.Keys[k]=Rotation;
	}
}
inline void LoadScalingFrames(BYTE * pData,UINT TimeStampCount,UINT TimeStampOffset,UINT KeyCount,UINT KeyOffset,ANIMATION_FRAME<CD3DVector3>& AniFrame)
{
	UINT32 * pTimeStamps=(UINT *)(pData+TimeStampOffset);
	AniFrame.TimeStamps.Resize(TimeStampCount);
	for(UINT k=0;k<TimeStampCount;k++)
	{				
		AniFrame.TimeStamps[k]=pTimeStamps[k];								
	}
	CD3DVector3 * pKeys=(CD3DVector3 *)(pData+KeyOffset);
	AniFrame.Keys.Resize(KeyCount);
	for(UINT k=0;k<KeyCount;k++)
	{
		AniFrame.Keys[k]=BLZScalingToD3D(pKeys[k]);
	}
}

inline void LoadAniBlockFloat(BYTE * pData,M2_MODEL_ANIMATION_BLOCK& AniBlockInfo,ANIMATION_BLOCK<FLOAT>& AniBlock)
{
	AniBlock.InterpolationType=AniBlockInfo.InterpolationType;
	AniBlock.GlobalSequenceID=AniBlockInfo.GlobalSequenceID;

	M2_MODEL_ANIMATION_PAIR * pTimeStampPair=(M2_MODEL_ANIMATION_PAIR *)(pData+AniBlockInfo.TimestampsOffset);
	M2_MODEL_ANIMATION_PAIR * pKeyPair=(M2_MODEL_ANIMATION_PAIR *)(pData+AniBlockInfo.KeysOffset);

	AniBlock.Animations.Resize(AniBlockInfo.TimestampsCount);

	for(UINT j=0;j<AniBlockInfo.TimestampsCount;j++)
	{
		if(pTimeStampPair[j].Count)
		{			
			UINT32 * pTimeStamps=(UINT *)(pData+pTimeStampPair[j].Offset);
			AniBlock.Animations[j].TimeStamps.Resize(pTimeStampPair[j].Count);
			for(UINT k=0;k<pTimeStampPair[j].Count;k++)
			{				
				AniBlock.Animations[j].TimeStamps[k]=pTimeStamps[k];								
			}
			float * pKeys=(float *)(pData+pKeyPair[j].Offset);
			AniBlock.Animations[j].Keys.Resize(pKeyPair[j].Count);
			for(UINT k=0;k<pKeyPair[j].Count;k++)
			{
				AniBlock.Animations[j].Keys[k]=pKeys[k];
			}
		}

	}
}

inline void LoadAniBlockVector3(BYTE * pData,M2_MODEL_ANIMATION_BLOCK& AniBlockInfo,ANIMATION_BLOCK<CD3DVector3>& AniBlock)
{
	AniBlock.InterpolationType=AniBlockInfo.InterpolationType;
	AniBlock.GlobalSequenceID=AniBlockInfo.GlobalSequenceID;

	M2_MODEL_ANIMATION_PAIR * pTimeStampPair=(M2_MODEL_ANIMATION_PAIR *)(pData+AniBlockInfo.TimestampsOffset);
	M2_MODEL_ANIMATION_PAIR * pKeyPair=(M2_MODEL_ANIMATION_PAIR *)(pData+AniBlockInfo.KeysOffset);

	AniBlock.Animations.Resize(AniBlockInfo.TimestampsCount);

	for(UINT j=0;j<AniBlockInfo.TimestampsCount;j++)
	{
		if(pTimeStampPair[j].Count)
		{					
			UINT32 * pTimeStamps=(UINT *)(pData+pTimeStampPair[j].Offset);		
			AniBlock.Animations[j].TimeStamps.Resize(pTimeStampPair[j].Count);
			for(UINT k=0;k<pTimeStampPair[j].Count;k++)
			{				
				AniBlock.Animations[j].TimeStamps[k]=(pTimeStamps[k]);								
			}
			CD3DVector3 * pKeys=(CD3DVector3 *)(pData+pKeyPair[j].Offset);
			AniBlock.Animations[j].Keys.Resize(pKeyPair[j].Count);
			for(UINT k=0;k<pKeyPair[j].Count;k++)
			{
				AniBlock.Animations[j].Keys[k]=(pKeys[k]);
			}
		}		
	}
}

inline void LoadAniBlockQuaternion(BYTE * pData,M2_MODEL_ANIMATION_BLOCK& AniBlockInfo,ANIMATION_BLOCK<CD3DQuaternion>& AniBlock)
{
	AniBlock.InterpolationType=AniBlockInfo.InterpolationType;
	AniBlock.GlobalSequenceID=AniBlockInfo.GlobalSequenceID;

	M2_MODEL_ANIMATION_PAIR * pTimeStampPair=(M2_MODEL_ANIMATION_PAIR *)(pData+AniBlockInfo.TimestampsOffset);
	M2_MODEL_ANIMATION_PAIR * pKeyPair=(M2_MODEL_ANIMATION_PAIR *)(pData+AniBlockInfo.KeysOffset);

	AniBlock.Animations.Resize(AniBlockInfo.TimestampsCount);
	for(UINT j=0;j<AniBlockInfo.TimestampsCount;j++)
	{		

		if(pTimeStampPair[j].Count)
		{	
			UINT32 * pTimeStamps=(UINT *)(pData+pTimeStampPair[j].Offset);		
			AniBlock.Animations[j].TimeStamps.Resize(pTimeStampPair[j].Count);
			for(UINT k=0;k<pTimeStampPair[j].Count;k++)
			{				
				AniBlock.Animations[j].TimeStamps[k]=(pTimeStamps[k]);								
			}
			BLZ_SHORT_QUATERNION * pKeys=(BLZ_SHORT_QUATERNION *)(pData+pKeyPair[j].Offset);
			AniBlock.Animations[j].Keys.Resize(pKeyPair[j].Count);
			for(UINT k=0;k<pKeyPair[j].Count;k++)
			{
				CD3DQuaternion Rotation=ShortQuaternionToLongQuaternion(pKeys[k]);
				Rotation.Normalize();
				AniBlock.Animations[j].Keys[k]=(Rotation);
			}
		}				
	}	
}

inline void LoadAniBlockQuaternion2(BYTE * pData,M2_MODEL_ANIMATION_BLOCK& AniBlockInfo,ANIMATION_BLOCK<CD3DQuaternion>& AniBlock)
{
	AniBlock.InterpolationType=AniBlockInfo.InterpolationType;
	AniBlock.GlobalSequenceID=AniBlockInfo.GlobalSequenceID;

	M2_MODEL_ANIMATION_PAIR * pTimeStampPair=(M2_MODEL_ANIMATION_PAIR *)(pData+AniBlockInfo.TimestampsOffset);
	M2_MODEL_ANIMATION_PAIR * pKeyPair=(M2_MODEL_ANIMATION_PAIR *)(pData+AniBlockInfo.KeysOffset);

	AniBlock.Animations.Resize(AniBlockInfo.TimestampsCount);
	for(UINT j=0;j<AniBlockInfo.TimestampsCount;j++)
	{		

		if(pTimeStampPair[j].Count)
		{	
			UINT32 * pTimeStamps=(UINT *)(pData+pTimeStampPair[j].Offset);		
			AniBlock.Animations[j].TimeStamps.Resize(pTimeStampPair[j].Count);
			for(UINT k=0;k<pTimeStampPair[j].Count;k++)
			{				
				AniBlock.Animations[j].TimeStamps[k]=(pTimeStamps[k]);								
			}
			CD3DQuaternion * pKeys=(CD3DQuaternion *)(pData+pKeyPair[j].Offset);
			AniBlock.Animations[j].Keys.Resize(pKeyPair[j].Count);
			for(UINT k=0;k<pKeyPair[j].Count;k++)
			{
				CD3DQuaternion Rotation=pKeys[k];
				Rotation.Normalize();
				AniBlock.Animations[j].Keys[k]=(Rotation);
			}
		}				
	}	
}

inline void LoadAniBlockAlpha(BYTE * pData,M2_MODEL_ANIMATION_BLOCK& AniBlockInfo,ANIMATION_BLOCK<FLOAT>& AniBlock)
{
	AniBlock.InterpolationType=AniBlockInfo.InterpolationType;
	AniBlock.GlobalSequenceID=AniBlockInfo.GlobalSequenceID;

	M2_MODEL_ANIMATION_PAIR * pTimeStampPair=(M2_MODEL_ANIMATION_PAIR *)(pData+AniBlockInfo.TimestampsOffset);
	M2_MODEL_ANIMATION_PAIR * pKeyPair=(M2_MODEL_ANIMATION_PAIR *)(pData+AniBlockInfo.KeysOffset);

	AniBlock.Animations.Resize(AniBlockInfo.TimestampsCount);

	for(UINT j=0;j<AniBlockInfo.TimestampsCount;j++)
	{
		if(pTimeStampPair[j].Count)
		{					

			UINT32 * pTimeStamps=(UINT *)(pData+pTimeStampPair[j].Offset);							
			AniBlock.Animations[j].TimeStamps.Resize(pTimeStampPair[j].Count);
			for(UINT k=0;k<pTimeStampPair[j].Count;k++)
			{				
				AniBlock.Animations[j].TimeStamps[k]=(pTimeStamps[k]);								
			}
			short * pKeys=(short *)(pData+pKeyPair[j].Offset);
			AniBlock.Animations[j].Keys.Resize(pKeyPair[j].Count);
			for(UINT k=0;k<pKeyPair[j].Count;k++)
			{
				FLOAT Alpha=(FLOAT)pKeys[k]/0x7FFF;
				AniBlock.Animations[j].Keys[k]=(Alpha);
			}

		}		
	}
}

inline void LoadAniBlockTranslation(BYTE * pData,M2_MODEL_ANIMATION_BLOCK& AniBlockInfo,ANIMATION_BLOCK<CD3DVector3>& AniBlock)
{
	AniBlock.InterpolationType=AniBlockInfo.InterpolationType;
	AniBlock.GlobalSequenceID=AniBlockInfo.GlobalSequenceID;

	M2_MODEL_ANIMATION_PAIR * pTimeStampPair=(M2_MODEL_ANIMATION_PAIR *)(pData+AniBlockInfo.TimestampsOffset);
	M2_MODEL_ANIMATION_PAIR * pKeyPair=(M2_MODEL_ANIMATION_PAIR *)(pData+AniBlockInfo.KeysOffset);

	AniBlock.Animations.Resize(AniBlockInfo.TimestampsCount);
	for(UINT j=0;j<AniBlockInfo.TimestampsCount;j++)
	{		

		if(pTimeStampPair[j].Count)
		{			
			LoadTranslationFrames(pData,pTimeStampPair[j].Count,pTimeStampPair[j].Offset,
				pKeyPair[j].Count,pKeyPair[j].Offset,AniBlock.Animations[j]);			
		}				
	}

}

inline void LoadAniBlockRotation(BYTE * pData,M2_MODEL_ANIMATION_BLOCK& AniBlockInfo,ANIMATION_BLOCK<CD3DQuaternion>& AniBlock)
{
	AniBlock.InterpolationType=AniBlockInfo.InterpolationType;
	AniBlock.GlobalSequenceID=AniBlockInfo.GlobalSequenceID;

	M2_MODEL_ANIMATION_PAIR * pTimeStampPair=(M2_MODEL_ANIMATION_PAIR *)(pData+AniBlockInfo.TimestampsOffset);
	M2_MODEL_ANIMATION_PAIR * pKeyPair=(M2_MODEL_ANIMATION_PAIR *)(pData+AniBlockInfo.KeysOffset);

	AniBlock.Animations.Resize(AniBlockInfo.TimestampsCount);
	for(UINT j=0;j<AniBlockInfo.TimestampsCount;j++)
	{		

		if(pTimeStampPair[j].Count)
		{	
			LoadRotationFrames(pData,pTimeStampPair[j].Count,pTimeStampPair[j].Offset,
				pKeyPair[j].Count,pKeyPair[j].Offset,AniBlock.Animations[j]);
		}				
	}	
}

inline void LoadAniBlockScaling(BYTE * pData,M2_MODEL_ANIMATION_BLOCK& AniBlockInfo,ANIMATION_BLOCK<CD3DVector3>& AniBlock)
{
	AniBlock.InterpolationType=AniBlockInfo.InterpolationType;
	AniBlock.GlobalSequenceID=AniBlockInfo.GlobalSequenceID;

	M2_MODEL_ANIMATION_PAIR * pTimeStampPair=(M2_MODEL_ANIMATION_PAIR *)(pData+AniBlockInfo.TimestampsOffset);
	M2_MODEL_ANIMATION_PAIR * pKeyPair=(M2_MODEL_ANIMATION_PAIR *)(pData+AniBlockInfo.KeysOffset);

	AniBlock.Animations.Resize(AniBlockInfo.TimestampsCount);
	for(UINT j=0;j<AniBlockInfo.TimestampsCount;j++)
	{		

		if(pTimeStampPair[j].Count)
		{			
			LoadScalingFrames(pData,pTimeStampPair[j].Count,pTimeStampPair[j].Offset,
				pKeyPair[j].Count,pKeyPair[j].Offset,AniBlock.Animations[j]);
		}				
	}	
}

inline void LoadAniBlockBool(BYTE * pData,M2_MODEL_ANIMATION_BLOCK& AniBlockInfo,ANIMATION_BLOCK<bool>& AniBlock)
{
	AniBlock.InterpolationType=AniBlockInfo.InterpolationType;
	AniBlock.GlobalSequenceID=AniBlockInfo.GlobalSequenceID;

	M2_MODEL_ANIMATION_PAIR * pTimeStampPair=(M2_MODEL_ANIMATION_PAIR *)(pData+AniBlockInfo.TimestampsOffset);
	M2_MODEL_ANIMATION_PAIR * pKeyPair=(M2_MODEL_ANIMATION_PAIR *)(pData+AniBlockInfo.KeysOffset);

	AniBlock.Animations.Resize(AniBlockInfo.TimestampsCount);

	for(UINT j=0;j<AniBlockInfo.TimestampsCount;j++)
	{
		if(pTimeStampPair[j].Count)
		{					

			UINT32 * pTimeStamps=(UINT *)(pData+pTimeStampPair[j].Offset);							
			AniBlock.Animations[j].TimeStamps.Resize(pTimeStampPair[j].Count);
			for(UINT k=0;k<pTimeStampPair[j].Count;k++)
			{				
				AniBlock.Animations[j].TimeStamps[k]=(pTimeStamps[k]);								
			}
			bool * pKeys=(bool *)(pData+pKeyPair[j].Offset);
			AniBlock.Animations[j].Keys.Resize(pKeyPair[j].Count);
			for(UINT k=0;k<pKeyPair[j].Count;k++)
			{
				AniBlock.Animations[j].Keys[k]=pKeys[k];
			}

		}		
	}
}

inline void LoadFakeAniBlockColor(BYTE * pData,M2_MODEL_FAKE_ANIMATION_BLOCK& AniBlockInfo,FAKE_ANIMATION_FRAME<CD3DVector3>& AniBlock)
{
	short * pTimeStamps=(short *)(pData+AniBlockInfo.TimestampsOffset);
	AniBlock.TimeStamps.Resize(AniBlockInfo.TimestampsCount);
	for(UINT k=0;k<AniBlockInfo.TimestampsCount;k++)
	{				
		AniBlock.TimeStamps[k]=pTimeStamps[k]/32767.0f;
	}
	CD3DVector3 * pKeys=(CD3DVector3 *)(pData+AniBlockInfo.KeysOffset);
	AniBlock.Keys.Resize(AniBlockInfo.KeysCount);
	for(UINT k=0;k<AniBlockInfo.KeysCount;k++)
	{
		AniBlock.Keys[k]=pKeys[k];
	}

}

inline void LoadFakeAniBlockAlpha(BYTE * pData,M2_MODEL_FAKE_ANIMATION_BLOCK& AniBlockInfo,FAKE_ANIMATION_FRAME<FLOAT>& AniBlock)
{
	short * pTimeStamps=(short *)(pData+AniBlockInfo.TimestampsOffset);	
	AniBlock.TimeStamps.Resize(AniBlockInfo.TimestampsCount);
	for(UINT k=0;k<AniBlockInfo.TimestampsCount;k++)
	{				
		AniBlock.TimeStamps[k]=pTimeStamps[k]/32767.0f;
	}
	WORD * pKeys=(WORD *)(pData+AniBlockInfo.KeysOffset);
	AniBlock.Keys.Resize(AniBlockInfo.KeysCount);
	for(UINT k=0;k<AniBlockInfo.KeysCount;k++)
	{
		FLOAT Alpha=(FLOAT)pKeys[k]/0x7FFF;
		AniBlock.Keys[k]=Alpha;
	}
}
inline void LoadFakeAniBlockSize(BYTE * pData,M2_MODEL_FAKE_ANIMATION_BLOCK& AniBlockInfo,FAKE_ANIMATION_FRAME<CD3DVector2>& AniBlock)
{
	short * pTimeStamps=(short *)(pData+AniBlockInfo.TimestampsOffset);
	AniBlock.TimeStamps.Resize(AniBlockInfo.TimestampsCount);
	for(UINT k=0;k<AniBlockInfo.TimestampsCount;k++)
	{				
		AniBlock.TimeStamps[k]=pTimeStamps[k]/32767.0f;
	}
	CD3DVector2 * pKeys=(CD3DVector2 *)(pData+AniBlockInfo.KeysOffset);
	AniBlock.Keys.Resize(AniBlockInfo.KeysCount);
	for(UINT k=0;k<AniBlockInfo.KeysCount;k++)
	{
		AniBlock.Keys[k]=pKeys[k];
	}
}
inline void LoadFakeAniBlockShort(BYTE * pData,M2_MODEL_FAKE_ANIMATION_BLOCK& AniBlockInfo,FAKE_ANIMATION_FRAME<short>& AniBlock)
{
	short * pTimeStamps=(short *)(pData+AniBlockInfo.TimestampsOffset);
	AniBlock.TimeStamps.Resize(AniBlockInfo.TimestampsCount);
	for(UINT k=0;k<AniBlockInfo.TimestampsCount;k++)
	{				
		AniBlock.TimeStamps[k]=pTimeStamps[k]/32767.0f;
	}
	short * pKeys=(short *)(pData+AniBlockInfo.KeysOffset);
	AniBlock.Keys.Resize(AniBlockInfo.KeysCount);
	for(UINT k=0;k<AniBlockInfo.KeysCount;k++)
	{
		AniBlock.Keys[k]=pKeys[k];
	}
}

}