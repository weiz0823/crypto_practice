#include "rsa.hpp"
namespace cryp {
void RSAPrvKey::PrintInfo() {
    auto bit_len = n_.BitLen();
    std::printf("---Start RSA-%llu private key---\n", bit_len);
    std::puts("p =");
    p_.Print();
    std::puts("");
    std::puts("q =");
    q_.Print();
    std::puts("");
    std::puts("n = pq =");
    n_.Print();
    std::puts("");
    std::puts("m = lambda(n) =");
    m_.Print();
    std::puts("");
    std::puts("Private exponent d =");
    d_.Print();
    std::puts("");
    std::puts("> additional information on private key");
    std::puts("dp = d mod p-1 =");
    dp_.Print();
    std::puts("");
    std::puts("dq = d mod q-1 =");
    dq_.Print();
    std::puts("");
    std::puts("qinv = q^-1 mod p =");
    qinv_.Print();
    std::puts("");
    std::printf("---End RSA-%llu private key---\n", bit_len);
}
BI RSAPrvKey::DecryptPrimitive(const BI& cipher) {
    if (cipher.Sign() || cipher >= n_) {
        std::fputs("Error: ciphertext representative out of range.", stderr);
        return BI(0);
    } else {
        return calc::PowMod(cipher, d_, n_);
    }
}
BI RSAPrvKey::DecryptPrimitiveCRT(const BI& cipher) {
    if (cipher.Sign() || cipher >= n_) {
        std::fputs("Error: ciphertext representative out of range.", stderr);
        return BI(0);
    } else {
        // m2 = msg % q
        BI m2 = calc::PowMod(cipher, dq_, q_);
        return m2 + q_ * ((calc::PowMod(cipher, dp_, p_) - m2) * qinv_ % p_);
    }
}
BI RSAPrvKey::SignaturePrimitive(const BI& msg) {
    return DecryptPrimitiveCRT(msg);
}

RSAPubKey::RSAPubKey(const RSAPrvKey& prv) : n_(prv.n_) {
    // e * d = 1 (mod m)
    calc::ExtGcdBin(prv.d_, prv.m_, &e_, nullptr);
}
void RSAPubKey::PrintInfo() {
    auto bit_len = n_.BitLen();
    std::printf("---Start RSA-%llu public key---\n", bit_len);
    std::puts("n = ");
    n_.Print();
    std::puts("");
    std::puts("Public exponent e =");
    e_.Print();
    std::puts("");
    std::printf("---End RSA-%llu public key---\n", bit_len);
}
BI RSAPubKey::EncryptPrimitive(const BI& msg) {
    if (msg.Sign() || msg >= n_) {
        std::fputs(" Error:message representative out of range.", stderr);
        return BI(0);
    } else {
        return calc::PowMod(msg, e_, n_);
    }
}
BI RSAPubKey::VerificationPrimitive(const BI& sign) {
    return EncryptPrimitive(sign);
}

void RSA::KeyGen(RSAPubKey* pub_key, RSAPrvKey* prv_key, int bit_len,
                 int verbose) {
    if (!pub_key) {
        std::fprintf(stderr, "Error: pub_key is nullptr.\n");
        return;
    } else if (!prv_key) {
        std::fprintf(stderr, "Error: prv_key is nullptr.\n");
        return;
    }
    if (verbose >= 2) std::printf("Start RSA-%d key generation...\n", bit_len);
    // half length, and 128 bits per limb
    int bitq = bit_len >> 8, bitr = (bit_len >> 1) & 127;
    if (bitr)
        ++bitq;
    else
        bitr = 128;
    // get prime p
    do
        prv_key->p_.GenRandom(bitq, bitr);
    while ((bit_len >> 1) - prv_key->p_.log2() > 0.5);
    prv_key->p_.ToNextPrime();
    if (verbose >= 2)
        std::printf("Got %d-bit (%.2lf) prime p\n", bit_len >> 1,
                    prv_key->p_.log2());
    // get prime q
    do
        prv_key->q_.GenRandom(bitq, bitr);
    while ((bit_len >> 1) - prv_key->q_.log2() > 0.5);
    prv_key->q_.ToNextPrime();
    if (verbose >= 2)
        std::printf("Got %d-bit (%.2lf) prime q\n", bit_len >> 1,
                    prv_key->q_.log2());
    // don't forget to copy n_ also to public key
    pub_key->n_ = prv_key->n_ = prv_key->p_ * prv_key->q_;
    prv_key->m_ = calc::GcdBin(--prv_key->p_, --prv_key->q_);
    prv_key->m_ = prv_key->p_ * prv_key->q_ / prv_key->m_;
    // fixed small prime for public key is ok
    pub_key->e_ = BI(65537);
    calc::ExtGcdBin(pub_key->e_, prv_key->m_, &prv_key->d_, nullptr);
    // extended gcd ensures magnitude small, but don't ensure sign
    if (prv_key->d_.Sign()) prv_key->d_ += prv_key->m_;
    prv_key->dp_ = prv_key->d_ % prv_key->p_;
    prv_key->dq_ = prv_key->d_ % prv_key->q_;
    calc::ExtGcdBin(++prv_key->q_, ++prv_key->p_, &prv_key->qinv_, nullptr);
    if (prv_key->qinv_.Sign()) prv_key->qinv_ += prv_key->p_;
    // output information
    if (verbose > 0) {
        pub_key->PrintInfo();
        prv_key->PrintInfo();
    }
    if (verbose >= 3) {
        std::printf("Verify:\n    message: ");
        BI msg;
        msg.GenRandom(1).Print();
        std::printf("\n    cipher: ");
        BI cipher = pub_key->EncryptPrimitive(msg);
        cipher.Print();
        std::printf("\n    recover message: ");
        BI rec = prv_key->DecryptPrimitive(cipher);
        rec.Print();
        std::printf("\n        message match? ");
        std::cout << std::boolalpha << (rec == msg) << std::endl;
        std::printf("    recover message (using CRT): ");
        rec = prv_key->DecryptPrimitiveCRT(cipher);
        rec.Print();
        std::printf("\n        message match? ");
        std::cout << std::boolalpha << (rec == msg) << std::endl;
    }
}
bool RSA::KeyMatch(const RSAPubKey& pub_key, const RSAPrvKey& prv_key) {
    return pub_key.n_ == prv_key.n_ &&
           pub_key.e_ * prv_key.d_ % prv_key.m_ == BI(1);
}
}  // namespace cryp
