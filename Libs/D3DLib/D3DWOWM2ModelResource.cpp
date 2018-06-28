/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWOWM2ModelResource.cpp                                */
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



IMPLEMENT_CLASS_INFO(CD3DWOWM2ModelResource,CD3DObjectResource);



typedef CEasyMap<int,int> CIntMap;

class CSubMeshTrimer
{
protected:
	struct INDEXDATA
	{
		WORD NewIndex;
		WORD OldIndex;
	};
	CEasyArray<CD3DWOWM2ModelResource::MODEL_VERTEXT>	m_VertextData;
	CEasyArray<INDEXDATA>								m_IndexData;
	int 												m_VertextSize;
	int 												m_MatterialID;
	CIntMap												m_IndexMap;
public:
	CSubMeshTrimer(){m_VertextSize=0;m_MatterialID=0;}
	~CSubMeshTrimer(){}

	void SetMaterialID(int ID)
	{
		m_MatterialID=ID;
	}
	int GetMaterialID()
	{
		return m_MatterialID;
	}

	bool AddVertext(WORD Index,BYTE * pVertextData,int VertextSize,DWORD FVF)
	{		
		INDEXDATA IndexData;
		bool HaveFind=false;
		IndexData.OldIndex=Index;
		if(m_VertextSize)
		{
			if(m_VertextSize!=VertextSize)
				return false;
		}
		else
			m_VertextSize=VertextSize;		
		int NewIndex=m_IndexMap[Index]-1;
		if(NewIndex>=0)
		{		
			IndexData.NewIndex=NewIndex;
			m_IndexData.Add(IndexData);
		}
		else
		{
			IndexData.NewIndex=m_VertextData.GetCount();
			m_IndexMap[Index]=IndexData.NewIndex+1;
			m_IndexData.Add(IndexData);

			CD3DWOWM2ModelResource::MODEL_VERTEXT Vertex;
			ZeroMemory(&Vertex,sizeof(Vertex));
			DWORD FVFPos=FVF&D3DFVF_POSITION_MASK;
			int DataSize=0;			
			switch(FVFPos)
			{
			case D3DFVF_XYZ:	
				DataSize=sizeof(CD3DVector3);
				Vertex.Pos=*((CD3DVector3 *)pVertextData);
				pVertextData+=DataSize;
				break;
			case D3DFVF_XYZRHW:	
				DataSize=sizeof(CD3DVector4);
				Vertex.Pos=*((CD3DVector3 *)pVertextData);
				pVertextData+=DataSize;
				break;
			case D3DFVF_XYZB1:				
				DataSize=sizeof(CD3DVector4)+sizeof(FLOAT);
				Vertex.Pos=*((CD3DVector3 *)pVertextData);
				pVertextData+=DataSize;
				break;
			case D3DFVF_XYZB2:				
				DataSize=sizeof(CD3DVector4)+sizeof(FLOAT)*2;
				Vertex.Pos=*((CD3DVector3 *)pVertextData);
				pVertextData+=DataSize;
				break;
			case D3DFVF_XYZB3:
				DataSize=sizeof(CD3DVector4)+sizeof(FLOAT)*3;
				Vertex.Pos=*((CD3DVector3 *)pVertextData);
				pVertextData+=DataSize;
				break;
			case D3DFVF_XYZB4:
				DataSize=sizeof(CD3DVector4)+sizeof(FLOAT)*4;
				Vertex.Pos=*((CD3DVector3 *)pVertextData);
				pVertextData+=DataSize;
				break;
			case D3DFVF_XYZB5:
				DataSize=sizeof(CD3DVector4)+sizeof(FLOAT)*5;
				Vertex.Pos=*((CD3DVector3 *)pVertextData);
				pVertextData+=DataSize;
				break;
			case D3DFVF_XYZW:
				DataSize=sizeof(CD3DVector4);
				Vertex.Pos=*((CD3DVector3 *)pVertextData);
				pVertextData+=DataSize;
				break;
			}
			if(FVF&D3DFVF_NORMAL)
			{
				Vertex.Normal=*((CD3DVector3 *)pVertextData);
				pVertextData+=sizeof(CD3DVector3);
			}
			if(FVF&D3DFVF_PSIZE)
			{
				pVertextData+=sizeof(FLOAT);
			}
			if(FVF&D3DFVF_DIFFUSE)
			{
				Vertex.Diffuse=*((D3DCOLOR *)pVertextData);
				pVertextData+=sizeof(D3DCOLOR);
			}
			if(FVF&D3DFVF_SPECULAR)
			{
				pVertextData+=sizeof(D3DCOLOR);
			}
			UINT TexCount=(FVF&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT;
			if(TexCount)
			{
				Vertex.TextureCoord=*((CD3DVector2 *)pVertextData);
				pVertextData+=sizeof(CD3DVector2)*TexCount;
			}
			m_VertextData.Add(Vertex);
			
		}
		return true;

	}
	int GetIndexCount()
	{
		return (int)m_IndexData.GetCount();
	}
	int GetVertexCount()
	{
		if(m_VertextSize)
			return m_VertextData.GetCount();
		else
			return 0;
	}
	int GetVertexSize()
	{
		return m_VertextSize;
	}
	void GetIndex(WORD * pBuff,int Size)
	{
		if(Size>GetIndexCount())
			Size=GetIndexCount();
		for(int i=0;i<Size;i++)
			pBuff[i]=m_IndexData[i].NewIndex;
	}
	void GetVertex(LPVOID pBuff,int Size)
	{
		if(Size<(int)m_VertextData.GetCount()*m_VertextSize)
			Size=(int)(int)m_VertextData.GetCount()*m_VertextSize;
		memcpy(pBuff,&(m_VertextData[0]),Size);
	}

};


CD3DWOWM2ModelResource::CD3DWOWM2ModelResource(void)
{
	m_HasBoneAni=false;
	m_HasMaterialAni=false;
	m_HasSkinMeshAni=false;
	m_SkinMeshBoneCount=0;
	m_BoneAniCacheSize=0;
	m_BoneAniCacheFreq=0;
	m_Version=0;
}

CD3DWOWM2ModelResource::CD3DWOWM2ModelResource(CD3DObjectResourceManager* pManager)
	:CD3DObjectResource(pManager)
{
	m_HasBoneAni=false;
	m_HasMaterialAni=false;
	m_HasSkinMeshAni=false;
	m_SkinMeshBoneCount=0;
	m_BoneAniCacheSize=0;
	m_BoneAniCacheFreq=0;
	m_Version=0;
}

CD3DWOWM2ModelResource::~CD3DWOWM2ModelResource(void)
{
	Destory();
}

void CD3DWOWM2ModelResource::Destory()
{
	for(UINT i=0;i<m_SubMeshList.GetCount();i++)
	{
		SAFE_RELEASE(m_SubMeshList[i]);
	}

	for(UINT i=0;i<m_ParticleEmitters.GetCount();i++)
	{
		SAFE_RELEASE(m_ParticleEmitters[i].pModelResource);
	}
	
	for(UINT i=0;i<m_RibbonEmitters.GetCount();i++)
	{
		SAFE_RELEASE(m_RibbonEmitters[i].pModelResource);		
	}
	m_SkinFiles.Clear();
	m_SubMeshList.Clear();	
	m_AnimationSequence.Clear();
	m_Bones.Clear();		
	m_KeyBoneIndex.Clear();
	m_GlobalSequences.Clear();
	m_Attachments.Clear();	
	m_ParticleEmitters.Clear();
	m_RibbonEmitters.Clear();
	m_BoneAniCache.Clear();
	m_GlobalBoneAniCache.Clear();
	m_HasBoneAni=false;
	m_HasMaterialAni=false;
	m_HasSkinMeshAni=false;
	m_SkinMeshBoneCount=0;
	m_BoneAniCacheSize=0;
	m_BoneAniCacheFreq=0;
	CD3DObjectResource::Destory();
}

bool CD3DWOWM2ModelResource::Reset()
{
	bool Ret=true;
	for(UINT i=0;i<m_SubMeshList.GetCount();i++)
	{		
		Ret=Ret&&m_SubMeshList[i]->Reset();
	}	
	return Ret&&CD3DObjectResource::Reset();
}
bool CD3DWOWM2ModelResource::Restore()
{
	bool Ret=true;
	for(UINT i=0;i<m_SubMeshList.GetCount();i++)
	{		
		Ret=Ret&&m_SubMeshList[i]->Restore();
	}	
	return Ret&&CD3DObjectResource::Restore();
}

bool CD3DWOWM2ModelResource::LoadFromFile(LPCTSTR szModelFileName)
{

	Destory();

	IFileAccessor * pFile;


	CEasyBuffer ModelData;
	

	pFile=CD3DWOWM2Model::CreateFileAccessor();
	if(pFile==NULL)
		return false;
	if(!pFile->Open(szModelFileName,IFileAccessor::modeRead))
	{
		PrintD3DLog(_T("文件%s打开失败"),szModelFileName);
		pFile->Release();
		return false;	
	}
	UINT FileSize=(UINT)pFile->GetSize();	
	ModelData.Create(FileSize);	
	pFile->Read(ModelData.GetBuffer(),FileSize);
	ModelData.SetUsedSize(FileSize);
	pFile->Release();

	SetName(szModelFileName);

	CEasyString ModelPath=szModelFileName;
	int Pos=ModelPath.ReverseFind('.');
	if(Pos<0)
	{
		return false;
	}
	ModelPath=ModelPath.Left(Pos);

	BLZ_M2_HEADER * pHeader=(BLZ_M2_HEADER *)ModelData.GetBuffer();
	if(pHeader->Tag!=BLZ_M2_HEADER_TAG)
		return false;

	m_Flag=pHeader->ModelFlag;
	m_Version=pHeader->Version;

	m_ModelFilePath=szModelFileName;

	CheckSkins(ModelPath);

	if(m_SkinFiles.GetCount()<=0)
		return false;

	//PrintD3DDebugLog("M2 Flag=0x%X",pHeader->ModelFlag);

	if(!LoadColorAnimation((BYTE *)ModelData.GetBuffer()))
		return false;

	if(!LoadTransparencyAnimation((BYTE *)ModelData.GetBuffer()))
		return false;

	if(!LoadTextureUVAnimation((BYTE *)ModelData.GetBuffer()))
		return false;

	if(!LoadBones((BYTE *)ModelData.GetBuffer(),ModelPath))
		return false;

	if(!LoadAttachments((BYTE *)ModelData.GetBuffer()))
		return false;
	
	if(!LoadCameraInfos((BYTE *)ModelData.GetBuffer()))
		return false;

	if(!LoadRibbonEmitters((BYTE *)ModelData.GetBuffer()))
		return false;

	if(!LoadParticleEmitters((BYTE *)ModelData.GetBuffer()))
		return false;
	
	if(!LoadSkin(m_SkinFiles[0],m_SubMeshList,(BYTE *)ModelData.GetBuffer()))
		return false;
	

	CheckAni();
	

	CreateBounding();


	return true;

}

bool CD3DWOWM2ModelResource::LoadFromXFile(LPCTSTR szModelFileName)
{
	HRESULT hr;
	ID3DXMesh	*pMesh;	
	ID3DXBuffer *pMaterialsBuffer;	
	D3DXMATERIAL *pMaterials;	
	DWORD NumMaterials;		
	DWORD * pAttributes;
	CEasyArray<CSubMeshTrimer>  OrgSubMeshs;
	BYTE * pVertexBuff=NULL;
	WORD * pIndexBuff=NULL;


	Destory();


	IFileAccessor * pFile;

	pFile=CD3DWOWM2Model::CreateFileAccessor();
	if(pFile==NULL)
		return false;
	if(!pFile->Open(szModelFileName,IFileAccessor::modeRead))
	{
		pFile->Release();
		return false;	
	}
	int FileSize=(int)pFile->GetSize();	
	BYTE * pData=new BYTE[FileSize];
	pFile->Read(pData,FileSize);
	pFile->Release();

	hr=D3DXLoadMeshFromXInMemory( pData,FileSize, D3DXMESH_SYSTEMMEM, 
		m_pManager->GetDevice()->GetD3DDevice(),
		NULL, &pMaterialsBuffer, NULL,
		&NumMaterials, &pMesh);

	delete[] pData;

	if(FAILED(hr))
		return false;


	//获取顶点数据

	LPDIRECT3DVERTEXBUFFER9 pVertexsBuffer;
	D3DVERTEXBUFFER_DESC	VertexsBufferDesc;
	pMesh->GetVertexBuffer(&pVertexsBuffer);
	pVertexsBuffer->GetDesc(&VertexsBufferDesc);	
	pVertexsBuffer->Lock(0,0,(LPVOID *)&pVertexBuff,D3DLOCK_READONLY);	

	int VertexSize=pMesh->GetNumBytesPerVertex();
	int VertexCount=VertexsBufferDesc.Size/VertexSize;

	//获取索引数据
	LPDIRECT3DINDEXBUFFER9	pIndexsBuffer;
	D3DINDEXBUFFER_DESC		IndexsBufferDesc;

	pMesh->GetIndexBuffer(&pIndexsBuffer);
	pIndexsBuffer->GetDesc(&IndexsBufferDesc);			
	pIndexsBuffer->Lock(0,0,(LPVOID *)&pIndexBuff,D3DLOCK_READONLY);		

	int IndexCount=IndexsBufferDesc.Size/sizeof(WORD);



	//获取材质和面的对应信息
	pMaterials=(D3DXMATERIAL *)pMaterialsBuffer->GetBufferPointer();

	hr=pMesh->LockAttributeBuffer(D3DLOCK_READONLY,&pAttributes);
	if(FAILED(hr))
	{
		SAFE_RELEASE(pMesh);	
		SAFE_RELEASE(pMaterialsBuffer);
		return false;
	}	
	//按材质建立SubMesh
	OrgSubMeshs.Resize(NumMaterials);
	for(int i=0;i<(int)OrgSubMeshs.GetCount();i++)
	{
		OrgSubMeshs[i].SetMaterialID(i);
	}

	//按材质划分SubMesh
	for(int i=0;i<(int)pMesh->GetNumFaces();i++)
	{
		OrgSubMeshs[pAttributes[i]].AddVertext(pIndexBuff[i*3],pVertexBuff+pIndexBuff[i*3]*VertexSize,VertexSize,pMesh->GetFVF());
		OrgSubMeshs[pAttributes[i]].AddVertext(pIndexBuff[i*3+1],pVertexBuff+pIndexBuff[i*3+1]*VertexSize,VertexSize,pMesh->GetFVF());
		OrgSubMeshs[pAttributes[i]].AddVertext(pIndexBuff[i*3+2],pVertexBuff+pIndexBuff[i*3+2]*VertexSize,VertexSize,pMesh->GetFVF());

	}

	pMesh->UnlockAttributeBuffer();


	//除去空的SubMesh
	for(int i=NumMaterials-1;i>=0;i--)
	{
		if(OrgSubMeshs[i].GetIndexCount()<=0)
			OrgSubMeshs.Delete(i);
	}

	for(UINT i=0;i<OrgSubMeshs.GetCount();i++)
	{
		CD3DSubMesh * pD3DSubMesh=new CD3DSubMesh(m_pManager->GetDevice());

		pD3DSubMesh->GetVertexFormat().FVF=D3DFVF_XYZB5|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_LASTBETA_UBYTE4;
		pD3DSubMesh->GetVertexFormat().VertexSize=sizeof(MODEL_VERTEXT);
		pD3DSubMesh->GetVertexFormat().IndexSize=sizeof(WORD);
		pD3DSubMesh->SetPrimitiveType(D3DPT_TRIANGLELIST);
		pD3DSubMesh->SetPrimitiveCount(OrgSubMeshs[i].GetIndexCount()/3);		
		pD3DSubMesh->SetVertexCount(OrgSubMeshs[i].GetVertexCount());		
		pD3DSubMesh->SetIndexCount(OrgSubMeshs[i].GetIndexCount());

		int Size=OrgSubMeshs[i].GetVertexCount()*sizeof(MODEL_VERTEXT);		

		pD3DSubMesh->AllocDXVertexBuffer();


		BYTE * pBuff;
		pD3DSubMesh->GetDXVertexBuffer()->Lock(0,0,(LPVOID *)&pBuff,0);
		OrgSubMeshs[i].GetVertex(pBuff,Size);
		pD3DSubMesh->GetDXVertexBuffer()->Unlock();

		Size=OrgSubMeshs[i].GetIndexCount();

		pD3DSubMesh->AllocDXIndexBuffer();	


		pD3DSubMesh->GetDXIndexBuffer()->Lock(0,0,(LPVOID *)&pBuff,0);
		OrgSubMeshs[i].GetIndex((WORD *)pBuff,Size);
		pD3DSubMesh->GetDXIndexBuffer()->Unlock();




		int MatID=OrgSubMeshs[i].GetMaterialID();



		pD3DSubMesh->GetMaterial().SetMaterial(pMaterials[MatID].MatD3D);
		if(pMaterials[MatID].pTextureFilename)
		{
			CEasyString TexFileName=pMaterials[MatID].pTextureFilename;
			CD3DTexture * pTexture=m_pManager->GetDevice()->GetTextureManager()->LoadTexture(TexFileName,0);
			UINT64 TextureFlag=(D3D_TEX_FLAG_WRAP_X|D3D_TEX_FLAG_WRAP_Y)<<CD3DWOWM2ModelResource::TF_TEXTURE_FLAG_SHIFT;
			if(pTexture)
			{
				CEasyStringA TexFXName;
				CEasyStringA MatFXName;
				TexFXName.Format("TexLay%d",pD3DSubMesh->GetMaterial().GetTextureLayerCount());
				MatFXName.Format("UVMatrix%d",pD3DSubMesh->GetMaterial().GetTextureLayerCount());
				pD3DSubMesh->GetMaterial().AddTexture(pTexture,TextureFlag,TexFXName,MatFXName);			
			}
		}	

		if((pMesh->GetFVF()&D3DFVF_NORMAL)==0)
		{
			UINT64 SubMeshProperty=pD3DSubMesh->GetProperty();
			SubMeshProperty|=D3D_RENDER_FLAG_NO_LIGHT<<CD3DWOWM2ModelResource::SMP_RENDER_FLAG_SHIFT;
			pD3DSubMesh->SetProperty(SubMeshProperty);
		}
		
		BuildFX(pD3DSubMesh);

		m_SubMeshList.Add(pD3DSubMesh);

	}	

	pVertexsBuffer->Unlock();
	pIndexsBuffer->Unlock();
	SAFE_RELEASE(pVertexsBuffer);
	SAFE_RELEASE(pIndexsBuffer);
	SAFE_RELEASE(pMesh);	
	SAFE_RELEASE(pMaterialsBuffer);
	CreateBounding();
	return true;
}

bool CD3DWOWM2ModelResource::ChangeSkin(UINT Index)
{
	if(Index<m_SkinFiles.GetCount())
	{
		for(UINT i=0;i<m_SubMeshList.GetCount();i++)
		{
			SAFE_RELEASE(m_SubMeshList[i]);
		}
		
		m_SubMeshList.Clear();	
	
		
		IFileAccessor * pFile;


		CEasyBuffer ModelData;


		pFile=CD3DWOWM2Model::CreateFileAccessor();
		if(pFile==NULL)
			return false;
		if(!pFile->Open(m_ModelFilePath,IFileAccessor::modeRead))
		{
			pFile->Release();
			return false;	
		}
		UINT FileSize=(UINT)pFile->GetSize();	
		ModelData.Create(FileSize);	
		pFile->Read(ModelData.GetBuffer(),FileSize);
		ModelData.SetUsedSize(FileSize);
		pFile->Release();


		return LoadSkin(m_SkinFiles[Index],m_SubMeshList,(BYTE *)ModelData.GetBuffer());
	}

	return false;
}

void CD3DWOWM2ModelResource::PickResource(CUSOResourceManager * pResourceManager,UINT Param)
{
	CD3DObjectResource::PickResource(pResourceManager,Param);
	for(UINT i=0;i<m_ParticleEmitters.GetCount();i++)
	{	
		m_ParticleEmitters[i].pModelResource->PickResource(pResourceManager,Param);
		pResourceManager->AddResource(m_ParticleEmitters[i].pModelResource);		
	}
	for(UINT i=0;i<m_RibbonEmitters.GetCount();i++)
	{
		m_RibbonEmitters[i].pModelResource->PickResource(pResourceManager,Param);
		pResourceManager->AddResource(m_RibbonEmitters[i].pModelResource);		
	}
}

bool CD3DWOWM2ModelResource::ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	//PrintBoneInfo();

	if(!CD3DObjectResource::ToSmartStruct(Packet,pResourceManager,Param))
		return false;	

	for(UINT i=0;i<m_AnimationSequence.GetCount();i++)
	{
		UINT BufferSize;
		void * pBuffer=Packet.PrepareMember(BufferSize);
		CSmartStruct SubPacket(pBuffer,BufferSize,true);
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_BAS_ANIMATION_ID,m_AnimationSequence[i].AnimationID));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_BAS_ANIMATION_NAME,(LPCTSTR)m_AnimationSequence[i].AnimationName));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_BAS_SUBANIMATION_ID,m_AnimationSequence[i].SubAnimationID));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_BAS_INDEX,m_AnimationSequence[i].Index));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_BAS_LENGTH,m_AnimationSequence[i].Length));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_BAS_MOVING_SPEED,m_AnimationSequence[i].MovingSpeed));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_BAS_FLAGS,m_AnimationSequence[i].Flags));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_BAS_FLAGS2,m_AnimationSequence[i].Flags2));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_BAS_PLAYBACK_SPEED,m_AnimationSequence[i].PlaybackSpeed));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_BAS_BOUNDING_BOX,(char *)&(m_AnimationSequence[i].BoundingBox),
			sizeof(m_AnimationSequence[i].BoundingBox)));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_BAS_BOUNDING_SPHERE,(char *)&(m_AnimationSequence[i].BoundingSphere),
			sizeof(m_AnimationSequence[i].BoundingSphere)));
		if(!Packet.FinishMember(SST_D3DWMMR_BONE_ANIMATION_SEQUENCE,SubPacket.GetDataLen()))
			return false;
	}
	for(UINT i=0;i<m_ColorAnimations.GetCount();i++)
	{
		UINT BufferSize;
		void * pBuffer=Packet.PrepareMember(BufferSize);
		CSmartStruct SubPacket(pBuffer,BufferSize,true);
		
		if(!PackAnimationBlock<CD3DVector3>(m_ColorAnimations[i].ColorAnimations,SubPacket,SST_CA_COLOR_ANIMATION))
			return false;
		
		if(!PackAnimationBlock<FLOAT>(m_ColorAnimations[i].AlphaAnimations,SubPacket,SST_CA_ALPHA_ANIMATION))
			return false;
			
		if(!Packet.FinishMember(SST_D3DWMMR_COLOR_ANIMATION,SubPacket.GetDataLen()))
			return false;
	}
	for(UINT i=0;i<m_TransparencyAnimations.GetCount();i++)
	{
		UINT BufferSize;
		void * pBuffer=Packet.PrepareMember(BufferSize);
		CSmartStruct SubPacket(pBuffer,BufferSize,true);		
			
		if(!PackAnimationBlock<FLOAT>(m_TransparencyAnimations[i].AlphaAnimations,SubPacket,SST_TA_ALPHA_ANIMATION))
			return false;
		
		if(!Packet.FinishMember(SST_D3DWMMR_TRANSPARENCY_ANIMATION,SubPacket.GetDataLen()))
			return false;
	}

	for(UINT i=0;i<m_TextureUVAnimations.GetCount();i++)
	{
		UINT BufferSize;
		void * pBuffer=Packet.PrepareMember(BufferSize);
		CSmartStruct SubPacket(pBuffer,BufferSize,true);		
		
		if(!PackAnimationBlock<CD3DVector3>(m_TextureUVAnimations[i].Translations,SubPacket,SST_TUVA_TRANSLATIONS))
			return false;
		
		if(!PackAnimationBlock<CD3DQuaternion>(m_TextureUVAnimations[i].Rotations,SubPacket,SST_TUVA_ROTATIONS))
			return false;
	
		if(!PackAnimationBlock<CD3DVector3>(m_TextureUVAnimations[i].Scalings,SubPacket,SST_TUVA_SCALINGS))
			return false;
			
		if(!Packet.FinishMember(SST_D3DWMMR_TEXTURE_UV_ANIMATION,SubPacket.GetDataLen()))
			return false;
	}

	for(UINT i=0;i<m_Bones.GetCount();i++)
	{
		UINT BufferSize;
		void * pBuffer=Packet.PrepareMember(BufferSize);
		CSmartStruct SubPacket(pBuffer,BufferSize,true);		

		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_BI_ANIMATION_SEQ,m_Bones[i].AnimationSeq));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_BI_FLAGS,m_Bones[i].Flags));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_BI_PARENT_BONE,m_Bones[i].ParentBone));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_BI_GEOSET_ID,m_Bones[i].GeosetID));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_BI_PIVOT_POINT,(char *)(&m_Bones[i].PivotPoint),
			sizeof(m_Bones[i].PivotPoint)));
		
		if(!PackAnimationBlock<CD3DVector3>(m_Bones[i].Translations,SubPacket,SST_BI_TRANSLATIONS))
			return false;
		
		if(!PackAnimationBlock<CD3DQuaternion>(m_Bones[i].Rotations,SubPacket,SST_BI_ROTATIONS))
			return false;
		
		if(!PackAnimationBlock<CD3DVector3>(m_Bones[i].Scalings,SubPacket,SST_BI_SCALINGS))
			return false;
			
		if(!Packet.FinishMember(SST_D3DWMMR_BONE,SubPacket.GetDataLen()))
			return false;
	}
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMMR_SKIN_MESH_BONE_COUNT,m_SkinMeshBoneCount));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMMR_KEY_BONE,
		(char *)m_KeyBoneIndex.GetBuffer(),m_KeyBoneIndex.GetCount()*sizeof(short)));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMMR_GLOBAL_SEQUENCE,
		(char *)m_GlobalSequences.GetBuffer(),m_GlobalSequences.GetCount()*sizeof(UINT)));
	for(UINT i=0;i<m_Attachments.GetCount();i++)
	{
		UINT BufferSize;
		void * pBuffer=Packet.PrepareMember(BufferSize);
		CSmartStruct SubPacket(pBuffer,BufferSize,true);	
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_AI_ID,m_Attachments[i].ID));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_AI_BONE,m_Attachments[i].Bone));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_AI_NAME,m_Attachments[i].Name));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_AI_POSITION,
			(char *)(&m_Attachments[i].Position),sizeof(m_Attachments[i].Position)));
		if(!Packet.FinishMember(SST_D3DWMMR_ATTACHMENT,SubPacket.GetDataLen()))
			return false;
	}

	for(UINT i=0;i<m_ParticleEmitters.GetCount();i++)
	{
		UINT BufferSize;
		void * pBuffer=Packet.PrepareMember(BufferSize);
		CSmartStruct SubPacket(pBuffer,BufferSize,true);	

		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_PEBI_MODEL_RESOURCE,
			m_ParticleEmitters[i].pModelResource->GetName()));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_PEBI_POSITION,
			(char *)(&m_ParticleEmitters[i].BindPosition),sizeof(m_ParticleEmitters[i].BindPosition)));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_PEBI_BONE,m_ParticleEmitters[i].BindBone));
		
		if(!Packet.FinishMember(SST_D3DWMMR_PARTICLE_EMITTER,SubPacket.GetDataLen()))
			return false;
	}

	for(UINT i=0;i<m_RibbonEmitters.GetCount();i++)
	{
		UINT BufferSize;
		void * pBuffer=Packet.PrepareMember(BufferSize);
		CSmartStruct SubPacket(pBuffer,BufferSize,true);	

		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_REBI_MODEL_RESOURCE,
			m_RibbonEmitters[i].pModelResource->GetName()));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_REBI_POSITION,
			(char *)(&m_RibbonEmitters[i].BindPosition),sizeof(m_RibbonEmitters[i].BindPosition)));
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_REBI_BONE,m_RibbonEmitters[i].BindBone));
		
		if(!Packet.FinishMember(SST_D3DWMMR_RIBBON_EMITTER,SubPacket.GetDataLen()))
			return false;
	}
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMMR_FLAG,m_Flag));
	return true;
}

bool CD3DWOWM2ModelResource::FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CD3DObjectResource::FromSmartStruct(Packet,pResourceManager,Param))
		return false;

	UINT AniSeqCount=0;
	UINT ColorAniCount=0;
	UINT TraAniCount=0;
	UINT UVAniCount=0;
	UINT BoneCount=0;	
	UINT AttachmentCount=0;
	UINT PECount=0;
	UINT RECount=0;
	void * Pos=Packet.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartValue Value=Packet.GetNextMember(Pos,MemberID);
		switch(MemberID)
		{	
		case SST_D3DWMMR_BONE_ANIMATION_SEQUENCE:
			AniSeqCount++;
			break;
		case SST_D3DWMMR_COLOR_ANIMATION:
			ColorAniCount++;
			break;
		case SST_D3DWMMR_TRANSPARENCY_ANIMATION:
			TraAniCount++;
			break;
		case SST_D3DWMMR_TEXTURE_UV_ANIMATION:
			UVAniCount++;
			break;
		case SST_D3DWMMR_BONE:
			BoneCount++;
			break;	
		case SST_D3DWMMR_ATTACHMENT:
			AttachmentCount++;
			break;
		case SST_D3DWMMR_PARTICLE_EMITTER:
			PECount++;
			break;
		case SST_D3DWMMR_RIBBON_EMITTER:
			RECount++;
			break;
		case SST_D3DWMMR_FLAG:
			m_Flag=Value;
			break;
		}
	}	
	m_AnimationSequence.Resize(AniSeqCount);
	m_ColorAnimations.Resize(ColorAniCount);
	m_TransparencyAnimations.Resize(TraAniCount);
	m_TextureUVAnimations.Resize(UVAniCount);
	m_Bones.Resize(BoneCount);	
	m_Attachments.Resize(AttachmentCount);
	m_ParticleEmitters.Resize(PECount);
	m_RibbonEmitters.Resize(RECount);

	AniSeqCount=0;
	ColorAniCount=0;
	TraAniCount=0;
	UVAniCount=0;
	BoneCount=0;	
	AttachmentCount=0;
	PECount=0;
	RECount=0;
	Pos=Packet.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartStruct Value=Packet.GetNextMember(Pos,MemberID);
		switch(MemberID)
		{		
		case SST_D3DWMMR_COLOR_ANIMATION:
			{
				CSmartStruct AniPacket=Value.GetMember(SST_CA_COLOR_ANIMATION);
				int AniCount=GetPacketAniBlockCount(AniPacket);
				m_ColorAnimations[ColorAniCount].ColorAnimations.Animations.Resize(AniCount);

				AniPacket=Value.GetMember(SST_CA_ALPHA_ANIMATION);				
				AniCount=GetPacketAniBlockCount(AniPacket);				
				m_ColorAnimations[ColorAniCount].AlphaAnimations.Animations.Resize(AniCount);
			}
			ColorAniCount++;
			break;
		case SST_D3DWMMR_TRANSPARENCY_ANIMATION:
			{
				CSmartStruct AniPacket=Value.GetMember(SST_TA_ALPHA_ANIMATION);
				int AniCount=GetPacketAniBlockCount(AniPacket);
				m_TransparencyAnimations[TraAniCount].AlphaAnimations.Animations.Resize(AniCount);
			}
			TraAniCount++;
			break;
		case SST_D3DWMMR_TEXTURE_UV_ANIMATION:
			{
				CSmartStruct AniPacket=Value.GetMember(SST_TUVA_TRANSLATIONS);
				int AniCount=GetPacketAniBlockCount(AniPacket);
				m_TextureUVAnimations[UVAniCount].Translations.Animations.Resize(AniCount);

				AniPacket=Value.GetMember(SST_TUVA_SCALINGS);
				AniCount=GetPacketAniBlockCount(AniPacket);
				m_TextureUVAnimations[UVAniCount].Rotations.Animations.Resize(AniCount);

				AniPacket=Value.GetMember(SST_TUVA_ROTATIONS);
				AniCount=GetPacketAniBlockCount(AniPacket);
				m_TextureUVAnimations[UVAniCount].Scalings.Animations.Resize(AniCount);
			}
			UVAniCount++;
			break;
		case SST_D3DWMMR_BONE:
			{
				CSmartStruct AniPacket=Value.GetMember(SST_BI_TRANSLATIONS);
				int AniCount=GetPacketAniBlockCount(AniPacket);
				m_Bones[BoneCount].Translations.Animations.Resize(AniCount);

				AniPacket=Value.GetMember(SST_BI_ROTATIONS);
				AniCount=GetPacketAniBlockCount(AniPacket);
				m_Bones[BoneCount].Rotations.Animations.Resize(AniCount);

				AniPacket=Value.GetMember(SST_BI_SCALINGS);
				AniCount=GetPacketAniBlockCount(AniPacket);
				m_Bones[BoneCount].Scalings.Animations.Resize(AniCount);
			}
			BoneCount++;
			break;		
		case SST_D3DWMMR_PARTICLE_EMITTER:			
			PECount++;
			break;
		case SST_D3DWMMR_RIBBON_EMITTER:			
			RECount++;
			break;
		}
	}

	AniSeqCount=0;
	ColorAniCount=0;
	TraAniCount=0;
	UVAniCount=0;
	BoneCount=0;	
	AttachmentCount=0;
	PECount=0;
	RECount=0;
	Pos=Packet.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartValue Value=Packet.GetNextMember(Pos,MemberID);
		switch(MemberID)
		{	
		case SST_D3DWMMR_BONE_ANIMATION_SEQUENCE:
			{
				CSmartStruct SubPacket=Value;
				
				m_AnimationSequence[AniSeqCount].AnimationID=SubPacket.GetMember(SST_BAS_ANIMATION_ID);
				m_AnimationSequence[AniSeqCount].AnimationName=(LPCTSTR)SubPacket.GetMember(SST_BAS_ANIMATION_NAME);
				m_AnimationSequence[AniSeqCount].SubAnimationID=SubPacket.GetMember(SST_BAS_SUBANIMATION_ID);
				m_AnimationSequence[AniSeqCount].Index=SubPacket.GetMember(SST_BAS_INDEX);
				m_AnimationSequence[AniSeqCount].Length=SubPacket.GetMember(SST_BAS_LENGTH);
				m_AnimationSequence[AniSeqCount].MovingSpeed=SubPacket.GetMember(SST_BAS_MOVING_SPEED);
				m_AnimationSequence[AniSeqCount].Flags=SubPacket.GetMember(SST_BAS_FLAGS);
				m_AnimationSequence[AniSeqCount].Flags2=SubPacket.GetMember(SST_BAS_FLAGS2);
				m_AnimationSequence[AniSeqCount].PlaybackSpeed=SubPacket.GetMember(SST_BAS_PLAYBACK_SPEED);
				memcpy(&m_AnimationSequence[AniSeqCount].BoundingBox,(LPCSTR)SubPacket.GetMember(SST_BAS_BOUNDING_BOX),
					sizeof(m_AnimationSequence[AniSeqCount].BoundingBox));
				memcpy(&m_AnimationSequence[AniSeqCount].BoundingSphere,(LPCSTR)SubPacket.GetMember(SST_BAS_BOUNDING_SPHERE),
					sizeof(m_AnimationSequence[AniSeqCount].BoundingSphere));				
			}
			AniSeqCount++;
			break;
		case SST_D3DWMMR_COLOR_ANIMATION:
			{
				CSmartStruct SubPacket=Value;
				if(!UnpackAnimationBlock<CD3DVector3>(m_ColorAnimations[ColorAniCount].ColorAnimations,SubPacket,SST_CA_COLOR_ANIMATION))
					return false;
				if(!UnpackAnimationBlock<FLOAT>(m_ColorAnimations[ColorAniCount].AlphaAnimations,SubPacket,SST_CA_ALPHA_ANIMATION))
					return false;
			}
			ColorAniCount++;
			break;
		case SST_D3DWMMR_TRANSPARENCY_ANIMATION:
			{
				CSmartStruct SubPacket=Value;				
				if(!UnpackAnimationBlock<FLOAT>(m_TransparencyAnimations[TraAniCount].AlphaAnimations,SubPacket,SST_TA_ALPHA_ANIMATION))
					return false;
			}
			TraAniCount++;
			break;
		case SST_D3DWMMR_TEXTURE_UV_ANIMATION:
			{
				CSmartStruct SubPacket=Value;
				if(!UnpackAnimationBlock<CD3DVector3>(m_TextureUVAnimations[UVAniCount].Translations,SubPacket,SST_TUVA_TRANSLATIONS))
					return false;
				if(!UnpackAnimationBlock<CD3DQuaternion>(m_TextureUVAnimations[UVAniCount].Rotations,SubPacket,SST_TUVA_ROTATIONS))
					return false;
				if(!UnpackAnimationBlock<CD3DVector3>(m_TextureUVAnimations[UVAniCount].Scalings,SubPacket,SST_TUVA_SCALINGS))
					return false;
			}
			UVAniCount++;
			break;
		case SST_D3DWMMR_BONE:
			{
				CSmartStruct SubPacket=Value;
				m_Bones[BoneCount].AnimationSeq=SubPacket.GetMember(SST_BI_ANIMATION_SEQ);
				m_Bones[BoneCount].Flags=SubPacket.GetMember(SST_BI_FLAGS);
				m_Bones[BoneCount].ChildCount=0;
				m_Bones[BoneCount].ParentBone=SubPacket.GetMember(SST_BI_PARENT_BONE);
				if(m_Bones[BoneCount].ParentBone>=0)
				{
					m_Bones[m_Bones[BoneCount].ParentBone].ChildCount++;
				}
				m_Bones[BoneCount].GeosetID=SubPacket.GetMember(SST_BI_GEOSET_ID);
				memcpy(&m_Bones[BoneCount].PivotPoint,(LPCSTR)SubPacket.GetMember(SST_BI_PIVOT_POINT),
					sizeof(m_Bones[BoneCount].PivotPoint));
				if(!UnpackAnimationBlock<CD3DVector3>(m_Bones[BoneCount].Translations,SubPacket,SST_BI_TRANSLATIONS))
					return false;
				if(!UnpackAnimationBlock<CD3DQuaternion>(m_Bones[BoneCount].Rotations,SubPacket,SST_BI_ROTATIONS))
					return false;
				if(!UnpackAnimationBlock<CD3DVector3>(m_Bones[BoneCount].Scalings,SubPacket,SST_BI_SCALINGS))
					return false;
			}
			BoneCount++;
			break;
		case SST_D3DWMMR_SKIN_MESH_BONE_COUNT:
			m_SkinMeshBoneCount=Value;
			break;
		case SST_D3DWMMR_KEY_BONE:
			m_KeyBoneIndex.Resize(Value.GetLength()/sizeof(short));
			memcpy(m_KeyBoneIndex.GetBuffer(),(LPCSTR)Value,
				sizeof(short)*m_KeyBoneIndex.GetCount());
			break;
		case SST_D3DWMMR_GLOBAL_SEQUENCE:
			m_GlobalSequences.Resize(Value.GetLength()/sizeof(UINT));
			memcpy(m_GlobalSequences.GetBuffer(),(LPCSTR)Value,
				sizeof(UINT)*m_GlobalSequences.GetCount());
			break;
		case SST_D3DWMMR_ATTACHMENT:
			{
				CSmartStruct SubPacket=Value;
				m_Attachments[AttachmentCount].ID=SubPacket.GetMember(SST_AI_ID);
				m_Attachments[AttachmentCount].Bone=SubPacket.GetMember(SST_AI_BONE);
				m_Attachments[AttachmentCount].Name=(LPCTSTR)SubPacket.GetMember(SST_AI_NAME);				
				memcpy(&m_Attachments[AttachmentCount].Position,(LPCSTR)SubPacket.GetMember(SST_AI_POSITION),
					sizeof(m_Attachments[AttachmentCount].Position));				
			}
			AttachmentCount++;
			break;
		case SST_D3DWMMR_PARTICLE_EMITTER:
			{
				CSmartStruct SubPacket=Value;
				LPCTSTR szResourceName=SubPacket.GetMember(SST_PEBI_MODEL_RESOURCE);
				m_ParticleEmitters[PECount].pModelResource=(CD3DParticleEmitterResource *)pResourceManager->FindResource(szResourceName,GET_CLASS_INFO(CD3DParticleEmitterResource));
				if(m_ParticleEmitters[PECount].pModelResource)
					m_ParticleEmitters[PECount].pModelResource->AddUseRef();
				memcpy(&m_ParticleEmitters[PECount].BindPosition,(LPCSTR)SubPacket.GetMember(SST_PEBI_POSITION),
					sizeof(m_ParticleEmitters[PECount].BindPosition));
				m_ParticleEmitters[PECount].BindBone=SubPacket.GetMember(SST_PEBI_BONE);						
			}
			PECount++;
			break;
		case SST_D3DWMMR_RIBBON_EMITTER:
			{
				CSmartStruct SubPacket=Value;

				LPCTSTR szResourceName=SubPacket.GetMember(SST_REBI_MODEL_RESOURCE);
				m_RibbonEmitters[RECount].pModelResource=(CD3DRibbonEmitterResource *)pResourceManager->FindResource(szResourceName,GET_CLASS_INFO(CD3DRibbonEmitterResource));
				if(m_RibbonEmitters[RECount].pModelResource)
					m_RibbonEmitters[RECount].pModelResource->AddUseRef();
				memcpy(&m_RibbonEmitters[RECount].BindPosition,(LPCSTR)SubPacket.GetMember(SST_REBI_POSITION),
					sizeof(m_RibbonEmitters[RECount].BindPosition));
				m_RibbonEmitters[RECount].BindBone=SubPacket.GetMember(SST_REBI_BONE);				
			
			}
			RECount++;
			break;
		}
	}	
	CheckAni();
	//PrintBoneInfo();
	BuildSubMeshData();
	
	return true;
}

UINT CD3DWOWM2ModelResource::GetSmartStructSize(UINT Param)
{
	UINT Size=CD3DObjectResource::GetSmartStructSize(Param);	

	for(UINT i=0;i<m_AnimationSequence.GetCount();i++)
	{
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_AnimationSequence[i].AnimationID));		
		Size += CSmartStruct::GetStringMemberSize(m_AnimationSequence[i].AnimationName.GetLength());
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_AnimationSequence[i].SubAnimationID));
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_AnimationSequence[i].Index));
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_AnimationSequence[i].Length));
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_AnimationSequence[i].MovingSpeed));
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_AnimationSequence[i].Flags));
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_AnimationSequence[i].Flags2));
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_AnimationSequence[i].PlaybackSpeed));
		Size += CSmartStruct::GetStringMemberSizeA(sizeof(m_AnimationSequence[i].BoundingBox));
		Size += CSmartStruct::GetStringMemberSizeA(sizeof(m_AnimationSequence[i].BoundingSphere));
		Size+=CSmartStruct::GetStructMemberSize(0);
	}
	for(UINT i=0;i<m_ColorAnimations.GetCount();i++)
	{
		Size+=GetAnimationBlockSmartStructSize<CD3DVector3>(m_ColorAnimations[i].ColorAnimations);
		Size+=GetAnimationBlockSmartStructSize<FLOAT>(m_ColorAnimations[i].AlphaAnimations);
		Size+=CSmartStruct::GetStructMemberSize(0);
	}
	for(UINT i=0;i<m_TransparencyAnimations.GetCount();i++)
	{
		Size+=GetAnimationBlockSmartStructSize<FLOAT>(m_TransparencyAnimations[i].AlphaAnimations);
		Size+=CSmartStruct::GetStructMemberSize(0);		
	}

	for(UINT i=0;i<m_TextureUVAnimations.GetCount();i++)
	{
		Size+=GetAnimationBlockSmartStructSize<CD3DVector3>(m_TextureUVAnimations[i].Translations);
		Size+=GetAnimationBlockSmartStructSize<CD3DQuaternion>(m_TextureUVAnimations[i].Rotations);
		Size+=GetAnimationBlockSmartStructSize<CD3DVector3>(m_TextureUVAnimations[i].Scalings);
		Size+=CSmartStruct::GetStructMemberSize(0);
	}

	for(UINT i=0;i<m_Bones.GetCount();i++)
	{
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_Bones[i].AnimationSeq));
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_Bones[i].Flags));
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_Bones[i].ParentBone));
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_Bones[i].GeosetID));
		Size += CSmartStruct::GetStringMemberSizeA(sizeof(m_Bones[i].PivotPoint));
		Size+=GetAnimationBlockSmartStructSize<CD3DVector3>(m_Bones[i].Translations);
		Size+=GetAnimationBlockSmartStructSize<CD3DQuaternion>(m_Bones[i].Rotations);
		Size+=GetAnimationBlockSmartStructSize<CD3DVector3>(m_Bones[i].Scalings);
		Size+=CSmartStruct::GetStructMemberSize(0);
	}

	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_SkinMeshBoneCount));
	Size += CSmartStruct::GetStringMemberSizeA(m_KeyBoneIndex.GetCount()*sizeof(short));
	Size += CSmartStruct::GetStringMemberSizeA(m_GlobalSequences.GetCount()*sizeof(UINT));

	for(UINT i=0;i<m_Attachments.GetCount();i++)
	{
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_Attachments[i].ID));
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_Attachments[i].Bone));
		Size += CSmartStruct::GetStringMemberSize(m_Attachments[i].Name.GetLength());
		Size += CSmartStruct::GetStringMemberSizeA(sizeof(m_Attachments[i].Position));
		Size+=CSmartStruct::GetStructMemberSize(0);
	}

	for(UINT i=0;i<m_ParticleEmitters.GetCount();i++)
	{
		Size += CSmartStruct::GetStringMemberSize(m_ParticleEmitters[i].pModelResource->GetNameLength());
		Size += CSmartStruct::GetStringMemberSizeA(sizeof(m_ParticleEmitters[i].BindPosition));
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_ParticleEmitters[i].BindBone));	
		
		Size+=CSmartStruct::GetStructMemberSize(0);
	}
	for(UINT i=0;i<m_RibbonEmitters.GetCount();i++)
	{
		Size += CSmartStruct::GetStringMemberSize(m_RibbonEmitters[i].pModelResource->GetNameLength());
		Size += CSmartStruct::GetStringMemberSizeA(sizeof(m_RibbonEmitters[i].BindPosition));
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_RibbonEmitters[i].BindBone));	
	
		Size+=CSmartStruct::GetStructMemberSize(0);
	}

	Size+=CSmartStruct::GetStructMemberSize(sizeof(m_Flag));
	return Size;
}	

bool CD3DWOWM2ModelResource::LoadSkin(LPCTSTR SkinFileName,CEasyArray<CD3DSubMesh *>& SubMeshList,BYTE * pModelData)
{
	
	IFileAccessor * pFile;

	pFile=CD3DWOWM2Model::CreateFileAccessor();
	if(pFile==NULL)
		return false;

	CEasyBuffer SkinData;

	if(!pFile->Open(SkinFileName,IFileAccessor::modeRead))
	{
		pFile->Release();
		return false;	
	}
	UINT FileSize=(UINT)pFile->GetSize();	
	SkinData.Create(FileSize);	
	pFile->Read(SkinData.GetBuffer(),FileSize);
	SkinData.SetUsedSize(FileSize);
	pFile->Release();


	SKIN_HEADER * pSkinHeader=(SKIN_HEADER *)SkinData.GetBuffer();
	if(pSkinHeader->Tag!=BLZ_SKIN_HEADER_TAG)
		return false;


	BLZ_M2_HEADER * pHeader=(BLZ_M2_HEADER *)pModelData;

	M2_MODEL_VERTEXT * pVertices=(M2_MODEL_VERTEXT *)(pModelData+pHeader->VerticesOffset);


	SKIN_SUB_MESH * pSubMesh=(SKIN_SUB_MESH *)((BYTE *)SkinData.GetBuffer()+pSkinHeader->SubmeshesOffset);



	SKIN_VERTEX_PROPERTIES * pVertexProperties=(SKIN_VERTEX_PROPERTIES *)((BYTE *)SkinData.GetBuffer()+pSkinHeader->PropertiesOffset);

	UINT SubMeshCount=pSkinHeader->SubmeshesCount;
	WORD * pIndices=(WORD *)((BYTE *)SkinData.GetBuffer()+pSkinHeader->IndicesOffset);
	WORD * pTriangles=(WORD *)((BYTE *)SkinData.GetBuffer()+pSkinHeader->TrianglesOffset);

	m_Vertices.Resize(pHeader->VerticesCount);
	for(UINT i=0;i<pHeader->VerticesCount;i++)
	{
		m_Vertices[i].Pos=BLZTranslationToD3D(pVertices[i].Pos);
		for(int b=0;b<MAX_VERTEX_BONE_BIND;b++)
		{
			m_Vertices[i].BoneWeight[b]=pVertices[i].BoneWeight[b]/255.0f;					
			m_Vertices[i].BoneID[b]=pVertices[i].BoneIndex[b];				
			if(IsAniBone(m_Vertices[i].BoneID[b])&&m_Vertices[i].BoneWeight[b]>0)
			{
				if(m_Vertices[i].BoneID[b]+1>(BYTE)m_SkinMeshBoneCount)
				{
					m_SkinMeshBoneCount=m_Vertices[i].BoneID[b]+1;
				}
			}
		}			
		m_Vertices[i].Normal=BLZTranslationToD3D(pVertices[i].Normal);

		m_Vertices[i].TextureCoord=pVertices[i].Tex;			

		m_Vertices[i].Diffuse=(((UINT)(pVertices[i].Unknow[0]*255))&0xFF)|
			(((UINT)(pVertices[i].Unknow[1]*255))<<8);
	}

	m_Indices.Resize(pSkinHeader->TrianglesCount);
	for(UINT i=0;i<pSkinHeader->TrianglesCount;i++)
	{
		UINT Triangle=(i/3)*3+(2-i%3);

		WORD Index=pTriangles[Triangle];
		if(Index>=pSkinHeader->IndicesCount)
		{
			PrintD3DLog(_T("Error Triangle"));
		}
		m_Indices[i]=pIndices[Index];
		if(m_Indices[i]>=pHeader->VerticesCount)
		{
			PrintD3DLog(_T("Error Indice"));
		}			
	}

	int TotalVertexCount=0;
	for(UINT i=0;i<SubMeshCount;i++)
	{		
		CD3DSubMesh * pD3DSubMesh=new CD3DSubMesh(m_pManager->GetDevice());

		bool HasSkinMeshAni=false;

		pD3DSubMesh->GetVertexFormat().FVF=D3DFVF_XYZB5|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_LASTBETA_UBYTE4;
		pD3DSubMesh->GetVertexFormat().VertexSize=sizeof(MODEL_VERTEXT);
		pD3DSubMesh->GetVertexFormat().IndexSize=sizeof(WORD);
		pD3DSubMesh->SetPrimitiveType(D3DPT_TRIANGLELIST);
		pD3DSubMesh->SetPrimitiveCount(pSubMesh[i].TrianglesCount/3);		
		pD3DSubMesh->SetIndexStart(pSubMesh[i].StartTriangle);
		pD3DSubMesh->SetIndexCount(pSubMesh[i].TrianglesCount);	
		pD3DSubMesh->SetVertexStart(pSubMesh[i].StartVertex);
		pD3DSubMesh->SetVertexCount(pSubMesh[i].VerticesCount);

		UINT StartVertex=pSubMesh[i].StartVertex;
		UINT EndVertex=pSubMesh[i].StartVertex+pSubMesh[i].VerticesCount-1;
		

		pD3DSubMesh->SetRenderBufferUsed(CD3DSubMesh::BUFFER_USE_CUSTOM);
		pD3DSubMesh->SetOrginDataBufferUsed(CD3DSubMesh::BUFFER_USE_CUSTOM);


		for(UINT j=pSubMesh[i].StartTriangle;j<pSubMesh[i].StartTriangle+pSubMesh[i].TrianglesCount;j++)
		{
			WORD Index=m_Indices[j];
			if(Index<StartVertex)
			{
				StartVertex=Index;
				PrintD3DDebugLog(_T("Index值有异常，修正Vertex范围"));
			}
			if(Index>EndVertex)
			{
				EndVertex=Index;
				PrintD3DDebugLog(_T("Index值有异常，修正Vertex范围"));
			}
			if(!HasSkinMeshAni)
			{
				for(int b=0;b<MAX_VERTEX_BONE_BIND;b++)
				{
					if(IsAniBone(m_Vertices[Index].BoneID[b]))
					{
						HasSkinMeshAni=true;
						break;
					}
				}			
			}
			//if(HasSkinMeshAni)
			//{
			//	break;
			//}
		}

		pD3DSubMesh->SetVertexStart(StartVertex);
		pD3DSubMesh->SetVertexCount(EndVertex-StartVertex+1);

		pD3DSubMesh->SetID(pSubMesh[i].ID+i*10000);
		CEasyString SubMeshName;
		int Part=pSubMesh[i].ID/100;
		int Type=pSubMesh[i].ID%100;
		SubMeshName.Format(_T("%02d-%02d(%d)"),Part,Type,i);
		pD3DSubMesh->SetName(SubMeshName);

		if(HasSkinMeshAni)
		{
			pD3DSubMesh->AddProperty(CD3DSubMesh::SMF_IS_ANI_MESH);
			m_HasSkinMeshAni=true;
		}

		SubMeshList.Add(pD3DSubMesh);
	}

	for(UINT i=0;i<m_SubMeshList.GetCount();i++)
	{
		MakeSubmeshMaterial(pModelData,(BYTE *)SkinData.GetBuffer(),i,m_SubMeshList[i]);		
	}

	BuildSubMeshData();

	return true;
}

void CD3DWOWM2ModelResource::MakeSubmeshMaterial(BYTE * pModelData,BYTE * pSkinData,UINT SubMeshIndex,CD3DSubMesh * pD3DSubMesh)
{	
	if(pModelData==NULL||pSkinData==NULL)
		return;

	BLZ_M2_HEADER * pHeader=(BLZ_M2_HEADER *)pModelData;


	

	WORD * pTexturesLookup=(WORD *)(pModelData+pHeader->TexLookupOffset);
	short * pTextureAnimLookup=(short *)(pModelData+pHeader->TexAnimLookupOffset);
	short * pTransparencylookup=(short *)(pModelData+pHeader->TransLookupOffset);
	short * pTextureUnitsLookup=(short *)(pModelData+pHeader->TexUnitsOffset);	


	M2_MODEL_TEXTURE * pTextures=(M2_MODEL_TEXTURE *)(pModelData+pHeader->TexturesOffset);
	M2_MODEL_ANIMATION_BLOCK * pTransparency=(M2_MODEL_ANIMATION_BLOCK *)(pModelData+pHeader->TransparencyOffset);
	M2_MODEL_RENDER_FLAG * pRenderFlags=(M2_MODEL_RENDER_FLAG *)(pModelData+pHeader->RenderFlagsOffset);


	SKIN_HEADER * pSkinHeader=(SKIN_HEADER *)pSkinData;


	SKIN_SUB_MESH * pSubMesh=(SKIN_SUB_MESH *)(pSkinData+pSkinHeader->SubmeshesOffset);

	SKIN_TEXTURE_UNIT * pTextureUnits=(SKIN_TEXTURE_UNIT *)(pSkinData+pSkinHeader->TextureUnitsOffset);

	BYTE * pVertexProperties=pSkinData+pSkinHeader->PropertiesOffset;


	
	D3DCOLORVALUE WhiteColor={1.0f,1.0f,1.0f,1.0f};
	D3DCOLORVALUE GrayColor={0.8f,0.8f,0.8f,1.0f};
	D3DCOLORVALUE BlackColor={0.0f,0.0f,0.0f,1.0f};

	pD3DSubMesh->GetMaterial().GetMaterial().Ambient=WhiteColor;
	pD3DSubMesh->GetMaterial().GetMaterial().Diffuse=WhiteColor;
	pD3DSubMesh->GetMaterial().GetMaterial().Specular=WhiteColor;
	pD3DSubMesh->GetMaterial().GetMaterial().Emissive=BlackColor;
	pD3DSubMesh->GetMaterial().GetMaterial().Power=40.0f;
	

	for(UINT i=0;i<pSkinHeader->TextureUnitsCount;i++)
	{		
		if(pTextureUnits[i].SubmeshIndex==SubMeshIndex)
		{			
			M2_MODEL_TEXTURE * pTexture=pTextures+pTexturesLookup[pTextureUnits[i].Texture];

			int RenderFlagsIndex=pTextureUnits[i].RenderFlags;

			UINT TextureType=pTexture->Type;
			UINT TextureFlag=pTexture->Flags;			

			UINT RenderFlag=pRenderFlags[RenderFlagsIndex].Flags;
			UINT BlendingMode=pRenderFlags[RenderFlagsIndex].BlendingMode;
			UINT MeshFlag=0;

			if(pTextureUnitsLookup[pTextureUnits[i].TexUnitNumber2]<0&&
				(RenderFlag&D3D_RENDER_FLAG_DISABLE_ZWRITE))
			{
				TextureFlag|=D3D_TEX_FLAG_ENV_MAP;
			}

			CD3DTexture * pTex=NULL;

			if(TextureType==D3D_TEXTURE_TYPE_DIRECT)
			{
				CEasyString TextureFileName=(char *)pModelData+pTexture->FileNameOffset;
				pTex=m_pManager->GetDevice()->GetTextureManager()->LoadTexture(TextureFileName);
			}
			short ColorAniIndex=pTextureUnits[i].ColorIndex;
			short TransparencyAniIndex=-1;
			short UVAniIndex=-1;
			if(m_ColorAnimations.GetCount())
			{
				if(pTextureUnits[i].Transparency<pHeader->TransLookupCount)
					TransparencyAniIndex=pTransparencylookup[pTextureUnits[i].Transparency];
			}
			if(m_TextureUVAnimations.GetCount())
			{
				if(pTextureUnits[i].TextureAnim<pHeader->TexAnimLookupCount)
					UVAniIndex=pTextureAnimLookup[pTextureUnits[i].TextureAnim];
			}

			if(UVAniIndex>=0)
				TextureFlag|=D3D_TEX_FLAG_UV_ANI;

			if(pHeader->ModelFlag&BLZ_M2_MODEL_FLAG_USE_VERTEX_COLOR)
			{
				if(pTextureUnits[i].Mode==2)
					MeshFlag|=D3D_MESH_FLAG_USE_VERTEX_ALPHA1;
			}
			


			UINT64 SubMeshProperty=0;

			UINT64 TextureProperty=0;

			TextureProperty|=((UINT64)TextureFlag<<TF_TEXTURE_FLAG_SHIFT)&TF_TEXTURE_FLAG_MASK;
			TextureProperty|=((UINT64)BlendingMode<<TF_BLEND_MODE_SHIFT)&TF_BLEND_MODE_MASK;
			TextureProperty|=((UINT64)TextureType<<TF_SKIN_TEXTURE_TYPE_SHIFT)&TF_SKIN_TEXTURE_TYPE_MASK;
			TextureProperty|=((UINT64)(UVAniIndex+1)<<TF_UV_ANI_INDEX_SHIFT)&TF_UV_ANI_INDEX_MASK;

			if(pD3DSubMesh->GetMaterial().GetTextureLayerCount()==0)
			{
				SubMeshProperty|=((UINT64)RenderFlag<<SMP_RENDER_FLAG_SHIFT)&SMP_RENDER_FLAG_MASK;
				SubMeshProperty|=((UINT64)BlendingMode<<SMP_BLEND_MODE_SHIFT)&SMP_BLEND_MODE_MASK;
				SubMeshProperty|=((UINT64)MeshFlag<<SMP_MESH_FLAG_SHIFT)&SMP_MESH_FLAG_MASK;
				SubMeshProperty|=((UINT64)(ColorAniIndex+1)<<SMP_COLOR_ANI_INDEX_SHIFT)&SMP_COLOR_ANI_INDEX_MASK;
				SubMeshProperty|=((UINT64)(TransparencyAniIndex+1)<<SMP_TRANSPARENCY_ANI_INDEX_SHIFT)&SMP_TRANSPARENCY_ANI_INDEX_MASK;
				if(BlendingMode>=D3D_BLEND_MODE_ALPHA_BLENDING)
				{
					SubMeshProperty|=CD3DSubMesh::SMF_IS_TRANSPARENT;
				}
				if(BlendingMode==D3D_BLEND_MODE_OPAQUE)
				{
					SubMeshProperty|=CD3DSubMesh::SMF_HAVE_COLLIDE;
				}
				pD3DSubMesh->AddProperty(SubMeshProperty);
			}
		
			CEasyStringA TexFXName;
			CEasyStringA MatFXName;
			TexFXName.Format("TexLay%d",pD3DSubMesh->GetMaterial().GetTextureLayerCount());
			MatFXName.Format("UVMatrix%d",pD3DSubMesh->GetMaterial().GetTextureLayerCount());
			pD3DSubMesh->GetMaterial().AddTexture(pTex,TextureProperty,TexFXName,MatFXName);			
			
		}
	}
	
	BuildFX(pD3DSubMesh);

	pD3DSubMesh->GetMaterial().CaculateHashCode();
	
}


void CD3DWOWM2ModelResource::CreateBounding()
{
	for(UINT i=0;i<m_SubMeshList.GetCount();i++)
	{		
		m_SubMeshList[i]->CreateBoundingBox();
		m_SubMeshList[i]->CreateBoundingSphere();
		if(i==0)
		{
			m_BoundingBox=m_SubMeshList[i]->GetBoundingBox();
			m_BoundingSphere=m_SubMeshList[i]->GetBoundingSphere();
		}
		else
		{
			m_BoundingBox+=m_SubMeshList[i]->GetBoundingBox();
			m_BoundingSphere+=m_SubMeshList[i]->GetBoundingSphere();
		}		
	}

}

bool CD3DWOWM2ModelResource::LoadBones(BYTE * pModelData,LPCTSTR szModelPath)
{
	BLZ_M2_HEADER * pHeader=(BLZ_M2_HEADER *)pModelData;
	M2_MODEL_BONE * pBones=(M2_MODEL_BONE *)(pModelData+pHeader->BonesOffset);	


	short * pBoneLookup=(short *)(pModelData+pHeader->BoneLookupOffset);
	short * pKeyBoneLookup=(short *)(pModelData+pHeader->KeyBoneLookupOffset);

	M2_MODEL_ANIMATION_SEQUENCE * pAnimationSequence=(M2_MODEL_ANIMATION_SEQUENCE *)(pModelData+pHeader->AnimationsOffset);
	
	DWORD * pGlobalSequences=(DWORD *)(pModelData+pHeader->GlobalSequencesOffset);

	m_GlobalSequences.Resize(pHeader->GlobalSequencesCount);
	for(UINT i=0;i<pHeader->GlobalSequencesCount;i++)
	{
		m_GlobalSequences[i]=pGlobalSequences[i];
	}
	
	m_AnimationSequence.Resize(pHeader->AnimationsCount);
	for(UINT i=0;i<pHeader->AnimationsCount;i++)
	{
		
		m_AnimationSequence[i].AnimationID=pAnimationSequence[i].AnimationID;
		m_AnimationSequence[i].SubAnimationID=pAnimationSequence[i].SubAnimationID;
		m_AnimationSequence[i].Index=i;
		m_AnimationSequence[i].Length=pAnimationSequence[i].Length;
		m_AnimationSequence[i].MovingSpeed=pAnimationSequence[i].MovingSpeed;
		m_AnimationSequence[i].Flags=pAnimationSequence[i].Flags;
		m_AnimationSequence[i].Flags2=pAnimationSequence[i].Flags2;
		m_AnimationSequence[i].PlaybackSpeed=pAnimationSequence[i].PlaybackSpeed;
		m_AnimationSequence[i].BoundingBox.m_Min=BLZTranslationToD3D(pAnimationSequence[i].BoundingBox[0]);
		m_AnimationSequence[i].BoundingBox.m_Max=BLZTranslationToD3D(pAnimationSequence[i].BoundingBox[1]);
		m_AnimationSequence[i].BoundingBox.Rebuild();
		m_AnimationSequence[i].BoundingSphere.m_Center=0.0f;
		m_AnimationSequence[i].BoundingSphere.m_Radius=pAnimationSequence[i].Radius;
		CBLZWOWDatabase::BLZ_DB_ANIMATION_DATA_RECORD * pData=CBLZWOWDatabase::GetInstance()->
			FindAnimationData(m_AnimationSequence[i].AnimationID);
		if(pData)
		{
			m_AnimationSequence[i].AnimationName=pData->Name;
		}		
	}
	
	m_Bones.Resize(pHeader->BonesCount);
	for(UINT i=0;i<pHeader->BonesCount;i++)
	{			
		
		m_Bones[i].AnimationSeq=pBones[i].AnimationSeq;
		m_Bones[i].Flags=pBones[i].Flags;
		m_Bones[i].ChildCount=0;
		m_Bones[i].ParentBone=pBones[i].ParentBone;
		if(m_Bones[i].ParentBone>=0)
		{
			m_Bones[m_Bones[i].ParentBone].ChildCount++;
		}
		m_Bones[i].GeosetID=pBones[i].GeosetID;
		m_Bones[i].PivotPoint=BLZTranslationToD3D(pBones[i].PivotPoint);

		m_Bones[i].Translations.InterpolationType=pBones[i].Translation.InterpolationType;
		m_Bones[i].Translations.GlobalSequenceID=pBones[i].Translation.GlobalSequenceID;
		m_Bones[i].Translations.Animations.Resize(pBones[i].Translation.TimestampsCount);

		m_Bones[i].Rotations.InterpolationType=pBones[i].Rotation.InterpolationType;
		m_Bones[i].Rotations.GlobalSequenceID=pBones[i].Rotation.GlobalSequenceID;
		m_Bones[i].Rotations.Animations.Resize(pBones[i].Rotation.TimestampsCount);

		m_Bones[i].Scalings.InterpolationType=pBones[i].Scaling.InterpolationType;
		m_Bones[i].Scalings.GlobalSequenceID=pBones[i].Scaling.GlobalSequenceID;
		m_Bones[i].Scalings.Animations.Resize(pBones[i].Scaling.TimestampsCount);

		//if(m_Bones[i].Translations.GlobalSequenceID>=0)
		//{
		//	PrintD3DLog("BoneInfo[%u].Translations.GlobalSequenceID=%u",i,m_Bones[i].Translations.GlobalSequenceID);
		//}
		//if(m_Bones[i].Rotations.GlobalSequenceID>=0)
		//{
		//	PrintD3DLog("BoneInfo[%u].Rotations.GlobalSequenceID=%u",i,m_Bones[i].Rotations.GlobalSequenceID);
		//}
		//if(m_Bones[i].Scalings.GlobalSequenceID>=0)
		//{
		//	PrintD3DLog("BoneInfo[%u].Scalings.GlobalSequenceID=%u",i,m_Bones[i].Scalings.GlobalSequenceID);
		//}

		for(UINT AniIndex=0;AniIndex<m_AnimationSequence.GetCount();AniIndex++)
		{
			if(m_AnimationSequence[AniIndex].Flags&BLZ_M2_ANIMATION_SEQUENCE_FLAG_HAVE_KEY_DATA)
			{	
				LoadBoneAnimation(pModelData,pModelData,m_Bones[i],pBones[i],AniIndex);
			}
		}

		
	}

	for(UINT i=0;i<m_AnimationSequence.GetCount();i++)
	{
		if((m_AnimationSequence[i].Flags&BLZ_M2_ANIMATION_SEQUENCE_FLAG_HAVE_KEY_DATA)==0)
		{
			LoadAnimationFromFile(pModelData,m_AnimationSequence[i].AnimationID,m_AnimationSequence[i].SubAnimationID,
				i,szModelPath,pBones);
		}
	}
	m_KeyBoneIndex.Resize(pHeader->KeyBoneLookupCount);
	for(UINT i=0;i<pHeader->KeyBoneLookupCount;i++)
	{
		m_KeyBoneIndex[i]=pKeyBoneLookup[i];
	}
		
	return true;
}

int CD3DWOWM2ModelResource::GetAnimationID(LPCTSTR AnimationName)
{
	for(UINT i=0;i<m_AnimationSequence.GetCount();i++)
	{
		if(_tcsicmp(m_AnimationSequence[i].AnimationName,AnimationName)==0)
		{
			return m_AnimationSequence[i].AnimationID;
		}
	}
	return -1;
}

CD3DWOWM2ModelResource::ANIMATION_SEQUENCE * CD3DWOWM2ModelResource::FindAnimationInfo(UINT AnimationID,UINT SubAnimationID)
{
	for(UINT i=0;i<m_AnimationSequence.GetCount();i++)
	{
		if(m_AnimationSequence[i].AnimationID==AnimationID&&m_AnimationSequence[i].SubAnimationID==SubAnimationID)
		{
			return &(m_AnimationSequence[i]);
		}
	}
	return NULL;
}

int CD3DWOWM2ModelResource::GetAnimationIndex(UINT AnimationID,UINT SubAnimationID)
{
	for(UINT i=0;i<m_AnimationSequence.GetCount();i++)
	{
		if(m_AnimationSequence[i].AnimationID==AnimationID&&m_AnimationSequence[i].SubAnimationID==SubAnimationID)
		{
			return i;
		}
	}
	return -1;
}

CD3DWOWM2ModelResource::ANIMATION_SEQUENCE * CD3DWOWM2ModelResource::GetAnimationInfo(UINT AnimationID,UINT SubAnimationID)
{
	for(UINT i=0;i<m_AnimationSequence.GetCount();i++)
	{
		if(m_AnimationSequence[i].AnimationID==AnimationID&&m_AnimationSequence[i].SubAnimationID==SubAnimationID)
		{
			return m_AnimationSequence.GetObject(i);
		}
	}
	return NULL;
}


UINT CD3DWOWM2ModelResource::GetAnimationCount()
{
	return m_AnimationSequence.GetCount();
}

CD3DWOWM2ModelResource::ANIMATION_SEQUENCE * CD3DWOWM2ModelResource::GetAnimationInfoByIndex(UINT AnimationIndex)
{
	return m_AnimationSequence.GetObject(AnimationIndex);
}


UINT CD3DWOWM2ModelResource::GetBoneCount()
{
	return m_Bones.GetCount();
}

UINT CD3DWOWM2ModelResource::GetSkinMeshBoneCount()
{
	return m_SkinMeshBoneCount;
}

CD3DWOWM2ModelResource::MODEL_BONE * CD3DWOWM2ModelResource::GetBoneInfo(UINT Index)
{
	if(Index<m_Bones.GetCount())
	{
		return &(m_Bones[Index]);
	}
	return NULL;
}

int CD3DWOWM2ModelResource::GetKeyBone(UINT Key)
{
	if(Key<m_KeyBoneIndex.GetCount())
	{
		return m_KeyBoneIndex[Key];
	}
	return -1;
}

UINT CD3DWOWM2ModelResource::GetKeyBoneCount()
{
	return m_Bones.GetCount();
}


bool CD3DWOWM2ModelResource::MakeAnimationBoneFrame(UINT AnimationIndex,UINT Time,bool IsLoop,BONE_MATRIX * pMatrixBuffer,UINT MatrixBufferSize,bool ResetMatrix)
{	
	if(MatrixBufferSize<m_Bones.GetCount())
		return false;


	ANIMATION_SEQUENCE * pAniInfo=m_AnimationSequence.GetObject(AnimationIndex);

	if(pAniInfo==NULL)
		return false;

	for(UINT i=0;i<m_Bones.GetCount();i++)
	{

		bool IsAnimated=MakeOneAnimationBoneFrame(pAniInfo,Time,IsLoop,i,pMatrixBuffer+i,true);

		
		if(ResetMatrix&&(!IsAnimated))
		{
			pMatrixBuffer[i].Matrix.SetIdentity();
		}
		
		
	}
	
	return true;
}

bool CD3DWOWM2ModelResource::MakeAnimationBoneFrameByTree(UINT AnimationIndex,UINT Time,bool IsLoop,UINT StartBone,BONE_MATRIX * pMatrixBuffer,UINT MatrixBufferSize,bool ResetMatrix)
{

	ANIMATION_SEQUENCE * pAniInfo=m_AnimationSequence.GetObject(AnimationIndex);

	if(pAniInfo==NULL)
		return false;

	return MakeAnimationBoneFrameByTree(pAniInfo,Time,IsLoop,StartBone,pMatrixBuffer,MatrixBufferSize,ResetMatrix);
}


bool CD3DWOWM2ModelResource::MakeAnimationBoneFrameByTree(ANIMATION_SEQUENCE * pAniInfo,UINT Time,bool IsLoop,UINT StartBone,BONE_MATRIX * pMatrixBuffer,UINT MatrixBufferSize,bool ResetMatrix)
{
	if(StartBone>=m_Bones.GetCount()||MatrixBufferSize<GetBoneCount())
		return false;

	bool IsAnimated=MakeOneAnimationBoneFrame(pAniInfo,Time,IsLoop,StartBone,pMatrixBuffer+StartBone,false);


	if(ResetMatrix&&(!IsAnimated))
	{
		pMatrixBuffer[StartBone].Matrix.SetIdentity();
	}	

	for(UINT i=0;i<m_Bones.GetCount();i++)
	{
		if(m_Bones[i].ParentBone==StartBone)
		{
			MakeAnimationBoneFrameByTree(pAniInfo,Time,IsLoop,i,pMatrixBuffer,MatrixBufferSize,ResetMatrix);
		}
	}
	return true;
}

bool CD3DWOWM2ModelResource::MakeColorAnimationFrame(UINT AniIndex,UINT Time,bool IsLoop,D3DCOLORVALUE& Color)
{
	if(AniIndex<m_ColorAnimations.GetCount())
	{
		CD3DVector3 ColorRGB(1.0f,1.0f,1.0f);
		FLOAT Alpha=1.0f;

		if(m_ColorAnimations[AniIndex].ColorAnimations.Animations.GetCount())
		{
			UINT AniLength=0;
			if(m_ColorAnimations[AniIndex].ColorAnimations.GlobalSequenceID>=0)
			{
				AniLength=m_GlobalSequences[m_ColorAnimations[AniIndex].ColorAnimations.GlobalSequenceID];
			}
			GetInterpolationValue(Time,IsLoop,
				m_ColorAnimations[AniIndex].ColorAnimations.InterpolationType,
				AniLength,
				m_ColorAnimations[AniIndex].ColorAnimations.Animations[0],
				ColorRGB);
		}

		if(m_ColorAnimations[AniIndex].AlphaAnimations.Animations.GetCount())
		{
			UINT AniLength=0;
			if(m_ColorAnimations[AniIndex].AlphaAnimations.GlobalSequenceID>=0)
			{
				AniLength=m_GlobalSequences[m_ColorAnimations[AniIndex].AlphaAnimations.GlobalSequenceID];
			}
			GetInterpolationValue(Time,IsLoop,
				m_ColorAnimations[AniIndex].AlphaAnimations.InterpolationType,
				AniLength,
				m_ColorAnimations[AniIndex].AlphaAnimations.Animations[0],
				Alpha);
		}
		
		Color.r=ColorRGB.x;
		Color.g=ColorRGB.y;
		Color.b=ColorRGB.z;
		Color.a=Alpha;
		
		return true;
		
	}
	return false;
}

bool CD3DWOWM2ModelResource::MakeTransparencyAnimationFrame(UINT AniIndex,UINT Time,bool IsLoop,FLOAT& Alpha)
{
	if(AniIndex<m_TransparencyAnimations.GetCount())
	{
		if(m_TransparencyAnimations[AniIndex].AlphaAnimations.Animations.GetCount())
		{
			UINT AniLength=0;
			if(m_TransparencyAnimations[AniIndex].AlphaAnimations.GlobalSequenceID>=0)
			{
				AniLength=m_GlobalSequences[m_TransparencyAnimations[AniIndex].AlphaAnimations.GlobalSequenceID];
			}
			return GetInterpolationValue(Time,IsLoop,
				m_TransparencyAnimations[AniIndex].AlphaAnimations.InterpolationType,
				AniLength,
				m_TransparencyAnimations[AniIndex].AlphaAnimations.Animations[0],
				Alpha);
		}
	}
	return false;
}


CD3DWOWM2ModelResource::MODEL_ATTACHMENT * CD3DWOWM2ModelResource::GetAttachment(UINT ID)
{
	for(UINT i=0;i<m_Attachments.GetCount();i++)
	{
		if(m_Attachments[i].ID==ID)
		{
			return &(m_Attachments[i]);
		}
	}
	return NULL;
}

UINT CD3DWOWM2ModelResource::GetAttachmentCount()
{
	return m_Attachments.GetCount();
}
CD3DWOWM2ModelResource::MODEL_ATTACHMENT * CD3DWOWM2ModelResource::GetAttachmentByIndex(UINT Index)
{
	if(Index<m_Attachments.GetCount())
	{
		return &(m_Attachments[Index]);
	}
	return NULL;
}

UINT CD3DWOWM2ModelResource::GetParticleEmitterCount()
{
	return m_ParticleEmitters.GetCount();
}
CD3DWOWM2ModelResource::PARTICLE_EMITTER_BIND_INFO * CD3DWOWM2ModelResource::GetParticleEmitterInfo(UINT Index)
{	
	return m_ParticleEmitters.GetObject(Index);	
}


UINT CD3DWOWM2ModelResource::GetRibbonEmitterCount()
{
	return m_RibbonEmitters.GetCount();
}

CD3DWOWM2ModelResource::RIBBON_EMITTER_BIND_INFO * CD3DWOWM2ModelResource::GetRibbonEmitterInfo(UINT Index)
{
	return m_RibbonEmitters.GetObject(Index);
}

UINT CD3DWOWM2ModelResource::GetCameraCount()
{
	return m_CameraInfos.GetCount();
}

CD3DWOWM2ModelResource::CAMERA_INFO * CD3DWOWM2ModelResource::GetCameraInfo(UINT Index)
{
	return m_CameraInfos.GetObject(Index);
}

bool CD3DWOWM2ModelResource::MakeTextureUVAniFrame(UINT Index,UINT Time,bool IsLoop,CD3DMatrix& Frame)
{
	if(Index<m_TextureUVAnimations.GetCount())
	{
		Frame.SetIdentity();

		GetInterpolationTransformGlobal(Time,IsLoop,Frame,
			m_TextureUVAnimations[Index].Translations,
			m_TextureUVAnimations[Index].Rotations,
			m_TextureUVAnimations[Index].Scalings,
			m_GlobalSequences,
			true);	
		Frame=CD3DMatrix::FromTranslation(-0.5f,-0.5f,-0.5f)*Frame*CD3DMatrix::FromTranslation(0.5f,0.5f,0.5f);
		return true;
	}
	return false;
}

void CD3DWOWM2ModelResource::SetBoneAniCache(UINT CacheSize,UINT CacheFreq)
{
	
	m_BoneAniCacheSize=CacheSize;
	m_BoneAniCacheFreq=CacheFreq;
	if(m_BoneAniCacheSize&&m_GlobalBoneAniCache.GetCount()==0)
	{
		BuildGlobalBoneAniCache();
	}
}

UINT CD3DWOWM2ModelResource::CaculateDataSize()
{
	UINT DataSize=0;
	for(UINT i=0;i<m_SubMeshList.GetCount();i++)
	{
		DataSize+=m_SubMeshList[i]->GetVertexFormat().VertexSize*m_SubMeshList[i]->GetVertexCount();
		DataSize+=m_SubMeshList[i]->GetVertexFormat().IndexSize*m_SubMeshList[i]->GetIndexCount();
	}
	for(UINT i=0;i<m_AnimationSequence.GetCount();i++)
	{
		DataSize+=sizeof(ANIMATION_SEQUENCE)+m_AnimationSequence[i].AnimationName.GetBufferSize();
	}
	for(UINT i=0;i<m_ColorAnimations.GetCount();i++)
	{
		DataSize+=CaculateAnimationBlockSize<FLOAT>(m_ColorAnimations[i].AlphaAnimations);
		DataSize+=CaculateAnimationBlockSize<CD3DVector3>(m_ColorAnimations[i].ColorAnimations);		
	}
	for(UINT i=0;i<m_TransparencyAnimations.GetCount();i++)
	{
		DataSize+=CaculateAnimationBlockSize<FLOAT>(m_TransparencyAnimations[i].AlphaAnimations);
	}
	for(UINT i=0;i<m_TextureUVAnimations.GetCount();i++)
	{
		DataSize+=CaculateAnimationBlockSize<CD3DVector3>(m_TextureUVAnimations[i].Scalings);
		DataSize+=CaculateAnimationBlockSize<CD3DQuaternion>(m_TextureUVAnimations[i].Rotations);
		DataSize+=CaculateAnimationBlockSize<CD3DVector3>(m_TextureUVAnimations[i].Translations);
	}

	for(UINT i=0;i<m_Bones.GetCount();i++)
	{
		DataSize+=sizeof(MODEL_BONE);
		DataSize+=CaculateAnimationBlockSize<CD3DVector3>(m_Bones[i].Scalings);
		DataSize+=CaculateAnimationBlockSize<CD3DQuaternion>(m_Bones[i].Rotations);
		DataSize+=CaculateAnimationBlockSize<CD3DVector3>(m_Bones[i].Translations);
	}
	DataSize+=m_KeyBoneIndex.GetCount()*sizeof(short);
	DataSize+=m_GlobalSequences.GetCount()*sizeof(UINT);
	for(UINT i=0;i<m_Attachments.GetCount();i++)
	{
		DataSize+=sizeof(MODEL_ATTACHMENT)+m_Attachments[i].Name.GetBufferSize();
	}
	return DataSize;
}

bool CD3DWOWM2ModelResource::LoadAnimationFromFile(BYTE * pData,UINT AnimationID,UINT SubAnimationID,UINT AniIndex,LPCTSTR szModelPath,M2_MODEL_BONE * pOrgBoneInfo)
{
	CEasyString AniFileName;
	AniFileName.Format(_T("%s%04d-%02d.anim"),szModelPath,AnimationID,SubAnimationID);

	IFileAccessor * pFile;


	CEasyBuffer AniData;

	pFile=CD3DWOWM2Model::CreateFileAccessor();
	if(pFile==NULL)
		return false;
	if(!pFile->Open(AniFileName,IFileAccessor::modeRead))
	{
		PrintD3DLog(_T("打开外部动画文件%s失败"),(LPCTSTR)AniFileName);
		pFile->Release();
		return false;	
	}
	UINT FileSize=(UINT)pFile->GetSize();	
	AniData.Create(FileSize);	
	pFile->Read(AniData.GetBuffer(),FileSize);
	AniData.SetUsedSize(FileSize);
	pFile->Release();

	for(UINT i=0;i<m_Bones.GetCount();i++)
	{
		LoadBoneAnimation(pData,(BYTE *)AniData.GetBuffer(),m_Bones[i],pOrgBoneInfo[i],AniIndex);
	}

	PrintD3DLog(_T("装载了外部动画文件%s"),(LPCTSTR)AniFileName);
	return true;
}

bool CD3DWOWM2ModelResource::LoadColorAnimation(BYTE * pModelData)
{
	BLZ_M2_HEADER * pHeader=(BLZ_M2_HEADER *)pModelData;

	M2_COLOR_ANIMATION * pColorAni=(M2_COLOR_ANIMATION *)(pModelData+pHeader->ColorsOffset);

	m_ColorAnimations.Resize(pHeader->ColorsCount);
	for(UINT i=0;i<pHeader->ColorsCount;i++)
	{
		LoadAniBlockVector3(pModelData,pColorAni[i].ColorAni,m_ColorAnimations[i].ColorAnimations);
		LoadAniBlockAlpha(pModelData,pColorAni[i].AlphaAni,m_ColorAnimations[i].AlphaAnimations);		
	}

	return true;

}

bool CD3DWOWM2ModelResource::LoadTransparencyAnimation(BYTE * pModelData)
{
	BLZ_M2_HEADER * pHeader=(BLZ_M2_HEADER *)pModelData;

	M2_TRANSPARENCY_ANIMATION * pTransparencyAni=(M2_TRANSPARENCY_ANIMATION *)(pModelData+pHeader->TransparencyOffset);

	m_TransparencyAnimations.Resize(pHeader->TransparencyCount);
	for(UINT i=0;i<pHeader->TransparencyCount;i++)
	{
		LoadAniBlockAlpha(pModelData,pTransparencyAni[i].AlphaAni,m_TransparencyAnimations[i].AlphaAnimations);		
	}

	return true;
}

bool CD3DWOWM2ModelResource::LoadRibbonEmitters(BYTE * pModelData)
{
	BLZ_M2_HEADER * pHeader=(BLZ_M2_HEADER *)pModelData;

	//PrintD3DLog("一共有%u个条带生成器",
	//	pHeader->RibbonEmittersCount);

	M2_RIBBON_EMITTER * pRibbonEmitters=(M2_RIBBON_EMITTER *)(pModelData+pHeader->RibbonEmittersOffset);

	WORD * pTexturesLookup=(WORD *)(pModelData+pHeader->TexLookupOffset);

	M2_MODEL_TEXTURE * pTextures=(M2_MODEL_TEXTURE *)(pModelData+pHeader->TexturesOffset);

	M2_MODEL_RENDER_FLAG * pRenderFlags=(M2_MODEL_RENDER_FLAG *)(pModelData+pHeader->RenderFlagsOffset);


	m_RibbonEmitters.Resize(pHeader->RibbonEmittersCount);
	for(UINT i=0;i<pHeader->RibbonEmittersCount;i++)
	{
		CEasyString Name;
		m_RibbonEmitters[i].pModelResource=new CD3DRibbonEmitterResource(m_pManager);
		Name.Format(_T("%s_RE_%d"),GetName(),i);
		m_RibbonEmitters[i].pModelResource->SetName(Name);
		m_RibbonEmitters[i].pModelResource->LoadM2RibbonEmitter(i,pRibbonEmitters+i,pModelData,m_GlobalSequences);
		m_RibbonEmitters[i].BindBone=pRibbonEmitters[i].BoneID;
		m_RibbonEmitters[i].BindPosition=BLZTranslationToD3D(pRibbonEmitters[i].Position);
	}

	return true;
}
bool CD3DWOWM2ModelResource::LoadParticleEmitters(BYTE * pModelData)
{
	BLZ_M2_HEADER * pHeader=(BLZ_M2_HEADER *)pModelData;

	//PrintD3DLog("一共有%u个粒子生成器",
	//	pHeader->ParticleEmittersCount);

	int size=sizeof(M2_PARTICLE_EMITTER);

	M2_PARTICLE_EMITTER * pParticleEmitters=(M2_PARTICLE_EMITTER *)(pModelData+pHeader->ParticleEmittersOffset);


	m_ParticleEmitters.Resize(pHeader->ParticleEmittersCount);
	for(UINT i=0;i<pHeader->ParticleEmittersCount;i++)
	{
		CEasyString Name;
		//PrintD3DLog("Particle%u,Flag=0x%X",i,pParticleEmitters[i].Flags);
		m_ParticleEmitters[i].pModelResource=new CD3DParticleEmitterResource(m_pManager);
		Name.Format(_T("%s_PE_%d"),GetName(),i);
		m_ParticleEmitters[i].pModelResource->SetName(Name);
		m_ParticleEmitters[i].pModelResource->LoadM2ParticleEmitter(i,pParticleEmitters+i,pModelData,m_GlobalSequences);
		m_ParticleEmitters[i].BindPosition=BLZTranslationToD3D(pParticleEmitters[i].Position);
		m_ParticleEmitters[i].BindBone=pParticleEmitters[i].Bone;
		
	}

	return true;
}

bool CD3DWOWM2ModelResource::LoadTextureUVAnimation(BYTE * pModelData)
{
	BLZ_M2_HEADER * pHeader=(BLZ_M2_HEADER *)pModelData;

	//PrintD3DLog("一共有%u个纹理动画",
	//	pHeader->TexAnimsCount);

	M2_TEXTURE_UV_ANIMATION * pTexAnims=(M2_TEXTURE_UV_ANIMATION *)(pModelData+pHeader->TexAnimsOffset);

	m_TextureUVAnimations.Resize(pHeader->TexAnimsCount);
	for(UINT i=0;i<pHeader->TexAnimsCount;i++)
	{
		LoadAniBlockVector3(pModelData,pTexAnims[i].Translation,m_TextureUVAnimations[i].Translations);
		LoadAniBlockQuaternion2(pModelData,pTexAnims[i].Rotation,m_TextureUVAnimations[i].Rotations);
		LoadAniBlockVector3(pModelData,pTexAnims[i].Scaling,m_TextureUVAnimations[i].Scalings);		
	}

	return true;
}

void CD3DWOWM2ModelResource::LoadBoneAnimation(BYTE * pData,BYTE * pAniData,MODEL_BONE& BoneInfo,M2_MODEL_BONE& OrgBoneInfo,UINT AniIndex)
{
	{
		M2_MODEL_ANIMATION_PAIR * pTimeStampPair=(M2_MODEL_ANIMATION_PAIR *)(pData+OrgBoneInfo.Translation.TimestampsOffset);
		M2_MODEL_ANIMATION_PAIR * pKeyPair=(M2_MODEL_ANIMATION_PAIR *)(pData+OrgBoneInfo.Translation.KeysOffset);
	

		if(AniIndex<OrgBoneInfo.Translation.TimestampsCount)
		{		

			if(pTimeStampPair[AniIndex].Count)
			{					
				LoadTranslationFrames(pAniData,pTimeStampPair[AniIndex].Count,pTimeStampPair[AniIndex].Offset,
					pKeyPair[AniIndex].Count,pKeyPair[AniIndex].Offset,BoneInfo.Translations.Animations[AniIndex]);						
			}				
		}
	}	


	{
		M2_MODEL_ANIMATION_PAIR * pTimeStampPair=(M2_MODEL_ANIMATION_PAIR *)(pData+OrgBoneInfo.Rotation.TimestampsOffset);
		M2_MODEL_ANIMATION_PAIR * pKeyPair=(M2_MODEL_ANIMATION_PAIR *)(pData+OrgBoneInfo.Rotation.KeysOffset);
	

		if(AniIndex<OrgBoneInfo.Rotation.TimestampsCount)
		{				

			if(pTimeStampPair[AniIndex].Count)
			{
				LoadRotationFrames(pAniData,pTimeStampPair[AniIndex].Count,pTimeStampPair[AniIndex].Offset,
					pKeyPair[AniIndex].Count,pKeyPair[AniIndex].Offset,BoneInfo.Rotations.Animations[AniIndex]);
			}				
		}
	}	

	{
		M2_MODEL_ANIMATION_PAIR * pTimeStampPair=(M2_MODEL_ANIMATION_PAIR *)(pData+OrgBoneInfo.Scaling.TimestampsOffset);
		M2_MODEL_ANIMATION_PAIR * pKeyPair=(M2_MODEL_ANIMATION_PAIR *)(pData+OrgBoneInfo.Scaling.KeysOffset);


		if(AniIndex<OrgBoneInfo.Scaling.TimestampsCount)
		{			

			if(pTimeStampPair[AniIndex].Count)
			{
				LoadScalingFrames(pAniData,pTimeStampPair[AniIndex].Count,pTimeStampPair[AniIndex].Offset,
					pKeyPair[AniIndex].Count,pKeyPair[AniIndex].Offset,BoneInfo.Scalings.Animations[AniIndex]);
			}				
		}
	}
}



//WORD CD3DWOWM2ModelResource::RebuildVertexIndex(CEasyArray<WORD>& VertexIndexList,WORD VertexIndex)
//{
//	for(UINT i=0;i<VertexIndexList.GetCount();i++)
//	{
//		if(VertexIndexList[i]==VertexIndex)
//		{
//			return i;
//		}
//	}
//	VertexIndexList.Add(VertexIndex);
//	return VertexIndexList.GetCount()-1;
//}

void CD3DWOWM2ModelResource::BuildSubMeshData()
{
	for(UINT i=0;i<m_SubMeshList.GetCount();i++)
	{
		m_SubMeshList[i]->SetVertices((BYTE *)m_Vertices.GetBuffer());
		m_SubMeshList[i]->SetIndices((BYTE *)m_Indices.GetBuffer());
	}
}

bool CD3DWOWM2ModelResource::LoadAttachments(BYTE * pModelData)
{
	if(pModelData==NULL)
		return false;

	BLZ_M2_HEADER * pHeader=(BLZ_M2_HEADER *)pModelData;

	M2_MODEL_ATTACHMENT * pAttachments=(M2_MODEL_ATTACHMENT *)(pModelData+pHeader->AttachmentsOffset);	
	short * pAttachmentLookup=(short *)(pModelData+pHeader->AttachLookupOffset);

	m_Attachments.Resize(pHeader->AttachmentsCount);
	for(UINT i=0;i<pHeader->AttachmentsCount;i++)
	{
	

		m_Attachments[i].ID=pAttachments[i].ID;
		m_Attachments[i].Bone=pAttachments[i].Bone;
		m_Attachments[i].Position=BLZTranslationToD3D(pAttachments[i].Position);
		if(m_Attachments[i].ID<CAI_MAX)
		{
			m_Attachments[i].Name=CHAR_ATTACHMENT_NAME[m_Attachments[i].ID];
		}
	}
	return true;
}


bool CD3DWOWM2ModelResource::LoadCameraInfos(BYTE * pModelData)
{
	if(pModelData==NULL)
		return false;

	BLZ_M2_HEADER * pHeader=(BLZ_M2_HEADER *)pModelData;

	M2_CAMERA_INFO * pCameraInfos=(M2_CAMERA_INFO *)(pModelData+pHeader->CamerasOffset);	
	short * pCameraLookup=(short *)(pModelData+pHeader->CameraLookupOffset);

	m_CameraInfos.Resize(pHeader->CamerasCount);
	for(UINT i=0;i<pHeader->CamerasCount;i++)
	{
		m_CameraInfos[i].Type=pCameraInfos[i].Type;
		//m_CameraInfos[i].FOV=pCameraInfos[i].FOV*35*D3DX_PI/180;
		m_CameraInfos[i].FarClipping=pCameraInfos[i].FarClipping;
		m_CameraInfos[i].NearClipping=pCameraInfos[i].NearClipping;
		m_CameraInfos[i].Position=BLZTranslationToD3D(pCameraInfos[i].Position);
		m_CameraInfos[i].Target=BLZTranslationToD3D(pCameraInfos[i].Target);

		LoadAniBlockTranslation(pModelData,pCameraInfos[i].TranslationPos,m_CameraInfos[i].TranslationPos);
		LoadAniBlockTranslation(pModelData,pCameraInfos[i].TranslationTar,m_CameraInfos[i].TranslationTar);
		LoadAniBlockScaling(pModelData,pCameraInfos[i].Scaling,m_CameraInfos[i].Scaling);
		LoadAniBlockFloat(pModelData,pCameraInfos[i].FOV,m_CameraInfos[i].FOV);

		for(UINT j=0;j<m_CameraInfos[i].FOV.Animations.GetCount();j++)
		{
			for(UINT k=0;k<m_CameraInfos[i].FOV.Animations[j].Keys.GetCount();k++)
			{
				m_CameraInfos[i].FOV.Animations[j].Keys[k]=m_CameraInfos[i].FOV.Animations[j].Keys[k]*35*D3DX_PI/180;
			}
		}
	}

	return true;
}

void CD3DWOWM2ModelResource::BuildFX(CD3DSubMesh * pSubMesh)
{
	CEasyString  FXName;
	CEasyStringA PSDiffuseFunction;
	CEasyStringA VSDiffuseFunction;
	CEasyStringA EnableZWrite;
	CEasyStringA EnableFog;
	CEasyStringA CullMode;
	CEasyStringA EnableAlphaBlend;
	CEasyStringA BlendOp;
	CEasyStringA SrcBlend;
	CEasyStringA DestBlend;
	CEasyStringA EnableAlphaTest;
	CEasyStringA VertexAlphaOperation;
	CEasyStringA VShader;	
	CEasyStringA TextureAddrU[4];
	CEasyStringA TextureAddrV[4];
	CEasyStringA TextureUVAni;
	CEasyStringA OtherPSOperation;
	CEasyStringA Temp;

	UINT64 SubMeshProperty=pSubMesh->GetProperty();
	UINT64 RenderFlag=(SubMeshProperty&SMP_RENDER_FLAG_MASK)>>SMP_RENDER_FLAG_SHIFT;
	UINT64 BlendMode=(SubMeshProperty&SMP_BLEND_MODE_MASK)>>SMP_BLEND_MODE_SHIFT;
	UINT64 MeshFlag=(SubMeshProperty&SMP_MESH_FLAG_MASK)>>SMP_MESH_FLAG_SHIFT;
	
	FXName.Format(_T("M2Model\\FX%02llX_%02llX_%02llX"),RenderFlag,BlendMode,MeshFlag);
	
	if(SubMeshProperty&CD3DSubMesh::SMF_IS_ANI_MESH)
	{
		if(m_SkinMeshBoneCount>MAX_SHADER_BONE_NUM)
		{
			FXName+="_SoftBone";
			VShader="VShaderNormal";
		}		
		else
		{
			VShader="VShaderWithSkinMesh";
		}
	}
	else
	{
		FXName+="_NoBone";
		VShader="VShaderNormal";
	}
	
	if(RenderFlag&D3D_RENDER_FLAG_NO_LIGHT)
	{		
		PSDiffuseFunction=" * Input.Diffuse.a";
		VSDiffuseFunction="1.0f";
	}
	else
	{
		PSDiffuseFunction=" * Input.Diffuse";
		VSDiffuseFunction="CaculateDiffuse(Pos,Normal)";
	}	
	
	if(RenderFlag&D3D_RENDER_FLAG_NO_FOG)
	{
		EnableFog="False";
	}
	else
	{
		EnableFog="True";
	}
	if(RenderFlag&D3D_RENDER_FLAG_NO_CULL)
	{
		CullMode="None";
	}
	else
	{
		CullMode="CCW";
	}
	
	if(RenderFlag&D3D_RENDER_FLAG_DISABLE_ZWRITE)
	{		
		EnableZWrite="False";
	}
	else
	{		
		EnableZWrite="True";
	}

	EnableAlphaBlend="False";	
	EnableAlphaTest="False";
	BlendOp="Add";
	SrcBlend="SrcAlpha";
	DestBlend="InvSrcAlpha";

	switch(BlendMode)
	{	
	case D3D_BLEND_MODE_ALPHA_TEST:
		EnableAlphaTest="True";
		break;
	case D3D_BLEND_MODE_ALPHA_BLENDING:
		EnableAlphaBlend="True";
		break;
	case D3D_BLEND_MODE_ADDITIVE:
		EnableAlphaBlend="True";
		SrcBlend="One";
		DestBlend="One";
		break;
	case D3D_BLEND_MODE_ADDITIVE_ALPHA:
		EnableAlphaBlend="True";
		SrcBlend="SrcAlpha";
		DestBlend="One";
		break;
	case D3D_BLEND_MODE_MODULATE:		
		PrintD3DLog(_T("未处理的混合模式:D3D_BLEND_MODE_MODULATE[%s]"),GetName());
		break;
	case D3D_BLEND_MODE_MUL:
		PrintD3DLog(_T("未处理的混合模式:D3D_BLEND_MODE_MUL[%s]"),GetName());
		break;
	}
	if(MeshFlag&D3D_MESH_FLAG_USE_VERTEX_ALPHA1)
	{
		VertexAlphaOperation="Output.Diffuse.a =  Input.Diffuse.b;";
	}
	else if(MeshFlag&D3D_MESH_FLAG_USE_VERTEX_ALPHA2)
	{
		VertexAlphaOperation="Output.Diffuse.a =  Input.Diffuse.g;";
	}

	for(UINT i=0;i<4;i++)
	{
		TextureAddrU[i]="Clamp";
		TextureAddrV[i]="Clamp";
	}	
	for(UINT i=0;i<pSubMesh->GetMaterial().GetTextureLayerCount();i++)
	{
		UINT64 TextureProperty=pSubMesh->GetMaterial().GetTextureProperty(i);
		UINT64 TextureBlendMode=(TextureProperty&TF_BLEND_MODE_MASK)>>TF_BLEND_MODE_SHIFT;
		UINT64 TextureFlag=(TextureProperty&TF_TEXTURE_FLAG_MASK)>>TF_TEXTURE_FLAG_SHIFT;		


		Temp.Format("_TEX%02llX_%02llX",TextureBlendMode,TextureFlag);
		FXName+=(LPCSTR)Temp;
		if(TextureFlag&D3D_TEX_FLAG_WRAP_X)
		{
			TextureAddrU[i]="Wrap";
		}
		if(TextureFlag&D3D_TEX_FLAG_WRAP_Y)
		{
			TextureAddrV[i]="Wrap";
		}
		if(i)
		{			
			if(TextureFlag&D3D_TEX_FLAG_ENV_MAP)
			{
				Temp.Format("	float4 Color%d=texCUBE( Sampler%d, Input.EnvTex );",i,i);
				OtherPSOperation+=Temp;
				OtherPSOperation+="\r\n";				
			}
			else
			{
				Temp.Format("	float4 Color%d=tex2D( Sampler%d, Input.Tex0.xy );",i,i);
				OtherPSOperation+=Temp;
				OtherPSOperation+="\r\n";
			}
			
			switch(TextureBlendMode)
			{	
			case D3D_BLEND_MODE_ALPHA_TEST:
			case D3D_BLEND_MODE_ALPHA_BLENDING:
				Temp.Format("	Output.Color=Output.Color*(1-Color%d.a) + Color%d*Color%d.a;",i,i,i);
				OtherPSOperation+=Temp;
				OtherPSOperation+="\r\n";	
				break;
			case D3D_BLEND_MODE_ADDITIVE:	
				Temp.Format("	Output.Color=Output.Color + Color%d;",i);
				OtherPSOperation+=Temp;
				OtherPSOperation+="\r\n";	
				break;
			case D3D_BLEND_MODE_ADDITIVE_ALPHA:					
				Temp.Format("	Output.Color=Output.Color + Color%d*Color%d.a;",i,i);
				OtherPSOperation+=Temp;
				OtherPSOperation+="\r\n";	
				break;
			case D3D_BLEND_MODE_MODULATE:
				Temp.Format("	Output.Color=Output.Color*(1-Color%d.a) + Color%d*Color%d.a;",i,i,i);
				OtherPSOperation+=Temp;
				OtherPSOperation+="\r\n";	
				break;
			case D3D_BLEND_MODE_MUL:
				Temp.Format("	Output.Color=Output.Color*Color%d*2.0f;",i);
				OtherPSOperation+=Temp;
				OtherPSOperation+="\r\n";	
				break;
			}	
		}
		else
		{
			if(TextureFlag&D3D_TEX_FLAG_UV_ANI)
			{
				TextureUVAni="Output.Tex0.xy  = mul(float4(Input.Tex0.xy,0,1.0f),UVMatrix0)";
			}
			else
			{
				TextureUVAni="Output.Tex0.xy  = Input.Tex0.xy";
			}
		}
	}	

	CEasyStringA FxContent;
	
	FxContent=M2_MODEL_FX;	
	FxContent.Replace("<PSDiffuseFunction>",PSDiffuseFunction);
	FxContent.Replace("<VSDiffuseFunction>",VSDiffuseFunction);
	FxContent.Replace("<EnableZWrite>",EnableZWrite);
	FxContent.Replace("<EnableFog>",EnableFog);
	FxContent.Replace("<CullMode>",CullMode);
	FxContent.Replace("<EnableAlphaBlend>",EnableAlphaBlend);
	FxContent.Replace("<BlendOp>",BlendOp);
	FxContent.Replace("<SrcBlend>",SrcBlend);
	FxContent.Replace("<DestBlend>",DestBlend);
	FxContent.Replace("<EnableAlphaTest>",EnableAlphaTest);
	FxContent.Replace("<VertexAlphaOperation>",VertexAlphaOperation);
	FxContent.Replace("<TextureUVAni>",TextureUVAni);
	FxContent.Replace("<VShader>",VShader);	
	for(UINT i=0;i<4;i++)
	{		
		Temp.Format("<TextureAddrU%d>",i);
		FxContent.Replace(Temp,TextureAddrU[0]);
		Temp.Format("<TextureAddrV%d>",i);
		FxContent.Replace(Temp,TextureAddrV[0]);
	}
	FxContent.Replace("<OtherPSOperation>",OtherPSOperation);
	

	CD3DFX * pFX=m_pManager->GetDevice()->GetFXManager()->LoadFXFromMemory(FXName,FxContent.GetBuffer(),FxContent.GetLength());		
	pSubMesh->GetMaterial().SetFX(pFX);
}


bool CD3DWOWM2ModelResource::MakeOneAnimationBoneFrame(ANIMATION_SEQUENCE * pAniInfo,UINT Time,bool IsLoop,UINT Bone,BONE_MATRIX * pMatrix,bool UseCache)
{

	bool IsAnimated=false;


	if(Bone>=m_Bones.GetCount()||pMatrix==NULL)
		return IsAnimated;

	pMatrix->BoneID=Bone;
	pMatrix->ParentID=m_Bones[Bone].ParentBone;
	pMatrix->Flags=m_Bones[Bone].Flags;
	pMatrix->PivotPoint=m_Bones[Bone].PivotPoint;	

	if(pAniInfo)
	{			
		BONE_ANI_CACHE * pAniCache=NULL;
		if(m_BoneAniCacheSize&&UseCache)
		{
			pAniCache=FindBoneAniCache(pAniInfo->Index);
			if(pAniCache==NULL)
			{
				pAniCache=BuildBoneAniCache(pAniInfo);
			}
		}

		if(pAniCache)
		{
			IsAnimated=GetInterpolationTransformGlobal(Time,true,pMatrix->Matrix,
				m_Bones[Bone].Translations,m_Bones[Bone].Rotations,m_Bones[Bone].Scalings,m_GlobalSequences,false);
			if(!IsAnimated)			
			{
				IsAnimated=MakeOneAnimationBoneFrameFromCache(pAniCache,Time,IsLoop,Bone,pMatrix->Matrix);
			}
			
		}
		else
		{
			IsAnimated=GetInterpolationTransformWithGlobal(Time,IsLoop,pAniInfo->Index,pAniInfo->Length,pMatrix->Matrix,
				m_Bones[Bone].Translations,m_Bones[Bone].Rotations,m_Bones[Bone].Scalings,m_GlobalSequences);			
		}	
	}
	
	return IsAnimated;
}




bool CD3DWOWM2ModelResource::BuildGlobalBoneAniCache()
{	
	m_GlobalBoneAniCache.Clear();	
	UINT FrameTimeSpan=1000/m_BoneAniCacheFreq;
	if(m_GlobalSequences.GetCount())
	{
		m_GlobalBoneAniCache.Resize(m_GlobalSequences.GetCount());
		for(UINT i=0;i<m_Bones.GetCount();i++)
		{
			int GlobalSequenceID=-1;
			if(m_Bones[i].Translations.GlobalSequenceID>=0)
				GlobalSequenceID=m_Bones[i].Translations.GlobalSequenceID;
			if(m_Bones[i].Rotations.GlobalSequenceID>=0)
				GlobalSequenceID=m_Bones[i].Rotations.GlobalSequenceID;
			if(m_Bones[i].Scalings.GlobalSequenceID>=0)
				GlobalSequenceID=m_Bones[i].Scalings.GlobalSequenceID;

			if(GlobalSequenceID>=0)
			{
				BONE_ANI_CACHE& AniCache=m_GlobalBoneAniCache[GlobalSequenceID];
				AniCache.AniIndex=GlobalSequenceID;
				AniCache.Bones.Resize(AniCache.Bones.GetCount()+1);
				BONE_ANI_CACHE_FRAME& BoneFrames=AniCache.Bones[AniCache.Bones.GetCount()-1];
				BoneFrames.BoneID=i;
				UINT FrameCount=m_GlobalSequences[GlobalSequenceID]/FrameTimeSpan;
				BoneFrames.Matrix.Resize(FrameCount);
				for(UINT j=0;j<FrameCount;j++)
				{
					GetInterpolationTransformGlobal(FrameTimeSpan*j,false,BoneFrames.Matrix[j],
						m_Bones[i].Translations,
						m_Bones[i].Rotations,
						m_Bones[i].Scalings,
						m_GlobalSequences,
						false);
				}
			}
		}
	}	
	return true;
}

CD3DWOWM2ModelResource::BONE_ANI_CACHE * CD3DWOWM2ModelResource::BuildBoneAniCache(ANIMATION_SEQUENCE * pAniInfo)
{
	while(m_BoneAniCache.GetObjectCount()>m_BoneAniCacheSize)
	{
		m_BoneAniCache.DeleteObject(m_BoneAniCache.GetFirstObjectPos());
	}
	UINT FrameTimeSpan=1000/m_BoneAniCacheFreq;
	UINT FrameCount=ceil((float)pAniInfo->Length/FrameTimeSpan);
	BONE_ANI_CACHE * pAniCache=m_BoneAniCache.GetObject(m_BoneAniCache.PushBack());
	pAniCache->AniIndex=pAniInfo->Index;
	pAniCache->Bones.Resize(m_Bones.GetCount());
	for(UINT i=0;i<m_Bones.GetCount();i++)
	{		
		BONE_ANI_CACHE_FRAME& BoneFrames=pAniCache->Bones[i];
		BoneFrames.BoneID=i;			
		BoneFrames.Matrix.Resize(FrameCount);		
		for(UINT j=0;j<FrameCount;j++)
		{
			BoneFrames.Matrix[j].SetIdentity();
			UINT Time=FrameTimeSpan*j;
			if(Time>=pAniInfo->Length)
				Time=pAniInfo->Length-1;
			GetInterpolationTransform(Time,false,pAniInfo->Index,pAniInfo->Length,BoneFrames.Matrix[j],
				m_Bones[i].Translations,
				m_Bones[i].Rotations,
				m_Bones[i].Scalings);			
		}		
	}	
	return pAniCache;
}

CD3DWOWM2ModelResource::BONE_ANI_CACHE * CD3DWOWM2ModelResource::FindBoneAniCache(UINT AniIndex)
{
	void * Pos=m_BoneAniCache.GetFirstObjectPos();
	while(Pos)
	{
		BONE_ANI_CACHE * pAniCache=m_BoneAniCache.GetNext(Pos);
		if(pAniCache->AniIndex==AniIndex)
		{
			return pAniCache;
		}
	}
	return NULL;
}

bool CD3DWOWM2ModelResource::MakeOneAnimationBoneFrameFromCache(BONE_ANI_CACHE * pAniCache,UINT Time,bool IsLoop,UINT Bone,CD3DMatrix& TransformMatrix)
{
	if(Bone<pAniCache->Bones.GetCount())
	{
		BONE_ANI_CACHE_FRAME& BoneFrames=pAniCache->Bones[Bone];
		if(BoneFrames.Matrix.GetCount())
		{
			UINT FrameTimeSpan=1000/m_BoneAniCacheFreq;
			UINT Frame=Time/FrameTimeSpan;
			Frame=Frame%BoneFrames.Matrix.GetCount();
			TransformMatrix=BoneFrames.Matrix[Frame];			
			return true;
		}
	}
	return false;
}




void CD3DWOWM2ModelResource::CheckAni()
{
	m_HasBoneAni=false;
	for(UINT i=0;i<m_Bones.GetCount();i++)
	{		
		if(IsAniBone(i))
		{
			m_HasBoneAni=true;
			break;
		}
	}
	m_HasMaterialAni=false;
	for(UINT i=0;i<m_SubMeshList.GetCount();i++)
	{
		UINT64 SubMeshProperty=m_SubMeshList[i]->GetProperty();
		BYTE ColorAniIndex=(SubMeshProperty&CD3DWOWM2ModelResource::SMP_COLOR_ANI_INDEX_MASK)>>
			CD3DWOWM2ModelResource::SMP_COLOR_ANI_INDEX_SHIFT;
		BYTE TransparencyAniIndex=(SubMeshProperty&CD3DWOWM2ModelResource::SMP_TRANSPARENCY_ANI_INDEX_MASK)>>
			CD3DWOWM2ModelResource::SMP_TRANSPARENCY_ANI_INDEX_SHIFT;

		if(ColorAniIndex||TransparencyAniIndex)
		{
			m_HasMaterialAni=true;
			return;
		}

		CD3DSubMeshMaterial& Material=m_SubMeshList[i]->GetMaterial();
		
		if(Material.GetTextureLayerCount()>0)
		{			
			for(UINT j=0;j<Material.GetTextureLayerCount();j++)
			{
				UINT64 TextureProperty=Material.GetTextureProperty(j);
				BYTE UVAniIndex=(TextureProperty&CD3DWOWM2ModelResource::TF_UV_ANI_INDEX_MASK)>>
					CD3DWOWM2ModelResource::TF_UV_ANI_INDEX_SHIFT;
				if(UVAniIndex)
				{
					m_HasMaterialAni=true;
					return;
				}
			}
		}
	}
}

bool CD3DWOWM2ModelResource::IsAniBone(UINT Bone)
{
	if(Bone<m_Bones.GetCount())
	{
		if(m_Bones[Bone].Flags&(BONE_FLAG_BILLBOARD|BONE_FLAG_BILLBOARD_HORIZONTAL))
		{
			return true;
		}
		for(UINT j=0;j<m_Bones[Bone].Translations.Animations.GetCount();j++)
		{
			if(m_Bones[Bone].Translations.Animations[j].TimeStamps.GetCount())
			{
				return true;
			}			
		}
		for(UINT j=0;j<m_Bones[Bone].Rotations.Animations.GetCount();j++)
		{
			if(m_Bones[Bone].Rotations.Animations[j].TimeStamps.GetCount())
			{
				return true;
			}				
		}
		for(UINT j=0;j<m_Bones[Bone].Scalings.Animations.GetCount();j++)
		{
			if(m_Bones[Bone].Scalings.Animations[j].TimeStamps.GetCount())
			{
				return true;
			}				
		}		
	}
	return false;
}

void CD3DWOWM2ModelResource::CheckSkins(LPCTSTR szModelPath)
{
	IFileAccessor * pFile=CD3DWOWM2Model::CreateFileAccessor();
	if(pFile)
	{		
		CEasyString SkinFileName;
		for(UINT i=0;i<MAX_M2_SKIN;i++)
		{
			SkinFileName.Format(_T("%s%02d.skin"),(LPCTSTR)szModelPath,i);
			if(pFile->Open(SkinFileName,IFileAccessor::modeRead))
			{
				m_SkinFiles.Add(SkinFileName);
				pFile->Close();
			}

		}
		SAFE_RELEASE(pFile);
	}		
	
}

}

