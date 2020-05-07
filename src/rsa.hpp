#ifndef RSA_HPP
#define RSA_HPP
#include "bigint64.hpp"
#include "bin2text.hpp"
#include "oid.hpp"
#include "serialize.hpp"

namespace cryp {
using BI = calc::BigInt<calc::uint128_t>;

enum RSAPubKeyFmt { kPKCS, kSSH, kRFC3279, kPEM };

inline const OID id_pkcs_1("pkcs-1", "1.2.840.113549.1.1",
                           "/ISO/Member-Body/US/RSADSI/PKCS/PKCS-1");
inline const OID id_rsa_encryption("rsaEncryption", id_pkcs_1, "1",
                                   "RSAEncryption");
inline const OID id_rsaes_oaep("id-RSAES-OAEP", id_pkcs_1, "7", "RSAES-OAEP");
inline const OID id_rsassa_pss("id-RSASSA-PSS", id_pkcs_1, "10", "RSASSA-PSS");
// Standards: RFC 8017: PKCS #1
// only partially follow PKCS#1 v2.2, not for serious use
// private key stores information about public key,
// but isn't inheritance, so that functions don't get mixed up
class RSAPrvKey {
    BI p_, q_, m_, d_;
    BI n_, e_;
    BI dp_, dq_, qinv_;

   public:
    void PrintInfo();
    BI DecryptPrimitive(const BI& cipher);
    BI DecryptPrimitiveCRT(const BI& cipher);
    inline BI SignaturePrimitive(const BI& msg);
    friend class RSA;
    friend class RSAPubKey;
};
class RSAPubKey {
    BI n_, e_;

   public:
    RSAPubKey() = default;
    // construct public key from private key
    explicit RSAPubKey(const RSAPrvKey& prv) : n_(prv.n_), e_(prv.e_) {}
    RSAPubKey(const uint8_t* data, enum RSAPubKeyFmt fmt);
    void PrintInfo();
    inline void PrintKey(enum RSAPubKeyFmt fmt, const Bin2Text& bin2text);
    std::vector<uint8_t> Serialize(enum RSAPubKeyFmt fmt);
    BI EncryptPrimitive(const BI& msg);
    inline BI VerificationPrimitive(const BI& sign);
    friend class RSA;
};
class RSA {
   public:
    static void KeyGen(RSAPubKey* pub_key, RSAPrvKey* prv_key,
                       int bit_len = 1024, int verbose = 1);
    static inline bool KeyMatch(const RSAPubKey& pub_key,
                                const RSAPrvKey& prv_key);
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
