#pragma once
#include "hash.hpp"

namespace cryp {
inline const OID id_sha1("id-sha1", id_secsig_alg, "26", "SHA1");
// Secure Hash Function SHA-1
// Standard: FIPS 180: Secure Hash Standard (SHS)
class SHA1 final : public SecureHashFunc {
    uint32_t a0_ = 0x67452301;
    uint32_t b0_ = 0xefcdab89;
    uint32_t c0_ = 0x98badcfe;
    uint32_t d0_ = 0x10325476;
    uint32_t e0_ = 0xc3d2e1f0;
    uint64_t msg_len_ = 0, chunk_len_ = 0;
    uint8_t msg_[64];
    // core, modify state vector by message block
    void HashProcess();

   public:
    SHA1() : SecureHashFunc(id_sha1, 160) {}
    uint64_t HashUpdate(std::FILE* file) override;
    uint64_t HashUpdate(const uint8_t* src, uint64_t bytelen) override;
    uint64_t HashFinal(uint8_t* dst) override;
};
inline SHA1 g_sha1;
}  // namespace cryp
