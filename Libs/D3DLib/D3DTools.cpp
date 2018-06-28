/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DTools.cpp		                                        */
/*      创建日期:  2012年03月01日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

namespace D3DLib{

LPCTSTR D3DFormatToString(D3DFORMAT Format)
{
	switch(Format)
	{
	case D3DFMT_R8G8B8:
		return "D3DFMT_R8G8B8";
	case D3DFMT_A8R8G8B8:
		return "D3DFMT_A8R8G8B8";
	case D3DFMT_X8R8G8B8:
		return "D3DFMT_X8R8G8B8";
	case D3DFMT_R5G6B5:
		return "D3DFMT_R5G6B5";
	case D3DFMT_X1R5G5B5:
		return "D3DFMT_X1R5G5B5";
	case D3DFMT_A1R5G5B5:
		return "D3DFMT_A1R5G5B5";
	case D3DFMT_A4R4G4B4:
		return "D3DFMT_A4R4G4B4";
	case D3DFMT_R3G3B2:
		return "D3DFMT_R3G3B2";
	case D3DFMT_A8:
		return "D3DFMT_A8";
	case D3DFMT_A8R3G3B2:
		return "D3DFMT_A8R3G3B2";
	case D3DFMT_X4R4G4B4:
		return "D3DFMT_X4R4G4B4";
	case D3DFMT_A2B10G10R10:
		return "D3DFMT_A2B10G10R10";
	case D3DFMT_A8B8G8R8:
		return "D3DFMT_A8B8G8R8";
	case D3DFMT_X8B8G8R8:
		return "D3DFMT_X8B8G8R8";
	case D3DFMT_G16R16:
		return "D3DFMT_G16R16";
	case D3DFMT_A2R10G10B10:
		return "D3DFMT_A2R10G10B10";
	case D3DFMT_A16B16G16R16:
		return "D3DFMT_A16B16G16R16";
	case D3DFMT_A8P8:
		return "D3DFMT_A8P8";
	case D3DFMT_P8:
		return "D3DFMT_P8";
	case D3DFMT_L8:
		return "D3DFMT_L8";
	case D3DFMT_A8L8:
		return "D3DFMT_A8L8";
	case D3DFMT_A4L4:
		return "D3DFMT_A4L4";
	case D3DFMT_V8U8:
		return "D3DFMT_V8U8";
	case D3DFMT_L6V5U5:
		return "D3DFMT_L6V5U5";
	case D3DFMT_X8L8V8U8:
		return "D3DFMT_X8L8V8U8";
	case D3DFMT_Q8W8V8U8:
		return "D3DFMT_Q8W8V8U8";
	case D3DFMT_V16U16:
		return "D3DFMT_V16U16";
	case D3DFMT_A2W10V10U10:
		return "D3DFMT_A2W10V10U10";
	case D3DFMT_D16_LOCKABLE:
		return "D3DFMT_D16_LOCKABLE";
	case D3DFMT_D32:
		return "D3DFMT_D32";
	case D3DFMT_D15S1:
		return "D3DFMT_D15S1";
	case D3DFMT_D24S8:
		return "D3DFMT_D24S8";
	case D3DFMT_D24X8:
		return "D3DFMT_D24X8";
	case D3DFMT_D24X4S4:
		return "D3DFMT_D24X4S4";
	case D3DFMT_D16:
		return "D3DFMT_D16";
	case D3DFMT_D32F_LOCKABLE:
		return "D3DFMT_D32F_LOCKABLE";
	case D3DFMT_D24FS8:
		return "D3DFMT_D24FS8";
#ifdef D3DFMT_D32_LOCKABLE
	case D3DFMT_D32_LOCKABLE:
		return "D3DFMT_D32_LOCKABLE";
#endif
#ifdef D3DFMT_S8_LOCKABLE
	case D3DFMT_S8_LOCKABLE:
		return "D3DFMT_S8_LOCKABLE";
#endif
	case D3DFMT_L16:
		return "D3DFMT_L16";
	case D3DFMT_VERTEXDATA:
		return "D3DFMT_VERTEXDATA";
	case D3DFMT_INDEX16:
		return "D3DFMT_INDEX16";
	case D3DFMT_INDEX32:
		return "D3DFMT_INDEX32";
	case D3DFMT_Q16W16V16U16:
		return "D3DFMT_Q16W16V16U16";
	case D3DFMT_R16F:
		return "D3DFMT_R16F";
	case D3DFMT_G16R16F:
		return "D3DFMT_G16R16F";
	case D3DFMT_A16B16G16R16F:
		return "D3DFMT_A16B16G16R16F";
	case D3DFMT_R32F:
		return "D3DFMT_R32F";
	case D3DFMT_G32R32F:
		return "D3DFMT_G32R32F";
	case D3DFMT_A32B32G32R32F:
		return "D3DFMT_A32B32G32R32F";
	case D3DFMT_CxV8U8:
		return "D3DFMT_CxV8U8";
#ifdef D3DFMT_A1
	case D3DFMT_A1:
		return "D3DFMT_A1";
#endif
#ifdef D3DFMT_A2B10G10R10_XR_BIAS
	case D3DFMT_A2B10G10R10_XR_BIAS:
		return "D3DFMT_A2B10G10R10_XR_BIAS";
#endif
#ifdef D3DFMT_BINARYBUFFER
	case D3DFMT_BINARYBUFFER:
		return "D3DFMT_BINARYBUFFER";
#endif
	case D3DFMT_UYVY:
		return "D3DFMT_UYVY";
	case D3DFMT_R8G8_B8G8:
		return "D3DFMT_R8G8_B8G8";
	case D3DFMT_YUY2:
		return "D3DFMT_YUY2";
	case D3DFMT_G8R8_G8B8:
		return "D3DFMT_G8R8_G8B8";
	case D3DFMT_DXT1:
		return "D3DFMT_DXT1";
	case D3DFMT_DXT2:
		return "D3DFMT_DXT2";
	case D3DFMT_DXT3:
		return "D3DFMT_DXT3";
	case D3DFMT_DXT4:
		return "D3DFMT_DXT4";
	case D3DFMT_DXT5:
		return "D3DFMT_DXT5";
	case D3DFMT_MULTI2_ARGB8:
		return "D3DFMT_MULTI2_ARGB8";
	default:
		return "UNKNOWN";
	}
}

}