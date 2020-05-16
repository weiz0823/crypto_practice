#pragma once
#include <cstring>
#include <iostream>

#include "array_stream.hpp"
#include "asn1.hpp"
#include "bitrotate.hpp"
namespace cryp {
using OID = ASN1::OID;
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
class ASN1_HashAlgorithm : public ASN1::Sequence {
   protected:
    const ASN1::OID* hash_id_;
    BytesT code_;
    // param: null, use global null object
   public:
    explicit ASN1_HashAlgorithm(const OID* hash_id) : hash_id_(hash_id) {
        hash_id_->Encode(&code_);
        g_asn1_null.Encode(&code_);
        auto v = Wrap(code_);
        code_.insert(code_.begin(), v.begin(), v.end());
    }
    void Encode(BytesT* dst) const { *dst += code_; }
    int Decode(ASN1::OID* oid, BytesT::const_iterator pos,
               BytesT::const_iterator max_pos,
               BytesT::const_iterator* end_pos = nullptr) const {
        if (end_pos) *end_pos = pos;
        if (pos + 1 > max_pos) return -1;
        if (Unwrap(pos, max_pos, &pos, &max_pos)) return -1;
        if (oid->Decode(pos, max_pos, &pos)) return -1;
        if (g_asn1_null.Decode(pos, max_pos, &pos)) return -1;
        if (pos != max_pos) return -1;
        if (end_pos) *end_pos = pos;
        return 0;
    }
};
class ASN1_DigestInfo : public ASN1::Sequence {
   protected:
    const ASN1_HashAlgorithm* hash_alg_;
    ASN1::OctetString digest_;
    BytesT code_;

   public:
    explicit ASN1_DigestInfo(const ASN1_HashAlgorithm* hash_alg)
        : hash_alg_(hash_alg) {}
    void SetDigest(const ByteT* data, LenT len) {
        digest_.Set(data, len);
        code_.clear();
        hash_alg_->Encode(&code_);
        digest_.Encode(&code_);
        auto v = Wrap(code_);
        code_.insert(code_.begin(), v.begin(), v.end());
    }
    void Encode(BytesT* dst) const { *dst += code_; }
    int Decode(ASN1::OID* oid, BytesT* digest, BytesT::const_iterator pos,
               BytesT::const_iterator max_pos,
               BytesT::const_iterator* end_pos = nullptr) const {
        if (end_pos) *end_pos = pos;
        if (pos + 1 > max_pos) return -1;
        if (Unwrap(pos, max_pos, &pos, &max_pos)) return -1;
        if (hash_alg_->Decode(oid, pos, max_pos, &pos)) return -1;
        if (digest_.Decode(digest, pos, max_pos, &pos)) return -1;
        if (pos != max_pos) return -1;
        if (end_pos) *end_pos = pos;
        return 0;
    }
};
}  // namespace cryp
