using System;
using System.Collections.Generic;
using System.Text;


namespace DOSSystem
{
    public enum SST_PAIR
    {
        KEY = 1,
	    DATA
    };

    public enum SMART_VALUE_TYPE
    {
        VT_NULL,
        VT_CHAR,
        VT_UCHAR,
        VT_SHORT,
        VT_USHORT,
        VT_INT,
        VT_UINT,
        VT_BIGINT,
        VT_UBIGINT,
        VT_FLOAT,
        VT_DOUBLE,
        VT_STRING_UTF8,
        VT_STRING_UCS16,
        VT_STRUCT,
        VT_STRING_ANSI,
        VT_BINARY,
        VT_ARRAY,
        VT_BOOL,
        VT_UNKNOWN,
    };
    public enum STRING_CODE_PAGE
    {
        STRING_CODE_PAGE_ANSI,
        STRING_CODE_PAGE_UTF8,
        STRING_CODE_PAGE_UCS16,
    };
    public class CSmartValue : ICloneable
    {

        static byte[] NULL_DATA = { (byte)SMART_VALUE_TYPE.VT_NULL };
        public static STRING_CODE_PAGE INTERNAL_STRING_CODE_PAGE = STRING_CODE_PAGE.STRING_CODE_PAGE_UCS16;

        byte[] m_pData = NULL_DATA;
        uint m_StartIndex = 0;
        uint m_DataLen = (uint)NULL_DATA.Length;

        public CSmartValue(byte[] pData, uint StartIndex, uint DataLen, SMART_VALUE_TYPE ClearType)
	    {
            m_pData = NULL_DATA;
            m_StartIndex = 0;
            m_DataLen = (uint)NULL_DATA.Length;
            Attach(pData,StartIndex,DataLen,ClearType);
	    }

        public CSmartValue(SMART_VALUE_TYPE Type = SMART_VALUE_TYPE.VT_NULL, uint Len = 0)
        {
            m_pData = NULL_DATA;
            m_StartIndex = 0;
            m_DataLen = (uint)NULL_DATA.Length;
            Create(Type, Len);
        }

        public bool Create(SMART_VALUE_TYPE Type, uint Len)
	    {
		    Destory();
            switch (Type)
            {
                case SMART_VALUE_TYPE.VT_NULL:
                    m_pData = NULL_DATA;
                    m_StartIndex = 0;
                    m_DataLen = (uint)NULL_DATA.Length;
                    break;
                case SMART_VALUE_TYPE.VT_CHAR:
                case SMART_VALUE_TYPE.VT_UCHAR:
                    m_DataLen = sizeof(byte) + sizeof(byte);
                    m_pData = new byte[m_DataLen];
                    m_pData[0] = (byte)Type;
                    break;
                case SMART_VALUE_TYPE.VT_SHORT:
                case SMART_VALUE_TYPE.VT_USHORT:
                    m_DataLen = sizeof(short) + sizeof(byte);
                    m_pData = new byte[m_DataLen];
                    m_pData[0] = (byte)Type;
                    break;
                case SMART_VALUE_TYPE.VT_INT:
                case SMART_VALUE_TYPE.VT_UINT:
                    m_DataLen = sizeof(int) + sizeof(byte);
                    m_pData = new byte[m_DataLen];
                    m_pData[0] = (byte)Type;
                    break;
                case SMART_VALUE_TYPE.VT_BIGINT:
                case SMART_VALUE_TYPE.VT_UBIGINT:
                    m_DataLen = sizeof(long) + sizeof(byte);
                    m_pData = new byte[m_DataLen];
                    m_pData[0] = (byte)Type;
                    break;
                case SMART_VALUE_TYPE.VT_FLOAT:
                    m_DataLen = sizeof(float) + sizeof(byte);
                    m_pData = new byte[m_DataLen];
                    m_pData[0] = (byte)Type;
                    break;
                case SMART_VALUE_TYPE.VT_DOUBLE:
                    m_DataLen = sizeof(double) + sizeof(byte);
                    m_pData = new byte[m_DataLen];
                    m_pData[0] = (byte)Type;
                    break;
                case SMART_VALUE_TYPE.VT_STRING_UTF8:
                case SMART_VALUE_TYPE.VT_STRING_ANSI:
                    m_DataLen = Len + sizeof(byte) + sizeof(byte) + sizeof(uint);
                    m_pData = new byte[m_DataLen];
                    m_pData[0] = (byte)Type;
                    break;
                case SMART_VALUE_TYPE.VT_STRING_UCS16:
                    m_DataLen = Len + sizeof(char) + sizeof(byte) + sizeof(uint);
                    m_pData = new byte[m_DataLen];
                    m_pData[0] = (byte)Type;
                    break;
                case SMART_VALUE_TYPE.VT_STRUCT:
                case SMART_VALUE_TYPE.VT_BINARY:
                case SMART_VALUE_TYPE.VT_ARRAY:
                    m_DataLen = Len + sizeof(byte) + sizeof(uint);
                    m_pData = new byte[m_DataLen];
                    m_pData[0] = (byte)Type;
                    break;
                case SMART_VALUE_TYPE.VT_BOOL:
                    m_DataLen = sizeof(byte) + sizeof(byte);
                    m_pData = new byte[m_DataLen];
                    m_pData[0] = (byte)Type;
                    break;
                default:
                    return false;
            }

		    return true;
	    }

        public bool Attach(byte[] pData, uint StartIndex, uint DataLen, SMART_VALUE_TYPE ClearType)
	    {
		    Destory();
		    m_pData=pData;
            m_StartIndex = StartIndex;

            SMART_VALUE_TYPE DataType = ClearType;
		    uint RealDataLen=0;

            if (ClearType == SMART_VALUE_TYPE.VT_UNKNOWN)
		    {
			    DataType=GetValueType();
			    RealDataLen=GetLength();			
		    }

            switch (DataType)
            {
                case SMART_VALUE_TYPE.VT_NULL:
                    m_DataLen = (uint)NULL_DATA.Length;
                    break;
                case SMART_VALUE_TYPE.VT_CHAR:
                case SMART_VALUE_TYPE.VT_UCHAR:
                    m_DataLen = sizeof(byte) + sizeof(byte);
                    break;
                case SMART_VALUE_TYPE.VT_SHORT:
                case SMART_VALUE_TYPE.VT_USHORT:
                    m_DataLen = sizeof(short) + sizeof(byte);
                    break;
                case SMART_VALUE_TYPE.VT_INT:
                case SMART_VALUE_TYPE.VT_UINT:
                    m_DataLen = sizeof(int) + sizeof(byte);
                    break;
                case SMART_VALUE_TYPE.VT_BIGINT:
                case SMART_VALUE_TYPE.VT_UBIGINT:
                    m_DataLen = sizeof(long) + sizeof(byte);
                    break;
                case SMART_VALUE_TYPE.VT_FLOAT:
                    m_DataLen = sizeof(float) + sizeof(byte);
                    break;
                case SMART_VALUE_TYPE.VT_DOUBLE:
                    m_DataLen = sizeof(double) + sizeof(byte);
                    break;
                case SMART_VALUE_TYPE.VT_STRING_UTF8:
                case SMART_VALUE_TYPE.VT_STRING_ANSI:
                    m_DataLen = RealDataLen + sizeof(byte) + sizeof(uint) + sizeof(byte);
                    break;
                case SMART_VALUE_TYPE.VT_STRING_UCS16:
                    m_DataLen = RealDataLen + sizeof(byte) + sizeof(uint) + sizeof(char);
                    break;
                case SMART_VALUE_TYPE.VT_STRUCT:
                case SMART_VALUE_TYPE.VT_BINARY:
                case SMART_VALUE_TYPE.VT_ARRAY:
                    m_DataLen = RealDataLen + sizeof(byte) + sizeof(uint);
                    break;
                case SMART_VALUE_TYPE.VT_BOOL:
                    m_DataLen = sizeof(byte) + sizeof(byte);
                    break;
                default:
                    return false;
            }

		    if(DataLen<m_DataLen)
		    {
			    Destory();
			    return false;
		    }

		    m_DataLen=DataLen;

            if (ClearType == SMART_VALUE_TYPE.VT_UNKNOWN)
		    {
			    switch(DataType)
			    {
                case SMART_VALUE_TYPE.VT_STRING_UTF8:
                case SMART_VALUE_TYPE.VT_STRING_ANSI:
                    m_pData[m_StartIndex + RealDataLen + sizeof(byte) + sizeof(uint)] = 0;
				    break;
                case SMART_VALUE_TYPE.VT_STRING_UCS16:
                    m_pData[m_StartIndex + RealDataLen + sizeof(byte) + sizeof(uint)] = 0;
                    m_pData[m_StartIndex + RealDataLen + sizeof(byte) + sizeof(uint) + 1] = 0;
                    break;               
			    }
		    }
		    else
		    {
			    switch(ClearType)
			    {
                case SMART_VALUE_TYPE.VT_NULL:
                    m_pData[m_StartIndex] = (byte)ClearType;
                    break;
                case SMART_VALUE_TYPE.VT_CHAR:
                case SMART_VALUE_TYPE.VT_UCHAR:
                    m_pData[m_StartIndex] = (byte)ClearType;
                    Array.Clear(m_pData, (int)m_StartIndex + sizeof(byte), sizeof(byte));
				    break;
                case SMART_VALUE_TYPE.VT_SHORT:
                case SMART_VALUE_TYPE.VT_USHORT:
                    m_pData[m_StartIndex] = (byte)ClearType;
                    Array.Clear(m_pData, (int)m_StartIndex + sizeof(byte), sizeof(short));
				    break;
                case SMART_VALUE_TYPE.VT_INT:
                case SMART_VALUE_TYPE.VT_UINT:
                    m_pData[m_StartIndex] = (byte)ClearType;
                    Array.Clear(m_pData, (int)m_StartIndex + sizeof(byte), sizeof(int));
				    break;
                case SMART_VALUE_TYPE.VT_BIGINT:
                case SMART_VALUE_TYPE.VT_UBIGINT:
                    m_pData[m_StartIndex] = (byte)ClearType;
                    Array.Clear(m_pData, (int)m_StartIndex + sizeof(byte), sizeof(long));
				    break;
                case SMART_VALUE_TYPE.VT_FLOAT:
                    m_pData[m_StartIndex] = (byte)ClearType;
                    Array.Clear(m_pData, (int)m_StartIndex + sizeof(byte), sizeof(float));
				    break;
                case SMART_VALUE_TYPE.VT_DOUBLE:
                    m_pData[m_StartIndex] = (byte)ClearType;
                    Array.Clear(m_pData, (int)m_StartIndex + sizeof(byte), sizeof(double));
				    break;
                case SMART_VALUE_TYPE.VT_STRING_UTF8:
                case SMART_VALUE_TYPE.VT_STRING_ANSI:
                    m_pData[m_StartIndex] = (byte)ClearType;
                    Array.Clear(m_pData, (int)m_StartIndex + sizeof(byte), sizeof(uint) + sizeof(byte));
				    break;
                case SMART_VALUE_TYPE.VT_STRING_UCS16:
                    m_pData[m_StartIndex] = (byte)ClearType;
                    Array.Clear(m_pData, (int)m_StartIndex + sizeof(byte), sizeof(uint) + sizeof(char));
				    break;
                case SMART_VALUE_TYPE.VT_STRUCT:
                case SMART_VALUE_TYPE.VT_BINARY:
                    m_pData[m_StartIndex] = (byte)ClearType;
                    Array.Clear(m_pData, (int)m_StartIndex + sizeof(byte), sizeof(uint));
                    break;              
                case SMART_VALUE_TYPE.VT_BOOL:
                    m_pData[m_StartIndex] = (byte)ClearType;
                    Array.Clear(m_pData, (int)m_StartIndex + sizeof(byte), sizeof(byte));
				    break;
			    }
		    }
		    return true;
	    }

        public void Destory()
	    {
            m_pData = NULL_DATA;
            m_StartIndex = 0;
            m_DataLen = (uint)NULL_DATA.Length;
        }

        public SMART_VALUE_TYPE GetValueType()
	    {
		    if(m_pData==null)
			    return SMART_VALUE_TYPE.VT_UNKNOWN;
		    return (SMART_VALUE_TYPE)m_pData[m_StartIndex];
	    }
        public uint GetLength()
	    {
            switch (GetValueType())
            {
                case SMART_VALUE_TYPE.VT_NULL:
                    return 0;
                case SMART_VALUE_TYPE.VT_CHAR:
                case SMART_VALUE_TYPE.VT_UCHAR:
                    return sizeof(byte);
                case SMART_VALUE_TYPE.VT_SHORT:
                case SMART_VALUE_TYPE.VT_USHORT:
                    return sizeof(short);
                case SMART_VALUE_TYPE.VT_INT:
                case SMART_VALUE_TYPE.VT_UINT:
                    return sizeof(int);
                case SMART_VALUE_TYPE.VT_BIGINT:
                case SMART_VALUE_TYPE.VT_UBIGINT:
                    return sizeof(long);
                case SMART_VALUE_TYPE.VT_FLOAT:
                    return sizeof(float);
                case SMART_VALUE_TYPE.VT_DOUBLE:
                    return sizeof(double);
                case SMART_VALUE_TYPE.VT_STRING_UTF8:
                case SMART_VALUE_TYPE.VT_STRING_ANSI:
                case SMART_VALUE_TYPE.VT_STRING_UCS16:
                case SMART_VALUE_TYPE.VT_STRUCT:
                case SMART_VALUE_TYPE.VT_BINARY:
                case SMART_VALUE_TYPE.VT_ARRAY:
                    return BitConverter.ToUInt32(m_pData, (int)m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_BOOL:
                    return sizeof(byte);
            }
		    return 0;
	    }
        void SetLength(uint Len)
        {
            switch (GetValueType())
            {
                case SMART_VALUE_TYPE.VT_STRING_UTF8:
                case SMART_VALUE_TYPE.VT_STRING_ANSI:
                case SMART_VALUE_TYPE.VT_STRING_UCS16:
                case SMART_VALUE_TYPE.VT_STRUCT:
                case SMART_VALUE_TYPE.VT_BINARY:
                case SMART_VALUE_TYPE.VT_ARRAY:
                    Array.Copy(BitConverter.GetBytes(Len), 0, m_pData, m_StartIndex + sizeof(byte), sizeof(uint));
                    break;
            }
        }
        public uint GetDataLen()
	    {
            switch (GetValueType())
            {
                case SMART_VALUE_TYPE.VT_NULL:
                    return sizeof(byte);
                case SMART_VALUE_TYPE.VT_CHAR:
                case SMART_VALUE_TYPE.VT_UCHAR:
                    return sizeof(byte) + sizeof(byte);
                case SMART_VALUE_TYPE.VT_SHORT:
                case SMART_VALUE_TYPE.VT_USHORT:
                    return sizeof(short) + sizeof(byte);
                case SMART_VALUE_TYPE.VT_INT:
                case SMART_VALUE_TYPE.VT_UINT:
                    return sizeof(int) + sizeof(byte);
                case SMART_VALUE_TYPE.VT_BIGINT:
                case SMART_VALUE_TYPE.VT_UBIGINT:
                    return sizeof(long) + sizeof(byte);
                case SMART_VALUE_TYPE.VT_FLOAT:
                    return sizeof(float) + sizeof(byte);
                case SMART_VALUE_TYPE.VT_DOUBLE:
                    return sizeof(double) + sizeof(byte);
                case SMART_VALUE_TYPE.VT_STRING_UTF8:
                case SMART_VALUE_TYPE.VT_STRING_ANSI:
                    return BitConverter.ToUInt32(m_pData, (int)m_StartIndex + sizeof(byte)) + sizeof(byte) + sizeof(uint) + sizeof(byte);
                case SMART_VALUE_TYPE.VT_STRING_UCS16:
                    return BitConverter.ToUInt32(m_pData, (int)m_StartIndex + sizeof(byte)) + sizeof(byte) + sizeof(uint) + sizeof(char);
                case SMART_VALUE_TYPE.VT_STRUCT:
                case SMART_VALUE_TYPE.VT_BINARY:
                case SMART_VALUE_TYPE.VT_ARRAY:
                    return BitConverter.ToUInt32(m_pData, (int)m_StartIndex + sizeof(byte)) + sizeof(byte) + sizeof(uint);
                case SMART_VALUE_TYPE.VT_BOOL:
                    return sizeof(byte) + sizeof(byte);
            }
		    return 0;
	    }
        public byte[] GetData()
	    {
		    return m_pData;
	    }
        public uint GetDataStartIndex()
        {
            return m_StartIndex;
        }
        public static implicit operator sbyte(CSmartValue Value)
        {
            switch (Value.GetValueType())
            {
                case SMART_VALUE_TYPE.VT_CHAR:
                    return (sbyte)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
                case SMART_VALUE_TYPE.VT_UCHAR:
                    return (sbyte)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
                case SMART_VALUE_TYPE.VT_SHORT:
                    return (sbyte)BitConverter.ToInt16(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_USHORT:
                    return (sbyte)BitConverter.ToUInt16(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_INT:
                    return (sbyte)BitConverter.ToInt32(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_UINT:
                    return (sbyte)BitConverter.ToUInt32(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_BIGINT:
                    return (sbyte)BitConverter.ToInt64(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_UBIGINT:
                    return (sbyte)BitConverter.ToUInt64(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_FLOAT:
                    return (sbyte)BitConverter.ToSingle(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_DOUBLE:
                    return (sbyte)BitConverter.ToDouble(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_BOOL:
                    return (sbyte)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
            }
            return 0;
        }
        public static implicit operator byte(CSmartValue Value)
	    {
		    switch(Value.GetValueType())
		    {		
		    case SMART_VALUE_TYPE.VT_CHAR:
			    return Value.m_pData[Value.m_StartIndex+sizeof(byte)];
		    case SMART_VALUE_TYPE.VT_UCHAR:
			    return Value.m_pData[Value.m_StartIndex+sizeof(byte)];
		    case SMART_VALUE_TYPE.VT_SHORT:
			    return (byte)BitConverter.ToInt16(Value.m_pData,(int)Value.m_StartIndex+sizeof(byte));
		    case SMART_VALUE_TYPE.VT_USHORT:
			    return (byte)BitConverter.ToUInt16(Value.m_pData,(int)Value.m_StartIndex+sizeof(byte));
		    case SMART_VALUE_TYPE.VT_INT:
			    return (byte)BitConverter.ToInt32(Value.m_pData,(int)Value.m_StartIndex+sizeof(byte));
		    case SMART_VALUE_TYPE.VT_UINT:
			    return (byte)BitConverter.ToUInt32(Value.m_pData,(int)Value.m_StartIndex+sizeof(byte));
		    case SMART_VALUE_TYPE.VT_BIGINT:
			    return (byte)BitConverter.ToInt64(Value.m_pData,(int)Value.m_StartIndex+sizeof(byte));
		    case SMART_VALUE_TYPE.VT_UBIGINT:
                return (byte)BitConverter.ToUInt64(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
		    case SMART_VALUE_TYPE.VT_FLOAT:
                return (byte)BitConverter.ToSingle(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
		    case SMART_VALUE_TYPE.VT_DOUBLE:
                return (byte)BitConverter.ToDouble(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
		    case SMART_VALUE_TYPE.VT_BOOL:
                return Value.m_pData[Value.m_StartIndex + sizeof(byte)];
		    }
		    return 0;
	    }

        public static implicit operator short(CSmartValue Value)
        {
            switch (Value.GetValueType())
            {
                case SMART_VALUE_TYPE.VT_CHAR:
                    return (short)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
                case SMART_VALUE_TYPE.VT_UCHAR:
                    return (short)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
                case SMART_VALUE_TYPE.VT_SHORT:
                    return (short)BitConverter.ToInt16(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_USHORT:
                    return (short)BitConverter.ToUInt16(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_INT:
                    return (short)BitConverter.ToInt32(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_UINT:
                    return (short)BitConverter.ToUInt32(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_BIGINT:
                    return (short)BitConverter.ToInt64(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_UBIGINT:
                    return (short)BitConverter.ToUInt64(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_FLOAT:
                    return (short)BitConverter.ToSingle(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_DOUBLE:
                    return (short)BitConverter.ToDouble(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_BOOL:
                    return (short)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
            }
            return 0;
        }
        public static implicit operator ushort(CSmartValue Value)
        {
            switch (Value.GetValueType())
            {
                case SMART_VALUE_TYPE.VT_CHAR:
                    return (ushort)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
                case SMART_VALUE_TYPE.VT_UCHAR:
                    return (ushort)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
                case SMART_VALUE_TYPE.VT_SHORT:
                    return (ushort)BitConverter.ToInt16(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_USHORT:
                    return (ushort)BitConverter.ToUInt16(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_INT:
                    return (ushort)BitConverter.ToInt32(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_UINT:
                    return (ushort)BitConverter.ToUInt32(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_BIGINT:
                    return (ushort)BitConverter.ToInt64(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_UBIGINT:
                    return (ushort)BitConverter.ToUInt64(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_FLOAT:
                    return (ushort)BitConverter.ToSingle(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_DOUBLE:
                    return (ushort)BitConverter.ToDouble(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_BOOL:
                    return (ushort)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
            }
            return 0;
        }
        public static implicit operator int(CSmartValue Value)
        {
            switch (Value.GetValueType())
            {
                case SMART_VALUE_TYPE.VT_CHAR:
                    return (int)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
                case SMART_VALUE_TYPE.VT_UCHAR:
                    return (int)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
                case SMART_VALUE_TYPE.VT_SHORT:
                    return (int)BitConverter.ToInt16(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_USHORT:
                    return (int)BitConverter.ToUInt16(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_INT:
                    return (int)BitConverter.ToInt32(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_UINT:
                    return (int)BitConverter.ToUInt32(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_BIGINT:
                    return (int)BitConverter.ToInt64(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_UBIGINT:
                    return (int)BitConverter.ToUInt64(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_FLOAT:
                    return (int)BitConverter.ToSingle(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_DOUBLE:
                    return (int)BitConverter.ToDouble(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_BOOL:
                    return (int)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
            }
            return 0;
        }
        public static implicit operator uint(CSmartValue Value)
        {
            switch (Value.GetValueType())
            {
                case SMART_VALUE_TYPE.VT_CHAR:
                    return (uint)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
                case SMART_VALUE_TYPE.VT_UCHAR:
                    return (uint)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
                case SMART_VALUE_TYPE.VT_SHORT:
                    return (uint)BitConverter.ToInt16(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_USHORT:
                    return (uint)BitConverter.ToUInt16(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_INT:
                    return (uint)BitConverter.ToInt32(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_UINT:
                    return (uint)BitConverter.ToUInt32(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_BIGINT:
                    return (uint)BitConverter.ToInt64(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_UBIGINT:
                    return (uint)BitConverter.ToUInt64(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_FLOAT:
                    return (uint)BitConverter.ToSingle(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_DOUBLE:
                    return (uint)BitConverter.ToDouble(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_BOOL:
                    return (uint)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
            }
            return 0;
        }
        public static implicit operator long(CSmartValue Value)
        {
            switch (Value.GetValueType())
            {
                case SMART_VALUE_TYPE.VT_CHAR:
                    return (long)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
                case SMART_VALUE_TYPE.VT_UCHAR:
                    return (long)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
                case SMART_VALUE_TYPE.VT_SHORT:
                    return (long)BitConverter.ToInt16(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_USHORT:
                    return (long)BitConverter.ToUInt16(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_INT:
                    return (long)BitConverter.ToInt32(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_UINT:
                    return (long)BitConverter.ToUInt32(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_BIGINT:
                    return (long)BitConverter.ToInt64(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_UBIGINT:
                    return (long)BitConverter.ToUInt64(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_FLOAT:
                    return (long)BitConverter.ToSingle(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_DOUBLE:
                    return (long)BitConverter.ToDouble(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_BOOL:
                    return (long)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
            }
            return 0;
        }
        public static implicit operator ulong(CSmartValue Value)
        {
            switch (Value.GetValueType())
            {
                case SMART_VALUE_TYPE.VT_CHAR:
                    return (ulong)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
                case SMART_VALUE_TYPE.VT_UCHAR:
                    return (ulong)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
                case SMART_VALUE_TYPE.VT_SHORT:
                    return (ulong)BitConverter.ToInt16(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_USHORT:
                    return (ulong)BitConverter.ToUInt16(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_INT:
                    return (ulong)BitConverter.ToInt32(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_UINT:
                    return (ulong)BitConverter.ToUInt32(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_BIGINT:
                    return (ulong)BitConverter.ToInt64(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_UBIGINT:
                    return (ulong)BitConverter.ToUInt64(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_FLOAT:
                    return (ulong)BitConverter.ToSingle(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_DOUBLE:
                    return (ulong)BitConverter.ToDouble(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_BOOL:
                    return (ulong)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
            }
            return 0;
        }
        public static implicit operator float(CSmartValue Value)
        {
            switch (Value.GetValueType())
            {
                case SMART_VALUE_TYPE.VT_CHAR:
                    return (float)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
                case SMART_VALUE_TYPE.VT_UCHAR:
                    return (float)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
                case SMART_VALUE_TYPE.VT_SHORT:
                    return (float)BitConverter.ToInt16(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_USHORT:
                    return (float)BitConverter.ToUInt16(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_INT:
                    return (float)BitConverter.ToInt32(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_UINT:
                    return (float)BitConverter.ToUInt32(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_BIGINT:
                    return (float)BitConverter.ToInt64(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_UBIGINT:
                    return (float)BitConverter.ToUInt64(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_FLOAT:
                    return (float)BitConverter.ToSingle(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_DOUBLE:
                    return (float)BitConverter.ToDouble(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_BOOL:
                    return (float)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
            }
            return 0;
        }
        public static implicit operator double(CSmartValue Value)
        {
            switch (Value.GetValueType())
            {
                case SMART_VALUE_TYPE.VT_CHAR:
                    return (double)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
                case SMART_VALUE_TYPE.VT_UCHAR:
                    return (double)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
                case SMART_VALUE_TYPE.VT_SHORT:
                    return (double)BitConverter.ToInt16(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_USHORT:
                    return (double)BitConverter.ToUInt16(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_INT:
                    return (double)BitConverter.ToInt32(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_UINT:
                    return (double)BitConverter.ToUInt32(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_BIGINT:
                    return (double)BitConverter.ToInt64(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_UBIGINT:
                    return (double)BitConverter.ToUInt64(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_FLOAT:
                    return (double)BitConverter.ToSingle(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_DOUBLE:
                    return (double)BitConverter.ToDouble(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_BOOL:
                    return (double)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
            }
            return 0;
        }

        public static implicit operator bool(CSmartValue Value)
        {
            switch (Value.GetValueType())
            {
                case SMART_VALUE_TYPE.VT_CHAR:
                    return Value.m_pData[Value.m_StartIndex + sizeof(byte)] != 0;
                case SMART_VALUE_TYPE.VT_UCHAR:
                    return Value.m_pData[Value.m_StartIndex + sizeof(byte)] != 0;
                case SMART_VALUE_TYPE.VT_SHORT:
                    return BitConverter.ToInt16(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte)) != 0;
                case SMART_VALUE_TYPE.VT_USHORT:
                    return BitConverter.ToUInt16(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte)) != 0;
                case SMART_VALUE_TYPE.VT_INT:
                    return BitConverter.ToInt32(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte)) != 0;
                case SMART_VALUE_TYPE.VT_UINT:
                    return BitConverter.ToUInt32(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte)) != 0;
                case SMART_VALUE_TYPE.VT_BIGINT:
                    return BitConverter.ToInt64(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte)) != 0;
                case SMART_VALUE_TYPE.VT_UBIGINT:
                    return BitConverter.ToUInt64(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte)) != 0;
                case SMART_VALUE_TYPE.VT_FLOAT:
                    return BitConverter.ToSingle(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte)) != 0;
                case SMART_VALUE_TYPE.VT_DOUBLE:
                    return BitConverter.ToDouble(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte)) != 0;
                case SMART_VALUE_TYPE.VT_BOOL:
                    return Value.m_pData[Value.m_StartIndex + sizeof(byte)] != 0;
            }
            return false;
        }

        public static implicit operator byte[](CSmartValue Value)
        {
            switch (Value.GetValueType())
            {
                case SMART_VALUE_TYPE.VT_BINARY:
                case SMART_VALUE_TYPE.VT_STRING_ANSI:
                case SMART_VALUE_TYPE.VT_STRING_UTF8:
                case SMART_VALUE_TYPE.VT_STRING_UCS16:
                    {
                        byte[] Temp = new byte[Value.GetLength()];
                        Array.Copy(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte) + sizeof(uint), Temp, 0, Value.GetLength());
                        return Temp;
                    }
            }
            return null;
        }

        public static implicit operator string(CSmartValue Value)
        {            
            switch (Value.GetValueType())
            {
                case SMART_VALUE_TYPE.VT_STRING_ANSI:
                    {
                        string Temp = Encoding.GetEncoding(936).GetString(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte) + sizeof(uint), (int)Value.GetLength());
                        return Temp;
                    }
                case SMART_VALUE_TYPE.VT_STRING_UTF8:
                    {
                        string Temp = Encoding.UTF8.GetString(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte) + sizeof(uint), (int)Value.GetLength());
                        return Temp;
                    }               
                case SMART_VALUE_TYPE.VT_STRING_UCS16:
                    {
                        string Temp = Encoding.Unicode.GetString(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte) + sizeof(uint), (int)Value.GetLength());
                        return Temp;
                    }                
            }
            return null;
        }

        public static implicit operator CSmartStruct(CSmartValue Value)
        {
            switch (Value.GetValueType())
            {
                case SMART_VALUE_TYPE.VT_STRUCT:
                    return new CSmartStruct(Value.m_pData, Value.m_StartIndex, Value.m_DataLen, false);
            }
            return new CSmartStruct();
        }

        public static implicit operator CSmartArray(CSmartValue Value)
        {
            switch (Value.GetValueType())
            {
                case SMART_VALUE_TYPE.VT_ARRAY:
                    return new CSmartArray(Value.m_pData, Value.m_StartIndex, Value.m_DataLen, false);
            }
            return new CSmartArray();
        }
        public static implicit operator CVariedValue(CSmartValue Value)
        {
            switch (Value.GetValueType())
            {
                case SMART_VALUE_TYPE.VT_CHAR:
                    return (sbyte)Value.m_pData[Value.m_StartIndex + sizeof(byte)];
                case SMART_VALUE_TYPE.VT_UCHAR:
                    return Value.m_pData[Value.m_StartIndex + sizeof(byte)];
                case SMART_VALUE_TYPE.VT_SHORT:
                    return BitConverter.ToInt16(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_USHORT:
                    return BitConverter.ToUInt16(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_INT:
                    return BitConverter.ToInt32(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_UINT:
                    return BitConverter.ToUInt32(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_BIGINT:
                    return BitConverter.ToInt64(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_UBIGINT:
                    return BitConverter.ToUInt64(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_FLOAT:
                    return BitConverter.ToSingle(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_DOUBLE:
                    return BitConverter.ToDouble(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte));
                case SMART_VALUE_TYPE.VT_STRING_ANSI:
                    {
                        string Temp = Encoding.GetEncoding(936).GetString(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte) + sizeof(uint), (int)Value.GetLength());
                        return Temp;
                    }
                case SMART_VALUE_TYPE.VT_STRING_UTF8:
                    {
                        string Temp = Encoding.UTF8.GetString(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte) + sizeof(uint), (int)Value.GetLength());
                        return Temp;
                    }
                case SMART_VALUE_TYPE.VT_STRUCT:
                case SMART_VALUE_TYPE.VT_STRING_UCS16:
                    {
                        string Temp = Encoding.Unicode.GetString(Value.m_pData, (int)Value.m_StartIndex + sizeof(byte) + sizeof(uint), (int)Value.GetLength());
                        return Temp;
                    }
                case SMART_VALUE_TYPE.VT_BINARY:
                    {
                        return CVariedValue.FromBinary(Value.m_pData, Value.m_StartIndex + sizeof(byte) + sizeof(uint), Value.GetLength());
                    }
                case SMART_VALUE_TYPE.VT_ARRAY:
                    {                        
                        CSmartArray Array = Value;
                        CVariedValue VValue = CVariedValue.CreateArray((int)Array.GetArrayLength());
                        int Pos = Array.GetFirstMemberPosition();
                        while (Pos >= 0)
                        {
                            VValue.AddChild(Array.GetNextMember(ref Pos));
                        }
                        return VValue;
                    }
                case SMART_VALUE_TYPE.VT_BOOL:
                    {
                        CSmartStruct Struct = Value;
                        CVariedValue VValue = CVariedValue.CreateTable();
                        int Pos = Struct.GetFirstMemberPosition();
                        while (Pos >= 0)
                        {
                            ushort MemberID = 0;
                            CSmartStruct SubStruct = Struct.GetNextMember(ref Pos,ref MemberID);
                            if(SubStruct.IsMemberExist((ushort)SST_PAIR.KEY)&& SubStruct.IsMemberExist((ushort)SST_PAIR.DATA))
                            {
                                VValue.AddChild(SubStruct.GetMember((ushort)SST_PAIR.KEY), SubStruct.GetMember((ushort)SST_PAIR.DATA));
                            }
                        }
                        return VValue;
                    }
            }
            return new CVariedValue();
        }

        public static implicit operator CSmartValue(sbyte Value)
        {
            CSmartValue SmartValue = new CSmartValue();
            SmartValue.Create(SMART_VALUE_TYPE.VT_CHAR, 0);
            SmartValue.m_pData[sizeof(byte)] = (byte)Value;
            return SmartValue;
        }
        public static implicit operator CSmartValue(byte Value)
        {
            CSmartValue SmartValue = new CSmartValue();
            SmartValue.Create(SMART_VALUE_TYPE.VT_UCHAR, 0);
            SmartValue.m_pData[sizeof(byte)] = (byte)Value;
            return SmartValue;
        }
        public static implicit operator CSmartValue(short Value)
        {
            CSmartValue SmartValue = new CSmartValue();
            SmartValue.Create(SMART_VALUE_TYPE.VT_SHORT, 0);
            Array.Copy(BitConverter.GetBytes(Value), 0, SmartValue.m_pData, sizeof(byte), sizeof(short));
            return SmartValue;
        }
        public static implicit operator CSmartValue(ushort Value)
        {
            CSmartValue SmartValue = new CSmartValue();
            SmartValue.Create(SMART_VALUE_TYPE.VT_USHORT, 0);
            Array.Copy(BitConverter.GetBytes(Value), 0, SmartValue.m_pData, sizeof(byte), sizeof(ushort));
            return SmartValue;
        }
        public static implicit operator CSmartValue(int Value)
        {
            CSmartValue SmartValue = new CSmartValue();           
            SmartValue.Create(SMART_VALUE_TYPE.VT_INT, 0);
            Array.Copy(BitConverter.GetBytes(Value), 0, SmartValue.m_pData, sizeof(byte), sizeof(int));
            return SmartValue;
        }
        public static implicit operator CSmartValue(uint Value)
        {
            CSmartValue SmartValue = new CSmartValue();            
            SmartValue.Create(SMART_VALUE_TYPE.VT_UINT, 0);
            Array.Copy(BitConverter.GetBytes(Value), 0, SmartValue.m_pData, sizeof(byte), sizeof(uint));
            return SmartValue;
        }
        public static implicit operator CSmartValue(long Value)
        {
            CSmartValue SmartValue = new CSmartValue();           
            SmartValue.Create(SMART_VALUE_TYPE.VT_BIGINT, 0);
            Array.Copy(BitConverter.GetBytes(Value), 0, SmartValue.m_pData, sizeof(byte), sizeof(long));
            return SmartValue;
        }
        public static implicit operator CSmartValue(ulong Value)
        {
            CSmartValue SmartValue = new CSmartValue();           
            SmartValue.Create(SMART_VALUE_TYPE.VT_UBIGINT, 0);
            Array.Copy(BitConverter.GetBytes(Value), 0, SmartValue.m_pData, sizeof(byte), sizeof(ulong));
            return SmartValue;
        }
        public static implicit operator CSmartValue(float Value)
        {
            CSmartValue SmartValue = new CSmartValue();
            SmartValue.Create(SMART_VALUE_TYPE.VT_FLOAT, 0);
            Array.Copy(BitConverter.GetBytes(Value), 0, SmartValue.m_pData, sizeof(byte), sizeof(float));
            return SmartValue;
        }
        public static implicit operator CSmartValue(double Value)
        {
            CSmartValue SmartValue = new CSmartValue();
            SmartValue.Create(SMART_VALUE_TYPE.VT_DOUBLE, 0);
            Array.Copy(BitConverter.GetBytes(Value), 0, SmartValue.m_pData, sizeof(byte), sizeof(double));
            return SmartValue;
        }
        public static implicit operator CSmartValue(bool Value)
        {
            CSmartValue SmartValue = new CSmartValue();
            SmartValue.Create(SMART_VALUE_TYPE.VT_BOOL, 0);
            SmartValue.m_pData[sizeof(byte)] = Value ? (byte)1 : (byte)0;
            return SmartValue;
        }
        public static implicit operator CSmartValue(string Value)
        {
            CSmartValue SmartValue = new CSmartValue();
            switch (INTERNAL_STRING_CODE_PAGE)
            {
                case STRING_CODE_PAGE.STRING_CODE_PAGE_ANSI:
                    {
                        SmartValue.Create(SMART_VALUE_TYPE.VT_STRING_ANSI, (uint)Encoding.GetEncoding(936).GetByteCount(Value));
                        SmartValue.SetValue(Value);                        
                    }
                    break;
                case STRING_CODE_PAGE.STRING_CODE_PAGE_UTF8:
                    {
                        SmartValue.Create(SMART_VALUE_TYPE.VT_STRING_UTF8, (uint)Encoding.UTF8.GetByteCount(Value));
                        SmartValue.SetValue(Value);                        
                    }
                    break;
                case STRING_CODE_PAGE.STRING_CODE_PAGE_UCS16:
                    {
                        SmartValue.Create(SMART_VALUE_TYPE.VT_STRING_UCS16, (uint)Encoding.Unicode.GetByteCount(Value));
                        SmartValue.SetValue(Value);
                    }
                    break;
            }
            return SmartValue;
        }
        public static implicit operator CSmartValue(byte[] Value)
        {
            CSmartValue SmartValue = new CSmartValue();
            SmartValue.Create(SMART_VALUE_TYPE.VT_BINARY, (uint)Value.Length);
            SmartValue.SetValue(Value);
            return SmartValue;
        }
        public static implicit operator CSmartValue(CSmartArray Value)
        {
            CSmartValue SmartValue = new CSmartValue();
            SmartValue.m_pData = Value.GetData();
            SmartValue.m_StartIndex = Value.GetDataStartIndex();
            SmartValue.m_DataLen = Value.GetDataLen();
            return SmartValue;
        }
        public static implicit operator CSmartValue(CSmartStruct Value)
        {
            CSmartValue SmartValue = new CSmartValue();
            SmartValue.m_pData = Value.GetData();
            SmartValue.m_StartIndex = Value.GetDataStartIndex();
            SmartValue.m_DataLen = Value.GetDataLen();
            return SmartValue;
        }
        public static implicit operator CSmartValue(CVariedValue Value)
        {
            CSmartValue SmartValue = new CSmartValue();
            switch(Value.Type)
            {
                case VARIED_VALUE_TYPE.NIL:
                    break;
                case VARIED_VALUE_TYPE.BOOLEAN:
                    SmartValue.Create(SMART_VALUE_TYPE.VT_BOOL, 0);
                    SmartValue.m_pData[sizeof(byte)] = ((bool)Value) ? (byte)1 : (byte)0;
                    break;
                case VARIED_VALUE_TYPE.INT8:
                    SmartValue.Create(SMART_VALUE_TYPE.VT_CHAR, 0);
                    SmartValue.m_pData[sizeof(byte)] = (byte)Value;
                    break;
                case VARIED_VALUE_TYPE.UINT8:
                    SmartValue.Create(SMART_VALUE_TYPE.VT_UCHAR, 0);
                    SmartValue.m_pData[sizeof(byte)] = (byte)Value;
                    break;
                case VARIED_VALUE_TYPE.INT16:
                    SmartValue.Create(SMART_VALUE_TYPE.VT_SHORT, 0);
                    Array.Copy(BitConverter.GetBytes((short)Value), 0, SmartValue.m_pData, sizeof(byte), sizeof(short));
                    break;
                case VARIED_VALUE_TYPE.UINT16:
                    SmartValue.Create(SMART_VALUE_TYPE.VT_USHORT, 0);
                    Array.Copy(BitConverter.GetBytes((ushort)Value), 0, SmartValue.m_pData, sizeof(byte), sizeof(ushort));
                    break;
                case VARIED_VALUE_TYPE.INT32:
                    SmartValue.Create(SMART_VALUE_TYPE.VT_INT, 0);
                    Array.Copy(BitConverter.GetBytes((int)Value), 0, SmartValue.m_pData, sizeof(byte), sizeof(int));
                    break;
                case VARIED_VALUE_TYPE.UINT32:
                    SmartValue.Create(SMART_VALUE_TYPE.VT_UINT, 0);
                    Array.Copy(BitConverter.GetBytes((uint)Value), 0, SmartValue.m_pData, sizeof(byte), sizeof(uint));
                    break;
                case VARIED_VALUE_TYPE.INT64:
                    SmartValue.Create(SMART_VALUE_TYPE.VT_BIGINT, 0);
                    Array.Copy(BitConverter.GetBytes((long)Value), 0, SmartValue.m_pData, sizeof(byte), sizeof(long));
                    break;
                case VARIED_VALUE_TYPE.UINT64:
                    SmartValue.Create(SMART_VALUE_TYPE.VT_UBIGINT, 0);
                    Array.Copy(BitConverter.GetBytes((ulong)Value), 0, SmartValue.m_pData, sizeof(byte), sizeof(ulong));
                    break;
                case VARIED_VALUE_TYPE.FLOAT32:
                    SmartValue.Create(SMART_VALUE_TYPE.VT_FLOAT, 0);
                    Array.Copy(BitConverter.GetBytes((float)Value), 0, SmartValue.m_pData, sizeof(byte), sizeof(float));
                    break;
                case VARIED_VALUE_TYPE.FLOAT64:
                    SmartValue.Create(SMART_VALUE_TYPE.VT_DOUBLE, 0);
                    Array.Copy(BitConverter.GetBytes((double)Value), 0, SmartValue.m_pData, sizeof(byte), sizeof(double));
                    break;
                case VARIED_VALUE_TYPE.STRING:
                    switch (INTERNAL_STRING_CODE_PAGE)
                    {
                        case STRING_CODE_PAGE.STRING_CODE_PAGE_ANSI:
                            {
                                SmartValue.Create(SMART_VALUE_TYPE.VT_STRING_ANSI, (uint)Encoding.GetEncoding(936).GetByteCount(Value));
                                SmartValue.SetValue((string)Value);
                            }
                            break;
                        case STRING_CODE_PAGE.STRING_CODE_PAGE_UTF8:
                            {
                                SmartValue.Create(SMART_VALUE_TYPE.VT_STRING_UTF8, (uint)Encoding.UTF8.GetByteCount(Value));
                                SmartValue.SetValue((string)Value);
                            }
                            break;
                        case STRING_CODE_PAGE.STRING_CODE_PAGE_UCS16:
                            {
                                SmartValue.Create(SMART_VALUE_TYPE.VT_STRING_UCS16, (uint)Encoding.Unicode.GetByteCount(Value));
                                SmartValue.SetValue((string)Value);
                            }
                            break;
                    }
                    break;
                case VARIED_VALUE_TYPE.BINARY:
                    SmartValue.Create(SMART_VALUE_TYPE.VT_BINARY, Value.GetLength());
                    SmartValue.SetValue((byte[])Value);
                    break;
                case VARIED_VALUE_TYPE.ARRAY:
                    {
                        CSmartArray Array = new CSmartArray(GetVariedValueSize(Value));
                        foreach(CVariedValue Child in Value)
                        {
                            Array.AddMember(Child);
                        }
                        SmartValue = Array;
                    }
                    break;
                case VARIED_VALUE_TYPE.TABLE:
                    {
                        CSmartStruct Struct = new CSmartStruct(GetVariedValueSize(Value));
                        foreach (KeyValuePair<CVariedValue, CVariedValue> pair in Value)
                        {
                            CSmartStruct SubStruct = Struct.PrepareSubStruct();
                            SubStruct.AddMember((ushort)SST_PAIR.KEY, pair.Key);
                            SubStruct.AddMember((ushort)SST_PAIR.DATA, pair.Value);
                            Struct.FinishMember(1, SubStruct.GetDataLen());
                        }
                        SmartValue = Struct;
                    }
                    break;
            }
            return SmartValue;
        }
        public bool SetValue<T>(T Value) where T : IConvertible
        {
            if(m_pData!=null)
            {
                switch (GetValueType())
                {
                    case SMART_VALUE_TYPE.VT_CHAR:
                    case SMART_VALUE_TYPE.VT_UCHAR:
                        m_pData[m_StartIndex + sizeof(byte)] = Convert.ToByte(Value);
                        break;
                    case SMART_VALUE_TYPE.VT_SHORT:
                        Array.Copy(BitConverter.GetBytes(Convert.ToInt16(Value)), 0, m_pData, m_StartIndex + sizeof(byte), sizeof(short));
                        break;
                    case SMART_VALUE_TYPE.VT_USHORT:
                        Array.Copy(BitConverter.GetBytes(Convert.ToUInt16(Value)), 0, m_pData, m_StartIndex + sizeof(byte), sizeof(short));
                        break;
                    case SMART_VALUE_TYPE.VT_INT:
                        Array.Copy(BitConverter.GetBytes(Convert.ToInt32(Value)), 0, m_pData, m_StartIndex + sizeof(byte), sizeof(int));
                        break;
                    case SMART_VALUE_TYPE.VT_UINT:
                        Array.Copy(BitConverter.GetBytes(Convert.ToUInt32(Value)), 0, m_pData, m_StartIndex + sizeof(byte), sizeof(int));
                        break;
                    case SMART_VALUE_TYPE.VT_BIGINT:
                        Array.Copy(BitConverter.GetBytes(Convert.ToInt64(Value)), 0, m_pData, m_StartIndex + sizeof(byte), sizeof(long));
                        break;
                    case SMART_VALUE_TYPE.VT_UBIGINT:
                        Array.Copy(BitConverter.GetBytes(Convert.ToUInt64(Value)), 0, m_pData, m_StartIndex + sizeof(byte), sizeof(long));
                        break;
                    case SMART_VALUE_TYPE.VT_FLOAT:
                        Array.Copy(BitConverter.GetBytes(Convert.ToSingle(Value)), 0, m_pData, m_StartIndex + sizeof(byte), sizeof(float));
                        break;
                    case SMART_VALUE_TYPE.VT_DOUBLE:
                        Array.Copy(BitConverter.GetBytes(Convert.ToDouble(Value)), 0, m_pData, m_StartIndex + sizeof(byte), sizeof(double));
                        break;
                    case SMART_VALUE_TYPE.VT_BOOL:
                        if (Convert.ToBoolean(Value))
                            m_pData[m_StartIndex + sizeof(byte)] = 1;
                        else
                            m_pData[m_StartIndex + sizeof(byte)] = 0;
                        break;
                    default:
                        return false;
                }
                return true;
            }
            return false;
        }
        public bool SetValue(string Value)
        {
            if (m_pData != null)
            {
                switch (GetValueType())
                {
                    case SMART_VALUE_TYPE.VT_STRING_ANSI:
                        {
                            byte[] Temp = Encoding.GetEncoding(936).GetBytes(Value);

                            int MaxLen = (int)m_DataLen - sizeof(byte) - sizeof(uint) - sizeof(byte);
                            if (Temp.Length < MaxLen)
                                MaxLen = Temp.Length;
                            Array.Copy(Temp, 0, m_pData, m_StartIndex + sizeof(byte) + sizeof(uint), MaxLen);
                            m_pData[m_StartIndex + sizeof(byte) + sizeof(uint) + MaxLen] = 0;
                            SetLength((uint)MaxLen);
                        }
                        break;
                    case SMART_VALUE_TYPE.VT_STRING_UTF8:
                        {                            
                            byte[] Temp = Encoding.UTF8.GetBytes(Value);

                            int MaxLen = (int)m_DataLen - sizeof(byte) - sizeof(uint) - sizeof(byte);
                            if (Temp.Length < MaxLen)
                                MaxLen = Temp.Length;
                            Array.Copy(Temp, 0, m_pData, m_StartIndex + sizeof(byte) + sizeof(uint), MaxLen);
                            m_pData[m_StartIndex + sizeof(byte) + sizeof(uint) + MaxLen] = 0;
                            SetLength((uint)MaxLen);
                        }
                        break;
                    
                    case SMART_VALUE_TYPE.VT_STRING_UCS16:
                        {
                            byte[] Temp = Encoding.Unicode.GetBytes(Value);
                            int MaxLen = (int)m_DataLen - sizeof(byte) - sizeof(uint) - sizeof(char);
                            if (Temp.Length < MaxLen)
                                MaxLen = Temp.Length;
                            Array.Copy(Temp, 0, m_pData, m_StartIndex + sizeof(byte) + sizeof(uint), MaxLen);
                            m_pData[m_StartIndex + sizeof(byte) + sizeof(uint) + MaxLen] = 0;
                            m_pData[m_StartIndex + sizeof(byte) + sizeof(uint) + MaxLen + 1] = 0;
                            SetLength((uint)MaxLen);
                        }
                        break;                   
                    default:
                        return false;
                }
                return true;
            }
            return false;
        }
        public bool SetValue(byte[] Value)
        {
            if (m_pData != null)
            {
                switch (GetValueType())
                {
                    case SMART_VALUE_TYPE.VT_BINARY:
                        {
                            int MaxLen = (int)m_DataLen - sizeof(byte) - sizeof(uint);
                            if (Value.Length < MaxLen)
                                MaxLen = Value.Length;
                            Array.Copy(Value, 0, m_pData, m_StartIndex + sizeof(byte) + sizeof(uint), MaxLen);
                            SetLength((uint)MaxLen);
                        }
                        break;                    
                }
                return true;
            }
            return false;
        }
        public bool SetValue(CSmartArray Value)
        {
            if (m_pData != null)
            {
                if (Value.GetData() != null)
                {
                    switch (GetValueType())
                    {
                        case SMART_VALUE_TYPE.VT_ARRAY:
                            {
                                if (m_DataLen < Value.GetDataLen())
                                    return false;
                                Array.Copy(Value.GetData(), (int)Value.GetDataStartIndex(), m_pData, (int)m_StartIndex, (int)Value.GetDataLen());
                            }
                            break;
                        default:
                            return false;
                    }
                }
                return true;
            }
            return false;
        }
        public bool SetValue(CSmartStruct Value)
        {
            if (m_pData != null)
            {
				if(Value.GetData() != null)
				{
	                switch (GetValueType())
	                {
	                    case SMART_VALUE_TYPE.VT_STRUCT:
	                        {
	                            if (m_DataLen < Value.GetDataLen())
	                                return false;
	                            Array.Copy(Value.GetData(), (int)Value.GetDataStartIndex(), m_pData, (int)m_StartIndex, (int)Value.GetDataLen());
	                        }
	                        break;                    
	                    default:
	                        return false;
	                }
				}
                return true;
            }
            return false;
        }
        public bool SetValue(CSmartValue Value)
        {
            if (m_pData != null)
            {
                if (m_DataLen < Value.GetDataLen())
                    return false;
                Array.Copy(Value.GetData(), (int)Value.GetDataStartIndex(), m_pData, (int)m_StartIndex, (int)Value.GetDataLen());
                return true;
            }
            return false;
        }

        public bool IsNull()
        {
            return GetValueType() == SMART_VALUE_TYPE.VT_NULL;
        }

        public static SMART_VALUE_TYPE SetGetTypeByValue(sbyte Value)
        {
            return SMART_VALUE_TYPE.VT_CHAR;
        }
        public static SMART_VALUE_TYPE SetGetTypeByValue(byte Value)
        {
            return SMART_VALUE_TYPE.VT_UCHAR;
        }
        public static SMART_VALUE_TYPE SetGetTypeByValue(short Value)
        {
            return SMART_VALUE_TYPE.VT_SHORT;
        }
        public static SMART_VALUE_TYPE SetGetTypeByValue(ushort Value)
        {
            return SMART_VALUE_TYPE.VT_USHORT;
        }
        public static SMART_VALUE_TYPE SetGetTypeByValue(int Value)
        {
            return SMART_VALUE_TYPE.VT_INT;
        }
        public static SMART_VALUE_TYPE SetGetTypeByValue(uint Value)
        {
            return SMART_VALUE_TYPE.VT_UINT;
        }
        public static SMART_VALUE_TYPE SetGetTypeByValue(long Value)
        {
            return SMART_VALUE_TYPE.VT_BIGINT;
        }
        public static SMART_VALUE_TYPE SetGetTypeByValue(ulong Value)
        {
            return SMART_VALUE_TYPE.VT_UBIGINT;
        }
        public static SMART_VALUE_TYPE SetGetTypeByValue(float Value)
        {
            return SMART_VALUE_TYPE.VT_FLOAT;
        }
        public static SMART_VALUE_TYPE SetGetTypeByValue(double Value)
        {
            return SMART_VALUE_TYPE.VT_DOUBLE;
        }
        public static SMART_VALUE_TYPE SetGetTypeByValue(bool Value)
        {
            return SMART_VALUE_TYPE.VT_BOOL;
        }
        public static uint GetValueSize(string Value)
        {
            uint DataLen = 0;
            switch (INTERNAL_STRING_CODE_PAGE)
            {
                case STRING_CODE_PAGE.STRING_CODE_PAGE_ANSI:
                    DataLen = (uint)Encoding.GetEncoding(936).GetByteCount(Value);
                    break;
                case STRING_CODE_PAGE.STRING_CODE_PAGE_UTF8:
                    DataLen = (uint)Encoding.UTF8.GetByteCount(Value);
                    break;
                case STRING_CODE_PAGE.STRING_CODE_PAGE_UCS16:
                    DataLen = (uint)Encoding.Unicode.GetByteCount(Value);
                    break;
            }
            return DataLen;
        }
        public static uint GetVariedValueSize(CVariedValue Value)
        {
            switch (Value.Type)
            {
                case VARIED_VALUE_TYPE.BOOLEAN:
                    return sizeof(bool);
                case VARIED_VALUE_TYPE.INT8:
                    return sizeof(sbyte);
                case VARIED_VALUE_TYPE.UINT8:
                    return sizeof(byte);
                case VARIED_VALUE_TYPE.INT16:
                    return sizeof(short);
                case VARIED_VALUE_TYPE.UINT16:
                    return sizeof(ushort);
                case VARIED_VALUE_TYPE.INT32:
                    return sizeof(int);
                case VARIED_VALUE_TYPE.UINT32:
                    return sizeof(uint);
                case VARIED_VALUE_TYPE.INT64:
                    return sizeof(long);
                case VARIED_VALUE_TYPE.UINT64:
                    return sizeof(ulong);
                case VARIED_VALUE_TYPE.FLOAT32:
                    return sizeof(float);
                case VARIED_VALUE_TYPE.FLOAT64:
                    return sizeof(double);
                case VARIED_VALUE_TYPE.STRING:
                    return GetValueSize((string)Value);
                case VARIED_VALUE_TYPE.BINARY:
                    return Value.GetLength();
                case VARIED_VALUE_TYPE.ARRAY:
                    {
                        uint Size = 0;
                        foreach (CVariedValue Child in Value)
                        {
                            Size += CSmartArray.GetVariedMemberSize(Child);
                        }
                        return Size;
                    }
                case VARIED_VALUE_TYPE.TABLE:
                    {
                        uint Size = 0;
                        foreach (KeyValuePair<CVariedValue, CVariedValue> pair in Value)
                        {
                            uint SubSize = CSmartStruct.GetVariedMemberSize(pair.Key) + CSmartStruct.GetVariedMemberSize(pair.Value);
                            Size += CSmartStruct.GetStructMemberSize(SubSize);
                        }
                        return Size;
                    }
            }
            return 0;
        }
        public bool CloneFrom(CSmartValue Value)
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
        public object Clone()
        {
            CSmartStruct Struct = new CSmartStruct();
            Struct.CloneFrom(this);
            return Struct;
        }
    }
}
