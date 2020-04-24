#ifndef RSA_HPP
#define RSA_HPP
#include "bigint64.hpp"

namespace cryp {
using BI = calc::BigInt<calc::uint128_t>;
// only partially follow PKCS#1 v2.2, not for serious use
class RSAPrvKey {
    BI p_, q_, m_, d_;
    BI n_;
    BI dp_, dq_, qinv_;

   public:
    void PrintInfo();
    BI DecryptPrimitive(const BI& cipher);
    BI DecryptPrimitiveCRT(const BI& cipher);
    friend class RSA;
    friend class RSAPubKey;
};
class RSAPubKey {
    BI n_, e_;

   public:
    RSAPubKey() = default;
    // construct public key from private key
    RSAPubKey(const RSAPrvKey& prv);
    void PrintInfo();
    BI EncryptPrimitive(const BI& msg);
    friend class RSA;
};
class RSA {
   public:
    static void KeyGen(RSAPubKey* pub_key, RSAPrvKey* prv_key,
                       int bit_len = 1024, int verbose = 1);
    static bool KeyMatch(const RSAPubKey& pub_key, const RSAPrvKey& prv_key);
};
}  // namespace cryp

#endif /* RSA_HPP */
