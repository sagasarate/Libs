/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DFX.cpp                                                */
/*      创建日期:  2009年09月25日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

namespace D3DLib{

IMPLEMENT_FILE_PATH_MANAGER(CD3DFX)
IMPLEMENT_FILE_CHANNEL_MANAGER(CD3DFX)

IMPLEMENT_CLASS_INFO(CD3DFX,CNameObject);

CD3DFX::CD3DFX():CNameObject()
{
	m_pManager=NULL;
	m_pEffect=NULL;
	m_hActiveTech=NULL;
}

CD3DFX::CD3DFX(CD3DFXManager * pD3DFXManager):CNameObject()
{
	m_pManager=pD3DFXManager;
	m_pEffect=NULL;
	m_hActiveTech=NULL;
}

CD3DFX::~CD3DFX(void)
{
	Destory();
	if(m_pManager&&GetID())
		m_pManager->DeleteFX(GetID());
}

void CD3DFX::Destory()
{
	SAFE_RELEASE(m_pEffect);
	m_hActiveTech=NULL;
	CNameObject::Destory();
}

bool CD3DFX::Reset()
{
	SAFE_RELEASE(m_pEffect);
	return true;
}

bool CD3DFX::Restore()
{
	if(m_CompiledEffectData.GetUsedSize())
		return LoadFXDirect(m_CompiledEffectData.GetBuffer(),m_CompiledEffectData.GetUsedSize());
	return true;
}

//void CD3DFX::Release()
//{
//	DecUseRef();
//	if(GetUseRef()<=0)
//	{		
//		delete this;
//	}
//}

bool CD3DFX::LoadFromFile(LPCTSTR FileName)
{
	IFileAccessor * pFile;


	CEasyString FxFileName=FindFileOne(FileName);
	pFile=CreateFileAccessor();
	if(pFile==NULL)
		return false;

	PrintD3DDebugLog(_T("装载FX<%s>....."),(LPCTSTR)FxFileName);
	if(pFile->Open(FxFileName,IFileAccessor::modeRead))
	{
		UINT Size=(UINT)pFile->GetSize();

		m_EffectData.Create(Size+1);		

		Size=(UINT)pFile->Read(m_EffectData.GetBuffer(),Size);
		m_EffectData.SetUsedSize(Size);
		m_EffectData.PushConstBack(0,1);
		
		pFile->Release();
		
		if(LoadFromMemory(NULL,0))
		{
			PrintD3DDebugLog(_T("装载FX<%s>成功"),(LPCTSTR)FxFileName);
			return true;
		}
	}
	else
		pFile->Release();
	PrintD3DLog(_T("装载FX<%s>失败%d"),(LPCTSTR)FxFileName,GetLastError());
	return false;
}

#ifdef D3D_DEBUG_INFO

bool CD3DFX::LoadFromFileDirect(LPCTSTR FileName)
{
	HRESULT hr;
	LPD3DXBUFFER pErrors=NULL;
	LPD3DXEFFECTPOOL pEffectPool=NULL;
	Reset();

	if(m_pManager)
		pEffectPool=m_pManager->GetEffectPool();
	CEasyString FxFileName=FindFileOne(FileName);
	hr=D3DXCreateEffectFromFile(
		m_pManager->GetDevice()->GetD3DDevice(),
		FxFileName,
		NULL,
		m_pManager->GetBuildInFXIncludeParser(),
		D3DXSHADER_DEBUG|D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT,
		pEffectPool,
		&m_pEffect,
		&pErrors);
	if(m_pEffect==NULL)
	{
		CEasyString ErrorMsg;
		if(pErrors)
			ErrorMsg.Format(_T("无法加载FX,Err=(%s)"),(char*)(pErrors->GetBufferPointer()));
		else
			ErrorMsg.Format(_T("无法加载FX"));
		MessageBox(GetForegroundWindow(),ErrorMsg,"无法加载FX",MB_OK);
		SAFE_RELEASE(pErrors);
		return false;
	}

	m_hActiveTech=m_pEffect->GetTechnique(0);

	return true;
}

#endif

bool CD3DFX::LoadFromMemory(const void * pData,int DataSize)
{	
	if(pData)
	{
		m_EffectData.Create(DataSize+1);
		m_EffectData.PushBack(pData,DataSize);
		m_EffectData.PushConstBack(0,1);
	}

	UINT CompileFlag=0;
	
	LPD3DXEFFECTCOMPILER pCompiler=NULL;
	LPD3DXBUFFER pErrors=NULL;
	
	if(D3DXCreateEffectCompiler((LPCSTR)m_EffectData.GetBuffer(),m_EffectData.GetUsedSize(),
		NULL,m_pManager->GetBuildInFXIncludeParser(),
		CompileFlag,
		&pCompiler,
		&pErrors)==D3D_OK)
	{
		LPD3DXBUFFER pCompiledData;
		if(pCompiler->CompileEffect(CompileFlag,&pCompiledData,&pErrors)==D3D_OK)
		{
			m_CompiledEffectData.Create(pCompiledData->GetBufferSize());
			m_CompiledEffectData.PushBack(pCompiledData->GetBufferPointer(),
				pCompiledData->GetBufferSize());
			SAFE_RELEASE(pCompiledData);

			if(pErrors)
				PrintD3DLog(_T("%s"),(LPCTSTR)(pErrors->GetBufferPointer()));

			return LoadFXDirect(m_CompiledEffectData.GetBuffer(),m_CompiledEffectData.GetUsedSize());
		}
		else
		{
			if(pErrors)
				PrintD3DLog(_T("无法编译FX,Err=(%s)"),(LPCTSTR)(pErrors->GetBufferPointer()));
			else
				PrintD3DLog(_T("无法编译FX"));
			SAFE_RELEASE(pErrors);
		}
		SAFE_RELEASE(pCompiler);
		return false;
	}
	else
	{
		if(pErrors)
			PrintD3DLog(_T("无法创建FX编译器,Err=(%s)"),(LPCTSTR)(pErrors->GetBufferPointer()));
		else
			PrintD3DLog(_T("无法创建FX编译"));
		SAFE_RELEASE(pErrors);
		return false;
	}
	
	
}

bool CD3DFX::LoadCompiledFromMemory(const void * pData,int DataSize,const void * pSrcData,int SrcDataSize)
{
	m_CompiledEffectData.Create(DataSize);
	m_CompiledEffectData.PushBack(pData,DataSize);

	m_EffectData.Create(SrcDataSize+1);
	m_EffectData.PushBack(pSrcData,SrcDataSize);
	m_EffectData.PushConstBack(0,1);


	return LoadFXDirect(m_CompiledEffectData.GetBuffer(),m_CompiledEffectData.GetUsedSize());
}


bool CD3DFX::SetActiveTechnique(int Index)
{	
	if(m_pEffect)
	{
		D3DXHANDLE  hTech = m_pEffect->GetTechnique(Index);
		if(hTech == NULL)
			return false;
		if(FAILED(m_pEffect->ValidateTechnique(hTech)))
			return false;

		m_hActiveTech = hTech;
	}
	else
		return false;

	
	return true;
}

bool CD3DFX::SetActiveTechnique(LPCSTR TechName)
{
	if(m_pEffect)
	{
		D3DXHANDLE  hTech = m_pEffect->GetTechniqueByName(TechName);
		if(hTech == NULL)
			return false;
		if(FAILED(m_pEffect->ValidateTechnique(hTech)))
			return false;		

		m_hActiveTech = hTech;
	}
	else
		return false;
	return true;
}

bool CD3DFX::UseActiveTechnique()
{
	if(m_hActiveTech)
	{
		if(FAILED(m_pEffect->SetTechnique(m_hActiveTech)))
			return false;
	}
	return false;
}

bool CD3DFX::UseTechnique(LPCSTR TechName)
{
	if(m_pEffect)
	{
		D3DXHANDLE  hTech = m_pEffect->GetTechniqueByName(TechName);
		if(hTech == NULL)
			return false;
		if(FAILED(m_pEffect->ValidateTechnique(hTech)))
			return false;		

		if(FAILED(m_pEffect->SetTechnique(hTech)))
			return false;
	}
	else
		return false;
	return true;
}

int CD3DFX::Begin()
{
	UINT Pass;
	if(SUCCEEDED(m_pEffect->Begin(&Pass,D3DXFX_DONOTSAVESTATE)))
	{
		return (int)Pass;
	}
	return -1;
}

bool CD3DFX::End()
{
	if(SUCCEEDED(m_pEffect->End()))
		return true;
	else
		return false;
}

bool CD3DFX::BeginPass(int Pass)
{
	if(SUCCEEDED(m_pEffect->BeginPass(Pass)))
		return true;
	else
		return false;
}

bool CD3DFX::EndPass(int Pass)
{
	if(SUCCEEDED(m_pEffect->EndPass()))
		return true;
	else
		return false;
}


bool CD3DFX::SetTexture(LPCSTR ParamName,CD3DTexture * pTexture)
{	
	HRESULT	hr;

	if(pTexture==NULL)
		return false;

	

	if(pTexture->GetD3DTexture())
	{
		hr=m_pEffect->SetTexture(ParamName,pTexture->GetD3DTexture());
		if(SUCCEEDED(hr))
		{
			return true;
		} 
	}
	return false;
}

bool CD3DFX::SetMatrix(LPCSTR ParamName,const CD3DMatrix& Mat)
{	
	HRESULT	hr;
	
	hr=m_pEffect->SetMatrix(ParamName,&Mat);
	if(SUCCEEDED(hr))
	{
		return true;
	} 
	
	return false;
}

bool CD3DFX::SetMatrixArray(LPCSTR ParamName,const CD3DMatrix * pMats,int Count)
{	
	HRESULT	hr;

	if(pMats==NULL)
		return false;

	
	hr=m_pEffect->SetMatrixArray(ParamName,pMats,Count);
	if(SUCCEEDED(hr))
	{
		return true;
	} 
	
	return false;
}

bool CD3DFX::SetVector(LPCSTR ParamName,const CD3DVector2& Vec)
{
	HRESULT	hr;
	hr=m_pEffect->SetFloatArray(ParamName,(CONST FLOAT *)Vec,2);
	if(SUCCEEDED(hr))
	{
		return true;
	} 
	return false;
}

bool CD3DFX::SetVector(LPCSTR ParamName,const CD3DVector3& Vec)
{
	HRESULT	hr;
	hr=m_pEffect->SetFloatArray(ParamName,(CONST FLOAT *)Vec,3);
	if(SUCCEEDED(hr))
	{
		return true;
	} 
	return false;
}

bool CD3DFX::SetVector(LPCSTR ParamName,const CD3DVector4& Vec)
{
	HRESULT	hr;
	hr=m_pEffect->SetVector(ParamName,&Vec);
	if(SUCCEEDED(hr))
	{
		return true;
	} 
	return false;
}

bool CD3DFX::SetQuaternion(LPCSTR ParamName,const CD3DQuaternion& Quat)
{
	HRESULT	hr;
	hr=m_pEffect->SetFloatArray(ParamName,(CONST FLOAT *)Quat,4);
	if(SUCCEEDED(hr))
	{
		return true;
	} 
	return false;
}

bool CD3DFX::SetColor(LPCSTR ParamName,const D3DCOLORVALUE& Color)
{
	HRESULT	hr;
	hr=m_pEffect->SetFloatArray(ParamName,(CONST FLOAT *)&Color,4);
	if(SUCCEEDED(hr))
	{
		return true;
	} 
	return false;
}

bool CD3DFX::SetColor(LPCSTR ParamName,D3DCOLOR Color)
{
	D3DCOLORVALUE ColorValue;
	ColorValue.a=((FLOAT)((Color>>24)&0xFF))/255.0f;
	ColorValue.r=((FLOAT)((Color>>16)&0xFF))/255.0f;
	ColorValue.g=((FLOAT)((Color>>8)&0xFF))/255.0f;
	ColorValue.b=((FLOAT)((Color)&0xFF))/255.0f;
	return SetColor(ParamName,ColorValue);
}

bool CD3DFX::SetInt(LPCSTR ParamName,int Value)
{
	HRESULT	hr;
	hr=m_pEffect->SetInt(ParamName,Value);
	if(SUCCEEDED(hr))
	{
		return true;
	} 
	return false;
}

bool CD3DFX::SetIntArray(LPCSTR ParamName,const int * pValues,int Count)
{
	HRESULT	hr;
	hr=m_pEffect->SetIntArray(ParamName,pValues,Count);
	if(SUCCEEDED(hr))
	{
		return true;
	} 
	return false;
}

bool CD3DFX::SetFloat(LPCSTR ParamName,FLOAT Value)
{
	HRESULT	hr;
	hr=m_pEffect->SetFloat(ParamName,Value);
	if(SUCCEEDED(hr))
	{
		return true;
	} 
	return false;
}

bool CD3DFX::SetBool(LPCSTR ParamName,BOOL Value)
{
	HRESULT	hr;
	hr=m_pEffect->SetBool(ParamName,Value);
	if(SUCCEEDED(hr))
	{
		return true;
	} 
	return false;
}

bool CD3DFX::SetValue(LPCSTR ParamName,LPCVOID pData,UINT DataSize)
{
	HRESULT	hr;
	hr=m_pEffect->SetValue(ParamName,pData,DataSize);
	if(SUCCEEDED(hr))
	{
		return true;
	} 
	return false;
}

bool CD3DFX::GetInt(LPCSTR ParamName,int& Value)
{
	HRESULT	hr;
	hr=m_pEffect->GetInt(ParamName,&Value);
	if(SUCCEEDED(hr))
	{
		return true;
	} 
	return false;
}

bool CD3DFX::GetFloat(LPCSTR ParamName,FLOAT& Value)
{
	HRESULT	hr;
	hr=m_pEffect->GetFloat(ParamName,&Value);
	if(SUCCEEDED(hr))
	{
		return true;
	} 
	return false;
}

bool CD3DFX::GetBool(LPCSTR ParamName,BOOL& Value)
{
	HRESULT	hr;
	hr=m_pEffect->GetBool(ParamName,&Value);
	if(SUCCEEDED(hr))
	{
		return true;
	} 
	return false;
}

bool CD3DFX::CommitChanges()
{
	HRESULT	hr;
	hr=m_pEffect->CommitChanges();
	if(SUCCEEDED(hr))
	{
		return true;
	} 
	return false;
}

bool CD3DFX::LoadFXDirect(const void * pData,int DataSize)
{
	HRESULT hr;
	LPD3DXBUFFER pErrors=NULL;
	LPD3DXEFFECTPOOL pEffectPool=NULL;

	Reset();

	if(m_pManager)
		pEffectPool=m_pManager->GetEffectPool();

	hr=D3DXCreateEffect(
		m_pManager->GetDevice()->GetD3DDevice(),
		pData,
		DataSize,
		NULL,
		m_pManager->GetBuildInFXIncludeParser(),
#ifdef D3D_DEBUG_INFO
		D3DXSHADER_DEBUG|D3DXSHADER_SKIPOPTIMIZATION,		
#else
		D3DXSHADER_SKIPVALIDATION,
#endif
		pEffectPool,
		&m_pEffect,
		&pErrors);
	if(m_pEffect==NULL)
	{
		if(pErrors)
			PrintD3DLog(_T("无法加载FX,Err=(%s)"),(LPCTSTR)(pErrors->GetBufferPointer()));
		else
			PrintD3DLog(_T("无法加载FX"));
		SAFE_RELEASE(pErrors);
		return false;
	}

	m_hActiveTech=m_pEffect->GetTechnique(0);

	return true;
}



bool CD3DFX::ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CNameObject::ToSmartStruct(Packet,pResourceManager,Param))
		return false;	
	if(m_EffectData.GetUsedSize())
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DFX_EFFECT_DATA,(LPCSTR)m_EffectData.GetBuffer(),m_EffectData.GetUsedSize()));
	}

	if(m_CompiledEffectData.GetUsedSize())
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DFX_COMPILED_EFFECT_DATA,(LPCSTR)m_CompiledEffectData.GetBuffer(),m_CompiledEffectData.GetUsedSize()));
	}
	
	return true;
}
bool CD3DFX::FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CNameObject::FromSmartStruct(Packet,pResourceManager,Param))
		return false;

	CSmartValue EffectData=Packet.GetMember(SST_D3DFX_EFFECT_DATA);
	CSmartValue CompiledEffectData=Packet.GetMember(SST_D3DFX_COMPILED_EFFECT_DATA);

	if(CompiledEffectData.GetLength())
	{
		if(!LoadCompiledFromMemory((LPCSTR)CompiledEffectData,CompiledEffectData.GetLength(),
			(LPCSTR)EffectData,EffectData.GetLength()))
			return false;
	}
	else
	{
		if(!LoadFromMemory((LPCSTR)EffectData,EffectData.GetLength()))
			return false;
	}

	if(m_pManager)
		m_pManager->AddFX(this,GetName());

	return true;
}

UINT CD3DFX::GetSmartStructSize(UINT Param)
{
	UINT Size=CNameObject::GetSmartStructSize(Param);	
	Size += CSmartStruct::GetStringMemberSizeA(m_EffectData.GetUsedSize());
	Size += CSmartStruct::GetStringMemberSizeA(m_CompiledEffectData.GetUsedSize());
	return Size;
}



}