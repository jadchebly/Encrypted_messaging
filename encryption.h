#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <stddef.h> // For size_t

// XOR encryption function
void xor_encrypt_decrypt(char *data, size_t data_len, const char *key, size_t key_len);

// XOR decryption function
void xor_decrypt(char *data, size_t data_len, const char *key, size_t key_len);

#endif // ENCRYPTION_H
