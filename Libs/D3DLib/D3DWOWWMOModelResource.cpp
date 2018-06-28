/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWOWWMOModelResource.cpp                               */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

namespace D3DLib{

IMPLEMENT_CLASS_INFO(CD3DWOWWMOModelResource,CD3DObjectResource);

CD3DWOWWMOModelResource::CD3DWOWWMOModelResource(void)
{
}

CD3DWOWWMOModelResource::CD3DWOWWMOModelResource(CD3DObjectResourceManager* pManager)
	:CD3DObjectResource(pManager)
{
	
}

CD3DWOWWMOModelResource::~CD3DWOWWMOModelResource(void)
{	
	Destory();
}


void CD3DWOWWMOModelResource::Destory()
{
	for(UINT i=0;i<m_DoodadInfos.GetCount();i++)
	{
		SAFE_RELEASE(m_DoodadInfos[i].pDoodadModel);
	}
	m_DoodadInfos.Clear();
	m_DoodadSets.Clear();
	for(UINT i=0;i<m_Groups.GetCount();i++)
	{
		for(UINT j=0;j<m_Groups[i].GroupSubMeshList.GetCount();j++)
		{
			SAFE_RELEASE(m_Groups[i].GroupSubMeshList[j]);			
		}
		for(UINT j=0;j<m_Groups[i].RenderBatchs.GetCount();j++)
		{
			SAFE_RELEASE(m_Groups[i].RenderBatchs[j].pTexture1);
			SAFE_RELEASE(m_Groups[i].RenderBatchs[j].pTexture2);
			SAFE_RELEASE(m_Groups[i].RenderBatchs[j].pFX);
		}
	}
	m_Groups.Clear();
	CD3DObjectResource::Destory();
}

bool CD3DWOWWMOModelResource::Reset()
{
	return CD3DObjectResource::Reset();
}

bool CD3DWOWWMOModelResource::Restore()
{
	return CD3DObjectResource::Restore();
}

bool CD3DWOWWMOModelResource::LoadFromFile(LPCTSTR ModelFileName)
{
	IFileAccessor * pFile;

	pFile=CD3DWOWWMOModel::CreateFileAccessor();
	if(pFile==NULL)
		return false;
	if(!pFile->Open(ModelFileName,IFileAccessor::modeRead))
	{
		PrintD3DLog(_T("文件%s打开失败"),ModelFileName);
		pFile->Release();
		return false;	
	}

	CBLZChunkFile WMOChunk;

	if(!WMOChunk.Load(pFile))
	{
		PrintD3DLog(_T("文件%s格式错误"),ModelFileName);
		pFile->Release();
		return false;
	}
	pFile->Release();	

	Destory();

	BLZ_CHUNK_VERSION * pVersion=(BLZ_CHUNK_VERSION *)WMOChunk.GetFirstChunk(CHUNK_ID_VERSION);
	if(pVersion==NULL)
		return false;

	BLZ_CHUNK_MOHD * pHeader=(BLZ_CHUNK_MOHD *)WMOChunk.GetFirstChunk(CHUNK_ID_WMO_MOHD);
	if(pHeader==NULL)
		return false;

	BLZ_CHUNK_MOTX * pTextureNames=(BLZ_CHUNK_MOTX *)WMOChunk.GetFirstChunk(CHUNK_ID_WMO_MOTX);
	if(pHeader->MaterialCount&&pTextureNames==NULL)
		return false;

	BLZ_CHUNK_MOMT * pMaterials=(BLZ_CHUNK_MOMT *)WMOChunk.GetFirstChunk(CHUNK_ID_WMO_MOMT);
	if(pHeader->MaterialCount&&pMaterials==NULL)
		return false;

	BLZ_CHUNK_MOGN * pGroupNames=(BLZ_CHUNK_MOGN *)WMOChunk.GetFirstChunk(CHUNK_ID_WMO_MOGN);
	if(pHeader->GroupCount&&pGroupNames==NULL)
		return false;

	BLZ_CHUNK_MOGI * pGroups=(BLZ_CHUNK_MOGI *)WMOChunk.GetFirstChunk(CHUNK_ID_WMO_MOGI);
	if(pHeader->GroupCount&&pGroups==NULL)
		return false;

	BLZ_CHUNK_MOPV * pPortalVertices=(BLZ_CHUNK_MOPV *)WMOChunk.GetFirstChunk(CHUNK_ID_WMO_MOPV);
	if(pHeader->PortalCount&&pPortalVertices==NULL)
		return false;

	BLZ_CHUNK_MOPT * pPortalInfos=(BLZ_CHUNK_MOPT *)WMOChunk.GetFirstChunk(CHUNK_ID_WMO_MOPT);
	if(pHeader->PortalCount&&pPortalInfos==NULL)
		return false;

	BLZ_CHUNK_MOPR * pPortalRelationship=(BLZ_CHUNK_MOPR *)WMOChunk.GetFirstChunk(CHUNK_ID_WMO_MOPR);
	if(pHeader->PortalCount&&pPortalRelationship==NULL)
		return false;

	BLZ_CHUNK_MOVV * pVisibleBlockVertices=(BLZ_CHUNK_MOVV *)WMOChunk.GetFirstChunk(CHUNK_ID_WMO_MOVV);
	BLZ_CHUNK_MOVB * pVisibleBlockList=(BLZ_CHUNK_MOVB *)WMOChunk.GetFirstChunk(CHUNK_ID_WMO_MOVB);

	BLZ_CHUNK_MOLT * pLights=(BLZ_CHUNK_MOLT *)WMOChunk.GetFirstChunk(CHUNK_ID_WMO_MOLT);
	if(pHeader->LightCount&&pLights==NULL)
		return false;

	BLZ_CHUNK_MODS * pDoodadSets=(BLZ_CHUNK_MODS *)WMOChunk.GetFirstChunk(CHUNK_ID_WMO_MODS);
	if(pHeader->DoodadSetCount&&pDoodadSets==NULL)
		return false;

	BLZ_CHUNK_MODN * pDoodadFileNames=(BLZ_CHUNK_MODN *)WMOChunk.GetFirstChunk(CHUNK_ID_WMO_MODN);
	if(pHeader->DoodadCount&&pDoodadFileNames==NULL)
		return false;

	BLZ_CHUNK_MODD * pDoodads=(BLZ_CHUNK_MODD *)WMOChunk.GetFirstChunk(CHUNK_ID_WMO_MODD);
	if(pHeader->DoodadCount&&pDoodads==NULL)
		return false;
	
	BLZ_CHUNK_MFOG * pFog=(BLZ_CHUNK_MFOG *)WMOChunk.GetFirstChunk(CHUNK_ID_WMO_MFOG);
	if(pFog==NULL)
		return false;

	m_BoundingBox.m_Min=BLZTranslationToD3D(pHeader->BoundingBox[0]);
	m_BoundingBox.m_Max=BLZTranslationToD3D(pHeader->BoundingBox[1]);
	m_BoundingBox.Rebuild();

	BLZ_CHUNK_MCVP * pConvexVolumePlanes=(BLZ_CHUNK_MCVP *)WMOChunk.GetFirstChunk(CHUNK_ID_WMO_MCVP);

	if(!LoadDoodads(pHeader->DoodadCount,pHeader->DoodadSetCount,pDoodadSets,pDoodadFileNames,pDoodads))
		return false;

	if(!LoadGroups(ModelFileName,pHeader->GroupCount,pGroups,pGroupNames,pMaterials,pHeader->MaterialCount,pTextureNames))
		return false;

	UINT GroupPortalCount=pPortalRelationship->ChunkSize/sizeof(WMOPortalRelationship);
	UINT PortalCount=pPortalInfos->ChunkSize/sizeof(WMOPortalInfo);
	if(pHeader->PortalCount!=PortalCount)
	{
		PrintD3DLog(_T("Portal数量有异常"));
	}

	for(UINT i=0;i<GroupPortalCount;i++)
	{
		PORTAL_INFO PortalInfo;
		PortalInfo.GroupIndex=pPortalRelationship->PortalRelationship[i].GroupIndex;
		PortalInfo.Filler=pPortalRelationship->PortalRelationship[i].Filler;
		UINT Index=pPortalRelationship->PortalRelationship[i].PortalIndex;
		if(Index<PortalCount)
		{
			PortalInfo.Normal=BLZTranslationToD3D(pPortalInfos->PortalInfo[Index].Normal)*pPortalRelationship->PortalRelationship[i].Side;
			PortalInfo.Factor=pPortalInfos->PortalInfo[Index].Unknown;
			for(UINT j=0;j<pPortalInfos->PortalInfo[Index].VertexCount;j++)
			{
				UINT VertexIndex=j+pPortalInfos->PortalInfo[Index].VertexIndex;
				PortalInfo.Vertices.Add(BLZTranslationToD3D(pPortalVertices->PortalVertices[VertexIndex]));
			}
			if(PortalInfo.Vertices.GetCount()>=4)
			{
				PortalInfo.Center=(PortalInfo.Vertices[0]+PortalInfo.Vertices[2])/2;
			}
			GROUP_INFO * pGroup=GetGroupInfo(PortalInfo.GroupIndex);
			if(pGroup)
			{
				pGroup->PortalList.Add(PortalInfo);
			}
			else
			{
				PrintD3DLog(_T("Portal(%d)找不到对应Group(%d)"),i,PortalInfo.GroupIndex);
			}
		}
		else
		{
			PrintD3DLog(_T("Portal(%d)找不到"),Index);
		}
	}

	BuildSubMeshs();

	return true;
}


UINT CD3DWOWWMOModelResource::GetDoodadSetCount()
{
	return m_DoodadSets.GetCount();
}
CD3DWOWWMOModelResource::DOODAD_SET_INFO * CD3DWOWWMOModelResource::GetDoodadSetInfo(UINT Index)
{
	return m_DoodadSets.GetObject(Index);
}

UINT CD3DWOWWMOModelResource::GetDoodadCount()
{
	return m_DoodadInfos.GetCount();
}
CD3DWOWWMOModelResource::DOODAD_INFO * CD3DWOWWMOModelResource::GetDoodadInfo(UINT Index)
{
	return m_DoodadInfos.GetObject(Index);
}

UINT CD3DWOWWMOModelResource::GetGroupCount()
{
	return m_Groups.GetCount();
}
CD3DWOWWMOModelResource::GROUP_INFO * CD3DWOWWMOModelResource::GetGroupInfo(UINT Index)
{
	return m_Groups.GetObject(Index);
}

void CD3DWOWWMOModelResource::PickResource(CUSOResourceManager * pResourceManager,UINT Param)
{
	CD3DObjectResource::PickResource(pResourceManager,Param);	
	for(UINT i=0;i<m_DoodadInfos.GetCount();i++)
	{
		if(m_DoodadInfos[i].pDoodadModel)
		{
			m_DoodadInfos[i].pDoodadModel->PickResource(pResourceManager,Param);
			pResourceManager->AddResource(m_DoodadInfos[i].pDoodadModel);
		}
	}
	for(UINT i=0;i<m_Groups.GetCount();i++)
	{
		for(UINT j=0;j<m_Groups[i].GroupSubMeshList.GetCount();j++)
		{
			m_Groups[i].GroupSubMeshList[j]->PickResource(pResourceManager,Param);
		}
	}
}

bool CD3DWOWWMOModelResource::ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CD3DObjectResource::ToSmartStruct(Packet,pResourceManager,Param))
		return false;

	{
		UINT BufferSize;
		void * pBuffer=Packet.PrepareMember(BufferSize);
		CSmartStruct DoodadInfoList(pBuffer,BufferSize,true);
		for(UINT i=0;i<m_DoodadInfos.GetCount();i++)
		{
			pBuffer=DoodadInfoList.PrepareMember(BufferSize);
			CSmartStruct DoodadInfo(pBuffer,BufferSize,true);
			CHECK_SMART_STRUCT_ADD_AND_RETURN(DoodadInfo.AddMember(SST_DI_MODEL_RESOURCE,m_DoodadInfos[i].pDoodadModel->GetName()));
			CHECK_SMART_STRUCT_ADD_AND_RETURN(DoodadInfo.AddMember(SST_DI_TRANSLATION,
				(LPCTSTR)(&m_DoodadInfos[i].Translation),sizeof(m_DoodadInfos[i].Translation)));
			CHECK_SMART_STRUCT_ADD_AND_RETURN(DoodadInfo.AddMember(SST_DI_ROTATION,
				(LPCTSTR)(&m_DoodadInfos[i].Rotation),sizeof(m_DoodadInfos[i].Rotation)));
			CHECK_SMART_STRUCT_ADD_AND_RETURN(DoodadInfo.AddMember(SST_DI_SCALING,
				(LPCTSTR)(&m_DoodadInfos[i].Scaling),sizeof(m_DoodadInfos[i].Scaling)));
			CHECK_SMART_STRUCT_ADD_AND_RETURN(DoodadInfo.AddMember(SST_DI_COLOR,(UINT)m_DoodadInfos[i].Color));
			CHECK_SMART_STRUCT_ADD_AND_RETURN(DoodadInfo.AddMember(SST_DI_GROUP_INDEX,m_DoodadInfos[i].GroupIndex));
			if(!DoodadInfoList.FinishMember(SST_DIL_DOODAD_INFO,DoodadInfo.GetDataLen()))
				return false;
		}
		if(!Packet.FinishMember(SST_D3DWWMR_DOODAD_INFO_LIST,DoodadInfoList.GetDataLen()))
			return false;
	}

	{
		UINT BufferSize;
		void * pBuffer=Packet.PrepareMember(BufferSize);
		CSmartStruct DoodadSetList(pBuffer,BufferSize,true);
		for(UINT i=0;i<m_DoodadSets.GetCount();i++)
		{
			pBuffer=DoodadSetList.PrepareMember(BufferSize);
			CSmartStruct DoodadSet(pBuffer,BufferSize,true);			
			CHECK_SMART_STRUCT_ADD_AND_RETURN(DoodadSet.AddMember(SST_DS_NAME,(LPCTSTR)m_DoodadSets[i].Name));			
			CHECK_SMART_STRUCT_ADD_AND_RETURN(DoodadSet.AddMember(SST_DS_START_DOODAD,m_DoodadSets[i].StartDoodad));
			CHECK_SMART_STRUCT_ADD_AND_RETURN(DoodadSet.AddMember(SST_DS_DOODAD_COUNT,m_DoodadSets[i].DoodadCount));
			if(!DoodadSetList.FinishMember(SST_DSL_DOODAD_SET,DoodadSet.GetDataLen()))
				return false;
		}
		if(!Packet.FinishMember(SST_D3DWWMR_DOODAD_SET_LIST,DoodadSetList.GetDataLen()))
			return false;
	}

	{
		UINT BufferSize;
		void * pBuffer=Packet.PrepareMember(BufferSize);
		CSmartStruct GroupList(pBuffer,BufferSize,true);
		for(UINT i=0;i<m_Groups.GetCount();i++)
		{
			pBuffer=GroupList.PrepareMember(BufferSize);
			CSmartStruct GroupInfo(pBuffer,BufferSize,true);			
			CHECK_SMART_STRUCT_ADD_AND_RETURN(GroupInfo.AddMember(SST_GI_INDEX,m_Groups[i].Index));
			CHECK_SMART_STRUCT_ADD_AND_RETURN(GroupInfo.AddMember(SST_GI_NAME,(LPCTSTR)m_Groups[i].Name));
			CHECK_SMART_STRUCT_ADD_AND_RETURN(GroupInfo.AddMember(SST_GI_FLAG,m_Groups[i].Flags));
			CHECK_SMART_STRUCT_ADD_AND_RETURN(GroupInfo.AddMember(SST_GI_BOUNDING_BOX,
				(LPCTSTR)(&m_Groups[i].BoundingBox),sizeof(m_Groups[i].BoundingBox)));
			
			{
				pBuffer=GroupInfo.PrepareMember(BufferSize);
				CSmartStruct PortalList(pBuffer,BufferSize,true);

				for(UINT j=0;j<m_Groups[i].PortalList.GetCount();j++)
				{				
					pBuffer=PortalList.PrepareMember(BufferSize);
					CSmartStruct PortalInfo(pBuffer,BufferSize,true);

					CHECK_SMART_STRUCT_ADD_AND_RETURN(PortalInfo.AddMember(SST_PI_GROUP_INDEX,m_Groups[i].PortalList[j].GroupIndex));
					CHECK_SMART_STRUCT_ADD_AND_RETURN(PortalInfo.AddMember(SST_PI_FILLER,m_Groups[i].PortalList[j].Filler));
					CHECK_SMART_STRUCT_ADD_AND_RETURN(PortalInfo.AddMember(SST_PI_NORMAL,
						(LPCTSTR)(&m_Groups[i].PortalList[j].Normal),sizeof(m_Groups[i].PortalList[j].Normal)));
					CHECK_SMART_STRUCT_ADD_AND_RETURN(PortalInfo.AddMember(SST_PI_CENTER,
						(LPCTSTR)(&m_Groups[i].PortalList[j].Center),sizeof(m_Groups[i].PortalList[j].Center)));
					CHECK_SMART_STRUCT_ADD_AND_RETURN(PortalInfo.AddMember(SST_PI_FACTOR,m_Groups[i].PortalList[j].Factor));

					CHECK_SMART_STRUCT_ADD_AND_RETURN(PortalInfo.AddMember(SST_PI_VERTICES,
						(LPCTSTR)(m_Groups[i].PortalList[j].Vertices.GetBuffer()),
						sizeof(CD3DVector3)*m_Groups[i].PortalList[j].Vertices.GetCount()));


					if(!PortalList.FinishMember(SST_PL_PORTAL_INFO,PortalInfo.GetDataLen()))
						return false;
				}
				if(!GroupInfo.FinishMember(SST_GI_PORTAL_LIST,PortalList.GetDataLen()))
					return false;
			}
			CHECK_SMART_STRUCT_ADD_AND_RETURN(GroupInfo.AddMember(SST_GI_FACE_INDEX_LIST,
				(LPCTSTR)m_Groups[i].IndexList.GetBuffer(),sizeof(WORD)*m_Groups[i].IndexList.GetCount()));
			CHECK_SMART_STRUCT_ADD_AND_RETURN(GroupInfo.AddMember(SST_GI_FACE_VERTEX_LIST,
				(LPCTSTR)m_Groups[i].VertexList.GetBuffer(),sizeof(MODEL_VERTEXT)*m_Groups[i].VertexList.GetCount()));
			{
				pBuffer=GroupInfo.PrepareMember(BufferSize);
				CSmartStruct BSPTree(pBuffer,BufferSize,true);

				for(UINT j=0;j<m_Groups[i].BSPTree.GetCount();j++)
				{
					pBuffer=BSPTree.PrepareMember(BufferSize);
					CSmartStruct BSPNodeInfo(pBuffer,BufferSize,true);

					CHECK_SMART_STRUCT_ADD_AND_RETURN(BSPNodeInfo.AddMember(SST_BN_PLANE_TYPE,m_Groups[i].BSPTree[j].PlaneType));
					CHECK_SMART_STRUCT_ADD_AND_RETURN(BSPNodeInfo.AddMember(SST_BN_RIGHT_CHILD_INDEX,m_Groups[i].BSPTree[j].RightChildIndex));
					CHECK_SMART_STRUCT_ADD_AND_RETURN(BSPNodeInfo.AddMember(SST_BN_LEFT_CHILD_INDEX,m_Groups[i].BSPTree[j].LeftChildIndex));
					CHECK_SMART_STRUCT_ADD_AND_RETURN(BSPNodeInfo.AddMember(SST_BN_FACE_COUNT,m_Groups[i].BSPTree[j].FaceCount));
					CHECK_SMART_STRUCT_ADD_AND_RETURN(BSPNodeInfo.AddMember(SST_BN_FIRST_FACE,m_Groups[i].BSPTree[j].FirstFace));
					CHECK_SMART_STRUCT_ADD_AND_RETURN(BSPNodeInfo.AddMember(SST_BN_DISTANCE,m_Groups[i].BSPTree[j].Distance));

					if(!BSPTree.FinishMember(SST_BT_BSP_NODE_INFO,BSPNodeInfo.GetDataLen()))
						return false;
				}
				if(!GroupInfo.FinishMember(SST_GI_BSP_TREE,BSPTree.GetDataLen()))
					return false;
			}
			CHECK_SMART_STRUCT_ADD_AND_RETURN(GroupInfo.AddMember(SST_GI_BSP_FACE_LIST,
				(LPCTSTR)m_Groups[i].BSPFaceList.GetBuffer(),sizeof(WORD)*m_Groups[i].BSPFaceList.GetCount()));

			{
				pBuffer=GroupInfo.PrepareMember(BufferSize);
				CSmartStruct RenderBatchList(pBuffer,BufferSize,true);

				for(UINT j=0;j<m_Groups[i].RenderBatchs.GetCount();j++)
				{
					pBuffer=RenderBatchList.PrepareMember(BufferSize);
					CSmartStruct RenderBatchInfo(pBuffer,BufferSize,true);

					CHECK_SMART_STRUCT_ADD_AND_RETURN(RenderBatchInfo.AddMember(SST_RBI_START_INDEX,m_Groups[i].RenderBatchs[j].StartIndex));
					CHECK_SMART_STRUCT_ADD_AND_RETURN(RenderBatchInfo.AddMember(SST_RBI_INDEX_COUNT,m_Groups[i].RenderBatchs[j].IndexCount));
					CHECK_SMART_STRUCT_ADD_AND_RETURN(RenderBatchInfo.AddMember(SST_RBI_START_VERTEX,m_Groups[i].RenderBatchs[j].StartVertex));
					CHECK_SMART_STRUCT_ADD_AND_RETURN(RenderBatchInfo.AddMember(SST_RBI_VERTEX_COUNT,m_Groups[i].RenderBatchs[j].VertexCount));

					if(m_Groups[i].RenderBatchs[j].pTexture1)
						CHECK_SMART_STRUCT_ADD_AND_RETURN(RenderBatchInfo.AddMember(SST_RBI_TEXTURE1,m_Groups[i].RenderBatchs[j].pTexture1->GetName()));

					if(m_Groups[i].RenderBatchs[j].pTexture2)
						CHECK_SMART_STRUCT_ADD_AND_RETURN(RenderBatchInfo.AddMember(SST_RBI_TEXTURE2,m_Groups[i].RenderBatchs[j].pTexture2->GetName()));

					if(m_Groups[i].RenderBatchs[j].pFX)
						CHECK_SMART_STRUCT_ADD_AND_RETURN(RenderBatchInfo.AddMember(SST_RBI_FX,m_Groups[i].RenderBatchs[j].pFX->GetName()));

					if(!RenderBatchList.FinishMember(SST_BRL_RENDER_BATCH_INFO,RenderBatchInfo.GetDataLen()))
						return false;
				}
				if(!GroupInfo.FinishMember(SST_GI_RENER_BATCH_LIST,RenderBatchList.GetDataLen()))
					return false;
			}

			CHECK_SMART_STRUCT_ADD_AND_RETURN(GroupInfo.AddMember(SST_GI_FACE_FLAGS,
				(LPCTSTR)m_Groups[i].FaceFlags.GetBuffer(),sizeof(BYTE)*m_Groups[i].FaceFlags.GetCount()));

			if(!GroupList.FinishMember(SST_GL_GROUP_INFO,GroupInfo.GetDataLen()))
				return false;
		}
		if(!Packet.FinishMember(SST_D3DWWMR_GROUP_LIST,GroupList.GetDataLen()))
			return false;
	}

	return true;
}
bool CD3DWOWWMOModelResource::FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	Destory();

	if(!CD3DObjectResource::FromSmartStruct(Packet,pResourceManager,Param))
		return false;


	UINT DoodadInfoCount=0;
	UINT DoodadSetCount=0;
	UINT GroupCount=0;

	void * Pos=Packet.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartStruct SubPacket=Packet.GetNextMember(Pos,MemberID);
		switch(MemberID)
		{	
		case SST_D3DWWMR_DOODAD_INFO_LIST:
			DoodadInfoCount=SubPacket.GetMemberCount();
			break;
		case SST_D3DWWMR_DOODAD_SET_LIST:
			DoodadSetCount=SubPacket.GetMemberCount();
			break;
		case SST_D3DWWMR_GROUP_LIST:
			GroupCount=SubPacket.GetMemberCount();
			break;
		}
	}

	m_DoodadInfos.Resize(DoodadInfoCount);
	m_DoodadSets.Resize(DoodadSetCount);
	m_Groups.Resize(GroupCount);

	DoodadInfoCount=0;
	DoodadSetCount=0;
	GroupCount=0;


	Pos=Packet.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartStruct SubPacket=Packet.GetNextMember(Pos,MemberID);
		switch(MemberID)
		{		
		case SST_D3DWWMR_GROUP_LIST:
			{
				void * SubPos=SubPacket.GetFirstMemberPosition();
				while(SubPos)
				{
					WORD SubMemberID;
					CSmartStruct GroupInfo=Packet.GetNextMember(SubPos,SubMemberID);
					if(SubMemberID==SST_GL_GROUP_INFO)
					{
						UINT PortalCount=0;
						UINT BSPNodeCount=0;
						UINT RenderBatchCount=0;
						void * GroupPos=GroupInfo.GetFirstMemberPosition();
						while(GroupPos)
						{
							WORD GroupMemberID;
							CSmartStruct SubSubPacket=GroupInfo.GetNextMember(GroupPos,GroupMemberID);
							switch(GroupMemberID)
							{								
							case SST_GI_PORTAL_LIST:
								{
									void * PortalPos=SubSubPacket.GetFirstMemberPosition();
									while(PortalPos)
									{
										WORD ProtalInfoMemberID;
										CSmartStruct ProtalInfo=SubSubPacket.GetNextMember(PortalPos,ProtalInfoMemberID);
										if(ProtalInfoMemberID==SST_PL_PORTAL_INFO)
										{
												PortalCount++;
										}
									}
								}
								break;
							case SST_GI_BSP_TREE:
								{
									void * BSPNodeInfoPos=SubSubPacket.GetFirstMemberPosition();
									while(BSPNodeInfoPos)
									{
										WORD BSPNodeInfoMemberID;
										CSmartStruct BSPNodeInfo=SubSubPacket.GetNextMember(BSPNodeInfoPos,BSPNodeInfoMemberID);
										if(BSPNodeInfoMemberID==SST_BT_BSP_NODE_INFO)
										{
											BSPNodeCount++;
										}
									}
								}
								break;
							case SST_GI_RENER_BATCH_LIST:
								{
									void * RederBatchInfoPos=SubSubPacket.GetFirstMemberPosition();
									while(RederBatchInfoPos)
									{
										WORD RederBatchInfoMemberID;
										CSmartStruct RederBatchInfo=SubSubPacket.GetNextMember(RederBatchInfoPos,RederBatchInfoMemberID);
										if(RederBatchInfoMemberID==SST_BRL_RENDER_BATCH_INFO)
										{
											RenderBatchCount++;
										}
									}
								}
								break;
							}
						}
						m_Groups[GroupCount].PortalList.Resize(PortalCount);
						m_Groups[GroupCount].BSPTree.Resize(BSPNodeCount);
						m_Groups[GroupCount].RenderBatchs.Resize(RenderBatchCount);
						GroupCount++;
					}
				}
			}
			break;
		}
	}

	DoodadInfoCount=0;
	DoodadSetCount=0;
	GroupCount=0;

	Pos=Packet.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartStruct SubPacket=Packet.GetNextMember(Pos,MemberID);
		switch(MemberID)
		{	
		case SST_D3DWWMR_DOODAD_INFO_LIST:
			{
				void * SubPos=SubPacket.GetFirstMemberPosition();
				while(SubPos)
				{
					WORD SubMemberID;
					CSmartStruct DoodadInfo=Packet.GetNextMember(SubPos,SubMemberID);
					if(SubMemberID==SST_DIL_DOODAD_INFO)
					{
						LPCTSTR szResourceName=DoodadInfo.GetMember(SST_DI_MODEL_RESOURCE);
						m_DoodadInfos[DoodadInfoCount].pDoodadModel=(CD3DWOWM2ModelResource *)pResourceManager->FindResource(szResourceName,GET_CLASS_INFO(CD3DWOWM2ModelResource));
						if(m_DoodadInfos[DoodadInfoCount].pDoodadModel)
							m_DoodadInfos[DoodadInfoCount].pDoodadModel->AddUseRef();
						memcpy(&(m_DoodadInfos[DoodadInfoCount].Translation),(LPCSTR)DoodadInfo.GetMember(SST_DI_TRANSLATION),
							sizeof(m_DoodadInfos[DoodadInfoCount].Translation));
						memcpy(&(m_DoodadInfos[DoodadInfoCount].Rotation),(LPCSTR)DoodadInfo.GetMember(SST_DI_ROTATION),
							sizeof(m_DoodadInfos[DoodadInfoCount].Rotation));
						memcpy(&(m_DoodadInfos[DoodadInfoCount].Scaling),(LPCSTR)DoodadInfo.GetMember(SST_DI_SCALING),
							sizeof(m_DoodadInfos[DoodadInfoCount].Scaling));
						m_DoodadInfos[DoodadInfoCount].Color=(UINT)DoodadInfo.GetMember(SST_DI_COLOR);
						m_DoodadInfos[DoodadInfoCount].GroupIndex=DoodadInfo.GetMember(SST_DI_GROUP_INDEX);
						DoodadInfoCount++;
					}
				}
			}
			break;
		case SST_D3DWWMR_DOODAD_SET_LIST:
			{
				void * SubPos=SubPacket.GetFirstMemberPosition();
				while(SubPos)
				{
					WORD SubMemberID;
					CSmartStruct DoodadSet=Packet.GetNextMember(SubPos,SubMemberID);
					if(SubMemberID==SST_DSL_DOODAD_SET)
					{
						m_DoodadSets[DoodadSetCount].Name=(LPCTSTR)DoodadSet.GetMember(SST_DS_NAME);
						m_DoodadSets[DoodadSetCount].StartDoodad=DoodadSet.GetMember(SST_DS_START_DOODAD);
						m_DoodadSets[DoodadSetCount].DoodadCount=DoodadSet.GetMember(SST_DS_DOODAD_COUNT);
						DoodadSetCount++;
					}
				}
			}
			break;
		case SST_D3DWWMR_GROUP_LIST:
			{
				void * SubPos=SubPacket.GetFirstMemberPosition();
				while(SubPos)
				{
					WORD SubMemberID;
					CSmartStruct GroupInfo=Packet.GetNextMember(SubPos,SubMemberID);
					if(SubMemberID==SST_GL_GROUP_INFO)
					{
						UINT PortalCount=0;
						UINT BSPNodeCount=0;
						UINT RenderBatchCount=0;
						void * GroupPos=GroupInfo.GetFirstMemberPosition();
						while(GroupPos)
						{
							WORD GroupMemberID;
							CSmartValue Value=GroupInfo.GetNextMember(GroupPos,GroupMemberID);
							switch(GroupMemberID)
							{
							case SST_GI_INDEX:
								m_Groups[GroupCount].Index=Value;
								break;
							case SST_GI_NAME:
								m_Groups[GroupCount].Name=(LPCTSTR)Value;
								break;
							case SST_GI_FLAG:
								m_Groups[GroupCount].Flags=Value;
								break;
							case SST_GI_BOUNDING_BOX:
								memcpy(&(m_Groups[GroupCount].BoundingBox),(LPCSTR)Value,sizeof(m_Groups[GroupCount].BoundingBox));
								break;
							case SST_GI_PORTAL_LIST:
								{
									CSmartStruct SubSubPacket=Value;
									void * PortalInfoPos=SubSubPacket.GetFirstMemberPosition();
									while(PortalInfoPos)
									{
										WORD PortalInfoMemberID;
										CSmartStruct PortalInfo=SubSubPacket.GetNextMember(PortalInfoPos,PortalInfoMemberID);
										if(PortalInfoMemberID==SST_PL_PORTAL_INFO)
										{											
											void * PortalPos=PortalInfo.GetFirstMemberPosition();
											while(PortalPos)
											{
												WORD PortalMember;
												CSmartValue SubValue=PortalInfo.GetNextMember(PortalPos,PortalMember);
												switch(PortalMember)
												{
												case SST_PI_GROUP_INDEX:
													m_Groups[GroupCount].PortalList[PortalCount].GroupIndex=SubValue;
													break;
												case SST_PI_FILLER:
													m_Groups[GroupCount].PortalList[PortalCount].Filler=SubValue;
													break;
												case SST_PI_NORMAL:
													memcpy(&m_Groups[GroupCount].PortalList[PortalCount].Normal,
														(LPCTSTR)SubValue,sizeof(m_Groups[GroupCount].PortalList[PortalCount].Normal));
													break;
												case SST_PI_CENTER:
													memcpy(&m_Groups[GroupCount].PortalList[PortalCount].Center,
														(LPCTSTR)SubValue,sizeof(m_Groups[GroupCount].PortalList[PortalCount].Center));
													break;
												case SST_PI_FACTOR:
													m_Groups[GroupCount].PortalList[PortalCount].Factor=SubValue;
													break;
												case SST_PI_VERTICES:
													{
														UINT Count=SubValue.GetLength()/sizeof(CD3DVector3);
														m_Groups[GroupCount].PortalList[PortalCount].Vertices.Resize(Count);
														memcpy(m_Groups[GroupCount].PortalList[PortalCount].Vertices.GetBuffer(),
															(LPCTSTR)SubValue,sizeof(CD3DVector3)*Count);
													}
													break;
												}
											}
											PortalCount++;
										}
									}
								}
								break;
							case SST_GI_FACE_INDEX_LIST:
								{
									UINT IndexCount=Value.GetLength()/sizeof(WORD);
									m_Groups[GroupCount].IndexList.Resize(IndexCount);
									memcpy(m_Groups[GroupCount].IndexList.GetBuffer(),(LPCSTR)Value,sizeof(WORD)*IndexCount);
								}
								break;
							case SST_GI_FACE_VERTEX_LIST:
								{
									UINT VertexCount=Value.GetLength()/sizeof(MODEL_VERTEXT);
									m_Groups[GroupCount].VertexList.Resize(VertexCount);
									memcpy(m_Groups[GroupCount].VertexList.GetBuffer(),(LPCSTR)Value,sizeof(MODEL_VERTEXT)*VertexCount);
								}
								break;
							case SST_GI_BSP_TREE:
								{
									CSmartStruct SubSubPacket=Value;
									void * BSPNodeInfoPos=SubSubPacket.GetFirstMemberPosition();
									while(BSPNodeInfoPos)
									{
										WORD BSPNodeInfoMemberID;
										CSmartStruct BSPNodeInfo=SubSubPacket.GetNextMember(BSPNodeInfoPos,BSPNodeInfoMemberID);
										if(BSPNodeInfoMemberID==SST_BT_BSP_NODE_INFO)
										{
											m_Groups[GroupCount].BSPTree[BSPNodeCount].PlaneType=BSPNodeInfo.GetMember(SST_BN_PLANE_TYPE);
											m_Groups[GroupCount].BSPTree[BSPNodeCount].RightChildIndex=BSPNodeInfo.GetMember(SST_BN_RIGHT_CHILD_INDEX);
											m_Groups[GroupCount].BSPTree[BSPNodeCount].LeftChildIndex=BSPNodeInfo.GetMember(SST_BN_LEFT_CHILD_INDEX);
											m_Groups[GroupCount].BSPTree[BSPNodeCount].FaceCount=BSPNodeInfo.GetMember(SST_BN_FACE_COUNT);
											m_Groups[GroupCount].BSPTree[BSPNodeCount].FirstFace=BSPNodeInfo.GetMember(SST_BN_FIRST_FACE);
											m_Groups[GroupCount].BSPTree[BSPNodeCount].Distance=BSPNodeInfo.GetMember(SST_BN_DISTANCE);
											m_Groups[GroupCount].BSPTree[BSPNodeCount].pFaceBoard=NULL;
											BSPNodeCount++;
										}
									}
								}
								break;
							case SST_GI_BSP_FACE_LIST:
								{
									UINT FaceCount=Value.GetLength()/sizeof(WORD);
									m_Groups[GroupCount].BSPFaceList.Resize(FaceCount);
									memcpy(m_Groups[GroupCount].BSPFaceList.GetBuffer(),(LPCSTR)Value,sizeof(WORD)*FaceCount);
								}
								break;
							case SST_GI_RENER_BATCH_LIST:
								{
									CSmartStruct SubSubPacket=Value;
									void * RederBatchInfoPos=SubSubPacket.GetFirstMemberPosition();
									while(RederBatchInfoPos)
									{
										WORD RederBatchInfoMemberID;
										CSmartStruct RederBatchInfo=SubSubPacket.GetNextMember(RederBatchInfoPos,RederBatchInfoMemberID);
										if(RederBatchInfoMemberID==SST_BRL_RENDER_BATCH_INFO)
										{
											m_Groups[GroupCount].RenderBatchs[RenderBatchCount].StartIndex=RederBatchInfo.GetMember(SST_RBI_START_INDEX);
											m_Groups[GroupCount].RenderBatchs[RenderBatchCount].IndexCount=RederBatchInfo.GetMember(SST_RBI_INDEX_COUNT);
											m_Groups[GroupCount].RenderBatchs[RenderBatchCount].StartVertex=RederBatchInfo.GetMember(SST_RBI_START_VERTEX);
											m_Groups[GroupCount].RenderBatchs[RenderBatchCount].VertexCount=RederBatchInfo.GetMember(SST_RBI_VERTEX_COUNT);
											LPCTSTR szResourceName=RederBatchInfo.GetMember(SST_RBI_TEXTURE1);
											m_Groups[GroupCount].RenderBatchs[RenderBatchCount].pTexture1=(CD3DTexture *)pResourceManager->FindResource(szResourceName,GET_CLASS_INFO(CD3DTexture));
											if(m_Groups[GroupCount].RenderBatchs[RenderBatchCount].pTexture1)
												m_Groups[GroupCount].RenderBatchs[RenderBatchCount].pTexture1->AddUseRef();
											szResourceName=RederBatchInfo.GetMember(SST_RBI_TEXTURE2);
											m_Groups[GroupCount].RenderBatchs[RenderBatchCount].pTexture2=(CD3DTexture *)pResourceManager->FindResource(szResourceName,GET_CLASS_INFO(CD3DTexture));
											if(m_Groups[GroupCount].RenderBatchs[RenderBatchCount].pTexture2)
												m_Groups[GroupCount].RenderBatchs[RenderBatchCount].pTexture2->AddUseRef();
											szResourceName=RederBatchInfo.GetMember(SST_RBI_FX);
											m_Groups[GroupCount].RenderBatchs[RenderBatchCount].pFX=(CD3DFX *)pResourceManager->FindResource(szResourceName,GET_CLASS_INFO(CD3DFX));
											if(m_Groups[GroupCount].RenderBatchs[RenderBatchCount].pFX)
												m_Groups[GroupCount].RenderBatchs[RenderBatchCount].pFX->AddUseRef();
											RenderBatchCount++;
										}
									}
								}
								break;
							case SST_GI_FACE_FLAGS:
								{
									UINT FaceCount=Value.GetLength()/sizeof(BYTE);
									m_Groups[GroupCount].FaceFlags.Resize(FaceCount);
									memcpy(m_Groups[GroupCount].FaceFlags.GetBuffer(),(LPCSTR)Value,sizeof(BYTE)*FaceCount);
								}
								break;
							}
						}
						GroupCount++;
					}
				}
			}
			break;
		}
	}
	BuildSubMeshs();
	return true;
}
UINT CD3DWOWWMOModelResource::GetSmartStructSize(UINT Param)
{
	UINT Size=CD3DObjectResource::GetSmartStructSize(Param);
	Size += CSmartStruct::GetStructMemberSize(0);
	for(UINT i=0;i<m_DoodadInfos.GetCount();i++)
	{
		Size += CSmartStruct::GetStringMemberSize(m_DoodadInfos[i].pDoodadModel->GetNameLength());
		Size += CSmartStruct::GetStringMemberSizeA(sizeof(m_DoodadInfos[i].Translation));
		Size += CSmartStruct::GetStringMemberSizeA(sizeof(m_DoodadInfos[i].Rotation));
		Size += CSmartStruct::GetStringMemberSizeA(sizeof(m_DoodadInfos[i].Scaling));
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_DoodadInfos[i].Color));
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_DoodadInfos[i].GroupIndex));
		Size+=CSmartStruct::GetStructMemberSize(0);
	}
	Size+=CSmartStruct::GetStructMemberSize(0);
	for(UINT i=0;i<m_DoodadSets.GetCount();i++)
	{		
		Size += CSmartStruct::GetStringMemberSize(m_DoodadSets[i].Name.GetLength());
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_DoodadSets[i].StartDoodad));
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_DoodadSets[i].DoodadCount));
		Size+=CSmartStruct::GetStructMemberSize(0);
	}
	Size+=CSmartStruct::GetStructMemberSize(0);
	for(UINT i=0;i<m_Groups.GetCount();i++)
	{	
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_Groups[i].Index));
		Size += CSmartStruct::GetStringMemberSize(m_Groups[i].Name.GetLength());
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_Groups[i].Flags));
		Size += CSmartStruct::GetStringMemberSizeA(sizeof(m_Groups[i].BoundingBox));
		
		for(UINT j=0;j<m_Groups[i].PortalList.GetCount();j++)
		{
			Size+=CSmartStruct::GetFixMemberSize(sizeof(m_Groups[i].PortalList[j].GroupIndex));
			Size+=CSmartStruct::GetFixMemberSize(sizeof(m_Groups[i].PortalList[j].Filler));
			Size += CSmartStruct::GetStringMemberSizeA(sizeof(m_Groups[i].PortalList[j].Normal));
			Size += CSmartStruct::GetStringMemberSizeA(sizeof(m_Groups[i].PortalList[j].Center));
			Size+=CSmartStruct::GetFixMemberSize(sizeof(m_Groups[i].PortalList[j].Factor));
			Size += CSmartStruct::GetStringMemberSizeA(sizeof(CD3DVector3)*m_Groups[i].PortalList[j].Vertices.GetCount());
			Size+=CSmartStruct::GetStructMemberSize(0);
		}
		Size+=CSmartStruct::GetStructMemberSize(0);

		Size += CSmartStruct::GetStringMemberSizeA(sizeof(WORD)*m_Groups[i].IndexList.GetCount());
		Size += CSmartStruct::GetStringMemberSizeA(sizeof(MODEL_VERTEXT)*m_Groups[i].VertexList.GetCount());

		for(UINT j=0;j<m_Groups[i].BSPTree.GetCount();j++)
		{
			Size+=CSmartStruct::GetFixMemberSize(sizeof(m_Groups[i].BSPTree[j].PlaneType));
			Size+=CSmartStruct::GetFixMemberSize(sizeof(m_Groups[i].BSPTree[j].RightChildIndex));
			Size+=CSmartStruct::GetFixMemberSize(sizeof(m_Groups[i].BSPTree[j].LeftChildIndex));
			Size+=CSmartStruct::GetFixMemberSize(sizeof(m_Groups[i].BSPTree[j].FaceCount));
			Size+=CSmartStruct::GetFixMemberSize(sizeof(m_Groups[i].BSPTree[j].FirstFace));
			Size+=CSmartStruct::GetFixMemberSize(sizeof(m_Groups[i].BSPTree[j].Distance));
			Size+=CSmartStruct::GetStructMemberSize(0);
		}
		Size+=CSmartStruct::GetStructMemberSize(0);

		Size += CSmartStruct::GetStringMemberSizeA(sizeof(WORD)*m_Groups[i].BSPFaceList.GetCount());

		for(UINT j=0;j<m_Groups[i].RenderBatchs.GetCount();j++)
		{
			Size+=CSmartStruct::GetFixMemberSize(sizeof(m_Groups[i].RenderBatchs[j].StartIndex));
			Size+=CSmartStruct::GetFixMemberSize(sizeof(m_Groups[i].RenderBatchs[j].IndexCount));
			Size+=CSmartStruct::GetFixMemberSize(sizeof(m_Groups[i].RenderBatchs[j].StartVertex));
			Size+=CSmartStruct::GetFixMemberSize(sizeof(m_Groups[i].RenderBatchs[j].VertexCount));			
			if(m_Groups[i].RenderBatchs[j].pTexture1)
				Size += CSmartStruct::GetStringMemberSize(m_Groups[i].RenderBatchs[j].pTexture1->GetNameLength());
			if(m_Groups[i].RenderBatchs[j].pTexture2)
				Size += CSmartStruct::GetStringMemberSize(m_Groups[i].RenderBatchs[j].pTexture2->GetNameLength());
			if(m_Groups[i].RenderBatchs[j].pFX)
				Size += CSmartStruct::GetStringMemberSize(m_Groups[i].RenderBatchs[j].pFX->GetNameLength());
			Size+=CSmartStruct::GetStructMemberSize(0);
		}
		Size+=CSmartStruct::GetStructMemberSize(0);

		Size += CSmartStruct::GetStringMemberSizeA(sizeof(BYTE)*m_Groups[i].FaceFlags.GetCount());

		Size+=CSmartStruct::GetStructMemberSize(0);
	}
	return Size;
}

bool CD3DWOWWMOModelResource::LoadGroup(GROUP_INFO& GroupInfo,LPCTSTR ModelFileName,BLZ_CHUNK_MOMT * pMaterials,UINT MaterialCount,BLZ_CHUNK_MOTX * pTextureNames,BLZ_CHUNK_MOGN * pGroupNames)
{
	IFileAccessor * pFile;

	pFile=CD3DWOWWMOModel::CreateFileAccessor();
	if(pFile==NULL)
		return false;
	if(!pFile->Open(ModelFileName,IFileAccessor::modeRead))
	{
		PrintD3DLog(_T("文件%s打开失败"),ModelFileName);
		pFile->Release();
		return false;	
	}

	CBLZChunkFile WMOChunk;

	if(!WMOChunk.Load(pFile))
	{
		pFile->Release();
		return false;
	}
	pFile->Release();

	BLZ_CHUNK_MOGP * pGroup=(BLZ_CHUNK_MOGP *)WMOChunk.GetFirstChunk(CHUNK_ID_WMO_MOGP);
	if(pGroup==NULL)
		return false;

	if(pGroup->Name>=0&&pGroup->Name<(int)pGroupNames->ChunkSize)
	{
		GroupInfo.Name=pGroupNames->GroupFileNames+pGroup->Name;
	}
	GroupInfo.Flags=pGroup->Flags;

	CBLZChunkFile SubChunk;

	if(!SubChunk.Load((BYTE *)pGroup+sizeof(BLZ_CHUNK_MOGP),pGroup->ChunkSize-sizeof(BLZ_CHUNK_MOGP)+sizeof(BLZ_CHUNK_HEADER)))
		return false;

	BLZ_CHUNK_MOPY * pTriangleMaterialInfo=(BLZ_CHUNK_MOPY *)SubChunk.GetFirstChunk(CHUNK_ID_WMO_MOPY);
	if(pTriangleMaterialInfo==NULL)
		return false;
	UINT TriangleMaterialInfoCount=pTriangleMaterialInfo->ChunkSize/sizeof(WMOTriangleMaterialInfo);

	BLZ_CHUNK_MOVI * pIndices=(BLZ_CHUNK_MOVI *)SubChunk.GetFirstChunk(CHUNK_ID_WMO_MOVI);
	if(pIndices==NULL)
		return false;
	UINT TotalIndexCount=pIndices->ChunkSize/sizeof(UINT16);
	UINT TotalTriangleCount=TotalIndexCount/3;

	BLZ_CHUNK_MOVT * pVertices=(BLZ_CHUNK_MOVT *)SubChunk.GetFirstChunk(CHUNK_ID_WMO_MOVT);
	if(pVertices==NULL)
		return false;
	UINT TotalVertexCount=pVertices->ChunkSize/sizeof(CD3DVector3);

	BLZ_CHUNK_MONR * pNormals=(BLZ_CHUNK_MONR *)SubChunk.GetFirstChunk(CHUNK_ID_WMO_MONR);
	if(pNormals==NULL)
		return false;
	UINT TotalNormalCount=pNormals->ChunkSize/sizeof(CD3DVector3);

	BLZ_CHUNK_MOTV * pTextureUVs=(BLZ_CHUNK_MOTV *)SubChunk.GetFirstChunk(CHUNK_ID_WMO_MOTV);
	if(pTextureUVs==NULL)
		return false;
	UINT TotalUVCount=pTextureUVs->ChunkSize/sizeof(CD3DVector2);

	BLZ_CHUNK_MOBA * pRenderBatchs=(BLZ_CHUNK_MOBA *)SubChunk.GetFirstChunk(CHUNK_ID_WMO_MOBA);
	if(pRenderBatchs==NULL)
		return false;
	UINT RenderBatchCount=pRenderBatchs->ChunkSize/sizeof(WMORenderBatch);

	BLZ_CHUNK_MOLR * pLightRefrences=(BLZ_CHUNK_MOLR *)SubChunk.GetFirstChunk(CHUNK_ID_WMO_MOLR);

	BLZ_CHUNK_MODR * pDoodadRefrences=(BLZ_CHUNK_MODR *)SubChunk.GetFirstChunk(CHUNK_ID_WMO_MODR);



	BLZ_CHUNK_MOBN * pBSPNodes=(BLZ_CHUNK_MOBN *)SubChunk.GetFirstChunk(CHUNK_ID_WMO_MOBN);
	UINT BSPNodeCount=0;
	if(pBSPNodes)
	{
		BSPNodeCount=pBSPNodes->ChunkSize/(sizeof(BLZ_CHUNK_MOBN)-sizeof(BLZ_CHUNK_HEADER));
	}

	BLZ_CHUNK_MOBR * pBSPFaces=(BLZ_CHUNK_MOBR *)SubChunk.GetFirstChunk(CHUNK_ID_WMO_MOBR);
	UINT BSPFaceCount=0;
	if(pBSPFaces)
	{
		BSPFaceCount=pBSPFaces->ChunkSize/sizeof(UINT16);
	}

	BLZ_CHUNK_MOCV * pVertexColors=(BLZ_CHUNK_MOCV *)SubChunk.GetFirstChunk(CHUNK_ID_WMO_MOCV);

	BLZ_CHUNK_MLIQ * pLiquidInfos=(BLZ_CHUNK_MLIQ *)SubChunk.GetFirstChunk(CHUNK_ID_WMO_MLIQ);

	UINT StartBatch=0;
	UINT TotalBatchCount=pGroup->Batches[0]+pGroup->Batches[1]+pGroup->Batches[2];
	if(RenderBatchCount<TotalBatchCount)
		return false;

	if(pDoodadRefrences)
	{
		UINT DoodadCount=pDoodadRefrences->ChunkSize/sizeof(UINT16);
		for(UINT i=0;i<DoodadCount;i++)
		{
			WORD Index=pDoodadRefrences->DoodadIndices[i];
			if(Index<m_DoodadInfos.GetCount())
			{
				m_DoodadInfos[Index].GroupIndex=GroupInfo.Index;
			}
		}
	}

	//调整三角面的手性和顶点的坐标系
	GroupInfo.IndexList.Resize(TotalIndexCount);
	GroupInfo.VertexList.Resize(TotalVertexCount);
	
	for(UINT i=0;i<TotalIndexCount;i++)
	{
		GroupInfo.IndexList[i]=pIndices->VertexIndices[(i/3)*3+2-(i%3)];
	}
	for(UINT i=0;i<TotalVertexCount;i++)
	{
		GroupInfo.VertexList[i].Pos=BLZTranslationToD3D(pVertices->Vertices[i]);
		GroupInfo.VertexList[i].Normal=BLZTranslationToD3D(pNormals->Normals[i]);
		GroupInfo.VertexList[i].TextureCoord=pTextureUVs->TextureCoordinates[i];	
		GroupInfo.VertexList[i].Diffuse=0xFFEEEEEE;
	}

	//三角面属性
	GroupInfo.FaceFlags.Resize(TriangleMaterialInfoCount);
	for(UINT i=0;i<TriangleMaterialInfoCount;i++)
	{
		GroupInfo.FaceFlags[i]=pTriangleMaterialInfo->TriangleMaterialInfo[i].Flags;
	}

	//BSP树
	if(pBSPNodes&&pBSPFaces)
	{
		GroupInfo.BSPTree.Resize(BSPNodeCount);
		GroupInfo.BSPFaceList.Resize(BSPFaceCount);
		for(UINT i=0;i<BSPNodeCount;i++)
		{
			GroupInfo.BSPTree[i].PlaneType=pBSPNodes->BSPNodes[i].PlaneType;
			if(GroupInfo.BSPTree[i].PlaneType==BPT_XY)
			{
				GroupInfo.BSPTree[i].RightChildIndex=pBSPNodes->BSPNodes[i].LeftChild;
				GroupInfo.BSPTree[i].LeftChildIndex=pBSPNodes->BSPNodes[i].RightChild;
				GroupInfo.BSPTree[i].Distance=-pBSPNodes->BSPNodes[i].Distance;
			}
			else
			{
				GroupInfo.BSPTree[i].RightChildIndex=pBSPNodes->BSPNodes[i].RightChild;
				GroupInfo.BSPTree[i].LeftChildIndex=pBSPNodes->BSPNodes[i].LeftChild;
				GroupInfo.BSPTree[i].Distance=pBSPNodes->BSPNodes[i].Distance;
			}		
			GroupInfo.BSPTree[i].FaceCount=pBSPNodes->BSPNodes[i].FaceCount;
			GroupInfo.BSPTree[i].FirstFace=pBSPNodes->BSPNodes[i].FirstFace;		
			GroupInfo.BSPTree[i].pFaceBoard=NULL;
		}
		for(UINT i=0;i<BSPFaceCount;i++)
		{
			GroupInfo.BSPFaceList[i]=pBSPFaces->BSPFaceIndices[i];
		}
	}

	for(UINT i=StartBatch;i<TotalBatchCount;i++)
	{
		RENDER_BATCH_INFO BatchInfo;

		BatchInfo.StartIndex=pRenderBatchs->RenderBatchs[i].StartIndex;
		BatchInfo.IndexCount=pRenderBatchs->RenderBatchs[i].IndexCount;
		BatchInfo.StartVertex=pRenderBatchs->RenderBatchs[i].StartVertex;
		BatchInfo.VertexCount=pRenderBatchs->RenderBatchs[i].EndVertex-pRenderBatchs->RenderBatchs[i].StartVertex+1;

		int MaterialIndex=pRenderBatchs->RenderBatchs[i].Material;
		for(UINT j=0;j<BatchInfo.VertexCount;j++)
		{
			GroupInfo.VertexList[j+BatchInfo.StartVertex].Diffuse=pMaterials->Materials[MaterialIndex].TextureColor1;
			//GroupInfo.VertexList[GroupInfo.IndexList[j]].Diffuse=0xFFFFFFFF;
		}

		if(!MakeMaterial(BatchInfo,pMaterials->Materials[MaterialIndex],pTextureNames))
			return false;

		GroupInfo.RenderBatchs.Add(BatchInfo);
	}

	return true;
}

bool CD3DWOWWMOModelResource::LoadDoodads(UINT DoodadCount,UINT DoodadSetCount,BLZ_CHUNK_MODS * pDoodadSets,BLZ_CHUNK_MODN * pDoodadFileNames,BLZ_CHUNK_MODD * pDoodads)
{
	if(DoodadCount)
	{
		UINT RealDoodadCount=pDoodads->ChunkSize/sizeof(SMODoodadInfo);
		if(DoodadCount>RealDoodadCount)
		{
			//PrintSystemLog(0,"CD3DWOWWMOModelResource::LoadDoodads:DoodadCount有误");
			DoodadCount=RealDoodadCount;
		}
		m_DoodadInfos.Resize(DoodadCount);
		for(UINT i=0;i<DoodadCount;i++)
		{
			m_DoodadInfos[i].pDoodadModel=NULL;
			if(pDoodads->Doodads[i].NameIndex<pDoodadFileNames->ChunkSize)
			{
				CEasyString ModelFileName=pDoodadFileNames->DoodadFileNames+pDoodads->Doodads[i].NameIndex;
				CEasyString SkinFileName;
				int Pos=ModelFileName.ReverseFind('.');
				if(Pos>=0)
				{
					ModelFileName=ModelFileName.Left(Pos);
					ModelFileName+=".m2";

					CEasyString ObjectName = ModelFileName + "_" + CFileTools::GetPathFileName(SkinFileName);

					CD3DWOWM2ModelResource* pResource=
						dynamic_cast<CD3DWOWM2ModelResource*>(m_pManager->GetResource(ObjectName));
					if(!pResource)
					{
						pResource=new CD3DWOWM2ModelResource(m_pManager);
						if(pResource->LoadFromFile(ModelFileName))
						{
							//PrintSystemLog(0,"加载了[%s]",(LPCTSTR)ModelFileName);
							if(!m_pManager->AddResource(pResource,ObjectName))
							{
								pResource->Release();
								pResource=NULL;
							}
						}
						else
						{
							pResource->Release();
							pResource=NULL;
						}						
					}
					else
					{
						pResource->AddUseRef();
					}	
					m_DoodadInfos[i].pDoodadModel=pResource;
				}	
				
				m_DoodadInfos[i].Translation=BLZTranslationToD3D(pDoodads->Doodads[i].Translation);
				m_DoodadInfos[i].Rotation=BLZRotationToD3D(pDoodads->Doodads[i].Rotation);
				m_DoodadInfos[i].Scaling=pDoodads->Doodads[i].Scaling;
				m_DoodadInfos[i].Color=pDoodads->Doodads[i].Color;
				m_DoodadInfos[i].GroupIndex=0xffff;
			}			
		}
		if(DoodadSetCount)
		{
			m_DoodadSets.Resize(DoodadSetCount);
			for(UINT i=0;i<DoodadSetCount;i++)
			{
				m_DoodadSets[i].Name=pDoodadSets->DoodadSets[i].Name;
				m_DoodadSets[i].StartDoodad=pDoodadSets->DoodadSets[i].FirstInstanceIndex;
				m_DoodadSets[i].DoodadCount=pDoodadSets->DoodadSets[i].DoodadCount;
			}
		}
	}
	return true;
}

bool CD3DWOWWMOModelResource::LoadGroups(LPCTSTR ModelFileName,UINT GroupCount,BLZ_CHUNK_MOGI * pGroups,BLZ_CHUNK_MOGN * pGroupNames,BLZ_CHUNK_MOMT * pMaterials,UINT MaterialCount,BLZ_CHUNK_MOTX * pTextureNames)
{
	if(GroupCount)
	{
		m_Groups.Resize(GroupCount);
		CEasyString GroupFileName=ModelFileName;
		CEasyString GroupFileExt;
		int Pos=GroupFileName.ReverseFind('.');
		if(Pos>=0)
		{
			GroupFileExt=GroupFileName.Right(GroupFileName.GetLength()-Pos);
			GroupFileName=GroupFileName.Left(Pos);
		}

		for(UINT i=0;i<GroupCount;i++)
		{
			if(pGroups->Group[i].NameOffset>=0&&pGroups->Group[i].NameOffset<(int)pGroupNames->ChunkSize)
			{
				m_Groups[i].Name=pGroupNames->GroupFileNames+pGroups->Group[i].NameOffset;
			}
			m_Groups[i].Index=i;
			m_Groups[i].Flags=pGroups->Group[i].Flags;
			m_Groups[i].BoundingBox.m_Min=BLZTranslationToD3D(pGroups->Group[i].BoundingBox[0]);
			m_Groups[i].BoundingBox.m_Max=BLZTranslationToD3D(pGroups->Group[i].BoundingBox[1]);
			m_Groups[i].BoundingBox.Rebuild();
			CEasyString FileName;
			FileName.Format(_T("%s_%03d%s"),
				(LPCTSTR)GroupFileName,i,(LPCTSTR)GroupFileExt);
			if(!LoadGroup(m_Groups[i],FileName,pMaterials,MaterialCount,pTextureNames,pGroupNames))
				return false;
		}
	}
	return true;
}





bool CD3DWOWWMOModelResource::MakeMaterial(RENDER_BATCH_INFO& BatchInfo,WMOMaterial& MaterialInfo,BLZ_CHUNK_MOTX * pTextureNames)
{
	
	CEasyString TextureFileName1=pTextureNames->TextureFileNames+MaterialInfo.Texture1;
	CEasyString TextureFileName2=pTextureNames->TextureFileNames+MaterialInfo.Texture2;

	BatchInfo.pTexture1=m_pManager->GetDevice()->GetTextureManager()->LoadTexture(TextureFileName1);
	if(TextureFileName2[0])
	{
		BatchInfo.pTexture2=m_pManager->GetDevice()->GetTextureManager()->LoadTexture(TextureFileName2);
	}
	else
	{
		BatchInfo.pTexture2=NULL;
	}

	BatchInfo.pFX=BuildFX(MaterialInfo.BlendMode,MaterialInfo.Flags);
	return true;
}


CD3DFX * CD3DWOWWMOModelResource::BuildFX(UINT BlendMode,UINT TextureFlag)
{
	CEasyString	 FXName;
	CEasyStringA EnableZWrite;
	CEasyStringA EnableFog;
	CEasyStringA CullMode;
	CEasyStringA EnableAlphaBlend;
	CEasyStringA BlendOp;
	CEasyStringA SrcBlend;
	CEasyStringA DestBlend;
	CEasyStringA EnableAlphaTest;
	CEasyStringA DiffuseOperation;
	
	FXName.Format(_T("WMOModel\\0x%X_0x%X"),
		BlendMode,
		TextureFlag);
	

	EnableFog="False";
	
	if(TextureFlag&WMOMF_NO_CULL)
	{
		CullMode="None";
	}
	else
	{
		CullMode="CCW";
	}

	if(TextureFlag&WMOMF_EMISSIVE)
	{
		DiffuseOperation="Input.Diffuse";
	}
	else
	{
		DiffuseOperation="CaculateDiffuse(Pos,Normal)";
	}

		
	EnableZWrite="True";

	EnableAlphaBlend="False";	
	EnableAlphaTest="False";
	BlendOp="Add";
	SrcBlend="SrcAlpha";
	DestBlend="InvSrcAlpha";
	switch(BlendMode)
	{	
	case WMOBM_OPAQUE:		
		break;
	case WMOBM_ALPHA_TEST:
		EnableAlphaTest="True";
		break;	
	}	

	

	CEasyStringA FxContent;

	
	FxContent=WMO_MODEL_FX;
	
	FxContent.Replace("<EnableZWrite>",EnableZWrite);
	FxContent.Replace("<EnableFog>",EnableFog);
	FxContent.Replace("<CullMode>",CullMode);
	FxContent.Replace("<EnableAlphaBlend>",EnableAlphaBlend);
	FxContent.Replace("<BlendOp>",BlendOp);
	FxContent.Replace("<SrcBlend>",SrcBlend);
	FxContent.Replace("<DestBlend>",DestBlend);
	FxContent.Replace("<EnableAlphaTest>",EnableAlphaTest);
	FxContent.Replace("<DiffuseOperation>",DiffuseOperation);
	

	CD3DFX * pFX=m_pManager->GetDevice()->GetFXManager()->LoadFXFromMemory(FXName,FxContent.GetBuffer(),FxContent.GetLength());	
	return pFX;
}



void CD3DWOWWMOModelResource::BuildSubMeshs()
{
	for(UINT i=0;i<m_Groups.GetCount();i++)
	{
		for(UINT j=0;j<m_Groups[i].RenderBatchs.GetCount();j++)
		{
			CD3DSubMesh * pD3DSubMesh=new CD3DSubMesh(m_pManager->GetDevice());

			UINT StartIndex=m_Groups[i].RenderBatchs[j].StartIndex;
			UINT IndexCount=m_Groups[i].RenderBatchs[j].IndexCount;
			UINT StartVertex=m_Groups[i].RenderBatchs[j].StartVertex;
			UINT VertexCount=m_Groups[i].RenderBatchs[j].VertexCount;


			pD3DSubMesh->GetVertexFormat().FVF=D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1;
			pD3DSubMesh->GetVertexFormat().VertexSize=sizeof(MODEL_VERTEXT);
			pD3DSubMesh->GetVertexFormat().IndexSize=sizeof(WORD);
			pD3DSubMesh->SetPrimitiveType(D3DPT_TRIANGLELIST);
			pD3DSubMesh->SetPrimitiveCount(IndexCount/3);		
			pD3DSubMesh->SetVertexCount(VertexCount);
			pD3DSubMesh->SetIndexCount(IndexCount);

			pD3DSubMesh->SetIndexStart(StartIndex);
			pD3DSubMesh->SetVertexStart(StartVertex);

			pD3DSubMesh->SetIndices((BYTE *)m_Groups[i].IndexList.GetBuffer());
			pD3DSubMesh->SetVertices((BYTE *)m_Groups[i].VertexList.GetBuffer());

			pD3DSubMesh->SetRenderBufferUsed(CD3DSubMesh::BUFFER_USE_CUSTOM);
			pD3DSubMesh->SetOrginDataBufferUsed(CD3DSubMesh::BUFFER_USE_CUSTOM);
			
			if(m_Groups[i].RenderBatchs[j].pTexture1)
			{
				pD3DSubMesh->GetMaterial().AddTexture(m_Groups[i].RenderBatchs[j].pTexture1,0,"TexLay0","");
				m_Groups[i].RenderBatchs[j].pTexture1->AddUseRef();
			}
			if(m_Groups[i].RenderBatchs[j].pTexture2)
			{
				pD3DSubMesh->GetMaterial().AddTexture(m_Groups[i].RenderBatchs[j].pTexture2,0,"TexLay1","");
				m_Groups[i].RenderBatchs[j].pTexture2->AddUseRef();
			}
			if(m_Groups[i].RenderBatchs[j].pFX)
			{
				pD3DSubMesh->GetMaterial().SetFX(m_Groups[i].RenderBatchs[j].pFX);
				m_Groups[i].RenderBatchs[j].pFX->AddUseRef();
			}

			D3DCOLORVALUE WhiteColor={1.0f,1.0f,1.0f,1.0f};
			D3DCOLORVALUE GrayColor={0.8f,0.8f,0.8f,1.0f};
			D3DCOLORVALUE BlackColor={0.0f,0.0f,0.0f,1.0f};

			pD3DSubMesh->GetMaterial().GetMaterial().Ambient=WhiteColor;
			pD3DSubMesh->GetMaterial().GetMaterial().Diffuse=WhiteColor;
			pD3DSubMesh->GetMaterial().GetMaterial().Specular=WhiteColor;
			pD3DSubMesh->GetMaterial().GetMaterial().Emissive=BlackColor;
			pD3DSubMesh->GetMaterial().GetMaterial().Power=40.0f;

			pD3DSubMesh->GetMaterial().CaculateHashCode();
						

			UINT64 SubMeshProperty=pD3DSubMesh->GetProperty();
			if(m_Groups[i].Flags&WMOGF_OUTDOOR)
			{
				SubMeshProperty|=SMP_OUTSIDE;
			}
			if(m_Groups[i].Flags&WMOGF_INDOOR)
			{
				SubMeshProperty|=SMP_INSIDE;
			}

			pD3DSubMesh->CreateBounding();

			pD3DSubMesh->SetID(m_Groups[i].Index*1000+j);
			CEasyString SubMeshName;
			SubMeshName.Format(_T("%s-%03d-%03d"),
				(LPCTSTR)m_Groups[i].Name,m_Groups[i].Index,j);
			pD3DSubMesh->SetName(SubMeshName);

			m_Groups[i].GroupSubMeshList.Add(pD3DSubMesh);			
		}
	}
}


}