/*
	100% free public domain implementation of the HMAC-SHA1 algorithm
	by Chien-Chung, Chung (Jim Chung) <jimchung1221@gmail.com>
*/


#ifndef __HMAC_SHA1_H__
#define __HMAC_SHA1_H__




class CHMAC_SHA1 : public CSHA1
{
    private:
		BYTE m_ipad[64];
        BYTE m_opad[64];

		char * szReport ;
		char * SHA1_Key ;
		char * AppendBuf1 ;
		char * AppendBuf2 ;


	public:
		
		enum {
			SHA1_DIGEST_LENGTH	= 20,
			SHA1_BLOCK_SIZE		= 64,
			HMAC_BUF_LEN		= 4096
		} ;

		CHMAC_SHA1()
			:szReport(MONITORED_NEW_ARRAY(_T("CHMAC_SHA1"), char, HMAC_BUF_LEN)),
			AppendBuf1(MONITORED_NEW_ARRAY(_T("CHMAC_SHA1"), char, HMAC_BUF_LEN)),
			AppendBuf2(MONITORED_NEW_ARRAY(_T("CHMAC_SHA1"), char, HMAC_BUF_LEN)),
			SHA1_Key(MONITORED_NEW_ARRAY(_T("CHMAC_SHA1"), char, HMAC_BUF_LEN))
		{}

        ~CHMAC_SHA1()
        {
			MONITORED_DELETE_ARRAY(szReport);
			MONITORED_DELETE_ARRAY(AppendBuf1);
			MONITORED_DELETE_ARRAY(AppendBuf2);
			MONITORED_DELETE_ARRAY(SHA1_Key);
        }

		void HMAC_SHA1(BYTE *text, size_t text_len, BYTE *key, size_t key_len, BYTE *digest);
		CEasyString HMAC_SHA1_String(BYTE *text, size_t text_len, BYTE *key, size_t key_len);
};


#endif /* __HMAC_SHA1_H__ */
