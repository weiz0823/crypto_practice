#pragma once
#include "bigint64.hpp"
#include "hash.hpp"
#include "mgf.hpp"
#include "msg_encode.hpp"
namespace cryp {
using BI = calc::BigInt<calc::uint128_t>;
class EMEOAEP final : public MsgEncode {
    SecureHashFunc* hash_;
    MaskGenFunc* mgf_;

   public:
    explicit EMEOAEP(SecureHashFunc* hash = nullptr, MaskGenFunc* mgf = nullptr)
        : hash_(hash), mgf_(mgf) {}
    inline void SetHashFunc(SecureHashFunc* hash) { hash_ = hash; }
    inline void SetMaskGenFunc(MaskGenFunc* mgf) { mgf_ = mgf; }
    int Encode(const ByteT* msg, LenT msg_len, const ByteT* label,
               LenT label_len, ByteT* dst, LenT dst_len);
    inline int Encode(const ByteT* msg, LenT msg_len, ByteT* dst,
                      LenT dst_len) override {
        return Encode(msg, msg_len, nullptr, 0, dst, dst_len);
    }
    int Decode(const ByteT* encoded, LenT src_len, const ByteT* label,
               LenT label_len, BytesT* dst);
    inline int Decode(const ByteT* encoded, LenT src_len,
                      BytesT* dst) override {
        return Decode(encoded, src_len, nullptr, 0, dst);
    }
};
}  // namespace cryp
