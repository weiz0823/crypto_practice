#include "md5.hpp"
#include <cstring>

uint64_t MD5::Calculate(std::FILE* file, uint8_t* dest) {
    uint32_t a0 = 0x67452301;
    uint32_t b0 = 0xefcdab89;
    uint32_t c0 = 0x98badcfe;
    uint32_t d0 = 0x10325476;
    uint32_t a, b, c, d;
    uint32_t f;
    uint32_t j;
    size_t msg_len = 0, chunk_len = 0;
    uint8_t msg[64];
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
            std::memcpy(msg + 56, &msg_len, 8);
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
        for (uint8_t i = 0; i < 64; ++i) {
            if (i < 16) {
                f = (b & c) | (~b & d);
                j = i;
            } else if (i < 32) {
                f = (d & b) | (~d & c);
                j = 5 * i + 1;
                // j = (i << 2) + i + 1;
            } else if (i < 48) {
                f = b ^ c ^ d;
                j = 3 * i + 5;
                // j = i + i + i + 5;
            } else {
                f = c ^ (b | ~d);
                j = 7 * i;
                // j = (i << 3) - i;
            }
            j = (j & 15) << 2;
            f += a + msg[j] + uint32_t(msg[j + 1] << 8) +
                 uint32_t(msg[j + 2] << 16) + uint32_t(msg[j + 3] << 24) + k[i];
            a = d;
            d = c;
            c = b;
            b += (f << s[i]) | (f >> (32 - s[i]));
        }
        a0 += a;
        b0 += b;
        c0 += c;
        d0 += d;
    } while (chunk_len >= 64);
    for (uint8_t i = 0; i < 4; ++i) {
        dest[i] = a0 & 255;
        dest[i + 4] = b0 & 255;
        dest[i + 8] = c0 & 255;
        dest[i + 12] = d0 & 255;
        a0 >>= 8;
        b0 >>= 8;
        c0 >>= 8;
        d0 >>= 8;
    }
    return msg_len;
}
