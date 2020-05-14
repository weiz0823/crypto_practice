#pragma once
#include "hash.hpp"
#include "mgf.hpp"
#include "mgf1.hpp"
#include "random_engine.hpp"
#include "pubkey_sign_encode.hpp"
namespace cryp {
class EMSAPSS final : public SignEncode {
    SecureHashFunc* hash_;
    MaskGenFunc* mgf_;
    RandomNumberGen<ByteT>* rnd_;
    LenT salt_len_;
    // compatibility, MUST be 0xbc for PKCS#1
    ByteT trailer_field_ = 0xbc;

   public:
    explicit EMSAPSS(SecureHashFunc* hash = &g_sha1,
                     MaskGenFunc* mgf = &g_mgf1_sha1, LenT slen = 20,
                     RandomNumberGen<ByteT>* rnd = &g_rng32)
        : hash_(hash), mgf_(mgf), rnd_(rnd), salt_len_(slen) {}
    void SetHashFunc(SecureHashFunc* hash) { hash_ = hash; }
    void SetMaskGenFunc(MaskGenFunc* mgf) { mgf_ = mgf; }
    void SetRandomNumberGen(RandomNumberGen<ByteT>* rnd) { rnd_ = rnd; }
    void SetSaltLen(LenT slen) { salt_len_ = slen; }
    int Encode(const ByteT* msg, LenT msg_len, ByteT* dst,
               LenT dst_len) override;
    int Verify(const ByteT* msg, LenT msg_len, const ByteT* encoded,
               LenT src_len) override;
};
}  // namespace cryp
