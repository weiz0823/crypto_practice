#include "sha2.hpp"
#include <cstring>
#include "uint128.hpp"

namespace cryp {

uint64_t SHA2::SHA224_256(std::FILE* file, uint8_t* dest, uint32_t* a0,
                          uint32_t t) {
    uint32_t k[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
        0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
        0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
        0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
        0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
        0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
    uint32_t a[8];
    uint32_t f1, f2;
    size_t msg_len = 0, chunk_len = 0, len_tmp = 0;
    uint8_t msg[64];
    uint32_t w[64];
    bool flag = false;
    do {
        chunk_len = std::fread(msg, 1, 64, file);
        msg_len += chunk_len << 3;
        if (chunk_len < 56) {
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

        for (uint8_t i = 0; i < 8; ++i) a[i] = a0[i];
        for (uint8_t i = 0; i < 64; ++i) {
            if (i < 16) {
                w[i] =
                    uint32_t(msg[4 * i + 3]) + uint32_t(msg[4 * i + 2] << 8) +
                    uint32_t(msg[4 * i + 1] << 16) + uint32_t(msg[4 * i] << 24);
            } else {
                w[i] = w[i - 16] + w[i - 7];
                w[i] += RightRotate32(w[i - 15], 7) ^
                        RightRotate32(w[i - 15], 18) ^ (w[i - 15] >> 3);
                w[i] += RightRotate32(w[i - 2], 17) ^
                        RightRotate32(w[i - 2], 19) ^ (w[i - 2] >> 10);
            }
            f1 = k[i] + w[i] + a[7];
            f1 += RightRotate32(a[4], 6) ^ RightRotate32(a[4], 11) ^
                  RightRotate32(a[4], 25);
            f1 += (a[4] & a[5]) ^ (~a[4] & a[6]);
            f2 = RightRotate32(a[0], 2) ^ RightRotate32(a[0], 13) ^
                 RightRotate32(a[0], 22);
            f2 += (a[0] & a[1]) ^ (a[0] & a[2]) ^ (a[1] & a[2]);
            for (uint8_t j = 7; j > 0; --j) a[j] = a[j - 1];
            a[4] += f1;
            a[0] = f1 + f2;
        }
        for (uint8_t i = 0; i < 8; ++i) a0[i] += a[i];
    } while (chunk_len >= 64);
    t = (t + 7) >> 3;
    if (t > 32) t = 32;
    uint32_t i;
    for (i = 0; i < (t >> 2); ++i)
        for (uint8_t j = 3; j != uint8_t(-1); --j) {
            dest[(i << 2) + j] = a0[i] & 255;
            a0[i] >>= 8;
        }
    uint8_t j = 7;
    for (; j != uint8_t(t - 1 - (i << 2)); --j) a0[i] >>= 8;
    for (; j != uint8_t(-1); --j) {
        dest[(i << 2) + j] = a0[i] & 255;
        a0[i] >>= 8;
    }
    return msg_len;
}
void SHA2::SHA512Process(uint64_t* a0, uint8_t* msg) {
    uint64_t f1, f2;
    uint64_t w[80];
    const uint64_t k[80] = {
        0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f,
        0xe9b5dba58189dbbc, 0x3956c25bf348b538, 0x59f111f1b605d019,
        0x923f82a4af194f9b, 0xab1c5ed5da6d8118, 0xd807aa98a3030242,
        0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
        0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235,
        0xc19bf174cf692694, 0xe49b69c19ef14ad2, 0xefbe4786384f25e3,
        0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65, 0x2de92c6f592b0275,
        0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
        0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f,
        0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725,
        0x06ca6351e003826f, 0x142929670a0e6e70, 0x27b70a8546d22ffc,
        0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
        0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6,
        0x92722c851482353b, 0xa2bfe8a14cf10364, 0xa81a664bbc423001,
        0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218,
        0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
        0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99,
        0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb,
        0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3, 0x748f82ee5defb2fc,
        0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
        0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915,
        0xc67178f2e372532b, 0xca273eceea26619c, 0xd186b8c721c0c207,
        0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, 0x06f067aa72176fba,
        0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
        0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc,
        0x431d67c49c100d4c, 0x4cc5d4becb3e42b6, 0x597f299cfc657e2a,
        0x5fcb6fab3ad6faec, 0x6c44198c4a475817};
    uint64_t a[8];
    for (uint8_t i = 0; i < 8; ++i) a[i] = a0[i];
    for (uint8_t i = 0; i < 80; ++i) {
        if (i < 16) {
            w[i] = uint64_t(msg[8 * i]);
            for (uint8_t j = 1; j < 8; ++j) {
                w[i] <<= 8;
                w[i] += uint64_t(msg[8 * i + j]);
            }
        } else {
            w[i] = w[i - 16] + w[i - 7];
            w[i] += RightRotate64(w[i - 15], 1) ^ RightRotate64(w[i - 15], 8) ^
                    (w[i - 15] >> 7);
            w[i] += RightRotate64(w[i - 2], 19) ^ RightRotate64(w[i - 2], 61) ^
                    (w[i - 2] >> 6);
        }
        f1 = k[i] + w[i] + a[7];
        f1 += RightRotate64(a[4], 14) ^ RightRotate64(a[4], 18) ^
              RightRotate64(a[4], 41);
        f1 += (a[4] & a[5]) ^ (~a[4] & a[6]);
        f2 = RightRotate64(a[0], 28) ^ RightRotate64(a[0], 34) ^
             RightRotate64(a[0], 39);
        f2 += (a[0] & a[1]) ^ (a[0] & a[2]) ^ (a[1] & a[2]);
        for (uint8_t j = 7; j > 0; --j) a[j] = a[j - 1];
        a[4] += f1;
        a[0] = f1 + f2;
    }
    for (uint8_t i = 0; i < 8; ++i) a0[i] += a[i];
}
uint64_t SHA2::SHA384_512(std::FILE* file, uint8_t* dest, uint64_t* a0,
                          uint32_t t) {
    uint128_t msg_len(0), len_tmp(0);
    size_t chunk_len = 0;
    uint8_t msg[128];
    bool flag = false;
    do {
        chunk_len = std::fread(msg, 1, 128, file);
        msg_len += uint128_t(chunk_len << 3);
        if (chunk_len < 112) {
            if (flag)
                msg[chunk_len] = 0;
            else
                msg[chunk_len] = 128;
            std::memset(msg + chunk_len + 1, 0, 111 - chunk_len);
            len_tmp = msg_len;
            for (uint8_t i = 127; i >= 112; --i) {
                msg[i] = len_tmp & 255;
                len_tmp >>= 8;
            }
        } else if (chunk_len < 128) {
            msg[chunk_len] = 128;
            std::memset(msg + chunk_len + 1, 0, 127 - chunk_len);
            chunk_len = 128;
            flag = true;
        }

        SHA512Process(a0, msg);
    } while (chunk_len >= 128);
    t = (t + 7) >> 3;
    if (t > 64) t = 64;
    uint32_t i;
    for (i = 0; i < (t >> 3); ++i)
        for (uint8_t j = 7; j != uint8_t(-1); --j) {
            dest[(i << 3) + j] = a0[i] & 255;
            a0[i] >>= 8;
        }
    uint8_t j = 7;
    for (; j != uint8_t(t - 1 - (i << 3)); --j) a0[i] >>= 8;
    for (; j != uint8_t(-1); --j) {
        dest[(i << 3) + j] = a0[i] & 255;
        a0[i] >>= 8;
    }
    return uint64_t(msg_len);
}
// generate initial value for SHA512/t
uint64_t SHA2::SHA512t_IVGen(uint8_t* src, size_t tot_len, uint64_t* dest) {
    uint64_t a0[8] = {0xcfac43c256196cad, 0x1ec20b20216f029e,
                      0x99cb56d75b315d8e, 0x00ea509ffab89354,
                      0xf4abf7da08432774, 0x3ea0cd298e9bc9ba,
                      0xba267c0e5ee418ce, 0xfe4568bcb6db84dc};
    uint128_t msg_len(0), len_tmp(0);
    size_t chunk_len = 0;
    uint8_t msg[128];
    bool flag = false;
    size_t read_pos = 0;
    do {
        chunk_len = 128;
        for (size_t i = 0; i < 128; ++i) {
            if (read_pos + i >= tot_len) {
                chunk_len = i;
                break;
            } else {
                msg[i] = src[read_pos + i];
            }
        }
        read_pos += chunk_len;
        msg_len += uint128_t(chunk_len << 3);
        if (chunk_len < 112) {
            if (flag)
                msg[chunk_len] = 0;
            else
                msg[chunk_len] = 128;
            std::memset(msg + chunk_len + 1, 0, 111 - chunk_len);
            len_tmp = msg_len;
            for (uint8_t i = 127; i >= 112; --i) {
                msg[i] = len_tmp & 255;
                len_tmp >>= 8;
            }
        } else if (chunk_len < 128) {
            msg[chunk_len] = 128;
            std::memset(msg + chunk_len + 1, 0, 127 - chunk_len);
            chunk_len = 128;
            flag = true;
        }

        SHA512Process(a0, msg);
    } while (chunk_len >= 128);
    for (uint8_t i = 0; i < 8; ++i) dest[i] = a0[i];
    return uint64_t(msg_len);
}
uint64_t SHA2::SHA224(std::FILE* file, uint8_t* dest) {
    uint32_t a0[8] = {0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939,
                      0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4};
    return SHA224_256(file, dest, a0, 224);
}
uint64_t SHA2::SHA256(std::FILE* file, uint8_t* dest) {
    uint32_t a0[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                      0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
    return SHA224_256(file, dest, a0, 256);
}
uint64_t SHA2::SHA384(std::FILE* file, uint8_t* dest) {
    uint64_t a0[8] = {0xcbbb9d5dc1059ed8, 0x629a292a367cd507,
                      0x9159015a3070dd17, 0x152fecd8f70e5939,
                      0x67332667ffc00b31, 0x8eb44a8768581511,
                      0xdb0c2e0d64f98fa7, 0x47b5481dbefa4fa4};
    return SHA384_512(file, dest, a0, 384);
}
uint64_t SHA2::SHA512(std::FILE* file, uint8_t* dest) {
    uint64_t a0[8] = {0x6a09e667f3bcc908, 0xbb67ae8584caa73b,
                      0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
                      0x510e527fade682d1, 0x9b05688c2b3e6c1f,
                      0x1f83d9abfb41bd6b, 0x5be0cd19137e2179};
    return SHA384_512(file, dest, a0, 512);
}
uint64_t SHA2::SHA512t(std::FILE* file, uint8_t* dest, uint32_t t) {
    uint64_t a0[8];
    char s[32];
    std::sprintf(s, "SHA-512/%u", t);
    SHA512t_IVGen(reinterpret_cast<uint8_t*>(s), std::strlen(s), a0);
    return SHA384_512(file, dest, a0, t);
}
}  // namespace cryp
