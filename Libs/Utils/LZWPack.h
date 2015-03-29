/****************************************************************************/
/*                                                                          */
/*      文件名:    LZWPack.h                                                */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


#define FILE_STREAM			1
#define MEMORY_STREAM		2
#define PACK_SUCCESS		0
#define PACK_FAILED			1
#define PACK_OPEN			1
#define PACK_COPY			2
#define PACK_READY			1
#define PACK_PACKING		2
#define PACK_UNPACKING		3
#define PACK_GIF_MODE		1
#define PACK_NORMAL_MODE	2


class CLZWPack;

typedef CLZWPack	*LPCLZWPack;
typedef int (* OUTFUN)(LPCLZWPack pack,BYTE ch,LPVOID param);
typedef int (* INFUN)(LPCLZWPack pack,BYTE& ch,LPVOID param);
typedef int CODE_TYPE;

class CLZWPack  
{
	CODE_TYPE		m_OldCode;
	CODE_TYPE		m_NextCode;
	CODE_TYPE		m_NextCodeLimit;
	WORD			m_CodeLength;
	WORD			m_MaxCodeLength;
	WORD			m_CodeBit;
	WORD			m_StartBits;

	WORD			m_EOI;
	WORD			m_CLEAR;
	
	DWORD			m_OutCharCount;
	DWORD			m_CodeBufferSize;

	CODE_TYPE 		*m_pCTlink;
	BYTE			*m_pCTfirst;
	BYTE			*m_pCTlast;	
	CODE_TYPE		*m_pCTnext;
	IFileAccessor	*m_pFileAccessor;

	BYTE			*m_pOutBuffer;
	DWORD			m_OutBufferSize;
	DWORD			m_OutBufferPtr;

	BYTE			*m_pInBuffer;
	DWORD			m_InBufferSize;
	DWORD			m_InBufferPtr;

	BYTE			*m_pOStack;
	int				m_OStackPtr;
	
	BYTE			m_CodeChar;
	WORD			m_Index;
	BYTE			m_buffer256[256];

	WORD			m_FileFrom;
	WORD			m_PackState;
	WORD			m_WorkMode;
	
	OUTFUN			m_OutFun;
	INFUN			m_InFun;
	LPVOID			m_InFunParam;
	LPVOID			m_OutFunParam;

	
public:
	CLZWPack();
	virtual ~CLZWPack();

	void SetMaxCodeLength(WORD length);

	void SetWorkMode(WORD mode)
	{
		m_WorkMode=mode;
	}
	WORD GetWorkMode()
	{
		return m_WorkMode;
	}

	OUTFUN SetOutFunction(OUTFUN fun,LPVOID param);
	INFUN SetInFunction(INFUN fun,LPVOID param);


	
	int StartPack(LPCTSTR FileName,WORD StartBits,WORD MaxBits);
	int StartPack(IFileAccessor	*pFileAccessor,WORD StartBits,WORD MaxBits);
	int StartPack(void * pData,DWORD DataSize,WORD StartBits,WORD MaxBits);
	int Pack(void *data,DWORD DataSize);
	int EndPack();

	
	int StartUnpack(LPCTSTR FileName);
	int StartUnpack(IFileAccessor	*pFileAccessor);
	int StartUnpack(void * pData,DWORD DataSize);
	int Unpack(void *data,DWORD DataSize);	
	int EndUnpack();

	DWORD GetOutBufferUsedSize()
	{
		return m_OutBufferPtr;
	}

	

protected:
	int PackAlloc();
	int UnpackAlloc();
	void BufferFree();

	void PackInit();
	void UnpackInit();	


	CODE_TYPE LookUpCT(CODE_TYPE code,BYTE ThisChar);
	void InsertCT(CODE_TYPE code,CODE_TYPE OldCode);
	int PutCode(CODE_TYPE code);
	int GetCode(CODE_TYPE& Code);
	int PutChar(BYTE ch)
	{
		if(m_OutFun(this,ch,m_OutFunParam)) 
			return 1; 
		else 
		{
			m_OutCharCount++;
			return 0;
		}
	}
	int GetChar(BYTE& ch)
	{
		return m_InFun(this,ch,m_InFunParam);
	}
	
	int PutString(CODE_TYPE code);
	
	static int OutCharToFile(LPCLZWPack pack,BYTE ch,LPVOID param);
	static int InCharFromFile(LPCLZWPack pack,BYTE& ch,LPVOID param);
	static int InCharFromBuffer(LPCLZWPack pack,BYTE& ch,LPVOID param);
	static int OutCharToBuffer(LPCLZWPack pack,BYTE ch,LPVOID param);
};


