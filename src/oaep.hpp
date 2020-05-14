#pragma once
#include "hash.hpp"
#include "mgf.hpp"
#include "mgf1.hpp"
#include "pubkey_msg_encode.hpp"
#include "random_engine.hpp"
namespace cryp {
class EMEOAEP final : public MsgEncode {
    SecureHashFunc* hash_;
    MaskGenFunc* mgf_;
    RandomNumberGen<ByteT>* rnd_;

   public:
    explicit EMEOAEP(SecureHashFunc* hash = &g_sha1,
                     MaskGenFunc* mgf = &g_mgf1_sha1,
                     RandomNumberGen<ByteT>* rnd = &g_rng32)
        : hash_(hash), mgf_(mgf), rnd_(rnd) {}
    void SetHashFunc(SecureHashFunc* hash) { hash_ = hash; }
    void SetMaskGenFunc(MaskGenFunc* mgf) { mgf_ = mgf; }
    void SetRandomNumberGen(RandomNumberGen<ByteT>* rnd) { rnd_ = rnd; }
    int Encode(const ByteT* msg, LenT msg_len, const ByteT* label,
               LenT label_len, ByteT* dst, LenT dst_len);
    int Encode(const ByteT* msg, LenT msg_len, ByteT* dst,
               LenT dst_len) override {
        return Encode(msg, msg_len, nullptr, 0, dst, dst_len);
    }
    int Decode(const ByteT* encoded, LenT src_len, const ByteT* label,
               LenT label_len, BytesT* dst);
    int Decode(const ByteT* encoded, LenT src_len, BytesT* dst) override {
        return Decode(encoded, src_len, nullptr, 0, dst);
    }
};
}  // namespace cryp
