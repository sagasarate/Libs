/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DIFLTexture.cpp                                        */
/*      创建日期:  2009年09月25日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

namespace D3DLib{

IMPLEMENT_CLASS_INFO(CD3DIFLTexture,CD3DTexture);

CD3DIFLTexture::CD3DIFLTexture():CD3DTexture()
{
	m_CurFrame=0;	
	m_FlipStartTime=0.0f;
}

CD3DIFLTexture::CD3DIFLTexture(CD3DTextureManager * pManager):CD3DTexture(pManager)
{
	m_CurFrame=0;
	m_FlipStartTime=0.0f;
	if(m_pManager)
	{
		m_pManager->AddAniTexture(this);
	}
}

CD3DIFLTexture::~CD3DIFLTexture(void)
{
	if(m_pManager)
	{
		m_pManager->DelAniTexture(this);
	}
	Destory();
}

bool CD3DIFLTexture::LoadTexture(LPCTSTR TextureFileName,UINT MipLevels,bool UseFilter,bool IsManaged,D3DCOLOR KeyColor)
{	
	CEasyString Temp;

	if(m_pManager==NULL)
		return false;
	Temp=FindFileOne(TextureFileName);
#ifdef _DEBUG
	PrintD3DLog(_T("装载IFL纹理<%s>..."), Temp);
#endif

	CStringFile	Lines(Temp,m_FileChannel);
	
	for(int i=0;i<Lines.GetLineCount();i++)
	{
		TCHAR Line[MAX_PATH+1];
		_tcscpy_s(Line,MAX_PATH,Lines[i]);
		
		TCHAR * time=_tcschr(Line,' ');
		if(time)
		{
			TEXTURE_FRAME Frame;
			time[0]=0;
			time++;			
			Temp=FindFileOne(Line);
			Frame.pTexture=m_pManager->LoadTexture(Temp,MipLevels,UseFilter,IsManaged,KeyColor);
			if(Frame.pTexture)
			{
				Frame.FrameTime=(FLOAT)_tstof(time);
				m_TextrueList.Add(Frame);
			}
			else
			{
				SAFE_DELETE(Frame.pTexture);
			}

		}		
	}
	if(m_TextrueList.GetCount())
		return true;
	else
		return false;
}

void CD3DIFLTexture::AddFrame(CD3DTexture *	pTexture,float FrameTime)
{
	if(pTexture)
	{
		TEXTURE_FRAME Frame;

		Frame.pTexture=pTexture;
		Frame.FrameTime=FrameTime;

		m_TextrueList.Add(Frame);
	}
	
}

LPDIRECT3DTEXTURE9 CD3DIFLTexture::GetD3DTexture()
{
	if(m_CurFrame<m_TextrueList.GetCount())
	{		
		return m_TextrueList[m_CurFrame].pTexture->GetD3DTexture();
	}
	return NULL;
}

void CD3DIFLTexture::Update(FLOAT Time)
{
	if(m_TextrueList.GetCount())
	{	
		if(Time-m_FlipStartTime>m_TextrueList[m_CurFrame].FrameTime)
		{
			m_FlipStartTime=Time;
			m_CurFrame++;
			if(m_CurFrame<0)
				m_CurFrame=(int)m_TextrueList.GetCount()-1;
			if(m_CurFrame>=(int)m_TextrueList.GetCount())
				m_CurFrame=0;			
		}	
	}
}



void CD3DIFLTexture::Destory()
{
	for(int i=0;i<(int)m_TextrueList.GetCount();i++)
	{
		SAFE_RELEASE(m_TextrueList[i].pTexture);
	}	
	m_TextrueList.Clear();
	CD3DTexture::Destory();
}

bool CD3DIFLTexture::Reset()
{
	for(int i=0;i<(int)m_TextrueList.GetCount();i++)
	{
		m_TextrueList[i].pTexture->Reset();
	}
	return true;
}

bool CD3DIFLTexture::Restore()
{
	bool Ret=true;
	for(int i=0;i<(int)m_TextrueList.GetCount();i++)
	{
		if(!m_TextrueList[i].pTexture->Restore())
			Ret=false;
	}
	return Ret;
}

int CD3DIFLTexture::GetFrameCount()
{
	return (int)m_TextrueList.GetCount();
}

CD3DTexture * CD3DIFLTexture::GetFrameTexture(int FrameIndex)
{
	if(FrameIndex>=0&&FrameIndex<(int)m_TextrueList.GetCount())
	{
		return m_TextrueList[FrameIndex].pTexture;
	}
	return NULL;
}

void CD3DIFLTexture::PickResource(CUSOResourceManager * pResourceManager,UINT Param)
{
	for(int i=0;i<(int)m_TextrueList.GetCount();i++)
	{
		pResourceManager->AddResource(m_TextrueList[i].pTexture);
	}	
}

bool CD3DIFLTexture::ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CNameObject::ToSmartStruct(Packet,pResourceManager,Param))
		return false;	

	for(UINT i=0;i<m_TextrueList.GetCount();i++)
	{
		UINT BufferSize;
		void * pBuffer=Packet.PrepareMember(BufferSize);
		CSmartStruct SubPacket(pBuffer,BufferSize,true);

		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_FI_TIME,m_TextrueList[i].FrameTime));		
		CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_FI_TEXTURE,m_TextrueList[i].pTexture->GetName()));
		if(!Packet.FinishMember(SST_D3DITEX_FRAME,SubPacket.GetDataLen()))
			return false;
	}

	return true;
}
bool CD3DIFLTexture::FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CNameObject::FromSmartStruct(Packet,pResourceManager,Param))
		return false;

	void * Pos=Packet.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartStruct Value=Packet.GetNextMember(Pos,MemberID);
		switch(MemberID)
		{	
		case SST_D3DITEX_FRAME:
			{
				TEXTURE_FRAME Frame;
				Frame.FrameTime=Value.GetMember(SST_FI_TIME);
				LPCTSTR szResourceName=Value.GetMember(SST_FI_TEXTURE);
				Frame.pTexture=(CD3DTexture *)pResourceManager->FindResource(szResourceName,GET_CLASS_INFO(CD3DTexture));
				if(Frame.pTexture)
				{
					Frame.pTexture->AddUseRef();
					m_TextrueList.Add(Frame);
				}
			}
			break;
		}
	}

	if(m_pManager)
		m_pManager->AddTexture(this,GetName());

	return true;
}

UINT CD3DIFLTexture::GetSmartStructSize(UINT Param)
{
	UINT Size=CNameObject::GetSmartStructSize(Param);

	for(UINT i=0;i<m_TextrueList.GetCount();i++)
	{
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_TextrueList[i].FrameTime));
		Size += CSmartStruct::GetStringMemberSize(m_TextrueList[i].pTexture->GetNameLength());
		Size+=CSmartStruct::GetStructMemberSize(0);
	}	
	return Size;
}





}