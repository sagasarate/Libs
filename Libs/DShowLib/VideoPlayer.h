/****************************************************************************/
/*                                                                          */
/*      文件名:    VideoPlayer.h                                            */
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

enum VIDEO_PLAYER_PLAY_MODE
{
	VP_PLAY_MODE_WINDOWED=0x0,
	VP_PLAY_MODE_WINDOWLESS=0x1,
	VP_PLAY_MODE_RENDERLESS=0x2,
	VP_PLAY_MODE_VMR_MASK=0xff,
	VP_PLAY_MODE_USER_ALLOCATOR=0x100,
};

class CVideoPlayer :
	public CNameObject
{
protected:
	IFilterGraph*					m_pFilterGraph;
	IGraphBuilder*					m_pGraphBuilder;

	IVMRSurfaceAllocatorNotify9*	m_pVMRSurfaceAllocatorNotify9;
	IVMRWindowlessControl9*			m_pVMRWindowlessControl9;

	IMediaControl*					m_pMediaControl;
	IMediaSeeking*					m_pMediaSeeking;
	IMediaEvent*					m_pMediaEvent;

	DECLARE_CLASS_INFO(CVideoPlayer)
public:
	CVideoPlayer(void);
	virtual ~CVideoPlayer(void);

	virtual void Destory();

	BOOL Create(int Mode,HWND hPlayWnd);

	VOID GetVideoSize(int& Width,int& Height);	

	LONGLONG GetTimeLength();

	LONGLONG GetMaxPos();

	LONGLONG GetCurPos();

	void SetCurPos(LONGLONG Pos);

	HRESULT GetPlayEvent(long *lEventCode,LONG_PTR *lParam1,LONG_PTR *lParam2,long msTimeout);

	BOOL Run();
	BOOL Stop();
	BOOL Pause();

	int GetPlayStatus();
};


}