/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWOWWDLModelResource.cpp                               */
/*      创建日期:  2011年06月08日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/

#include "StdAfx.h"

namespace D3DLib{

const WORD	WDL_HOLE_MASK[16]={	
	0x1,	0x2,	0x4,	0x8,
	0x10,	0x20,	0x40,	0x80,
	0x100,	0x200,	0x400,	0x800,
	0x1000,0x2000,	0x4000,	0x8000};

IMPLEMENT_CLASS_INFO(CD3DWOWWDLModelResource,CD3DObjectResource);

CD3DWOWWDLModelResource::CD3DWOWWDLModelResource(void)
{
}

CD3DWOWWDLModelResource::CD3DWOWWDLModelResource(CD3DObjectResourceManager* pManager)
	:CD3DObjectResource(pManager)
{

}

CD3DWOWWDLModelResource::~CD3DWOWWDLModelResource(void)
{
	Destory();
}

void CD3DWOWWDLModelResource::Destory()
{
	CD3DObjectResource::Destory();
}

bool CD3DWOWWDLModelResource::Reset()
{
	return CD3DObjectResource::Reset();
}
bool CD3DWOWWDLModelResource::Restore()
{
	return CD3DObjectResource::Restore();
}

bool CD3DWOWWDLModelResource::LoadFromFile(LPCTSTR ModelFileName)
{
	IFileAccessor * pFile;	

	Destory();

	pFile=CD3DWOWWMOModel::CreateFileAccessor();
	if(pFile==NULL)
		return false;
	if(!pFile->Open(ModelFileName,IFileAccessor::modeRead))
	{
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

	BLZ_CHUNK_MWMO * pMWMO=(BLZ_CHUNK_MWMO *)ADTChunk.GetFirstChunk(CHUNK_ID_WDL_MWMO);
	BLZ_CHUNK_MWID * pMWID=(BLZ_CHUNK_MWID *)ADTChunk.GetFirstChunk(CHUNK_ID_WDL_MWID);
	BLZ_CHUNK_MODF * pMODF=(BLZ_CHUNK_MODF *)ADTChunk.GetFirstChunk(CHUNK_ID_WDL_MODF);
	BLZ_CHUNK_MAOF * pMAOF=(BLZ_CHUNK_MAOF *)ADTChunk.GetFirstChunk(CHUNK_ID_WDL_MAOF);

	UINT IndexCount=3*4*16*16;

	UINT VertexCount=545;

	for(UINT Y=0;Y<64;Y++)
	{
		for(UINT X=0;X<64;X++)
		{
			if(pMAOF->AreaLowOffsets[Y][X])
			{
				BLZ_CHUNK_MARE * pMARE=(BLZ_CHUNK_MARE *)ADTChunk.GetChunkByOffset(CHUNK_ID_WDL_MARE,pMAOF->AreaLowOffsets[Y][X]);
				if(pMARE)
				{
					BLZ_CHUNK_MAHO * pMAHO=(BLZ_CHUNK_MAHO *)ADTChunk.GetChunkByOffset(CHUNK_ID_WDL_MAHO,pMAOF->AreaLowOffsets[Y][X]+sizeof(BLZ_CHUNK_MARE));
					if(pMAHO)
					{
						CD3DSubMesh * pD3DSubMesh=new CD3DSubMesh(m_pManager->GetDevice());

						pD3DSubMesh->GetVertexFormat().FVF=D3DFVF_XYZ;
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

						pD3DSubMesh->GetMaterial().SetFX(m_pManager->GetDevice()->GetFXManager()->LoadFXFromMemory(_T("WDL_MODEL_FX"),WDL_MODEL_FX,strlen(WDL_MODEL_FX)));

						WORD * pModelIndices=NULL;
						pD3DSubMesh->GetDXIndexBuffer()->Lock(0,0,(LPVOID *)&pModelIndices,0);

						MODEL_VERTEXT * pModelVertices=NULL;
						pD3DSubMesh->GetDXVertexBuffer()->Lock(0,0,(LPVOID *)&pModelVertices,0);


						CD3DVector3 Pos;
						Pos.x=X*BLZ_ADT_MAP_AREA_SIZE;
						Pos.z=-(Y*BLZ_ADT_MAP_AREA_SIZE);
						Pos.y=0;

						
						float Pitch=BLZ_ADT_MAP_AREA_SIZE/32;

						UINT VertexIndex=0;
						UINT InnerIndex=0;
						UINT OuterIndex=0;
						int Line=0;
						while(VertexIndex<545)
						{
							if(Line&1)
							{
								//16 vertices/line
								for(int x=0;x<16;x++)
								{
									pModelVertices[VertexIndex].Pos.x=Pitch+Pitch*x*2;
									pModelVertices[VertexIndex].Pos.z=-Pitch*Line;					
									pModelVertices[VertexIndex].Pos.y=pMARE->HeightmapInner[InnerIndex];
									pModelVertices[VertexIndex].Pos+=Pos;
					
									InnerIndex++;
									VertexIndex++;
								}

							}
							else
							{
								//17 vertices/line
								for(int x=0;x<17;x++)
								{
									pModelVertices[VertexIndex].Pos.x=Pitch*x*2;
									pModelVertices[VertexIndex].Pos.z=-Pitch*Line;
									pModelVertices[VertexIndex].Pos.y=pMARE->HeightmapOuter[OuterIndex];
									pModelVertices[VertexIndex].Pos+=Pos;

									OuterIndex++;
									VertexIndex++;
								}
							}
							Line++;
						}	


						UINT IndexIndex=0;
						for(int y=0;y<16;y++)
						{
							for(int x=0;x<16;x++)
							{
								if((WDL_HOLE_MASK[x]&((WORD)pMAHO->Holes[y]))==0)
								{
									pModelIndices[IndexIndex]	=(y*16+(y+1)*17)+x;
									pModelIndices[IndexIndex+1]	=(y*16+(y+1)*17)+x+16;
									pModelIndices[IndexIndex+2]	=(y*16+(y+1)*17)+x-17;


									pModelIndices[IndexIndex+3]	=(y*16+(y+1)*17)+x;
									pModelIndices[IndexIndex+4]	=(y*16+(y+1)*17)+x+17;
									pModelIndices[IndexIndex+5]	=(y*16+(y+1)*17)+x+16;


									pModelIndices[IndexIndex+6]	=(y*16+(y+1)*17)+x;
									pModelIndices[IndexIndex+7]	=(y*16+(y+1)*17)+x-16;
									pModelIndices[IndexIndex+8]	=(y*16+(y+1)*17)+x+17;


									pModelIndices[IndexIndex+9]	=(y*16+(y+1)*17)+x;
									pModelIndices[IndexIndex+10]=(y*16+(y+1)*17)+x-17;
									pModelIndices[IndexIndex+11]=(y*16+(y+1)*17)+x-16;				

									IndexIndex+=3*4;
								}
							}
						}

						pD3DSubMesh->SetIndexCount(IndexIndex);
						pD3DSubMesh->SetPrimitiveCount(IndexIndex/3);


						pD3DSubMesh->GetDXVertexBuffer()->Unlock();

						pD3DSubMesh->GetDXIndexBuffer()->Unlock();

						pD3DSubMesh->SetID(Y*100+X);
						CEasyString SubMeshName;
						SubMeshName.Format(_T("[%d,%d]"),X,Y);
						pD3DSubMesh->SetName(SubMeshName);


						m_SubMeshList.Add(pD3DSubMesh);
					}
					else
					{
						PrintD3DLog(_T("CD3DWOWWDLModelResource::LoadFromFile:区块[%d,%d]没有洞信息"),X,Y);
					}
				}
			}
		}
	}

	CreateBounding();

	return true;
}


void CD3DWOWWDLModelResource::PickResource(CUSOResourceManager * pResourceManager,UINT Param)
{

}
bool CD3DWOWWDLModelResource::ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	return false;
}
bool CD3DWOWWDLModelResource::FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	return false;
}
UINT CD3DWOWWDLModelResource::GetSmartStructSize(UINT Param)
{
	return 0;
}

}