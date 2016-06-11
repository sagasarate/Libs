using System;
using System.Runtime.CompilerServices;

namespace DOSSystem
{
   
    
    public class DOS_OBJECT_REGISTER_INFO_EX
    {
        public OBJECT_ID ObjectID;
        public int Weight = 1;
        public int ObjectGroupIndex = -1;
        public uint MsgQueueSize = 1024;
        public uint MsgProcessLimit = 32;
        public object Object = null;
    };

    

    public class DistributedObjectOperator
    {
        IntPtr m_ObjectHandle;

        DistributedObjectOperator(IntPtr ObjectHandle)
        {
            m_ObjectHandle = ObjectHandle;
        }

        public static uint GetRouterID()
        {
            return InternalCallGetRouterID();
        }
        public OBJECT_ID GetObjectID()
        {
            return InternalCallGetObjectID(m_ObjectHandle);
        }
        public int GetGroupIndex()
        {
            return InternalCallGetGroupIndex(m_ObjectHandle);
        }
        public bool SendMessage(OBJECT_ID ReceiverID, uint MsgID, ushort MsgFlag, byte[] Data, int StartIndex, int DataLen)
        {
            return InternalCallSendMessage(m_ObjectHandle, ReceiverID, MsgID, MsgFlag, Data, StartIndex, DataLen);
        }
        public bool SendMessageMulti(OBJECT_ID[] ReceiverIDList, bool IsSorted, uint MsgID, ushort MsgFlag, byte[] Data, int StartIndex, int DataLen)
        {
            return InternalCallSendMessageMulti(m_ObjectHandle, ReceiverIDList, IsSorted, MsgID, MsgFlag, Data, StartIndex, DataLen);
        }


        public bool RegisterMsgMap(OBJECT_ID ProxyObjectID, uint[] MsgIDList)
        {
            return InternalCallRegisterMsgMap(m_ObjectHandle, ProxyObjectID, MsgIDList);
        }
        public bool UnregisterMsgMap(OBJECT_ID ProxyObjectID, uint[] MsgIDList)
        {
            return InternalCallUnregisterMsgMap(m_ObjectHandle, ProxyObjectID, MsgIDList);
        }
        public bool RegisterGlobalMsgMap(ushort ProxyRouterID, byte ProxyType, uint[] MsgIDList)
        {
            return InternalCallRegisterGlobalMsgMap(m_ObjectHandle, ProxyRouterID, ProxyType, MsgIDList);
        }
        public bool UnregisterGlobalMsgMap(ushort ProxyRouterID, byte ProxyType, uint[] MsgIDList)
        {
            return InternalCallUnregisterGlobalMsgMap(m_ObjectHandle, ProxyRouterID, ProxyType, MsgIDList);
        }

        public bool SetUnhanleMsgReceiver(ushort ProxyRouterID, byte ProxyType)
        {
            return InternalCallSetUnhanleMsgReceiver(m_ObjectHandle, ProxyRouterID, ProxyType);
        }

        public bool AddConcernedObject(OBJECT_ID ObjectID, bool NeedTest)
        {
            return InternalCallAddConcernedObject(m_ObjectHandle, ObjectID, NeedTest);
        }
        public bool DeleteConcernedObject(OBJECT_ID ObjectID)
        {
            return InternalCallDeleteConcernedObject(m_ObjectHandle, ObjectID);
        }

        public bool FindObject(uint ObjectType)
        {
            return InternalCallFindObject(m_ObjectHandle, ObjectType);
        }
        public bool ReportObject(OBJECT_ID TargetID, byte[] Data, int StartIndex, int DataLen)
        {
            return InternalCallReportObject(m_ObjectHandle, TargetID, Data, StartIndex, DataLen);
        }
        public bool CloseProxyObject(OBJECT_ID ProxyObjectID, uint Delay)
        {
            return InternalCallCloseProxyObject(m_ObjectHandle, ProxyObjectID, Delay);
        }
        public bool RequestProxyObjectIP(OBJECT_ID ProxyObjectID)
        {
            return InternalCallRequestProxyObjectIP(m_ObjectHandle, ProxyObjectID);
        }

        public static bool RegisterObject(uint PluginID, DOS_OBJECT_REGISTER_INFO_EX ObjectRegisterInfo)
        {
            return InternalCallRegisterObject(PluginID, ObjectRegisterInfo);
        }

        public bool RegisterObject(DOS_OBJECT_REGISTER_INFO_EX ObjectRegisterInfo)
        {
            return InternalCallRegisterObject(m_ObjectHandle, ObjectRegisterInfo);
        }
        public void Release()
        {
            InternalCallRelease(m_ObjectHandle);
        }

        public bool QueryShutDown(OBJECT_ID TargetID, int Level)
        {
            return InternalCallQueryShutDown(m_ObjectHandle, TargetID, Level);
        }
        public void ShutDown(uint PluginID)
        {
            InternalCallShutDown(m_ObjectHandle, PluginID);
        }

        public static bool RegisterLogger(uint LogChannel, string FileName)
        {
            return InternalCallRegisterLogger(LogChannel, FileName);
        }
        public static bool RegisterCSVLogger(uint LogChannel, string FileName, string CSVLogHeader)
        {
            return InternalCallRegisterCSVLogger(LogChannel, FileName, CSVLogHeader);
        }


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static uint InternalCallGetRouterID();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static OBJECT_ID InternalCallGetObjectID(IntPtr ObjectHandle);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static int InternalCallGetGroupIndex(IntPtr ObjectHandle);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallSendMessage(IntPtr ObjectHandle, OBJECT_ID ReceiverID, uint MsgID, ushort MsgFlag, byte[] Data, int StartIndex, int DataLen);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallSendMessageMulti(IntPtr ObjectHandle, OBJECT_ID[] ReceiverIDList, bool IsSorted, uint MsgID, ushort MsgFlag, byte[] Data, int StartIndex, int DataLen);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallRegisterMsgMap(IntPtr ObjectHandle, OBJECT_ID ProxyObjectID, uint[] MsgIDList);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallUnregisterMsgMap(IntPtr ObjectHandle, OBJECT_ID ProxyObjectID, uint[] MsgIDList);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallRegisterGlobalMsgMap(IntPtr ObjectHandle, ushort ProxyRouterID, byte ProxyType, uint[] MsgIDList);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallUnregisterGlobalMsgMap(IntPtr ObjectHandle, ushort ProxyRouterID, byte ProxyType, uint[] MsgIDList);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallSetUnhanleMsgReceiver(IntPtr ObjectHandle, ushort ProxyRouterID, byte ProxyType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallAddConcernedObject(IntPtr ObjectHandle, OBJECT_ID ObjectID, bool NeedTest);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallDeleteConcernedObject(IntPtr ObjectHandle, OBJECT_ID ObjectID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallFindObject(IntPtr ObjectHandle, uint ObjectType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallReportObject(IntPtr ObjectHandle, OBJECT_ID TargetID, byte[] Data, int StartIndex, int DataLen);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallCloseProxyObject(IntPtr ObjectHandle, OBJECT_ID ProxyObjectID, uint Delay);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallRequestProxyObjectIP(IntPtr ObjectHandle, OBJECT_ID ProxyObjectID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallRegisterObject(uint PluginID, DOS_OBJECT_REGISTER_INFO_EX ObjectRegisterInfo);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallRegisterObject(IntPtr ObjectHandle, DOS_OBJECT_REGISTER_INFO_EX ObjectRegisterInfo);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static void InternalCallRelease(IntPtr ObjectHandle);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallQueryShutDown(IntPtr ObjectHandle,OBJECT_ID TargetID, int Level);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static void InternalCallShutDown(IntPtr ObjectHandle,uint PluginID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallRegisterLogger(uint LogChannel, string FileName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallRegisterCSVLogger(uint LogChannel, string FileName, string CSVLogHeader);
    };
}