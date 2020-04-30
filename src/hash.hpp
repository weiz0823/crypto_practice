#pragma once
#include <cstring>
#include <iostream>

#include "array_stream.hpp"
#include "bitrotate.hpp"
#include "oid.hpp"
namespace cryp {
inline const OID id_digest("digestAlgorithm", "1.2.840.113549.2",
                           "/ISO/Member-Body/US/RSADSI/DigestAlgorithm");
inline const OID id_nist_hash(
    "hashalgs", "2.16.840.1.101.3.4.2",
    "/Joint-ISO-ITU-T/Country/US/Organization/gov/CSOR/NISTAlgorithm/HashAlgs");
inline const OID id_secsig_alg(
    "algorithms", "1.3.14.3.2",
    "/ISO/Identified-Organization/OIW/SecSIG/Algorithms");
// Cryptographic Secure Hash Functions
// Message and hash value always byte-aligned
class SecureHashFunc {
   protected:
    OID oid_;
    uint64_t hlen_;  // bitlen of hash value

   public:
    SecureHashFunc(OID alg_id, uint64_t hashlen)
        : oid_(std::move(alg_id)), hlen_(hashlen) {}
    // hash without final padding, and don't get a final hash value
    // hash a (possibly large) file to an octet string (array of bytes)
    // return bitlen of hashed file
    virtual uint64_t HashUpdate(std::FILE* file) = 0;
    // hash array of bytes without padding
    virtual uint64_t HashUpdate(const uint8_t* src, uint64_t bytelen) = 0;
    // hash with final padding, output hash value, and reset to default state
    // okay to 'read' from nullptr
    virtual uint64_t HashFinal(uint8_t* dst) = 0;
    inline virtual uint64_t Hash(std::FILE* file, uint8_t* dst) {
        HashUpdate(file);
        return HashFinal(dst);
    }
    inline virtual uint64_t Hash(const uint8_t* src, uint64_t bytelen,
                                 uint8_t* dst) {
        HashUpdate(src, bytelen);
        return HashFinal(dst);
    }
    // hash length read-only
    inline uint64_t HashLen() const { return hlen_; }
    virtual ~SecureHashFunc() = default;
};
}  // namespace cryp
