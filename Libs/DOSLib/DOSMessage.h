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
	UINT GetHashCode() const
	{
		return CHashCodeMaker::GetHashCode(ID);
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
#define MAKE_PROXY_GROUP_INDEX(ProxyType) (BYTE)((ProxyType&0xFF)|0xFF00)
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
		WORD			CRC;
	};	
protected:
	DOS_SIMPLE_MESSAGE_HEAD	m_MsgHeader;
	char					m_DataBuffer[4];
public:
	void Init()
	{
		m_MsgHeader.MsgLen = sizeof(DOS_SIMPLE_MESSAGE_HEAD);
		m_MsgHeader.MsgID = 0;
		m_MsgHeader.MsgFlag = 0;
		m_MsgHeader.CRC = 0;
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
	void SetCRC(WORD CRC)
	{
		m_MsgHeader.CRC = CRC;
	}
	WORD GetCRC() const 
	{
		return m_MsgHeader.CRC;
	}
	//void * GetMsgData()
	//{
	//	return m_DataBuffer;
	//}
	const void * GetMsgData() const
	{
		return m_DataBuffer;
	}
	bool PutMsgData(MSG_LEN_TYPE Pos, LPCVOID pData, MSG_LEN_TYPE DataLen)
	{
		if (Pos + DataLen <= GetDataLength())
		{
			memcpy(m_DataBuffer + Pos, pData, DataLen);
			return true;
		}
		else
		{
			assert(false);
			return false;
		}
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
				BYTE											Reserve[6];
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
	void SetData(LPCVOID pData,MSG_LEN_TYPE Length)
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
	//void * GetMsgData()
	//{
	//	return m_DataBuffer;
	//}
	const void * GetMsgData() const
	{
		return m_DataBuffer;
	}
	bool PutMsgData(MSG_LEN_TYPE Pos, LPCVOID pData, MSG_LEN_TYPE DataLen)
	{
		if (Pos + DataLen <= GetDataLength())
		{
			memcpy(m_DataBuffer + Pos, pData, DataLen);
			return true;
		}
		else
		{
			assert(false);
			return false;
		}
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
		WORD MsgFlag = m_MsgHeader.MsgFlag;
		MSG_ID_TYPE MsgID = m_MsgHeader.MsgID;
		MSG_LEN_TYPE MsgLen = CDOSSimpleMessage::GetMsgHeaderLength() + GetDataLength();
		m_MsgHeader.SimpleMsgHeader.MsgFlag=MsgFlag;
		m_MsgHeader.SimpleMsgHeader.MsgID=MsgID;
		m_MsgHeader.SimpleMsgHeader.MsgLen = MsgLen;
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
	//void * GetPacketData()
	//{
	//	return &m_PacketLen;
	//}
	const void * GetPacketData() const
	{
		return &m_PacketLen;
	}
	bool CloneFrom(LPCVOID pPacket, MSG_LEN_TYPE PacketLen)
	{
		MSG_LEN_TYPE SrcPacketLen = *((MSG_LEN_TYPE *)pPacket);
		if (SrcPacketLen == PacketLen)
		{
			if (PacketLen <= GetAllocSize())
			{
				memcpy((void *)GetPacketData(), pPacket, PacketLen);
				return true;
			}
			else
			{
				assert(false);
				return false;
			}
		}
		else
		{
			assert(false);
			return false;
		}
	}
	bool SetTargetIDs(ID_LIST_COUNT_TYPE TargetCount,OBJECT_ID * pTargetIDs)
	{
		MSG_LEN_TYPE PacketLen = sizeof(MSG_LEN_TYPE) + m_Message.GetMsgLength() + sizeof(ID_LIST_COUNT_TYPE) + sizeof(OBJECT_ID)*TargetCount;
		if (PacketLen <= GetAllocSize())
		{
			SetTargetIDCount(TargetCount);
			if (pTargetIDs)
				memcpy(GetTargetIDPtr(), pTargetIDs, sizeof(OBJECT_ID)*TargetCount);
			return true;
		}
		else
		{
			assert(false);
			return false;
		}		
	}	
	ID_LIST_COUNT_TYPE GetTargetIDCount()
	{
		ID_LIST_COUNT_TYPE Count = *((ID_LIST_COUNT_TYPE *)((BYTE *)GetPacketData() + sizeof(MSG_LEN_TYPE) + m_Message.GetMsgLength()));
		MSG_LEN_TYPE PacketLen = sizeof(MSG_LEN_TYPE) + m_Message.GetMsgLength() + sizeof(ID_LIST_COUNT_TYPE) + sizeof(OBJECT_ID)*Count;
		if (PacketLen > m_PacketLen)
		{
			assert(false);
		}
		return Count;
	}
	void SetTargetIDCount(ID_LIST_COUNT_TYPE Count)
	{
		MSG_LEN_TYPE PacketLen = sizeof(MSG_LEN_TYPE) + m_Message.GetMsgLength() + sizeof(ID_LIST_COUNT_TYPE) + sizeof(OBJECT_ID)*Count;
		if (PacketLen <= GetAllocSize())
		{
			*((ID_LIST_COUNT_TYPE *)((BYTE *)GetPacketData() + sizeof(MSG_LEN_TYPE) + m_Message.GetMsgLength())) = Count;
		}
		else
		{
			assert(false);
		}
	}
	const OBJECT_ID * GetTargetIDs() const
	{
		return (OBJECT_ID *)((BYTE *)GetPacketData()+sizeof(MSG_LEN_TYPE)+m_Message.GetMsgLength()+sizeof(ID_LIST_COUNT_TYPE));
	}
	OBJECT_ID * GetTargetIDPtr()
	{
		return (OBJECT_ID *)((BYTE *)GetPacketData() + sizeof(MSG_LEN_TYPE) + m_Message.GetMsgLength() + sizeof(ID_LIST_COUNT_TYPE));
	}
	bool AddTargetID(const OBJECT_ID& TargetID)
	{
		MSG_LEN_TYPE PacketLen = sizeof(MSG_LEN_TYPE) + m_Message.GetMsgLength() + sizeof(ID_LIST_COUNT_TYPE) + sizeof(OBJECT_ID)*(GetTargetIDCount() + 1);
		if (PacketLen <= GetAllocSize())
		{
			if(!IsTargetIDExist(TargetID))
			{
				GetTargetIDPtr()[GetTargetIDCount()] = TargetID;
				SetTargetIDCount(GetTargetIDCount() + 1);
			}
			return true;
		}
		else
		{
			assert(false);
			return false;
		}
	}
	bool SortTargetIDs()
	{
		if (GetTargetIDCount() > 1)
		{
			qsort(GetTargetIDPtr(), GetTargetIDCount(), sizeof(OBJECT_ID), OBJECT_ID::Compare);
			return true;
		}
		return false;
	}
	bool IsTargetIDExist(const OBJECT_ID& TargetID)
	{
		ID_LIST_COUNT_TYPE Count = GetTargetIDCount();
		const OBJECT_ID * pObjectID = GetTargetIDs();
		for (UINT i = 0; i < Count; i++)
		{
			if (pObjectID[i] == TargetID)
				return true;
		}
		return false;
	}
	void MakePacketLength()
	{
		m_PacketLen=sizeof(MSG_LEN_TYPE)+GetMessage().GetMsgLength()+sizeof(ID_LIST_COUNT_TYPE)+sizeof(OBJECT_ID)*GetTargetIDCount();
	}
	bool CheckPacket()
	{
		return m_PacketLen<=m_AllocSize;
	}
	bool VerfyPacketLen()
	{
		MSG_LEN_TYPE PacketLen = sizeof(MSG_LEN_TYPE) + m_Message.GetMsgLength() + sizeof(ID_LIST_COUNT_TYPE) + sizeof(OBJECT_ID)*GetTargetIDCount();
		return PacketLen == GetPacketLength();
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

struct SHUTDOWN_INFO
{
	BYTE	Level;
	UINT	Param;
};

struct PROTOCOL_OPTION
{
	UINT	Flag;
};

struct BROADCAST_MASK_SET_INFO
{
	OBJECT_ID	ProxyObjectID;
	UINT64		Mask;
};
struct BROADCAST_GROUP_SET_INFO
{
	OBJECT_ID	ProxyObjectID;
	UINT64		GroupID;
};

struct MASK_BROADCAST_INFO
{
	UINT64			Mask;
	MSG_ID_TYPE		MsgID;
	WORD			MsgFlag;
};
struct GROUP_BROADCAST_INFO
{
	UINT64			GroupID;
	MSG_ID_TYPE		MsgID;
	WORD			MsgFlag;
};

#pragma pack(pop)


struct DISPATCHED_MSG
{
	OBJECT_ID			TargetObjectID;
	CDOSMessagePacket*	pMsgPacket;
};

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
	DSM_PROXY_SET_BROADCAST_MASK,	
	DSM_PROXY_ADD_BROADCAST_MASK,
	DSM_PROXY_REMOVE_BROADCAST_MASK,
	DSM_PROXY_ADD_BROADCAST_GROUP,
	DSM_PROXY_REMOVE_BROADCAST_GROUP,
	DSM_PROXY_BROADCAST_BY_MASK,
	DSM_PROXY_BROADCAST_BY_GROUP,
	DSM_ROUTE_LINK_LOST=100,
	DSM_ROUTE_LINK_START,
	DSM_OBJECT_ALIVE_TEST=200,	
	DSM_OBJECT_FIND,
	DSM_OBJECT_REPORT,
	DSM_SYSTEM_SHUTDOWN,	
	DSM_PROTOCOL_OPTION,	
	DSM_MAX,
};

enum DOS_MESSAGE_FLAG
{
	DOS_MESSAGE_FLAG_SYSTEM_MESSAGE=1,
	DOS_MESSAGE_FLAG_COMPRESSED=(1<<1),
	DOS_MESSAGE_FLAG_CAN_CACHE = (1 << 2),
	DOS_MESSAGE_FLAG_ENCRYPT = (1 << 3),
	DOS_MESSAGE_FLAG_NO_COMPRESS = (1 << 4),
};

enum GLOBAL_MSG_MAP_TYPE
{
	GLOBAL_MSG_MAP_TYPE_OVERLAP,
	GLOBAL_MSG_MAP_TYPE_RANDOM,
	GLOBAL_MSG_MAP_TYPE_BROADCAST,
};

enum PROTOCOL_OPTION_FLAG
{
	PROTOCOL_OPTION_FLAG_UP_MSG_USE_ENCRYPT = 1,
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

inline BOOL PrintDOSMsgStatLog(LPCTSTR Format, ...)
{
	va_list vl;
	va_start(vl, Format);
	BOOL ret = CLogManager::GetInstance()->PrintLogVL(LOG_DOS_MSG_STATE_CHANNEL, ILogPrinter::LOG_LEVEL_NORMAL, NULL, Format, vl);
	va_end(vl);
	return ret;
}

#define PrintDOSLog(_Format, ...)	PrintDOSLogWithTag(_T(__PRETTY_FUNCTION__), _Format, ##__VA_ARGS__)
#define PrintDOSDebugLog(_Format, ...)	PrintDOSDebugLogWithTag(_T(__PRETTY_FUNCTION__), _Format, ##__VA_ARGS__)