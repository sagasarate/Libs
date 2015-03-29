#pragma once

namespace D3DLib{

class CBLZWOWSceneLoader :
	public CEasyThread
{
public:
	enum
	{		
		SCENE_VIEW_IN_BORDER=1,
		SCENE_VIEW_OUT_BORDER=2,
	};
	enum SCENE_LOAD_RESULT
	{
		SLR_ERROR,
		SLR_READY,
		SLR_NEED_WAIT,
	};
	enum SCENE_LOAD_STATUS
	{
		SLS_IDLE,
		SLS_CRUCIAL_WORING,
		SLS_WORING,
		SLS_WORK_DONE,
	};
protected:
	enum MAP_AREA_STATUS
	{
		MAS_NOT_EXIST,
		MAS_NOT_LOAD,
		MAS_LOADING,
		MAS_LOADED,
	};
	struct MAP_AREA_INFO
	{
		CD3DWOWADTModel *					pModel;
		UINT								MapID;
		CBLZWOWDatabase::BLZ_DB_MAP_INFO *	pMapInfo;
		int									AreaX;
		int									AreaY;
		volatile int						Status;
		MAP_AREA_INFO()
		{
			pModel=NULL;
			MapID=-1;
			pMapInfo=NULL;
			AreaX=0;
			AreaY=0;
			Status=MAS_NOT_EXIST;
		}
	};
	enum TASK_TYPE
	{
		TASK_NONE,
		TASK_OBJECT_LOAD,
		TASK_OBJECT_PRE_RELEASE,
		TASK_OBJECT_RELEASE,
		TASK_AREA_PRE_LOAD,
		TASK_CRUCIAL_AREA_LOAD,
		TASK_AREA_LOAD,		
		TASK_AREA_PRE_RELEASE,
		TASK_AREA_RELEASE,		
		TASK_PRE_LOAD_FINISH,
		TASK_ALL_FINISH,
	};
	struct TASK_INFO
	{
		int				TaskType;
		CD3DObject *	pModel;
		MAP_AREA_INFO *	pMapArea;
		TASK_INFO()
		{
			TaskType=TASK_NONE;
			pModel=NULL;
			pMapArea=NULL;
		}
	};

	CD3DDevice *						m_pDevice;
	CD3DSceneRender *					m_pRender;
	UINT								m_CurMapID;
	bool								m_CurMapIsBigAlphaMask;
	FLOAT								m_CurPosX;
	FLOAT								m_CurPosY;
	volatile int						m_CurAreaX;
	volatile int						m_CurAreaY;
	CBLZWOWDatabase::BLZ_DB_MAP_INFO *	m_pCurMapInfo;
	CD3DWOWWDLModel *					m_pLowSceneModel;

	volatile UINT						m_LoadSatus;
	CEasyArray<MAP_AREA_INFO>			m_SceneAreas;
	CEasyArray<CD3DLight *>				m_SceneLight;

	CThreadSafeIDStorage<TASK_INFO>		m_TaskQueue;
	CThreadSafeIDStorage<TASK_INFO>		m_FinishTaskQueue;

public:
	CBLZWOWSceneLoader(void);
	~CBLZWOWSceneLoader(void);

	bool Init(CD3DDevice * pDevice,CD3DSceneRender * pRender);
	void Destory();

	int MoveTo(UINT MapID,FLOAT PosX,FLOAT PosY);

	virtual BOOL OnRun();
	int Update(int ProcessLimit);

	int GetStatus();
	void AddSceneLight(CD3DLight * pLight);
	bool DeleteSceneLight(CD3DLight * pLight);
protected:
	bool FirstLoad(UINT MapID,FLOAT PosX,FLOAT PosY);
	bool IsAreaInInView(int AreaX,int AreaY);
	bool IsAreaInOutView(int AreaX,int AreaY);
	void CheckTasks();
	void LoadArea(TASK_INFO& TaskInfo);
	void LoadAreaObjects(TASK_INFO& TaskInfo);
	void PreReleaseArea(MAP_AREA_INFO * pMapArea);
	void PushObjectFinishEvent(CD3DObject * pObject);
	void PushObjectPreReleaseEvent(CD3DObject * pObject);
};


inline int CBLZWOWSceneLoader::GetStatus()
{
	return m_Status;
}

}