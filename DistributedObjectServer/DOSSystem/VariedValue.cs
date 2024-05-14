using System;
using System.Collections.Generic;
using System.Reflection;
using Newtonsoft.Json.Linq;
using Newtonsoft.Json;
using System.Collections;

namespace DOSSystem
{
    public enum VARIED_VALUE_TYPE : byte
    {
        NIL = 0,
        BOOLEAN,
        INT8,
        UINT8,
        INT16,
        UINT16,
        INT32,
        UINT32,
        INT64,
        UINT64,
        FLOAT32,
        FLOAT64,
        STRING,
        BINARY,
        ARRAY,
        TABLE
    };
    public class CVariedValue : IEqualityComparer<CVariedValue>, ICloneable, IEnumerable
    {
        class EmptyEnumerator : IEnumerator
        {
            public object Current { get { return null; } }

            public bool MoveNext()
            {
                return false;
            }
            public void Reset()
            {
            }            
        }

        VARIED_VALUE_TYPE m_Type = VARIED_VALUE_TYPE.NIL;
        object m_Value = null;

        public CVariedValue(VARIED_VALUE_TYPE Type = VARIED_VALUE_TYPE.NIL, object Value = null)
        {
            m_Type = Type;
            m_Value = Value;
        }
        public void CloneFrom(CVariedValue Value)
        {
            m_Type = Value.m_Type;
            switch (m_Type)
            {
                case VARIED_VALUE_TYPE.BINARY:
                    m_Value = ((byte[])Value.m_Value).Clone();
                    break;
                case VARIED_VALUE_TYPE.ARRAY:
                    m_Value = new List<CVariedValue>((List<CVariedValue>)Value.m_Value);
                    break;
                case VARIED_VALUE_TYPE.TABLE:
                    m_Value = new Dictionary<CVariedValue, CVariedValue>((Dictionary<CVariedValue, CVariedValue>)Value.m_Value);
                    break;
                default:
                    m_Value = Value.m_Value;
                    break;
            }
        }
        public object Clone()
        {
            CVariedValue CloneValue = new CVariedValue();
            CloneValue.CloneFrom(this);
            return CloneValue;
        }
        public bool Equals(CVariedValue x, CVariedValue y)
        {
            return x.m_Value.Equals(y.m_Value);
        }

        public int GetHashCode(CVariedValue obj)
        {
            return obj.m_Value.GetHashCode();
        }        
        IEnumerator IEnumerable.GetEnumerator()
        {
            switch (m_Type)
            {
                case VARIED_VALUE_TYPE.ARRAY:
                    return ((List<CVariedValue>)m_Value).GetEnumerator();
                case VARIED_VALUE_TYPE.TABLE:
                    return ((Dictionary<CVariedValue, CVariedValue>)m_Value).GetEnumerator();
            }
            return new EmptyEnumerator();
        }
        public void Clear()
        {
            m_Type = VARIED_VALUE_TYPE.NIL;
            m_Value = null;
        }
        public VARIED_VALUE_TYPE Type
        {
            get { return m_Type; }
        }
        public object Value
        {
            get { return m_Value; }
        }
        public static implicit operator CVariedValue(bool Value)
        {
            return new CVariedValue(VARIED_VALUE_TYPE.BOOLEAN, Value);
        }

        public static implicit operator CVariedValue(sbyte Value)
        {
            return new CVariedValue(VARIED_VALUE_TYPE.INT8, Value);
        }

        public static implicit operator CVariedValue(byte Value)
        {
            return new CVariedValue(VARIED_VALUE_TYPE.UINT8, Value);
        }

        public static implicit operator CVariedValue(short Value)
        {
            return new CVariedValue(VARIED_VALUE_TYPE.INT16, Value);
        }

        public static implicit operator CVariedValue(ushort Value)
        {
            return new CVariedValue(VARIED_VALUE_TYPE.UINT16, Value);
        }

        public static implicit operator CVariedValue(int Value)
        {
            return new CVariedValue(VARIED_VALUE_TYPE.INT32, Value);
        }

        public static implicit operator CVariedValue(uint Value)
        {
            return new CVariedValue(VARIED_VALUE_TYPE.UINT32, Value);
        }

        public static implicit operator CVariedValue(long Value)
        {
            return new CVariedValue(VARIED_VALUE_TYPE.INT64, Value);
        }

        public static implicit operator CVariedValue(ulong Value)
        {
            return new CVariedValue(VARIED_VALUE_TYPE.UINT64, Value);
        }

        public static implicit operator CVariedValue(float Value)
        {
            return new CVariedValue(VARIED_VALUE_TYPE.FLOAT32, Value);
        }

        public static implicit operator CVariedValue(double Value)
        {
            return new CVariedValue(VARIED_VALUE_TYPE.FLOAT64, Value);
        }

        public static implicit operator CVariedValue(string Value)
        {
            if (Value != null)
                return new CVariedValue(VARIED_VALUE_TYPE.STRING, Value);
            else
                return new CVariedValue();
        }

        public static implicit operator CVariedValue(byte[] Value)
        {
            if (Value != null)
                return new CVariedValue(VARIED_VALUE_TYPE.BINARY, Value.Clone());
            else
                return new CVariedValue();
        }

        public static implicit operator bool(CVariedValue Value)
        {
            switch (Value.Type)
            {
                case VARIED_VALUE_TYPE.BOOLEAN:
                    return (bool)Value.Value;
                case VARIED_VALUE_TYPE.INT8:
                    return ((sbyte)Value.Value) != 0;
                case VARIED_VALUE_TYPE.UINT8:
                    return ((byte)Value.Value) != 0;
                case VARIED_VALUE_TYPE.INT16:
                    return ((short)Value.Value) != 0;
                case VARIED_VALUE_TYPE.UINT16:
                    return ((ushort)Value.Value) != 0;
                case VARIED_VALUE_TYPE.INT32:
                    return ((int)Value.Value) != 0;
                case VARIED_VALUE_TYPE.UINT32:
                    return ((uint)Value.Value) != 0;
                case VARIED_VALUE_TYPE.INT64:
                    return ((long)Value.Value) != 0;
                case VARIED_VALUE_TYPE.UINT64:
                    return ((ulong)Value.Value) != 0;
                case VARIED_VALUE_TYPE.FLOAT32:
                    return ((float)Value.Value) != 0;
                case VARIED_VALUE_TYPE.FLOAT64:
                    return ((double)Value.Value) != 0;
            }
            return false;
        }


        public static implicit operator sbyte(CVariedValue Value)
        {
            switch (Value.Type)
            {
                case VARIED_VALUE_TYPE.BOOLEAN:
                    return (sbyte)(((bool)Value.Value) ? 1 : 0);
                case VARIED_VALUE_TYPE.INT8:
                    return ((sbyte)Value.Value);
                case VARIED_VALUE_TYPE.UINT8:
                    return (sbyte)((byte)Value.Value);
                case VARIED_VALUE_TYPE.INT16:
                    return (sbyte)((short)Value.Value);
                case VARIED_VALUE_TYPE.UINT16:
                    return (sbyte)((ushort)Value.Value);
                case VARIED_VALUE_TYPE.INT32:
                    return (sbyte)((int)Value.Value);
                case VARIED_VALUE_TYPE.UINT32:
                    return (sbyte)((uint)Value.Value);
                case VARIED_VALUE_TYPE.INT64:
                    return (sbyte)((long)Value.Value);
                case VARIED_VALUE_TYPE.UINT64:
                    return (sbyte)((ulong)Value.Value);
                case VARIED_VALUE_TYPE.FLOAT32:
                    return (sbyte)((float)Value.Value);
                case VARIED_VALUE_TYPE.FLOAT64:
                    return (sbyte)((double)Value.Value);
            }
            return 0;
        }


        public static implicit operator byte(CVariedValue Value)
        {
            switch (Value.Type)
            {
                case VARIED_VALUE_TYPE.BOOLEAN:
                    return (byte)(((bool)Value.Value) ? 1 : 0);
                case VARIED_VALUE_TYPE.INT8:
                    return (byte)((sbyte)Value.Value);
                case VARIED_VALUE_TYPE.UINT8:
                    return ((byte)Value.Value);
                case VARIED_VALUE_TYPE.INT16:
                    return (byte)((short)Value.Value);
                case VARIED_VALUE_TYPE.UINT16:
                    return (byte)((ushort)Value.Value);
                case VARIED_VALUE_TYPE.INT32:
                    return (byte)((int)Value.Value);
                case VARIED_VALUE_TYPE.UINT32:
                    return (byte)((uint)Value.Value);
                case VARIED_VALUE_TYPE.INT64:
                    return (byte)((long)Value.Value);
                case VARIED_VALUE_TYPE.UINT64:
                    return (byte)((ulong)Value.Value);
                case VARIED_VALUE_TYPE.FLOAT32:
                    return (byte)((float)Value.Value);
                case VARIED_VALUE_TYPE.FLOAT64:
                    return (byte)((double)Value.Value);
            }
            return 0;
        }


        public static implicit operator short(CVariedValue Value)
        {
            switch (Value.Type)
            {
                case VARIED_VALUE_TYPE.BOOLEAN:
                    return (short)(((bool)Value.Value) ? 1 : 0);
                case VARIED_VALUE_TYPE.INT8:
                    return (short)((sbyte)Value.Value);
                case VARIED_VALUE_TYPE.UINT8:
                    return (short)((byte)Value.Value);
                case VARIED_VALUE_TYPE.INT16:
                    return ((short)Value.Value);
                case VARIED_VALUE_TYPE.UINT16:
                    return (short)((ushort)Value.Value);
                case VARIED_VALUE_TYPE.INT32:
                    return (short)((int)Value.Value);
                case VARIED_VALUE_TYPE.UINT32:
                    return (short)((uint)Value.Value);
                case VARIED_VALUE_TYPE.INT64:
                    return (short)((long)Value.Value);
                case VARIED_VALUE_TYPE.UINT64:
                    return (short)((ulong)Value.Value);
                case VARIED_VALUE_TYPE.FLOAT32:
                    return (short)((float)Value.Value);
                case VARIED_VALUE_TYPE.FLOAT64:
                    return (short)((double)Value.Value);
            }
            return 0;
        }


        public static implicit operator ushort(CVariedValue Value)
        {
            switch (Value.Type)
            {
                case VARIED_VALUE_TYPE.BOOLEAN:
                    return (ushort)(((bool)Value.Value) ? 1 : 0);
                case VARIED_VALUE_TYPE.INT8:
                    return (ushort)((sbyte)Value.Value);
                case VARIED_VALUE_TYPE.UINT8:
                    return (ushort)((byte)Value.Value);
                case VARIED_VALUE_TYPE.INT16:
                    return (ushort)((short)Value.Value);
                case VARIED_VALUE_TYPE.UINT16:
                    return ((ushort)Value.Value);
                case VARIED_VALUE_TYPE.INT32:
                    return (ushort)((int)Value.Value);
                case VARIED_VALUE_TYPE.UINT32:
                    return (ushort)((uint)Value.Value);
                case VARIED_VALUE_TYPE.INT64:
                    return (ushort)((long)Value.Value);
                case VARIED_VALUE_TYPE.UINT64:
                    return (ushort)((ulong)Value.Value);
                case VARIED_VALUE_TYPE.FLOAT32:
                    return (ushort)((float)Value.Value);
                case VARIED_VALUE_TYPE.FLOAT64:
                    return (ushort)((double)Value.Value);
            }
            return 0;
        }


        public static implicit operator int(CVariedValue Value)
        {
            switch (Value.Type)
            {
                case VARIED_VALUE_TYPE.BOOLEAN:
                    return (int)(((bool)Value.Value) ? 1 : 0);
                case VARIED_VALUE_TYPE.INT8:
                    return (int)((sbyte)Value.Value);
                case VARIED_VALUE_TYPE.UINT8:
                    return (int)((byte)Value.Value);
                case VARIED_VALUE_TYPE.INT16:
                    return (int)((short)Value.Value);
                case VARIED_VALUE_TYPE.UINT16:
                    return (int)((ushort)Value.Value);
                case VARIED_VALUE_TYPE.INT32:
                    return ((int)Value.Value);
                case VARIED_VALUE_TYPE.UINT32:
                    return (int)((uint)Value.Value);
                case VARIED_VALUE_TYPE.INT64:
                    return (int)((long)Value.Value);
                case VARIED_VALUE_TYPE.UINT64:
                    return (int)((ulong)Value.Value);
                case VARIED_VALUE_TYPE.FLOAT32:
                    return (int)((float)Value.Value);
                case VARIED_VALUE_TYPE.FLOAT64:
                    return (int)((double)Value.Value);
            }
            return 0;
        }


        public static implicit operator uint(CVariedValue Value)
        {
            switch (Value.Type)
            {
                case VARIED_VALUE_TYPE.BOOLEAN:
                    return (uint)(((bool)Value.Value) ? 1 : 0);
                case VARIED_VALUE_TYPE.INT8:
                    return (uint)((sbyte)Value.Value);
                case VARIED_VALUE_TYPE.UINT8:
                    return (uint)((byte)Value.Value);
                case VARIED_VALUE_TYPE.INT16:
                    return (uint)((short)Value.Value);
                case VARIED_VALUE_TYPE.UINT16:
                    return (uint)((ushort)Value.Value);
                case VARIED_VALUE_TYPE.INT32:
                    return (uint)((int)Value.Value);
                case VARIED_VALUE_TYPE.UINT32:
                    return ((uint)Value.Value);
                case VARIED_VALUE_TYPE.INT64:
                    return (uint)((long)Value.Value);
                case VARIED_VALUE_TYPE.UINT64:
                    return (uint)((ulong)Value.Value);
                case VARIED_VALUE_TYPE.FLOAT32:
                    return (uint)((float)Value.Value);
                case VARIED_VALUE_TYPE.FLOAT64:
                    return (uint)((double)Value.Value);
            }
            return 0;
        }

        public static implicit operator long(CVariedValue Value)
        {
            switch (Value.Type)
            {
                case VARIED_VALUE_TYPE.BOOLEAN:
                    return (long)(((bool)Value.Value) ? 1 : 0);
                case VARIED_VALUE_TYPE.INT8:
                    return (long)((sbyte)Value.Value);
                case VARIED_VALUE_TYPE.UINT8:
                    return (long)((byte)Value.Value);
                case VARIED_VALUE_TYPE.INT16:
                    return (long)((short)Value.Value);
                case VARIED_VALUE_TYPE.UINT16:
                    return (long)((ushort)Value.Value);
                case VARIED_VALUE_TYPE.INT32:
                    return (long)((int)Value.Value);
                case VARIED_VALUE_TYPE.UINT32:
                    return (long)((uint)Value.Value);
                case VARIED_VALUE_TYPE.INT64:
                    return ((long)Value.Value);
                case VARIED_VALUE_TYPE.UINT64:
                    return (long)((ulong)Value.Value);
                case VARIED_VALUE_TYPE.FLOAT32:
                    return (long)((float)Value.Value);
                case VARIED_VALUE_TYPE.FLOAT64:
                    return (long)((double)Value.Value);
            }
            return 0;
        }


        public static implicit operator ulong(CVariedValue Value)
        {
            switch (Value.Type)
            {
                case VARIED_VALUE_TYPE.BOOLEAN:
                    return (ulong)(((bool)Value.Value) ? 1 : 0);
                case VARIED_VALUE_TYPE.INT8:
                    return (ulong)((sbyte)Value.Value);
                case VARIED_VALUE_TYPE.UINT8:
                    return (ulong)((byte)Value.Value);
                case VARIED_VALUE_TYPE.INT16:
                    return (ulong)((short)Value.Value);
                case VARIED_VALUE_TYPE.UINT16:
                    return (ulong)((ushort)Value.Value);
                case VARIED_VALUE_TYPE.INT32:
                    return (ulong)((int)Value.Value);
                case VARIED_VALUE_TYPE.UINT32:
                    return (ulong)((uint)Value.Value);
                case VARIED_VALUE_TYPE.INT64:
                    return (ulong)((long)Value.Value);
                case VARIED_VALUE_TYPE.UINT64:
                    return ((ulong)Value.Value);
                case VARIED_VALUE_TYPE.FLOAT32:
                    return (ulong)((float)Value.Value);
                case VARIED_VALUE_TYPE.FLOAT64:
                    return (ulong)((double)Value.Value);
            }
            return 0;
        }


        public static implicit operator float(CVariedValue Value)
        {
            switch (Value.Type)
            {
                case VARIED_VALUE_TYPE.BOOLEAN:
                    return (float)(((bool)Value.Value) ? 1 : 0);
                case VARIED_VALUE_TYPE.INT8:
                    return (float)((sbyte)Value.Value);
                case VARIED_VALUE_TYPE.UINT8:
                    return (float)((byte)Value.Value);
                case VARIED_VALUE_TYPE.INT16:
                    return (float)((short)Value.Value);
                case VARIED_VALUE_TYPE.UINT16:
                    return (float)((ushort)Value.Value);
                case VARIED_VALUE_TYPE.INT32:
                    return (float)((int)Value.Value);
                case VARIED_VALUE_TYPE.UINT32:
                    return (float)((uint)Value.Value);
                case VARIED_VALUE_TYPE.INT64:
                    return (float)((long)Value.Value);
                case VARIED_VALUE_TYPE.UINT64:
                    return (float)((ulong)Value.Value);
                case VARIED_VALUE_TYPE.FLOAT32:
                    return ((float)Value.Value);
                case VARIED_VALUE_TYPE.FLOAT64:
                    return (float)((double)Value.Value);
            }
            return 0;
        }


        public static implicit operator double(CVariedValue Value)
        {
            switch (Value.Type)
            {
                case VARIED_VALUE_TYPE.BOOLEAN:
                    return (double)(((bool)Value.Value) ? 1 : 0);
                case VARIED_VALUE_TYPE.INT8:
                    return (double)((sbyte)Value.Value);
                case VARIED_VALUE_TYPE.UINT8:
                    return (double)((byte)Value.Value);
                case VARIED_VALUE_TYPE.INT16:
                    return (double)((short)Value.Value);
                case VARIED_VALUE_TYPE.UINT16:
                    return (double)((ushort)Value.Value);
                case VARIED_VALUE_TYPE.INT32:
                    return (double)((int)Value.Value);
                case VARIED_VALUE_TYPE.UINT32:
                    return (double)((uint)Value.Value);
                case VARIED_VALUE_TYPE.INT64:
                    return (double)((long)Value.Value);
                case VARIED_VALUE_TYPE.UINT64:
                    return (double)((ulong)Value.Value);
                case VARIED_VALUE_TYPE.FLOAT32:
                    return (double)((float)Value.Value);
                case VARIED_VALUE_TYPE.FLOAT64:
                    return ((double)Value.Value);
            }
            return 0;
        }


        public static implicit operator string(CVariedValue Value)
        {
            if (Value.Type == VARIED_VALUE_TYPE.STRING)
                return (string)Value.Value;
            return "";
        }

        public static implicit operator byte[](CVariedValue Value)
        {
            if (Value.Type == VARIED_VALUE_TYPE.BINARY)
                return (byte[])Value.Value;
            return new byte[0];
        }

        public uint GetLength()
        {
            switch (m_Type)
            {
                case VARIED_VALUE_TYPE.BOOLEAN:
                    return sizeof(bool);
                case VARIED_VALUE_TYPE.INT8:
                case VARIED_VALUE_TYPE.UINT8:
                    return sizeof(char);
                case VARIED_VALUE_TYPE.INT16:
                case VARIED_VALUE_TYPE.UINT16:
                    return sizeof(short);
                case VARIED_VALUE_TYPE.INT32:
                case VARIED_VALUE_TYPE.UINT32:
                    return sizeof(int);
                case VARIED_VALUE_TYPE.INT64:
                case VARIED_VALUE_TYPE.UINT64:
                    return sizeof(long);
                case VARIED_VALUE_TYPE.FLOAT32:
                    return sizeof(float);
                case VARIED_VALUE_TYPE.FLOAT64:
                    return sizeof(double);
                case VARIED_VALUE_TYPE.STRING:
                    return (uint)((string)m_Value).Length;
                case VARIED_VALUE_TYPE.BINARY:
                    return (uint)((byte[])m_Value).Length;
                case VARIED_VALUE_TYPE.ARRAY:
                    return (uint)((List<CVariedValue>)m_Value).Count;
                case VARIED_VALUE_TYPE.TABLE:
                    return (uint)((Dictionary<CVariedValue, CVariedValue>)m_Value).Count;
            }
            return 0;
        }

        public static CVariedValue FromBinary(byte[] Value, uint StartIndex, uint Len)
        {
            if (StartIndex + Len < (uint)Value.Length)
            {
                CVariedValue Bin = new CVariedValue(VARIED_VALUE_TYPE.BINARY, new byte[Len]);
                Array.Copy(Value, (int)StartIndex, (byte[])Bin.m_Value, 0, (int)Len);
            }
            return new CVariedValue();
        }
        public static CVariedValue CreateArray(int capacity = 16)
        {
            return new CVariedValue(VARIED_VALUE_TYPE.ARRAY, new List<CVariedValue>(capacity));
        }
        public static CVariedValue CreateTable(int capacity = 16)
        {
            return new CVariedValue(VARIED_VALUE_TYPE.TABLE, new Dictionary<CVariedValue, CVariedValue>(capacity));
        }
        

        public CVariedValue GetChild(CVariedValue Key)
        {
            switch (m_Type)
            {
                case VARIED_VALUE_TYPE.ARRAY:
                    int Index = Key;
                    List<CVariedValue> Array = (List<CVariedValue>)m_Value;
                    if (Index < Array.Count)
                        return Array[Index];
                    break;
                case VARIED_VALUE_TYPE.TABLE:
                    Dictionary<CVariedValue, CVariedValue> Table = (Dictionary<CVariedValue, CVariedValue>)m_Value;
                    CVariedValue Value;
                    if (Table.TryGetValue(Key, out Value))
                    {
                        return Value;
                    }
                    break;
            }
            return null;
        }
        public bool AddChild(CVariedValue Data)
        {
            if (m_Type == VARIED_VALUE_TYPE.ARRAY)
            {
                List<CVariedValue> Array = (List<CVariedValue>)m_Value;
                Array.Add(Data);
                return true;
            }
            return false;
        }
        public bool AddChild(CVariedValue Key, CVariedValue Data)
        {
            switch (m_Type)
            {
                case VARIED_VALUE_TYPE.ARRAY:
                    int Index = Key;
                    List<CVariedValue> Array = (List<CVariedValue>)m_Value;
                    Array.Insert(Index, Data);
                    return true;
                case VARIED_VALUE_TYPE.TABLE:
                    Dictionary<CVariedValue, CVariedValue> Table = (Dictionary<CVariedValue, CVariedValue>)m_Value;
                    Table.Remove(Key);
                    Table.Add(Key, Data);
                    return true;
            }
            return false;
        }

        public override string ToString()
        {
            switch (m_Type)
            {
                case VARIED_VALUE_TYPE.BOOLEAN:
                    return ((bool)m_Value) ? "true" : "false";
                case VARIED_VALUE_TYPE.INT8:
                    return ((sbyte)m_Value).ToString();
                case VARIED_VALUE_TYPE.UINT8:
                    return ((byte)m_Value).ToString();
                case VARIED_VALUE_TYPE.INT16:
                    return ((short)m_Value).ToString();
                case VARIED_VALUE_TYPE.UINT16:
                    return ((ushort)m_Value).ToString();
                case VARIED_VALUE_TYPE.INT32:
                    return ((int)m_Value).ToString();
                case VARIED_VALUE_TYPE.UINT32:
                    return ((uint)m_Value).ToString();
                case VARIED_VALUE_TYPE.INT64:
                    return ((long)m_Value).ToString();
                case VARIED_VALUE_TYPE.UINT64:
                    return ((ulong)m_Value).ToString();
                case VARIED_VALUE_TYPE.FLOAT32:
                    return ((float)m_Value).ToString();
                case VARIED_VALUE_TYPE.FLOAT64:
                    return ((double)m_Value).ToString();
                case VARIED_VALUE_TYPE.STRING:
                    return $"\"{((string)m_Value)}\"";
                case VARIED_VALUE_TYPE.BINARY:
                    return $"\"@B64{Convert.ToBase64String((byte[])m_Value)}\"";
                case VARIED_VALUE_TYPE.ARRAY:                    
                case VARIED_VALUE_TYPE.TABLE:
                    JToken JsonValue = ToJson();
                    return JsonValue.ToString(Formatting.None);
                default:
                    return "null";
            }
        }
        public JToken ToJson()
        {
            switch(m_Type)
            {
                case VARIED_VALUE_TYPE.BOOLEAN:
                    return new JValue((bool)m_Value);
                case VARIED_VALUE_TYPE.INT8:
                    return new JValue((sbyte)m_Value);
                case VARIED_VALUE_TYPE.UINT8:
                    return new JValue((byte)m_Value);
                case VARIED_VALUE_TYPE.INT16:
                    return new JValue((short)m_Value);
                case VARIED_VALUE_TYPE.UINT16:
                    return new JValue((ushort)m_Value);
                case VARIED_VALUE_TYPE.INT32:
                    return new JValue((int)m_Value);
                case VARIED_VALUE_TYPE.UINT32:
                    return new JValue((uint)m_Value);
                case VARIED_VALUE_TYPE.INT64:
                    return new JValue((long)m_Value);
                case VARIED_VALUE_TYPE.UINT64:
                    return new JValue((ulong)m_Value);
                case VARIED_VALUE_TYPE.FLOAT32:
                    return new JValue((float)m_Value);
                case VARIED_VALUE_TYPE.FLOAT64:
                    return new JValue((double)m_Value);
                case VARIED_VALUE_TYPE.STRING:
                    return new JValue((string)m_Value);
                case VARIED_VALUE_TYPE.BINARY:
                    return new JValue(Convert.ToBase64String((byte[])m_Value));
                case VARIED_VALUE_TYPE.ARRAY:
                    {
                        JArray JsonValue = new JArray();
                        foreach (CVariedValue Child in this)
                        {
                            JToken ChildNode = Child.ToJson();
                            if (ChildNode != null)
                                JsonValue.Add(ChildNode);
                        }
                        return JsonValue;
                    }
                case VARIED_VALUE_TYPE.TABLE:
                    {
                        JObject JsonValue = new JObject();
                        foreach (KeyValuePair<CVariedValue, CVariedValue> pair in this)
                        {
                            JToken KeyNode = pair.Key.ToJson();
                            JToken ValueNode = pair.Value.ToJson();
                            if (KeyNode != null && ValueNode != null)
                                JsonValue.Add((string)KeyNode, ValueNode);
                        }
                        return JsonValue;
                    }
            }
            return new JValue((object)null);
        }
        public static CVariedValue FromString(string Str, int MinByte = 1)
        {
            Str = Str.Trim();
            if (Str == "null")
            {
                return new CVariedValue();
            }
            else if (Str == "true")
            {
                return true;
            }
            else if (Str == "false")
            {
                return false;
            }
            else if (Str.StartsWith("\""))
            {
                Str = Str.Trim('"');
                if(Str.StartsWith("@B64"))
                {
                    return Convert.FromBase64String(Str.Substring(4, Str.Length - 4));
                }
                else
                {
                    return Str;
                }
            }            
            else if (Str.StartsWith("{") && Str.EndsWith("}"))
            {
                try
                {
                    JToken JsonValue = JObject.Parse(Str);
                    return FromJson(JsonValue);
                }
                catch(JsonReaderException e)
                {
                    
                }                
            }
            else if (Str.StartsWith("[") && Str.EndsWith("]"))
            {
                try
                {
                    JToken JsonValue = JArray.Parse(Str);
                    return FromJson(JsonValue);
                }
                catch (JsonReaderException e)
                {

                }
            }
            else if (Str.IndexOf('.') >= 0 || Str.IndexOf('e') >= 0 || Str.IndexOf('E') >= 0)
            {
                double Value = 0;
                if (double.TryParse(Str, out Value))
                {
                    return Value;
                }

            }
            else if (Str.IndexOf('-') >= 0)
            {
                long Value = 0;
                if (long.TryParse(Str, out Value))
                {
                    ulong Positive = (ulong)(-Value);
                    if (MinByte <= 1 && Positive <= 0x7F)
                        return (sbyte)Value;
                    else if (MinByte <= 2 && Positive <= 0x7FFF)
                        return (short)Value;
                    else if (MinByte <= 4 && Positive <= 0x7FFFFFFF)
                        return (int)Value;
                    else
                        return Value;
                }

            }
            else
            {
                ulong Value = 0;
                if (ulong.TryParse(Str, out Value))
                {
                    if (MinByte <= 1 && Value <= 0xFF)
                        return (byte)Value;
                    else if (MinByte <= 2 && Value <= 0xFFFF)
                        return (ushort)Value;
                    else if (MinByte <= 4 && Value <= 0xFFFFFFFF)
                        return (uint)Value;
                    else
                        return Value;
                }
                
            }
            return null;
        }

        public static CVariedValue FromJson(JToken JsonValue)
        {
            switch(JsonValue.Type)
            {
                case JTokenType.Object:
                    {
                        CVariedValue Value = CVariedValue.CreateTable((JsonValue as JObject).Count);
                        foreach (var Pair in JsonValue as JObject)
                        {
                            Value.AddChild(FromJson(Pair.Key), FromJson(Pair.Value));
                        }
                        return Value;
                    }
                case JTokenType.Array:
                    {
                        CVariedValue Value = CVariedValue.CreateArray((JsonValue as JArray).Count);
                        foreach (JToken ArrayElement in JsonValue)
                        {
                            Value.AddChild(FromJson(ArrayElement));
                        }
                        return Value;
                    }
                case JTokenType.Integer:
                    return (long)JsonValue;
                case JTokenType.Float:
                    return (double)JsonValue;
                case JTokenType.String:
                    return (string)JsonValue;
                case JTokenType.Boolean:
                    return (bool)JsonValue;
            }
            return new CVariedValue();
        }
    }
}