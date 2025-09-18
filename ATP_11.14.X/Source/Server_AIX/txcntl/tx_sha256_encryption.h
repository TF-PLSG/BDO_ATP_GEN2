#ifndef _SHA256_H_
#define _SHA256_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SHA256_DIGEST_LENGTH 32

/**
 * @brief Context for ciper operatins based on sha256
 */
typedef struct {
    /** global state */
    uint32_t state[8];
    /** processed bytes counter */
    uint32_t count[2];
    /** data buffer */
    unsigned char buf[64];
} sha256_context_t;

/**
 * @brief SHA-256 initialization.  Begins a SHA-256 operation.
 *
 * @param ctx  sha256_context_t handle to init
 */
void sha256_init(sha256_context_t *ctx);

/**
 * @brief Add bytes into the hash
 *
 * @param ctx  sha256_context_t handle to use
 * @param in   pointer to the input buffer
 * @param len  length of the buffer
 */
void sha256_update(sha256_context_t *ctx, const void *in, size_t len);

/**
 * @brief SHA-256 finalization.  Pads the input data, exports the hash value,
 * and clears the context state.
 *
 * @param digest resulting digest, this is the hash of all the bytes
 * @param ctx    sha256_context_t handle to use
 */
void sha256_final(unsigned char digest[32], sha256_context_t *ctx);

/**
 * @brief A wrapper function to simplify the generation of a hash, this is
 * usefull for generating sha256 for one buffer
 *
 * @param d pointer to the buffer to generate hash from
 * @param n length of the buffer
 * @param md optional pointer to an array for the result, length must be
 *           SHA256_DIGEST_LENGTH
 *           if md == NULL, one static buffer is used
 */
unsigned char *sha256(const unsigned char *d, size_t n, unsigned char *md);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* _SHA256_H_ */