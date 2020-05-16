#include "dsa.hpp"
namespace cryp {
void DSADomain::RandomGenPQ(LenT l, LenT n, int verbose) {
    if (!((l == 1024 && n == 160) || (l == 2048 && n == 224) ||
          (l == 2048 && n == 256) || (l == 3072 && n == 256))) {
        std::fputs("Security warning(DSADomain): (L, N) pair not standard\n",
                   stderr);
    }
    if (verbose >= 1)
        std::printf("Start DSA-(%llu, %llu) domain parameters generation...\n",
                    l, n);
    BI tmp;
    while (true) {
        q_.RandomBits(n);
        q_.ToNextPrime();
        BI q2 = q_ << 1;
        if (verbose >= 1)
            std::printf("Got %llu-bit (%.2lf) prime q\n", n, q_.log2());
        for (LenT i = 0; i < (l << 2); ++i) {
            p_.RandomBits(l);
            tmp = p_ % q2;
            p_ -= tmp;
            ++p_;  // p = 1 (mod 2q)
            if (p_.BitLen() < l) continue;
            if (p_.isProbablePrime()) {
                if (verbose >= 1)
                    std::printf("Got %llu-bit (%.2lf) prime p\n", l, p_.log2());
                return;
            }
        }
        if (verbose >= 2) std::printf("prime p failed, reset q\n");
    }
}
void DSADomain::RandomGenG(int verbose) {
    BI e = (p_ - 1) / q_;
    BI h(1);
    const BI one(1);
    do
        g_ = calc::PowMod(++h, e, p_);
    while (g_ == one);
    if (verbose > 0) std::puts("Got generator g");
    if (verbose > 1) PrintInfo();
}
void DSADomain::PrintInfo() const {
    LenT plen = p_.BitLen();
    LenT qlen = q_.BitLen();
    std::printf("---Start DSA-(%llu, %llu) domain parameters---\n", plen, qlen);
    std::puts("Prime p =");
    p_.Print();
    std::puts("");
    std::puts("Prime factor q =");
    q_.Print();
    std::puts("");
    std::puts("Generator g =");
    g_.Print();
    std::puts("");
    std::printf("---End DSA-(%llu, %llu) domain parameters---\n", plen, qlen);
}

void DSAPrvKey::PrintInfo() const {
    std::printf("---Start DSA-(%llu, %llu) private key---\n", keylen_, qlen_);
    std::puts("x =");
    x_.Print();
    std::puts("");
    std::printf("---End DSA-(%llu, %llu) private key---\n", keylen_, qlen_);
}
void DSAPrvKey::MessageSecretNumRandom(BI* k, BI* kinv) const {
    int term = qlen_ + 64;
    auto rnd_bits = new ByteT[term];
    for (int i = 0; i < term; ++i) rnd_bits[i] = rng_->Generate(0, 255);
    *k = OS2IP(rnd_bits, rnd_bits + term) % (domain_->q_ - 1);
    ++*k;  // k in [1, q-1]
    if (kinv) {
        calc::ExtGcdBin(*k, domain_->q_, kinv, nullptr);
        while (kinv->Sign()) *kinv += domain_->q_;
    }
    delete[] rnd_bits;
}
void DSAPrvKey::Sign(BI* r, BI* s, const ByteT* msg, LenT len) const {
    LenT klen = (keylen_ + 7) >> 3;
    LenT hlen = (hash_->HashLen() + 7) >> 3;
    BI k, kinv;
    auto z = new ByteT[hlen];
    hash_->Hash(msg, len, z);
    do {
        MessageSecretNumRandom(&k, &kinv);
        *r = calc::PowMod(domain_->g_, k, domain_->p_) % domain_->q_;
        *s = (OS2IP(z, z + std::min(klen, hlen)) + x_ * (*r)) * kinv %
             domain_->q_;
    } while (!*r || !*s);  // rarely loop
    delete[] z;
}

void DSAPubKey::PrintInfo() const {
    std::printf("---Start DSA-(%llu, %llu) public key---\n", keylen_, qlen_);
    std::puts("y =");
    y_.Print();
    std::puts("");
    std::printf("---End DSA-(%llu, %llu) public key---\n", keylen_, qlen_);
}
int DSAPubKey::Verify(const ByteT* msg, LenT msg_len, const BI& r,
                      const BI& s) const {
    int rv = 0;
    if (r.Sign() || r >= domain_->q_ || s.Sign() || s >= domain_->q_) {
        std::fputs("Error(DSAVerify): (r, s) not in proper range\n", stderr);
        return -1;
    }
    LenT klen = (keylen_ + 7) >> 3;
    LenT hlen = (hash_->HashLen() + 7) >> 3;
    auto z = new ByteT[hlen];
    hash_->Hash(msg, msg_len, z);
    BI sinv;
    calc::ExtGcdBin(s, domain_->q_, &sinv, nullptr);
    while (sinv.Sign()) sinv += domain_->q_;
    BI pg = calc::PowMod(
        domain_->g_, OS2IP(z, z + std::min(klen, hlen)) * sinv % domain_->q_,
        domain_->p_);
    BI py = calc::PowMod(y_, r * sinv % domain_->q_, domain_->p_);
    if (pg * py % domain_->p_ % domain_->q_ != r) {
        std::fputs("Security warning(DSAVerify): invalid signature\n", stderr);
        rv |= 1;
    }
    delete[] z;
    return rv;
}

void DSA::KeygenRandom(DSAPubKey* pub_key, DSAPrvKey* prv_key,
                       const DSADomain& domain, LenT l, LenT n, int verbose) {
    if (!prv_key) {
        std::fprintf(stderr, "Error(DSA Keygen): prv_key is nullptr.\n");
        return;
    }
    prv_key->keylen_ = l;
    prv_key->qlen_ = n;
    prv_key->domain_ = &domain;
    if (verbose >= 2)
        std::printf("Start DSA-(%llu, %llu) key generation...\n", l, n);
    auto rnd_bits = new ByteT[n + 64];
    int term = n + 64;
    for (int i = 0; i < term; ++i)
        rnd_bits[i] = prv_key->rng_->Generate(0, 255);
    prv_key->x_ = OS2IP(rnd_bits, rnd_bits + term) % (domain.q_ - 1);
    ++prv_key->x_;  // x in [1, q-1]
    if (verbose >= 1) std::puts("Got private key x");
    if (verbose >= 2) prv_key->PrintInfo();
    if (pub_key) {
        pub_key->keylen_ = l;
        pub_key->qlen_ = n;
        pub_key->domain_ = &domain;
        pub_key->y_ = calc::PowMod(domain.g_, prv_key->x_, domain.p_);
        if (verbose >= 1) std::puts("Got public key y");
        if (verbose >= 2) pub_key->PrintInfo();
    }
    delete[] rnd_bits;
}
}  // namespace cryp
