#include "sha1.hpp"
#include <cstring>

uint64_t SHA1::Calculate(std::FILE* file, uint8_t* dest) {
    uint32_t a0 = 0x67452301;
    uint32_t b0 = 0xefcdab89;
    uint32_t c0 = 0x98badcfe;
    uint32_t d0 = 0x10325476;
    uint32_t e0 = 0xc3d2e1f0;
    uint32_t a, b, c, d, e;
    uint32_t f;
    uint32_t j;
    size_t msg_len = 0, chunk_len = 0, len_tmp = 0;
    uint8_t msg[64];
    uint32_t w[80];
    bool flag = false;
    do {
        chunk_len = std::fread(msg, 1, 64, file);
        msg_len += chunk_len << 3;
        if (chunk_len <= 56) {
            if (flag)
                msg[chunk_len] = 0;
            else
                msg[chunk_len] = 128;
            std::memset(msg + chunk_len + 1, 0, 55 - chunk_len);
            len_tmp = msg_len;
            for (uint8_t i = 63; i >= 56; --i) {
                msg[i] = len_tmp & 255;
                len_tmp >>= 8;
            }
        } else if (chunk_len < 64) {
            msg[chunk_len] = 128;
            std::memset(msg + chunk_len + 1, 0, 63 - chunk_len);
            chunk_len = 64;
            flag = true;
        }

        a = a0;
        b = b0;
        c = c0;
        d = d0;
        e = e0;
        for (uint8_t i = 0; i < 80; ++i) {
            if (i < 16) {
                w[i] =
                    uint32_t(msg[4 * i + 3]) + uint32_t(msg[4 * i + 2] << 8) +
                    uint32_t(msg[4 * i + 1] << 16) + uint32_t(msg[4 * i] << 24);
            } else {
                w[i] =
                    LeftRotate(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
            }
            if (i < 20) {
                f = (b & c) | (~b & d);
                j = 0x5A827999;
            } else if (i < 40) {
                f = b ^ c ^ d;
                j = 0x6ED9EBA1;
            } else if (i < 60) {
                f = (b & c) | (b & d) | (c & d);
                j = 0x8F1BBCDC;
            } else {
                f = b ^ c ^ d;
                j = 0xCA62C1D6;
            }
            f += e + LeftRotate(a, 5) + j + w[i];
            e = d;
            d = c;
            c = LeftRotate(b, 30);
            b = a;
            a = f;
        }
        a0 += a;
        b0 += b;
        c0 += c;
        d0 += d;
        e0 += e;
    } while (chunk_len >= 64);
    for (uint8_t i = 3; i != uint8_t(-1); --i) {
        dest[i] = a0 & 255;
        dest[i + 4] = b0 & 255;
        dest[i + 8] = c0 & 255;
        dest[i + 12] = d0 & 255;
        dest[i + 16] = e0 & 255;
        a0 >>= 8;
        b0 >>= 8;
        c0 >>= 8;
        d0 >>= 8;
        e0 >>= 8;
    }
    return msg_len;
}
