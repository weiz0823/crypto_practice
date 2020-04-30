#pragma once
#include <cstdint>
#include "oid.hpp"
namespace cryp {
// Standards: FIPS 186: Digital Signature Standard (DSS)
// public key cryptography abstract class
class PublicKeyCrypto {
   protected:
    OID oid_;
    uint64_t keylen_;

   public:
    PublicKeyCrypto(OID alg_id, uint64_t key_len)
        : oid_(std::move(alg_id)), keylen_(key_len) {}
};
}  // namespace cryp
