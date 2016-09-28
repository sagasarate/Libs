using System;
using System.Collections.Generic;
using System.Text;

namespace DOSSystem
{
    public class CSmartStruct
    {
        byte[]      m_pData;
        uint        m_StartIndex;
        uint        m_DataLen;
        const uint HEAD_SIZE=(sizeof(byte)+sizeof(uint));
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
            Attach(Struct.m_pData, Struct.m_StartIndex, Struct.m_DataLen,false);
        }       

	    public bool Create(uint BufferLen)
	    {
		    Destory();
            m_StartIndex = 0;
            m_DataLen = BufferLen+sizeof(byte)+sizeof(uint);
            m_pData = new byte[m_DataLen];
            m_pData[m_StartIndex] = (byte)CSmartValue.SMART_VALUE_TYPE.VT_STRUCT;
            SetLength(0);
		    return true;
	    }

	    public bool Attach(byte[] pData,uint StartIndex,uint DataLen,bool IsEmpty)
        {
            if(pData==null)
		        return false;
            if (StartIndex + DataLen > pData.Length)
                return false;
            
	        Destory();
           
	        m_pData=pData;
            m_StartIndex = StartIndex;
            m_DataLen = DataLen;
            if(IsEmpty)
	        {
                if (DataLen >= sizeof(byte) + sizeof(uint))
                {
                    m_pData[m_StartIndex] = (byte)CSmartValue.SMART_VALUE_TYPE.VT_STRUCT;
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
                if ((CSmartValue.SMART_VALUE_TYPE)m_pData[StartIndex] != CSmartValue.SMART_VALUE_TYPE.VT_STRUCT)
		        {
			        Destory();
			        return false;
		        }
	        }
	        return true;
        }
        public bool CloneFrom(CSmartStruct Value)
	    {
		    if(Value.GetData()==null)
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
		    m_pData=null;
            m_StartIndex = 0;
            m_DataLen = 0;
	    }

	    public void Clear()
	    {
            SetLength(0);
	    }

	    public uint GetLength()
	    {
		    if(m_pData==null)
			    return 0;
            return BitConverter.ToUInt32(m_pData,(int)m_StartIndex+1);
	    }
        void SetLength(uint Len)
        {
            Array.Copy(BitConverter.GetBytes(Len), 0, m_pData, m_StartIndex+1, sizeof(uint));
        }

	    public uint GetDataLen()
	    {
		    if(m_pData==null)
			    return 0;
		    uint DataLen=GetLength()+sizeof(byte)+sizeof(uint);
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
		    if(m_pData==null)
			    return -1;
		    if(GetFreeLen()<sizeof(ushort))
		    {
			    nUsableSize=0;
			    return -1;
		    }
		    nUsableSize=GetFreeLen()-sizeof(ushort);
		    return (int)m_StartIndex+sizeof(byte)+sizeof(uint)+(int)GetLength()+sizeof(ushort);
	    }
	    public bool FinishMember(ushort ID,uint MemberSize)
	    {
		    if(m_pData==null)
			    return false;
		    if(ID==0)
			    return false;
		    uint NeedSize=sizeof(short)+MemberSize;
		    if(GetFreeLen()<NeedSize)
		    {
			    return false;
		    }
		    uint StartIndex=m_StartIndex+sizeof(byte)+sizeof(uint)+GetLength();
            Array.Copy(BitConverter.GetBytes(ID), 0, m_pData, StartIndex, sizeof(ushort));
            SetLength(GetLength() + NeedSize);
		    return true;
	    }

        public bool AddMember(ushort ID, sbyte Value)
        {
            uint BufferSize = 0;
            int StartIndex = PrepareMember(ref BufferSize);
            if (StartIndex >= 0)
            {
                CSmartValue SmartValue = new CSmartValue();
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, CSmartValue.SMART_VALUE_TYPE.VT_CHAR))
                {
                   if(SmartValue.SetValue(Value))
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
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, CSmartValue.SMART_VALUE_TYPE.VT_UCHAR))
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
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, CSmartValue.SMART_VALUE_TYPE.VT_SHORT))
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
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, CSmartValue.SMART_VALUE_TYPE.VT_USHORT))
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
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, CSmartValue.SMART_VALUE_TYPE.VT_INT))
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
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, CSmartValue.SMART_VALUE_TYPE.VT_UINT))
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
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, CSmartValue.SMART_VALUE_TYPE.VT_BIGINT))
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
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, CSmartValue.SMART_VALUE_TYPE.VT_UBIGINT))
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
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, CSmartValue.SMART_VALUE_TYPE.VT_FLOAT))
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
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, CSmartValue.SMART_VALUE_TYPE.VT_DOUBLE))
                {
                    if (SmartValue.SetValue(Value))
                        return FinishMember(ID, SmartValue.GetDataLen());
                }
            }
            return false;
        }
        public bool AddMember(ushort ID,bool Value)
	    {
		    uint BufferSize=0;
		    int StartIndex = PrepareMember(ref BufferSize);
		    if (StartIndex >= 0)
		    {
			   CSmartValue SmartValue = new CSmartValue();
               if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, CSmartValue.SMART_VALUE_TYPE.VT_BOOL))
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

                
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, CSmartValue.SMART_VALUE_TYPE.VT_STRING))                
                //if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, CSmartValue.SMART_VALUE_TYPE.VT_USTRING))
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
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, CSmartValue.SMART_VALUE_TYPE.VT_STRING))
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
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, CSmartValue.SMART_VALUE_TYPE.VT_STRUCT))
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
                if (SmartValue.Attach(m_pData, (uint)StartIndex, BufferSize, CSmartValue.SMART_VALUE_TYPE.VT_STRUCT))
                {
                    if (SmartValue.SetValue(Value))
                        return FinishMember(ID, SmartValue.GetDataLen());
                }
            }
            return false;
        }
        public int GetMemberCount()
	    {
		    if(m_pData==null)
			    return 0;
		    uint HeadIndex=m_StartIndex;
		    uint TailIndex=m_StartIndex+GetDataLen();
		    int MemberCount=0;
		    HeadIndex+=sizeof(byte)+sizeof(uint);
		    while(HeadIndex+sizeof(ushort)<TailIndex)
		    {
			    HeadIndex+=sizeof(ushort);

			    CSmartValue Value=new CSmartValue(m_pData,HeadIndex,TailIndex-HeadIndex,CSmartValue.SMART_VALUE_TYPE.VT_UNKNOWN);
                HeadIndex += Value.GetDataLen();
			    MemberCount++;
		    }
		    return MemberCount;
	    }
        public CSmartValue GetMemberByIndex(uint Index)
	    {
		    CSmartValue Value=new CSmartValue();

		    if(m_pData==null)
			    return Value;
		    uint HeadIndex=m_StartIndex;
		    uint TailIndex=m_StartIndex+GetDataLen();

		    HeadIndex+=sizeof(byte)+sizeof(uint);
		    while(HeadIndex+sizeof(ushort)<TailIndex)
		    {
			    HeadIndex+=sizeof(ushort);
                if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, CSmartValue.SMART_VALUE_TYPE.VT_UNKNOWN))
				    break;
                HeadIndex += Value.GetDataLen();
			    if(Index==0)
				    return Value;
			    Index--;
		    }
		    Value.Destory();
		    return Value;

	    }
        public CSmartValue GetMember(ushort ID)
	    {
		    CSmartValue Value=new CSmartValue();

		    if(m_pData==null)
			    return Value;
		    uint HeadIndex=m_StartIndex;
		    uint TailIndex=m_StartIndex+GetDataLen();

		    HeadIndex+=sizeof(byte)+sizeof(uint);
		    while(HeadIndex+sizeof(ushort)<TailIndex)
		    {
			    ushort MemberID=BitConverter.ToUInt16(m_pData,(int)HeadIndex);
			    HeadIndex+=sizeof(ushort);
                if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, CSmartValue.SMART_VALUE_TYPE.VT_UNKNOWN))
				    break;
			    if(MemberID==ID)
				    return Value;
                HeadIndex += Value.GetDataLen();
		    }
		    Value.Destory();
		    return Value;
	    }
        public bool IsMemberExist(ushort ID)
	    {
		    if(IDToIndex(ID)==INVALID_MEMBER_ID)
		    {
			    return false;
		    }
		    return true;
	    }
	    public uint IDToIndex(ushort ID)
	    {
		    CSmartValue Value=new CSmartValue();

		    if(m_pData==null)
			    return INVALID_MEMBER_ID;

		    uint HeadIndex=m_StartIndex;
		    uint TailIndex=m_StartIndex+GetDataLen();

		    uint Index=0;

		    HeadIndex+=sizeof(byte)+sizeof(uint);
		    while(HeadIndex+sizeof(ushort)<TailIndex)
		    {
			    ushort MemberID=BitConverter.ToUInt16(m_pData,(int)HeadIndex);
			    HeadIndex+=sizeof(ushort);
			    if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, CSmartValue.SMART_VALUE_TYPE.VT_UNKNOWN))
				    break;
			    if(MemberID==ID)
				    return Index;
			    HeadIndex += Value.GetDataLen();
			    Index++;
		    }
		    return INVALID_MEMBER_ID;
	    }
	    public ushort IndexToID(uint Index)
	    {
		    CSmartValue Value=new CSmartValue();

		    if(m_pData==null)
			    return 0;

		    uint HeadIndex=m_StartIndex;
		    uint TailIndex=m_StartIndex+GetDataLen();
		    ushort MemberID=0;

		    HeadIndex+=sizeof(byte)+sizeof(uint);
		    while(HeadIndex+sizeof(ushort)<TailIndex)
		    {
			    MemberID=BitConverter.ToUInt16(m_pData,(int)HeadIndex);
			    if(Index==0)
				    return MemberID;
                HeadIndex += sizeof(ushort);
                if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, CSmartValue.SMART_VALUE_TYPE.VT_UNKNOWN))
                    break;
                HeadIndex += Value.GetDataLen();
			    Index--;
		    }
		    return 0;
	    }
        public int GetFirstMemberPosition()
	    {
		    if(m_pData==null)
			    return 0;


		    uint HeadIndex=m_StartIndex;
		    uint TailIndex=m_StartIndex+GetDataLen();
		    HeadIndex+=sizeof(byte)+sizeof(uint);

		    if(HeadIndex<TailIndex)
			    return (int)HeadIndex;
		    else
			    return 0;
	    }
        public CSmartValue GetNextMember(ref int Pos, ref ushort MemberID)
	    {
		    CSmartValue Value=new CSmartValue();

		    if(m_pData==null)
		    {
			    Pos=0;
			    return Value;
		    }


		    if(Pos!=0)
		    {
			    uint HeadIndex=(uint)Pos;
			    uint TailIndex=m_StartIndex+GetDataLen();
			    MemberID=BitConverter.ToUInt16(m_pData,(int)HeadIndex);

			    HeadIndex+=sizeof(ushort);

			    if(HeadIndex>=TailIndex)
			    {
				    Pos=0;
			    }
			    else
			    {
                    if (!Value.Attach(m_pData, HeadIndex, TailIndex - HeadIndex, CSmartValue.SMART_VALUE_TYPE.VT_UNKNOWN))
				    {
					    Pos=0;
					    return Value;
				    }
                    Pos = (int)(HeadIndex + Value.GetDataLen());
                    if (Pos >= (int)TailIndex)
				    {
					    Pos=0;
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
        public static uint GetStringMemberSize(uint StrLen)
        {
            return sizeof(ushort) + sizeof(byte) + sizeof(uint) + sizeof(char) * (StrLen + 1);
        }
        public static uint GetStringMemberSizeUTF8(string Str)
        {
            uint StrLen = 0;
            if (!string.IsNullOrEmpty(Str))
            {
                StrLen = (uint)Encoding.UTF8.GetByteCount(Str);
            }            
            return sizeof(ushort) + sizeof(byte) + sizeof(uint) + StrLen + 1;
        }
    }
}
