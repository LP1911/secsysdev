/*
    Lorenzo Palleschi D24126922
    This file contains the actual implementation
    of AES-128 encryption and decryption,
    matching Python's layout for shift_rows and mix_columns.
*/

#include <stdlib.h>
#include <string.h> 
#include "rijndael.h"


// S-BOX table pulled from python code (AES Values)
// Used as the lookup table within sub_bytes
static const unsigned char s_box[256] = {
    0x63,0x7C,0x77,0x7B,0xF2,0x6B,0x6F,0xC5,0x30,0x01,0x67,0x2B,0xFE,0xD7,0xAB,0x76,
    0xCA,0x82,0xC9,0x7D,0xFA,0x59,0x47,0xF0,0xAD,0xD4,0xA2,0xAF,0x9C,0xA4,0x72,0xC0,
    0xB7,0xFD,0x93,0x26,0x36,0x3F,0xF7,0xCC,0x34,0xA5,0xE5,0xF1,0x71,0xD8,0x31,0x15,
    0x04,0xC7,0x23,0xC3,0x18,0x96,0x05,0x9A,0x07,0x12,0x80,0xE2,0xEB,0x27,0xB2,0x75,
    0x09,0x83,0x2C,0x1A,0x1B,0x6E,0x5A,0xA0,0x52,0x3B,0xD6,0xB3,0x29,0xE3,0x2F,0x84,
    0x53,0xD1,0x00,0xED,0x20,0xFC,0xB1,0x5B,0x6A,0xCB,0xBE,0x39,0x4A,0x4C,0x58,0xCF,
    0xD0,0xEF,0xAA,0xFB,0x43,0x4D,0x33,0x85,0x45,0xF9,0x02,0x7F,0x50,0x3C,0x9F,0xA8,
    0x51,0xA3,0x40,0x8F,0x92,0x9D,0x38,0xF5,0xBC,0xB6,0xDA,0x21,0x10,0xFF,0xF3,0xD2,
    0xCD,0x0C,0x13,0xEC,0x5F,0x97,0x44,0x17,0xC4,0xA7,0x7E,0x3D,0x64,0x5D,0x19,0x73,
    0x60,0x81,0x4F,0xDC,0x22,0x2A,0x90,0x88,0x46,0xEE,0xB8,0x14,0xDE,0x5E,0x0B,0xDB,
    0xE0,0x32,0x3A,0x0A,0x49,0x06,0x24,0x5C,0xC2,0xD3,0xAC,0x62,0x91,0x95,0xE4,0x79,
    0xE7,0xC8,0x37,0x6D,0x8D,0xD5,0x4E,0xA9,0x6C,0x56,0xF4,0xEA,0x65,0x7A,0xAE,0x08,
    0xBA,0x78,0x25,0x2E,0x1C,0xA6,0xB4,0xC6,0xE8,0xDD,0x74,0x1F,0x4B,0xBD,0x8B,0x8A,
    0x70,0x3E,0xB5,0x66,0x48,0x03,0xF6,0x0E,0x61,0x35,0x57,0xB9,0x86,0xC1,0x1D,0x9E,
    0xE1,0xF8,0x98,0x11,0x69,0xD9,0x8E,0x94,0x9B,0x1E,0x87,0xE9,0xCE,0x55,0x28,0xDF,
    0x8C,0xA1,0x89,0x0D,0xBF,0xE6,0x42,0x68,0x41,0x99,0x2D,0x0F,0xB0,0x54,0xBB,0x16
};

// Inverse S-BOX table pulled from python code (AES Values)
// Used for invert_sub_bytes function for decrypting
static const unsigned char inv_s_box[256] = {
    0x52,0x09,0x6A,0xD5,0x30,0x36,0xA5,0x38,0xBF,0x40,0xA3,0x9E,0x81,0xF3,0xD7,0xFB,
    0x7C,0xE3,0x39,0x82,0x9B,0x2F,0xFF,0x87,0x34,0x8E,0x43,0x44,0xC4,0xDE,0xE9,0xCB,
    0x54,0x7B,0x94,0x32,0xA6,0xC2,0x23,0x3D,0xEE,0x4C,0x95,0x0B,0x42,0xFA,0xC3,0x4E,
    0x08,0x2E,0xA1,0x66,0x28,0xD9,0x24,0xB2,0x76,0x5B,0xA2,0x49,0x6D,0x8B,0xD1,0x25,
    0x72,0xF8,0xF6,0x64,0x86,0x68,0x98,0x16,0xD4,0xA4,0x5C,0xCC,0x5D,0x65,0xB6,0x92,
    0x6C,0x70,0x48,0x50,0xFD,0xED,0xB9,0xDA,0x5E,0x15,0x46,0x57,0xA7,0x8D,0x9D,0x84,
    0x90,0xD8,0xAB,0x00,0x8C,0xBC,0xD3,0x0A,0xF7,0xE4,0x58,0x05,0xB8,0xB3,0x45,0x06,
    0xD0,0x2C,0x1E,0x8F,0xCA,0x3F,0x0F,0x02,0xC1,0xAF,0xBD,0x03,0x01,0x13,0x8A,0x6B,
    0x3A,0x91,0x11,0x41,0x4F,0x67,0xDC,0xEA,0x97,0xF2,0xCF,0xCE,0xF0,0xB4,0xE6,0x73,
    0x96,0xAC,0x74,0x22,0xE7,0xAD,0x35,0x85,0xE2,0xF9,0x37,0xE8,0x1C,0x75,0xDF,0x6E,
    0x47,0xF1,0x1A,0x71,0x1D,0x29,0xC5,0x89,0x6F,0xB7,0x62,0x0E,0xAA,0x18,0xBE,0x1B,
    0xFC,0x56,0x3E,0x4B,0xC6,0xD2,0x79,0x20,0x9A,0xDB,0xC0,0xFE,0x78,0xCD,0x5A,0xF4,
    0x1F,0xDD,0xA8,0x33,0x88,0x07,0xC7,0x31,0xB1,0x12,0x10,0x59,0x27,0x80,0xEC,0x5F,
    0x60,0x51,0x7F,0xA9,0x19,0xB5,0x4A,0x0D,0x2D,0xE5,0x7A,0x9F,0x93,0xC9,0x9C,0xEF,
    0xA0,0xE0,0x3B,0x4D,0xAE,0x2A,0xF5,0xB0,0xC8,0xEB,0xBB,0x3C,0x83,0x53,0x99,0x61,
    0x17,0x2B,0x04,0x7E,0xBA,0x77,0xD6,0x26,0xE1,0x69,0x14,0x63,0x55,0x21,0x0C,0x7D
};

 // Replaces each individual byte within the block with a byte from the S-BOX table
void sub_bytes(unsigned char *block) {
    for (int i = 0; i < 16; i++) {
        block[i] = s_box[ block[i] ];
    }
}

 // Replaces each individual byte within the block with a byte from the Inverse S-BOX table
void invert_sub_bytes(unsigned char *block) {
    for (int i = 0; i < 16; i++) {
        block[i] = inv_s_box[ block[i] ];
    }
}

void shift_rows(unsigned char *block) {
    unsigned char temp[16];

    // Row 0: Isn't shifted
    temp[0]  = block[0];
    temp[4]  = block[4];
    temp[8]  = block[8];
    temp[12] = block[12];

    // Row 1: is shifted left by 1
    temp[1]  = block[5];
    temp[5]  = block[9];
    temp[9]  = block[13];
    temp[13] = block[1];

    // Row 2: is shifted left by 2
    temp[2]  = block[10];
    temp[6]  = block[14];
    temp[10] = block[2];
    temp[14] = block[6];

    // Row 3: is shifted left by 3
    temp[3]  = block[15];
    temp[7]  = block[3];
    temp[11] = block[7];
    temp[15] = block[11];

    // Temp is copied back into block
    for (int i=0; i<16; i++){
        block[i] = temp[i];
    }
}

void invert_shift_rows(unsigned char *block) {
    unsigned char temp[16];

    // Row 0: Isn't shifted
    temp[0]  = block[0];
    temp[4]  = block[4];
    temp[8]  = block[8];
    temp[12] = block[12];

    // Row 1: is shifted right by 1
    temp[1]  = block[13];
    temp[5]  = block[1];
    temp[9]  = block[5];
    temp[13] = block[9];

    // Row 2: is shifted right by 2
    temp[2]  = block[10];
    temp[6]  = block[14];
    temp[10] = block[2];
    temp[14] = block[6];

    // Row 3: is shifted right by 3
    temp[3]  = block[7];
    temp[7]  = block[11];
    temp[11] = block[15];
    temp[15] = block[3];

    // Temp is copied back into block
    for (int i=0; i<16; i++){
        block[i] = temp[i];
    }
}


static unsigned char xtime(unsigned char a) {
    return (a & 0x80) ? ((a << 1) ^ 0x1B) : (a << 1);
}

// r points to 4 bytes which then has the mix_columns formula applied
static void mix_single_row(unsigned char *r) {
    unsigned char a0 = r[0];
    unsigned char a1 = r[1];
    unsigned char a2 = r[2];
    unsigned char a3 = r[3];

    unsigned char t = a0 ^ a1 ^ a2 ^ a3;
    unsigned char u = a0;
    r[0] ^= t ^ xtime(a0 ^ a1);
    r[1] ^= t ^ xtime(a1 ^ a2);
    r[2] ^= t ^ xtime(a2 ^ a3);
    r[3] ^= t ^ xtime(a3 ^ u);
}

void mix_columns(unsigned char *block) {
    for(int row=0; row<4; row++){
        unsigned char r[4];
        int base = row*4;
        r[0] = block[base+0];
        r[1] = block[base+1];
        r[2] = block[base+2];
        r[3] = block[base+3];

        mix_single_row(r);

        block[base+0] = r[0];
        block[base+1] = r[1];
        block[base+2] = r[2];
        block[base+3] = r[3];
    }
}

void invert_mix_columns(unsigned char *block) {
    // Tweak each row, then do mix_single_row
    for(int row=0; row<4; row++){
        int base = row*4;
        unsigned char a0 = block[base+0];
        unsigned char a1 = block[base+1];
        unsigned char a2 = block[base+2];
        unsigned char a3 = block[base+3];

        unsigned char u = xtime( xtime(a0 ^ a2) );
        unsigned char v = xtime( xtime(a1 ^ a3) );

        block[base+0]  ^= u;
        block[base+1]  ^= v;
        block[base+2]  ^= u;
        block[base+3]  ^= v;
    }

    for(int row=0; row<4; row++){
        unsigned char r[4];
        int base = row*4;
        r[0] = block[base+0];
        r[1] = block[base+1];
        r[2] = block[base+2];
        r[3] = block[base+3];

        mix_single_row(r);

        block[base+0] = r[0];
        block[base+1] = r[1];
        block[base+2] = r[2];
        block[base+3] = r[3];
    }
}


void add_round_key(unsigned char *block, unsigned char *round_key) {
    for(int i=0; i<16; i++){
        block[i] ^= round_key[i];
    }
}

static const unsigned char r_con[10] = {
    0x01, 0x02, 0x04, 0x08, 0x10,
    0x20, 0x40, 0x80, 0x1B, 0x36
};


// Rotate word (a0->a1)
static void rotate_word(unsigned char *word) {
    unsigned char temp = word[0];
    word[0] = word[1];
    word[1] = word[2];
    word[2] = word[3];
    word[3] = temp;
}

// Puts s_box onto each of the 4 bytes
static void sub_word(unsigned char *word) {
    for(int i=0; i<4; i++){
        word[i] = s_box[word[i]];
    }
}

// Takes a 16 byte key and returns a pointer to 176 bytes which has the 11 round keys
unsigned char *expand_key(unsigned char *cipher_key) {
    unsigned char *expanded = (unsigned char *)malloc(176);
    if(!expanded) {
        // Deals with out of memory issue
        return NULL;
    }

    memcpy(expanded, cipher_key, 16);

    // We already have 4 words. We need 40 more to get 44 total (for AES-128).
    int current_word = 4;
    int total_words = 44;
    int rcon_index = 0;

    unsigned char temp[4];

    while(current_word < total_words) {
        // Take the previous word into temp
        int word_offset = (current_word - 1) * 4;
        temp[0] = expanded[word_offset + 0];
        temp[1] = expanded[word_offset + 1];
        temp[2] = expanded[word_offset + 2];
        temp[3] = expanded[word_offset + 3];

        // For each 4 words, apply rotate, sub_word, and Rcon
        if(current_word % 4 == 0) {
            rotate_word(temp);
            sub_word(temp);
            // XOR with Rcon
            temp[0] ^= r_con[rcon_index];
            rcon_index++;
        }

        // XOR temp with the word 4 words before
        int prev_word_offset = (current_word - 4) * 4;
        temp[0] ^= expanded[prev_word_offset + 0];
        temp[1] ^= expanded[prev_word_offset + 1];
        temp[2] ^= expanded[prev_word_offset + 2];
        temp[3] ^= expanded[prev_word_offset + 3];

        // Store temp into expanded
        int dest_offset = current_word * 4;
        expanded[dest_offset + 0] = temp[0];
        expanded[dest_offset + 1] = temp[1];
        expanded[dest_offset + 2] = temp[2];
        expanded[dest_offset + 3] = temp[3];

        current_word++;
    }

    return expanded; // The 176 bytes
}

unsigned char *aes_encrypt_block(unsigned char *plaintext, unsigned char *key) {
    //Expand the key to get 176 bytes
    unsigned char *expanded = expand_key(key);

    // Copy the plaintext into a local 16-byte state
    unsigned char state[16];
    for(int i=0; i<16; i++){
        state[i] = plaintext[i];
    }

    add_round_key(state, &expanded[0]);

    for(int round=1; round<=9; round++){
        sub_bytes(state);
        shift_rows(state);
        mix_columns(state);
        add_round_key(state, &expanded[round * 16]);
    }
    sub_bytes(state);
    shift_rows(state);
    add_round_key(state, &expanded[160]);

    // Set the output to the state
    unsigned char *output = malloc(16);
    for(int i=0; i<16; i++){
        output[i] = state[i];
    }

    // Free the expanded key
    free(expanded);

    return output;
}


unsigned char *aes_decrypt_block(unsigned char *ciphertext, unsigned char *key) {
    unsigned char *expanded = expand_key(key);
    unsigned char state[16];
    for(int i=0; i<16; i++){
        state[i] = ciphertext[i];
    }
    add_round_key(state, &expanded[160]);
    invert_shift_rows(state);
    invert_sub_bytes(state);
    for(int round=9; round>=1; round--){
        add_round_key(state, &expanded[round*16]);
        invert_mix_columns(state);
        invert_shift_rows(state);
        invert_sub_bytes(state);
    }

    add_round_key(state, &expanded[0]);

    unsigned char *output = malloc(16);
    for(int i=0; i<16; i++){
        output[i] = state[i];
    }
    free(expanded);
    return output;
}