#include "decryption.h"
#include "aes.h"
#include "base64.h"

int magic_key[] = {59, 55, 63, 56, 55, 63, 49, 53, 120, 9, 48, 36, 63, 50, 55, 57};
size_t magic_size = sizeof(magic_key) / sizeof(int);

inline size_t short_encode(char *src, char *out, size_t size) {
    for (int i = 0; i < size; ++i) {
        *(out + i) = *(src + i) ^ 86;
    }
    return size;
}

inline size_t code2char(const int *code, char *magic, size_t size) {
    for (int i = 0; i < size; ++i) {
        *(magic + i) = (char) *(code + i);
    }
    return size;
}

inline size_t pkcs7_cutting(unsigned char *data, size_t size) {
    if (size <= 0) {
        return 0;
    }

    unsigned char *cur_point = data + (size - 1);
    int pad = (int) *cur_point;
    if (pad < 0) {
        return 0;
    }

    for (int i = 0; i < pad; ++i) {
        if ((int) *(cur_point - i) != pad) {
            return size;
        }
    }

    memset(data + (size - pad), 0, pad);
    return size - pad;
}

void js_decr(const char *c, size_t len, std::string &out) {
    unsigned char *src = new unsigned char[len + 1];
    memset(src, 0, len + 1);
    size_t es_size = base64_decode(c, len, src);
    char *magic = new char[magic_size + 1];
    memset(magic, 0, magic_size + 1);
    code2char(magic_key, magic, magic_size);
    short_encode(magic, magic, magic_size);
    //
    struct AES_ctx ctx;
    unsigned char *key = reinterpret_cast<unsigned char *>(magic);
    AES_init_ctx_iv(&ctx, key, key);
    AES_CBC_decrypt_buffer(&ctx, src, es_size);
    size_t size = pkcs7_cutting(src, es_size);
    out.assign(reinterpret_cast<const char *>(src), size);
    delete[] src;
    delete[] magic;
}