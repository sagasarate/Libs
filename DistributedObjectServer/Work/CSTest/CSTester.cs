using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using DOSSystem;


namespace CSTest
{
    public class CSTester : ICSTester, ICSTesterAck
    {
        DistributedObjectOperator m_Operator;
        OBJECT_ID m_CurMsgSenderID;
        OBJECT_ID m_TargetID;
        Dictionary<uint, CBaseMsgHandler.MsgHandler> m_MsgMap = new Dictionary<uint, CBaseMsgHandler.MsgHandler>();
        CCSTesterMsgHandler m_CSTesterMsgHandler = new CCSTesterMsgHandler();
        CCSTesterAckMsgHandler m_CSTesterAckMsgHandler = new CCSTesterAckMsgHandler();

        uint TotalDelay = 0;
        uint DelayCount = 0;

        uint CountStartTime = 0;

        public bool IsActive = false;
        bool Initialize(DistributedObjectOperator Operator)
        {

            m_Operator = Operator;
            //m_Operator.SetUnhanleMsgReceiver(0xFFFF, 0xFF);            

            m_CSTesterMsgHandler.InitMsgMap(this, m_MsgMap);
            m_CSTesterAckMsgHandler.InitMsgMap(this, m_MsgMap);

            CountStartTime = (uint)System.Environment.TickCount;
            m_TargetID.ID = 0;


            m_Operator.SetServerWorkStatus(2);
            Logger.Log("CSTester Initialized");
            return true;
        }
        void Destory()
        {

        }
        bool OnPreTranslateMessage(uint MsgID, ushort MsgFlag, OBJECT_ID SenderID, byte[] Data)
        {
            return false;
        }
        bool OnMessage(uint MsgID, ushort MsgFlag, OBJECT_ID SenderID, byte[] Data)
        {
            CBaseMsgHandler.MsgHandler Handler;
            if (m_MsgMap.TryGetValue(MsgID,out Handler))
            {
                m_CurMsgSenderID = SenderID;
                CSmartStruct Packet = new CSmartStruct(Data, 0, (uint)Data.Length, false);
                Handler(Packet);
                return true;
            }
            return false;
        }
        bool OnSystemMessage(uint MsgID, ushort MsgFlag, OBJECT_ID SenderID, byte[] Data)
        {
            return false;
        }
        void OnConcernedObjectLost(OBJECT_ID ObjectID)
        {

        }
        bool OnFindObject(OBJECT_ID CallerID)
        {
            return false;
        }
        void OnObjectReport(OBJECT_ID ObjectID, byte[] Data)
        {
            if (ObjectID != m_Operator.GetObjectID())
            {
                m_TargetID = ObjectID;
                Logger.Log("Finded{0}", m_TargetID);
                if (IsActive)
                {
                    byte[] PingData = new byte[32];
                    for(int i=0;i<100;i++)
                    {
                        CCSTesterMsgCaller MsgCaller = new CCSTesterMsgCaller(m_Operator, m_TargetID);
                        MsgCaller.Ping((uint)System.Environment.TickCount, PingData);
                    }                 
                }
            }
        }
        void OnProxyObjectIPReport(OBJECT_ID ProxyObjectID, uint Port, string IPStr)
        {

        }
        void OnShutDown(byte Level, uint Param)
        {

        }
        int Update(int ProcessPacketLimit)
        {
            uint CurTime = (uint)System.Environment.TickCount;
            if (CurTime - CountStartTime > 1000)
            {
                CountStartTime = CurTime;
                float DelayAvg = (float)TotalDelay / (float)DelayCount;
                Logger.Log("Delay={0},Count={1}", DelayAvg, DelayCount);
                TotalDelay = 0;
                DelayCount = 0;

                if (m_TargetID.ID == 0)
                {
                    m_Operator.FindObject(0x11);
                }
            }
            return 0;
        }

        public int Ping(uint Time, byte[] Data)
        {
            CCSTesterMsgCaller MsgCaller = new CCSTesterMsgCaller(m_Operator, m_CurMsgSenderID);
            MsgCaller.Ping((uint)System.Environment.TickCount, Data);

            CCSTesterAckMsgCaller AckMsgCaller = new CCSTesterAckMsgCaller(m_Operator, m_CurMsgSenderID);
            AckMsgCaller.PingAck(0, Time);
            return 0;
        }

        public int PingAck(short Result, uint Time)
        {
            uint CurTime = (uint)System.Environment.TickCount;
            TotalDelay += CurTime - Time;
            DelayCount++;
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

};