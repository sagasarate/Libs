using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DOSSystem
{
    public class HashMD5
    {
        class MD5_CTX
        {
            public uint[] I = new uint[2];                   /* number of _bits_ handled mod 2^64 */
            public uint[] Buf = new uint[4];                                    /* scratch buffer */
            public byte[] In = new byte[64];                              /* input buffer */
            public byte[] digest = new byte[16];     /* actual digest after MD5Final call */
        };

        static byte[] PADDING = new byte[64] {
          0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        const int S11 = 7;
        const int S12 = 12;
        const int S13 = 17;
        const int S14 = 22;
        const int S21 = 5;
        const int S22 = 9;
        const int S23 = 14;
        const int S24 = 20;
        const int S31 = 4;
        const int S32 = 11;
        const int S33 = 16;
        const int S34 = 23;
        const int S41 = 6;
        const int S42 = 10;
        const int S43 = 15;
        const int S44 = 21;

        MD5_CTX m_mdContext = new MD5_CTX();

        public HashMD5()
        {
            Init();
        }
        public void Init()
        {
            m_mdContext.I[0] = 0;
            m_mdContext.I[1] = 0;

            /* Load magic initialization constants.
            */
            m_mdContext.Buf[0] = 0x67452301;
            m_mdContext.Buf[1] = 0xefcdab89;
            m_mdContext.Buf[2] = 0x98badcfe;
            m_mdContext.Buf[3] = 0x10325476;
        }
        public void AddData(byte[] Data, int StartIndex = 0, int DataLen = 0)
        {
            if (Data == null)
                return;

            if (DataLen == 0)
                DataLen = Data.Length;

            if (Data.Length < StartIndex + DataLen)
                return;

            uint[] In = new uint[16];
            int mdi;
            uint i, ii;

            /* compute number of bytes mod 64 */
            mdi = (int)((m_mdContext.I[0] >> 3) & 0x3F);

            /* update number of bits */
            if ((m_mdContext.I[0] + ((uint)DataLen << 3)) < m_mdContext.I[0])
                m_mdContext.I[1]++;
            m_mdContext.I[0] += ((uint)DataLen << 3);
            m_mdContext.I[1] += ((uint)DataLen >> 29);

            for (int j = 0; j < DataLen; j++)
            {
                /* add new character to buffer, increment mdi */
                m_mdContext.In[mdi++] = Data[j + StartIndex];

                /* transform if necessary */
                if (mdi == 0x40)
                {
                    for (i = 0, ii = 0; i < 16; i++, ii += 4)
                        In[i] = (((uint)m_mdContext.In[ii + 3]) << 24) |
                                (((uint)m_mdContext.In[ii + 2]) << 16) |
                                (((uint)m_mdContext.In[ii + 1]) << 8) |
                                ((uint)m_mdContext.In[ii]);
                    Transform(m_mdContext.Buf, In);
                    mdi = 0;
                }
            }
        }
	    public void MD5Final()
        {
            uint[] In = new uint[16];
            int mdi;
            uint i, ii;
            int padLen;

            /* save number of bits */
            In[14] = m_mdContext.I[0];
            In[15] = m_mdContext.I[1];

            /* compute number of bytes mod 64 */
            mdi = (int)((m_mdContext.I[0] >> 3) & 0x3F);

            /* pad out to 56 mod 64 */
            padLen = (mdi < 56) ? (56 - mdi) : (120 - mdi);
            AddData(PADDING, 0, padLen);

            /* append length in bits and transform */
            for (i = 0, ii = 0; i < 14; i++, ii += 4)
            {
                In[i] = (((uint)m_mdContext.In[ii + 3]) << 24) |
                    (((uint)m_mdContext.In[ii + 2]) << 16) |
                    (((uint)m_mdContext.In[ii + 1]) << 8) |
                    ((uint)m_mdContext.In[ii]);
            }
	
            Transform(m_mdContext.Buf, In);

            /* store buffer in digest */
            for (i = 0, ii = 0; i < 4; i++, ii += 4)
            {
                m_mdContext.digest[ii] = (byte)(m_mdContext.Buf[i] & 0xFF);
            m_mdContext.digest[ii + 1] =
                (byte)((m_mdContext.Buf[i] >> 8) & 0xFF);
            m_mdContext.digest[ii + 2] =
                (byte)((m_mdContext.Buf[i] >> 16) & 0xFF);
            m_mdContext.digest[ii + 3] =
                (byte)((m_mdContext.Buf[i] >> 24) & 0xFF);
        }
    }
        public string GetMD5CodeString()
        {
            string HashCode = string.Empty;
            int i;

            for (i = 0; i < 16; i++)
            {
                HashCode += $"{m_mdContext.digest[i]:x2}";
            }
            return HashCode;
        }
        public byte[] GetMD5Code()
        {
            return m_mdContext.digest;
        }
	    void Transform(uint[] Buf, uint[] In)
        {
            uint a = Buf[0], b = Buf[1], c = Buf[2], d = Buf[3];

            /* Round 1 */

            FF(ref a, b, c, d, In[0], S11, 3614090360); /* 1 */
            FF(ref d, a, b, c, In[1], S12, 3905402710); /* 2 */
            FF(ref c, d, a, b, In[2], S13, 606105819); /* 3 */
            FF(ref b, c, d, a, In[3], S14, 3250441966); /* 4 */
            FF(ref a, b, c, d, In[4], S11, 4118548399); /* 5 */
            FF(ref d, a, b, c, In[5], S12, 1200080426); /* 6 */
            FF(ref c, d, a, b, In[6], S13, 2821735955); /* 7 */
            FF(ref b, c, d, a, In[7], S14, 4249261313); /* 8 */
            FF(ref a, b, c, d, In[8], S11, 1770035416); /* 9 */
            FF(ref d, a, b, c, In[9], S12, 2336552879); /* 10 */
            FF(ref c, d, a, b, In[10], S13, 4294925233); /* 11 */
            FF(ref b, c, d, a, In[11], S14, 2304563134); /* 12 */
            FF(ref a, b, c, d, In[12], S11, 1804603682); /* 13 */
            FF(ref d, a, b, c, In[13], S12, 4254626195); /* 14 */
            FF(ref c, d, a, b, In[14], S13, 2792965006); /* 15 */
            FF(ref b, c, d, a, In[15], S14, 1236535329); /* 16 */

            /* Round 2 */

            GG(ref a, b, c, d, In[1], S21, 4129170786); /* 17 */
            GG(ref d, a, b, c, In[6], S22, 3225465664); /* 18 */
            GG(ref c, d, a, b, In[11], S23, 643717713); /* 19 */
            GG(ref b, c, d, a, In[0], S24, 3921069994); /* 20 */
            GG(ref a, b, c, d, In[5], S21, 3593408605); /* 21 */
            GG(ref d, a, b, c, In[10], S22, 38016083); /* 22 */
            GG(ref c, d, a, b, In[15], S23, 3634488961); /* 23 */
            GG(ref b, c, d, a, In[4], S24, 3889429448); /* 24 */
            GG(ref a, b, c, d, In[9], S21, 568446438); /* 25 */
            GG(ref d, a, b, c, In[14], S22, 3275163606); /* 26 */
            GG(ref c, d, a, b, In[3], S23, 4107603335); /* 27 */
            GG(ref b, c, d, a, In[8], S24, 1163531501); /* 28 */
            GG(ref a, b, c, d, In[13], S21, 2850285829); /* 29 */
            GG(ref d, a, b, c, In[2], S22, 4243563512); /* 30 */
            GG(ref c, d, a, b, In[7], S23, 1735328473); /* 31 */
            GG(ref b, c, d, a, In[12], S24, 2368359562); /* 32 */

            /* Round 3 */

            HH(ref a, b, c, d, In[5], S31, 4294588738); /* 33 */
            HH(ref d, a, b, c, In[8], S32, 2272392833); /* 34 */
            HH(ref c, d, a, b, In[11], S33, 1839030562); /* 35 */
            HH(ref b, c, d, a, In[14], S34, 4259657740); /* 36 */
            HH(ref a, b, c, d, In[1], S31, 2763975236); /* 37 */
            HH(ref d, a, b, c, In[4], S32, 1272893353); /* 38 */
            HH(ref c, d, a, b, In[7], S33, 4139469664); /* 39 */
            HH(ref b, c, d, a, In[10], S34, 3200236656); /* 40 */
            HH(ref a, b, c, d, In[13], S31, 681279174); /* 41 */
            HH(ref d, a, b, c, In[0], S32, 3936430074); /* 42 */
            HH(ref c, d, a, b, In[3], S33, 3572445317); /* 43 */
            HH(ref b, c, d, a, In[6], S34, 76029189); /* 44 */
            HH(ref a, b, c, d, In[9], S31, 3654602809); /* 45 */
            HH(ref d, a, b, c, In[12], S32, 3873151461); /* 46 */
            HH(ref c, d, a, b, In[15], S33, 530742520); /* 47 */
            HH(ref b, c, d, a, In[2], S34, 3299628645); /* 48 */

            /* Round 4 */

            II(ref a, b, c, d, In[0], S41, 4096336452); /* 49 */
            II(ref d, a, b, c, In[7], S42, 1126891415); /* 50 */
            II(ref c, d, a, b, In[14], S43, 2878612391); /* 51 */
            II(ref b, c, d, a, In[5], S44, 4237533241); /* 52 */
            II(ref a, b, c, d, In[12], S41, 1700485571); /* 53 */
            II(ref d, a, b, c, In[3], S42, 2399980690); /* 54 */
            II(ref c, d, a, b, In[10], S43, 4293915773); /* 55 */
            II(ref b, c, d, a, In[1], S44, 2240044497); /* 56 */
            II(ref a, b, c, d, In[8], S41, 1873313359); /* 57 */
            II(ref d, a, b, c, In[15], S42, 4264355552); /* 58 */
            II(ref c, d, a, b, In[6], S43, 2734768916); /* 59 */
            II(ref b, c, d, a, In[13], S44, 1309151649); /* 60 */
            II(ref a, b, c, d, In[4], S41, 4149444226); /* 61 */
            II(ref d, a, b, c, In[11], S42, 3174756917); /* 62 */
            II(ref c, d, a, b, In[2], S43, 718787259); /* 63 */
            II(ref b, c, d, a, In[9], S44, 3951481745); /* 64 */

            Buf[0] += a;
            Buf[1] += b;
            Buf[2] += c;
            Buf[3] += d;
        }

        static uint F(uint x, uint y, uint z)
        {
            return (((x) & (y)) | ((~x) & (z)));
        }
        static uint G(uint x, uint y, uint z)
        {
            return (((x) & (z)) | ((y) & (~z)));
        }
        static uint H(uint x, uint y, uint z)
        {
            return ((x) ^ (y) ^ (z));
        }
        static uint I(uint x, uint y, uint z)
        {
            return ((y) ^ ((x) | (~z)));
        }

        static void FF(ref uint a, uint b, uint c, uint d, uint mj, int s, uint ti)
        {
            a = a + F(b, c, d) + mj + ti;
            a = a << s | a >> (32 - s);
            a += b;
        }
        static void GG(ref uint a, uint b, uint c, uint d, uint mj, int s, uint ti)
        {
            a = a + G(b, c, d) + mj + ti;
            a = a << s | a >> (32 - s);
            a += b;
        }
        static void HH(ref uint a, uint b, uint c, uint d, uint mj, int s, uint ti)
        {
            a = a + H(b, c, d) + mj + ti;
            a = a << s | a >> (32 - s);
            a += b;
        }
        static void II(ref uint a, uint b, uint c, uint d, uint mj, int s, uint ti)
        {
            a = a + I(b, c, d) + mj + ti;
            a = a << s | a >> (32 - s);
            a += b;
        }
    }
}
