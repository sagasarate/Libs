using System;
using System.Runtime.CompilerServices;

namespace DOSSystem
{
   
    
    public class DOS_OBJECT_REGISTER_INFO_EX
    {
        public OBJECT_ID ObjectID;
        public string ObjectTypeName;
        public int Weight = 1;
        public int ObjectGroupIndex = -1;
        public uint MsgQueueSize = 1024;
        public uint MsgProcessLimit = 32;
        public uint Flag = 0;
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
        public bool RegisterGlobalMsgMap(ushort ProxyRouterID, byte ProxyType, uint MsgID, int MapType)
        {
            return InternalCallRegisterGlobalMsgMap(m_ObjectHandle, ProxyRouterID, ProxyType, MsgID, MapType);
        }
        public bool UnregisterGlobalMsgMap(ushort ProxyRouterID, byte ProxyType, uint MsgID)
        {
            return InternalCallUnregisterGlobalMsgMap(m_ObjectHandle, ProxyRouterID, ProxyType, MsgID);
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

        public bool QueryShutDown(OBJECT_ID TargetID, byte Level, uint Param)
        {
            return InternalCallQueryShutDown(m_ObjectHandle, TargetID, Level, Param);
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

        public bool RegisterCommandReceiver()
        {
            return InternalCallRegisterCommandReceiver(m_ObjectHandle);
        }
	    public bool UnregisterCommandReceiver()
        {
            return InternalCallUnregisterCommandReceiver(m_ObjectHandle);
        }

        public void SetServerWorkStatus(byte WorkStatus)
        {
            InternalCallSetServerWorkStatus(m_ObjectHandle, WorkStatus);
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
        extern static bool InternalCallRegisterGlobalMsgMap(IntPtr ObjectHandle, ushort ProxyRouterID, byte ProxyType, uint MsgID, int MapType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallUnregisterGlobalMsgMap(IntPtr ObjectHandle, ushort ProxyRouterID, byte ProxyType, uint MsgID);
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
        extern static bool InternalCallQueryShutDown(IntPtr ObjectHandle, OBJECT_ID TargetID, byte Level, uint Param);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static void InternalCallShutDown(IntPtr ObjectHandle,uint PluginID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallRegisterLogger(uint LogChannel, string FileName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallRegisterCSVLogger(uint LogChannel, string FileName, string CSVLogHeader);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallRegisterCommandReceiver(IntPtr ObjectHandle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallUnregisterCommandReceiver(IntPtr ObjectHandle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static bool InternalCallSetServerWorkStatus(IntPtr ObjectHandle, byte WorkStatus);
        
    };

    class DistributedObjectSample
    {
        public bool Initialize(DistributedObjectOperator Operator)
        {
            return false;
        }
        public void Destory()
        {

        }
        public bool OnPreTranslateMessage(uint MsgID, ushort MsgFlag, OBJECT_ID SenderID, byte[] Data)
        {
            return false;
        }
        public bool OnMessage(uint MsgID, ushort MsgFlag, OBJECT_ID SenderID, byte[] Data)
        {
            return false;
        }
        public bool OnSystemMessage(uint MsgID, ushort MsgFlag, OBJECT_ID SenderID, byte[] Data)
        {
            return false;
        }
        public void OnConcernedObjectLost(OBJECT_ID ObjectID)
        {

        }
        public bool OnFindObject(OBJECT_ID CallerID)
        {
            return false;
        }
        public void OnObjectReport(OBJECT_ID ObjectID, byte[] Data)
        {
            
        }
        public void OnProxyObjectIPReport(OBJECT_ID ProxyObjectID, uint Port, string IPStr)
        {

        }
        public void OnShutDown(byte Level, uint Param)
        {

        }
        public int Update(int ProcessPacketLimit)
        {
            return 0;
        }

        public void OnException(System.Exception Exp)
        {

        }

        public bool OnConsoleCommand(string Command)
        {
            return false;
        }
    };

    class DistributedObjectPluginSample
    {
        public bool InitPlugin(uint PluginID, uint LogChannel, string ConfigDir, string LogDir)
        {
            return true;
        }
        public void QueryReleasePlugin()
        {

        }
        public bool CheckPluginRelease()
        {
            return true;
        }
    };
}