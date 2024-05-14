using System;
using System.Collections.Generic;
using System.Text;

namespace DOSSystem
{
    public class CSmartArray : ICloneable
    {
        byte[] m_pData;
        uint m_StartIndex;
        uint m_DataLen;
        uint m_ElementSize;
        const uint HEAD_SIZE = (sizeof(byte) + sizeof(uint));

        public class ElementTypeNotMatchException : Exception
        {
            public ElementTypeNotMatchException(string Msg) : base(Msg)
            {

            }
        }

        public CSmartArray()
        {
            m_StartIndex = 0;
            m_DataLen = 0;
            m_ElementSize = 0;
        }
        public CSmartArray(uint BufferLen)
        {
            m_StartIndex = 0;
            m_DataLen = 0;
            m_ElementSize = 0;
            Create(BufferLen);
        }
        public CSmartArray(byte[] pData, uint StartIndex, uint DataLen, bool IsEmpty)
        {
            m_StartIndex = 0;
            m_DataLen = 0;
            m_ElementSize = 0;
            Attach(pData, StartIndex, DataLen, IsEmpty);
        }
        public CSmartArray(CSmartArray Struct)
        {
            m_StartIndex = 0;
            m_DataLen = 0;
            m_ElementSize = 0;
            Attach(Struct.m_pData, Struct.m_StartIndex, Struct.m_DataLen, false);
        }

        public bool Create(uint BufferLen)
        {
            Destory();
            m_StartIndex = 0;
            m_DataLen = BufferLen + sizeof(byte) + sizeof(uint);
            m_ElementSize = 0;
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
                    m_pData[m_StartIndex] = (byte)SMART_VALUE_TYPE.VT_ARRAY;
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
                if ((SMART_VALUE_TYPE)m_pData[StartIndex] != SMART_VALUE_TYPE.VT_ARRAY)
                {
                    Destory();
                    return false;
                }
            }
            CheckElements();
            return true;
        }
        public bool CloneFrom(CSmartArray Value)
        {
            if (Value.GetData() == null)
                return false;

            Destory();
            m_pData = new byte[Value.m_DataLen];
            m_StartIndex = 0;
            m_DataLen = Value.m_DataLen;
            m_ElementSize = Value.m_ElementSize;
            Array.Copy(Value.m_pData, Value.m_StartIndex, m_pData, m_StartIndex, m_DataLen);
            return true;
        }

        public void Destory()
        {
            m_pData = null;
            m_StartIndex = 0;
            m_DataLen = 0;
            m_ElementSize = 0;
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
        public int GetMemberCount()
        {
            return (int)GetLength();
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

        public uint GetArrayLength()
        {
            if ((m_pData == null) || (m_DataLen < sizeof(byte) + sizeof(uint)))
                return 0;
            uint ArrayDataStart = m_StartIndex + sizeof(byte) + sizeof(uint);
            uint ArrayDataLen = GetLength();
            if (ArrayDataLen + sizeof(byte) + sizeof(uint) > m_DataLen)
                ArrayDataLen = m_DataLen - sizeof(byte) - sizeof(uint);
            if (ArrayDataLen > 0)
            {
                if (m_ElementSize > 0)
                {
                    //定长元素
                    return ArrayDataLen / m_ElementSize;
                }
                else
                {
                    //变长元素
                    CSmartValue Value = new CSmartValue();
                    uint Count = 0;
                    uint HeadIndex = ArrayDataStart;
                    uint TailIndex = HeadIndex + ArrayDataLen;

                    while (HeadIndex + sizeof(byte) < TailIndex)
                    {
                        if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, SMART_VALUE_TYPE.VT_UNKNOWN))
                            break;
                        Count++;
                        HeadIndex += Value.GetDataLen();
                    }
                    return Count;
                }
            }
            return 0;
        }

        public SMART_VALUE_TYPE GetElementType()
        {
            if (m_ElementSize > 0 && GetLength() > 0)
            {
                return (SMART_VALUE_TYPE)m_pData[m_StartIndex + sizeof(byte) + sizeof(uint)];
            }
            return SMART_VALUE_TYPE.VT_UNKNOWN;
        }
        public uint GetElementSize()
        {
            return m_ElementSize;
        }
        public CSmartValue GetMember(uint Index)
        {
            CSmartValue Value = new CSmartValue();

            if (m_ElementSize > 0)
            {
                //定长元素
                uint ArrayLen = GetLength() / m_ElementSize;
                if (Index < ArrayLen)
                {

                    uint StartIndex = m_StartIndex + sizeof(byte) + sizeof(uint) + m_ElementSize * Index;
                    Value.Attach(m_pData, StartIndex, m_ElementSize, SMART_VALUE_TYPE.VT_UNKNOWN);
                    return Value;
                }
            }
            else
            {
                //变长元素
                uint HeadIndex = m_StartIndex + sizeof(byte) + sizeof(uint);
                uint TailIndex = HeadIndex + GetLength();

                while (HeadIndex + sizeof(byte) < TailIndex)
                {
                    if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, SMART_VALUE_TYPE.VT_UNKNOWN))
                        break;
                    if (Index == 0)
                        return Value;
                    HeadIndex += Value.GetDataLen();
                }
            }
            return Value;
        }
        public CSmartValue this[uint Index]
        {
            get
            {
                return GetMember(Index);
            }
            set
            {
                CSmartValue Element = GetMember(Index);
                if (Element.GetValueType() == Element.GetValueType())
                    Element.SetValue(value);
                else
                    throw new ElementTypeNotMatchException("ElementTypeNotMatch");
            }
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
        public CSmartValue GetNextMember(ref int Pos)
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

        public CSmartArray GetNextArrayMember(ref int Pos)
        {
            CSmartArray Value = new CSmartArray();

            if (m_pData == null)
            {
                Pos = 0;
                return Value;
            }


            if (Pos != 0)
            {
                uint HeadIndex = (uint)Pos;
                uint TailIndex = m_StartIndex + GetDataLen();

                if (HeadIndex >= TailIndex)
                {
                    Pos = 0;
                }
                else
                {
                    if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, false))
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

        public CSmartStruct GetNextStructMember(ref int Pos)
        {
            CSmartStruct Value = new CSmartStruct();

            if (m_pData == null)
            {
                Pos = 0;
                return Value;
            }


            if (Pos != 0)
            {
                uint HeadIndex = (uint)Pos;
                uint TailIndex = m_StartIndex + GetDataLen();

                if (HeadIndex >= TailIndex)
                {
                    Pos = 0;
                }
                else
                {
                    if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, false))
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

        public bool GetArray<T>(List<sbyte> Array)
        {
            if (GetLength() > 0)
            {
                if (m_ElementSize > 0)
                {
                    CSmartValue Value = new CSmartValue();

                    uint ArrayLen = GetLength() / m_ElementSize;
                    Array.Capacity = (int)ArrayLen;
                    for (uint i = 0; i < ArrayLen; i++)
                    {
                        if (!Value.Attach(m_pData, m_StartIndex + sizeof(byte) + sizeof(uint) + m_ElementSize * i, m_ElementSize, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                    }
                }
                else
                {
                    uint HeadIndex = m_StartIndex + sizeof(byte) + sizeof(uint);
                    uint TailIndex = HeadIndex + GetLength();
                    CSmartValue Value = new CSmartValue();

                    while (HeadIndex + sizeof(byte) < TailIndex)
                    {
                        if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                        HeadIndex += Value.GetDataLen();
                    }
                }
                return true;
            }
            else
            {
                return true;
            }
        }
        public bool GetArray(List<byte> Array)
        {
            if (GetLength() > 0)
            {
                if (m_ElementSize > 0)
                {
                    CSmartValue Value = new CSmartValue();

                    uint ArrayLen = GetLength() / m_ElementSize;
                    Array.Capacity = (int)ArrayLen;
                    for (uint i = 0; i < ArrayLen; i++)
                    {
                        if (!Value.Attach(m_pData, m_StartIndex + sizeof(byte) + sizeof(uint) + m_ElementSize * i, m_ElementSize, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                    }
                }
                else
                {
                    uint HeadIndex = m_StartIndex + sizeof(byte) + sizeof(uint);
                    uint TailIndex = HeadIndex + GetLength();
                    CSmartValue Value = new CSmartValue();

                    while (HeadIndex + sizeof(byte) < TailIndex)
                    {
                        if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                        HeadIndex += Value.GetDataLen();
                    }
                }
                return true;
            }
            else
            {
                return true;
            }
        }
        public bool GetArray(List<short> Array)
        {
            if (GetLength() > 0)
            {
                if (m_ElementSize > 0)
                {
                    CSmartValue Value = new CSmartValue();

                    uint ArrayLen = GetLength() / m_ElementSize;
                    Array.Capacity = (int)ArrayLen;
                    for (uint i = 0; i < ArrayLen; i++)
                    {
                        if (!Value.Attach(m_pData, m_StartIndex + sizeof(byte) + sizeof(uint) + m_ElementSize * i, m_ElementSize, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                    }
                    return true;
                }
                else
                {
                    uint HeadIndex = m_StartIndex + sizeof(byte) + sizeof(uint);
                    uint TailIndex = HeadIndex + GetLength();
                    CSmartValue Value = new CSmartValue();

                    while (HeadIndex + sizeof(byte) < TailIndex)
                    {
                        if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                        HeadIndex += Value.GetDataLen();
                    }
                }
                return true;
            }
            else
            {
                return true;
            }
        }
        public bool GetArray(List<ushort> Array)
        {
            if (GetLength() > 0)
            {
                if (m_ElementSize > 0)
                {
                    CSmartValue Value = new CSmartValue();

                    uint ArrayLen = GetLength() / m_ElementSize;
                    Array.Capacity = (int)ArrayLen;
                    for (uint i = 0; i < ArrayLen; i++)
                    {
                        if (!Value.Attach(m_pData, m_StartIndex + sizeof(byte) + sizeof(uint) + m_ElementSize * i, m_ElementSize, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                    }
                    return true;
                }
                else
                {
                    uint HeadIndex = m_StartIndex + sizeof(byte) + sizeof(uint);
                    uint TailIndex = HeadIndex + GetLength();
                    CSmartValue Value = new CSmartValue();

                    while (HeadIndex + sizeof(byte) < TailIndex)
                    {
                        if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                        HeadIndex += Value.GetDataLen();
                    }
                }
                return true;
            }
            else
            {
                return true;
            }
        }
        public bool GetArray(List<int> Array)
        {
            if (GetLength() > 0)
            {
                if (m_ElementSize > 0)
                {
                    CSmartValue Value = new CSmartValue();

                    uint ArrayLen = GetLength() / m_ElementSize;
                    Array.Capacity = (int)ArrayLen;
                    for (uint i = 0; i < ArrayLen; i++)
                    {
                        if (!Value.Attach(m_pData, m_StartIndex + sizeof(byte) + sizeof(uint) + m_ElementSize * i, m_ElementSize, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                    }
                    return true;
                }
                else
                {
                    uint HeadIndex = m_StartIndex + sizeof(byte) + sizeof(uint);
                    uint TailIndex = HeadIndex + GetLength();
                    CSmartValue Value = new CSmartValue();

                    while (HeadIndex + sizeof(byte) < TailIndex)
                    {
                        if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                        HeadIndex += Value.GetDataLen();
                    }
                }
                return true;
            }
            else
            {
                return true;
            }
        }
        public bool GetArray(List<uint> Array)
        {
            if (GetLength() > 0)
            {
                if (m_ElementSize > 0)
                {
                    CSmartValue Value = new CSmartValue();

                    uint ArrayLen = GetLength() / m_ElementSize;
                    Array.Capacity = (int)ArrayLen;
                    for (uint i = 0; i < ArrayLen; i++)
                    {
                        if (!Value.Attach(m_pData, m_StartIndex + sizeof(byte) + sizeof(uint) + m_ElementSize * i, m_ElementSize, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                    }
                    return true;
                }
                else
                {
                    uint HeadIndex = m_StartIndex + sizeof(byte) + sizeof(uint);
                    uint TailIndex = HeadIndex + GetLength();
                    CSmartValue Value = new CSmartValue();

                    while (HeadIndex + sizeof(byte) < TailIndex)
                    {
                        if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                        HeadIndex += Value.GetDataLen();
                    }
                }
                return true;
            }
            else
            {
                return true;
            }
        }
        public bool GetArray(List<long> Array)
        {
            if (GetLength() > 0)
            {
                if (m_ElementSize > 0)
                {
                    CSmartValue Value = new CSmartValue();

                    uint ArrayLen = GetLength() / m_ElementSize;
                    Array.Capacity = (int)ArrayLen;
                    for (uint i = 0; i < ArrayLen; i++)
                    {
                        if (!Value.Attach(m_pData, m_StartIndex + sizeof(byte) + sizeof(uint) + m_ElementSize * i, m_ElementSize, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                    }
                    return true;
                }
                else
                {
                    uint HeadIndex = m_StartIndex + sizeof(byte) + sizeof(uint);
                    uint TailIndex = HeadIndex + GetLength();
                    CSmartValue Value = new CSmartValue();

                    while (HeadIndex + sizeof(byte) < TailIndex)
                    {
                        if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                        HeadIndex += Value.GetDataLen();
                    }
                }
                return true;
            }
            else
            {
                return true;
            }
        }
        public bool GetArray(List<ulong> Array)
        {
            if (GetLength() > 0)
            {
                if (m_ElementSize > 0)
                {
                    CSmartValue Value = new CSmartValue();

                    uint ArrayLen = GetLength() / m_ElementSize;
                    Array.Capacity = (int)ArrayLen;
                    for (uint i = 0; i < ArrayLen; i++)
                    {
                        if (!Value.Attach(m_pData, m_StartIndex + sizeof(byte) + sizeof(uint) + m_ElementSize * i, m_ElementSize, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                    }
                    return true;
                }
                else
                {
                    uint HeadIndex = m_StartIndex + sizeof(byte) + sizeof(uint);
                    uint TailIndex = HeadIndex + GetLength();
                    CSmartValue Value = new CSmartValue();

                    while (HeadIndex + sizeof(byte) < TailIndex)
                    {
                        if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                        HeadIndex += Value.GetDataLen();
                    }
                }
                return true;
            }
            else
            {
                return true;
            }
        }
        public bool GetArray(List<float> Array)
        {
            if (GetLength() > 0)
            {
                if (m_ElementSize > 0)
                {
                    CSmartValue Value = new CSmartValue();

                    uint ArrayLen = GetLength() / m_ElementSize;
                    Array.Capacity = (int)ArrayLen;
                    for (uint i = 0; i < ArrayLen; i++)
                    {
                        if (!Value.Attach(m_pData, m_StartIndex + sizeof(byte) + sizeof(uint) + m_ElementSize * i, m_ElementSize, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                    }
                    return true;
                }
                else
                {
                    uint HeadIndex = m_StartIndex + sizeof(byte) + sizeof(uint);
                    uint TailIndex = HeadIndex + GetLength();
                    CSmartValue Value = new CSmartValue();

                    while (HeadIndex + sizeof(byte) < TailIndex)
                    {
                        if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                        HeadIndex += Value.GetDataLen();
                    }
                }
                return true;
            }
            else
            {
                return true;
            }
        }
        public bool GetArray(List<double> Array)
        {
            if (GetLength() > 0)
            {
                if (m_ElementSize > 0)
                {
                    CSmartValue Value = new CSmartValue();

                    uint ArrayLen = GetLength() / m_ElementSize;
                    Array.Capacity = (int)ArrayLen;
                    for (uint i = 0; i < ArrayLen; i++)
                    {
                        if (!Value.Attach(m_pData, m_StartIndex + sizeof(byte) + sizeof(uint) + m_ElementSize * i, m_ElementSize, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                    }
                    return true;
                }
                else
                {
                    uint HeadIndex = m_StartIndex + sizeof(byte) + sizeof(uint);
                    uint TailIndex = HeadIndex + GetLength();
                    CSmartValue Value = new CSmartValue();

                    while (HeadIndex + sizeof(byte) < TailIndex)
                    {
                        if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                        HeadIndex += Value.GetDataLen();
                    }
                }
                return true;
            }
            else
            {
                return true;
            }
        }
        public bool GetArray(List<bool> Array)
        {
            if (GetLength() > 0)
            {
                if (m_ElementSize > 0)
                {
                    CSmartValue Value = new CSmartValue();

                    uint ArrayLen = GetLength() / m_ElementSize;
                    Array.Capacity = (int)ArrayLen;
                    for (uint i = 0; i < ArrayLen; i++)
                    {
                        if (!Value.Attach(m_pData, m_StartIndex + sizeof(byte) + sizeof(uint) + m_ElementSize * i, m_ElementSize, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                    }
                }
                else
                {
                    uint HeadIndex = m_StartIndex + sizeof(byte) + sizeof(uint);
                    uint TailIndex = HeadIndex + GetLength();
                    CSmartValue Value = new CSmartValue();

                    while (HeadIndex + sizeof(byte) < TailIndex)
                    {
                        if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                        HeadIndex += Value.GetDataLen();
                    }
                }
                return true;
            }
            else
            {
                return true;
            }
        }

        public bool GetArray(List<string> Array)
        {
            if (GetLength() > 0)
            {
                uint HeadIndex = m_StartIndex + sizeof(byte) + sizeof(uint);
                uint TailIndex = HeadIndex + GetLength();
                CSmartValue Value = new CSmartValue();

                while (HeadIndex + sizeof(byte) < TailIndex)
                {
                    Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, SMART_VALUE_TYPE.VT_UNKNOWN);
                    Array.Add(Value);
                    HeadIndex += Value.GetDataLen();
                }
                return true;
            }
            else
            {
                return true;
            }
        }
        public bool GetArray(List<byte[]> Array)
        {
            if (GetLength() > 0)
            {
                uint HeadIndex = m_StartIndex + sizeof(byte) + sizeof(uint);
                uint TailIndex = HeadIndex + GetLength();
                CSmartValue Value = new CSmartValue();

                while (HeadIndex + sizeof(byte) < TailIndex)
                {
                    Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, SMART_VALUE_TYPE.VT_UNKNOWN);
                    Array.Add(Value);
                    HeadIndex += Value.GetDataLen();
                }
                return true;
            }
            else
            {
                return true;
            }
        }

        public bool GetArray(List<CSmartStruct> Array)
        {
            if (GetLength() > 0)
            {
                SMART_VALUE_TYPE ElementType = GetElementType();
                if (ElementType == SMART_VALUE_TYPE.VT_STRUCT)
                {
                    uint HeadIndex = m_StartIndex + sizeof(byte) + sizeof(uint);
                    uint TailIndex = HeadIndex + GetLength();
                    CSmartStruct Value = new CSmartValue();

                    while ((HeadIndex + sizeof(byte) < TailIndex) && (m_pData[HeadIndex] == (byte)ElementType))
                    {
                        Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, false);
                        Array.Add(Value);
                        HeadIndex += Value.GetDataLen();
                    }
                    return true;
                }
                return false;
            }
            else
            {
                return true;
            }
        }

        public bool GetArray(List<CVariedValue> Array)
        {
            if (GetLength() > 0)
            {
                if (m_ElementSize > 0)
                {
                    CSmartValue Value = new CSmartValue();

                    uint ArrayLen = GetLength() / m_ElementSize;
                    Array.Capacity = (int)ArrayLen;
                    for (uint i = 0; i < ArrayLen; i++)
                    {
                        if (!Value.Attach(m_pData, m_StartIndex + sizeof(byte) + sizeof(uint) + m_ElementSize * i, m_ElementSize, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                    }
                }
                else
                {
                    uint HeadIndex = m_StartIndex + sizeof(byte) + sizeof(uint);
                    uint TailIndex = HeadIndex + GetLength();
                    CSmartValue Value = new CSmartValue();

                    while (HeadIndex + sizeof(byte) < TailIndex)
                    {
                        if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, SMART_VALUE_TYPE.VT_UNKNOWN))
                            return false;
                        Array.Add(Value);
                        HeadIndex += Value.GetDataLen();
                    }
                }
                return true;
            }
            else
            {
                return true;
            }
        }

        CSmartValue PrepareMember(SMART_VALUE_TYPE Type)
        {
            CSmartValue SmartValue = new CSmartValue();
            uint StartIndex = m_StartIndex + sizeof(byte) + sizeof(uint) + GetLength();
            uint DataLen = m_DataLen - (StartIndex - m_StartIndex);
            if (SmartValue.Attach(m_pData, StartIndex, DataLen, Type))
            {
                return SmartValue;
            }
            return null;
        }

        public CSmartStruct PrepareSubStruct()
        {
            CSmartStruct SubStruct = null;

            if (m_pData != null && GetFreeLen() >= sizeof(byte) + sizeof(uint))
            {
                SubStruct = new CSmartStruct();
                uint StartIndex = m_StartIndex + sizeof(byte) + sizeof(uint) + GetLength();
                uint DataLen = m_DataLen - (StartIndex - m_StartIndex);
                if (!SubStruct.Attach(m_pData, StartIndex, DataLen, true))
                    return null;
            }

            return SubStruct;
        }
        public CSmartArray PrepareSubArray()
        {
            CSmartArray SubStruct = null;

            if (m_pData != null && GetFreeLen() >= sizeof(byte) + sizeof(uint))
            {
                SubStruct = new CSmartArray();
                uint StartIndex = m_StartIndex + sizeof(byte) + sizeof(uint) + GetLength();
                uint DataLen = m_DataLen - (StartIndex - m_StartIndex);
                if (!SubStruct.Attach(m_pData, StartIndex, DataLen, true))
                    return null;
            }

            return SubStruct;
        }
        public bool FinishMember(uint MemberSize)
        {
            if (m_pData == null)
                return false;

            if (GetFreeLen() < MemberSize)
            {
                return false;
            }
            uint StartIndex = m_StartIndex + sizeof(byte) + sizeof(uint) + GetLength();
            uint NewElementSize = GetElementSizeByType((SMART_VALUE_TYPE)m_pData[StartIndex]);
            if (GetLength() > 0)
            {
                //如果新元素和已有元素大小不同，ElementSize设置为0代表改数组不是规则数组	
                if (m_ElementSize != NewElementSize)
                    m_ElementSize = 0;
            }
            else
            {
                m_ElementSize = NewElementSize;
            }
            SetLength(GetLength() + MemberSize);
            return true;
        }
        public bool AddMemberNull()
        {
            CSmartValue SmartValue = PrepareMember(SMART_VALUE_TYPE.VT_NULL);
            if (SmartValue != null)
            {
                return FinishMember(SmartValue.GetDataLen());
            }
            return false;
        }
        public bool AddMember<T>(T Value) where T : IConvertible
        {
            CSmartValue SmartValue = null;

            if (Value is sbyte)
                SmartValue = PrepareMember(SMART_VALUE_TYPE.VT_CHAR);
            else if (Value is byte)
                SmartValue = PrepareMember(SMART_VALUE_TYPE.VT_UCHAR);
            else if (Value is short)
                SmartValue = PrepareMember(SMART_VALUE_TYPE.VT_SHORT);
            else if (Value is ushort)
                SmartValue = PrepareMember(SMART_VALUE_TYPE.VT_USHORT);
            else if (Value is int)
                SmartValue = PrepareMember(SMART_VALUE_TYPE.VT_INT);
            else if (Value is uint)
                SmartValue = PrepareMember(SMART_VALUE_TYPE.VT_UINT);
            else if (Value is long)
                SmartValue = PrepareMember(SMART_VALUE_TYPE.VT_BIGINT);
            else if (Value is ulong)
                SmartValue = PrepareMember(SMART_VALUE_TYPE.VT_UBIGINT);
            else if (Value is float)
                SmartValue = PrepareMember(SMART_VALUE_TYPE.VT_FLOAT);
            else if (Value is double)
                SmartValue = PrepareMember(SMART_VALUE_TYPE.VT_DOUBLE);
            else if (Value is bool)
                SmartValue = PrepareMember(SMART_VALUE_TYPE.VT_BOOL);

            if (SmartValue != null)
            {
                if (SmartValue.SetValue(Value))
                    return FinishMember(SmartValue.GetDataLen());
            }
            return false;
        }


        public bool AddMember(string Value)
        {
            CSmartValue SmartValue = null;
            switch (CSmartValue.INTERNAL_STRING_CODE_PAGE)
            {
                case STRING_CODE_PAGE.STRING_CODE_PAGE_ANSI:
                    SmartValue = PrepareMember(SMART_VALUE_TYPE.VT_STRING_ANSI);
                    break;
                case STRING_CODE_PAGE.STRING_CODE_PAGE_UTF8:
                    SmartValue = PrepareMember(SMART_VALUE_TYPE.VT_STRING_UTF8);
                    break;
                case STRING_CODE_PAGE.STRING_CODE_PAGE_UCS16:
                    SmartValue = PrepareMember(SMART_VALUE_TYPE.VT_STRING_UCS16);
                    break;
            }

            if (SmartValue != null)
            {
                if (SmartValue.SetValue(Value))
                    return FinishMember(SmartValue.GetDataLen());
            }
            return false;
        }
        public bool AddMember(byte[] Value)
        {
            CSmartValue SmartValue = PrepareMember(SMART_VALUE_TYPE.VT_BINARY);
            if (SmartValue != null)
            {
                if (SmartValue.SetValue(Value))
                    return FinishMember(SmartValue.GetDataLen());
            }
            return false;
        }

        public bool AddMember(CSmartArray Value)
        {
            CSmartValue SmartValue = PrepareMember(SMART_VALUE_TYPE.VT_ARRAY);
            if (SmartValue != null)
            {
                if (SmartValue.SetValue(Value))
                    return FinishMember(SmartValue.GetDataLen());
            }
            return false;
        }

        public bool AddMember(CSmartStruct Value)
        {
            CSmartValue SmartValue = PrepareMember(SMART_VALUE_TYPE.VT_STRUCT);
            if (SmartValue != null)
            {
                if (SmartValue.SetValue(Value))
                    return FinishMember(SmartValue.GetDataLen());
            }
            return false;
        }
        public bool AddMember(CSmartValue Value)
        {
            CSmartValue SmartValue = PrepareMember(Value.GetValueType());
            if (SmartValue != null)
            {
                if (SmartValue.SetValue(Value))
                    return FinishMember(SmartValue.GetDataLen());
            }
            return false;
        }
        public bool AddMember(CVariedValue Value)
        {
            switch (Value.Type)
            {
                case VARIED_VALUE_TYPE.NIL:
                    return AddMemberNull();
                case VARIED_VALUE_TYPE.BOOLEAN:
                    return AddMember((bool)Value);
                case VARIED_VALUE_TYPE.INT8:
                    return AddMember((sbyte)Value);
                case VARIED_VALUE_TYPE.UINT8:
                    return AddMember((byte)Value);
                case VARIED_VALUE_TYPE.INT16:
                    return AddMember((short)Value);
                case VARIED_VALUE_TYPE.UINT16:
                    return AddMember((ushort)Value);
                case VARIED_VALUE_TYPE.INT32:
                    return AddMember((int)Value);
                case VARIED_VALUE_TYPE.UINT32:
                    return AddMember((uint)Value);
                case VARIED_VALUE_TYPE.INT64:
                    return AddMember((long)Value);
                case VARIED_VALUE_TYPE.UINT64:
                    return AddMember((ulong)Value);
                case VARIED_VALUE_TYPE.FLOAT32:
                    return AddMember((float)Value);
                case VARIED_VALUE_TYPE.FLOAT64:
                    return AddMember((double)Value);
                case VARIED_VALUE_TYPE.STRING:
                    return AddMember((string)Value);
                case VARIED_VALUE_TYPE.BINARY:
                    return AddMember((byte[])Value);
                case VARIED_VALUE_TYPE.ARRAY:
                case VARIED_VALUE_TYPE.TABLE:
                    return AddMember((CSmartValue)Value);
            }
            return false;
        }

        public bool AddArray<T>(List<T> Array) where T : IConvertible
        {
            if (Array.Count > 0)
            {
                SMART_VALUE_TYPE ArrayType = SMART_VALUE_TYPE.VT_UNKNOWN;
                uint ArrayElementSize = 0;
                if (Array[0] is sbyte)
                {
                    ArrayType = SMART_VALUE_TYPE.VT_CHAR;
                    ArrayElementSize = sizeof(byte) + sizeof(sbyte);
                }
                else if (Array[0] is byte)
                {
                    ArrayType = SMART_VALUE_TYPE.VT_UCHAR;
                    ArrayElementSize = sizeof(byte) + sizeof(byte);
                }
                else if (Array[0] is short)
                {
                    ArrayType = SMART_VALUE_TYPE.VT_SHORT;
                    ArrayElementSize = sizeof(byte) + sizeof(short);
                }
                else if (Array[0] is ushort)
                {
                    ArrayType = SMART_VALUE_TYPE.VT_USHORT;
                    ArrayElementSize = sizeof(byte) + sizeof(ushort);
                }
                else if (Array[0] is int)
                {
                    ArrayType = SMART_VALUE_TYPE.VT_INT;
                    ArrayElementSize = sizeof(byte) + sizeof(int);
                }
                else if (Array[0] is uint)
                {
                    ArrayType = SMART_VALUE_TYPE.VT_UINT;
                    ArrayElementSize = sizeof(byte) + sizeof(uint);
                }
                else if (Array[0] is long)
                {
                    ArrayType = SMART_VALUE_TYPE.VT_BIGINT;
                    ArrayElementSize = sizeof(byte) + sizeof(long);
                }
                else if (Array[0] is ulong)
                {
                    ArrayType = SMART_VALUE_TYPE.VT_UBIGINT;
                    ArrayElementSize = sizeof(byte) + sizeof(ulong);
                }
                else if (Array[0] is float)
                {
                    ArrayType = SMART_VALUE_TYPE.VT_FLOAT;
                    ArrayElementSize = sizeof(byte) + sizeof(float);
                }
                else if (Array[0] is double)
                {
                    ArrayType = SMART_VALUE_TYPE.VT_DOUBLE;
                    ArrayElementSize = sizeof(byte) + sizeof(double);
                }
                else if (Array[0] is bool)
                {
                    ArrayType = SMART_VALUE_TYPE.VT_BOOL;
                    ArrayElementSize = sizeof(byte) + sizeof(bool);
                }
                else
                {
                    return false;
                }
                uint NeedSize = ArrayElementSize * (uint)Array.Count;
                if (GetFreeLen() >= NeedSize)
                {
                    uint StartIndex = m_StartIndex + sizeof(byte) + sizeof(uint) + GetLength();
                    CSmartValue Element = new CSmartValue();
                    foreach (T Value in Array)
                    {
                        if (Element.Attach(m_pData, StartIndex, ArrayElementSize, ArrayType))
                            Element.SetValue(Value);
                        else
                            return false;
                        StartIndex += ArrayElementSize;
                    }
                    return FinishMember(NeedSize);
                }
            }
            return false;
        }
        public bool AddArray(List<string> Array)
        {
            if (Array.Count > 0)
            {
                uint NeedSize = 0;
                foreach (string Value in Array)
                {
                    NeedSize += GetStringMemberSize(Value);
                }
                if (GetFreeLen() >= NeedSize)
                {
                    foreach (string Value in Array)
                    {
                        if (!AddMember(Value))
                            return false;
                    }
                    return true;
                }
            }
            return false;
        }
        public bool AddArray(List<byte[]> Array)
        {
            if (Array.Count > 0)
            {
                uint NeedSize = 0;
                foreach (byte[] Value in Array)
                {
                    NeedSize += GetBinaryMemberSize((uint)Value.Length);
                }
                if (GetFreeLen() >= NeedSize)
                {
                    foreach (byte[] Value in Array)
                    {
                        if (!AddMember(Value))
                            return false;
                    }
                    return true;
                }
            }
            return false;
        }
        public bool AddArray(List<CSmartStruct> Array)
        {
            if (Array.Count > 0)
            {
                uint NeedSize = 0;
                foreach (CSmartStruct Value in Array)
                {
                    NeedSize += GetStructMemberSize(Value.GetDataLen());
                }
                if (GetFreeLen() >= NeedSize)
                {
                    foreach (CSmartStruct Value in Array)
                    {
                        if (!AddMember(Value))
                            return false;
                    }
                    return true;
                }
            }
            return false;
        }
        public bool AddArray(List<CVariedValue> Array)
        {
            if (Array.Count > 0)
            {
                uint NeedSize = 0;
                foreach (CVariedValue Value in Array)
                {
                    NeedSize += GetVariedMemberSize(Value);
                }
                if (GetFreeLen() >= NeedSize)
                {
                    foreach (CVariedValue Value in Array)
                    {
                        if (!AddMember(Value))
                            return false;
                    }
                    return true;
                }
            }
            return false;
        }
        public static uint GetFixMemberSize(uint TypeLen)
        {
            return sizeof(byte) + TypeLen;
        }
        public static uint GetArrayMemberSize(uint ArraySize)
        {
            return sizeof(byte) + sizeof(uint) + ArraySize;
        }
        public static uint GetStructMemberSize(uint StructSize)
        {
            return sizeof(byte) + sizeof(uint) + StructSize;
        }
        public static uint GetBinaryMemberSize(uint Len)
        {
            return sizeof(byte) + sizeof(uint) + sizeof(byte) * (Len);
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
            return sizeof(byte) + sizeof(uint) + DataLen;
        }
        public static uint GetVariedMemberSize(CVariedValue Value)
        {
            switch (Value.Type)
            {
                case VARIED_VALUE_TYPE.NIL:
                    return sizeof(byte);
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
                    return GetStringMemberSize((string)Value);
            }
            return sizeof(byte);
        }

        protected void CheckElements()
        {
            m_ElementSize = 0;
            int Pos = GetFirstMemberPosition();
            while (Pos > 0)
            {
                CSmartValue Value = GetNextMember(ref Pos);
                uint Size = GetElementSizeByType(Value.GetValueType());

                if (Size > 0)
                {
                    if (m_ElementSize == 0)
                    {
                        m_ElementSize = Size;
                    }
                    else if (m_ElementSize != Size)
                    {
                        m_ElementSize = 0;
                        break;
                    }
                }
                else
                {
                    m_ElementSize = 0;
                    break;
                }
            }
        }
        protected uint GetElementSizeByType(SMART_VALUE_TYPE Type)
        {
            switch (Type)
            {
                case SMART_VALUE_TYPE.VT_NULL:
                    return sizeof(byte);
                case SMART_VALUE_TYPE.VT_CHAR:
                case SMART_VALUE_TYPE.VT_UCHAR:
                    return sizeof(byte) + sizeof(char);
                case SMART_VALUE_TYPE.VT_SHORT:
                case SMART_VALUE_TYPE.VT_USHORT:
                    return sizeof(byte) + sizeof(short);
                case SMART_VALUE_TYPE.VT_INT:
                case SMART_VALUE_TYPE.VT_UINT:
                    return sizeof(byte) + sizeof(int);
                case SMART_VALUE_TYPE.VT_BIGINT:
                case SMART_VALUE_TYPE.VT_UBIGINT:
                    return sizeof(byte) + sizeof(long);
                case SMART_VALUE_TYPE.VT_FLOAT:
                    return sizeof(byte) + sizeof(float);
                case SMART_VALUE_TYPE.VT_DOUBLE:
                    return sizeof(byte) + sizeof(double);
                case SMART_VALUE_TYPE.VT_BOOL:
                    return sizeof(byte) + sizeof(byte);
            }
            return 0;
        }
        public object Clone()
        {
            CSmartArray Array = new CSmartArray();
            Array.CloneFrom(this);
            return Array;
        }
    }
}
