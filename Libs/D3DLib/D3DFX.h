/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DFX.h                                                  */
/*      创建日期:  2009年09月25日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace D3DLib{

class CD3DFXManager;
class CD3DTexture;

class CD3DFX :
	public CNameObject
{
protected:
	//struct STORAGE_STRUCT:public CNameObject::STORAGE_STRUCT
	//{		
	//	UINT			DataSize;	
	//};
	enum SST_MEMEBER_ID
	{
		SST_D3DFX_EFFECT_DATA=SST_NO_MAX,
		SST_D3DFX_COMPILED_EFFECT_DATA,
		SST_D3DFX_MAX=SST_NO_MAX+50,
	};

	CD3DFXManager *	m_pManager;
	LPD3DXEFFECT	m_pEffect;
	D3DXHANDLE		m_hActiveTech;
	CEasyBuffer		m_EffectData;
	CEasyBuffer		m_CompiledEffectData;

	DECLARE_FILE_PATH_MANAGER
	DECLARE_FILE_CHANNEL_MANAGER

	DECLARE_CLASS_INFO(CD3DFX)
	
public:
	CD3DFX();
	CD3DFX(CD3DFXManager * pD3DFXManager);
	virtual ~CD3DFX(void);
	void SetManager(CD3DFXManager * pD3DFXManager);
	CD3DFXManager * GetManager();

	virtual void Destory();
	virtual bool Reset();
	virtual bool Restore();

	LPCSTR GetFXContent();
	
	//virtual void Release();

	bool LoadFromFile(LPCTSTR FileName);
#ifdef D3D_DEBUG_INFO
	bool LoadFromFileDirect(LPCTSTR FileName);
#endif
	bool LoadFromMemory(const void * pData,int DataSize);
	bool LoadCompiledFromMemory(const void * pData,int DataSize,const void * pSrcData,int SrcDataSize);
	bool LoadFXDirect(const void * pData,int DataSize);

	bool SetActiveTechnique(int Index);
	bool SetActiveTechnique(LPCSTR TechName);

	bool UseActiveTechnique();
	bool UseTechnique(LPCSTR TechName);

	inline LPD3DXEFFECT GetEffect()
	{
		return m_pEffect;
	}

	int Begin();
	bool End();

	bool BeginPass(int Pass);
	bool EndPass(int Pass);

	bool SetTexture(LPCSTR ParamName,CD3DTexture * pTexture);
	bool SetMatrix(LPCSTR ParamName,const CD3DMatrix& Mat);
	bool SetMatrixArray(LPCSTR ParamName,const CD3DMatrix * pMats,int Count);
	bool SetVector(LPCSTR ParamName,const CD3DVector2& Vec);
	bool SetVector(LPCSTR ParamName,const CD3DVector3& Vec);
	bool SetVector(LPCSTR ParamName,const CD3DVector4& Vec);
	bool SetQuaternion(LPCSTR ParamName,const CD3DQuaternion& Quat);
	bool SetColor(LPCSTR ParamName,const D3DCOLORVALUE& Color);
	bool SetColor(LPCSTR ParamName,D3DCOLOR Color);
	bool SetInt(LPCSTR ParamName,int Value);
	bool SetIntArray(LPCSTR ParamName,const int * pValues,int Count);
	bool SetFloat(LPCSTR ParamName,FLOAT Value);
	bool SetBool(LPCSTR ParamName,BOOL Value);
	bool SetValue(LPCSTR ParamName,LPCVOID pData,UINT DataSize);

	bool GetInt(LPCSTR ParamName,int& Value);
	bool GetFloat(LPCSTR ParamName,FLOAT& Value);
	bool GetBool(LPCSTR ParamName,BOOL& Value);	


	bool CommitChanges();

	//virtual void AddUseRef()
	//{
	//	AtomicInc(&m_UseRef);
	//}
	//virtual void Release()
	//{
	//	DecUseRef();
	//	if(GetUseRef()<=0)
	//		delete this;
	//}

	virtual bool ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual UINT GetSmartStructSize(UINT Param=0);

protected:
	//virtual CNameObject::STORAGE_STRUCT * USOCreateHead(UINT Param=0);
	//virtual int USOWriteHead(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param=0);
	//virtual bool USOWriteData(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param=0);

	//virtual int USOReadHead(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param=0);
	//virtual int USOReadData(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,BYTE * pData,int DataSize,UINT Param=0);
	//virtual bool USOReadFinish(CNameObject::STORAGE_STRUCT * pHead,UINT Param=0);

	
	
};

inline void CD3DFX::SetManager(CD3DFXManager * pD3DFXManager)
{
	m_pManager=pD3DFXManager;
}

inline CD3DFXManager * CD3DFX::GetManager()
{
	return m_pManager;
}

inline LPCSTR CD3DFX::GetFXContent()
{
	return (LPCSTR)m_EffectData.GetBuffer();
}



}