#include "sha1.hpp"
namespace cryp {
void SHA1::HashProcess() {
    uint32_t a, b, c, d, e;
    uint32_t f;
    uint32_t j;
    uint32_t w[80];
    a = a0_;
    b = b0_;
    c = c0_;
    d = d0_;
    e = e0_;
    for (uint8_t i = 0; i < 80; ++i) {
        if (i < 16) {
            w[i] = uint32_t(msg_[4 * i + 3]) + uint32_t(msg_[4 * i + 2] << 8) +
                   uint32_t(msg_[4 * i + 1] << 16) +
                   uint32_t(msg_[4 * i] << 24);
        } else {
            w[i] = LeftRotate32(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
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
        f += e + LeftRotate32(a, 5) + j + w[i];
        e = d;
        d = c;
        c = LeftRotate32(b, 30);
        b = a;
        a = f;
    }
    a0_ += a;
    b0_ += b;
    c0_ += c;
    d0_ += d;
    e0_ += e;
}
uint64_t SHA1::HashUpdate(std::FILE* file) {
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
uint64_t SHA1::HashUpdate(const uint8_t* src, uint64_t bytelen) {
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
uint64_t SHA1::HashFinal(uint8_t* dst) {
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
    for (uint8_t i = 3; i != uint8_t(-1); --i) {
        dst[i] = a0_ & 255;
        dst[i + 4] = b0_ & 255;
        dst[i + 8] = c0_ & 255;
        dst[i + 12] = d0_ & 255;
        dst[i + 16] = e0_ & 255;
        a0_ >>= 8;
        b0_ >>= 8;
        c0_ >>= 8;
        d0_ >>= 8;
        e0_ >>= 8;
    }
    a0_ = 0x67452301;
    b0_ = 0xefcdab89;
    c0_ = 0x98badcfe;
    d0_ = 0x10325476;
    e0_ = 0xc3d2e1f0;
    return msg_len_;
}
}  // namespace cryp
