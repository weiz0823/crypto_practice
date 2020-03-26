#include "sha3.hpp"
#include <cstring>
namespace cryp {
void SHA3::Keccak_Permutation(uint64_t a[5][5]) {
    uint64_t c[5], d[5], pi[5][5];
    constexpr uint64_t rho_count[24] = {1,  3,  6,  10, 15, 21, 28, 36,
                                        45, 55, 2,  14, 27, 41, 56, 8,
                                        25, 43, 62, 18, 39, 61, 20, 44};
    constexpr uint64_t rho_x[24] = {1, 0, 2, 1, 2, 3, 3, 0, 1, 3, 1, 4,
                                    4, 0, 3, 4, 3, 2, 2, 0, 4, 2, 4, 1};
    constexpr uint64_t rho_y[24] = {0, 2, 1, 2, 3, 3, 0, 1, 3, 1, 4, 4,
                                    0, 3, 4, 3, 2, 2, 0, 4, 2, 4, 1, 1};
    constexpr uint64_t pi_x[5][5] = {{0, 3, 1, 4, 2},
                                     {1, 4, 2, 0, 3},
                                     {2, 0, 3, 1, 4},
                                     {3, 1, 4, 2, 0},
                                     {4, 2, 0, 3, 1}};
    constexpr uint64_t iota_rc[24] = {0x1,
                                      0x8082,
                                      0x800000000000808a,
                                      0x8000000080008000,
                                      0x808b,
                                      0x80000001,
                                      0x8000000080008081,
                                      0x8000000000008009,
                                      0x8a,
                                      0x88,
                                      0x80008009,
                                      0x8000000a,
                                      0x8000808b,
                                      0x800000000000008b,
                                      0x8000000000008089,
                                      0x8000000000008003,
                                      0x8000000000008002,
                                      0x8000000000000080,
                                      0x800a,
                                      0x800000008000000a,
                                      0x8000000080008081,
                                      0x8000000000008080,
                                      0x80000001,
                                      0x8000000080008008};
    for (auto rc : iota_rc) {
        // step theta, xor parity
        for (uint8_t i = 0; i < 5; ++i)
            c[i] = a[i][0] ^ a[i][1] ^ a[i][2] ^ a[i][3] ^ a[i][4];
        d[0] = c[4] ^ LeftRotate64(c[1], 1);
        d[1] = c[0] ^ LeftRotate64(c[2], 1);
        d[2] = c[1] ^ LeftRotate64(c[3], 1);
        d[3] = c[2] ^ LeftRotate64(c[4], 1);
        d[4] = c[3] ^ LeftRotate64(c[0], 1);
        for (uint8_t i = 0; i < 5; ++i)
            for (uint8_t j = 0; j < 5; ++j) a[i][j] ^= d[i];
        // step rho, rotate, only a(0,0) is not rotated
        for (uint8_t i = 0; i < 24; ++i)
            a[rho_x[i]][rho_y[i]] =
                LeftRotate64(a[rho_x[i]][rho_y[i]], rho_count[i]);
        // step pi, permutation, diverge to array pi
        for (uint8_t i = 0; i < 5; ++i)
            for (uint8_t j = 0; j < 5; ++j) pi[i][j] = a[pi_x[i][j]][i];
        // step chi, non-linear, merge back to array a
        for (uint8_t i = 0; i < 5; ++i) {
            a[0][i] = pi[0][i] ^ (~pi[1][i] & pi[2][i]);
            a[1][i] = pi[1][i] ^ (~pi[2][i] & pi[3][i]);
            a[2][i] = pi[2][i] ^ (~pi[3][i] & pi[4][i]);
            a[3][i] = pi[3][i] ^ (~pi[4][i] & pi[0][i]);
            a[4][i] = pi[4][i] ^ (~pi[0][i] & pi[1][i]);
        }
        // step iota, xor a LSFR sequence to break symmetry
        a[0][0] ^= rc;
    }
}
template <uint32_t CAP, uint8_t SUFFIX>
uint64_t SHA3::Keccak(std::FILE* file, uint8_t* dest, uint32_t out_len) {
    constexpr uint32_t RATE = 200 - CAP;
    // state array
    uint64_t a[5][5];
    std::memset(a, 0, sizeof(a));
    size_t chunk_len;
    size_t msg_len = 0;
    uint8_t msg[RATE];
    do {
        chunk_len = std::fread(msg, 1, RATE, file);
        msg_len += chunk_len << 3;
        // pad
        if (chunk_len < RATE - 1) {
            msg[chunk_len] = SUFFIX;
            std::memset(msg + chunk_len + 1, 0, RATE - 2 - chunk_len);
            msg[RATE - 1] = 0x80;
        } else if (chunk_len < RATE) {
            msg[chunk_len] = SUFFIX | 0x80;
        }
        // xor into state array, column major
        for (uint32_t i = 0; i < (RATE >> 3); ++i)
            for (uint32_t j = 0; j < 8; ++j)
                a[i % 5][i / 5] ^= (uint64_t(msg[(i << 3) + j]) << (j << 3));

        Keccak_Permutation(a);
    } while (chunk_len >= RATE);
    uint64_t tmp;
    uint32_t offset = 0;
    uint32_t i;
    for (i = 0; i < (out_len >> 3); ++i) {
        if (i - offset == 25) {
            offset = i;
            Keccak_Permutation(a);
        }
        tmp = a[(i - offset) % 5][(i - offset) / 5];
        for (uint8_t j = 0; j < 8; ++j) {
            dest[(i << 3) + j] = (tmp & 255);
            tmp >>= 8;
        }
    }
    if (i - offset == 25) {
        offset = i;
        Keccak_Permutation(a);
    }
    tmp = a[(i - offset) % 5][(i - offset) / 5];
    for (uint8_t j = 0; j < out_len - (i << 3); ++j) {
        dest[(i << 3) + j] = (tmp & 255);
        tmp >>= 8;
    }
    return msg_len;
}
uint64_t SHA3::SHA224(std::FILE* file, uint8_t* dest) {
    return Keccak<56, 0x06>(file, dest, 28);
}
uint64_t SHA3::SHA256(std::FILE* file, uint8_t* dest) {
    return Keccak<64, 0x06>(file, dest, 32);
}
uint64_t SHA3::SHA384(std::FILE* file, uint8_t* dest) {
    return Keccak<96, 0x06>(file, dest, 48);
}
uint64_t SHA3::SHA512(std::FILE* file, uint8_t* dest) {
    return Keccak<128, 0x06>(file, dest, 64);
}
uint64_t SHA3::SHAKE128(std::FILE* file, uint8_t* dest, uint32_t out_len) {
    return Keccak<32, 0x1f>(file, dest, out_len);
}
uint64_t SHA3::SHAKE256(std::FILE* file, uint8_t* dest, uint32_t out_len) {
    return Keccak<64, 0x1f>(file, dest, out_len);
}
uint64_t SHA3::RawSHAKE128(std::FILE* file, uint8_t* dest, uint32_t out_len) {
    return Keccak<32, 0x07>(file, dest, out_len);
}
uint64_t SHA3::RawSHAKE256(std::FILE* file, uint8_t* dest, uint32_t out_len) {
    return Keccak<64, 0x07>(file, dest, out_len);
}
}  // namespace cryp
