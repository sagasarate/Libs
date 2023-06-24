/****************************************************************************/
/*                                                                          */
/*      文件名:    Types.h                                                  */
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


#define MAX_2D_VERTICES			50*6

// Font creation flags
#define D3DFONT_BOLD        0x0001
#define D3DFONT_ITALIC      0x0002
#define D3DFONT_ZENABLE     0x0004

// Font rendering flags
#define TEXT_ALIGN_LEFT		1
#define TEXT_ALIGN_RIGHT	2
#define TEXT_ALIGN_TOP		4
#define TEXT_ALIGN_BOTTOM	8
#define TEXT_ALIGN_XCENTER	0
#define TEXT_ALIGN_YCENTER	0

#define MAX_TEXTURE_STORAGE	4096
#define MAX_FX_STORAGE		1024
#define MAX_FONT_STORAGE	1024

#define MAX_TEXTURE_LAYER		8
#define MAX_LIGHT				8
#define MAX_SHADER_BONE_NUM		0

#define MAX_HEIGHT			1.0e10f

#define MAX_PREPARE_RENDER_DATA_FAIL_COUNT	1


#define D3DCOLOR_ALPHA(Color)		((Color>>24)&0xFF)
#define D3DCOLOR_RED(Color)			((Color>>16)&0xFF)
#define D3DCOLOR_GREEN(Color)		((Color>>8)&0xFF)
#define D3DCOLOR_BLUE(Color)		(Color&0xFF)
#define D3DCOLOR_ADD(Color1,Color2)	D3DCOLOR_ARGB(D3DCOLOR_ALPHA(Color1)+D3DCOLOR_ALPHA(Color2),D3DCOLOR_RED(Color1)+D3DCOLOR_RED(Color2),D3DCOLOR_GREEN(Color1)+D3DCOLOR_GREEN(Color2),D3DCOLOR_BLUE(Color1)+D3DCOLOR_BLUE(Color2))
#define D3DCOLOR_MUL(Color,Value)	D3DCOLOR_ARGB((BYTE)(D3DCOLOR_ALPHA(Color)*(Value)),(BYTE)(D3DCOLOR_RED(Color)*(Value)),(BYTE)(D3DCOLOR_GREEN(Color)*(Value)),(BYTE)(D3DCOLOR_BLUE(Color)*(Value)))

enum D3D_RENDER_FLAG:UINT64
{
	D3D_RENDER_FLAG_NO_LIGHT=(1),
	D3D_RENDER_FLAG_NO_FOG=(1<<1),
	D3D_RENDER_FLAG_NO_CULL=(1<<2),
	D3D_RENDER_FLAG_BILL_BOARD=(1<<3),
	D3D_RENDER_FLAG_DISABLE_ZWRITE=(1<<4),
};

enum D3D_BLEND_MODE:UINT64
{
	D3D_BLEND_MODE_OPAQUE=0,
	D3D_BLEND_MODE_ALPHA_TEST=1,
	D3D_BLEND_MODE_ALPHA_BLENDING=2,
	D3D_BLEND_MODE_ADDITIVE=3,
	D3D_BLEND_MODE_ADDITIVE_ALPHA=4,
	D3D_BLEND_MODE_MODULATE=5,
	D3D_BLEND_MODE_MUL=6,
};

enum D3D_TEXTURE_FLAG:UINT64
{
	D3D_TEX_FLAG_WRAP_X=(1),
	D3D_TEX_FLAG_WRAP_Y=(1<<1),
	D3D_TEX_FLAG_ENV_MAP=(1<<2),
	D3D_TEX_FLAG_UV_ANI=(1<<3),
	D3D_TEX_FLAG_DEPTH_TEX=(1<<4),
};

enum D3D_MESH_FLAG:UINT64
{
	D3D_MESH_FLAG_USE_VERTEX_ALPHA1=(1),
	D3D_MESH_FLAG_USE_VERTEX_ALPHA2=(1<<1),
};

enum D3D_SKIN_TEXTURE_TYPE:UINT64
{	
	D3D_TEXTURE_TYPE_DIRECT=0,					//不可换皮
	D3D_TEXTURE_TYPE_BODY=1,					//身体
	D3D_TEXTURE_TYPE_CAPE=2,					//披风
	D3D_TEXTURE_TYPE_HAIR=6,					//头发
	D3D_TEXTURE_TYPE_FUR=8,						//毛发
	D3D_TEXTURE_TYPE_CREATURE_SKIN1=11,			
	D3D_TEXTURE_TYPE_CREATURE_SKIN2=12,
	D3D_TEXTURE_TYPE_CREATURE_SKIN3=13,
};

const CD3DVector3 LIGHT_START_DIR(0.0f,0.0f,1.0f);

enum MAX_VERTEX_BONE
{
	MAX_VERTEX_BONE_BIND=4,
};


struct RECTVERTEX { D3DXVECTOR4 p;   DWORD color;     FLOAT tu, tv; };


#define D3DFVF_RECTVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

inline RECTVERTEX InitRECTVertex( const D3DXVECTOR4& p, D3DCOLOR color,
                                      FLOAT tu, FLOAT tv )
{
    RECTVERTEX v;   v.p = p;   v.color = color;   v.tu = tu;   v.tv = tv;
    return v;
}

struct D3D_A8B8G8R8_PIXEL
{		
	BYTE Blue;
	BYTE Green;
	BYTE Red;
	BYTE Alpha;	
};


}
