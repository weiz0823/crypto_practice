#include "md5.hpp"
namespace cryp {
void MD5::HashProcess() {
    uint32_t a, b, c, d;
    a = a0_;
    b = b0_;
    c = c0_;
    d = d0_;
    uint32_t f;
    uint32_t j;
    for (uint8_t i = 0; i < 64; ++i) {
        if (i < 16) {
            f = (b & c) | (~b & d);
            j = i;
        } else if (i < 32) {
            f = (d & b) | (~d & c);
            j = uint32_t(i << 2) + i + 1;
        } else if (i < 48) {
            f = b ^ c ^ d;
            j = i + i + i + 5;
        } else {
            f = c ^ (b | ~d);
            j = uint32_t(i << 3) - i;
        }
        j = (j & 15) << 2;
        f += a + msg_[j] + uint32_t(msg_[j + 1] << 8) +
             uint32_t(msg_[j + 2] << 16) + uint32_t(msg_[j + 3] << 24) + K[i];
        a = d;
        d = c;
        c = b;
        b += (f << S[i]) | (f >> (32 - S[i]));
    }
    a0_ += a;
    b0_ += b;
    c0_ += c;
    d0_ += d;
}
uint64_t MD5::HashUpdate(std::FILE* file) {
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
uint64_t MD5::HashUpdate(const uint8_t* src, uint64_t bytelen) {
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
uint64_t MD5::HashFinal(uint8_t* dst) {
    uint64_t len_tmp = 0;
    // padding
    if (chunk_len_ < 56) {
        msg_[chunk_len_] = 0x80;
        std::memset(msg_ + chunk_len_ + 1, 0, 55 - chunk_len_);
        len_tmp = msg_len_;
        for (uint8_t i = 56; i < 64; ++i) {
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
        for (uint8_t i = 56; i < 64; ++i) {
            msg_[i] = len_tmp & 0xff;
            len_tmp >>= 8;
        }
        HashProcess();
    }
    // convert state vector to octet string
    for (uint8_t i = 0; i < 4; ++i) {
        dst[i] = a0_ & 0xff;
        dst[i + 4] = b0_ & 0xff;
        dst[i + 8] = c0_ & 0xff;
        dst[i + 12] = d0_ & 0xff;
        a0_ >>= 8;
        b0_ >>= 8;
        c0_ >>= 8;
        d0_ >>= 8;
    }
    // reset to default state
    a0_ = 0x67452301;
    b0_ = 0xefcdab89;
    c0_ = 0x98badcfe;
    d0_ = 0x10325476;
    len_tmp = msg_len_;
    msg_len_ = chunk_len_ = 0;
    // msg_ no need to reset
    return len_tmp;
}
}  // namespace cryp
