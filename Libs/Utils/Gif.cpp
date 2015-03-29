/****************************************************************************/
/*                                                                          */
/*      文件名:    Gif.cpp                                                  */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
// Gif.cpp: implementation of the CGif class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "stdlib.h"
#include "Gif.h"






//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGifBlock::CGifBlock()
{
	m_type=0;
	m_size=0;
	m_x=0;
	m_y=0;
	m_width=0;
	m_height=0;
	m_flag=0;
	m_delay=0;
	m_offset=0;
	m_BGColor=-1;
	m_next=NULL;
	m_previous=NULL;
}

CGifBlock::~CGifBlock()
{

}

void CGifBlock::Release()
{
	if(m_next!=NULL) m_next->m_previous=m_previous;
	if(m_previous!=NULL) m_previous->m_next=m_next;
	delete this;
}

void CGifBlock::ReleaseAll()
{
	if(m_next!=NULL) m_next->ReleaseAll();
	Release();
}


void CGifBlock::Add(LPCGifBlock	block)
{
	LPCGifBlock	temp;
	temp=this;
	while(temp->m_next!=NULL)
		temp=temp->m_next;
	block->m_previous=temp;
	temp->m_next=block;
}

void CGifBlock::AddImageBlock(DWORD offset,DWORD size,WORD x,WORD y,WORD width,WORD height,BYTE flag)
{
	LPCGifBlock	block;

	block=new CGifBlock();
	block->m_type=GIF_IMAGE_BLOCK;
	block->m_offset=offset;
	block->m_size=size;
	block->m_x=x;
	block->m_y=y;
	block->m_width=width;
	block->m_height=height;
	block->m_flag=flag;
	Add(block);
}

void CGifBlock::AddControlBlock(DWORD offset,DWORD size,WORD delay,int BGColor)
{
	LPCGifBlock	block;

	block=new CGifBlock();
	block->m_type=GIF_CONTROL_BLOCK;
	block->m_offset=offset;
	block->m_size=size;
	block->m_delay=delay;
	block->m_BGColor=BGColor;
	Add(block);
}

void CGifBlock::AddCommentBlock(DWORD offset,DWORD size)
{
	LPCGifBlock	block;

	block=new CGifBlock();
	block->m_type=GIF_COMMENT_BLOCK;
	block->m_offset=offset;
	block->m_size=size;
	Add(block);
}

void CGifBlock::AddTextBlock(DWORD offset,DWORD size)
{
	LPCGifBlock	block;

	block=new CGifBlock();
	block->m_type=GIF_TEXT_BLOCK;
	block->m_offset=offset;
	block->m_size=size;
	Add(block);
}

void CGifBlock::AddAppBlock(DWORD offset,DWORD size)
{
	LPCGifBlock	block;

	block=new CGifBlock();
	block->m_type=GIF_APP_BLOCK;
	block->m_offset=offset;
	block->m_size=size;
	Add(block);
}

CGif::CGif()
{
	m_pack.SetWorkMode(PACK_GIF_MODE);
	m_width=0;
	m_height=0;
	m_BGColor=0;
	m_pGifFile=NULL;
	m_ImageBlockCount=0;
	m_ControlBlockCount=0;
	m_OtherBlockCount=0;
	m_GlobalPalette=false;
	m_WorkState=GIF_READY;
	m_OpenFromHandle=false;
}

CGif::~CGif()
{
	Close();
}

int CGif::Open(LPCTSTR FileName)
{
	int			ExitCode;
	
	IFileAccessor * pFileAccessor=new CStandardFileAccessor();

	if(!pFileAccessor->Open(FileName,IFileAccessor::modeWrite|IFileAccessor::shareShareRead|IFileAccessor::modeCreateAlways))
	{
		SAFE_RELEASE(pFileAccessor);
		return 1;
	}
	ExitCode=Open(pFileAccessor);
	m_OpenFromHandle=false;
	return ExitCode;
}

int CGif::Open(IFileAccessor * pFileAccessor)
{
	GLOBAL_HEAD	head;
	int			PalSize,i;
	BYTE		pal[768];

	if(m_WorkState!=GIF_READY) return -1;	
	m_pGifFile=pFileAccessor;
	m_OpenFromHandle=true;
	m_WorkState=GIF_UNPACK;
	if(m_pGifFile==NULL) return 1;
	if(m_pGifFile->Read(&head,GLOBAL_HEAD_SIZE)!=GLOBAL_HEAD_SIZE) return 2;
	if(memcmp((LPCTSTR)head.version,"GIF",3)) return 3;
	m_width=head.ScreenWidth;
	m_height=head.ScreenHeight;
	m_BGColor=head.BGColor;
	if(head.GlobalFlag&0x80)
	{
		PalSize=1<<((head.GlobalFlag&7)+1);
		if(m_pGifFile->Read(pal,3*PalSize)!=(DWORD)PalSize*3) return 4;
		ZeroMemory(m_pal,sizeof(RGBQUAD)*256);
		for(i=0;i<PalSize;i++)
		{
			m_pal[i].rgbRed=pal[i*3];
			m_pal[i].rgbGreen=pal[i*3+1];
			m_pal[i].rgbBlue=pal[i*3+2];
		}
		m_GlobalPalette=true;
	}
	else m_GlobalPalette=false;
	return ReadBlocks();
}

int CGif::Close()
{
	if(m_pGifFile==NULL) return 1;
	if(m_WorkState==GIF_PACK)
	{
		BYTE	ch=0x3B;
		m_pGifFile->Write(&ch,1);
	}
	if(!m_OpenFromHandle)
		SAFE_RELEASE(m_pGifFile);
	m_width=0;
	m_height=0;
	m_BGColor=0;
	m_pGifFile=NULL;
	m_ImageBlockCount=0;
	m_ControlBlockCount=0;
	m_OtherBlockCount=0;
	if(m_ImageBlock.m_next!=NULL)
		m_ImageBlock.m_next->ReleaseAll();
	if(m_ControlBlock.m_next!=NULL)
		m_ControlBlock.m_next->ReleaseAll();
	if(m_OtherBlock.m_next!=NULL) 
		m_OtherBlock.m_next->ReleaseAll();
	m_ImageBlock.m_next=NULL;
	m_ControlBlock.m_next=NULL;
	m_OtherBlock.m_next=NULL;
	m_WorkState=GIF_READY;
	return 0;
}

int CGif::ReadBlocks()
{
	BYTE	ch;
	BOOL	BlockEnd=false;

	if(m_pGifFile==NULL) return 1;
	while(!BlockEnd)
	{
		if(m_pGifFile->Read(&ch,1)<1) break;
		switch(ch)
		{
		case 0x2c:
			if(ReadImageBlock()) return 2;
			break;
		case 0x3b:
			BlockEnd=true;
			break;
		case 0x21:
			if(m_pGifFile->Read(&ch,1)<1) break;
			switch(ch)
			{
			case 0xf9:
				if(ReadControlBlock()) return 3;
				break;
			case 0xfe:
				if(ReadCommentBlock()) return 4;
				break;
			case 0x01:
				if(ReadTextBlock()) return 4;
				break;
			case 0xff:
				if(ReadAppBlock()) return 4;
				break;
			}
			break;
		}
	}
	return 0;
}

int CGif::ReadImageBlock()
{
	int			offset;
	int			size;
	int			PalSize;
	BYTE		ch;
	LOCAL_HEAD	head;

	if(m_pGifFile==NULL) return 1;
	offset=(int)m_pGifFile->GetCurPos();
	size=0;
	if(m_pGifFile->Read(&head,LOCAL_HEAD_SIZE)!=LOCAL_HEAD_SIZE) return 2;
	if(head.LocalFlag&0x80)
	{
		PalSize=1<<((head.LocalFlag&7)+1);
		m_pGifFile->Seek(PalSize*3,IFileAccessor::seekCurrent);
		size+=PalSize;
	}
	m_pGifFile->Read(&ch,1);
	size=LOCAL_HEAD_SIZE+1;
	while(1)
	{
		if(m_pGifFile->Read(&ch,1)<1) break;
		if(ch==0) 
		{
			size++;
			break;
		}
		m_pGifFile->Seek(ch,IFileAccessor::seekCurrent);
		size+=ch;
	}
	m_ImageBlock.AddImageBlock(offset,size,head.x,head.y,head.width,head.height,head.LocalFlag);
	m_ImageBlockCount++;
	return 0;
}


int CGif::ReadControlBlock()
{
	int			offset;
	int			size;
	int			delay;
	int			BGColor;
	BYTE		flag;
	BYTE		ch;

	if(m_pGifFile==NULL) return 1;
	m_pGifFile->Read(&ch,1);
	size=ch;
	offset=(int)m_pGifFile->GetCurPos();
	m_pGifFile->Read(&flag,1);
	m_pGifFile->Read(&ch,1);
	delay=ch;
	m_pGifFile->Read(&ch,1);
	delay+=(WORD)ch<<8;
	m_pGifFile->Read(&ch,1);
	BGColor=ch;
	if((flag&1)==0) BGColor=-1;
	m_ControlBlock.AddControlBlock(offset,size,delay,BGColor);
	m_ControlBlockCount++;
	m_pGifFile->Read(&ch,1);
	return 0;
}

int CGif::ReadCommentBlock()
{
	int			offset;
	int			size;
	BYTE		ch;

	if(m_pGifFile==NULL) return 1;
	offset=(int)m_pGifFile->GetCurPos();
	size=0;
	while(1)
	{
		if(m_pGifFile->Read(&ch,1)<1) break;
		size++;
		if(ch==0) break;
	}
	m_OtherBlock.AddCommentBlock(offset,size);
	m_OtherBlockCount++;
	return 0;
}

int CGif::ReadTextBlock()
{
	int			offset;
	int			size;
	BYTE		ch;

	if(m_pGifFile==NULL) return 1;
	m_pGifFile->Read(&ch,1);
	size=ch;
	offset=(int)m_pGifFile->GetCurPos();
	m_pGifFile->Seek(size,IFileAccessor::seekCurrent);
	while(1)
	{
		if(m_pGifFile->Read(&ch,1)<1) break;
		if(ch==0) 
		{
			size++;
			break;
		}
		m_pGifFile->Seek(ch,IFileAccessor::seekCurrent);
		size+=ch;
	}
	m_OtherBlock.AddTextBlock(offset,size);
	m_OtherBlockCount++;
	return 0;
}

int CGif::ReadAppBlock()
{
	int			offset;
	int			size;
	BYTE		ch;

	if(m_pGifFile==NULL) return 1;
	m_pGifFile->Read(&ch,1);
	size=ch;
	offset=(int)m_pGifFile->GetCurPos();
	m_pGifFile->Seek(size,IFileAccessor::seekCurrent);
	while(1)
	{
		if(m_pGifFile->Read(&ch,1)<1) break;
		if(ch==0) 
		{
			size++;
			break;
		}
		m_pGifFile->Seek(ch,IFileAccessor::seekCurrent);
		size+=ch;
	}
	m_OtherBlock.AddAppBlock(offset,size);
	m_OtherBlockCount++;
	return 0;
}

int CGif::GetBits(int index, char * buff)
{
	LPCGifBlock	ImageBlock;
	int			offset;
	

	if(index<0||index>=m_ImageBlockCount) return 1;
	ImageBlock=m_ImageBlock.m_next;
	while(index&&ImageBlock!=NULL)
	{
		ImageBlock=ImageBlock->m_next;
		index--;
	}
	if(ImageBlock==NULL) return 2;
	offset=ImageBlock->m_offset+LOCAL_HEAD_SIZE;
	if(ImageBlock->m_flag&0x80)
		offset+=(1<<((ImageBlock->m_flag&7)+1))*3;
	m_pGifFile->Seek(offset,IFileAccessor::seekBegin);
	if(m_pack.StartUnpack(m_pGifFile)) return 3;
	m_pack.Unpack(buff,ImageBlock->m_width*ImageBlock->m_height);
	if(m_pack.EndUnpack()) return 5;
	return 0;
}

int CGif::GetY(int index)
{
	LPCGifBlock	ImageBlock;

	if(index<0||index>=m_ImageBlockCount) return 0;
	ImageBlock=m_ImageBlock.m_next;
	while(index&&ImageBlock!=NULL)
	{
		ImageBlock=ImageBlock->m_next;
		index--;
	}
	if(ImageBlock==NULL) return 0;
	return ImageBlock->m_x;
}

int CGif::GetX(int index)
{
	LPCGifBlock	ImageBlock;

	if(index<0||index>=m_ImageBlockCount) return 0;
	ImageBlock=m_ImageBlock.m_next;
	while(index&&ImageBlock!=NULL)
	{
		ImageBlock=ImageBlock->m_next;
		index--;
	}
	if(ImageBlock==NULL) return 0;
	return ImageBlock->m_y;
}

int CGif::GetWidth(int index)
{
	LPCGifBlock	ImageBlock;

	if(index<0||index>=m_ImageBlockCount) return m_width;
	ImageBlock=m_ImageBlock.m_next;
	while(index&&ImageBlock!=NULL)
	{
		ImageBlock=ImageBlock->m_next;
		index--;
	}
	if(ImageBlock==NULL) return m_width;
	return ImageBlock->m_width;
}

int CGif::GetHeight(int index)
{
	LPCGifBlock	ImageBlock;

	if(index<0||index>=m_ImageBlockCount) return m_height;
	ImageBlock=m_ImageBlock.m_next;
	while(index&&ImageBlock!=NULL)
	{
		ImageBlock=ImageBlock->m_next;
		index--;
	}
	if(ImageBlock==NULL) return m_height;
	return ImageBlock->m_height;
}

int CGif::GetBGColor(int index)
{
	LPCGifBlock	ControlBlock;
	int			BGColor;
	RGBQUAD		pal[256];

	if(index<0||index>=m_ControlBlockCount) 
	{
		BGColor=m_BGColor;
	}
	else
	{
		ControlBlock=m_ControlBlock.m_next;
		while(index&&ControlBlock!=NULL)
		{
			ControlBlock=ControlBlock->m_next;
			index--;
		}
		if(ControlBlock==NULL) BGColor=m_BGColor;
		else BGColor=ControlBlock->m_BGColor;
	}
	if(BGColor>=0&&BGColor<=255)
	{
		GetPalette(index,pal);
		return RGB(pal[BGColor].rgbRed,pal[BGColor].rgbGreen,pal[BGColor].rgbBlue);
	}
	else return -1;

}

int CGif::GetDelay(int index)
{
	LPCGifBlock	ControlBlock;

	if(index<0||index>=m_ControlBlockCount) return 0;
	ControlBlock=m_ControlBlock.m_next;
	while(index&&ControlBlock!=NULL)
	{
		ControlBlock=ControlBlock->m_next;
		index--;
	}
	if(ControlBlock==NULL) return 0;
	return ControlBlock->m_delay;

}

int CGif::GetPalette(int index, RGBQUAD * pal)
{
	LPCGifBlock	ImageBlock;
	int			offset,PalSize,i;
	BYTE		buff[768];
	CLZWPack		pack;

	memcpy(pal,m_pal,sizeof(RGBQUAD)*256);
	if(index<0||index>=m_ImageBlockCount) return 0;
	ImageBlock=m_ImageBlock.m_next;
	while(index&&ImageBlock!=NULL)
	{
		ImageBlock=ImageBlock->m_next;
		index--;
	}
	if(ImageBlock==NULL) return 0;
	offset=ImageBlock->m_offset+LOCAL_HEAD_SIZE;
	if(ImageBlock->m_flag&0x80)	
	{
		PalSize=1<<((ImageBlock->m_flag&7)+1);
		m_pGifFile->Seek(offset,IFileAccessor::seekBegin);
		if(m_pGifFile->Read(buff,3*PalSize)!=(DWORD)PalSize*3) return 1;
		for(i=0;i<PalSize;i++)
		{
			pal[i].rgbRed=buff[i*3];
			pal[i].rgbGreen=buff[i*3+1];
			pal[i].rgbBlue=buff[i*3+2];
		}
	}
	return 0;
}

HBITMAP CGif::GetBitmap(int index)
{
	HDC hdc;
	LPSTR bits,temp1,temp2;
	HBITMAP hbmp;
	BITMAPINFO *bih;
	int i,width,height;
	int LineWidth;

	width=GetWidth(index);
	height=GetHeight(index);
	LineWidth=width;
	if(LineWidth&1) LineWidth++;
	if(LineWidth&3) LineWidth+=2;
	bits=(LPSTR)malloc(width*height);
	if(GetBits(index,bits))
	{
		free(bits);
		return NULL;
	}
	bih=(BITMAPINFO *)malloc(sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*256);
	GetPalette(index,bih->bmiColors);
	bih->bmiHeader.biSize=sizeof(bih->bmiHeader);
	bih->bmiHeader.biWidth=width;
	bih->bmiHeader.biHeight=height;
	bih->bmiHeader.biPlanes=1;
	bih->bmiHeader.biBitCount=8;
	bih->bmiHeader.biCompression=0;
	bih->bmiHeader.biSizeImage=0;
	bih->bmiHeader.biXPelsPerMeter=0;
	bih->bmiHeader.biYPelsPerMeter=0;
	bih->bmiHeader.biClrUsed=0;
	bih->bmiHeader.biClrImportant=0;
	hdc=GetDC(NULL);
	if(hdc==NULL)
	{
		free(bits);
		free(bih);
		return NULL;
	}
	hbmp=CreateDIBSection(hdc,bih,DIB_RGB_COLORS,(void **)&temp1,NULL,0);
	if(hbmp==NULL)
	{
		free(bits);
		free(bih);
		return NULL;
	}
	temp1+=LineWidth*(height-1);
	temp2=bits;
	for(i=0;i<height;i++)
	{
		memcpy(temp1,temp2,width);
		temp1-=LineWidth;
		temp2+=width;
	}
	free(bits);
	free(bih);
	ReleaseDC(NULL,hdc);
	return hbmp;
}

HBITMAP CGif::GetMergeBitmap()
{
	HDC hdc;
	LPSTR bits,temp1,temp2;
	HBITMAP hbmp;
	BITMAPINFO *bih;
	int i,width,height;
	int LineWidth,BlockSize;;

	width=GetWidth(0);
	height=GetHeight(0)*GetImageBlockCount();
	LineWidth=width;
	if(LineWidth&1) LineWidth++;
	if(LineWidth&3) LineWidth+=2;
	BlockSize=width*GetHeight(0);
	bits=(LPSTR)malloc(width*height);
	temp1=bits;
	for(i=0;i<GetImageBlockCount();i++)
	{
		if(GetBits(i,temp1))
		{
			free(bits);
			return NULL;
		}
		temp1+=BlockSize;
	}
	bih=(BITMAPINFO *)malloc(sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*256);
	GetPalette(0,bih->bmiColors);
	bih->bmiHeader.biSize=sizeof(bih->bmiHeader);
	bih->bmiHeader.biWidth=width;
	bih->bmiHeader.biHeight=height;
	bih->bmiHeader.biPlanes=1;
	bih->bmiHeader.biBitCount=8;
	bih->bmiHeader.biCompression=0;
	bih->bmiHeader.biSizeImage=0;
	bih->bmiHeader.biXPelsPerMeter=0;
	bih->bmiHeader.biYPelsPerMeter=0;
	bih->bmiHeader.biClrUsed=0;
	bih->bmiHeader.biClrImportant=0;
	hdc=GetDC(NULL);
	if(hdc==NULL)
	{
		free(bits);
		free(bih);
		return NULL;
	}
	hbmp=CreateDIBSection(hdc,bih,DIB_RGB_COLORS,(void **)&temp1,NULL,0);
	if(hbmp==NULL)
	{
		free(bits);
		free(bih);
		return NULL;
	}
	temp1+=LineWidth*(height-1);
	temp2=bits;
	for(i=0;i<height;i++)
	{
		memcpy(temp1,temp2,width);
		temp1-=LineWidth;
		temp2+=width;
	}
	free(bits);
	free(bih);
	ReleaseDC(NULL,hdc);
	return hbmp;
}

int CGif::Create(LPCTSTR FileName, int width,int height,int BGColor,RGBQUAD * pal, int PalCount)
{
	GLOBAL_HEAD	head;
	int			i,PalSize;
	BYTE		ch=0;

	if(m_WorkState!=GIF_READY) return -1;
	m_WorkState=GIF_PACK;

	m_pGifFile=new CStandardFileAccessor();
	if(!m_pGifFile->Open(FileName,IFileAccessor::modeWrite|IFileAccessor::shareShareRead|IFileAccessor::modeCreateAlways))
	{
		SAFE_RELEASE(m_pGifFile);
		return 1;
	}
	strncpy_s((char *)head.version,6,"GIF89a",6);
	head.ScreenWidth=width;
	head.ScreenHeight=height;
	head.PAR=0;
	if(BGColor>=0) 
		head.BGColor=BGColor;
	else 
		head.BGColor=0;
	if(pal!=NULL)
	{
		i=PalCount;
		PalSize=0;
		while(i)
		{
			PalSize++;
			i=i>>1;
		}
		PalSize--;
		head.GlobalFlag=0xf0|PalSize-1;
		PalSize=1<<PalSize;
	}
	else head.GlobalFlag=0x70;
	if(m_pGifFile->Write(&head,GLOBAL_HEAD_SIZE)!=GLOBAL_HEAD_SIZE) return 2;
	if(pal!=NULL)
	{
		for(i=0;i<PalSize;i++)
		{
			if(i<PalCount)
			{
				if(m_pGifFile->Write(&(pal[i].rgbRed),1)<1) return 3;
				if(m_pGifFile->Write(&(pal[i].rgbGreen),1)<1) return 3;
				if(m_pGifFile->Write(&(pal[i].rgbBlue),1)<1) return 3;
			}
			else
			{
				if(m_pGifFile->Write(&ch,1)<1) return 3;
				if(m_pGifFile->Write(&ch,1)<1) return 3;
				if(m_pGifFile->Write(&ch,1)<1) return 3;
			}
		}
	}
	
	return 0;
}

int CGif::AddImageBlock(int x, int y, int width, int height, char * buff, RGBQUAD * pal, int PalCount)
{
	LOCAL_HEAD	head;
	int		i,PalSize;
	BYTE	ch;

	if(m_WorkState!=GIF_PACK) return -1;
	head.x=x;
	head.y=y;
	head.width=width;
	head.height=height;
	if(pal!=NULL)
	{
		i=PalCount;
		PalSize=0;
		while(i)
		{
			PalSize++;
			i=i>>1;
		}
		PalSize--;
		head.LocalFlag=0x80|(PalSize-1);
		PalSize=1<<PalSize;
	}
	else head.LocalFlag=0;
	ch=0x2c;
	if(m_pGifFile->Write(&ch,1)<1) return 1;
	if(m_pGifFile->Write(&head,LOCAL_HEAD_SIZE)!=LOCAL_HEAD_SIZE) return 2;
	ch=0;
	if(pal!=NULL)
	{
		for(i=0;i<PalSize;i++)
		{
			if(i<PalCount)
			{
				if(m_pGifFile->Write(&(pal[i].rgbRed),1)<1) return 3;
				if(m_pGifFile->Write(&(pal[i].rgbGreen),1)<1) return 3;
				if(m_pGifFile->Write(&(pal[i].rgbBlue),1)<1) return 3;
			}
			else
			{
				if(m_pGifFile->Write(&ch,1)<1) return 3;
				if(m_pGifFile->Write(&ch,1)<1) return 3;
				if(m_pGifFile->Write(&ch,1)<1) return 3;
			}
		}
	}
	if(m_pack.StartPack(m_pGifFile,8,12)) return 4;
	if(m_pack.Pack(buff,width*height)) return 5;
	if(m_pack.EndPack()) return 6;
	return 0;
}

int CGif::AddImageBlock(int x, int y, HBITMAP hbmp, BOOL SavePal)
{
	BITMAP	bmp;
	BITMAPINFO *bih;
	char * buff,*BmpBuff;
	char *temp1,*temp2;
	HDC	hdc;
	int	result,i;

	if(GetObject(hbmp,sizeof(BITMAP),&bmp)==0) return 7;
	bih=(BITMAPINFO *)malloc(sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*256);
	bih->bmiHeader.biSize=sizeof(bih->bmiHeader);
	bih->bmiHeader.biWidth=bmp.bmWidth;
	bih->bmiHeader.biHeight=bmp.bmHeight;
	bih->bmiHeader.biPlanes=1;
	bih->bmiHeader.biBitCount=8;
	bih->bmiHeader.biCompression=0;
	bih->bmiHeader.biSizeImage=0;
	bih->bmiHeader.biXPelsPerMeter=0;
	bih->bmiHeader.biYPelsPerMeter=0;
	bih->bmiHeader.biClrUsed=0;
	bih->bmiHeader.biClrImportant=0;
	hdc=::GetDC(NULL);
	if(hdc==NULL)
	{
		free(bih);
		return 8;
	}
	buff=(char *)malloc(bmp.bmWidth*bmp.bmHeight);
	BmpBuff=(char *)malloc(bmp.bmWidthBytes*bmp.bmHeight);
	if(GetDIBits(hdc,hbmp,0,bmp.bmHeight,BmpBuff,bih,DIB_RGB_COLORS)!=bmp.bmHeight)
	{		
		free(bih);
		free(buff);
		free(BmpBuff);
		return 9;
	}
	::ReleaseDC(NULL,hdc);
	temp1=buff;
	temp2=BmpBuff+(bmp.bmHeight-1)*bmp.bmWidthBytes;
	for(i=0;i<bmp.bmHeight;i++)
	{
		memcpy(temp1,temp2,bmp.bmWidth);
		temp1+=bmp.bmWidth;
		temp2-=bmp.bmWidthBytes;
	}
	if(SavePal)
		result=AddImageBlock(x,y,bmp.bmWidth,bmp.bmHeight,buff,bih->bmiColors,256);
	else
		result=AddImageBlock(x,y,bmp.bmWidth,bmp.bmHeight,buff);
	free(bih);
	free(buff);
	free(BmpBuff);
	if(result==bmp.bmHeight) return 0;
	else return 10;
}

int CGif::AddControlBlock(int delay, int BGColor)
{
	CONTROL_BLOCK	block;

	block.introducer=0x21;
	block.label=0xf9;
	block.size=4;
	block.zero=0;
	block.delay=delay;
	if(BGColor>=0)
	{
		block.flag=1;
		block.BGColor=BGColor;
	}
	else
	{
		block.flag=0;
		block.BGColor=0;
	}
	if(m_pGifFile->Write(&block,sizeof(CONTROL_BLOCK))!=sizeof(CONTROL_BLOCK)) return 1;
	return 0;
}

HBITMAP CGif::Convert(HBITMAP hbmp, int BitCount)
{
	BITMAP				bmp;
	BITMAPINFOHEADER	bih;
	HDC					hdc,hdc1,hdc2;
	HBITMAP				hbmp24;
	HGDIOBJ				old1,old2;
	char				*bits;

	if(GetObject(hbmp,sizeof(BITMAP),&bmp)==0) return NULL;
	bih.biSize=sizeof(bih);
	bih.biWidth=bmp.bmWidth;
	bih.biHeight=bmp.bmHeight;
	bih.biPlanes=1;
	bih.biBitCount=BitCount;
	bih.biCompression=0;
	bih.biSizeImage=0;
	bih.biXPelsPerMeter=0;
	bih.biYPelsPerMeter=0;
	bih.biClrUsed=0;
	bih.biClrImportant=0;
	hdc=::GetDC(NULL);
	if(hdc==NULL) return NULL;
	hbmp24=CreateDIBSection(hdc,(BITMAPINFO *)&bih,DIB_RGB_COLORS,(void **)&bits,NULL,0);
	::ReleaseDC(NULL,hdc);
	if(hbmp24!=NULL)
	{
		hdc1=CreateCompatibleDC(NULL);
		hdc2=CreateCompatibleDC(NULL);
		old1=SelectObject(hdc1,hbmp);
		old2=SelectObject(hdc2,hbmp24);
		if(BitBlt(hdc2,0,0,bmp.bmWidth,bmp.bmHeight,hdc1,0,0,SRCCOPY)==0)
		{
			DeleteObject(hbmp24);
			hbmp24=NULL;
		}
		SelectObject(hdc1,old1);
		SelectObject(hdc2,old2);
		::DeleteDC(hdc1);
		::DeleteDC(hdc2);
	}	
	return hbmp24;
}
