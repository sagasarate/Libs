/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DVideoTexture.h                                        */
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

class CVMRAllocator;
class CVMRImageCompositor;
class CFilterRenderOnTexture;

enum VIDEO_TIME_UNIT
{
	VTU_ORIGINAL=1,
	VTU_SECOND=10000000,
	VTU_MILLISECOND=10000,
};

enum VIDEO_PLAY_STATUS
{
	VPS_NONE=-1,
	VPS_STOPED=State_Stopped,
	VPS_PAUSED=State_Paused,
	VPS_PLAYING=State_Running,
};

class CD3DVideoTexture :
	public CD3DTexture
{
protected:
	IFilterGraph*               m_pFilterGraph;
	IGraphBuilder*              m_pGraphBuilder;
	IMediaControl*              m_pMediaControl;
	IMediaSeeking*              m_pMediaSeeking;
	IMediaEvent*                m_pMediaEvent;

	IBaseFilter *				m_pDirectVobSub;
	
	
	CFilterRenderOnTexture*			m_pFilterRenderOnTexture;

	LONGLONG						m_TimeUnit;



	char							m_VideoFile[MAX_PATH];
	bool							m_ForceLoadVobSub;

	DECLARE_CLASS_INFO(CD3DVideoTexture)
public:
	CD3DVideoTexture(void);
	CD3DVideoTexture(CD3DTextureManager * pManager);
	virtual ~CD3DVideoTexture(void);

	virtual void Destory();

	virtual bool Reset();
	virtual bool Restore();

	virtual LPDIRECT3DTEXTURE9  GetD3DTexture();

	bool Create(LPCTSTR FileName);

	virtual FLOAT GetUS();
	virtual FLOAT GetVS();

	VOID GetVideoSize(int& Width,int& Height);	

	LONGLONG GetTimeLength();

	LONGLONG GetMaxPos();

	LONGLONG GetCurPos();

	void SetCurPos(LONGLONG Pos);

	HRESULT GetPlayEvent(long *lEventCode,LONG_PTR *lParam1,LONG_PTR *lParam2,long msTimeout);

	BOOL Run();
	BOOL Stop(BOOL IsFlush=TRUE);
	BOOL Pause();

	int GetPlayStatus(int WaitTime=INFINITE);


	void SetTimeUnit(LONGLONG TimeUnit)
	{
		if(TimeUnit)
			m_TimeUnit=TimeUnit;
	}
	LONGLONG GetTimeUnit()
	{
		return m_TimeUnit;
	}

	void EnableForceLoadVobSub(bool Enable)
	{
		m_ForceLoadVobSub=Enable;
	}

	bool IsForceLoadVobSub()
	{
		return m_ForceLoadVobSub;
	}

	HRESULT FindFilter(IFilterGraph *pGraph,REFIID riid,__RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject);


	HRESULT EnumFilters (IFilterGraph *pGraph);
	HRESULT EnumPin(IBaseFilter *pFilter);
	HRESULT EnumMediaType(IPin *pPin);

};

}