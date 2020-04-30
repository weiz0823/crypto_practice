#pragma once
#include "hash.hpp"

namespace cryp {
inline const OID id_md5("id-md5", id_digest, "2", "MD5");
// Standards: RFC 1321: MD5 Message-digest
//            RFC 6151: Updated Security Considerations on MD5
// Not anti-collision, but can check for unintentional data modification
class MD5 final : public SecureHashFunc {
   private:
    // state
    uint32_t a0_ = 0x67452301;
    uint32_t b0_ = 0xefcdab89;
    uint32_t c0_ = 0x98badcfe;
    uint32_t d0_ = 0x10325476;
    // temporary save
    uint64_t msg_len_ = 0, chunk_len_ = 0;
    // 512 bits
    uint8_t msg_[64];
    // precomputed constants
    static constexpr uint8_t S[64] = {
        7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
        5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20,
        4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
        6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};
    static constexpr uint32_t K[64] = {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a,
        0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
        0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340,
        0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8,
        0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
        0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa,
        0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92,
        0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
        0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};
    // core, modify state vector by message block
    void HashProcess();

   public:
    MD5() : SecureHashFunc(id_md5, 128) {}
    uint64_t HashUpdate(std::FILE* file) override;
    uint64_t HashUpdate(const uint8_t* src, uint64_t bytelen) override;
    uint64_t HashFinal(uint8_t* dst) override;
};
}  // namespace cryp
