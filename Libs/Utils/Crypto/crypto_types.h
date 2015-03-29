#pragma once

typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;


typedef unsigned long long __u64;
typedef unsigned int __u32;
typedef unsigned short __u16;
typedef unsigned char __u8;

typedef unsigned long long __le64;
typedef unsigned int __le32;
typedef unsigned short __le16;
typedef unsigned char __le8;


/* Swap bytes in 16 bit value.  */
#define __bswap_constant_16(x) \
     ((unsigned short int) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)))
/* Swap bytes in 32 bit value.  */
#define __bswap_constant_32(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) |		      \
      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))
/* Swap bytes in 64 bit value.  */
# define __bswap_constant_64(x) \
     ((((x) & 0xff00000000000000ull) >> 56)				      \
      | (((x) & 0x00ff000000000000ull) >> 40)				      \
      | (((x) & 0x0000ff0000000000ull) >> 24)				      \
      | (((x) & 0x000000ff00000000ull) >> 8)				      \
      | (((x) & 0x00000000ff000000ull) << 8)				      \
      | (((x) & 0x0000000000ff0000ull) << 24)				      \
      | (((x) & 0x000000000000ff00ull) << 40)				      \
      | (((x) & 0x00000000000000ffull) << 56))
# if __BYTE_ORDER == __LITTLE_ENDIAN
#define cpu_to_le16(v16) (v16)
#define cpu_to_le32(v32) (v32)
#define cpu_to_le64(v64) (v64)
#else
#define cpu_to_le16(v16) __bswap_constant_16(v16)
#define cpu_to_le32(v32) __bswap_constant_32(v32)
#define cpu_to_le64(v64) __bswap_constant_64(v64)
#endif
# if __BYTE_ORDER == __LITTLE_ENDIAN
#define le16_to_cpu(v16) (v16)
#define le32_to_cpu(v32) (v32)
#define le64_to_cpu(v64) (v64)
#else
#define le16_to_cpu(v16) __bswap_constant_16(v16)
#define le32_to_cpu(v32) __bswap_constant_32(v32)
#define le64_to_cpu(v64) __bswap_constant_64(v64)
#endif


