#pragma once
#include "asn1.hpp"
#include "bin2text.hpp"
#include "bytes_mpint.hpp"
#include "emsapss.hpp"
#include "hash.hpp"
#include "mgf.hpp"
#include "mgf1.hpp"
#include "oaep.hpp"
#include "pkcs1_encode.hpp"
#include "pubkeycrypto.hpp"
#include "serialize.hpp"

namespace cryp {
using OID = ASN1::OID;
using BI = calc::BigInt<calc::uint128_t>;

enum RSAPubKeyFmt { kPKCS, kSSH, kRFC3279, kPEM };
enum RSAScheme {
    kRSAEncryption,
    kRSA_MD5,
    kRSA_SHA1,
    kRSA_SHA224,
    kRSA_SHA256,
    kRSA_SHA384,
    kRSA_SHA512,
    kRSA_SHA512_224,
    kRSA_SHA512_256,
    kRSAES_OAEP,
    kRSASSA_PSS,
};

inline const OID id_rsa_encryption("rsaEncryption", id_pkcs_1, "1",
                                   "RSAEncryption");
inline const OID id_rsaes_oaep("id-RSAES-OAEP", id_pkcs_1, "7", "RSAES-OAEP");
inline const OID id_rsassa_pss("id-RSASSA-PSS", id_pkcs_1, "10", "RSASSA-PSS");
inline const OID id_rsa_md5("md5WithRSAEncryption", id_pkcs_1, "4",
                            "MD5WithRSAEncryption");
inline const OID id_rsa_sha1("sha1WithRSAEncryption", id_pkcs_1, "5",
                             "SHA1WithRSAEncryption");
inline const OID id_rsa_sha224("sha224WithRSAEncryption", id_pkcs_1, "14",
                               "SHA224WithRSAEncryption");
inline const OID id_rsa_sha256("sha256WithRSAEncryption", id_pkcs_1, "11",
                               "SHA256WithRSAEncryption");
inline const OID id_rsa_sha384("sha384WithRSAEncryption", id_pkcs_1, "12",
                               "SHA384WithRSAEncryption");
inline const OID id_rsa_sha512("sha512WithRSAEncryption", id_pkcs_1, "13",
                               "SHA512WithRSAEncryption");
inline const OID id_rsa_sha512_224("sha512-224WithRSAEncryption", id_pkcs_1,
                                   "15", "SHA512-224WithRSAEncryption");
inline const OID id_rsa_sha512_256("sha512-256WithRSAEncryption", id_pkcs_1,
                                   "16", "SHA512-256WithRSAEncryption");
// Standards: RFC 8017: PKCS #1
// only partially follow PKCS#1 v2.2, not for serious use
// private key stores information about public key,
// but isn't inheritance, so that functions don't get mixed up
class RSAPrvKey final : public PKCPrivate {
    BI p_, q_, m_, d_;
    BI n_, e_;
    BI dp_, dq_, qinv_;
    RSAScheme scheme_;
    SecureHashFunc* hash_;
    MaskGenFunc* mgf_;

   public:
    explicit RSAPrvKey(SecureHashFunc* hash = nullptr,
                       MaskGenFunc* mgf = nullptr)
        : PKCPrivate(id_unknown, 0), hash_(hash), mgf_(mgf) {}
    void SetHashFunc(SecureHashFunc* hash) { hash_ = hash; }
    void SetMaskGenFunc(MaskGenFunc* mgf) { mgf_ = mgf; }
    void SetScheme(RSAScheme scheme);
    BytesT Decrypt(const ByteT* cipher, LenT len) override;
    BytesT Sign(const ByteT* msg, LenT len) override;
    void PrintInfo() const;
    BI DecryptPrimitive(const BI& cipher) const;
    BI DecryptPrimitiveCRT(const BI& cipher) const;
    BI SignaturePrimitive(const BI& msg) const;
    BytesT OAEPDecrypt(const ByteT* code, LenT code_len,
                       const ByteT* label = nullptr, LenT label_len = 0) const;
    BytesT PKCS1Decrypt(const ByteT* code, LenT code_len) const;
    BytesT PSSSign(const ByteT* msg, LenT msg_len, LenT slen = 20) const;
    friend class RSA;
    friend class RSAPubKey;
};
class RSAPubKey final : public PKCPublic {
    BI n_, e_;
    RSAScheme scheme_;
    SecureHashFunc* hash_;
    MaskGenFunc* mgf_;

   public:
    explicit RSAPubKey(SecureHashFunc* hash = nullptr,
                       MaskGenFunc* mgf = nullptr)
        : PKCPublic(id_unknown, 0), hash_(hash), mgf_(mgf) {}
    void SetHashFunc(SecureHashFunc* hash) { hash_ = hash; }
    void SetMaskGenFunc(MaskGenFunc* mgf) { mgf_ = mgf; }
    // construct public key from private key
    explicit RSAPubKey(const RSAPrvKey& prv)
        : PKCPublic(prv.oid_, prv.keylen_),
          n_(prv.n_),
          e_(prv.e_),
          scheme_(prv.scheme_),
          hash_(prv.hash_),
          mgf_(prv.mgf_) {}
    RSAPubKey(const ByteT* data, enum RSAPubKeyFmt fmt);
    BytesT Encrypt(const ByteT* msg, LenT len) override;
    int Verify(const ByteT* msg, LenT msg_len, const ByteT* sign,
               LenT sign_len) override;
    void SetScheme(RSAScheme scheme);
    void PrintInfo() const;
    void PrintKey(enum RSAPubKeyFmt fmt, const Bin2Text& bin2text) const;
    BytesT Serialize(enum RSAPubKeyFmt fmt) const;
    BI EncryptPrimitive(const BI& msg) const;
    BI VerificationPrimitive(const BI& sign) const;
    BytesT OAEPEncrypt(const ByteT* msg, LenT msg_len,
                       const ByteT* label = nullptr, LenT label_len = 0) const;
    BytesT PKCS1Encrypt(const ByteT* msg, LenT msg_len) const;
    int PSSVerify(const ByteT* msg, LenT msg_len, const ByteT* sign,
                  LenT sign_len, LenT slen = 20) const;
    friend class RSA;
};
class RSA {
   public:
    static void KeyGen(RSAPubKey* pub_key, RSAPrvKey* prv_key,
                       int bit_len = 1024, int verbose = 1);
    static inline bool KeyMatch(const RSAPubKey& pub_key,
                                const RSAPrvKey& prv_key);
};

inline BI RSAPrvKey::SignaturePrimitive(const BI& msg) const {
    return DecryptPrimitiveCRT(msg);
}
inline BytesT RSAPrvKey::Decrypt(const ByteT* cipher, LenT len) {
    if (scheme_ == kRSAEncryption)
        return PKCS1Decrypt(cipher, len);
    else if (scheme_ == kRSAES_OAEP)
        return OAEPDecrypt(cipher, len);
    else
        return PKCS1Decrypt(cipher, len);
}
inline BytesT RSAPrvKey::Sign(const ByteT* msg, LenT len) {
    return PSSSign(msg, len);
}

inline void RSAPubKey::PrintKey(enum RSAPubKeyFmt fmt,
                                const Bin2Text& bin2text) const {
    bin2text.Print(stdout, Serialize(fmt));
}
inline BI RSAPubKey::VerificationPrimitive(const BI& sign) const {
    return EncryptPrimitive(sign);
}
inline BytesT RSAPubKey::Encrypt(const ByteT* msg, LenT len) {
    if (scheme_ == kRSAEncryption)
        return PKCS1Encrypt(msg, len);
    else if (scheme_ == kRSAES_OAEP)
        return OAEPEncrypt(msg, len);
    else
        return PKCS1Encrypt(msg, len);
}
inline int RSAPubKey::Verify(const ByteT* msg, LenT msg_len, const ByteT* sign,
                             LenT sign_len) {
    return PSSVerify(msg, msg_len, sign, sign_len);
}

inline bool RSA::KeyMatch(const RSAPubKey& pub_key, const RSAPrvKey& prv_key) {
    return pub_key.n_ == prv_key.n_ && pub_key.e_ == prv_key.e_;
}
}  // namespace cryp
