#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aes_256.h"

static const uint8_t sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

static const uint8_t Rcon[15] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
    0x1B, 0x36, 0x6C, 0xD8, 0xAB, 0x4D, 0x9A
};

uint8_t mul2(uint8_t x) {
    return (x << 1) ^ ((x & 0x80) ? 0x1b : 0x00);
}

uint8_t mul3(uint8_t x) {
    return mul2(x) ^ x;
}

void pkcs7_pad(const uint8_t *input, size_t input_len, uint8_t **output, size_t *output_len) {
    size_t padding_len = AES_BLOCK_SIZE - (input_len % AES_BLOCK_SIZE);
    if (padding_len == 0) {
        padding_len = AES_BLOCK_SIZE;
    }

    *output_len = input_len + padding_len;
    *output = malloc(*output_len);
    if (*output == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    memcpy(*output, input, input_len);
    memset(*output + input_len, padding_len, padding_len);
}


void xor_blocks(uint8_t *block1, const uint8_t *block2) {
    for (int i = 0; i < AES_BLOCK_SIZE; i++) {
        block1[i] ^= block2[i];
    }
}

void sub_bytes(uint8_t *state) {
    for (int i = 0; i < AES_BLOCK_SIZE; i++) {
        state[i] = sbox[state[i]];
    }
}

void shift_rows(uint8_t *state) {
    uint8_t temp;

    temp = state[1];
    state[1] = state[5];
    state[5] = state[9];
    state[9] = state[13];
    state[13] = temp;

    temp = state[2];
    state[2] = state[10];
    state[10] = temp;

    temp = state[6];
    state[6] = state[14];
    state[14] = temp;

    temp = state[3];
    state[3] = state[15];
    state[15] = temp;
}

void mix_columns(uint8_t *state) {
    uint8_t temp[AES_BLOCK_SIZE];

    for (int i = 0; i < 4; i++) {
        temp[i * 4] = mul2(state[i * 4]) ^ mul3(state[i * 4 + 1]) ^ state[i * 4 + 2] ^ state[i * 4 + 3];
        temp[i * 4 + 1] = state[i * 4] ^ mul2(state[i * 4 + 1]) ^ mul3(state[i * 4 + 2]) ^ state[i * 4 + 3];
        temp[i * 4 + 2] = state[i * 4] ^ state[i * 4 + 1] ^ mul2(state[i * 4 + 2]) ^ mul3(state[i * 4 + 3]);
        temp[i * 4 + 3] = mul3(state[i * 4]) ^ state[i * 4 + 1] ^ state[i * 4 + 2] ^ mul2(state[i * 4 + 3]);
    }

    memcpy(state, temp, AES_BLOCK_SIZE);
}

void add_round_key(uint8_t *state, uint8_t *round_key) {
    for (int i = 0; i < AES_BLOCK_SIZE; i++) {
        state[i] ^= round_key[i];
    }
}
void key_expansion(const uint8_t *key, uint8_t *round_keys) {
    uint8_t temp[4];
    int i = 0;

    while (i < 8) {
        round_keys[i * 4] = key[i * 4];
        round_keys[i * 4 + 1] = key[i * 4 + 1];
        round_keys[i * 4 + 2] = key[i * 4 + 2];
        round_keys[i * 4 + 3] = key[i * 4 + 3];
        i++;
    }

    i = 8;
    while (i < (AES_ROUNDS + 1) * 4) {
        temp[0] = round_keys[(i - 1) * 4];
        temp[1] = round_keys[(i - 1) * 4 + 1];
        temp[2] = round_keys[(i - 1) * 4 + 2];
        temp[3] = round_keys[(i - 1) * 4 + 3];

        if (i % 8 == 0) {
            uint8_t temp_val = temp[0];
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = temp_val;

            temp[0] = sbox[temp[0]];
            temp[1] = sbox[temp[1]];
            temp[2] = sbox[temp[2]];
            temp[3] = sbox[temp[3]];

            temp[0] ^= Rcon[i / 8];
        }

        round_keys[i * 4] = round_keys[(i - 8) * 4] ^ temp[0];
        round_keys[i * 4 + 1] = round_keys[(i - 8) * 4 + 1] ^ temp[1];
        round_keys[i * 4 + 2] = round_keys[(i - 8) * 4 + 2] ^ temp[2];
        round_keys[i * 4 + 3] = round_keys[(i - 8) * 4 + 3] ^ temp[3];
        i++;
    }
}

void aes_encrypt_cbc(const uint8_t *input, const uint8_t *key, const uint8_t *iv, uint8_t *output, size_t length) {
    uint8_t round_keys[(AES_ROUNDS + 1) * 4 * AES_BLOCK_SIZE];
    key_expansion((uint8_t *)key, round_keys);

    uint8_t state[AES_BLOCK_SIZE];
    uint8_t previous_block[AES_BLOCK_SIZE];
    memcpy(previous_block, iv, AES_BLOCK_SIZE);

    size_t padded_len;
    uint8_t *padded_input;
    pkcs7_pad(input, length, &padded_input, &padded_len);

    for (size_t i = 0; i < padded_len; i += AES_BLOCK_SIZE) {
        memcpy(state, padded_input + i, AES_BLOCK_SIZE);
        xor_blocks(state, previous_block);
        add_round_key(state, round_keys);

        for (int round = 1; round < AES_ROUNDS; round++) {
            sub_bytes(state);
            shift_rows(state);
            mix_columns(state);
            add_round_key(state, round_keys + round * AES_BLOCK_SIZE);
        }

        sub_bytes(state);
        shift_rows(state);
        add_round_key(state, round_keys + AES_ROUNDS * AES_BLOCK_SIZE);

        memcpy(output + i, state, AES_BLOCK_SIZE);
        memcpy(previous_block, output + i, AES_BLOCK_SIZE);
    }

    free(padded_input);
}

void generate_subkeys(const uint8_t *key, uint8_t *K1, uint8_t *K2) {
    uint8_t L[AES_BLOCK_SIZE] = {0};
    uint8_t iv[AES_BLOCK_SIZE] = {0};
    aes_encrypt_cbc(L, (uint8_t*)key, iv, L, AES_BLOCK_SIZE);

    for (int i = 0; i < AES_BLOCK_SIZE - 1; i++) {
        K1[i] = L[i] << 1 | (L[i + 1] >> 7);
    }
    K1[AES_BLOCK_SIZE - 1] = L[AES_BLOCK_SIZE - 1] << 1;

    if (K1[0] & 0x80) {
        K1[AES_BLOCK_SIZE - 1] ^= 0x87;
    }

    for (int i = 0; i < AES_BLOCK_SIZE - 1; i++) {
        K2[i] = K1[i] << 1 | (K1[i + 1] >> 7);
    }
    K2[AES_BLOCK_SIZE - 1] = K1[AES_BLOCK_SIZE - 1] << 1;

    if (K2[0] & 0x80) {
        K2[AES_BLOCK_SIZE - 1] ^= 0x87;
    }
}

void compute_cmac(const uint8_t *message, size_t message_len, const uint8_t *key, uint8_t *mac) {
    uint8_t K1[AES_BLOCK_SIZE], K2[AES_BLOCK_SIZE];
    generate_subkeys(key, K1, K2);

    size_t num_blocks = (message_len + AES_BLOCK_SIZE - 1) / AES_BLOCK_SIZE;
    uint8_t block[AES_BLOCK_SIZE] = {0};
    uint8_t prev_block[AES_BLOCK_SIZE] = {0};

    for (size_t i = 0; i < num_blocks; i++) {
        size_t block_size = (i == num_blocks - 1) ? message_len % AES_BLOCK_SIZE : AES_BLOCK_SIZE;


        memcpy(block, message + i * AES_BLOCK_SIZE, block_size);

        if (block_size < AES_BLOCK_SIZE) {
            memset(block + block_size, 0, AES_BLOCK_SIZE - block_size);
            block[AES_BLOCK_SIZE - 1] |= 0x80;
        }

        xor_blocks(block, prev_block);

        aes_encrypt_cbc(block, key, prev_block, prev_block, AES_BLOCK_SIZE);

    }

    memcpy(mac, prev_block, 8);
}
