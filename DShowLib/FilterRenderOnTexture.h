/****************************************************************************/
/*                                                                          */
/*      文件名:    FilterRenderOnTexture.h                                  */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace DShowLib{


struct __declspec(uuid("{19D243C6-59C0-4dfa-B5B2-4CB9D64F8A40}")) CLSID_FilterRenderOnTexture;

enum TEXTURE_FORMAT_SUPPORT
{
	SUPPORT_D3DFMT_YUY2=1,
	SUPPORT_D3DFMT_UYVY=(1<<1),
	SUPPORT_D3DFMT_X8R8G8B8=(1<<2),
	SUPPORT_D3DFMT_R5G6B5=(1<<3),
};

enum MEDIA_SOURCE_PIXEL_FORMAT
{
	MSPF_YUY2=1,
	MSPF_UYVY,
	MSPF_R8G8B8,
	MSPF_X8R8G8B8,
	MSPF_R5G6B5,
	MSPF_I420,
};

class CFilterRenderOnTexture :
	public CBaseVideoRenderer
{
protected:	
	BOOL					m_IsUseDynamicTextures;
	BOOL					m_MustTexturePOW2;
	BOOL					m_IsUseInCube;
	FLOAT					m_fTU;
	FLOAT					m_fTV;
	int						m_Width;
	int						m_Height;
	int						m_OrgWidth;
	int						m_OrgHeight;
	DWORD					m_SourceFormat;
	DWORD					m_TextureFormat;
	DWORD					m_SupportTextureFormat;
	BYTE *					m_pPixelBuffer;
	LONG					m_lVidPitch;

	LPDIRECT3DTEXTURE9		m_pTargetTexture;
	CEasyCriticalSection    m_ObjectLock;
	volatile bool			m_IsVideoUpdate;
	
public:
	CFilterRenderOnTexture(CD3DDevice* pD3DDevice,HRESULT *phr,BOOL UseInCube=FALSE);
	~CFilterRenderOnTexture(void);

public:
	HRESULT CheckMediaType(const CMediaType *pmt );     // Format acceptable?
	HRESULT SetMediaType(const CMediaType *pmt );       // Video format notification
	HRESULT DoRenderSample(IMediaSample *pMediaSample); // New video sample

	void RenderToTexture();

	FLOAT GetUS()
	{
		return m_fTU;
	}
	FLOAT GetVS()
	{
		return m_fTV;
	}

	void GetSize(int& Width,int& Height)
	{
		Width=m_Width;
		Height=m_Height;
	}
	void GetOrgSize(int& Width,int& Height)
	{
		Width=m_OrgWidth;
		Height=m_OrgHeight;
	}
	DWORD GetTextureFormat()
	{
		return m_TextureFormat;
	}
	BOOL IsUseDynamicTextures()
	{
		return m_IsUseDynamicTextures;
	}
	bool SetTargetTexture(LPDIRECT3DTEXTURE9 pTargetTexture);
protected:
	void TranslatePixelFormat(DWORD SourcePixelFormat,int SourceWidth,int SourceHeight,UINT SourcePitch,BYTE * pSourceBuffer,DWORD TargetPixelFormat,UINT TargetPitch,BYTE * pTargetBuffer);
	
};

}