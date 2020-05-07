#pragma once
#include "hash.hpp"
#include "mgf.hpp"
#include "oid.hpp"
#include "rsa.hpp"
namespace cryp {
inline const OID id_mgf1("id-mgf1", id_pkcs_1, "8", "MGF1");
class MGF1 final : public MaskGenFunc {
    SecureHashFunc* hash_;

   public:
    explicit MGF1(SecureHashFunc* hash) : hash_(hash) {}
    int Generate(const uint8_t* src, uint64_t src_len, uint8_t* dst,
                 uint64_t dst_len) override;
};
}  // namespace cryp
