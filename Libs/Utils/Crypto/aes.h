#pragma once

#include "crypto_types.h"

#define AES_MIN_KEY_SIZE	16
#define AES_MAX_KEY_SIZE	32
#define AES_KEYSIZE_128		16
#define AES_KEYSIZE_192		24
#define AES_KEYSIZE_256		32
#define AES_BLOCK_SIZE		16
#define AES_MAX_KEYLENGTH	(15 * 16)
#define AES_MAX_KEYLENGTH_U32	(AES_MAX_KEYLENGTH / sizeof(u32))

/*
 * Please ensure that the first two fields are 16-byte aligned
 * relative to the start of the structure, i.e., don't move them!
 */
struct crypto_aes_ctx {
	u32 key_enc[AES_MAX_KEYLENGTH_U32];
	u32 key_dec[AES_MAX_KEYLENGTH_U32];
	u32 key_length;
};

extern const u32 crypto_ft_tab[4][256];
extern const u32 crypto_fl_tab[4][256];
extern const u32 crypto_it_tab[4][256];
extern const u32 crypto_il_tab[4][256];

int crypto_aes_expand_key(struct crypto_aes_ctx *ctx, const u8 *in_key,
		unsigned int key_len);
/*
 *in point to 16 bytes 
 *out point to 16 bytes
  */
void aes_encrypt(struct crypto_aes_ctx *ctx, u8 *out, const u8 *in);
void aes_decrypt(struct crypto_aes_ctx *ctx, u8 *out, const u8 *in);

