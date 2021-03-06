#pragma once
#include "hash.hpp"
namespace cryp {
using uint128_t = __uint128_t;
inline const OID id_sha224("id-sha224", id_nist_hash, "4", "SHA224");
inline const OID id_sha256("id-sha256", id_nist_hash, "1", "SHA256");
inline const OID id_sha384("id-sha384", id_nist_hash, "2", "SHA384");
inline const OID id_sha512("id-sha512", id_nist_hash, "3", "SHA512");
inline const OID id_sha512_224("id-sha512-224", id_nist_hash, "5",
                               "SHA512-224");
inline const OID id_sha512_256("id-sha512-256", id_nist_hash, "6",
                               "SHA512-256");
inline const ASN1_HashAlgorithm asn1_sha224(&id_sha224);
inline const ASN1_HashAlgorithm asn1_sha256(&id_sha256);
inline const ASN1_HashAlgorithm asn1_sha384(&id_sha384);
inline const ASN1_HashAlgorithm asn1_sha512(&id_sha512);
inline const ASN1_HashAlgorithm asn1_sha512_224(&id_sha512_224);
inline const ASN1_HashAlgorithm asn1_sha512_256(&id_sha512_256);
// SHA-2 Family
// Standard: FIPS 180: Secure Hash Standard
// 32-bit based on SHA256
class SHA2_32 : public SecureHashFunc {
   protected:
    static constexpr uint32_t K[64] = {
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
    uint32_t a0_[8];
    uint64_t msg_len_ = 0, chunk_len_ = 0;
    uint8_t msg_[64];
    void HashProcess();
    virtual void GetHash(uint8_t* dst) = 0;
    virtual void Reset() = 0;

   public:
    SHA2_32(OID alg_id, uint64_t hashlen)
        : SecureHashFunc(std::move(alg_id), hashlen) {}
    uint64_t HashUpdate(std::FILE* file) override;
    uint64_t HashUpdate(const uint8_t* src, uint64_t bytelen) override;
    uint64_t HashFinal(uint8_t* dst) override;
};
// 64-bit based ont SHA512
class SHA2_64 : public SecureHashFunc {
   protected:
    static constexpr uint64_t K[80] = {
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
    uint64_t a0_[8];
    uint128_t msg_len_ = 0;
    uint64_t chunk_len_ = 0;
    uint8_t msg_[128];
    void HashProcess();
    virtual void GetHash(uint8_t* dst) = 0;
    virtual void Reset() = 0;

   public:
    SHA2_64(OID alg_id, uint64_t hashlen)
        : SecureHashFunc(std::move(alg_id), hashlen) {}
    uint64_t HashUpdate(std::FILE* file) override;
    uint64_t HashUpdate(const uint8_t* src, uint64_t bytelen) override;
    uint64_t HashFinal(uint8_t* dst) override;
};
class SHA224 final : public SHA2_32 {
    void GetHash(uint8_t* dst) override;
    void Reset() override;

   public:
    SHA224() : SHA2_32(id_sha224, 224) { Reset(); }
};
class SHA256 final : public SHA2_32 {
    void GetHash(uint8_t* dst) override;
    void Reset() override;

   public:
    SHA256() : SHA2_32(id_sha256, 256) { Reset(); }
};
class SHA384 final : public SHA2_64 {
    void GetHash(uint8_t* dst) override;
    void Reset() override;

   public:
    SHA384() : SHA2_64(id_sha384, 384) { Reset(); }
};
class SHA512 final : public SHA2_64 {
    void GetHash(uint8_t* dst) override;
    void Reset() override;

   public:
    SHA512() : SHA2_64(id_sha512, 512) { Reset(); }
};
class SHA512t final : public SHA2_64 {
    void GetHash(uint8_t* dst) override;
    void Reset() override;
    uint64_t iv_[8] = {0xcfac43c256196cad, 0x1ec20b20216f029e,
                       0x99cb56d75b315d8e, 0x00ea509ffab89354,
                       0xf4abf7da08432774, 0x3ea0cd298e9bc9ba,
                       0xba267c0e5ee418ce, 0xfe4568bcb6db84dc};

   public:
    explicit SHA512t(uint64_t t);
};
inline SHA224 g_sha224;
inline SHA256 g_sha256;
inline SHA384 g_sha384;
inline SHA512 g_sha512;
inline SHA512t g_sha512_224(224);
inline SHA512t g_sha512_256(256);
}  // namespace cryp
