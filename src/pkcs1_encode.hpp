#pragma once
#include <iostream>
#include "pubkey_msg_encode.hpp"
#include "random_engine.hpp"
namespace cryp {
class EMEPKCS1 final : public MsgEncode {
    RandomNumberGen<ByteT>* rnd_;

   public:
    explicit EMEPKCS1(RandomNumberGen<ByteT>* rnd = &g_rng32) : rnd_(rnd) {}
    void SetRandomNumberGen(RandomNumberGen<ByteT>* rnd) { rnd_ = rnd; }
    int Encode(const ByteT* msg, LenT msg_len, ByteT* dst,
               LenT dst_len) override;
    int Decode(const ByteT* encoded, LenT src_len, BytesT* dst) override;
};
}  // namespace cryp
