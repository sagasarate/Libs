#include "stdafx.h"


static const size_t BASE32_ENCODE_INPUT = 5;
static const size_t BASE32_ENCODE_OUTPUT = 8;
static const char* const BASE32_ENCODE_TABLE = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567=";
static const size_t BASE32_DECODE_INPUT = 8;
static const size_t BASE32_DECODE_OUTPUT = 5;
static const size_t BASE32_DECODE_MAX_PADDING = 6;
static const unsigned char BASE32_DECODE_MAX = 31;
static const unsigned char BASE32_DECODE_TABLE[0x80] = {
	/*00-07*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	/*08-0f*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	/*10-17*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	/*18-1f*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	/*20-27*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	/*28-2f*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	/*30-37*/ 0xFF, 0xFF, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, /*6 = '2'-'7'*/
	/*38-3f*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0xFF, 0xFF, /*1 = '='*/
	/*40-47*/ 0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, /*7 = 'A'-'G'*/
	/*48-4f*/ 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, /*8 = 'H'-'O'*/
	/*50-57*/ 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, /*8 = 'P'-'W'*/
	/*58-5f*/ 0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, /*3 = 'X'-'Z'*/
	/*60-67*/ 0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, /*7 = 'a'-'g' (same as 'A'-'G')*/
	/*68-6f*/ 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, /*8 = 'h'-'o' (same as 'H'-'O')*/
	/*70-77*/ 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, /*8 = 'p'-'w' (same as 'P'-'W')*/
	/*78-7f*/ 0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF  /*3 = 'x'-'z' (same as 'X'-'Z')*/
};





size_t CBase32::base32EncodeGetLength(size_t size)
{
	/*
	* output 8 bytes for every 5 input:
	*                1        2        3        4        5
	* 1 = ---11111 = 11111---
	* 2 = ---111XX = -----111 XX------
	* 3 = ---11111 =          --11111-
	* 4 = ---1XXXX =          -------1 XXXX----
	* 5 = ---1111X =                   ----1111 X-------
	* 6 = ---11111 =                            -11111--
	* 7 = ---11XXX =                            ------11 XXX-----
	* 8 = ---11111 =                                     ---11111
	*/

	return (((size + BASE32_ENCODE_INPUT - 1) / BASE32_ENCODE_INPUT) * BASE32_ENCODE_OUTPUT) + 1; /*plus terminator*/
}

size_t CBase32::base32Encode(char* dest, const void* src, size_t size)
{
	if (dest && src)
	{
		unsigned char* pSrc = (unsigned char*)src;
		size_t dwSrcSize = size;
		size_t dwDestSize = 0;
		size_t dwBlockSize;
		unsigned char n1, n2, n3, n4, n5, n6, n7, n8;

		while (dwSrcSize >= 1)
		{
			/* Encode inputs */
			dwBlockSize = (dwSrcSize < BASE32_ENCODE_INPUT ? dwSrcSize : BASE32_ENCODE_INPUT);
			n1 = n2 = n3 = n4 = n5 = n6 = n7 = n8 = 0;
			switch (dwBlockSize)
			{
			case 5:
				n8 = (pSrc[4] & 0x1f);
				n7 = ((unsigned char)(pSrc[4] & 0xe0) >> 5);
			case 4:
				n7 |= ((unsigned char)(pSrc[3] & 0x03) << 3);
				n6 = ((unsigned char)(pSrc[3] & 0x7c) >> 2);
				n5 = ((unsigned char)(pSrc[3] & 0x80) >> 7);
			case 3:
				n5 |= ((unsigned char)(pSrc[2] & 0x0f) << 1);
				n4 = ((unsigned char)(pSrc[2] & 0xf0) >> 4);
			case 2:
				n4 |= ((unsigned char)(pSrc[1] & 0x01) << 4);
				n3 = ((unsigned char)(pSrc[1] & 0x3e) >> 1);
				n2 = ((unsigned char)(pSrc[1] & 0xc0) >> 6);
			case 1:
				n2 |= ((unsigned char)(pSrc[0] & 0x07) << 2);
				n1 = ((unsigned char)(pSrc[0] & 0xf8) >> 3);
				break;

			default:
				assert(0);
			}
			pSrc += dwBlockSize;
			dwSrcSize -= dwBlockSize;

			/* Validate */
			assert(n1 <= 31);
			assert(n2 <= 31);
			assert(n3 <= 31);
			assert(n4 <= 31);
			assert(n5 <= 31);
			assert(n6 <= 31);
			assert(n7 <= 31);
			assert(n8 <= 31);

			/* Padding */
			switch (dwBlockSize)
			{
			case 1: n3 = n4 = 32;
			case 2: n5 = 32;
			case 3: n6 = n7 = 32;
			case 4: n8 = 32;
			case 5:
				break;

			default:
				assert(0);
			}

			/* 8 outputs */
			*dest++ = BASE32_ENCODE_TABLE[n1];
			*dest++ = BASE32_ENCODE_TABLE[n2];
			*dest++ = BASE32_ENCODE_TABLE[n3];
			*dest++ = BASE32_ENCODE_TABLE[n4];
			*dest++ = BASE32_ENCODE_TABLE[n5];
			*dest++ = BASE32_ENCODE_TABLE[n6];
			*dest++ = BASE32_ENCODE_TABLE[n7];
			*dest++ = BASE32_ENCODE_TABLE[n8];
			dwDestSize += BASE32_ENCODE_OUTPUT;
		}
		*dest++ = '\x0'; /*append terminator*/

		return dwDestSize;
	}
	else
		return 0; /*ERROR - null pointer*/
}

int CBase32::cyoBase32Validate(const char* src, size_t size)
{
	/*
	* returns 0 if the source is a valid base32 encoding
	*/

	if (size % BASE32_DECODE_INPUT != 0)
		return -1; /*ERROR - size isn't a multiple of 8*/

	for (; size >= 1; --size, ++src)
	{
		unsigned char ch = *src;
		if ((ch >= 0x80) || (BASE32_DECODE_TABLE[ch] > BASE32_DECODE_MAX))
			break;
	}
	for (; 1 <= size && size <= BASE32_DECODE_MAX_PADDING; --size, ++src)
	{
		unsigned char ch = *src;
		if ((ch >= 0x80) || (BASE32_DECODE_TABLE[ch] != BASE32_DECODE_MAX + 1))
			break;
	}
	if (size != 0)
		return -2; /*ERROR - invalid base32 character*/

	return 0; /*OK*/
}

size_t CBase32::base32DecodeGetLength(size_t size)
{
	/*
	* output 5 bytes for every 8 input:
	*                1        2        3        4        5
	* 1 = ---11111 = 11111---
	* 2 = ---111XX = -----111 XX------
	* 3 = ---11111 =          --11111-
	* 4 = ---1XXXX =          -------1 XXXX----
	* 5 = ---1111X =                   ----1111 X-------
	* 6 = ---11111 =                            -11111--
	* 7 = ---11XXX =                            ------11 XXX-----
	* 8 = ---11111 =                                     ---11111
	*/

	if (size % BASE32_DECODE_INPUT == 0)
		return (((size + BASE32_DECODE_INPUT - 1) / BASE32_DECODE_INPUT) * BASE32_DECODE_OUTPUT) + 1; /*plus terminator*/
	else
		return 0; /*ERROR - size isn't a multiple of 8*/
}

size_t CBase32::base32Decode(void* dest, const char* src, size_t size)
{
	if (dest && src && (size % BASE32_DECODE_INPUT == 0))
	{
		unsigned char* pDest = (unsigned char*)dest;
		size_t dwSrcSize = size;
		size_t dwDestSize = 0;
		unsigned char in1, in2, in3, in4, in5, in6, in7, in8;

		while (dwSrcSize >= 1)
		{
			/* 8 inputs */
			in1 = *src++;
			in2 = *src++;
			in3 = *src++;
			in4 = *src++;
			in5 = *src++;
			in6 = *src++;
			in7 = *src++;
			in8 = *src++;
			dwSrcSize -= BASE32_DECODE_INPUT;

			/* Validate ascii */
			if (in1 >= 0x80 || in2 >= 0x80 || in3 >= 0x80 || in4 >= 0x80
				|| in5 >= 0x80 || in6 >= 0x80 || in7 >= 0x80 || in8 >= 0x80)
				return 0; /*ERROR - invalid base32 character*/

			/* Convert ascii to base16 */
			in1 = BASE32_DECODE_TABLE[in1];
			in2 = BASE32_DECODE_TABLE[in2];
			in3 = BASE32_DECODE_TABLE[in3];
			in4 = BASE32_DECODE_TABLE[in4];
			in5 = BASE32_DECODE_TABLE[in5];
			in6 = BASE32_DECODE_TABLE[in6];
			in7 = BASE32_DECODE_TABLE[in7];
			in8 = BASE32_DECODE_TABLE[in8];

			/* Validate base32 */
			if (in1 > BASE32_DECODE_MAX || in2 > BASE32_DECODE_MAX)
				return 0; /*ERROR - invalid base32 character*/
			/*the following can be padding*/
			if ((int)in3 > (int)BASE32_DECODE_MAX + 1 || (int)in4 > (int)BASE32_DECODE_MAX + 1 || (int)in5 > (int)BASE32_DECODE_MAX + 1
				|| (int)in6 > (int)BASE32_DECODE_MAX + 1 || (int)in7 > (int)BASE32_DECODE_MAX + 1 || (int)in8 > (int)BASE32_DECODE_MAX + 1)
				return 0; /*ERROR - invalid base32 character*/

			/* 5 outputs */
			*pDest++ = ((unsigned char)(in1 & 0x1f) << 3) | ((unsigned char)(in2 & 0x1c) >> 2);
			*pDest++ = ((unsigned char)(in2 & 0x03) << 6) | ((unsigned char)(in3 & 0x1f) << 1) | ((unsigned char)(in4 & 0x10) >> 4);
			*pDest++ = ((unsigned char)(in4 & 0x0f) << 4) | ((unsigned char)(in5 & 0x1e) >> 1);
			*pDest++ = ((unsigned char)(in5 & 0x01) << 7) | ((unsigned char)(in6 & 0x1f) << 2) | ((unsigned char)(in7 & 0x18) >> 3);
			*pDest++ = ((unsigned char)(in7 & 0x07) << 5) | (in8 & 0x1f);
			dwDestSize += BASE32_DECODE_OUTPUT;

			/* Padding */
			if (in8 == BASE32_DECODE_MAX + 1)
			{
				--dwDestSize;
				assert((in7 == BASE32_DECODE_MAX + 1 && in6 == BASE32_DECODE_MAX + 1) || (in7 != BASE32_DECODE_MAX + 1));
				if (in6 == BASE32_DECODE_MAX + 1)
				{
					--dwDestSize;
					if (in5 == BASE32_DECODE_MAX + 1)
					{
						--dwDestSize;
						assert((in4 == BASE32_DECODE_MAX + 1 && in3 == BASE32_DECODE_MAX + 1) || (in4 != BASE32_DECODE_MAX + 1));
						if (in3 == BASE32_DECODE_MAX + 1)
						{
							--dwDestSize;
						}
					}
				}
			}
		}
		*pDest++ = '\x0'; /*append terminator*/

		return dwDestSize;
	}
	else
		return 0; /*ERROR - null pointer, or size isn't a multiple of 8*/
}




bool CBase32::Encode(const CEasyStringA& InData, CEasyStringA& OutData)
{
	OutData.Resize(GetEncodeLen(InData.GetLength()));	
	size_t OutLen = base32Encode((char *)OutData.GetBuffer(), InData.GetBuffer(), InData.GetLength());
	OutData.SetLength(OutLen);
	return true;
}

bool CBase32::Encode(LPCVOID pInData, size_t DataLen, CEasyStringA& OutData)
{
	OutData.Resize(GetEncodeLen(DataLen));	
	size_t OutLen = base32Encode((char *)OutData.GetBuffer(), pInData, DataLen);
	OutData.SetLength(OutLen);
	return true;
}

CEasyStringA CBase32::Encode(const CEasyStringA& InData)
{
	CEasyStringA OutData;
	OutData.Resize(GetEncodeLen(InData.GetLength()));	
	size_t OutLen = base32Encode((char *)OutData.GetBuffer(), InData.GetBuffer(), InData.GetLength());
	OutData.SetLength(OutLen);
	return OutData;
}
CEasyStringA CBase32::Encode(LPCVOID pInData, size_t DataLen)
{
	CEasyStringA OutData;
	OutData.Resize(GetEncodeLen(DataLen));
	size_t OutLen = base32Encode((char *)OutData.GetBuffer(), pInData, DataLen);
	OutData.SetLength(OutLen);
	return OutData;
}


bool CBase32::Decode(const CEasyStringA& InData, CEasyBuffer& OutData)
{
	if (cyoBase32Validate(InData, InData.GetLength()))
		return false;
	OutData.Create((UINT)GetDecodeLen(InData, InData.GetLength()));
	size_t OutLen = base32Decode(OutData.GetBuffer(), InData.GetBuffer(), InData.GetLength());	
	OutData.SetUsedSize((UINT)OutLen);
	return true;
}

bool CBase32::Decode(const CEasyStringA& InData, BYTE * pOutData, size_t& OutLen)
{
	return Decode(InData, InData.GetLength(), pOutData, OutLen);
}

bool  CBase32::Decode(LPCSTR szInData, size_t InLen, CEasyBuffer& OutData)
{
	if (cyoBase32Validate(szInData, InLen))
		return false;
	OutData.Create((UINT)GetDecodeLen(szInData, InLen));
	size_t OutLen = base32Decode(OutData.GetBuffer(), szInData, InLen);
	OutData.SetUsedSize((UINT)OutLen);
	return true;
}