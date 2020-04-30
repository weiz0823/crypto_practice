#pragma once
#include "hash.hpp"
namespace cryp {
inline const OID id_sha3_224("id-sha3-224", id_nist_hash, "7", "SHA3-224");
inline const OID id_sha3_256("id-sha3-256", id_nist_hash, "8", "SHA3-256");
inline const OID id_sha3_384("id-sha3-384", id_nist_hash, "9", "SHA3-384");
inline const OID id_sha3_512("id-sha3-512", id_nist_hash, "10", "SHA3-512");
inline const OID id_shake128("id-shake128", id_nist_hash, "11", "SHAKE128");
inline const OID id_shake256("id-shake256", id_nist_hash, "12", "SHAKE256");
inline const OID id_shake128_len("id-shake128-len", id_nist_hash, "17",
                                 "SHAKE128-len");
inline const OID id_shake256_len("id-shake256-len", id_nist_hash, "18",
                                 "SHAKE256-len");
// Secure Hash Function SHA-3
// Standard: FIPS 202: SHA-3
template <uint32_t CAP, uint8_t PAD>
class SHA3Var : public SecureHashFunc {
   protected:
    static constexpr uint32_t RATE = 200 - (CAP >> 2);
    uint64_t a_[5][5];
    uint64_t msg_len_ = 0, chunk_len_ = 0;
    uint8_t msg_[RATE];
    void HashProcess();
    void GetHash(uint8_t* dst);
    OID GetOID(uint64_t t) {
        if constexpr (PAD == 0x06) {
            if constexpr (CAP == 224) return id_sha3_224;
            if constexpr (CAP == 256) return id_sha3_256;
            if constexpr (CAP == 384) return id_sha3_384;
            if constexpr (CAP == 512) return id_sha3_512;
        }
        if constexpr (PAD == 0x1f) {
            if constexpr (CAP == 128) {
                if (t == 128)
                    return id_shake128;
                else
                    return id_shake128_len;
            }
            if constexpr (CAP == 256) {
                if (t == 256)
                    return id_shake256;
                else
                    return id_shake256_len;
            }
        }
        return id_unknown;
    }

   public:
    explicit SHA3Var(uint64_t t) : SecureHashFunc(GetOID(t), t) {
        std::memset(a_, 0, sizeof(a_));
    }
    uint64_t HashUpdate(std::FILE* file) override;
    uint64_t HashUpdate(const uint8_t* src, uint64_t bytelen) override;
    uint64_t HashFinal(uint8_t* dst) override;
    inline void SetHashLen(uint64_t hl) { hlen_ = hl; }
};
template <uint32_t CAP, uint8_t PAD>
class SHA3 final : public SHA3Var<CAP, PAD> {
   public:
    SHA3() : SHA3Var<CAP, PAD>(CAP) {}
};
extern template class SHA3<224, 0x06>;
extern template class SHA3<256, 0x06>;
extern template class SHA3<384, 0x06>;
extern template class SHA3<512, 0x06>;
extern template class SHA3Var<128, 0x1f>;
extern template class SHA3Var<256, 0x1f>;
extern template class SHA3Var<128, 0x07>;
extern template class SHA3Var<256, 0x07>;
using SHA3_224 = SHA3<224, 0x06>;
using SHA3_256 = SHA3<256, 0x06>;
using SHA3_384 = SHA3<384, 0x06>;
using SHA3_512 = SHA3<512, 0x06>;
using SHAKE128 = SHA3Var<128, 0x1f>;
using SHAKE256 = SHA3Var<256, 0x1f>;
using RawSHAKE128 = SHA3Var<128, 0x07>;
using RawSHAKE256 = SHA3Var<256, 0x07>;
}  // namespace cryp
