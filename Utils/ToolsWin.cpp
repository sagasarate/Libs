#include "stdafx.h"

#pragma pack(push)
#pragma pack(1)

struct FONT_NAME_TABLE_HEADER
{
	WORD	FormatSelector;
	WORD	RecordCount;
	WORD	StringTableOffset;
};

struct FONT_NAME_TABLE_RECORD
{
	WORD	PlatformID;
	WORD	EncodingID;
	WORD	LanguageID;
	WORD	NameID;
	WORD	NameStringLen;
	WORD	NameStringOffset;
};

#pragma pack(pop)

#define SWAPWORD(x) MAKEWORD(HIBYTE(x), LOBYTE(x))
#define SWAPLONG(x) MAKELONG(SWAPWORD(HIWORD(x)), SWAPWORD(LOWORD(x)))

CEasyString GetFontNameEN(LOGFONT * pFont)
{
	CEasyString FontNameEN;
	HDC hdc=GetDC(NULL);
	HFONT hFont=CreateFontIndirect(pFont);
	HGDIOBJ hOldObj=SelectObject(hdc,hFont);	
	UINT DataSize=GetFontData(hdc,'eman',0,NULL,0);
	bool IsHorizontal=pFont->lfFaceName[0]=='@';
	if(DataSize!=GDI_ERROR)
	{
		BYTE * pBuffer = MONITORED_NEW_ARRAY(_T("GetFontNameEN"), BYTE, DataSize);
		if(GetFontData(hdc,'eman',0,pBuffer,DataSize)!=GDI_ERROR)
		{
			FONT_NAME_TABLE_HEADER * pHeader=(FONT_NAME_TABLE_HEADER *)pBuffer;
			pHeader->RecordCount=SWAPWORD(pHeader->RecordCount);
			pHeader->StringTableOffset=SWAPWORD(pHeader->StringTableOffset);
			FONT_NAME_TABLE_RECORD * pRecords=(FONT_NAME_TABLE_RECORD *)(pBuffer+sizeof(FONT_NAME_TABLE_HEADER));
			char * pStringTable=(char *)(pBuffer+pHeader->StringTableOffset);

			
			for(UINT i=0;i<pHeader->RecordCount;i++)
			{
				pRecords[i].PlatformID=SWAPWORD(pRecords[i].PlatformID);
				pRecords[i].EncodingID=SWAPWORD(pRecords[i].EncodingID);
				pRecords[i].LanguageID=SWAPWORD(pRecords[i].LanguageID);
				pRecords[i].NameID=SWAPWORD(pRecords[i].NameID);
				pRecords[i].NameStringLen=SWAPWORD(pRecords[i].NameStringLen);
				pRecords[i].NameStringOffset=SWAPWORD(pRecords[i].NameStringOffset);

				if(pRecords[i].NameID==4)
				{
					switch(pRecords[i].PlatformID)
					{
					case 0://	Apple Unicode	none
						
						break;
					case 1://	Macintosh	Script manager code
						if(pRecords[i].LanguageID==0)  //US English or UK English	
						{
							FontNameEN.SetString(pStringTable+pRecords[i].NameStringOffset,pRecords[i].NameStringLen);							
						}
						break;
					case 2://	ISO	ISO encoding
						if(pRecords[i].EncodingID==0)
						{
							FontNameEN.SetString(pStringTable+pRecords[i].NameStringOffset,pRecords[i].NameStringLen);
						}
						break;
					case 3://	Microsoft 	Microsoft encoding
						if(pRecords[i].LanguageID==0x0409||pRecords[i].LanguageID==0x0809)  //US English or UK English	
						{
							if(pRecords[i].EncodingID==0)
							{
								FontNameEN.SetString(pStringTable+pRecords[i].NameStringOffset,pRecords[i].NameStringLen);								
							}
							else
							{
								WCHAR * pUStr=(WCHAR *)(pStringTable+pRecords[i].NameStringOffset);
								for(UINT c=0;c<(UINT)(pRecords[i].NameStringLen/2);c++)
								{
									pUStr[c]=SWAPWORD(pUStr[c]);
								}
								FontNameEN.SetString(pUStr,pRecords[i].NameStringLen/2);
							}
							PrintSystemLog(_T("%s"),(LPCTSTR)FontNameEN);
						}
						break;
					}
				}
				if(FontNameEN.GetLength())
				{
					if(IsHorizontal)
					{
						FontNameEN='@'+FontNameEN;
					}
					break;
				}
			}
		}
		SAFE_DELETE_ARRAY(pBuffer);
	}
	SelectObject(hdc,hOldObj);
	DeleteObject(hFont);
	int err=GetLastError();

	ReleaseDC(NULL,hdc);

	

	return FontNameEN;
}


CEasyString GetFontNameCN(LOGFONT * pFont)
{
	CEasyString FontNameEN;
	HDC hdc=GetDC(NULL);
	HFONT hFont=CreateFontIndirect(pFont);
	HGDIOBJ hOldObj=SelectObject(hdc,hFont);	
	UINT DataSize=GetFontData(hdc,'eman',0,NULL,0);
	bool IsHorizontal=pFont->lfFaceName[0]=='@';
	if(DataSize!=GDI_ERROR)
	{
		BYTE * pBuffer = MONITORED_NEW_ARRAY(_T("GetFontNameEN"), BYTE, DataSize);
		if(GetFontData(hdc,'eman',0,pBuffer,DataSize)!=GDI_ERROR)
		{
			FONT_NAME_TABLE_HEADER * pHeader=(FONT_NAME_TABLE_HEADER *)pBuffer;
			pHeader->RecordCount=SWAPWORD(pHeader->RecordCount);
			pHeader->StringTableOffset=SWAPWORD(pHeader->StringTableOffset);
			FONT_NAME_TABLE_RECORD * pRecords=(FONT_NAME_TABLE_RECORD *)(pBuffer+sizeof(FONT_NAME_TABLE_HEADER));
			char * pStringTable=(char *)(pBuffer+pHeader->StringTableOffset);


			for(UINT i=0;i<pHeader->RecordCount;i++)
			{
				pRecords[i].PlatformID=SWAPWORD(pRecords[i].PlatformID);
				pRecords[i].EncodingID=SWAPWORD(pRecords[i].EncodingID);
				pRecords[i].LanguageID=SWAPWORD(pRecords[i].LanguageID);
				pRecords[i].NameID=SWAPWORD(pRecords[i].NameID);
				pRecords[i].NameStringLen=SWAPWORD(pRecords[i].NameStringLen);
				pRecords[i].NameStringOffset=SWAPWORD(pRecords[i].NameStringOffset);



				if(pRecords[i].NameID==4)
				{
					switch(pRecords[i].PlatformID)
					{
					case 0://	Apple Unicode	none
						break;
					case 1://	Macintosh	Script manager code						
						break;
					case 2://	ISO	ISO encoding
						
						break;
					case 3://	Microsoft 	Microsoft encoding
						if(pRecords[i].LanguageID==0x0804)  //Simplified Chinese	
						{
							if(pRecords[i].EncodingID==0)
							{
								FontNameEN.SetString(pStringTable+pRecords[i].NameStringOffset,pRecords[i].NameStringLen);
							}
							else
							{
								WCHAR * pUStr=(WCHAR *)(pStringTable+pRecords[i].NameStringOffset);
								for (UINT c = 0; c<(UINT)(pRecords[i].NameStringLen / 2); c++)
								{
									pUStr[c]=SWAPWORD(pUStr[c]);
								}
								FontNameEN.SetString(pUStr,pRecords[i].NameStringLen/2);
							}
						}
						break;
					}
				}
				if(FontNameEN.GetLength())
				{
					if(IsHorizontal)
					{
						FontNameEN='@'+FontNameEN;
					}
					break;
				}
			}
		}
		SAFE_DELETE_ARRAY(pBuffer);
	}
	SelectObject(hdc,hOldObj);
	DeleteObject(hFont);
	int err=GetLastError();

	ReleaseDC(NULL,hdc);



	return FontNameEN;
}