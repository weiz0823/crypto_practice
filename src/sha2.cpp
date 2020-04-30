#include "sha2.hpp"
namespace cryp {
void SHA2_32::HashProcess() {
    uint32_t a[8];
    uint32_t w[64];
    uint32_t f1, f2;
    for (uint8_t i = 0; i < 8; ++i) a[i] = a0_[i];
    for (uint8_t i = 0; i < 64; ++i) {
        if (i < 16) {
            w[i] = uint32_t(msg_[4 * i + 3]) + uint32_t(msg_[4 * i + 2] << 8) +
                   uint32_t(msg_[4 * i + 1] << 16) +
                   uint32_t(msg_[4 * i] << 24);
        } else {
            w[i] = w[i - 16] + w[i - 7];
            w[i] += RightRotate32(w[i - 15], 7) ^ RightRotate32(w[i - 15], 18) ^
                    (w[i - 15] >> 3);
            w[i] += RightRotate32(w[i - 2], 17) ^ RightRotate32(w[i - 2], 19) ^
                    (w[i - 2] >> 10);
        }
        f1 = K[i] + w[i] + a[7];
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
    for (uint8_t i = 0; i < 8; ++i) a0_[i] += a[i];
}
uint64_t SHA2_32::HashUpdate(std::FILE* file) {
    if (!file) return msg_len_;
    uint64_t old_chunk = chunk_len_;
    chunk_len_ = std::fread(msg_ + old_chunk, 1, 64 - old_chunk, file);
    msg_len_ += chunk_len_ << 3;
    if (old_chunk + chunk_len_ < 64) return msg_len_;
    do {
        HashProcess();
        chunk_len_ = std::fread(msg_, 1, 64, file);
        msg_len_ += chunk_len_ << 3;
    } while (chunk_len_ >= 64);
    return msg_len_;
}
uint64_t SHA2_32::HashUpdate(const uint8_t* src, uint64_t bytelen) {
    if (!src) return msg_len_;
    uint64_t old_chunk = chunk_len_;
    ArrayIstream stream(src, bytelen);
    chunk_len_ = stream.Read(msg_ + old_chunk, 64 - old_chunk);
    msg_len_ += chunk_len_ << 3;
    if (old_chunk + chunk_len_ < 64) return msg_len_;
    do {
        HashProcess();
        chunk_len_ = stream.Read(msg_, 64);
        msg_len_ += chunk_len_ << 3;
    } while (chunk_len_ >= 64);
    return msg_len_;
}
uint64_t SHA2_32::HashFinal(uint8_t* dst) {
    uint64_t len_tmp = 0;
    // padding
    if (chunk_len_ < 56) {
        msg_[chunk_len_] = 0x80;
        std::memset(msg_ + chunk_len_ + 1, 0, 55 - chunk_len_);
        len_tmp = msg_len_;
        for (uint8_t i = 63; i >= 56; --i) {
            msg_[i] = len_tmp & 0xff;
            len_tmp >>= 8;
        }
        HashProcess();
    } else {
        msg_[chunk_len_] = 0x80;
        std::memset(msg_ + chunk_len_ + 1, 0, 63 - chunk_len_);
        chunk_len_ = 64;
        HashProcess();
        // length information in next chunk
        std::memset(msg_, 0, 56);
        len_tmp = msg_len_;
        for (uint8_t i = 63; i >= 56; --i) {
            msg_[i] = len_tmp & 0xff;
            len_tmp >>= 8;
        }
        HashProcess();
    }
    GetHash(dst);
    Reset();
    return msg_len_;
}

void SHA2_64::HashProcess() {
    uint64_t f1, f2;
    uint64_t w[80];
    uint64_t a[8];
    for (uint8_t i = 0; i < 8; ++i) a[i] = a0_[i];
    for (uint8_t i = 0; i < 80; ++i) {
        if (i < 16) {
            w[i] = uint64_t(msg_[8 * i]);
            for (uint8_t j = 1; j < 8; ++j) {
                w[i] <<= 8;
                w[i] += uint64_t(msg_[8 * i + j]);
            }
        } else {
            w[i] = w[i - 16] + w[i - 7];
            w[i] += RightRotate64(w[i - 15], 1) ^ RightRotate64(w[i - 15], 8) ^
                    (w[i - 15] >> 7);
            w[i] += RightRotate64(w[i - 2], 19) ^ RightRotate64(w[i - 2], 61) ^
                    (w[i - 2] >> 6);
        }
        f1 = K[i] + w[i] + a[7];
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
    for (uint8_t i = 0; i < 8; ++i) a0_[i] += a[i];
}
uint64_t SHA2_64::HashUpdate(std::FILE* file) {
    if (!file) return msg_len_;
    uint64_t old_chunk = chunk_len_;
    chunk_len_ = std::fread(msg_ + old_chunk, 1, 128 - old_chunk, file);
    msg_len_ += chunk_len_ << 3;
    if (old_chunk + chunk_len_ < 128) return msg_len_;
    do {
        HashProcess();
        chunk_len_ = std::fread(msg_, 1, 128, file);
        msg_len_ += chunk_len_ << 3;
    } while (chunk_len_ >= 128);
    return msg_len_;
}
uint64_t SHA2_64::HashUpdate(const uint8_t* src, uint64_t bytelen) {
    if (!src) return msg_len_;
    uint64_t old_chunk = chunk_len_;
    ArrayIstream stream(src, bytelen);
    chunk_len_ = stream.Read(msg_ + old_chunk, 128 - old_chunk);
    msg_len_ += chunk_len_ << 3;
    if (old_chunk + chunk_len_ < 128) return msg_len_;
    do {
        HashProcess();
        chunk_len_ = stream.Read(msg_, 128);
        msg_len_ += chunk_len_ << 3;
    } while (chunk_len_ >= 128);
    return msg_len_;
}
uint64_t SHA2_64::HashFinal(uint8_t* dst) {
    uint128_t len_tmp = 0;
    // padding
    if (chunk_len_ < 112) {
        msg_[chunk_len_] = 0x80;
        std::memset(msg_ + chunk_len_ + 1, 0, 111 - chunk_len_);
        len_tmp = msg_len_;
        for (uint8_t i = 127; i >= 112; --i) {
            msg_[i] = len_tmp & 0xff;
            len_tmp >>= 8;
        }
        HashProcess();
    } else {
        msg_[chunk_len_] = 0x80;
        std::memset(msg_ + chunk_len_ + 1, 0, 127 - chunk_len_);
        chunk_len_ = 128;
        HashProcess();
        // length information in next chunk
        std::memset(msg_, 0, 112);
        len_tmp = msg_len_;
        for (uint8_t i = 127; i >= 112; --i) {
            msg_[i] = len_tmp & 0xff;
            len_tmp >>= 8;
        }
        HashProcess();
    }
    GetHash(dst);
    Reset();
    return msg_len_;
}

void SHA224::GetHash(uint8_t* dst) {
    // discard last
    for (int i = 0; i < 7; ++i)
        for (int j = 3; j >= 0; --j) {
            dst[4 * i + j] = a0_[i] & 0xff;
            a0_[i] >>= 8;
        }
}
void SHA224::Reset() {
    static constexpr uint32_t a[8] = {0xc1059ed8, 0x367cd507, 0x3070dd17,
                                      0xf70e5939, 0xffc00b31, 0x68581511,
                                      0x64f98fa7, 0xbefa4fa4};
    for (uint8_t i = 0; i < 8; ++i) a0_[i] = a[i];
    msg_len_ = 0;
    chunk_len_ = 0;
}

void SHA256::GetHash(uint8_t* dst) {
    for (int i = 0; i < 8; ++i)
        for (int j = 3; j >= 0; --j) {
            dst[4 * i + j] = a0_[i] & 0xff;
            a0_[i] >>= 8;
        }
}
void SHA256::Reset() {
    static constexpr uint32_t a[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372,
                                      0xa54ff53a, 0x510e527f, 0x9b05688c,
                                      0x1f83d9ab, 0x5be0cd19};
    for (uint8_t i = 0; i < 8; ++i) a0_[i] = a[i];
    msg_len_ = 0;
    chunk_len_ = 0;
}

void SHA384::GetHash(uint8_t* dst) {
    for (int i = 0; i < 6; ++i)
        for (int j = 7; j >= 0; --j) {
            dst[8 * i + j] = a0_[i] & 0xff;
            a0_[i] >>= 8;
        }
}
void SHA384::Reset() {
    static constexpr uint64_t a[8] = {0xcbbb9d5dc1059ed8, 0x629a292a367cd507,
                                      0x9159015a3070dd17, 0x152fecd8f70e5939,
                                      0x67332667ffc00b31, 0x8eb44a8768581511,
                                      0xdb0c2e0d64f98fa7, 0x47b5481dbefa4fa4};
    for (uint8_t i = 0; i < 8; ++i) a0_[i] = a[i];
    msg_len_ = 0;
    chunk_len_ = 0;
}

void SHA512::GetHash(uint8_t* dst) {
    for (int i = 0; i < 8; ++i)
        for (int j = 7; j >= 0; --j) {
            dst[8 * i + j] = a0_[i] & 0xff;
            a0_[i] >>= 8;
        }
}
void SHA512::Reset() {
    static constexpr uint64_t a[8] = {0x6a09e667f3bcc908, 0xbb67ae8584caa73b,
                                      0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
                                      0x510e527fade682d1, 0x9b05688c2b3e6c1f,
                                      0x1f83d9abfb41bd6b, 0x5be0cd19137e2179};
    for (uint8_t i = 0; i < 8; ++i) a0_[i] = a[i];
    msg_len_ = 0;
    chunk_len_ = 0;
}

void SHA512t::GetHash(uint8_t* dst) {
    uint64_t t = hlen_;
    t = (t + 7) >> 3;
    if (t > 64) t = 64;
    uint32_t i;
    for (i = 0; i < (t >> 3); ++i)
        for (uint8_t j = 7; j != uint8_t(-1); --j) {
            dst[(i << 3) + j] = a0_[i] & 255;
            a0_[i] >>= 8;
        }
    uint8_t j = 7;
    for (; j != uint8_t(t - 1 - (i << 3)); --j) a0_[i] >>= 8;
    for (; j != uint8_t(-1); --j) {
        dst[(i << 3) + j] = a0_[i] & 255;
        a0_[i] >>= 8;
    }
}
void SHA512t::Reset() {
    std::memcpy(a0_, iv_, 64);
    msg_len_ = chunk_len_ = 0;
}
SHA512t::SHA512t(uint64_t t)
    : SHA2_64(t == 224 ? id_sha512_224 : t == 256 ? id_sha512_256 : id_unknown,
              t > 512 ? 512 : t) {
    std::memcpy(a0_, iv_, 64);
    // do a hash to calculate iv
    char s[32];
    std::sprintf(s, "SHA-512/%llu", t);
    HashUpdate(reinterpret_cast<uint8_t*>(s), std::strlen(s));
    msg_[chunk_len_] = 0x80;
    std::memset(msg_ + chunk_len_ + 1, 0, 111 - chunk_len_);
    uint128_t len_tmp = msg_len_;
    for (uint8_t i = 127; i >= 112; --i) {
        msg_[i] = len_tmp & 0xff;
        len_tmp >>= 8;
    }
    HashProcess();
    std::memcpy(iv_, a0_, 64);
    Reset();
}
}  // namespace cryp
