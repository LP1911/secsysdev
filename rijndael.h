/*
	Lorenzo Palleschi D24126922
	This header file declares the public interface for 
	the AES-128 encryption and decryption functions.
 */

#ifndef RIJNDAEL_H
#define RIJNDAEL_H

#define BLOCK_ACCESS(block, row, col) (block[(row * 4) + col])
#define BLOCK_SIZE 16

/*
 * These should be the main encrypt/decrypt functions (i.e. the main
 * entry point to the library for programmes hoping to use it to
 * encrypt or decrypt data)
 */
void sub_bytes(unsigned char *block);
void invert_sub_bytes(unsigned char *block);

void shift_rows(unsigned char *block);
void invert_shift_rows(unsigned char *block);

void mix_columns(unsigned char *block);
void invert_mix_columns(unsigned char *block);


unsigned char *aes_encrypt_block(unsigned char *plaintext, unsigned char *key);
unsigned char *aes_decrypt_block(unsigned char *ciphertext, unsigned char *key);

#endif
