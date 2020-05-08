#ifndef RSA_HPP
#define RSA_HPP
#include "bigint64.hpp"
#include "bin2text.hpp"
#include "hash.hpp"
#include "mgf.hpp"
#include "oid.hpp"
#include "pubkeycrypto.hpp"
#include "serialize.hpp"

namespace cryp {
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

inline const OID id_pkcs_1("pkcs-1", "1.2.840.113549.1.1",
                           "/ISO/Member-Body/US/RSADSI/PKCS/PKCS-1");
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

   public:
    RSAPrvKey() : PKCPrivate(id_unknown, 0) {}
    BytesT Decrypt(const ByteT* cipher, LenT len) override;
    BytesT Sign(const ByteT* msg, LenT len) override;
    void PrintInfo();
    BI DecryptPrimitive(const BI& cipher);
    BI DecryptPrimitiveCRT(const BI& cipher);
    inline BI SignaturePrimitive(const BI& msg);
    friend class RSA;
    friend class RSAPubKey;
};
class RSAPubKey final : public PKCPublic {
    BI n_, e_;
    RSAScheme scheme_;

   public:
    RSAPubKey() : PKCPublic(id_unknown, 0) {}
    // construct public key from private key
    explicit RSAPubKey(const RSAPrvKey& prv)
        : PKCPublic(prv.oid_, prv.keylen_),
          n_(prv.n_),
          e_(prv.e_),
          scheme_(prv.scheme_) {}
    RSAPubKey(const ByteT* data, enum RSAPubKeyFmt fmt);
    BytesT Encrypt(const ByteT* msg, LenT len) override;
    BytesT Verify(const ByteT* sign, LenT len) override;
    void PrintInfo();
    inline void PrintKey(enum RSAPubKeyFmt fmt, const Bin2Text& bin2text);
    BytesT Serialize(enum RSAPubKeyFmt fmt);
    BI EncryptPrimitive(const BI& msg);
    inline BI VerificationPrimitive(const BI& sign);
    BytesT OAEPEncrypt(const ByteT* msg, LenT msg_len, ByteT* label,
                       LenT label_len);
    friend class RSA;
};
class RSA {
   public:
    static void KeyGen(RSAPubKey* pub_key, RSAPrvKey* prv_key,
                       int bit_len = 1024, int verbose = 1);
    static inline bool KeyMatch(const RSAPubKey& pub_key,
                                const RSAPrvKey& prv_key);
    static void SetScheme(RSAScheme scheme, RSAPubKey* pub_key,
                          RSAPrvKey* prv_key);
};

inline BI RSAPrvKey::SignaturePrimitive(const BI& msg) {
    return DecryptPrimitiveCRT(msg);
}

inline void RSAPubKey::PrintKey(enum RSAPubKeyFmt fmt,
                                const Bin2Text& bin2text) {
    bin2text.Print(stdout, Serialize(fmt));
}
inline BI RSAPubKey::VerificationPrimitive(const BI& sign) {
    return EncryptPrimitive(sign);
}

inline bool RSA::KeyMatch(const RSAPubKey& pub_key, const RSAPrvKey& prv_key) {
    return pub_key.n_ == prv_key.n_ && pub_key.e_ == prv_key.e_;
}
}  // namespace cryp

#endif /* RSA_HPP */
