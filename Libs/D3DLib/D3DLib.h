/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DLib.h                                                 */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

#define D3DLIB_VERSION	"1.0.0.0"

#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#undef new
#endif




#define LOG_D3D_ERROR_CHANNEL	1202


inline BOOL PrintD3DLog(DWORD Color,LPCTSTR Format,...)
{
	va_list vl;
	va_start(vl,Format);
	BOOL ret=CLogManager::GetInstance()->PrintLogVL(LOG_D3D_ERROR_CHANNEL,ILogPrinter::LOG_LEVEL_NORMAL,Color,Format,vl);
	va_end(vl);
	return ret;
}

inline BOOL PrintD3DDebugLog(DWORD Color,LPCTSTR Format,...)
{
	va_list vl;
	va_start(vl,Format);
	BOOL ret=CLogManager::GetInstance()->PrintLogVL(LOG_D3D_ERROR_CHANNEL,ILogPrinter::LOG_LEVEL_DEBUG,Color,Format,vl);
	va_end(vl);
	return ret;
}

#include <D3D9.h>

#include <D3DX9.h>

#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#define new new( __FILE__, __LINE__ )
#endif

#include <algorithm>

#include "D3DTools.h"

#include "D3DVector2.h"
#include "D3DVector3.h"
#include "D3DVector4.h"
#include "D3DQuaternion.h"
#include "D3DMatrix.h"
#include "D3DPlane.h"
#include "D3DBoundingBox.h"
#include "D3DBoundingSphere.h"
#include "D3DFrustum.h"

#include "Shaders.h"

#include "Types.h"



#include "D3DDevice.h"

#include "D3DSwapChain.h"

#include "D3DFX.h"
#include "D3DBuildInFXIncludeParser.h"
#include "D3DFXManager.h"

#include "D3DTexture.h"
#include "D3DTextTexture.h"
#include "D3DIFLTexture.h"
#include "D3DTextureManager.h"

#include "D3DSubMeshMaterial.h"
#include "D3DSubMesh.h"

#include "D3DBaseRender.h"
#include "D3DUIRender.h"
#include "D3DSceneRender.h"
#include "D3DRenderThread.h"

#include "AniDefines.h"
#include "D3DObject.h"

#include "D3DObjectResource.h"
#include "D3DObjectResourceManager.h"

#include "D3DCamera.h"
#include "D3DLight.h"

#include "D3DScene.h"

#include "D3DLine.h"
#include "D3DBoundingFrame.h"
#include "D3DBoard.h"
#include "D3DDummy.h"

#include "D3DBaseStaticModel.h"
//#include "D3DStaticModelResource.h"
//#include "D3DStaticModel.h"

#include "D3DBaseDynamicModel.h"

//#include "D3DBonedAniResource.h"
//#include "D3DBonedModelResource.h"
//#include "D3DBonedDynamicModel.h"
//
//#include "D3DFrameDynamicModel.h"

#include "BLZFileDefines.h"
#include "BLZChunkFile.h"
#include "BLZDBCFile.h"
#include "BLZBLSImporter.h"
#include "BLZWOWDatabase.h"
#include "D3DParticleEmitterResource.h"
#include "D3DBillBoardParticleEmitter.h"
#include "D3DRibbonEmitterResource.h"
#include "D3DRibbonEmitter.h"
#include "D3DWOWM2ModelResource.h"
#include "D3DWOWM2Model.h"
#include "D3DWOWM2CharacterModel.h"
#include "D3DWOWM2ItemModel.h"
#include "D3DWOWDoodadModel.h"

#include "D3DWOWWMOModelResource.h"
#include "D3DWOWWMOGroupModel.h"
#include "D3DWOWWMOModel.h"

#include "D3DWOWADTModelResource.h"
#include "D3DWOWADTModel.h"

#include "D3DWOWWDLModelResource.h"
#include "D3DWOWWDLModel.h"

#include "BLZWDTFile.h"

#include "BLZWOWSceneLoader.h"

#include "D3DBaseFont.h"
#include "D3DDX9Font.h"
#include "D3DFontCharClip.h"
#include "D3DFontCN.h"
#include "D3DFontManager.h"

#include "USOD3DObjectCreateFilter.h"



#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"d3d9.lib")




using namespace D3DLib;