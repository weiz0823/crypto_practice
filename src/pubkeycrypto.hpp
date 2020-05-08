#pragma once
#include <cstdint>
#include <vector>
#include "oid.hpp"
namespace cryp {
// Standards: FIPS 186: Digital Signature Standard (DSS)
// public key cryptography abstract class
using Byte = uint8_t;
using Bytes = std::vector<Byte>;
class PKCPrivate {
   protected:
    OID oid_;
    uint64_t keylen_;

   public:
    PKCPrivate(OID alg_id, uint64_t key_len)
        : oid_(std::move(alg_id)), keylen_(key_len) {}
    virtual Bytes Decrypt(const uint8_t* cipher, uint64_t len) = 0;
    virtual Bytes Sign(const uint8_t* msg, uint64_t len) = 0;
};
class PKCPublic {
   protected:
    OID oid_;
    uint64_t keylen_;

   public:
    PKCPublic(OID alg_id, uint64_t key_len)
        : oid_(std::move(alg_id)), keylen_(key_len) {}
    virtual Bytes Encrypt(const uint8_t* msg, uint64_t len) = 0;
    virtual Bytes Verify(const uint8_t* sign, uint64_t len) = 0;
};
}  // namespace cryp
