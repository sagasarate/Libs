/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSMessage.h                                             */
/*      创建日期:  2009年10月23日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

typedef WORD	ROUTE_ID_TYPE;

struct OBJECT_ID_BASE
{
	union
	{
		UINT64	ID;
		struct
		{
			WORD ObjectIndex;
			WORD GroupIndex;
			WORD ObjectTypeID;
			WORD RouterID;
		};
	};
};

struct OBJECT_ID:public OBJECT_ID_BASE
{
	OBJECT_ID()
	{
		ID=0;
	}	
	OBJECT_ID(const OBJECT_ID_BASE& ObjectID)
	{
		ID=ObjectID.ID;
	}
	OBJECT_ID(unsigned __int64 Value)
	{
		ID=Value;
	}
	OBJECT_ID& operator=(const OBJECT_ID_BASE& ObjectID)
	{
		ID=ObjectID.ID;
		return *this;
	}
	bool operator==(const OBJECT_ID& ObjectID) const
	{
		return ID==ObjectID.ID;
	}
	bool operator!=(const OBJECT_ID& ObjectID) const
	{
		return ID!=ObjectID.ID;
	}
	bool operator>(const OBJECT_ID& ObjectID) const
	{
		return ID>ObjectID.ID;
	}
	bool operator<(const OBJECT_ID& ObjectID) const
	{
		return ID<ObjectID.ID;
	}
	static int Compare(const void * Value1,const void * Value2);
};


inline int OBJECT_ID::Compare(const void * Value1,const void * Value2)
{
	if(((OBJECT_ID *)Value1)->ID==((OBJECT_ID *)Value2)->ID)
		return 0;
	else if(((OBJECT_ID *)Value1)->ID<((OBJECT_ID *)Value2)->ID)
		return -1;
	else
		return 1;
}

#define BROAD_CAST_ROUTER_ID		(0xFFFF)
#define BROAD_CAST_OBJECT_TYPE_ID	(0xFFFF)
#define BROAD_CAST_GROUP_INDEX		(0xFFFF)
#define BROAD_CAST_OBJECT_INDEX		(0xFFFF)
#define BROAD_CAST_PROXY_TYPE		(0xFF)
#define MAKE_PROXY_GROUP_INDEX(ProxyType) ((ProxyType&0xFF)|0xFF00)
#define GET_PROXY_TYPE_FROM_PROXY_GROUP_INDEX(GroupIndex) (GroupIndex&0xFF)

enum DOS_OBJECT_TYPE
{
	DOT_UNKNOW,
	DOT_PROXY_OBJECT,
	//DOT_MAIN_CONTROL_OBJECT,
	//DOT_MAIN_NORMAL_OBJECT,
	DOT_NORMAL_OBJECT,
	DOT_MAX,
};

//#define MAX_MESSAGE_PACKET_SIZE		4096

typedef unsigned int	MSG_ID_TYPE;
typedef unsigned int	MSG_LEN_TYPE;
typedef unsigned short	ID_LIST_COUNT_TYPE;


#pragma pack(push)

#pragma pack(1)

class CDOSSimpleMessage
{
public:
	struct DOS_SIMPLE_MESSAGE_HEAD
	{
		MSG_LEN_TYPE	MsgLen;
		MSG_ID_TYPE		MsgID;	
		WORD			MsgFlag;
	};	
protected:
	DOS_SIMPLE_MESSAGE_HEAD	m_MsgHeader;
	char					m_DataBuffer[4];
public:
	void Init()
	{
		m_MsgHeader.MsgLen=sizeof(DOS_SIMPLE_MESSAGE_HEAD);
		m_MsgHeader.MsgID=0;		
		m_MsgHeader.MsgFlag=0;
	}
	DOS_SIMPLE_MESSAGE_HEAD& GetMsgHeader() 
	{
		return m_MsgHeader;
	}
	const DOS_SIMPLE_MESSAGE_HEAD& GetMsgHeader() const
	{
		return m_MsgHeader;
	}
	void SetMsgID(MSG_ID_TYPE CmdID)
	{
		m_MsgHeader.MsgID=CmdID;
	}
	MSG_ID_TYPE GetMsgID() const
	{
		return m_MsgHeader.MsgID;
	}
	void SetDataLength(MSG_LEN_TYPE Length)
	{
		m_MsgHeader.MsgLen=sizeof(DOS_SIMPLE_MESSAGE_HEAD)+Length;
	}
	MSG_LEN_TYPE GetDataLength() const
	{
		return m_MsgHeader.MsgLen-sizeof(DOS_SIMPLE_MESSAGE_HEAD);
	}
	MSG_LEN_TYPE GetMsgLength() const
	{
		return m_MsgHeader.MsgLen;
	}
	void SetMsgFlag(WORD Flag)
	{
		m_MsgHeader.MsgFlag=Flag;
	}
	WORD GetMsgFlag() const
	{
		return m_MsgHeader.MsgFlag;
	}
	void * GetDataBuffer()
	{
		return m_DataBuffer;
	}
	const void * GetDataBuffer() const
	{
		return m_DataBuffer;
	}
	CSmartStruct GetDataPacket()
	{
		CSmartStruct DataPacket(m_DataBuffer,GetDataLength(),false);

		return DataPacket;
	}	
	void ClearDataPacket()
	{
		CSmartStruct DataPacket(m_DataBuffer,GetDataLength(),true);
	}
	
	static MSG_LEN_TYPE GetMsgHeaderLength()
	{
		return sizeof(DOS_SIMPLE_MESSAGE_HEAD);
	}
	static MSG_LEN_TYPE CaculateMsgLength(MSG_LEN_TYPE DataLength)
	{
		return sizeof(DOS_SIMPLE_MESSAGE_HEAD)+DataLength;
	}
};

class CDOSMessage
{
public:	
	struct DOS_MESSAGE_HEAD
	{
		union
		{
			struct
			{
				MSG_LEN_TYPE	MsgLen;		
				MSG_ID_TYPE		MsgID;
				WORD			MsgFlag;
				OBJECT_ID_BASE	SenderID;
			};
			struct  
			{
				OBJECT_ID_BASE									Reserve;
				CDOSSimpleMessage::DOS_SIMPLE_MESSAGE_HEAD		SimpleMsgHeader;
			};
		};
	};
protected:	
	DOS_MESSAGE_HEAD					m_MsgHeader;
	char								m_DataBuffer[4];
public:
	void Init()
	{
		m_MsgHeader.MsgLen=sizeof(DOS_MESSAGE_HEAD);
		m_MsgHeader.MsgID=0;
		m_MsgHeader.MsgFlag=0;
		m_MsgHeader.SenderID.ID=0;
	}
	DOS_MESSAGE_HEAD& GetMsgHeader()
	{
		return m_MsgHeader;
	}
	const DOS_MESSAGE_HEAD& GetMsgHeader() const
	{
		return m_MsgHeader;
	}
	void SetMsgID(MSG_ID_TYPE CmdID)
	{
		m_MsgHeader.MsgID=CmdID;
	}
	MSG_ID_TYPE GetMsgID() const
	{
		return m_MsgHeader.MsgID;
	}
	void SetDataLength(MSG_LEN_TYPE Length)
	{
		m_MsgHeader.MsgLen=sizeof(DOS_MESSAGE_HEAD)+Length;
	}
	MSG_LEN_TYPE GetDataLength() const
	{
		return m_MsgHeader.MsgLen-sizeof(DOS_MESSAGE_HEAD);
	}
	void SetMsgFlag(WORD Flag)
	{
		m_MsgHeader.MsgFlag=Flag;
	}
	WORD GetMsgFlag() const
	{
		return m_MsgHeader.MsgFlag;
	}
	void SetData(LPVOID pData,MSG_LEN_TYPE Length)
	{
		SetDataLength(Length);
		memcpy(m_DataBuffer,pData,Length);
	}
	MSG_LEN_TYPE GetMsgLength() const
	{
		return m_MsgHeader.MsgLen;
	}
	void SetSenderID(OBJECT_ID SenderID)
	{
		m_MsgHeader.SenderID=SenderID;
	}
	OBJECT_ID GetSenderID() const
	{
		return m_MsgHeader.SenderID;
	}
	void * GetDataBuffer()
	{
		return m_DataBuffer;
	}
	const void * GetDataBuffer() const
	{
		return m_DataBuffer;
	}
	CSmartStruct GetDataPacket()
	{
		CSmartStruct DataPacket(m_DataBuffer,GetDataLength(),false);

		return DataPacket;
	}
	CSmartStruct GetEmptyDataPacket()
	{
		CSmartStruct DataPacket(m_DataBuffer,GetDataLength(),true);

		return DataPacket;
	}
	void ClearDataPacket()
	{
		CSmartStruct DataPacket(m_DataBuffer,GetDataLength(),true);
	}
	CDOSSimpleMessage * MakeSimpleMessage()
	{
		m_MsgHeader.SimpleMsgHeader.MsgFlag=m_MsgHeader.MsgFlag;
		m_MsgHeader.SimpleMsgHeader.MsgID=m_MsgHeader.MsgID;
		m_MsgHeader.SimpleMsgHeader.MsgLen=CDOSSimpleMessage::GetMsgHeaderLength()+GetDataLength();		
		return (CDOSSimpleMessage *)(&m_MsgHeader.SimpleMsgHeader);
	}
	void MakeSimpleMessageHeader(CDOSSimpleMessage::DOS_SIMPLE_MESSAGE_HEAD& MsgHeader) const
	{
		MsgHeader.MsgFlag = m_MsgHeader.MsgFlag;
		MsgHeader.MsgID = m_MsgHeader.MsgID;
		MsgHeader.MsgLen = CDOSSimpleMessage::GetMsgHeaderLength() + GetDataLength();
	}
	static MSG_LEN_TYPE GetMsgHeaderLength()
	{
		return sizeof(DOS_MESSAGE_HEAD);
	}
	static MSG_LEN_TYPE CaculateMsgLength(MSG_LEN_TYPE DataLength)
	{
		return sizeof(DOS_MESSAGE_HEAD)+DataLength;
	}
};

class CDOSMessagePacket
{
protected:
	UINT			m_AllocTime;
	UINT			m_AllocSize;
	volatile UINT	m_RefCount;
	MSG_LEN_TYPE	m_PacketLen;
	CDOSMessage		m_Message;
public:	
	void Init()
	{
		m_Message.Init();
		m_AllocTime=0;
		m_RefCount=0;
		m_PacketLen=sizeof(MSG_LEN_TYPE)+CDOSMessage::GetMsgHeaderLength()+sizeof(ID_LIST_COUNT_TYPE);
	}
	CDOSMessage& GetMessage()
	{
		return m_Message;
	}	
	const CDOSMessage& GetMessage() const
	{
		return m_Message;
	}
	void SetAllocTime(UINT Time)
	{
		m_AllocTime=Time;
	}	
	UINT GetAllocTime() const
	{
		return m_AllocTime;
	}
	void SetAllocSize(UINT Size)
	{
		m_AllocSize=Size;
	}
	UINT GetAllocSize() const
	{
		return m_AllocSize;
	}
	UINT IncRefCount()
	{
		return AtomicInc(&m_RefCount);
	}
	UINT DecRefCount()
	{
		return AtomicDec(&m_RefCount);
	}
	UINT GetRefCount() const
	{
		return m_RefCount;
	}
	void SetPacketLength(MSG_LEN_TYPE PacketLength)
	{
		m_PacketLen=PacketLength;
	}
	MSG_LEN_TYPE GetPacketLength() const
	{
		return m_PacketLen;
	}
	void * GetPacketBuffer()
	{
		return &m_PacketLen;
	}	
	const void * GetPacketBuffer() const
	{
		return &m_PacketLen;
	}
	void SetTargetIDs(ID_LIST_COUNT_TYPE TargetCount,OBJECT_ID * pTargetIDs)
	{
		GetTargetIDCount()=TargetCount;
		if(pTargetIDs)
			memcpy(GetTargetIDs(),pTargetIDs,sizeof(OBJECT_ID)*TargetCount);
	}
	ID_LIST_COUNT_TYPE& GetTargetIDCount()
	{
		return *((ID_LIST_COUNT_TYPE *)((BYTE *)GetPacketBuffer()+sizeof(MSG_LEN_TYPE)+m_Message.GetMsgLength()));
	}
	OBJECT_ID * GetTargetIDs()
	{
		return (OBJECT_ID *)((BYTE *)GetPacketBuffer()+sizeof(MSG_LEN_TYPE)+m_Message.GetMsgLength()+sizeof(ID_LIST_COUNT_TYPE));
	}
	void AddTargetID(OBJECT_ID TargetID)
	{
		GetTargetIDs()[GetTargetIDCount()]=TargetID;
		GetTargetIDCount()++;
	}
	void MakePacketLength()
	{
		m_PacketLen=sizeof(MSG_LEN_TYPE)+GetMessage().GetMsgLength()+sizeof(ID_LIST_COUNT_TYPE)+sizeof(OBJECT_ID)*GetTargetIDCount();
	}
	bool CheckPacket()
	{
		return m_PacketLen<=m_AllocSize;
	}
	static MSG_LEN_TYPE CaculatePacketLength(MSG_LEN_TYPE DataLength,ID_LIST_COUNT_TYPE TargetIDCount)
	{
		return sizeof(MSG_LEN_TYPE)+CDOSMessage::CaculateMsgLength(DataLength)+sizeof(ID_LIST_COUNT_TYPE)+sizeof(OBJECT_ID)*TargetIDCount;
	}
	static MSG_LEN_TYPE CaculateRealPacketLength(MSG_LEN_TYPE PacketLength)
	{
		return sizeof(UINT)+sizeof(UINT)+sizeof(UINT)+PacketLength;
	}
};

struct PING_DATA
{
	UINT	Time;
	WORD	RecentDelay;
};

#pragma pack(pop)

enum DOS_SYSTEM_MESSAGE
{
	DSM_NONE=0,
	
	DSM_PROXY_REGISTER_MSG_MAP,
	DSM_PROXY_REGISTER_MSG_MAP_RESULT,
	DSM_PROXY_UNREGISTER_MSG_MAP,
	DSM_PROXY_UNREGISTER_MSG_MAP_RESULT,
	DSM_PROXY_REGISTER_GLOBAL_MSG_MAP,
	DSM_PROXY_REGISTER_GLOBAL_MSG_MAP_RESULT,
	DSM_PROXY_UNREGISTER_GLOBAL_MSG_MAP,
	DSM_PROXY_UNREGISTER_GLOBAL_MSG_MAP_RESULT,
	DSM_PROXY_DISCONNECT,
	DSM_PROXY_KEEP_ALIVE_PING,
	DSM_PROXY_KEEP_ALIVE_PONG,
	DSM_PROXY_GET_IP,
	DSM_PROXY_IP_REPORT,
	DSM_PROXY_SET_UNHANDLE_MSG_RECEIVER,
	DSM_ROUTE_LINK_LOST=100,
	DSM_OBJECT_ALIVE_TEST=200,	
	DSM_OBJECT_FIND,
	DSM_OBJECT_REPORT,
	DSM_SYSTEM_SHUTDOWN,	
	DSM_MAX,
};

enum DOS_MESSAGE_FLAG
{
	DOS_MESSAGE_FLAG_SYSTEM_MESSAGE=1,
	DOS_MESSAGE_FLAG_COMPRESSED=(1<<1),
	DOS_MESSAGE_FLAG_CAN_CACHE = (1 << 2),
};


inline BOOL PrintDOSLogWithTag(LPCTSTR Tag, LPCTSTR Format, ...)
{
	va_list vl;
	va_start(vl, Format);
	BOOL ret = CLogManager::GetInstance()->PrintLogVL(LOG_DOS_CHANNEL, ILogPrinter::LOG_LEVEL_NORMAL, Tag, Format, vl);
	va_end(vl);
	return ret;
}


inline BOOL PrintDOSDebugLogWithTag(LPCTSTR Tag, LPCTSTR Format, ...)
{
	va_list vl;
	va_start(vl, Format);
	BOOL ret = CLogManager::GetInstance()->PrintLogVL(LOG_DOS_CHANNEL, ILogPrinter::LOG_LEVEL_DEBUG, Tag, Format, vl);
	va_end(vl);
	return ret;
}

inline BOOL PrintDOSObjectStatLog(LPCTSTR Format, ...)
{
	va_list vl;
	va_start(vl, Format);
	BOOL ret = CLogManager::GetInstance()->PrintLogVL(LOG_DOS_OBJECT_STATE_CHANNEL, ILogPrinter::LOG_LEVEL_NORMAL, NULL, Format, vl);
	va_end(vl);
	return ret;
}

#define PrintDOSLog(_Format, ...)	PrintDOSLogWithTag(__PRETTY_FUNCTION__, _Format, ##__VA_ARGS__)
#define PrintDOSDebugLog(_Format, ...)	PrintDOSDebugLogWithTag(__PRETTY_FUNCTION__, _Format, ##__VA_ARGS__)