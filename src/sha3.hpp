#pragma once
#include "hash.hpp"
namespace cryp {
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

   public:
    SHA3Var(uint64_t t) : SecureHashFunc(t) { std::memset(a_, 0, sizeof(a_)); }
    virtual uint64_t HashUpdate(std::FILE* file) override;
    virtual uint64_t HashUpdate(const uint8_t* src, uint64_t bytelen) override;
    virtual uint64_t HashFinal(uint8_t* dst) override;
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
typedef SHA3<224, 0x06> SHA3_224;
typedef SHA3<256, 0x06> SHA3_256;
typedef SHA3<384, 0x06> SHA3_384;
typedef SHA3<512, 0x06> SHA3_512;
typedef SHA3Var<128, 0x1f> SHAKE128;
typedef SHA3Var<256, 0x1f> SHAKE256;
typedef SHA3Var<128, 0x07> RawSHAKE128;
typedef SHA3Var<256, 0x07> RawSHAKE256;
}  // namespace cryp
