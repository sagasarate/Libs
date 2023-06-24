/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DTexture.h                                             */
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

class CD3DTextureManager;

class CD3DTexture :
	public CNameObject
{
protected:		
	struct TEXTURE_ORG_INFO
	{
		UINT						MipLevels;
		int							Width;
		int							Height;
		DWORD						PixelFormat;
		DWORD						Usage;
		DWORD						Pool;
		bool						UseFilter;
		bool						IsManaged;
		bool						WantRestore;
		D3DCOLOR					KeyColor;
		UINT						DataSize;
	};

	//struct STORAGE_STRUCT:public CNameObject::STORAGE_STRUCT
	//{		
	//	UINT			MipLevels;
	//	bool			UseFilter;
	//	bool			IsManaged;
	//	D3DCOLOR		KeyColor;
	//	UINT			DataSize;
	//};

	enum SST_MEMBER_ID
	{
		SST_D3DTEX_TESTURE_DATA=SST_NO_MAX,
		SST_D3DTEX_MIP_LEVEL,
		SST_D3DTEX_USE_FILTER,
		SST_D3DTEX_IS_MANAGED,
		SST_D3DTEX_KEY_COLOR,
		SST_D3DTEX_MAX=SST_NO_MAX+50,
	};

	CD3DTextureManager *		m_pManager;
	LPDIRECT3DTEXTURE9			m_pTexture;
	D3DXIMAGE_INFO				m_TextureInfo;
	D3DSURFACE_DESC				m_TextureSurfaceInfo;	
	TEXTURE_ORG_INFO			m_TextureOrgInfo;
	LPD3DXBUFFER				m_pTextureSaveData;

	static bool					m_IsSaveOrgInfo;

	DECLARE_FILE_PATH_MANAGER
	DECLARE_FILE_CHANNEL_MANAGER

	DECLARE_CLASS_INFO(CD3DTexture)

public:
	CD3DTexture();
	CD3DTexture(CD3DTextureManager * pManager);
	virtual ~CD3DTexture();
	void SetManager(CD3DTextureManager * pManager);
	virtual void Destory();

	virtual bool Reset();
	virtual bool Restore();		

	virtual bool CreateTexture(int Width,int Height,DWORD PixelFormat,DWORD Usage=0,DWORD Pool=D3DPOOL_MANAGED,UINT MipLevels=1);

	virtual bool LoadTexture(LPCTSTR TextureFileName,UINT MipLevels=1,bool UseFilter=true,bool IsManaged=true,D3DCOLOR KeyColor=0);
	virtual bool LoadTexture(IFileAccessor * pFileAccessor,UINT MipLevels=1,bool UseFilter=true,bool IsManaged=true,D3DCOLOR KeyColor=0);
	virtual bool LoadTextureFromMemory(LPVOID pData,int DataSize,UINT MipLevels=1,bool UseFilter=true,bool IsManaged=true,D3DCOLOR KeyColor=0);
	virtual LPDIRECT3DTEXTURE9  GetD3DTexture()
	{return m_pTexture;}
	
	
	virtual void Update(FLOAT Time);
	virtual void OnPrepareRenderData();
	
	//virtual void Release();

	virtual int GetWidth()
	{return m_TextureInfo.Width;}

	virtual int GetHeight()
	{return m_TextureInfo.Height;}

	virtual FLOAT GetUS()
	{return ((FLOAT)m_TextureInfo.Width-0.5f)/m_TextureSurfaceInfo.Width;}

	virtual FLOAT GetVS()
	{return ((FLOAT)m_TextureInfo.Height-0.5f)/m_TextureSurfaceInfo.Height;}

	const LPCTSTR GetFilePath()
	{return GetName();}

	D3DXIMAGE_INFO& GetTextureInfo()
	{
		return m_TextureInfo;
	}


	bool LockBits(UINT Level,D3DLOCKED_RECT* pLockedRect,const RECT* pRect,DWORD Flags);
	bool UnlockBits(UINT Level);

	static void EnableSaveOrgInfo(bool Enable);

	virtual bool ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual UINT GetSmartStructSize(UINT Param=0);

protected:
	bool LoadBLPTexture(LPVOID pData,int DataSize,DWORD Usage,bool IsManaged);

	//virtual CNameObject::STORAGE_STRUCT * USOCreateHead(UINT Param=0);
	//virtual int USOWriteHead(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param=0);
	//virtual bool USOWriteData(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param=0);

	//virtual int USOReadHead(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param=0);
	//virtual int USOReadData(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,BYTE * pData,int DataSize,UINT Param=0);
	//virtual bool USOReadFinish(CNameObject::STORAGE_STRUCT * pHead,UINT Param=0);
};

inline void CD3DTexture::SetManager(CD3DTextureManager * pManager)
{
	m_pManager=pManager;
}

}