/*
 * aes_256.h
 *
 *  Created on: Nov 29, 2024
 *      Author: leela
 */

#include <stdint.h>
#include <stdlib.h>

#define AES_BLOCK_SIZE 16
#define AES_KEY_SIZE 32
#define AES_ROUNDS 14

void pkcs7_pad(const uint8_t *input, size_t input_len, uint8_t **output, size_t *output_len);
void xor_blocks(uint8_t *block1, const uint8_t *block2);
void sub_bytes(uint8_t *state);
void shift_rows(uint8_t *state);
void mix_columns(uint8_t *state);
void add_round_key(uint8_t *state, uint8_t *round_key);
void key_expansion(const uint8_t *key, uint8_t *round_keys);
void aes_encrypt_cbc(const uint8_t *input, const uint8_t *key, const uint8_t *iv, uint8_t *output, size_t length);
void generate_subkeys(const uint8_t *key, uint8_t *K1, uint8_t *K2);
void compute_cmac(const uint8_t *message, size_t message_len, const uint8_t *key, uint8_t *mac);

