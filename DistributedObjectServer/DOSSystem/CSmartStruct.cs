using System;
using System.Collections.Generic;
using System.Text;

namespace DOSSystem
{
    public class CSmartStruct : ICloneable
    {
        byte[] m_pData;
        uint m_StartIndex;
        uint m_DataLen;
        const uint HEAD_SIZE = (sizeof(byte) + sizeof(uint));
        const ushort INVALID_MEMBER_ID = 0xFFFF;



        public CSmartStruct()
        {
            m_StartIndex = 0;
            m_DataLen = 0;
        }
        public CSmartStruct(uint BufferLen)
        {
            m_StartIndex = 0;
            m_DataLen = 0;
            Create(BufferLen);
        }
        public CSmartStruct(byte[] pData, uint StartIndex, uint DataLen, bool IsEmpty)
        {
            m_StartIndex = 0;
            m_DataLen = 0;
            Attach(pData, StartIndex, DataLen, IsEmpty);
        }
        public CSmartStruct(CSmartStruct Struct)
        {
            m_StartIndex = 0;
            m_DataLen = 0;
            Attach(Struct.m_pData, Struct.m_StartIndex, Struct.m_DataLen, false);
        }

        public bool Create(uint BufferLen)
        {
            Destory();
            m_StartIndex = 0;
            m_DataLen = BufferLen + sizeof(byte) + sizeof(uint);
            m_pData = new byte[m_DataLen];
            m_pData[m_StartIndex] = (byte)SMART_VALUE_TYPE.VT_STRUCT;
            SetLength(0);
            return true;
        }

        public bool Attach(byte[] pData, uint StartIndex, uint DataLen, bool IsEmpty)
        {
            if (pData == null)
                return false;
            if (StartIndex + DataLen > pData.Length)
                return false;

            Destory();

            m_pData = pData;
            m_StartIndex = StartIndex;
            m_DataLen = DataLen;
            if (IsEmpty)
            {
                if (DataLen >= sizeof(byte) + sizeof(uint))
                {
                    m_pData[m_StartIndex] = (byte)SMART_VALUE_TYPE.VT_STRUCT;
                    SetLength(0);
                }
                else
                {
                    Destory();
                    return false;
                }
            }
            else
            {
                if ((SMART_VALUE_TYPE)m_pData[StartIndex] != SMART_VALUE_TYPE.VT_STRUCT)
                {
                    Destory();
                    return false;
                }
            }
            return true;
        }
        public bool CloneFrom(CSmartStruct Value)
        {
            if (Value.GetData() == null)
                return false;

            Destory();
            m_pData = new byte[Value.m_DataLen];
            m_StartIndex = 0;
            m_DataLen = Value.m_DataLen;
            Array.Copy(Value.m_pData, Value.m_StartIndex, m_pData, m_StartIndex, m_DataLen);
            return true;
        }

        public void Destory()
        {
            m_pData = null;
            m_StartIndex = 0;
            m_DataLen = 0;
        }

        public void Clear()
        {
            SetLength(0);
        }

        public uint GetLength()
        {
            if (m_pData == null)
                return 0;
            return BitConverter.ToUInt32(m_pData, (int)m_StartIndex + 1);
        }
        void SetLength(uint Len)
        {
            Array.Copy(BitConverter.GetBytes(Len), 0, m_pData, m_StartIndex + 1, sizeof(uint));
        }

        public uint GetDataLen()
        {
            if (m_pData == null)
                return 0;
            uint DataLen = GetLength() + sizeof(byte) + sizeof(uint);
            if (DataLen > (uint)m_DataLen)
                DataLen = (uint)m_DataLen;
            return DataLen;
        }
        public uint GetBufferLen()
        {
            return (uint)m_DataLen;
        }
        public uint GetFreeLen()
        {
            return m_DataLen - GetDataLen();
        }
        public byte[] GetData()
        {
            return m_pData;
        }
        public uint GetDataStartIndex()
        {
            return m_StartIndex;
        }


        public int PrepareMember(ref uint nUsableSize)
        {
            if (m_pData == null)
                return -1;
            if (GetFreeLen() < sizeof(ushort) + sizeof(byte))
            {
                nUsableSize = 0;
                return -1;
            }
            nUsableSize = GetFreeLen() - sizeof(ushort);
            return (int)m_StartIndex + sizeof(byte) + sizeof(uint) + (int)GetLength() + sizeof(ushort);
        }
        public CSmartStruct PrepareSubStruct()
        {
            CSmartStruct SubStruct = null;

            if (m_pData != null && GetFreeLen() >= sizeof(ushort) + sizeof(byte) + sizeof(uint))
            {
                SubStruct = new CSmartStruct();
                uint StartIndex = m_StartIndex + sizeof(byte) + sizeof(uint) + GetLength() + sizeof(ushort);
                uint DataLen = m_DataLen - (StartIndex - m_StartIndex);
                SubStruct.Attach(m_pData, StartIndex, DataLen, true);
            }
            return SubStruct;
        }
        public CSmartArray PrepareSubArray()
        {
            CSmartArray SubArray = null;

            if (m_pData != null && GetFreeLen() >= sizeof(ushort) + sizeof(byte) + sizeof(uint))
            {
                SubArray = new CSmartArray();
                uint StartIndex = m_StartIndex + sizeof(byte) + sizeof(uint) + GetLength() + sizeof(ushort);
                uint DataLen = m_DataLen - (StartIndex - m_StartIndex);
                SubArray.Attach(m_pData, StartIndex, DataLen, true);
            }
            return SubArray;
        }
        public bool FinishMember(ushort ID, uint MemberSize)
        {
            if (m_pData == null)
                return false;
            if (ID == 0)
                return false;
            uint NeedSize = sizeof(short) + MemberSize;
            if (GetFreeLen() < NeedSize)
            {
                return false;
            }
            uint StartIndex = m_StartIndex + sizeof(byte) + sizeof(uint) + GetLength();
            Array.Copy(BitConverter.GetBytes(ID), 0, m_pData, StartIndex, sizeof(ushort));
            SetLength(GetLength() + NeedSize);
            return true;
        }
        public bool AddMemberNull(ushort ID)
        {
            uint BufferSize = 0;
            int StartIndex = PrepareMember(ref BufferSize);
            if (StartIndex >= 0)
            {
                CSmartValue SmartValue = new CSmartValue();
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, SMART_VALUE_TYPE.VT_NULL))
                {
                    return FinishMember(ID, SmartValue.GetDataLen());
                }
            }
            return false;
        }
        public bool AddMember(ushort ID, sbyte Value)
        {
            uint BufferSize = 0;
            int StartIndex = PrepareMember(ref BufferSize);
            if (StartIndex >= 0)
            {
                CSmartValue SmartValue = new CSmartValue();
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, SMART_VALUE_TYPE.VT_CHAR))
                {
                    if (SmartValue.SetValue(Value))
                        return FinishMember(ID, SmartValue.GetDataLen());
                }
            }
            return false;
        }
        public bool AddMember(ushort ID, byte Value)
        {
            uint BufferSize = 0;
            int StartIndex = PrepareMember(ref BufferSize);
            if (StartIndex >= 0)
            {
                CSmartValue SmartValue = new CSmartValue();
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, SMART_VALUE_TYPE.VT_UCHAR))
                {
                    if (SmartValue.SetValue(Value))
                        return FinishMember(ID, SmartValue.GetDataLen());
                }
            }
            return false;
        }
        public bool AddMember(ushort ID, short Value)
        {
            uint BufferSize = 0;
            int StartIndex = PrepareMember(ref BufferSize);
            if (StartIndex >= 0)
            {
                CSmartValue SmartValue = new CSmartValue();
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, SMART_VALUE_TYPE.VT_SHORT))
                {
                    if (SmartValue.SetValue(Value))
                        return FinishMember(ID, SmartValue.GetDataLen());
                }
            }
            return false;
        }
        public bool AddMember(ushort ID, ushort Value)
        {
            uint BufferSize = 0;
            int StartIndex = PrepareMember(ref BufferSize);
            if (StartIndex >= 0)
            {
                CSmartValue SmartValue = new CSmartValue();
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, SMART_VALUE_TYPE.VT_USHORT))
                {
                    if (SmartValue.SetValue(Value))
                        return FinishMember(ID, SmartValue.GetDataLen());
                }
            }
            return false;
        }
        public bool AddMember(ushort ID, int Value)
        {
            uint BufferSize = 0;
            int StartIndex = PrepareMember(ref BufferSize);
            if (StartIndex >= 0)
            {
                CSmartValue SmartValue = new CSmartValue();
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, SMART_VALUE_TYPE.VT_INT))
                {
                    if (SmartValue.SetValue(Value))
                        return FinishMember(ID, SmartValue.GetDataLen());
                }
            }
            return false;
        }
        public bool AddMember(ushort ID, uint Value)
        {
            uint BufferSize = 0;
            int StartIndex = PrepareMember(ref BufferSize);
            if (StartIndex >= 0)
            {
                CSmartValue SmartValue = new CSmartValue();
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, SMART_VALUE_TYPE.VT_UINT))
                {
                    if (SmartValue.SetValue(Value))
                        return FinishMember(ID, SmartValue.GetDataLen());
                }
            }
            return false;
        }
        public bool AddMember(ushort ID, long Value)
        {
            uint BufferSize = 0;
            int StartIndex = PrepareMember(ref BufferSize);
            if (StartIndex >= 0)
            {
                CSmartValue SmartValue = new CSmartValue();
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, SMART_VALUE_TYPE.VT_BIGINT))
                {
                    if (SmartValue.SetValue(Value))
                        return FinishMember(ID, SmartValue.GetDataLen());
                }
            }
            return false;
        }
        public bool AddMember(ushort ID, ulong Value)
        {
            uint BufferSize = 0;
            int StartIndex = PrepareMember(ref BufferSize);
            if (StartIndex >= 0)
            {
                CSmartValue SmartValue = new CSmartValue();
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, SMART_VALUE_TYPE.VT_UBIGINT))
                {
                    if (SmartValue.SetValue(Value))
                        return FinishMember(ID, SmartValue.GetDataLen());
                }
            }
            return false;
        }
        public bool AddMember(ushort ID, float Value)
        {
            uint BufferSize = 0;
            int StartIndex = PrepareMember(ref BufferSize);
            if (StartIndex >= 0)
            {
                CSmartValue SmartValue = new CSmartValue();
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, SMART_VALUE_TYPE.VT_FLOAT))
                {
                    if (SmartValue.SetValue(Value))
                        return FinishMember(ID, SmartValue.GetDataLen());
                }
            }
            return false;
        }
        public bool AddMember(ushort ID, double Value)
        {
            uint BufferSize = 0;
            int StartIndex = PrepareMember(ref BufferSize);
            if (StartIndex >= 0)
            {
                CSmartValue SmartValue = new CSmartValue();
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, SMART_VALUE_TYPE.VT_DOUBLE))
                {
                    if (SmartValue.SetValue(Value))
                        return FinishMember(ID, SmartValue.GetDataLen());
                }
            }
            return false;
        }
        public bool AddMember(ushort ID, bool Value)
        {
            uint BufferSize = 0;
            int StartIndex = PrepareMember(ref BufferSize);
            if (StartIndex >= 0)
            {
                CSmartValue SmartValue = new CSmartValue();
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, SMART_VALUE_TYPE.VT_BOOL))
                {
                    if (SmartValue.SetValue(Value))
                        return FinishMember(ID, SmartValue.GetDataLen());
                }
            }
            return false;
        }
        public bool AddMember(ushort ID, string Value)
        {
            uint BufferSize = 0;
            int StartIndex = PrepareMember(ref BufferSize);
            if (StartIndex >= 0)
            {
                CSmartValue SmartValue = new CSmartValue();

                bool Ret = false;
                switch (CSmartValue.INTERNAL_STRING_CODE_PAGE)
                {
                    case STRING_CODE_PAGE.STRING_CODE_PAGE_ANSI:
                        Ret = SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, SMART_VALUE_TYPE.VT_STRING_ANSI);
                        break;
                    case STRING_CODE_PAGE.STRING_CODE_PAGE_UTF8:
                        Ret = SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, SMART_VALUE_TYPE.VT_STRING_UTF8);
                        break;
                    case STRING_CODE_PAGE.STRING_CODE_PAGE_UCS16:
                        Ret = SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, SMART_VALUE_TYPE.VT_STRING_UCS16);
                        break;
                }
                if (Ret)
                {
                    if (SmartValue.SetValue(Value))
                        return FinishMember(ID, SmartValue.GetDataLen());
                }
            }
            return false;
        }
        public bool AddMember(ushort ID, byte[] Value)
        {
            uint BufferSize = 0;
            int StartIndex = PrepareMember(ref BufferSize);
            if (StartIndex >= 0)
            {
                CSmartValue SmartValue = new CSmartValue();
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, SMART_VALUE_TYPE.VT_BINARY))
                {
                    if (SmartValue.SetValue(Value))
                        return FinishMember(ID, SmartValue.GetDataLen());
                }
            }
            return false;
        }
        public bool AddMember(ushort ID, CSmartStruct Value)
        {
            uint BufferSize = 0;
            int StartIndex = PrepareMember(ref BufferSize);
            if (StartIndex >= 0)
            {
                CSmartValue SmartValue = new CSmartValue();
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, SMART_VALUE_TYPE.VT_STRUCT))
                {
                    if (SmartValue.SetValue(Value))
                        return FinishMember(ID, SmartValue.GetDataLen());
                }
            }
            return false;
        }
        public bool AddMember(ushort ID, CSmartValue Value)
        {
            uint BufferSize = 0;
            int StartIndex = PrepareMember(ref BufferSize);
            if (StartIndex >= 0)
            {
                CSmartValue SmartValue = new CSmartValue();
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, SMART_VALUE_TYPE.VT_STRUCT))
                {
                    if (SmartValue.SetValue(Value))
                        return FinishMember(ID, SmartValue.GetDataLen());
                }
            }
            return false;
        }
        public bool AddMember(ushort ID, CVariedValue Value)
        {
            switch (Value.Type)
            {
                case VARIED_VALUE_TYPE.NIL:
                    return AddMemberNull(ID);
                case VARIED_VALUE_TYPE.BOOLEAN:
                    return AddMember(ID, (bool)Value);
                case VARIED_VALUE_TYPE.INT8:
                    return AddMember(ID, (sbyte)Value);
                case VARIED_VALUE_TYPE.UINT8:
                    return AddMember(ID, (byte)Value);
                case VARIED_VALUE_TYPE.INT16:
                    return AddMember(ID, (short)Value);
                case VARIED_VALUE_TYPE.UINT16:
                    return AddMember(ID, (ushort)Value);
                case VARIED_VALUE_TYPE.INT32:
                    return AddMember(ID, (int)Value);
                case VARIED_VALUE_TYPE.UINT32:
                    return AddMember(ID, (uint)Value);
                case VARIED_VALUE_TYPE.INT64:
                    return AddMember(ID, (long)Value);
                case VARIED_VALUE_TYPE.UINT64:
                    return AddMember(ID, (ulong)Value);
                case VARIED_VALUE_TYPE.FLOAT32:
                    return AddMember(ID, (float)Value);
                case VARIED_VALUE_TYPE.FLOAT64:
                    return AddMember(ID, (double)Value);
                case VARIED_VALUE_TYPE.STRING:
                    return AddMember(ID, (string)Value);
                case VARIED_VALUE_TYPE.BINARY:
                    return AddMember(ID, (byte[])Value);
                case VARIED_VALUE_TYPE.ARRAY:
                case VARIED_VALUE_TYPE.TABLE:
                    return AddMember(ID, (CSmartValue)Value);
            }
            return false;
        }
        public int GetMemberCount()
        {
            if (m_pData == null)
                return 0;
            uint HeadIndex = m_StartIndex;
            uint TailIndex = m_StartIndex + GetDataLen();
            int MemberCount = 0;
            HeadIndex += sizeof(byte) + sizeof(uint);
            while (HeadIndex + sizeof(ushort) < TailIndex)
            {
                HeadIndex += sizeof(ushort);

                CSmartValue Value = new CSmartValue(m_pData, HeadIndex, TailIndex - HeadIndex, SMART_VALUE_TYPE.VT_UNKNOWN);
                HeadIndex += Value.GetDataLen();
                MemberCount++;
            }
            return MemberCount;
        }
        public CSmartValue GetMemberByIndex(uint Index)
        {
            CSmartValue Value = new CSmartValue();

            if (m_pData == null)
                return Value;
            uint HeadIndex = m_StartIndex;
            uint TailIndex = m_StartIndex + GetDataLen();

            HeadIndex += sizeof(byte) + sizeof(uint);
            while (HeadIndex + sizeof(ushort) < TailIndex)
            {
                HeadIndex += sizeof(ushort);
                if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, SMART_VALUE_TYPE.VT_UNKNOWN))
                    break;
                HeadIndex += Value.GetDataLen();
                if (Index == 0)
                    return Value;
                Index--;
            }
            Value.Destory();
            return Value;

        }
        public CSmartValue GetMember(ushort ID)
        {
            CSmartValue Value = new CSmartValue();

            if (m_pData == null)
                return Value;
            uint HeadIndex = m_StartIndex;
            uint TailIndex = m_StartIndex + GetDataLen();

            HeadIndex += sizeof(byte) + sizeof(uint);
            while (HeadIndex + sizeof(ushort) < TailIndex)
            {
                ushort MemberID = BitConverter.ToUInt16(m_pData, (int)HeadIndex);
                HeadIndex += sizeof(ushort);
                if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, SMART_VALUE_TYPE.VT_UNKNOWN))
                    break;
                if (MemberID == ID)
                    return Value;
                HeadIndex += Value.GetDataLen();
            }
            Value.Destory();
            return Value;
        }
        public bool IsMemberExist(ushort ID)
        {
            if (IDToIndex(ID) == INVALID_MEMBER_ID)
            {
                return false;
            }
            return true;
        }
        public uint IDToIndex(ushort ID)
        {
            CSmartValue Value = new CSmartValue();

            if (m_pData == null)
                return INVALID_MEMBER_ID;

            uint HeadIndex = m_StartIndex;
            uint TailIndex = m_StartIndex + GetDataLen();

            uint Index = 0;

            HeadIndex += sizeof(byte) + sizeof(uint);
            while (HeadIndex + sizeof(ushort) < TailIndex)
            {
                ushort MemberID = BitConverter.ToUInt16(m_pData, (int)HeadIndex);
                HeadIndex += sizeof(ushort);
                if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, SMART_VALUE_TYPE.VT_UNKNOWN))
                    break;
                if (MemberID == ID)
                    return Index;
                HeadIndex += Value.GetDataLen();
                Index++;
            }
            return INVALID_MEMBER_ID;
        }
        public ushort IndexToID(uint Index)
        {
            CSmartValue Value = new CSmartValue();

            if (m_pData == null)
                return 0;

            uint HeadIndex = m_StartIndex;
            uint TailIndex = m_StartIndex + GetDataLen();
            ushort MemberID = 0;

            HeadIndex += sizeof(byte) + sizeof(uint);
            while (HeadIndex + sizeof(ushort) < TailIndex)
            {
                MemberID = BitConverter.ToUInt16(m_pData, (int)HeadIndex);
                if (Index == 0)
                    return MemberID;
                HeadIndex += sizeof(ushort);
                if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, SMART_VALUE_TYPE.VT_UNKNOWN))
                    break;
                HeadIndex += Value.GetDataLen();
                Index--;
            }
            return 0;
        }
        public int GetFirstMemberPosition()
        {
            if (m_pData == null)
                return 0;


            uint HeadIndex = m_StartIndex;
            uint TailIndex = m_StartIndex + GetDataLen();
            HeadIndex += sizeof(byte) + sizeof(uint);

            if (HeadIndex < TailIndex)
                return (int)HeadIndex;
            else
                return 0;
        }
        public CSmartValue GetNextMember(ref int Pos, ref ushort MemberID)
        {
            CSmartValue Value = new CSmartValue();

            if (m_pData == null)
            {
                Pos = 0;
                return Value;
            }


            if (Pos != 0)
            {
                uint HeadIndex = (uint)Pos;
                uint TailIndex = m_StartIndex + GetDataLen();
                MemberID = BitConverter.ToUInt16(m_pData, (int)HeadIndex);

                HeadIndex += sizeof(ushort);

                if (HeadIndex >= TailIndex)
                {
                    Pos = 0;
                }
                else
                {
                    if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, SMART_VALUE_TYPE.VT_UNKNOWN))
                    {
                        Pos = 0;
                        return Value;
                    }
                    Pos = (int)(HeadIndex + Value.GetDataLen());
                    if (Pos >= (int)TailIndex)
                    {
                        Pos = 0;
                    }
                }

            }
            return Value;
        }
        public static uint GetDataLenFromData(byte[] Data, uint Index)
        {
            if (Data == null || Data.Length - Index < HEAD_SIZE)
                return 0;
            return BitConverter.ToUInt32(Data, (int)Index + sizeof(byte)) + HEAD_SIZE;
        }

        public static uint GetEmptyStructSize()
        {
            return sizeof(byte) + sizeof(uint);
        }

        public static uint GetFixMemberSize(uint TypeLen)
        {
            return sizeof(ushort) + sizeof(byte) + TypeLen;
        }
        public static uint GetStructMemberSize(uint StructSize)
        {
            return sizeof(ushort) + sizeof(byte) + sizeof(uint) + StructSize;
        }
        public static uint GetArrayMemberSize(uint StructSize)
        {
            return sizeof(ushort) + sizeof(byte) + sizeof(uint) + StructSize;
        }
        public static uint GetBinaryMemberSize(uint Len)
        {
            return sizeof(ushort) + sizeof(byte) + sizeof(uint) + sizeof(byte) * (Len);
        }
        public static uint GetStringMemberSize(string Str)
        {
            uint DataLen = 0;
            switch (CSmartValue.INTERNAL_STRING_CODE_PAGE)
            {
                case STRING_CODE_PAGE.STRING_CODE_PAGE_ANSI:
                    DataLen = 1;
                    if (!string.IsNullOrEmpty(Str))
                    {
                        DataLen += (uint)Encoding.GetEncoding(936).GetByteCount(Str);
                    }
                    break;
                case STRING_CODE_PAGE.STRING_CODE_PAGE_UTF8:
                    DataLen = 1;
                    if (!string.IsNullOrEmpty(Str))
                    {
                        DataLen += (uint)Encoding.UTF8.GetByteCount(Str);
                    }
                    break;
                case STRING_CODE_PAGE.STRING_CODE_PAGE_UCS16:
                    DataLen = sizeof(char);
                    if (!string.IsNullOrEmpty(Str))
                    {
                        DataLen += (uint)Encoding.Unicode.GetByteCount(Str);
                    }
                    break;
            }
            return sizeof(ushort) + sizeof(byte) + sizeof(uint) + DataLen;
        }
        public static uint GetVariedMemberSize(CVariedValue Value)
        {
            switch (Value.Type)
            {
                case VARIED_VALUE_TYPE.NIL:
                    return sizeof(ushort) + sizeof(byte);
                case VARIED_VALUE_TYPE.BOOLEAN:
                    return GetFixMemberSize(sizeof(bool));
                case VARIED_VALUE_TYPE.INT8:
                    return GetFixMemberSize(sizeof(sbyte));
                case VARIED_VALUE_TYPE.UINT8:
                    return GetFixMemberSize(sizeof(byte));
                case VARIED_VALUE_TYPE.INT16:
                    return GetFixMemberSize(sizeof(short));
                case VARIED_VALUE_TYPE.UINT16:
                    return GetFixMemberSize(sizeof(ushort));
                case VARIED_VALUE_TYPE.INT32:
                    return GetFixMemberSize(sizeof(int));
                case VARIED_VALUE_TYPE.UINT32:
                    return GetFixMemberSize(sizeof(uint));
                case VARIED_VALUE_TYPE.INT64:
                    return GetFixMemberSize(sizeof(long));
                case VARIED_VALUE_TYPE.UINT64:
                    return GetFixMemberSize(sizeof(ulong));
                case VARIED_VALUE_TYPE.FLOAT32:
                    return GetFixMemberSize(sizeof(float));
                case VARIED_VALUE_TYPE.FLOAT64:
                    return GetFixMemberSize(sizeof(double));
                case VARIED_VALUE_TYPE.STRING:
                    return GetStringMemberSize((string)Value); ;
            }
            return sizeof(ushort) + sizeof(byte);
        }
        public object Clone()
        {
            CSmartStruct Struct = new CSmartStruct();
            Struct.CloneFrom(this);
            return Struct;
        }
    }
}
