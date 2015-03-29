/****************************************************************************/
/*                                                                          */
/*      文件名:    Gif.h                                                    */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


#define GLOBAL_HEAD_SIZE	13
#define LOCAL_HEAD_SIZE		9
#define GIF_IMAGE_BLOCK		1
#define GIF_CONTROL_BLOCK	2
#define GIF_COMMENT_BLOCK	3
#define	GIF_TEXT_BLOCK		4
#define GIF_APP_BLOCK		5
#define GIF_PACK			1
#define GIF_UNPACK			2
#define GIF_READY			3



struct GLOBAL_HEAD
{
	BYTE version[6];
	WORD ScreenWidth;
	WORD ScreenHeight;
	BYTE GlobalFlag;
	BYTE BGColor;
	BYTE PAR;
};

struct LOCAL_HEAD
{
	WORD x;
	WORD y;
	WORD width;
	WORD height;
	BYTE LocalFlag;
};

struct CONTROL_BLOCK
{
	BYTE	introducer;
	BYTE	label;
	BYTE	size;
	BYTE	flag;
	WORD	delay;
	BYTE	BGColor;
	BYTE	zero;
};

class CGif;
class CGifBlock;

typedef CGif		*LPCGif;
typedef CGifBlock	*LPCGifBlock;

class CGifBlock
{
public:
	
	WORD		m_type;
	DWORD		m_size;
	WORD		m_x;
	WORD		m_y;
	WORD		m_width;
	WORD		m_height;
	BYTE		m_flag;
	WORD		m_delay;
	int			m_BGColor;
	DWORD		m_offset;
	LPCGifBlock	m_next;
	LPCGifBlock	m_previous;

	CGifBlock();
	~CGifBlock();
	void Release();
	void ReleaseAll();
	void Add(LPCGifBlock block);
	void AddImageBlock(DWORD offset,DWORD size,WORD x,WORD y,WORD width,WORD height,BYTE flag);
	void AddControlBlock(DWORD offset,DWORD size,WORD delay,int BGColor);
	void AddCommentBlock(DWORD offset,DWORD size);
	void AddTextBlock(DWORD offset,DWORD size);
	void AddAppBlock(DWORD offset,DWORD size);
};

class CGif  
{
public:
	HBITMAP Convert(HBITMAP hbmp,int BitCount);
	int AddControlBlock(int delay,int BGColor);
	int AddImageBlock(int x,int y,HBITMAP hbmp,BOOL SavePal=false);
	int AddImageBlock(int x,int y,int width,int height,char *buff,RGBQUAD *pal=NULL,int PalCount=0);
	int Create(LPCTSTR FileName, int width,int height,int BGColor=-1,RGBQUAD *pal=NULL,int PalCount=0);
	HBITMAP GetBitmap(int index);
	HBITMAP GetMergeBitmap();
	int GetPalette(int index,RGBQUAD *pal);
	int GetDelay(int index);
	int GetBGColor(int index);
	int GetHeight(int index );
	int GetWidth(int index);
	int GetY(int index);
	int GetX(int index);
	int GetBits(int index,char *buff);
	int ReadAppBlock();
	int ReadTextBlock();
	int ReadCommentBlock();
	int ReadControlBlock();
	int ReadImageBlock();
	int ReadBlocks();
	int Open(LPCTSTR FileName);
	int Open(IFileAccessor * pFileAccessor);
	int Close();
	int	GetImageBlockCount()
	{return m_ImageBlockCount;}
	int	GetControlBlockCount()
	{return m_ControlBlockCount;}
	int	GetOtherBlockCount()
	{return m_OtherBlockCount;}
	BOOL HaveGlobalPalette()
	{return m_GlobalPalette;}
	CGif();
	~CGif();

protected:
	BOOL			m_OpenFromHandle;
	int				m_WorkState;
	CLZWPack		m_pack;
	int				m_width;
	int				m_height;
	int				m_BGColor;
	IFileAccessor	*m_pGifFile;
	CGifBlock		m_ImageBlock;
	CGifBlock		m_ControlBlock;
	CGifBlock		m_OtherBlock;
	int				m_ImageBlockCount;
	int				m_ControlBlockCount;
	int				m_OtherBlockCount;
	RGBQUAD			m_pal[256];
	BOOL			m_GlobalPalette;
};

