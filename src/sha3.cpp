#include "sha3.hpp"
namespace cryp {
// avoid multiple copy of core permutation
static void KeccakPermutation(uint64_t (&a)[5][5]) {
    // constants
    static constexpr uint64_t rho_count[24] = {1,  3,  6,  10, 15, 21, 28, 36,
                                               45, 55, 2,  14, 27, 41, 56, 8,
                                               25, 43, 62, 18, 39, 61, 20, 44};
    static constexpr uint64_t rho_x[24] = {1, 0, 2, 1, 2, 3, 3, 0, 1, 3, 1, 4,
                                           4, 0, 3, 4, 3, 2, 2, 0, 4, 2, 4, 1};
    static constexpr uint64_t rho_y[24] = {0, 2, 1, 2, 3, 3, 0, 1, 3, 1, 4, 4,
                                           0, 3, 4, 3, 2, 2, 0, 4, 2, 4, 1, 1};
    static constexpr uint64_t pi_x[5][5] = {{0, 3, 1, 4, 2},
                                            {1, 4, 2, 0, 3},
                                            {2, 0, 3, 1, 4},
                                            {3, 1, 4, 2, 0},
                                            {4, 2, 0, 3, 1}};
    static constexpr uint64_t iota_rc[24] = {0x1,
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

    uint64_t c[5], d[5], pi[5][5];
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

template <uint32_t CAP, uint8_t PAD>
void SHA3Var<CAP, PAD>::HashProcess() {
    // xor into state array, column major
    for (uint32_t i = 0; i < (RATE >> 3); ++i)
        for (uint32_t j = 0; j < 8; ++j)
            a_[i % 5][i / 5] ^= (uint64_t(msg_[(i << 3) + j]) << (j << 3));
    KeccakPermutation(a_);
}
template <uint32_t CAP, uint8_t PAD>
uint64_t SHA3Var<CAP, PAD>::HashUpdate(std::FILE* file) {
    if (!file) return msg_len_;
    uint64_t old_chunk = chunk_len_;
    chunk_len_ = std::fread(msg_ + old_chunk, 1, RATE - old_chunk, file);
    msg_len_ += chunk_len_ << 3;
    if (old_chunk + chunk_len_ < RATE) return msg_len_;
    do {
        HashProcess();
        chunk_len_ = std::fread(msg_, 1, RATE, file);
        msg_len_ += chunk_len_ << 3;
    } while (chunk_len_ >= RATE);
    return msg_len_;
}
template <uint32_t CAP, uint8_t PAD>
uint64_t SHA3Var<CAP, PAD>::HashUpdate(const uint8_t* src, uint64_t bytelen) {
    if (!src) return msg_len_;
    uint64_t old_chunk = chunk_len_;
    ArrayIstream stream(src, bytelen);
    chunk_len_ = stream.Read(msg_ + old_chunk, RATE - old_chunk);
    msg_len_ += chunk_len_ << 3;
    if (old_chunk + chunk_len_ < RATE) return msg_len_;
    do {
        HashProcess();
        chunk_len_ = stream.Read(msg_, RATE);
        msg_len_ += chunk_len_ << 3;
    } while (chunk_len_ >= RATE);
    return msg_len_;
}
template <uint32_t CAP, uint8_t PAD>
uint64_t SHA3Var<CAP, PAD>::HashFinal(uint8_t* dst) {
    if (chunk_len_ < RATE - 1) {
        msg_[chunk_len_] = PAD;
        std::memset(msg_ + chunk_len_ + 1, 0, RATE - 2 - chunk_len_);
        msg_[RATE - 1] = 0x80;
    } else {
        msg_[chunk_len_] = PAD | 0x80;
    }
    HashProcess();
    uint64_t tmp_len = msg_len_;
    GetHash(dst);
    // reset
    std::memset(a_, 0, sizeof(a_));
    msg_len_ = 0;
    chunk_len_ = 0;
    return tmp_len;
}
template <uint32_t CAP, uint8_t PAD>
void SHA3Var<CAP, PAD>::GetHash(uint8_t* dst) {
    uint64_t tmp;
    uint32_t offset = 0;
    uint32_t i;
    uint64_t hl = hlen_ >> 6;
    for (i = 0; i < hl; ++i) {
        if (i - offset == 25) {
            offset = i;
            KeccakPermutation(a_);
        }
        tmp = a_[(i - offset) % 5][(i - offset) / 5];
        for (uint8_t j = 0; j < 8; ++j) {
            dst[(i << 3) + j] = tmp & 0xff;
            tmp >>= 8;
        }
    }
    if (i - offset == 25) {
        offset = i;
        KeccakPermutation(a_);
    }
    tmp = a_[(i - offset) % 5][(i - offset) / 5];
    hl = hlen_ >> 3;
    for (uint8_t j = 0; j < hl - (i << 3); ++j) {
        dst[(i << 3) + j] = (tmp & 255);
        tmp >>= 8;
    }
}
template class SHA3<224, 0x06>;
template class SHA3<256, 0x06>;
template class SHA3<384, 0x06>;
template class SHA3<512, 0x06>;
template class SHA3Var<128, 0x1f>;
template class SHA3Var<256, 0x1f>;
template class SHA3Var<128, 0x07>;
template class SHA3Var<256, 0x07>;
}  // namespace cryp
