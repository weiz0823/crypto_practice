#pragma once
#include "asn1.hpp"
#include "bytes_mpint.hpp"
#include "hash.hpp"
#include "pubkeycrypto.hpp"
#include "random_engine.hpp"
#include "serialize.hpp"
#include "sha1.hpp"

namespace cryp {
using OID = ASN1::OID;
using BI = calc::BigInt<calc::uint128_t>;
// Standard: NIST FIPS 186: Digital Signature Standard
class DSADomain {
   protected:
    BI p_;
    BI q_;
    BI g_;
    SecureHashFunc* hash_;

   public:
    explicit DSADomain(SecureHashFunc* hash = &g_sha1) : hash_(hash) {}
    void SetHashFunc(SecureHashFunc* hash) { hash_ = hash; }
    void RandomGenPQ(LenT l = 1024, LenT n = 160, int verbose = 1);
    void RandomGenG(int verbose = 0);
    void RandomGen(LenT l = 1024, LenT n = 160, int verbose = 1) {
        RandomGenPQ(l, n, verbose);
        RandomGenG(verbose);
    }
    void PrintInfo() const;
    friend class DSA;
    friend class DSAPrvKey;
    friend class DSAPubKey;
};
class DSAPrvKey final : public PKCPrivate {
    BI x_;
    LenT qlen_;
    SecureHashFunc* hash_;
    RandomNumberGen<ByteT>* rng_;
    const DSADomain* domain_ = nullptr;
    void MessageSecretNumRandom(BI* k, BI* kinv) const;

   public:
    explicit DSAPrvKey(SecureHashFunc* hash = &g_sha1,
                       RandomNumberGen<ByteT>* rng = &g_rng32)
        : PKCPrivate(id_unknown, 0), hash_(hash), rng_(rng) {}
    BytesT Decrypt([[maybe_unused]] const ByteT* cipher,
                   [[maybe_unused]] LenT len) override {
        std::fputs("Error(DSADecrypt): no decryption scheme supported\n",
                   stderr);
        return BytesT();
    }
    void PrintInfo() const;
    BytesT Sign(const ByteT* msg, LenT len) override { return BytesT(); }
    void Sign(BI* r, BI* s, const ByteT* msg, LenT len) const;
    friend class DSA;
    friend class DSAPubKey;
};
class DSAPubKey final : public PKCPublic {
    BI y_;
    LenT qlen_;
    SecureHashFunc* hash_;
    const DSADomain* domain_ = nullptr;

   public:
    explicit DSAPubKey(SecureHashFunc* hash = &g_sha1)
        : PKCPublic(id_unknown, 0), hash_(hash) {}
    explicit DSAPubKey(const DSAPrvKey& prv_key, const DSADomain& domain)
        : PKCPublic(id_unknown, prv_key.keylen_),
          y_(calc::PowMod(domain.g_, prv_key.x_, domain.p_)),
          qlen_(prv_key.qlen_),
          hash_(prv_key.hash_),
          domain_(prv_key.domain_) {}
    BytesT Encrypt([[maybe_unused]] const ByteT* msg,
                   [[maybe_unused]] LenT len) override {
        std::fputs("Error(DSAEncrypt): no encryption scheme supported\n",
                   stderr);
        return BytesT();
    }
    void PrintInfo() const;
    int Verify(const ByteT* msg, LenT msg_len, const ByteT* sign,
               LenT sign_len) override {
        return -1;
    }
    int Verify(const ByteT* msg, LenT msg_len, const BI& r, const BI& s) const;
    friend class DSA;
};
class DSA {
   public:
    static void KeygenRandom(DSAPubKey* pub_key, DSAPrvKey* prv_key,
                             const DSADomain& domain, LenT l = 1024,
                             LenT n = 160, int verbose = 1);
};
}  // namespace cryp
