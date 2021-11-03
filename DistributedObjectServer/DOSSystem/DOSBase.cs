using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.CompilerServices;


namespace DOSSystem
{
    public struct OBJECT_ID
    {
        public ulong ID;
       
        public OBJECT_ID(ulong id)
        {
            ID = id;
            //Logger.Log("Create OBJECT_ID=0x{0:X}", ID);
        }
        public OBJECT_ID(ushort RouterID, ushort ObjectTypeID, ushort GroupIndex, ushort ObjectIndex)
        {
            ID = (((ulong)RouterID) << 48) | (((ulong)ObjectTypeID) << 32) | (((ulong)GroupIndex) << 16) | ((ulong)ObjectIndex);
        }
        public void MakeID(ushort RouterID, ushort ObjectTypeID, ushort GroupIndex, ushort ObjectIndex)
        {
            ID = (((ulong)RouterID) << 48) | (((ulong)ObjectTypeID) << 32) | (((ulong)GroupIndex) << 16) | ((ulong)ObjectIndex);
        }
        public ushort RouterID
        {
            get { return (ushort)((ID >> 48) & 0xFFFF); }
            set { ID=(ID&((ulong)0x0000FFFFFFFFFFFFL)|((ulong)value) << 48);}
        }
        public ushort ObjectTypeID
        {
            get { return (ushort)((ID >> 32) & 0xFFFF); }
            set { ID = (ID & ((ulong)0xFFFF0000FFFFFFFFL) | ((ulong)value) << 32); }
        }
        public ushort GroupIndex
        {
            get { return (ushort)((ID >> 16) & 0xFFFF); }
            set { ID = (ID & ((ulong)0xFFFFFFFF0000FFFFL) | ((ulong)value) << 16); }
        }
        public ushort ObjectIndex
        {
            get { return (ushort)(ID & 0xFFFF); }
            set { ID = (ID & ((ulong)0xFFFFFFFFFFFF0000L) | ((ulong)value)); }
        }
        public override string ToString ()
        {
            return string.Format("0x{0:X}", ID);
        }
        public static implicit operator ulong(OBJECT_ID ObjectID)
        {
            return ObjectID.ID;
        }
        public static implicit operator OBJECT_ID(ulong ID)
        {
            return new OBJECT_ID(ID);
        }
        public static bool operator==(OBJECT_ID ObjectID1,OBJECT_ID ObjectID2)
        {
            return ObjectID1.ID == ObjectID2.ID;
        }
        public static bool operator !=(OBJECT_ID ObjectID1, OBJECT_ID ObjectID2)
        {
            return ObjectID1.ID != ObjectID2.ID;
        }
        public override int GetHashCode()
        {
            return ID.GetHashCode();
        }
        public override bool Equals(object Other)
        {
            if (Other is OBJECT_ID)
            {
                return ID == ((OBJECT_ID)Other).ID;
            }
            return false;
        }
    }
    public enum COMMON_RESULT_CODE
    {
        COMMON_RESULT_SUCCEED = 0,
        COMMON_RESULT_MSG_PACK_ERROR = 189001,
        COMMON_RESULT_MSG_UNPACK_ERROR = 189002,
        COMMON_RESULT_MSG_ALLOC_ERROR = 189101,
        COMMON_RESULT_MSG_SEND_ERROR = 189102,
        COMMON_RESULT_MSG_HANDLER_NOT_FIND = 189103,
        COMMON_RESULT_FAILED = -2,
    };
    enum DOS_MESSAGE_FLAG
    {
        DOS_MESSAGE_FLAG_SYSTEM_MESSAGE = 1,
        DOS_MESSAGE_FLAG_COMPRESSED = (1 << 1),
        DOS_MESSAGE_FLAG_CAN_CACHE = (1 << 2),
        DOS_MESSAGE_FLAG_ENCRYPT = (1 << 3),
        DOS_MESSAGE_FLAG_NO_COMPRESS = (1 << 4),
    };

    enum BROAD_CAST_IDS
    {
        BROAD_CAST_ROUTER_ID = 0xFFFF,
        BROAD_CAST_OBJECT_TYPE_ID = 0xFFFF,
        BROAD_CAST_GROUP_INDEX = 0xFFFF,
        BROAD_CAST_OBJECT_INDEX = 0xFFFF,
        BROAD_CAST_PROXY_TYPE = 0xFF,
        BROAD_CAST_PROXY_ID = 0xFF,
    }
    
    public class CBaseMsgHandler
    {
        public static uint MakeMsgID(uint ModuleID, uint InterfaceID, uint MethodID, bool IsAck)
        {
            return ((((uint)ModuleID & 0x7F) << 24) | (((uint)InterfaceID & 0xFF) << 16) | (((uint)MethodID) & 0xFFFF) | (IsAck ? 0x80000000 : 0));
        }

        public delegate int MsgHandler(CSmartStruct MsgPacket);
    }

    public class Logger
    {
        public static uint LogChannel;

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static void InternalLog(uint LogChannel, string Msg);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static void InternalLogDebug(uint LogChannel, string Msg);

        public static void Log(string Msg)
        {
            InternalLog(Logger.LogChannel, Msg);
        }

        public static void LogDebug(string Msg)
        {
            InternalLogDebug(Logger.LogChannel, Msg);
        }
        public static void Log(string Format, params object[] Params)
        {
            InternalLog(Logger.LogChannel, string.Format(Format, Params));
        }

        public static void LogDebug(string Format, params object[] Params)
        {
            InternalLogDebug(Logger.LogChannel, string.Format(Format, Params));
        }
    };
}
