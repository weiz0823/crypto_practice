#pragma once
#include "asn1.hpp"
#include "type_aliases.hpp"
namespace cryp {
using OID = ASN1::OID;
// Standards: FIPS 186: Digital Signature Standard (DSS)
// public key cryptography abstract class
class PKCPrivate {
   protected:
    OID oid_;
    LenT keylen_;

   public:
    PKCPrivate(OID alg_id, LenT key_len)
        : oid_(std::move(alg_id)), keylen_(key_len) {}
    inline LenT KeyLen() const { return keylen_; }
    virtual BytesT Decrypt(const ByteT* cipher, LenT len) = 0;
    virtual BytesT Sign(const ByteT* msg, LenT len) = 0;
};
class PKCPublic {
   protected:
    OID oid_;
    LenT keylen_;

   public:
    PKCPublic(OID alg_id, LenT key_len)
        : oid_(std::move(alg_id)), keylen_(key_len) {}
    inline LenT KeyLen() const { return keylen_; }
    virtual BytesT Encrypt(const ByteT* msg, LenT len) = 0;
    virtual int Verify(const ByteT* msg, LenT msg_len, const ByteT* sign,
                       LenT sign_len) = 0;
};
}  // namespace cryp
