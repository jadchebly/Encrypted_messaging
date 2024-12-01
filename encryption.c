#include <stddef.h> // For size_t
#include "encryption.h"


// XOR encryption function
void xor_encrypt_decrypt(char *data, size_t data_len, const char *key, size_t key_len) {
    for (size_t i = 0; i < data_len; i++) {
        data[i] = (data[i] ^ key[i % key_len]) + 1; // Add offset to avoid null characters
    }
}

// XOR decryption function
void xor_decrypt(char *data, size_t data_len, const char *key, size_t key_len) {
    for (size_t i = 0; i < data_len; i++) {
        data[i] = (data[i] - 1) ^ key[i % key_len]; // Subtract offset before XOR
    }
}
