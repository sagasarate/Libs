/****************************************************************************/
/*                                                                          */
/*      文件名:    LZWPack.cpp                                              */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
// Pack.cpp: implementation of the CLZWPack class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLZWPack::CLZWPack()
{
	m_pCTlink=NULL;
	m_pCTfirst=NULL;
	m_pCTlast=NULL;
	m_pOStack=NULL;
	m_pCTnext=NULL;
	m_pFileAccessor=NULL;	
	m_OutFun=NULL;
	m_InFun=NULL;
	m_pOutBuffer=NULL;
	m_OutBufferSize=0;;
	m_OutBufferPtr=0;
	m_pInBuffer=NULL;
	m_InBufferSize=0;;
	m_InBufferPtr=0;
	m_OStackPtr=0;
	m_NextCode=0;
	m_NextCodeLimit=0;
	m_OutCharCount=0;
	m_OldCode=-1;
	m_StartBits=0;
	m_CodeLength=0;
	m_MaxCodeLength=12;
	m_CodeBufferSize=0;
	m_EOI=0;
	m_CLEAR=0;
	m_CodeBit=0;
	m_CodeChar=0;
	m_FileFrom=PACK_COPY;
	m_PackState=PACK_READY;
	m_WorkMode=PACK_NORMAL_MODE;
	m_Index=0;
	m_InFunParam=NULL;
	m_OutFunParam=NULL;
}

CLZWPack::~CLZWPack()
{
	if(m_PackState==PACK_PACKING) EndPack();
	if(m_PackState==PACK_UNPACKING) EndUnpack();
}

int CLZWPack::PackAlloc()
{
	DWORD size;
	BufferFree();
	size=1<<m_MaxCodeLength;
	m_CodeBufferSize=size;
	m_pCTlink=(CODE_TYPE *)MONITORED_MALLOC(_T("CLZWPack"), size*sizeof(CODE_TYPE));
	m_pCTfirst=(BYTE *)MONITORED_MALLOC(_T("CLZWPack"), size);
	m_pCTnext=(CODE_TYPE *)MONITORED_MALLOC(_T("CLZWPack"), size*sizeof(CODE_TYPE));
	if(m_pCTlink==NULL||m_pCTfirst==NULL||m_pCTnext==NULL)
		return 1;
	return 0;
}

int CLZWPack::UnpackAlloc()
{
	DWORD size;
	BufferFree();
	size=1<<m_MaxCodeLength;
	m_CodeBufferSize=size;
	m_pCTlink=(CODE_TYPE *)MONITORED_MALLOC(_T("CLZWPack"), size*sizeof(CODE_TYPE));
	m_pCTfirst=(BYTE *)MONITORED_MALLOC(_T("CLZWPack"), size);
	m_pCTlast=(BYTE *)MONITORED_MALLOC(_T("CLZWPack"), size);
	m_pOStack=(BYTE *)MONITORED_MALLOC(_T("CLZWPack"), size);
	if(m_pCTlink==NULL||m_pCTfirst==NULL||m_pCTlast==NULL||m_pOStack==NULL)
		return 1;
	ZeroMemory(m_pCTlink,size*sizeof(CODE_TYPE));
	ZeroMemory(m_pCTfirst,size);
	ZeroMemory(m_pCTlast,size);
	ZeroMemory(m_pOStack,size);
	return 0;
}
void CLZWPack::BufferFree()
{
	if(m_pCTlink!=NULL) MONITORED_FREE(m_pCTlink);
	if(m_pCTfirst!=NULL) MONITORED_FREE(m_pCTfirst);
	if(m_pCTlast!=NULL) MONITORED_FREE(m_pCTlast);
	if(m_pOStack!=NULL) MONITORED_FREE(m_pOStack);
	if(m_pCTnext!=NULL) MONITORED_FREE(m_pCTnext);
	m_pCTlink=NULL;
	m_pCTfirst=NULL;
	m_pCTlast=NULL;
	m_pOStack=NULL;
	m_pCTnext=NULL;
}

void CLZWPack::PackInit()
{
	DWORD i,size;
	size=1<<m_MaxCodeLength;
	m_NextCode=m_CLEAR+2;
	m_CodeLength=m_StartBits+1;
	m_NextCodeLimit=1<<m_CodeLength;
	m_OldCode=-1;
	for(i=0;i<m_CLEAR;i++)
		m_pCTfirst[i]=(BYTE)i;
	for(i=0;i<size;i++)
	{
		m_pCTlink[i]=-2;
		m_pCTnext[i]=-1;
	}
}

void CLZWPack::UnpackInit()
{
	DWORD i,size;
	size=1<<m_MaxCodeLength;
	
	m_NextCode=m_CLEAR+2;
	m_CodeLength=m_StartBits+1;
	m_NextCodeLimit=1<<m_CodeLength;
	m_OldCode=-1;
	for(i=0;i<m_CLEAR;i++)
	{
		m_pCTfirst[i]=(BYTE)i;
		m_pCTlast[i]=(BYTE)i;
		m_pCTlink[i]=-1;
	}
	for(i=m_CLEAR;i<size;i++)
		m_pCTlink[i]=-2;
}

void CLZWPack::SetMaxCodeLength(WORD length)
{
	if(m_pCTlink==NULL)
		m_MaxCodeLength=length;
}

int CLZWPack::StartPack(LPCTSTR FileName,WORD StartBits,WORD MaxBits)
{
	IFileAccessor * pFileAccessor = MONITORED_NEW(_T("CLZWPack"), CStandardFileAccessor);
	
	if(!pFileAccessor->Open(FileName,IFileAccessor::modeWrite|IFileAccessor::shareShareRead|IFileAccessor::modeCreateAlways))
	{
		SAFE_RELEASE(pFileAccessor);
		return 1;
	}
	m_FileFrom=PACK_OPEN;
	return StartPack(pFileAccessor,StartBits,MaxBits);
}

int CLZWPack::StartPack(IFileAccessor * pFileAccessor,WORD StartBits,WORD MaxBits)
{
	
	if(m_PackState!=PACK_READY) return -2;
	m_pFileAccessor=pFileAccessor;
	m_Index=0;
	m_CodeBit=0;
	m_CodeChar=0;
	m_StartBits=StartBits;
	m_MaxCodeLength=MaxBits;
	m_CLEAR=1<<m_StartBits;
	m_EOI=m_CLEAR+1;
	PackAlloc();
	PackInit();
	if(m_OutFun==NULL) m_OutFun=OutCharToFile;
	if(m_InFun==NULL) m_InFun=InCharFromBuffer;
	PutChar((BYTE)m_StartBits);
	if(m_WorkMode!=PACK_GIF_MODE)
		PutChar((BYTE)m_MaxCodeLength);
	PutCode(m_CLEAR);
	m_PackState=PACK_PACKING;
	return 0;
}

int CLZWPack::StartPack(void * pData,DWORD DataSize,WORD StartBits,WORD MaxBits)
{
	if(m_PackState!=PACK_READY) return -2;
	m_pFileAccessor=NULL;
	m_Index=0;
	m_CodeBit=0;
	m_CodeChar=0;
	m_StartBits=StartBits;
	m_MaxCodeLength=MaxBits;
	m_CLEAR=1<<m_StartBits;
	m_EOI=m_CLEAR+1;
	PackAlloc();
	PackInit();
	m_pOutBuffer=(BYTE *)pData;
	m_OutBufferSize=DataSize;
	m_OutBufferPtr=0;
	if(m_OutFun==NULL) m_OutFun=OutCharToBuffer;
	if(m_InFun==NULL) m_InFun=InCharFromBuffer;
	PutChar((BYTE)m_StartBits);
	if(m_WorkMode!=PACK_GIF_MODE)
		PutChar((BYTE)m_MaxCodeLength);
	PutCode(m_CLEAR);
	m_PackState=PACK_PACKING;
	return 0;
}

int CLZWPack::StartUnpack(LPCTSTR FileName)
{
	IFileAccessor * pFileAccessor = MONITORED_NEW(_T("CLZWPack"), CStandardFileAccessor);

	if(!pFileAccessor->Open(FileName,IFileAccessor::modeRead|IFileAccessor::shareShareRead|IFileAccessor::modeOpen))
	{
		SAFE_RELEASE(pFileAccessor);
		return 1;
	}	
	m_FileFrom=PACK_OPEN;
	return StartUnpack(pFileAccessor);
}

int CLZWPack::StartUnpack(IFileAccessor * pFileAccessor)
{
	BYTE fc;
	
	
	if(m_PackState!=PACK_READY) return -2;
	m_Index=0;
	m_pFileAccessor=pFileAccessor;
	if(m_OutFun==NULL) m_OutFun=OutCharToBuffer;
	if(m_InFun==NULL) m_InFun=InCharFromFile;
	m_CodeBit=8;
	m_CodeChar=0;
	if(GetChar(fc)) return 1;
	m_StartBits=fc;
	if(m_WorkMode!=PACK_GIF_MODE)
	{
		if(GetChar(fc)) return 1;
		m_MaxCodeLength=fc;
	}
	else m_MaxCodeLength=12;
	m_CLEAR=1<<m_StartBits;
	m_EOI=m_CLEAR+1;
	UnpackAlloc();
	UnpackInit();
	m_PackState=PACK_UNPACKING;
	return 0;
}

int CLZWPack::StartUnpack(void * pData,DWORD DataSize)
{
	BYTE fc;


	if(m_PackState!=PACK_READY) return -2;
	m_Index=0;
	m_pFileAccessor=NULL;
	m_pInBuffer=(BYTE *)pData;
	m_InBufferSize=DataSize;
	m_InBufferPtr=0;
	if(m_OutFun==NULL) m_OutFun=OutCharToBuffer;
	if(m_InFun==NULL) m_InFun=InCharFromBuffer;
	m_CodeBit=8;
	m_CodeChar=0;
	if(GetChar(fc)) return 1;
	m_StartBits=fc;
	if(m_WorkMode!=PACK_GIF_MODE)
	{
		if(GetChar(fc)) return 1;
		m_MaxCodeLength=fc;
	}
	else m_MaxCodeLength=12;
	m_CLEAR=1<<m_StartBits;
	m_EOI=m_CLEAR+1;
	UnpackAlloc();
	UnpackInit();
	m_PackState=PACK_UNPACKING;
	return 0;
}

int CLZWPack::Pack(void *data,DWORD DataSize)
{
	BYTE	ThisChar;
	CODE_TYPE		code;

	m_pInBuffer=(BYTE *)data;
	m_InBufferSize=DataSize;
	m_InBufferPtr=0;
	while(GetChar(ThisChar)==PACK_SUCCESS)
	{
		code=LookUpCT(m_OldCode,ThisChar);
		if(code!=-1) m_OldCode=code;
		else
		{
			PutCode(m_OldCode);
			m_OldCode=(CODE_TYPE)ThisChar;
		}
		if(m_NextCode>m_NextCodeLimit)
		{
			if(m_CodeLength>=m_MaxCodeLength)
			{
				PutCode(m_OldCode);
				PutCode(m_CLEAR);
				PackInit();
			}
			else
			{
				m_CodeLength++;
				m_NextCodeLimit<<=1;
				if(m_CodeLength>=m_MaxCodeLength)
					m_NextCodeLimit--;
			}
		}
	}
	return 0;
}

int CLZWPack::Unpack(void *data,DWORD DataSize)
{
	CODE_TYPE		code;

	m_pOutBuffer=(BYTE *)data;
	m_OutBufferSize=DataSize;
	m_OutBufferPtr=0;
	m_OutCharCount=0;
	if(PutString(-1)) return m_OutCharCount;
	while(!GetCode(code))
	{
		if(code==m_EOI) break;
		if(code==m_CLEAR) UnpackInit();
		else
		{
			if(m_pCTlink[code]==-2)
				InsertCT(m_OldCode,m_OldCode);
			else
				if(m_OldCode!=-1) InsertCT(code,m_OldCode);
			m_OldCode=code;
			if(PutString(code)) break;
		}
	}
	return m_OutCharCount;
}
	
		
int CLZWPack::EndPack()
{
	PutCode(m_OldCode);
	PutCode(m_EOI);
	if(m_WorkMode==PACK_GIF_MODE)
	{
		if(m_CodeBit>0)
		{
			m_buffer256[m_Index]=m_CodeChar;
			m_Index++;
		}
		if(m_Index>0)
		{
			PutChar((BYTE)m_Index);
			for(int i=0;i<m_Index;i++)
				if(PutChar(m_buffer256[i])==PACK_FAILED) return 1;
		}
		PutChar(0);
	}
	BufferFree();
	if(m_FileFrom==PACK_OPEN) 
		SAFE_RELEASE(m_pFileAccessor);
	m_pFileAccessor=NULL;
	m_PackState=PACK_READY;
	return 0;
}

int CLZWPack::EndUnpack()
{
	if(m_FileFrom==PACK_OPEN) 
		SAFE_RELEASE(m_pFileAccessor);
	m_pFileAccessor=NULL;
	BufferFree();
	m_PackState=PACK_READY;
	return 0;
}

CODE_TYPE CLZWPack::LookUpCT(CODE_TYPE code,BYTE ThisChar)
{
	if(code==-1) return (CODE_TYPE)ThisChar;
	if(m_pCTlink[code]==-2)
		m_pCTlink[code]=m_NextCode;
	else
	{
		code=m_pCTlink[code];
		while((m_pCTnext[code]!=-1)&&(m_pCTfirst[code]!=ThisChar))
			code=m_pCTnext[code];
		if(m_pCTfirst[code]==ThisChar) return code;
		m_pCTnext[code]=m_NextCode;
	}
	m_pCTfirst[m_NextCode++]=ThisChar;
	return -1;
}

void CLZWPack::InsertCT(CODE_TYPE code,CODE_TYPE OldCode)
{
	m_pCTlink[m_NextCode]=m_OldCode;
	m_pCTlast[m_NextCode]=m_pCTfirst[code];
	m_pCTfirst[m_NextCode]=m_pCTfirst[OldCode];
	if(++m_NextCode>=m_NextCodeLimit)
	{
		if(m_CodeLength<m_MaxCodeLength)
		{
			m_NextCodeLimit<<=1;
			m_CodeLength++;
		}
	}
}

int CLZWPack::PutCode(CODE_TYPE code)
{
	BYTE	bits,mask;
	int		BitsCount,BitsLeft,BitsLength;
	
	BitsCount=0;
	while(BitsCount<m_CodeLength)
	{
		if(m_CodeBit>=8)
		{
			if(m_WorkMode==PACK_GIF_MODE)
			{
				m_buffer256[m_Index]=m_CodeChar;
				m_Index++;
				if(m_Index>=255)
				{
					PutChar((BYTE)m_Index);
					for(int i=0;i<m_Index;i++)
						if(PutChar(m_buffer256[i])==PACK_FAILED) return 1;
					m_Index=0;
				}
			}
			else if(PutChar(m_CodeChar)==PACK_FAILED) return 1;
			m_CodeChar=0;
			m_CodeBit=0;
		}
		BitsLeft=8-m_CodeBit;
		if(BitsLeft>m_CodeLength-BitsCount)
			BitsLength=m_CodeLength-BitsCount;
		else BitsLength=BitsLeft;
		mask=0xff>>(8-BitsLength);
		bits=(BYTE)(code>>BitsCount);
		bits&=mask;
		bits=bits<<m_CodeBit;
		m_CodeChar=m_CodeChar|bits;
		m_CodeBit+=BitsLength;
		BitsCount+=BitsLength;
	}
	return 0;
}

int CLZWPack::GetCode(CODE_TYPE& Code)
{
	DWORD	bits,mask;
	int		BitsCount,BitsLeft,BitsLength;

	Code=0;
	BitsCount=0;
	BitsLeft=0;
	BitsLength=0;
	while(BitsCount<m_CodeLength)
	{
		if(m_CodeBit>=8)
		{
			if(m_WorkMode==PACK_GIF_MODE)
			{
				if(m_Index==0) 
				{
					GetChar(m_CodeChar);
					m_Index=m_CodeChar;
				}
				m_Index--;
			}
			if(GetChar(m_CodeChar)==PACK_FAILED) return 1;
			m_CodeBit=0;
			
		}
		BitsLeft=8-m_CodeBit;
		if(BitsLeft>m_CodeLength-BitsCount)
			BitsLength=m_CodeLength-BitsCount;
		else BitsLength=BitsLeft;

		mask=0xffffffff>>(32-BitsLength);
		bits=(DWORD)(m_CodeChar>>m_CodeBit);
		bits&=mask;
		bits=bits<<BitsCount;
		Code=Code|bits;
		m_CodeBit+=BitsLength;
		BitsCount+=BitsLength;
	}	
	return 0;
}

int CLZWPack::PutString(CODE_TYPE code)
{
	
	if(code>=0) m_OStackPtr=0;
	while(code>=0)
	{
		if((DWORD)m_OStackPtr>=m_CodeBufferSize) return 0;
		m_pOStack[m_OStackPtr++]=m_pCTlast[code];
		code=m_pCTlink[code];
	}
	for(m_OStackPtr--;m_OStackPtr>=0;m_OStackPtr--)
	{
		if(PutChar(m_pOStack[m_OStackPtr])==PACK_FAILED) 
		{
			//m_OStackPtr++;
			break;
		}
	}
	return ++m_OStackPtr;	
}

int CLZWPack::OutCharToFile(LPCLZWPack pack,BYTE ch,LPVOID param)
{
	UINT64 count=pack->m_pFileAccessor->Write(&ch,1);
	if(count) return PACK_SUCCESS;
	else return PACK_FAILED;
}

int CLZWPack::InCharFromFile(LPCLZWPack pack,BYTE& ch,LPVOID param)
{

	UINT64 count=pack->m_pFileAccessor->Read(&ch,1);
	if(count) return PACK_SUCCESS;
	else return PACK_FAILED;
	
}

int CLZWPack::InCharFromBuffer(LPCLZWPack pack,BYTE& ch,LPVOID param)
{
	if(pack->m_InBufferPtr>=pack->m_InBufferSize) return PACK_FAILED;
	ch=pack->m_pInBuffer[pack->m_InBufferPtr];
	(pack->m_InBufferPtr)++;
	return PACK_SUCCESS;
}

int CLZWPack::OutCharToBuffer(LPCLZWPack pack,BYTE ch,LPVOID param)
{
	if(pack->m_OutBufferPtr>=pack->m_OutBufferSize) 
		return PACK_FAILED;
	pack->m_pOutBuffer[pack->m_OutBufferPtr]=ch;
	(pack->m_OutBufferPtr)++;
	return PACK_SUCCESS;
}

OUTFUN CLZWPack::SetOutFunction(OUTFUN fun,LPVOID param)
{
	OUTFUN temp;
	temp=m_OutFun;
	m_OutFun=fun;
	m_OutFunParam=param;
	return temp;
}

INFUN CLZWPack::SetInFunction(INFUN fun,LPVOID param)
{
	INFUN temp;
	temp=m_InFun;
	m_InFun=fun;
	m_InFunParam=param;
	return temp;
}	