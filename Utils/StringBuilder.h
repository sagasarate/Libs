/****************************************************************************/
/*                                                                          */
/*      文件名:    StringBuilder.h                                         */
/*      创建日期:  2022年10月18日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CStringBuilder
{
protected:
	CEasyArray<CEasyString>	m_StrList;
	size_t					m_BlockSize;
public:
	CStringBuilder(size_t BlockSize = 4095)
	{
		m_BlockSize = BlockSize;
	}
	~CStringBuilder()
	{

	}
	void Clear()
	{
		m_StrList.Clear();
	}
	bool Append(LPCTSTR szStr, int StrLen = -1)
	{
		if (StrLen < 0)
			StrLen = (int)CEasyString::GetStrLen(szStr);
		if (StrLen > 0)
		{
			CEasyString& Buffer = PrepareBuffer(StrLen);
			return Buffer.Append(szStr, StrLen, false);
		}
		return true;
	}
	bool AppendFormated(LPCTSTR szFormat, ...)
	{
		va_list	vl;
		va_start(vl, szFormat);
		size_t StrLen = CEasyString::GetFormatLenVL(szFormat, vl);
		CEasyString& Str = PrepareBuffer(StrLen);
		bool Ret = Str.AppendFormatNoExpandVL(szFormat, vl);
		va_end(vl);
		return Ret;
	}	
	CEasyString GetStr()
	{
		size_t TotalLen = GetLength();		
		CEasyString OutStr;
		GetStr(OutStr, 0, TotalLen);		
		return OutStr;
	}
	size_t GetStr(CEasyString& OutStr, size_t StartIndex, size_t MaxLen)
	{
		if (MaxLen == 0)
			return 0;
		size_t TotalLen = 0;	
		size_t OutIndex = 0;
		size_t OutPos = 0;
		size_t OutLen = 0;		
		for (size_t i = 0; i < m_StrList.GetCount(); i++)
		{
			CEasyString& Str = m_StrList[i];
			TotalLen += Str.GetLength();
			if (TotalLen > StartIndex)
			{	
				if (OutLen)
				{
					if (OutLen + Str.GetLength() < MaxLen)
					{
						OutLen += MaxLen;
					}						
					else
					{
						OutLen = MaxLen;
						break;
					}						
				}
				else
				{
					OutIndex = Str.GetLength() - (TotalLen - StartIndex);
					if (OutLen + Str.GetLength() - OutIndex < MaxLen)
					{
						OutLen += Str.GetLength() - OutIndex;
					}
					else
					{
						OutLen = MaxLen;
						break;
					}
				}				
			}
			else
			{
				OutPos++;
			}
		}
		if (OutStr.GetBufferSize() < OutLen + 1)
			OutStr.Resize(OutLen, false);
		else
			OutStr.Empty();
		size_t LeftLen = OutLen;
		for (size_t i = OutPos; (i < m_StrList.GetCount()) && LeftLen; i++)
		{
			CEasyString& Str = m_StrList[i];
			size_t Index, Len;
			if (i == OutPos)
			{
				Index = OutIndex;
				Len = Str.GetLength() - OutIndex;				
			}
			else
			{
				Index = 0;
				Len = Str.GetLength();
			}
			if (Len < LeftLen)
			{
				LeftLen -= Len;
			}
			else
			{
				Len = LeftLen;
				LeftLen = 0;
			}
			OutStr.Append(Str.GetBuffer() + OutIndex, Len, false);

		}
		OutStr.TrimBuffer(OutLen);
		return OutLen;
	}
	size_t GetLength()
	{
		size_t TotalLen = 0;
		for (CEasyString& Str : m_StrList)
		{
			TotalLen += Str.GetLength();
		}
		return TotalLen;
	}	
	size_t GetBlockSize()
	{
		return m_BlockSize;
	}
	bool IsEmpty()
	{
		if (m_StrList.GetCount())
		{
			return m_StrList[0].IsEmpty();
		}
		return true;
	}
protected:
	CEasyString& PrepareBuffer(size_t NeedSize)
	{
		if (m_StrList.GetCount())
		{
			CEasyString* pStr = m_StrList.GetLast();
			if (pStr->GetLength() + NeedSize < pStr->GetBufferSize())
			{
				return *pStr;
			}
		}
		CEasyString* pStr = m_StrList.AddEmpty();
		if (NeedSize < m_BlockSize)
			NeedSize = m_BlockSize;
		pStr->Resize(NeedSize, false);
		return *pStr;
	}
};

