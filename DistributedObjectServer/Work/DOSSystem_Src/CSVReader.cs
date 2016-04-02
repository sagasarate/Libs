using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace DOSSystem
{
    public class CSVReader
    {
        protected Encoding  m_LocalEncoding;
        protected ArrayList m_Records;
        protected ArrayList m_ColumnNames;

        public CSVReader()
        {
            m_LocalEncoding = Encoding.Default;
        }

        public void SetLocalEncoding(Encoding LocalEncoding)
        {
            m_LocalEncoding = LocalEncoding;
        }

        public void Clear()
        {
            m_Records = null;
            m_ColumnNames = null;
        }
        public bool Open(string FileName,bool HaveHeader)
        {
            try
            {
                Encoding FileEncoding = m_LocalEncoding;

                try
                {
                    BinaryReader BinReader = new BinaryReader(File.OpenRead(FileName));

                    byte[] BOM = new byte[3];
                    int Count = BinReader.Read(BOM, 0, 3);
                    BinReader.Close();

                    if (Count >= 3 && BOM[0] == 0xEF && BOM[1] == 0xBB && BOM[2] == 0xBF)
                    {
                        FileEncoding = Encoding.UTF8;
                    }
                    else if (Count >= 2 && BOM[0] == 0xFF && BOM[1] == 0xFE)
                    {
                        FileEncoding = Encoding.Unicode;
                    }
                }
                catch (System.Exception ex)
                {
                    Logger.Log(ex.Message);
                    return false;
                }  
                
                StreamReader FileStream = new StreamReader(FileName, FileEncoding);
                string CSVData = FileStream.ReadToEnd();
                FileStream.Close();


                int RowCount = GetLineCount(CSVData);
                int StartIndex = 0;
                m_ColumnNames = new ArrayList();
                m_Records = new ArrayList();
                if (HaveHeader && RowCount>0)
                {
                    StartIndex = ParseLine(CSVData,StartIndex, m_ColumnNames);
                    RowCount--;
                }
                for (int i = 0; i < RowCount; i++)
                {
                    ArrayList LineRecoed=new ArrayList();
                    StartIndex = ParseLine(CSVData,StartIndex, LineRecoed);
                    m_Records.Add(LineRecoed);
                }

                return true;
            }
            catch (System.Exception ex)
            {
                Logger.Log(ex.Message);
                return false;
            }          
        }

        int GetLineCount(string CSVData)
        {
            int LineCount = 0;
            int LineLen = 0;
	        bool IsInString=false;
            int Ptr = 0;
            while (Ptr < CSVData.Length)
	        {
		        LineLen++;
                if (CSVData[Ptr] == '"')
		        {
			        IsInString=!IsInString;
		        }
                else if ((!IsInString) && (CSVData[Ptr] == '\r' || CSVData[Ptr] == '\n'))
		        {
			        LineCount++;
			        LineLen=0;
                    if (CSVData[Ptr] =='\r' && CSVData[Ptr+1] == '\n')
			        {
                        Ptr++;
			        }
		        }
                Ptr++;
	        }
	        if(LineLen>0)
	        {
		        LineCount++;
	        }
	        return LineCount;
        }

        int ParseLine(string CSVData,int StartIndex,ArrayList LineRecord)
        {
	        bool IsInStr=false;
            int HeadPtr = StartIndex;
            while (StartIndex < CSVData.Length)
	        {
                if (CSVData[StartIndex] == '"')
		        {
			        IsInStr=!IsInStr;
		        }
		        else if(!IsInStr)
		        {
                    if (CSVData[StartIndex] == '\r' || CSVData[StartIndex] == '\n')
			        {
                        if (CSVData[StartIndex] == '\r' && CSVData[StartIndex+1] == '\n')
				        {
                            LineRecord.Add(CSVData.Substring(HeadPtr, StartIndex - HeadPtr));
                            StartIndex += 2;
				        }
				        else
				        {
                            StartIndex++;
				        }				
				        break;
			        }
                    else if (CSVData[StartIndex] == ',')
			        {				        
                        LineRecord.Add(CSVData.Substring(HeadPtr, StartIndex - HeadPtr));
                        HeadPtr = StartIndex + 1;
			        }
		        }
                StartIndex++;
	        }
            for (int i = 0; i < LineRecord.Count; i++)
            {
                string szField = (string)LineRecord[i];                
                if (szField.Length>0)
                {
                    szField=szField.Trim('"');
                    szField=szField.Replace("\"\"", "\"");
                    LineRecord[i] = szField;
                }
            }
            return StartIndex;
        }

        public int GetRowCount()
        {
	        return m_Records.Count;
        }
        public int GetColCount()
        {
	        return m_ColumnNames.Count;
        }

        public string GetDataString(int Row, int Col, string Default)
        {
	        if(Row<m_Records.Count)
	        {
                ArrayList LineRecord=(ArrayList)m_Records[Row];
		        if(Col<LineRecord.Count)
		        {
			        return (string)LineRecord[Col];
		        }
	        }
	        return Default;
        }
        public string GetDataString(int Row, string ColName, string Default)
        {
	        for(int i=0;i<m_ColumnNames.Count;i++)
	        {
		        if(string.Compare((string)m_ColumnNames[i],ColName,true)==0)
		        {
			        return GetDataString(Row,i,Default);
		        }
	        }
	        return Default;
        }

        public int GetDataInt(int Row, int Col, int Default)
        {
	        string StringData=GetDataString(Row,Col,"");
	        if(StringData.Length>0)
	        {
		        return Convert.ToInt32(StringData);
	        }
	        return Default;
        }
        public int GetDataInt(int Row, string ColName, int Default)
        {
	        string StringData=GetDataString(Row,ColName,"");
	         if(StringData.Length>0)
	        {
		        return Convert.ToInt32(StringData);
	        }
	        return Default;
        }

        public long GetDataLong(int Row, int Col, long Default)
        {
            string StringData = GetDataString(Row, Col, "");
            if (StringData.Length > 0)
            {
                return Convert.ToInt64(StringData);
            }
            return Default;
        }
        public long GetDataLong(int Row, string ColName, long Default)
        {
            string StringData = GetDataString(Row, ColName, "");
            if (StringData.Length > 0)
            {
                return Convert.ToInt64(StringData);
            }
            return Default;
        }

        public float GetDataFloat(int Row, int Col, float Default)
        {
            string StringData = GetDataString(Row, Col, "");
            if (StringData.Length > 0)
            {
                return Convert.ToSingle(StringData);
            }
            return Default;
        }
        public float GetDataFloat(int Row, string ColName, float Default)
        {
            string StringData = GetDataString(Row, ColName, "");
            if (StringData.Length > 0)
            {
                return Convert.ToSingle(StringData);
            }
            return Default;
        }

        public double GetDataDouble(int Row, int Col, double Default)
        {
            string StringData = GetDataString(Row, Col, "");
            if (StringData.Length > 0)
            {
                return Convert.ToDouble(StringData);
            }
            return Default;
        }
        public double GetDataDouble(int Row, string ColName, double Default)
        {
            string StringData = GetDataString(Row, ColName, "");
            if (StringData.Length > 0)
            {
                return Convert.ToDouble(StringData);
            }
            return Default;
        }       
      
             
        public bool AddColumn(string ColName)
        {
            if (m_ColumnNames == null)
                m_ColumnNames = new ArrayList();
            m_ColumnNames.Add(ColName);
	        return true;
        }

        public bool AddRow()
        {
            if (m_Records == null)
                m_Records = new ArrayList();
            m_Records.Add(new ArrayList());
	        return true;
        }

        public bool AddDataString(string Data)
        {
            if (m_Records != null && m_Records.Count>0)
            {
                ArrayList LastRow = m_Records[m_Records.Count - 1] as ArrayList;
                LastRow.Add(Data);
            }	        
	        return true;
        }

        public bool AddDataInt(int Data)
        {
	        return AddDataString(Data.ToString());
        }

        public bool AddDataInt64(long Data)
        {
            return AddDataString(Data.ToString());
        }

        public bool AddDataDouble(double Data)
        {
            return AddDataString(Data.ToString());
        }

        public bool AddDataBool(bool Data)
        {
            return AddDataString(Data.ToString());
        }

        public bool Save(string FileName,bool WriteHeader)
        {
            try 
            {
                StreamWriter FileStream = new StreamWriter(FileName, false, m_LocalEncoding);
                try
                {
                    if (WriteHeader)
                    {
                        string Line = string.Empty;
                        foreach (string Value in m_ColumnNames)
                        {
                            string ColValue = Value.Replace("\"", "\"\"");
                            if (ColValue.IndexOf('"') >= 0 || ColValue.IndexOf(',') >= 0 || ColValue.IndexOf('\r') >= 0 || ColValue.IndexOf('\n') >= 0)
                                ColValue = '"' + ColValue + '"';
                            if (Line.Length == 0)
                                Line = ColValue;
                            else
                                Line += ',' + ColValue;
                        }
                        FileStream.WriteLine(Line);
                    }
                    foreach (ArrayList DataLine in m_Records)
                    {
                        string Line = string.Empty;
                        foreach (string Value in DataLine)
                        {
                            string ColValue = Value.Replace("\"", "\"\"");
                            if (ColValue.IndexOf('"') >= 0 || ColValue.IndexOf(',') >= 0 || ColValue.IndexOf('\r') >= 0 || ColValue.IndexOf('\n') >= 0)
                                ColValue = '"' + ColValue + '"';
                            if (Line.Length == 0)
                                Line = ColValue;
                            else
                                Line += ',' + ColValue;
                        }
                        FileStream.WriteLine(Line);
                    }
                    FileStream.Flush();
                    FileStream.Close();
                    return true;
                }
                catch (System.Exception ex)
                {
                    Logger.Log(ex.Message);
                    FileStream.Close();
                    return false;
                }  
            }
            catch (System.Exception ex)
            {
                Logger.Log(ex.Message);
                return false;
            }  
            
        }
    }
}
