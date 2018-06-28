/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWOWADTModelResource.cpp                               */
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


const WORD	ADT_HOLE_MASK[4][4]={	{0x1,	0x2,	0x4,	0x8},
									{0x10,	0x20,	0x40,	0x80},
									{0x100,	0x200,	0x400,	0x800},
									{0x1000,0x2000,	0x4000,	0x8000}};



IMPLEMENT_CLASS_INFO(CD3DWOWADTModelResource,CD3DObjectResource);

CD3DWOWADTModelResource::CD3DWOWADTModelResource(void)
{
	ZeroMemory(m_TerrainHeightInfo,sizeof(m_TerrainHeightInfo));
	ZeroMemory(m_TerrainHoleInfo,sizeof(m_TerrainHoleInfo));
	m_Position=0;
}

CD3DWOWADTModelResource::CD3DWOWADTModelResource(CD3DObjectResourceManager* pManager)
	:CD3DObjectResource(pManager)
{
	ZeroMemory(m_TerrainHeightInfo,sizeof(m_TerrainHeightInfo));
	ZeroMemory(m_TerrainHoleInfo,sizeof(m_TerrainHoleInfo));
	m_Position=0;
}

CD3DWOWADTModelResource::~CD3DWOWADTModelResource(void)
{	
	for(UINT i=0;i<m_M2ObjectList.GetCount();i++)
	{
		SAFE_RELEASE(m_M2ObjectList[i].pModelResource);
	}
	for(UINT i=0;i<m_WMOObjectList.GetCount();i++)
	{
		SAFE_RELEASE(m_WMOObjectList[i].pModelResource);
	}	
	m_M2ObjectList.Clear();
	m_WMOObjectList.Clear();
	Destory();
}

void CD3DWOWADTModelResource::Destory()
{
	CD3DObjectResource::Destory();
}

bool CD3DWOWADTModelResource::Reset()
{
	return CD3DObjectResource::Reset();
}

bool CD3DWOWADTModelResource::Restore()
{
	return CD3DObjectResource::Restore();
}

bool CD3DWOWADTModelResource::LoadFromFile(LPCTSTR ModelFileName,bool IsBigAlphaMask,bool BeLoadObject)
{
	

	Destory();

	if(!LoadTerrain(ModelFileName))
		return false;

	CEasyString FileName=ModelFileName;

	int Pos=FileName.ReverseFind('.');
	if(Pos<0)
		return false;
 
	FileName=FileName.Left(Pos);

	if(!LoadTextureInfo(FileName+"_tex0.adt",IsBigAlphaMask))
		return false;

	if(!LoadObjectsInfo(FileName+"_obj0.adt",BeLoadObject))
		return false;

	CreateBounding();
	return true;
}

UINT CD3DWOWADTModelResource::GetM2ObjectCount()
{
	return m_M2ObjectList.GetCount();
}
CD3DWOWADTModelResource::M2_OBJECT_INFO * CD3DWOWADTModelResource::GetM2ObjectInfo(UINT Index)
{
	return m_M2ObjectList.GetObject(Index);
}

UINT CD3DWOWADTModelResource::GetWMOObjectCount()
{
	return m_WMOObjectList.GetCount();
}
CD3DWOWADTModelResource::WMO_OBJECT_INFO * CD3DWOWADTModelResource::GetWMOObjectInfo(UINT Index)
{
	return m_WMOObjectList.GetObject(Index);
}	

bool CD3DWOWADTModelResource::GetHeightByXZ(FLOAT x,FLOAT z,FLOAT& Height,FLOAT& WaterHeight)
{
	if(x<m_Position.x||x>(m_Position.x+BLZ_ADT_MAP_AREA_SIZE)||
		z<m_Position.z-BLZ_ADT_MAP_AREA_SIZE||z>m_Position.z)
	{
		return false;
	}
	FLOAT dx=x-m_Position.x;
	FLOAT dz=m_Position.z-z;
	UINT Col=(UINT)(dx/BLZ_ADT_MAP_TILE_SIZE);
	UINT Row=(UINT)(dz/BLZ_ADT_MAP_TILE_SIZE);
	UINT BlockIndex=Row*BLZ_ADT_MAP_TILE_WIDTH+Col;
	if(BlockIndex>=BLZ_ADT_MAP_TILE_COUNT)
	{
		return false;
	}
	dx=dx-Col*BLZ_ADT_MAP_TILE_SIZE;
	dz=dz-Row*BLZ_ADT_MAP_TILE_SIZE;
	Col=(UINT)(dx/BLZ_ADT_MAP_TILE_BLOCK_SIZE);
	Row=(UINT)(dz/BLZ_ADT_MAP_TILE_BLOCK_SIZE);

	if((ADT_HOLE_MASK[Row/2][Col/2]&((WORD)m_TerrainHoleInfo[BlockIndex]))!=0)
	{
		return false;
	}

	FLOAT h1=m_TerrainHeightInfo[BlockIndex].TerrainHeight[Row*17+Col];
	FLOAT h2=m_TerrainHeightInfo[BlockIndex].TerrainHeight[Row*17+Col+1];
	FLOAT h3=m_TerrainHeightInfo[BlockIndex].TerrainHeight[(Row+1)*17+Col];
	FLOAT h4=m_TerrainHeightInfo[BlockIndex].TerrainHeight[(Row+1)*17+Col+1];
	FLOAT h5=m_TerrainHeightInfo[BlockIndex].TerrainHeight[Row*17+Col+9];

	FLOAT hmax=h1;
	hmax=hmax>h2?hmax:h2;
	hmax=hmax>h3?hmax:h3;
	hmax=hmax>h4?hmax:h4;
	hmax=hmax>h5?hmax:h5;
	hmax+=1;

	CD3DVector3 v1(0,h1,BLZ_ADT_MAP_TILE_BLOCK_SIZE);
	CD3DVector3 v2(BLZ_ADT_MAP_TILE_BLOCK_SIZE,h2,BLZ_ADT_MAP_TILE_BLOCK_SIZE);
	CD3DVector3 v3(0,h3,0);
	CD3DVector3 v4(BLZ_ADT_MAP_TILE_BLOCK_SIZE,h4,0);
	CD3DVector3 v5(BLZ_ADT_MAP_TILE_BLOCK_HALF_SIZE,h5,BLZ_ADT_MAP_TILE_BLOCK_HALF_SIZE);

	dx=dx-Col*BLZ_ADT_MAP_TILE_BLOCK_SIZE;
	dz=(Row+1)*BLZ_ADT_MAP_TILE_BLOCK_SIZE-dz;

	FLOAT f1=dx-dz;
	FLOAT f2=(BLZ_ADT_MAP_TILE_BLOCK_SIZE-dx)-dz;

	

	CD3DVector3 VOut;
	CD3DVector3 PTest(dx,hmax,dz);
	CD3DVector3 Dir(0,-1,0);
	FLOAT u,v,d;

	if(f1<0&&f2>=0)
	{	
		BOOL Ret=D3DXIntersectTri(&v3,&v1,&v5,&PTest,&Dir,&u,&v,&d);
		D3DXVec3BaryCentric(&VOut,&v3,&v1,&v5,u,v);
	}
	else if(f1<0&&f2<0)
	{
		BOOL Ret=D3DXIntersectTri(&v5,&v1,&v2,&PTest,&Dir,&u,&v,&d);
		D3DXVec3BaryCentric(&VOut,&v5,&v1,&v2,u,v);
	}
	else if(f1>=0&&f2<0)
	{
		BOOL Ret=D3DXIntersectTri(&v5,&v2,&v4,&PTest,&Dir,&u,&v,&d);
		D3DXVec3BaryCentric(&VOut,&v5,&v2,&v4,u,v);
	}
	else if(f1>=0&&f2>=0)
	{
		BOOL Ret=D3DXIntersectTri(&v3,&v5,&v4,&PTest,&Dir,&u,&v,&d);
		D3DXVec3BaryCentric(&VOut,&v3,&v5,&v4,u,v);
	}

	Height=VOut.y;
	DWORD NANValue=0xFFFFFFFF;
	WaterHeight=*((FLOAT *)&NANValue);
	if(m_TerrainHeightInfo[BlockIndex].HaveWater)
	{
		 h1=m_TerrainHeightInfo[BlockIndex].WaterHeight[Row*9+Col];
		 h2=m_TerrainHeightInfo[BlockIndex].WaterHeight[Row*9+Col+1];
		 h3=m_TerrainHeightInfo[BlockIndex].WaterHeight[(Row+1)*9+Col];
		 h4=m_TerrainHeightInfo[BlockIndex].WaterHeight[(Row+1)*9+Col+1];
		 if(_finite(h1)&&_finite(h2)&&_finite(h3)&&_finite(h4))
		 {
			 u=dx/BLZ_ADT_MAP_TILE_BLOCK_SIZE;
			 v=dz/BLZ_ADT_MAP_TILE_BLOCK_SIZE;
			 if(u>v)
			 {
				WaterHeight=h3+u*(h4-h3)+v*(h2-h4);
			 }
			 else
			 {
				 WaterHeight=h3+u*(h2-h1)+v*(h1-h3);
			 }
		 }
	}

	return true;
}

bool CD3DWOWADTModelResource::LoadTerrain(LPCTSTR ModelFileName)
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

	SetName(ModelFileName);

	CBLZChunkFile ADTChunk;

	if(!ADTChunk.Load(pFile))
	{
		pFile->Release();
		return false;
	}
	pFile->Release();	

	BLZ_CHUNK_VERSION * pVersion=(BLZ_CHUNK_VERSION *)ADTChunk.GetFirstChunk(CHUNK_ID_VERSION);

	BLZ_CHUNK_MHDR * pMHDR=(BLZ_CHUNK_MHDR *)ADTChunk.GetFirstChunk(CHUNK_ID_ADT_MHDR);

	UINT OffsetStart=sizeof(BLZ_CHUNK_VERSION)+sizeof(BLZ_CHUNK_HEADER);

	

	BLZ_CHUNK_MH2O * pMH2O=(BLZ_CHUNK_MH2O *)ADTChunk.GetChunkByOffset(CHUNK_ID_ADT_MH2O,OffsetStart+pMHDR->MH2OChunkOffset);

	BLZ_CHUNK_MFBO * pMFBO=(BLZ_CHUNK_MFBO *)ADTChunk.GetChunkByOffset(CHUNK_ID_ADT_MFBO,OffsetStart+pMHDR->MFBOChunkOffset);

	CEasyArray<CD3DSubMesh *> LiquidSubMeshList;

	CBLZChunkFile::CChunkList * pChunkList=ADTChunk.GetChunks(CHUNK_ID_ADT_MCNK);
	m_SubMeshList.Create(pChunkList->GetCount());
	
	for(UINT i=0;i<pChunkList->GetCount();i++)
	{
		CBLZChunkFile MCNKChunk;

		MCNKChunk.Load((BYTE *)((*pChunkList)[i]),((*pChunkList)[i])->ChunkSize+sizeof(BLZ_CHUNK_HEADER));

		BLZ_CHUNK_MCNK * pMCNK=(BLZ_CHUNK_MCNK *)MCNKChunk.GetFirstChunk(CHUNK_ID_ADT_MCNK);
		BLZ_CHUNK_MCVT * pMCVT=(BLZ_CHUNK_MCVT *)MCNKChunk.GetChunkByOffset(CHUNK_ID_ADT_MCVT,pMCNK->MCVTOffset);
		BLZ_CHUNK_MCCV * pMCCV=(BLZ_CHUNK_MCCV *)MCNKChunk.GetChunkByOffset(CHUNK_ID_ADT_MCCV,pMCNK->MCCVOffset);
		BLZ_CHUNK_MCNR * pMCNR=(BLZ_CHUNK_MCNR *)MCNKChunk.GetChunkByOffset(CHUNK_ID_ADT_MCNR,pMCNK->MCNROffset);	
		BLZ_CHUNK_MCLQ * pMCLQ=(BLZ_CHUNK_MCLQ *)MCNKChunk.GetChunkByOffset(CHUNK_ID_ADT_MCLQ,pMCNK->MCLQOffset);
	


		CD3DSubMesh * pD3DSubMesh=new CD3DSubMesh(m_pManager->GetDevice());


		UINT IndexCount=3*4*64;

		UINT VertexCount=145;

		pD3DSubMesh->GetVertexFormat().FVF=D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1;
		pD3DSubMesh->GetVertexFormat().VertexSize=sizeof(MODEL_VERTEXT);
		pD3DSubMesh->GetVertexFormat().IndexSize=sizeof(WORD);
		pD3DSubMesh->SetPrimitiveType(D3DPT_TRIANGLELIST);
		pD3DSubMesh->SetPrimitiveCount(IndexCount/3);		
		pD3DSubMesh->SetVertexCount(VertexCount);
		pD3DSubMesh->SetIndexCount(IndexCount);

		pD3DSubMesh->AllocDXIndexBuffer();
		pD3DSubMesh->AllocDXVertexBuffer();

		D3DCOLORVALUE WhiteColor={1.0f,1.0f,1.0f,1.0f};
		D3DCOLORVALUE GrayColor={0.8f,0.8f,0.8f,1.0f};
		D3DCOLORVALUE BlackColor={0.0f,0.0f,0.0f,1.0f};

		pD3DSubMesh->GetMaterial().GetMaterial().Ambient=WhiteColor;
		pD3DSubMesh->GetMaterial().GetMaterial().Diffuse=WhiteColor;
		pD3DSubMesh->GetMaterial().GetMaterial().Specular=WhiteColor;
		pD3DSubMesh->GetMaterial().GetMaterial().Emissive=BlackColor;
		pD3DSubMesh->GetMaterial().GetMaterial().Power=40.0f;

		


		WORD * pModelIndices=NULL;
		pD3DSubMesh->GetDXIndexBuffer()->Lock(0,0,(LPVOID *)&pModelIndices,0);

		MODEL_VERTEXT * pModelVertices=NULL;
		pD3DSubMesh->GetDXVertexBuffer()->Lock(0,0,(LPVOID *)&pModelVertices,0);


		CD3DVector3 Pos=pMCNK->Position;
		Pos.x=BLZ_ADT_MAP_TRANS_VALUE-Pos.x;
		Pos.y=BLZ_ADT_MAP_TRANS_VALUE-Pos.y;

		Pos=BLZTranslationToD3D(Pos);

		if(i==0)
			m_Position=Pos;

		m_TerrainHeightInfo[i].HaveWater=false;
		for(UINT h=0;h<TERRAIN_BLOCK_HEIGHT_COUNT;h++)
		{
			m_TerrainHeightInfo[i].TerrainHeight[h]=pMCVT->Heights[h]+Pos.y;
		}

		m_TerrainHoleInfo[i]=pMCNK->Holes;

		//CD3DSubMesh * pNormalSubMesh=new CD3DSubMesh;

		//pNormalSubMesh->GetVertexFormat().FVF=D3DFVF_XYZ|D3DFVF_DIFFUSE;
		//pNormalSubMesh->GetVertexFormat().VertexSize=sizeof(MODEL_NORMAL_VERTEXT);
		//pNormalSubMesh->SetVertexCount(VertexCount*2);
		//pNormalSubMesh->SetPrimitiveCount(VertexCount);
		//pNormalSubMesh->SetPrimitiveType(D3DPT_LINELIST);

		//pNormalSubMesh->AllocDXVertexBuffer(m_pManager->GetDevice());

		//pNormalSubMesh->GetMaterial().SetFX(
		//	m_pManager->GetDevice()->GetFXManager()->
		//	LoadFXFromMemory("DEFAULT_NORMAL_FX_NT",(void *)DEFAULT_NORMAL_FX_NT,
		//	(int)strlen(DEFAULT_NORMAL_FX_NT)));



		//MODEL_NORMAL_VERTEXT * pNormalVertices=NULL;
		//pNormalSubMesh->GetDXVertexBuffer()->Lock(0,0,(LPVOID *)&pNormalVertices,0);



		float Pitch=BLZ_ADT_MAP_TILE_SIZE/16;

		UINT VertexIndex=0;
		int Line=0;
		while(VertexIndex<145)
		{
			if(Line&1)
			{
				//8 vertices/line
				for(int x=0;x<8;x++)
				{
					pModelVertices[VertexIndex].Pos.x=Pitch+Pitch*x*2;
					pModelVertices[VertexIndex].Pos.z=-Pitch*Line;					
					pModelVertices[VertexIndex].Pos.y=pMCVT->Heights[VertexIndex];
					pModelVertices[VertexIndex].Pos+=Pos;

					CD3DVector3 Normal;
					Normal.z=pMCNR->Normals[VertexIndex][0]/127.0f;
					Normal.x=-pMCNR->Normals[VertexIndex][1]/127.0f;
					Normal.y=pMCNR->Normals[VertexIndex][2]/127.0f;
					pModelVertices[VertexIndex].Normal=Normal;

					pModelVertices[VertexIndex].Color=0xffffffff;

					pModelVertices[VertexIndex].Tex.x=(x*2.0f+1.0f)/16.0f;
					pModelVertices[VertexIndex].Tex.y=Line/16.0f;

					//pNormalVertices[VertexIndex*2].Pos=pModelVertices[VertexIndex].Pos;
					//pNormalVertices[VertexIndex*2].Color=0xFFFF0000;
					//pNormalVertices[VertexIndex*2+1].Pos=pModelVertices[VertexIndex].Pos+pModelVertices[VertexIndex].Normal*2;
					//pNormalVertices[VertexIndex*2+1].Color=0xFF0000FF;


					VertexIndex++;
				}

			}
			else
			{
				//9 vertices/line
				for(int x=0;x<9;x++)
				{
					pModelVertices[VertexIndex].Pos.x=Pitch*x*2;
					pModelVertices[VertexIndex].Pos.z=-Pitch*Line;
					pModelVertices[VertexIndex].Pos.y=pMCVT->Heights[VertexIndex];					
					pModelVertices[VertexIndex].Pos+=Pos;

					CD3DVector3 Normal;
					Normal.z=pMCNR->Normals[VertexIndex][0]/127.0f;
					Normal.x=-pMCNR->Normals[VertexIndex][1]/127.0f;
					Normal.y=pMCNR->Normals[VertexIndex][2]/127.0f;
					pModelVertices[VertexIndex].Normal=Normal;		

					pModelVertices[VertexIndex].Color=0xffffffff;

					pModelVertices[VertexIndex].Tex.x=(x*2.0f)/16.0f;
					pModelVertices[VertexIndex].Tex.y=Line/16.0f;


					//pNormalVertices[VertexIndex*2].Pos=pModelVertices[VertexIndex].Pos;
					//pNormalVertices[VertexIndex*2].Color=0xFFFF0000;
					//pNormalVertices[VertexIndex*2+1].Pos=pModelVertices[VertexIndex].Pos+pModelVertices[VertexIndex].Normal*2;
					//pNormalVertices[VertexIndex*2+1].Color=0xFF0000FF;


					VertexIndex++;
				}
			}
			Line++;
		}	

		

		UINT IndexIndex=0;
		for(int y=0;y<8;y++)
		{
			for(int x=0;x<8;x++)
			{
				if((ADT_HOLE_MASK[y/2][x/2]&((WORD)pMCNK->Holes))==0)
				{
					pModelIndices[IndexIndex]	=(y*8+(y+1)*9)+x;
					pModelIndices[IndexIndex+1]	=(y*8+(y+1)*9)+x+8;
					pModelIndices[IndexIndex+2]	=(y*8+(y+1)*9)+x-9;


					pModelIndices[IndexIndex+3]	=(y*8+(y+1)*9)+x;
					pModelIndices[IndexIndex+4]	=(y*8+(y+1)*9)+x+9;
					pModelIndices[IndexIndex+5]	=(y*8+(y+1)*9)+x+8;


					pModelIndices[IndexIndex+6]	=(y*8+(y+1)*9)+x;
					pModelIndices[IndexIndex+7]	=(y*8+(y+1)*9)+x-8;
					pModelIndices[IndexIndex+8]	=(y*8+(y+1)*9)+x+9;


					pModelIndices[IndexIndex+9]	=(y*8+(y+1)*9)+x;
					pModelIndices[IndexIndex+10]=(y*8+(y+1)*9)+x-9;
					pModelIndices[IndexIndex+11]=(y*8+(y+1)*9)+x-8;				

					IndexIndex+=3*4;
				}
			}
		}

		//pNormalSubMesh->GetDXVertexBuffer()->Unlock();

		pD3DSubMesh->SetIndexCount(IndexIndex);
		pD3DSubMesh->SetPrimitiveCount(IndexIndex/3);

		pD3DSubMesh->GetDXVertexBuffer()->Unlock();

		pD3DSubMesh->GetDXIndexBuffer()->Unlock();

		pD3DSubMesh->AddProperty(CD3DSubMesh::SMF_HAVE_COLLIDE);

		pD3DSubMesh->SetID(i);
		CEasyString SubMeshName;
		
		SubMeshName.Format(_T("%d"),i);
		pD3DSubMesh->SetName(SubMeshName);




		if(pMH2O)
		{
			m_TerrainHeightInfo[i].HaveWater=true;
			memset(m_TerrainHeightInfo[i].WaterHeight,0xFF,sizeof(m_TerrainHeightInfo[i].WaterHeight));
			MH2OInfo * pWaterInfo=(MH2OInfo *)((BYTE *)pMH2O+sizeof(BLZ_CHUNK_HEADER)+pMH2O->Header[i].InformationOffset);			
			UINT LayerCount=pMH2O->Header[i].LayerCount;
			for(UINT l=0;l<LayerCount;l++)
			{
				CD3DSubMesh * pWaterSubMesh=new CD3DSubMesh(m_pManager->GetDevice());

				IndexCount=pWaterInfo[l].Width*pWaterInfo[l].Height*2*3;

				VertexCount=(pWaterInfo[l].Width+1)*(pWaterInfo[l].Height+1);

				pWaterSubMesh->GetVertexFormat().FVF=D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
				pWaterSubMesh->GetVertexFormat().VertexSize=sizeof(MODEL_LIQUID_VERTEXT);
				pWaterSubMesh->GetVertexFormat().IndexSize=sizeof(WORD);
				pWaterSubMesh->SetPrimitiveType(D3DPT_TRIANGLELIST);
				pWaterSubMesh->SetPrimitiveCount(IndexCount/3);		
				pWaterSubMesh->SetVertexCount(VertexCount);
				pWaterSubMesh->SetIndexCount(IndexCount);

				pWaterSubMesh->AllocDXIndexBuffer();
				pWaterSubMesh->AllocDXVertexBuffer();


				pWaterSubMesh->GetMaterial().GetMaterial().Ambient=WhiteColor;
				pWaterSubMesh->GetMaterial().GetMaterial().Diffuse=WhiteColor;
				pWaterSubMesh->GetMaterial().GetMaterial().Specular=WhiteColor;
				pWaterSubMesh->GetMaterial().GetMaterial().Emissive=BlackColor;
				pWaterSubMesh->GetMaterial().GetMaterial().Power=40.0f;

				CD3DFX * pFX=BuildLiquidFX();
				pWaterSubMesh->GetMaterial().SetFX(pFX);
				pWaterSubMesh->SetTransparent(true);

				CD3DTexture * pLiquidTexture=LoadLiquidTexture(pWaterInfo[l].LiquidType);

				if(pLiquidTexture)
				{
					pWaterSubMesh->GetMaterial().AddTexture(pLiquidTexture,0,"TexLay0","");
				}

				pWaterSubMesh->GetMaterial().AddTexture(NULL,D3D_TEX_FLAG_DEPTH_TEX,"TexLay1","");

				pWaterSubMesh->GetMaterial().CaculateHashCode();

				float * pHeightMap=NULL;
				BYTE * pAlphaMap=NULL;


				if(pWaterInfo[l].HeightMapOffset)
				{
					BYTE * pData=(BYTE *)pMH2O+sizeof(BLZ_CHUNK_HEADER)+pWaterInfo[l].HeightMapOffset;
					if((pWaterInfo[l].Flags&MH2OF_NO_HEIGHT_MAP)==0)
					{
						pHeightMap=(float *)pData;
						pData+=sizeof(float)*(pWaterInfo[l].Width+1)*(pWaterInfo[l].Height+1);
					}
					//if(pWaterInfo[l].Flags&MH2OF_HAVE_HEIGHT_MAP)
					{
						pAlphaMap=pData;
					}
				}

				pModelIndices=NULL;
				pWaterSubMesh->GetDXIndexBuffer()->Lock(0,0,(LPVOID *)&pModelIndices,0);

				MODEL_LIQUID_VERTEXT * pLiquidVertices=NULL;
				pWaterSubMesh->GetDXVertexBuffer()->Lock(0,0,(LPVOID *)&pLiquidVertices,0);


				VertexIndex=0;
				for(int y=pWaterInfo[l].YOffset;y<=pWaterInfo[l].YOffset+pWaterInfo[l].Height;y++)
				{
					for(int x=pWaterInfo[l].XOffset;x<=pWaterInfo[l].XOffset+pWaterInfo[l].Width;x++)
					{
						pLiquidVertices[VertexIndex].Pos.x=Pitch*x*2;
						pLiquidVertices[VertexIndex].Pos.z=-Pitch*y*2;

						pLiquidVertices[VertexIndex].Pos+=Pos;

						int Index=(y-pWaterInfo[l].YOffset)*(pWaterInfo[l].Width+1)+(x-pWaterInfo[l].XOffset);

						assert(y*8+x<TERRAIN_BLOCK_WATER_HEIGHT_COUNT);

						if(pHeightMap)
						{
							pLiquidVertices[VertexIndex].Pos.y=pHeightMap[Index];
							m_TerrainHeightInfo[i].WaterHeight[y*9+x]=pHeightMap[Index];
						}
						else
						{
							pLiquidVertices[VertexIndex].Pos.y=pWaterInfo[l].HeightLevel1;
							m_TerrainHeightInfo[i].WaterHeight[y*9+x]=pWaterInfo[l].HeightLevel1;
						}

						if(pAlphaMap)
						{
							pLiquidVertices[VertexIndex].Color=(((pAlphaMap[Index])&0xFF)<<24);
						}
						else
						{
							pLiquidVertices[VertexIndex].Color=0xFF000000;
						}

						pLiquidVertices[VertexIndex].Tex.x=(x*2.0f)/16.0f;
						pLiquidVertices[VertexIndex].Tex.y=y*2/16.0f;

						VertexIndex++;
					}
				}

				BYTE * pRenderMask=NULL;
				if(pWaterInfo[l].Mask2Offset)
					pRenderMask=((BYTE *)pMH2O+sizeof(BLZ_CHUNK_HEADER)+pWaterInfo[l].Mask2Offset);

				IndexIndex=0;

				for(int y=0;y<pWaterInfo[l].Height;y++)
				{
					for(int x=0;x<pWaterInfo[l].Width;x++)
					{
						if(WantRenderWater(pRenderMask,y*pWaterInfo[l].Width+x))						
						{
							pModelIndices[IndexIndex]	=y*(pWaterInfo[l].Width+1)+x;
							pModelIndices[IndexIndex+1]	=y*(pWaterInfo[l].Width+1)+x+1;
							pModelIndices[IndexIndex+2]	=(y+1)*(pWaterInfo[l].Width+1)+x;


							pModelIndices[IndexIndex+3]	=(y+1)*(pWaterInfo[l].Width+1)+x;
							pModelIndices[IndexIndex+4]	=y*(pWaterInfo[l].Width+1)+x+1;
							pModelIndices[IndexIndex+5]	=(y+1)*(pWaterInfo[l].Width+1)+x+1;

							IndexIndex+=6;
						}
					}

				}

				pWaterSubMesh->SetIndexCount(IndexIndex);
				pWaterSubMesh->SetPrimitiveCount(IndexIndex/3);


				pWaterSubMesh->GetDXVertexBuffer()->Unlock();

				pWaterSubMesh->GetDXIndexBuffer()->Unlock();

				pWaterSubMesh->SetID(1000+i);

				pWaterSubMesh->SetProperty(pWaterSubMesh->GetProperty()|SMP_IS_WATER);

				SubMeshName.Format(_T("W%d-%d"),i,l);
				pWaterSubMesh->SetName(SubMeshName);

				LiquidSubMeshList.Add(pWaterSubMesh);
				pD3DSubMesh->AddProperty(SMP_RENDER_TO_DEPTH);				
			}
		}
		else if(pMCNK->LiquidSize>sizeof(BLZ_CHUNK_HEADER))
		{

			m_TerrainHeightInfo[i].HaveWater=true;
			memset(m_TerrainHeightInfo[i].WaterHeight,0xFF,sizeof(m_TerrainHeightInfo[i].WaterHeight));

			CD3DSubMesh * pWaterSubMesh=new CD3DSubMesh(m_pManager->GetDevice());


			IndexCount=8*8*2*3;

			VertexCount=9*9;

			pWaterSubMesh->GetVertexFormat().FVF=D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
			pWaterSubMesh->GetVertexFormat().VertexSize=sizeof(MODEL_LIQUID_VERTEXT);
			pWaterSubMesh->GetVertexFormat().IndexSize=sizeof(WORD);
			pWaterSubMesh->SetPrimitiveType(D3DPT_TRIANGLELIST);
			pWaterSubMesh->SetPrimitiveCount(IndexCount/3);		
			pWaterSubMesh->SetVertexCount(VertexCount);
			pWaterSubMesh->SetIndexCount(IndexCount);

			pWaterSubMesh->AllocDXIndexBuffer();
			pWaterSubMesh->AllocDXVertexBuffer();


			pWaterSubMesh->GetMaterial().GetMaterial().Ambient=WhiteColor;
			pWaterSubMesh->GetMaterial().GetMaterial().Diffuse=WhiteColor;
			pWaterSubMesh->GetMaterial().GetMaterial().Specular=WhiteColor;
			pWaterSubMesh->GetMaterial().GetMaterial().Emissive=BlackColor;
			pWaterSubMesh->GetMaterial().GetMaterial().Power=40.0f;

			CD3DFX * pFX=BuildLiquidFX();
			pWaterSubMesh->GetMaterial().SetFX(pFX);
			pWaterSubMesh->SetTransparent(true);

			int LiguidType;
			if(pMCNK->Flags&MCNK_FLAG_SLIME)
			{
				LiguidType=WLT_SLIMA;
			}
			else if(pMCNK->Flags&MCNK_FLAG_MAGMA)
			{
				LiguidType=WLT_MAGMA;
			}
			else if(pMCNK->Flags&MCNK_FLAG_OCEAN)
			{
				LiguidType=WLT_OCEAN;
			}
			else
			{
				LiguidType=WLT_RIVER;
			}

			CD3DTexture * pLiquidTexture=LoadLiquidTexture(LiguidType);

			if(pLiquidTexture)
			{
				pWaterSubMesh->GetMaterial().AddTexture(pLiquidTexture,0,"TexLay0","");
			}
			pWaterSubMesh->GetMaterial().AddTexture(NULL,D3D_TEX_FLAG_DEPTH_TEX,"TexLay1","");
			
			pWaterSubMesh->GetMaterial().CaculateHashCode();


			pModelIndices=NULL;
			pWaterSubMesh->GetDXIndexBuffer()->Lock(0,0,(LPVOID *)&pModelIndices,0);

			MODEL_LIQUID_VERTEXT * pLiquidVertices=NULL;
			pWaterSubMesh->GetDXVertexBuffer()->Lock(0,0,(LPVOID *)&pLiquidVertices,0);



			VertexIndex=0;
			for(int y=0;y<9;y++)
			{
				for(int x=0;x<9;x++)
				{
					pLiquidVertices[VertexIndex].Pos.x=Pitch*x*2;
					pLiquidVertices[VertexIndex].Pos.z=-Pitch*y*2;

					pLiquidVertices[VertexIndex].Pos+=Pos;


					pLiquidVertices[VertexIndex].Pos.y=pMCLQ->LiquidInfo[y][x].Height;



					if(pLiquidVertices[VertexIndex].Pos.y<pMCLQ->MinHeight)
						pLiquidVertices[VertexIndex].Pos.y=pMCLQ->MinHeight;
					if(pLiquidVertices[VertexIndex].Pos.y>pMCLQ->MaxHeight)
						pLiquidVertices[VertexIndex].Pos.y=pMCLQ->MaxHeight;

					m_TerrainHeightInfo[i].WaterHeight[y*9+x]=pLiquidVertices[VertexIndex].Pos.y;



					pLiquidVertices[VertexIndex].Color=(((pMCLQ->LiquidInfo[y][x].Color1)&0xFF)<<24);

					pLiquidVertices[VertexIndex].Tex.x=(x*2.0f)/16.0f;
					pLiquidVertices[VertexIndex].Tex.y=y*2/16.0f;

					VertexIndex++;
				}
			}

			IndexIndex=0;

			for(int y=0;y<8;y++)
			{
				for(int x=0;x<8;x++)
				{
					if((pMCLQ->RenderFlag[y][x]&8)==0)
					{
						pModelIndices[IndexIndex]	=y*9+x;
						pModelIndices[IndexIndex+1]	=y*9+x+1;
						pModelIndices[IndexIndex+2]	=(y+1)*9+x;


						pModelIndices[IndexIndex+3]	=(y+1)*9+x;
						pModelIndices[IndexIndex+4]	=y*9+x+1;
						pModelIndices[IndexIndex+5]	=(y+1)*9+x+1;

						IndexIndex+=6;
					}
				}
			}

			pWaterSubMesh->SetIndexCount(IndexIndex);
			pWaterSubMesh->SetPrimitiveCount(IndexIndex/3);


			pWaterSubMesh->GetDXVertexBuffer()->Unlock();

			pWaterSubMesh->GetDXIndexBuffer()->Unlock();

			pWaterSubMesh->SetID(1000+i);
			pWaterSubMesh->SetProperty(pWaterSubMesh->GetProperty()|SMP_IS_WATER);

			SubMeshName.Format(_T("L%d"),i);
			pWaterSubMesh->SetName(SubMeshName);

			LiquidSubMeshList.Add(pWaterSubMesh);
			pD3DSubMesh->AddProperty(SMP_RENDER_TO_DEPTH);			
		}

		m_SubMeshList.Add(pD3DSubMesh);


	}


	for(UINT i=0;i<LiquidSubMeshList.GetCount();i++)
	{
		m_SubMeshList.Add(LiquidSubMeshList[i]);
	}	
	return true;
}

bool CD3DWOWADTModelResource::LoadObjectsInfo(LPCTSTR ModelFileName,bool BeLoadObject)
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

	SetName(ModelFileName);

	CBLZChunkFile ADTChunk;

	if(!ADTChunk.Load(pFile))
	{
		pFile->Release();
		return false;
	}
	pFile->Release();	

	BLZ_CHUNK_VERSION * pVersion=(BLZ_CHUNK_VERSION *)ADTChunk.GetFirstChunk(CHUNK_ID_VERSION);

	BLZ_CHUNK_MMDX * pMMDX=(BLZ_CHUNK_MMDX *)ADTChunk.GetFirstChunk(CHUNK_ID_ADT_MMDX);

	BLZ_CHUNK_MMID * pMMID=(BLZ_CHUNK_MMID *)ADTChunk.GetFirstChunk(CHUNK_ID_ADT_MMID);

	BLZ_CHUNK_MWMO * pMWMO=(BLZ_CHUNK_MWMO *)ADTChunk.GetFirstChunk(CHUNK_ID_ADT_MWMO);

	BLZ_CHUNK_MWID * pMWID=(BLZ_CHUNK_MWID *)ADTChunk.GetFirstChunk(CHUNK_ID_ADT_MWID);

	BLZ_CHUNK_MDDF * pMDDF=(BLZ_CHUNK_MDDF *)ADTChunk.GetFirstChunk(CHUNK_ID_ADT_MDDF);

	BLZ_CHUNK_MODF * pMODF=(BLZ_CHUNK_MODF *)ADTChunk.GetFirstChunk(CHUNK_ID_ADT_MODF);

	UINT Count=pMDDF->ChunkSize/sizeof(ADT_M2_OBJECT_INFO);
	m_M2ObjectList.Resize(Count);
	for(UINT i=0;i<Count;i++)
	{
		m_M2ObjectList[i].ID=pMDDF->M2Objects[i].ID;
		m_M2ObjectList[i].pModelResource=NULL;
		CEasyString ObjectFileName=pMMDX->M2FileNames+pMMID->M2FileNameIndices[pMDDF->M2Objects[i].Index];
		int Pos=ObjectFileName.ReverseFind('.');
		if(Pos>=0)
		{
			ObjectFileName=ObjectFileName.Left(Pos);
			ObjectFileName+=".m2";


			m_M2ObjectList[i].pModelResource=NULL;
			m_M2ObjectList[i].ModelFilePath=ObjectFileName;
			m_M2ObjectList[i].Position.x=pMDDF->M2Objects[i].Position.x;
			m_M2ObjectList[i].Position.y=pMDDF->M2Objects[i].Position.y;
			m_M2ObjectList[i].Position.z=-pMDDF->M2Objects[i].Position.z;					
			m_M2ObjectList[i].Orientation=CD3DQuaternion::FromRotationYawPitchRoll(
				-pMDDF->M2Objects[i].Orientation.y*PI/180.0f,
				-pMDDF->M2Objects[i].Orientation.x*PI/180.0f,
				pMDDF->M2Objects[i].Orientation.z*PI/180.0f);
			m_M2ObjectList[i].Orientation.Normalize();
			m_M2ObjectList[i].Scale=pMDDF->M2Objects[i].Scale/1024.0f;
			m_M2ObjectList[i].Flag=pMDDF->M2Objects[i].Flags;

			if(BeLoadObject)
			{
				CD3DWOWM2ModelResource* pResource=
					dynamic_cast<CD3DWOWM2ModelResource*>(m_pManager->GetResource(ObjectFileName));
				if(!pResource)
				{
					pResource=new CD3DWOWM2ModelResource(m_pManager);
					if(pResource->LoadFromFile(ObjectFileName))
					{
						//PrintD3DLog("加载了[%s]",(LPCTSTR)ObjectFileName);
						if(!m_pManager->AddResource(pResource,ObjectFileName))
						{
							pResource->Release();
							pResource=NULL;
						}
					}
					else
					{
						PrintD3DLog(_T("加载M2文件%s失败"),(LPCTSTR)ObjectFileName);
						pResource->Release();
						pResource=NULL;
					}						
				}
				else
				{
					pResource->AddUseRef();
				}	
				m_M2ObjectList[i].pModelResource=pResource;
			}

		}			


	}


	Count=pMODF->ChunkSize/sizeof(ADT_WMO_OBJECT_INFO);
	m_WMOObjectList.Resize(Count);
	for(UINT i=0;i<Count;i++)
	{
		m_WMOObjectList[i].ID=pMODF->WMOObjects[i].ID;
		CEasyString FileName=pMWMO->WMOFileNames+pMWID->WMOFileNameIndices[pMODF->WMOObjects[i].Index];
		//FileName.MakeUpper();
		//FileName.Replace(".MDX",".M2");

		m_WMOObjectList[i].pModelResource=NULL;

		m_WMOObjectList[i].ModelFilePath=FileName;
		m_WMOObjectList[i].Position.x=pMODF->WMOObjects[i].Position.x;
		m_WMOObjectList[i].Position.y=pMODF->WMOObjects[i].Position.y;
		m_WMOObjectList[i].Position.z=-pMODF->WMOObjects[i].Position.z;		

		m_WMOObjectList[i].Orientation=CD3DQuaternion::FromRotationYawPitchRoll(
			-pMODF->WMOObjects[i].Orientation.y*PI/180.0f,
			-pMODF->WMOObjects[i].Orientation.x*PI/180.0f,
			pMODF->WMOObjects[i].Orientation.z*PI/180.0f);
		m_WMOObjectList[i].Orientation.Normalize();
		m_WMOObjectList[i].DoodadSet=pMODF->WMOObjects[i].DoodadSet;
		m_WMOObjectList[i].Flag=pMODF->WMOObjects[i].Flags;

		if(BeLoadObject)
		{
			CD3DWOWWMOModelResource* pResource=
				dynamic_cast<CD3DWOWWMOModelResource*>(m_pManager->GetResource(FileName));
			if(!pResource)
			{
				pResource=new CD3DWOWWMOModelResource(m_pManager);
				if(pResource->LoadFromFile(FileName))
				{
					//PrintD3DLog("加载了[%s]",(LPCTSTR)FileName);
					if(!m_pManager->AddResource(pResource,FileName))
					{
						pResource->Release();
						pResource=NULL;
					}
				}
				else
				{
					PrintD3DLog(_T("加载WMO文件%s失败"),(LPCTSTR)FileName);
					pResource->Release();
					pResource=NULL;
				}						
			}
			else
			{
				pResource->AddUseRef();
			}

			m_WMOObjectList[i].pModelResource=pResource;
		}

	}


	return true;
}

bool CD3DWOWADTModelResource::LoadTextureInfo(LPCTSTR ModelFileName,bool IsBigAlphaMask)
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

	SetName(ModelFileName);

	CBLZChunkFile ADTChunk;

	if(!ADTChunk.Load(pFile))
	{
		pFile->Release();
		return false;
	}
	pFile->Release();	

	BLZ_CHUNK_VERSION * pVersion=(BLZ_CHUNK_VERSION *)ADTChunk.GetFirstChunk(CHUNK_ID_VERSION);

	BLZ_CHUNK_MTEX * pMTEX=(BLZ_CHUNK_MTEX *)ADTChunk.GetFirstChunk(CHUNK_ID_ADT_MTEX);
	
	CEasyArray<CSmartPtr<CD3DTexture> >		TextureList;
	CEasyArray<TEXTURE_LAYER_INFO>			TextureLayerInfos;


	UINT Ptr=0;
	UINT TextureCount=0;
	while(Ptr<pMTEX->ChunkSize)
	{
		TextureCount++;
		Ptr+=strlen(pMTEX->TextureFileNames+Ptr)+1;
	}
	TextureList.Resize(TextureCount);
	Ptr=0;
	TextureCount=0;
	while(Ptr<pMTEX->ChunkSize)
	{
		CEasyString TextureFileName=pMTEX->TextureFileNames+Ptr;
		TextureFileName.MakeUpper();
		TextureFileName.Replace(_T(".BLP"),_T("_S.BLP"));
		TextureList[TextureCount]=m_pManager->GetDevice()->GetTextureManager()->LoadTexture(TextureFileName);
		TextureCount++;
		Ptr+=strlen(pMTEX->TextureFileNames+Ptr)+1;
	}

	CBLZChunkFile::CChunkList * pChunkList=ADTChunk.GetChunks(CHUNK_ID_ADT_MCNK);

	TextureLayerInfos.Resize(pChunkList->GetCount());

	for(UINT i=0;i<pChunkList->GetCount();i++)
	{
		CBLZChunkFile MCNKChunk;

		MCNKChunk.Load((BYTE *)((*pChunkList)[i])+sizeof(BLZ_CHUNK_HEADER),((*pChunkList)[i])->ChunkSize);
		
		BLZ_CHUNK_MCNK * pMCNK=(BLZ_CHUNK_MCNK *)MCNKChunk.GetFirstChunk(CHUNK_ID_ADT_MCNK);		
		
	
		BLZ_CHUNK_MCLY * pMCLY=(BLZ_CHUNK_MCLY *)MCNKChunk.GetFirstChunk(CHUNK_ID_ADT_MCLY);
		BLZ_CHUNK_MCAL * pMCAL=(BLZ_CHUNK_MCAL *)MCNKChunk.GetFirstChunk(CHUNK_ID_ADT_MCAL);
		BLZ_CHUNK_MCSH * pMCSH=(BLZ_CHUNK_MCSH *)MCNKChunk.GetFirstChunk(CHUNK_ID_ADT_MCSH);
	
		

		if(pMCLY)
		{
			UINT LayerCount=pMCLY->ChunkSize/sizeof(MCLYLayerInfo);

			if(!LoadAlphaLayer(TextureLayerInfos[i],LayerCount,pMCLY,pMCAL,i,IsBigAlphaMask))
				return false;
		}

		if(pMCSH)
		{
			LoadShadowMap(TextureLayerInfos[i],pMCSH,i);
		}

		CD3DSubMesh * pD3DSubMesh=m_SubMeshList[i];

		CD3DFX * pFX=BuildFX(i,TextureLayerInfos[i].LayerCount,TextureLayerInfos[i].pShadowMap!=NULL,IsBigAlphaMask);
		pD3DSubMesh->GetMaterial().SetFX(pFX);

		CEasyStringA TexFXName;
		for(UINT j=0;j<TextureLayerInfos[i].LayerCount;j++)
		{			
			CD3DTexture * pTexture=TextureList[TextureLayerInfos[i].Layers[j].TextureIndex];
			TexFXName.Format("TexLay%d",pD3DSubMesh->GetMaterial().GetTextureLayerCount());
			pD3DSubMesh->GetMaterial().AddTexture(pTexture,0,TexFXName,"");
			if(pTexture)
				pTexture->AddUseRef();
			if(TextureLayerInfos[i].Layers[j].Flag&MLIF_ALPHA_MAP)
			{
				TexFXName.Format("TexLay%d",pD3DSubMesh->GetMaterial().GetTextureLayerCount());
				pD3DSubMesh->GetMaterial().AddTexture(TextureLayerInfos[i].Layers[j].pAlphaMap,TP_ALPHA_MAP,TexFXName,"");
				if(TextureLayerInfos[i].Layers[j].pAlphaMap)
					TextureLayerInfos[i].Layers[j].pAlphaMap->AddUseRef();
			}			
		}

		if(TextureLayerInfos[i].pShadowMap)
		{
			pD3DSubMesh->GetMaterial().AddTexture(TextureLayerInfos[i].pShadowMap,TP_SHADOW_MAP,"TexShadow","");
			TextureLayerInfos[i].pShadowMap->AddUseRef();
		}

		pD3DSubMesh->GetMaterial().CaculateHashCode();
	}

	return true;
}


void CD3DWOWADTModelResource::PickResource(CUSOResourceManager * pResourceManager,UINT Param)
{
	CD3DObjectResource::PickResource(pResourceManager,Param);	
	for(UINT i=0;i<m_M2ObjectList.GetCount();i++)
	{
		if(m_M2ObjectList[i].pModelResource)
		{
			m_M2ObjectList[i].pModelResource->PickResource(pResourceManager,Param);
			pResourceManager->AddResource(m_M2ObjectList[i].pModelResource);
		}
	}
	for(UINT i=0;i<m_WMOObjectList.GetCount();i++)
	{
		if(m_WMOObjectList[i].pModelResource)
		{
			m_WMOObjectList[i].pModelResource->PickResource(pResourceManager,Param);
			pResourceManager->AddResource(m_WMOObjectList[i].pModelResource);
		}
	}
}

bool CD3DWOWADTModelResource::ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CD3DObjectResource::ToSmartStruct(Packet,pResourceManager,Param))
		return false;

	{
		UINT BufferSize;
		void * pBuffer=Packet.PrepareMember(BufferSize);
		CSmartStruct M2ObjectInfoList(pBuffer,BufferSize,true);
		for(UINT i=0;i<m_M2ObjectList.GetCount();i++)
		{
			pBuffer=M2ObjectInfoList.PrepareMember(BufferSize);
			CSmartStruct M2ObjectInfo(pBuffer,BufferSize,true);

			CHECK_SMART_STRUCT_ADD_AND_RETURN(M2ObjectInfo.AddMember(SST_MOI_ID,m_M2ObjectList[i].ID));
			CHECK_SMART_STRUCT_ADD_AND_RETURN(M2ObjectInfo.AddMember(SST_MOI_MODEL_RESOURCE,m_M2ObjectList[i].pModelResource->GetName()));
			CHECK_SMART_STRUCT_ADD_AND_RETURN(M2ObjectInfo.AddMember(SST_MOI_POSITION,
				(LPCTSTR)(&m_M2ObjectList[i].Position),sizeof(m_M2ObjectList[i].Position)));
			CHECK_SMART_STRUCT_ADD_AND_RETURN(M2ObjectInfo.AddMember(SST_MOI_ORIENTATION,
				(LPCTSTR)(&m_M2ObjectList[i].Orientation),sizeof(m_M2ObjectList[i].Orientation)));
			CHECK_SMART_STRUCT_ADD_AND_RETURN(M2ObjectInfo.AddMember(SST_MOI_SCALE,
				(LPCTSTR)(&m_M2ObjectList[i].Scale),sizeof(m_M2ObjectList[i].Scale)));
			
			if(!M2ObjectInfoList.FinishMember(SST_MOL_M2_OBJECT_INFO,M2ObjectInfo.GetDataLen()))
				return false;
		}
		if(!Packet.FinishMember(SST_D3DWAMR_M2_OBJECT_LIST,M2ObjectInfoList.GetDataLen()))
			return false;
	}

	{
		UINT BufferSize;
		void * pBuffer=Packet.PrepareMember(BufferSize);
		CSmartStruct WMOObjectInfoList(pBuffer,BufferSize,true);
		for(UINT i=0;i<m_WMOObjectList.GetCount();i++)
		{
			pBuffer=WMOObjectInfoList.PrepareMember(BufferSize);
			CSmartStruct WMOObjectInfo(pBuffer,BufferSize,true);

			CHECK_SMART_STRUCT_ADD_AND_RETURN(WMOObjectInfo.AddMember(SST_WOI_ID,m_WMOObjectList[i].ID));
			CHECK_SMART_STRUCT_ADD_AND_RETURN(WMOObjectInfo.AddMember(SST_WOI_MODEL_RESOURCE,m_WMOObjectList[i].pModelResource->GetName()));
			CHECK_SMART_STRUCT_ADD_AND_RETURN(WMOObjectInfo.AddMember(SST_WOI_POSITION,
				(LPCTSTR)(&m_WMOObjectList[i].Position),sizeof(m_WMOObjectList[i].Position)));
			CHECK_SMART_STRUCT_ADD_AND_RETURN(WMOObjectInfo.AddMember(SST_WOI_ORIENTATION,
				(LPCTSTR)(&m_WMOObjectList[i].Orientation),sizeof(m_WMOObjectList[i].Orientation)));
			CHECK_SMART_STRUCT_ADD_AND_RETURN(WMOObjectInfo.AddMember(SST_WOI_DOODAD_SET,m_WMOObjectList[i].DoodadSet));

			if(!WMOObjectInfoList.FinishMember(SST_WOL_WMO_OBJECT_INFO,WMOObjectInfo.GetDataLen()))
				return false;
		}
		if(!Packet.FinishMember(SST_D3DWAMR_WMO_OBJECT_LIST,WMOObjectInfoList.GetDataLen()))
			return false;
	}

	{
		UINT BufferSize;
		void * pBuffer=Packet.PrepareMember(BufferSize);
		CSmartStruct HeightInfoList(pBuffer,BufferSize,true);
		for(UINT i=0;i<BLZ_ADT_MAP_TILE_COUNT;i++)
		{
			pBuffer=HeightInfoList.PrepareMember(BufferSize);
			CSmartStruct HeightInfo(pBuffer,BufferSize,true);

			CHECK_SMART_STRUCT_ADD_AND_RETURN(HeightInfo.AddMember(SST_HI_HAVE_WATER,(BYTE)m_TerrainHeightInfo[i].HaveWater));
			CHECK_SMART_STRUCT_ADD_AND_RETURN(HeightInfo.AddMember(SST_HI_TERRAIN_HEIGHT,
				(LPCTSTR)(m_TerrainHeightInfo[i].TerrainHeight),sizeof(m_TerrainHeightInfo[i].TerrainHeight)));
			if(m_TerrainHeightInfo[i].HaveWater)
			{
				CHECK_SMART_STRUCT_ADD_AND_RETURN(HeightInfo.AddMember(SST_HI_WATER_HEIGHT,
					(LPCTSTR)(m_TerrainHeightInfo[i].WaterHeight),sizeof(m_TerrainHeightInfo[i].WaterHeight)));
			}
			if(!HeightInfoList.FinishMember(SST_HIL_HEIGHT_INFO,HeightInfo.GetDataLen()))
				return false;
		}
		if(!Packet.FinishMember(SST_D3DWAMR_HEIGHT_INFO_LIST,HeightInfoList.GetDataLen()))
			return false;
	}

	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWAMR_POSITION,
		(LPCTSTR)(&m_Position),sizeof(m_Position)));

	return true;
}

bool CD3DWOWADTModelResource::FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	Destory();

	if(!CD3DObjectResource::FromSmartStruct(Packet,pResourceManager,Param))
		return false;

	UINT M2ObjectCount=0;
	UINT WMOObjectCount=0;
	UINT HeightInfoCount=0;

	void * Pos=Packet.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartStruct SubPacket=Packet.GetNextMember(Pos,MemberID);
		switch(MemberID)
		{	
		case SST_D3DWAMR_M2_OBJECT_LIST:
			M2ObjectCount=SubPacket.GetMemberCount();
			break;
		case SST_D3DWAMR_WMO_OBJECT_LIST:
			WMOObjectCount=SubPacket.GetMemberCount();
			break;
		case SST_D3DWAMR_HEIGHT_INFO_LIST:
			HeightInfoCount=SubPacket.GetMemberCount();
			break;
		}
	}

	if(HeightInfoCount>BLZ_ADT_MAP_TILE_COUNT)
		return false;

	m_M2ObjectList.Resize(M2ObjectCount);
	m_WMOObjectList.Resize(WMOObjectCount);

	M2ObjectCount=0;
	WMOObjectCount=0;
	HeightInfoCount=0;

	Pos=Packet.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartValue Value=Packet.GetNextMember(Pos,MemberID);
		switch(MemberID)
		{	
		case SST_D3DWAMR_M2_OBJECT_LIST:
			{
				CSmartStruct SubPacket=Value;
				void * SubPos=SubPacket.GetFirstMemberPosition();
				while(SubPos)
				{
					WORD SubMemberID;
					CSmartStruct M2ObjectInfo=SubPacket.GetNextMember(SubPos,SubMemberID);
					if(SubMemberID==SST_MOL_M2_OBJECT_INFO)
					{
						m_M2ObjectList[M2ObjectCount].ID=M2ObjectInfo.GetMember(SST_MOI_ID);
						LPCTSTR szResourceName=M2ObjectInfo.GetMember(SST_MOI_MODEL_RESOURCE);
						m_M2ObjectList[M2ObjectCount].pModelResource=(CD3DWOWM2ModelResource *)pResourceManager->FindResource(szResourceName,GET_CLASS_INFO(CD3DWOWM2ModelResource));
						if(m_M2ObjectList[M2ObjectCount].pModelResource)
							m_M2ObjectList[M2ObjectCount].pModelResource->AddUseRef();
						memcpy(&(m_M2ObjectList[M2ObjectCount].Position),(LPCSTR)M2ObjectInfo.GetMember(SST_MOI_POSITION),
							sizeof(m_M2ObjectList[M2ObjectCount].Position));
						memcpy(&(m_M2ObjectList[M2ObjectCount].Orientation),(LPCSTR)M2ObjectInfo.GetMember(SST_MOI_ORIENTATION),
							sizeof(m_M2ObjectList[M2ObjectCount].Orientation));
						memcpy(&(m_M2ObjectList[M2ObjectCount].Scale),(LPCSTR)M2ObjectInfo.GetMember(SST_MOI_SCALE),
							sizeof(m_M2ObjectList[M2ObjectCount].Scale));						
						M2ObjectCount++;
					}
				}
			}
			break;
		case SST_D3DWAMR_WMO_OBJECT_LIST:
			{
				CSmartStruct SubPacket=Value;
				void * SubPos=SubPacket.GetFirstMemberPosition();
				while(SubPos)
				{
					WORD SubMemberID;
					CSmartStruct WMOObjectInfo=SubPacket.GetNextMember(SubPos,SubMemberID);
					if(SubMemberID==SST_WOL_WMO_OBJECT_INFO)
					{
						m_WMOObjectList[WMOObjectCount].ID=WMOObjectInfo.GetMember(SST_WOI_ID);
						LPCTSTR szResourceName=WMOObjectInfo.GetMember(SST_WOI_MODEL_RESOURCE);
						m_WMOObjectList[WMOObjectCount].pModelResource=(CD3DWOWWMOModelResource *)pResourceManager->FindResource(szResourceName,GET_CLASS_INFO(CD3DWOWWMOModelResource));
						if(m_WMOObjectList[WMOObjectCount].pModelResource)
							m_WMOObjectList[WMOObjectCount].pModelResource->AddUseRef();
						memcpy(&(m_WMOObjectList[WMOObjectCount].Position),(LPCSTR)WMOObjectInfo.GetMember(SST_WOI_POSITION),
							sizeof(m_WMOObjectList[WMOObjectCount].Position));
						memcpy(&(m_WMOObjectList[WMOObjectCount].Orientation),(LPCSTR)WMOObjectInfo.GetMember(SST_WOI_ORIENTATION),
							sizeof(m_WMOObjectList[WMOObjectCount].Orientation));
						m_WMOObjectList[WMOObjectCount].DoodadSet=WMOObjectInfo.GetMember(SST_WOI_DOODAD_SET);
						WMOObjectCount++;
					}
				}
			}
			break;	
		case SST_D3DWAMR_HEIGHT_INFO_LIST:
			{
				CSmartStruct SubPacket=Value;
				void * SubPos=SubPacket.GetFirstMemberPosition();
				while(SubPos)
				{
					WORD SubMemberID;
					CSmartStruct HeightInfo=SubPacket.GetNextMember(SubPos,SubMemberID);
					if(SubMemberID==SST_HIL_HEIGHT_INFO)
					{
						m_TerrainHeightInfo[HeightInfoCount].HaveWater=((BYTE)HeightInfo.GetMember(SST_HI_HAVE_WATER))!=0;
						memcpy(m_TerrainHeightInfo[HeightInfoCount].TerrainHeight,
							(LPCSTR)HeightInfo.GetMember(SST_HI_TERRAIN_HEIGHT),
							sizeof(m_TerrainHeightInfo[HeightInfoCount].TerrainHeight));
						if(m_TerrainHeightInfo[HeightInfoCount].HaveWater)
						{
							memcpy(m_TerrainHeightInfo[HeightInfoCount].WaterHeight,
								(LPCSTR)HeightInfo.GetMember(SST_HI_WATER_HEIGHT),
								sizeof(m_TerrainHeightInfo[HeightInfoCount].WaterHeight));
						}
						HeightInfoCount++;
					}
				}
			}
			break;
		case SST_D3DWAMR_POSITION:
			memcpy(&(m_Position),(LPCSTR)Value,
				sizeof(m_Position));
			break;
		}
	}

	return true;
}

UINT CD3DWOWADTModelResource::GetSmartStructSize(UINT Param)
{
	UINT Size=CD3DObjectResource::GetSmartStructSize(Param);
	Size+=CSmartStruct::GetStructMemberSize(0);
	for(UINT i=0;i<m_M2ObjectList.GetCount();i++)
	{
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_M2ObjectList[i].ID));
		Size += CSmartStruct::GetStringMemberSize(m_M2ObjectList[i].pModelResource->GetNameLength());
		Size += CSmartStruct::GetStringMemberSizeA(sizeof(m_M2ObjectList[i].Position));
		Size += CSmartStruct::GetStringMemberSizeA(sizeof(m_M2ObjectList[i].Orientation));
		Size += CSmartStruct::GetStringMemberSizeA(sizeof(m_M2ObjectList[i].Scale));
		Size+=CSmartStruct::GetStructMemberSize(0);
	}
	Size+=CSmartStruct::GetStructMemberSize(0);
	for(UINT i=0;i<m_WMOObjectList.GetCount();i++)
	{
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_WMOObjectList[i].ID));
		Size += CSmartStruct::GetStringMemberSize(m_WMOObjectList[i].pModelResource->GetNameLength());
		Size += CSmartStruct::GetStringMemberSizeA(sizeof(m_WMOObjectList[i].Position));
		Size += CSmartStruct::GetStringMemberSizeA(sizeof(m_WMOObjectList[i].Orientation));
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_WMOObjectList[i].DoodadSet));		
		Size+=CSmartStruct::GetStructMemberSize(0);
	}
	Size+=CSmartStruct::GetStructMemberSize(0);
	for(UINT i=0;i<BLZ_ADT_MAP_TILE_COUNT;i++)
	{
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_TerrainHeightInfo[i].HaveWater));
		Size += CSmartStruct::GetFixMemberSize(sizeof(m_TerrainHeightInfo[i].TerrainHeight));
		if(m_TerrainHeightInfo[i].HaveWater)
		{
			Size += CSmartStruct::GetFixMemberSize(sizeof(m_TerrainHeightInfo[i].WaterHeight));
		}		
		Size+=CSmartStruct::GetStructMemberSize(0);
	}

	Size += CSmartStruct::GetStringMemberSizeA(sizeof(m_Position));

	return Size;
}


CD3DFX * CD3DWOWADTModelResource::BuildFX(UINT Index,UINT LayCount,bool HaveShadowMap,bool UseNewShader)
{
	CEasyString FXName;
	CEasyString PSFunction;
	

	FXName.Format(_T("ADTModel\\0x%X%s"),
		LayCount,HaveShadowMap?"S":"");

	if(HaveShadowMap)
	{
		switch(LayCount)
		{
		case 4:
			PSFunction="PShaderFourLayerWithShadow";
			break;
		case 3:
			PSFunction="PShaderThreeLayerWithShadow";
			break;
		case 2:
			PSFunction="PShaderTwoLayerWithShadow";
			break;
		default:
			PSFunction="PShaderOneLayerWithShadow";
		}
	}
	else
	{
		switch(LayCount)
		{
		case 4:
			PSFunction="PShaderFourLayer";
			break;
		case 3:
			PSFunction="PShaderThreeLayer";
			break;
		case 2:
			PSFunction="PShaderTwoLayer";
			break;
		default:
			PSFunction="PShaderOneLayer";
		}
	}

	CEasyString FxContent;
	
	if(UseNewShader)
	{
		FxContent=ADT_MODEL_30_FX;		
	}
	else
	{
		FxContent=ADT_MODEL_20_FX;		
	}

	FxContent.Replace(_T("<PSFunction>"),PSFunction);
	

	CD3DFX * pFX=m_pManager->GetDevice()->GetFXManager()->LoadFXFromMemory(FXName,FxContent.GetBuffer(),FxContent.GetLength());	
	return pFX;
}

CD3DFX * CD3DWOWADTModelResource::BuildLiquidFX()
{
	CD3DFX * pFX=m_pManager->GetDevice()->GetFXManager()->LoadFXFromMemory(_T("ADT_LIQUID_FX"),ADT_LIQUID_FX,strlen(ADT_LIQUID_FX));	
	return pFX;
}

bool CD3DWOWADTModelResource::LoadAlphaLayer(TEXTURE_LAYER_INFO& LayInfo,UINT LayerCount,BLZ_CHUNK_MCLY * pMCLY,BLZ_CHUNK_MCAL * pMCAL,UINT ID,bool IsBigAlphaMask)
{
	if(LayerCount>BLZ_ADT_TEXTURE_LAYER_COUNT)
		return false;
	LayInfo.LayerCount=LayerCount;	
	for(UINT i=0;i<LayerCount;i++)
	{
		LayInfo.Layers[i].TextureIndex=pMCLY->Layers[i].TextureID;
		LayInfo.Layers[i].Flag=pMCLY->Layers[i].Flags;
		LayInfo.Layers[i].pAlphaMap=NULL;
		if(LayInfo.Layers[i].Flag&MLIF_ALPHA_MAP)
		{
			int AlphaDataType;
			if(LayInfo.Layers[i].Flag&MLIF_COMPRESSED_ALPHA_MAP)
			{
				AlphaDataType=ADT_COMPRESS;
			}
			else
			{
				if(IsBigAlphaMask)
					AlphaDataType=ADT_UNCOMPRESS_4096;
				else
					AlphaDataType=ADT_UNCOMPRESS_2048;				
			}
			UINT ProcessSize=0;
			LPBYTE pAlphaData=pMCAL->AlphaMapData+pMCLY->Layers[i].MCALOffset;
			UINT AlphaDataSize=pMCAL->ChunkSize-pMCLY->Layers[i].MCALOffset;
			LayInfo.Layers[i].pAlphaMap=CreateAlphaMap(AlphaDataType,pAlphaData,AlphaDataSize,ProcessSize,ID,i);
			if(LayInfo.Layers[i].pAlphaMap==NULL)
				return false;
			
		}
	}
	return true;
}

CD3DTexture * CD3DWOWADTModelResource::CreateAlphaMap(int DataType,LPBYTE pData,UINT DataSize,UINT& ProcessSize,UINT ID,UINT Layer)
{
	ProcessSize=0;	

	CSmartPtr<CD3DTexture>  pAlphaMap=new CD3DTexture(m_pManager->GetDevice()->GetTextureManager());
	if(!pAlphaMap->CreateTexture(64,64,D3DFMT_A8))
		return NULL;

	CEasyString Name;
	Name.Format(_T("%s\\LYA_%d_%d"),GetName(),ID,Layer);
	pAlphaMap->SetName(Name);
	
	switch(DataType)
	{
	case ADT_UNCOMPRESS_2048:
		{	
			//PrintD3DLog("4位AlphaMap");
			//if(DataSize>=2048)
			{
				D3DLOCKED_RECT LockedRect;
				if(pAlphaMap->LockBits(0,&LockedRect,NULL,0))
				{
					BYTE * pDest=(BYTE *)LockedRect.pBits;
					BYTE * pSrc=pData;
					for(UINT y=0;y<64;y++)
					{
						for(UINT x=0;x<64;x++)
						{
							pDest[x]=(pSrc[x/2]>>((x%2)*4))<<4;
							if(x==63)
								pDest[x]=pDest[x-1];
						}
						pDest+=LockedRect.Pitch;						
						if(y<62)
							pSrc+=32;
					}
					//CEasyBuffer Buffer(LockedRect.pBits,4096);
					//for(UINT i=0;i<2048;i++)
					//{
					//	Buffer.PushConstBack(((pData[i]&0xF)<<4),1);
					//	Buffer.PushConstBack((pData[i]&0xF0),1);
					//}
					pAlphaMap->UnlockBits(0);
					ProcessSize=2048;
					CD3DTexture * pRet=pAlphaMap;
					pAlphaMap.Detach();					
					return pRet;
				}
				else
				{
					return NULL;
				}
			}
		}
		break;
	case ADT_UNCOMPRESS_4096:
		{
			//PrintD3DLog("8位AlphaMap");
			//if(DataSize>=4096)
			{
				D3DLOCKED_RECT LockedRect;
				if(pAlphaMap->LockBits(0,&LockedRect,NULL,0))
				{
					CEasyBuffer Buffer(LockedRect.pBits,4096);
					Buffer.PushBack(pData,4096);
					pAlphaMap->UnlockBits(0);
					ProcessSize=4096;
					CD3DTexture * pRet=pAlphaMap;
					pAlphaMap.Detach();
					return pRet;
				}
				else
				{
					return NULL;
				}
			}
		}
		break;
	case ADT_COMPRESS:
		{
			//PrintD3DLog("压缩格式AlphaMap");
			D3DLOCKED_RECT LockedRect;
			if(pAlphaMap->LockBits(0,&LockedRect,NULL,0))
			{
				CEasyBuffer Buffer(LockedRect.pBits,4096);
				UINT AlphaCount=0;
				UINT ReadPtr=0;
				while(AlphaCount<4096)
				{
					if(pData[ReadPtr]&0x80)
					{
						BYTE Count=pData[ReadPtr]&0x7F;
						ReadPtr++;
						for(BYTE i=0;i<Count;i++)
						{
							Buffer.PushConstBack(pData[ReadPtr],1);
						}
						//Buffer.PushBack(pData+ReadPtr,Count);
						ReadPtr++;
						AlphaCount+=Count;
					}
					else
					{
						BYTE Count=pData[ReadPtr]&0x7F;
						ReadPtr++;
						Buffer.PushBack(pData+ReadPtr,Count);
						ReadPtr+=Count;
						AlphaCount+=Count;
					}
				}
				pAlphaMap->UnlockBits(0);
				ProcessSize=4096;
				CD3DTexture * pRet=pAlphaMap;
				pAlphaMap.Detach();
				return pRet;
			}
			else
			{
				return NULL;
			}
		}
		break;
	}
	return NULL;
}


bool CD3DWOWADTModelResource::LoadShadowMap(TEXTURE_LAYER_INFO& LayInfo,BLZ_CHUNK_MCSH * pMCSH,UINT ID)
{	
	CSmartPtr<CD3DTexture>  pAlphaMap=new CD3DTexture(m_pManager->GetDevice()->GetTextureManager());
	if(!pAlphaMap->CreateTexture(64,64,D3DFMT_A8))
		return false;

	CEasyString Name;
	Name.Format(_T("%s\\SHW_%d"),GetName(),ID);
	pAlphaMap->SetName(Name);

	D3DLOCKED_RECT LockedRect;
	if(pAlphaMap->LockBits(0,&LockedRect,NULL,0))
	{
		CEasyBuffer Buffer(LockedRect.pBits,4096);
		for(int y=0;y<64;y++)
		{
			UINT64 ShadowData;			
			ShadowData=pMCSH->ShadowMap[y];			
			for(int x=0;x<64;x++)
			{
				if(ShadowData&1)
					Buffer.PushConstBack(0xFF,1);
				else
					Buffer.PushConstBack(0x00,1);				
				ShadowData=ShadowData>>1;
			}
		}		
		pAlphaMap->UnlockBits(0);	
		LayInfo.pShadowMap=pAlphaMap;
		pAlphaMap.Detach();

		return true;
	}
	else
	{
		return false;
	}
}


CD3DTexture * CD3DWOWADTModelResource::LoadLiquidTexture(int LiquidType)
{
	CBLZWOWDatabase::BLZ_DB_LIQUID_TYPE * pLiquidInfo=CBLZWOWDatabase::GetInstance()->GetLiquidTypeInfo(LiquidType);
	if(pLiquidInfo)
	{
		CEasyString Name;
		CD3DIFLTexture * pLiquidTexture=NULL;
		Name.Format(_T("LiquidTexture%d"),LiquidType);
		pLiquidTexture=(CD3DIFLTexture *)m_pManager->GetDevice()->GetTextureManager()->GetTextrue(Name);
		if(pLiquidTexture==NULL)
		{
			pLiquidTexture=new CD3DIFLTexture(m_pManager->GetDevice()->GetTextureManager());
			pLiquidTexture->SetName(Name);
			for(int i=0;i<30;i++)
			{
				CEasyString FileName;

				FileName.Format(pLiquidInfo->TextureList[0],i+1);

				CD3DTexture * pTexture=m_pManager->GetDevice()->GetTextureManager()->LoadTexture(FileName);

				pLiquidTexture->AddFrame(pTexture,1.0f/30);
			}
			m_pManager->GetDevice()->GetTextureManager()->AddTexture(pLiquidTexture,Name);
		}
		else
		{
			pLiquidTexture->AddUseRef();
		}
		return pLiquidTexture;
	}
	return NULL;
}

bool CD3DWOWADTModelResource::WantRenderWater(BYTE * pRenderMask,int Pos)
{
	if(pRenderMask==NULL)
		return true;
	int Byte=Pos/8;
	int Bit=Pos%8;

	return ((pRenderMask[Byte]>>Bit)&1)==1;
}

}