#pragma once
#include "hash.hpp"
#include "mgf.hpp"
#include "oid.hpp"
#include "sha1.hpp"
namespace cryp {
inline const OID id_pkcs_1("pkcs-1", "1.2.840.113549.1.1",
                           "/ISO/Member-Body/US/RSADSI/PKCS/PKCS-1");
inline const OID id_mgf1("id-mgf1", id_pkcs_1, "8", "MGF1");
class MGF1 final : public MaskGenFunc {
    SecureHashFunc* hash_;

   public:
    explicit MGF1(SecureHashFunc* hash) : hash_(hash) {}
    int Generate(const ByteT* src, LenT src_len, ByteT* dst,
                 LenT dst_len) override;
};
inline MGF1 g_mgf1_sha1(&g_sha1);
}  // namespace cryp
