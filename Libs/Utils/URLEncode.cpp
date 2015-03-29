#include "StdAfx.h"

static unsigned char ToHex(unsigned char x) 
{ 
	return  x > 9 ? x + 55 : x + 48; 
}

static unsigned char FromHex(unsigned char x) 
{ 
	unsigned char y;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	else assert(0);
	return y;
}

CEasyStringA URLEncode(LPCSTR str,int Len)
{
	CEasyStringA strTemp = "";
	if(Len<0)
		Len=strlen(str);	
	for (size_t i = 0; i < Len; i++)
	{
		if (isalnum((unsigned char)str[i]) || 
			(str[i] == '-') ||
			(str[i] == '_') || 
			(str[i] == '.') || 
			(str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ')
			strTemp += "+";
		else
		{
			strTemp += '%';
			strTemp += (char)ToHex((unsigned char)str[i] >> 4);
			strTemp += (char)ToHex((unsigned char)str[i] % 16);
		}
	}
	return strTemp;
}

CEasyStringA URLDecode(LPCSTR str,int Len)
{
	CEasyStringA strTemp = "";
	if(Len<0)
		Len=strlen(str);
	for (size_t i = 0; i < Len; i++)
	{
		if (str[i] == '+') strTemp += ' ';
		else if (str[i] == '%')
		{
			assert(i + 2 < Len);
			unsigned char high = FromHex((unsigned char)str[++i]);
			unsigned char low = FromHex((unsigned char)str[++i]);
			strTemp += (char)(high*16 + low);
		}
		else strTemp += str[i];
	}
	return strTemp;
}