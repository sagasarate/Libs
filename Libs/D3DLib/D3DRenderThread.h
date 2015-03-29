#pragma once


namespace D3DLib{



class CD3DRenderThread :
	public CEasyThread
{
protected:
	enum
	{
		FRAME_COUNT_TIME=2000,
		MINI_RENDER_TIME=10,
	};
	struct RENDER_INSTANCE
	{
		volatile bool					Enable;
		CD3DSwapChain *					pSwapChain;
		D3DCOLOR						ClearColor;
		CEasyArray<CD3DBaseRender *>	RenderList;
		volatile int					ObjectCount;
		volatile int					SubMeshCount;
		volatile int					FaceCount;
		volatile int					VertexCount;
		RENDER_INSTANCE()
		{
			Enable=true;
			pSwapChain=NULL;
			ClearColor=D3DCOLOR_XRGB(128,128,128);
			ObjectCount=0;
			SubMeshCount=0;
			FaceCount=0;
			VertexCount=0;
		}
	};
	CD3DDevice *					m_pDevice;
	CEasyArray<RENDER_INSTANCE>		m_RenderInstances;
	

	CEasyTimer						m_FrameCountTimer;
	UINT							m_FrameCount;
	UINT64							m_TotalFrameCount;
	volatile float					m_FrameRate;

	CEasyTimer						m_RenderControlTimer;
	CEasyCriticalSection			m_EditLock;

	DECLARE_CLASS_INFO(CD3DRenderThread)
public:
	CD3DRenderThread(UINT LockSpinCount=0);
	~CD3DRenderThread(void);

	void SetDevice(CD3DDevice * pDevice)
	{
		m_pDevice=pDevice;
	}
	CD3DDevice * GetDevice()
	{
		return m_pDevice;
	}
	bool AddRender(UINT InstanceID,CD3DBaseRender * pRender)
	{
		if(InstanceID<m_RenderInstances.GetCount())
		{
			m_RenderInstances[InstanceID].RenderList.Add(pRender);
			return true;
		}
		return false;
	}
	bool SetClearColor(UINT InstanceID,D3DCOLOR ClearColor)
	{
		if(InstanceID<m_RenderInstances.GetCount())
		{
			m_RenderInstances[InstanceID].ClearColor=ClearColor;
			return true;
		}
		return false;
	}

	UINT AddRenderInstance(CD3DSwapChain * pSwapChain)
	{
		CAutoLock Lock(m_EditLock);

		m_RenderInstances.Resize(m_RenderInstances.GetCount()+1);
		m_RenderInstances[m_RenderInstances.GetCount()-1].pSwapChain=pSwapChain;
		return m_RenderInstances.GetCount()-1;
	}

	bool EnableRenderInstance(UINT InstanceID,bool Enable)
	{
		if(InstanceID<m_RenderInstances.GetCount())
		{
			m_RenderInstances[InstanceID].Enable=Enable;
			return true;
		}
		return false;
	}

	float GetFrameRate()
	{
		return m_FrameRate;
	}
	UINT64 GetFrameTotalCount()
	{
		return m_TotalFrameCount;
	}

	CEasyCriticalSection& GetLock()
	{
		return m_EditLock;
	}

	int GetRenderObjectCount(UINT InstanceID);
	int	GetRenderSubMeshCount(UINT InstanceID);
	int GetRenderFaceCount(UINT InstanceID);
	int GetRenderVertexCount(UINT InstanceID);

	virtual BOOL OnStart();
	virtual BOOL OnRun();
	virtual void OnTerminate();
protected:
	void DoRender(RENDER_INSTANCE * pRenderInstance);
};


inline int CD3DRenderThread::GetRenderObjectCount(UINT InstanceID)
{
	if(InstanceID<m_RenderInstances.GetCount())
	{
		return m_RenderInstances[InstanceID].ObjectCount;
	}
	return 0;
}
inline int CD3DRenderThread::GetRenderSubMeshCount(UINT InstanceID)
{
	if(InstanceID<m_RenderInstances.GetCount())
	{
		return m_RenderInstances[InstanceID].SubMeshCount;
	}
	return 0;
}
inline int CD3DRenderThread::GetRenderFaceCount(UINT InstanceID)
{
	if(InstanceID<m_RenderInstances.GetCount())
	{
		return m_RenderInstances[InstanceID].FaceCount;
	}
	return 0;
}
inline int CD3DRenderThread::GetRenderVertexCount(UINT InstanceID)
{
	if(InstanceID<m_RenderInstances.GetCount())
	{
		return m_RenderInstances[InstanceID].VertexCount;
	}
	return 0;
}

}