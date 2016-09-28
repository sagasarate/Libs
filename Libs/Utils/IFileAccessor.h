/****************************************************************************/
/*                                                                          */
/*      文件名:    IFileAccessor.h                                          */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class IFileAccessor:public CNameObject
{
protected:
	DECLARE_CLASS_INFO_STATIC(IFileAccessor)
public:	

	enum OPEN_MODE
	{
		modeRead =			(int) 0x00000,
		modeWrite =			(int) 0x00001,
		modeReadWrite =		(int) 0x00002,
		modeAccessMask =	(int) 0x00003,

		shareShareRead =	(int) 0x00004,
		shareShareWrite =	(int) 0x00008,		
		shareShareAll =		(int) 0x0000C,
		
		modeOpen =			(int) 0x00000,
		modeOpenAlways =	(int) 0x00010,
		modeCreate =		(int) 0x00020,
		modeCreateAlways =	(int) 0x00030,		
		modeTruncate =		(int) 0x00040,
		modeAppend =		(int) 0x00050,
		modeCreationMask =	(int) 0x000f0,

		AttrNormal =		(int) 0x00000,
		AttrHidden =		(int) 0x00100,
		AttrReadonly =		(int) 0x00200,
		AttrSystem =		(int) 0x00400,
		AttrArchive =		(int) 0x00800,
		

		typeText =			(int) 0x04000, // typeText and typeBinary are
		typeBinary =		(int) 0x08000, // used in derived classes only
		osNoBuffer =		(int) 0x10000,
		osWriteThrough =	(int) 0x20000,
		osRandomAccess =	(int) 0x40000,
		osSequentialScan =	(int) 0x80000,
		KeepOnExec =		(int) 0xf0000,
	};
	enum SEEK_MODE
	{
		seekBegin = 0x0, 
		seekCurrent = 0x1, 
		seekEnd = 0x2
	};

	IFileAccessor();

	virtual ~IFileAccessor(void);

	virtual BOOL Open(LPCTSTR FileName,int OpenMode)=0;
	virtual void Close()=0;

	virtual ULONG64 GetSize()=0;

	virtual ULONG64 Read(LPVOID pBuff,ULONG64 Size)=0;
	virtual ULONG64 Write(LPCVOID pBuff,ULONG64 Size)=0;


	virtual BOOL IsEOF()=0;
	
	virtual BOOL Seek(LONG64 Offset,int SeekMode)=0;
	virtual ULONG64 GetCurPos()=0;

	virtual BOOL SetCreateTime(const CEasyTime& Time)=0;
	virtual BOOL GetCreateTime(CEasyTime& Time)=0;

	virtual BOOL SetLastAccessTime(const CEasyTime& Time)=0;
	virtual BOOL GetLastAccessTime(CEasyTime& Time)=0;

	virtual BOOL SetLastWriteTime(const CEasyTime& Time)=0;
	virtual BOOL GetLastWriteTime(CEasyTime& Time)=0;

};
