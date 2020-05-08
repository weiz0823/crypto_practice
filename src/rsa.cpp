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
Bytes RSAPrvKey::Decrypt(const uint8_t* cipher, uint64_t len) {
    // reserved
    return Bytes();
}
Bytes RSAPrvKey::Sign(const uint8_t* msg, uint64_t len) {
    // reserved
    return Bytes();
}

RSAPubKey::RSAPubKey(const uint8_t* data, enum RSAPubKeyFmt fmt)
    : PKCPublic(id_unknown, 0) {
    // TODO(): complete other key formats
    switch (fmt) {
        case kSSH:
            const uint8_t *x, *y;
            // "ssh-rsa"
            DeserializeString(&x, &y, data);
            DeserializeString(&x, &y, y);
            e_ = BI(x, y);
            DeserializeString(&x, &y, y);
            n_ = BI(x, y);
            keylen_ = n_.BitLen();
            break;
        case kPKCS:
            break;
        case kPEM:
            break;
        case kRFC3279:
            break;
    }
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
Bytes RSAPubKey::Serialize(enum RSAPubKeyFmt fmt) {
    Bytes v;
    switch (fmt) {
        case kSSH:
            SerializeString(v, "ssh-rsa", 7);
            SerializeString(v, e_.Serialize());
            SerializeString(v, n_.Serialize());
            break;
        case kPKCS:
            break;
        case kPEM:
            break;
        case kRFC3279:
            break;
    }
    return v;
}
BI RSAPubKey::EncryptPrimitive(const BI& msg) {
    if (msg.Sign() || msg >= n_) {
        std::fputs(" Error:message representative out of range.", stderr);
        return BI(0);
    } else {
        return calc::PowMod(msg, e_, n_);
    }
}
Bytes RSAPubKey::Encrypt(const uint8_t* msg, uint64_t len) {
    // reserved
    return Bytes();
}
Bytes RSAPubKey::Verify(const uint8_t* sign, uint64_t len) {
    // reserved
    return Bytes();
}

void RSA::KeyGen(RSAPubKey* pub_key, RSAPrvKey* prv_key, int bit_len,
                 int verbose) {
    if (!prv_key) {
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
    prv_key->n_ = prv_key->p_ * prv_key->q_;
    prv_key->m_ = calc::GcdBin(--prv_key->p_, --prv_key->q_);
    prv_key->m_ = prv_key->p_ * prv_key->q_ / prv_key->m_;
    // fixed small prime for public key is ok
    prv_key->e_ = BI(65537);
    calc::ExtGcdBin(pub_key->e_, prv_key->m_, &prv_key->d_, nullptr);
    // extended gcd ensures magnitude small, but don't ensure sign
    if (prv_key->d_.Sign()) prv_key->d_ += prv_key->m_;
    prv_key->dp_ = prv_key->d_ % prv_key->p_;
    prv_key->dq_ = prv_key->d_ % prv_key->q_;
    calc::ExtGcdBin(++prv_key->q_, ++prv_key->p_, &prv_key->qinv_, nullptr);
    if (prv_key->qinv_.Sign()) prv_key->qinv_ += prv_key->p_;
    prv_key->keylen_ = bit_len;
    if (pub_key) {
        pub_key->e_ = prv_key->e_;
        pub_key->n_ = prv_key->n_;
        pub_key->keylen_ = bit_len;
    }
    // output information
    if (verbose > 0) {
        pub_key->PrintInfo();
        prv_key->PrintInfo();
    }
    if (verbose >= 3) {
        std::printf("Verify:\n    message: ");
        BI msg;
        msg.GenRandom(1).Print();
        std::printf("\n    ciphertext: ");
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
void RSA::SetScheme(RSAScheme scheme, RSAPubKey* pub_key, RSAPrvKey* prv_key) {
    if (pub_key) pub_key->scheme_ = scheme;
    if (prv_key) prv_key->scheme_ = scheme;
    switch (scheme) {
        case kRSAEncryption:
            if (pub_key) pub_key->oid_ = id_rsa_encryption;
            if (prv_key) prv_key->oid_ = id_rsa_encryption;
            break;
        case kRSA_MD5:
            if (pub_key) pub_key->oid_ = id_rsa_md5;
            if (prv_key) prv_key->oid_ = id_rsa_md5;
            break;
        case kRSA_SHA1:
            if (pub_key) pub_key->oid_ = id_rsa_sha1;
            if (prv_key) prv_key->oid_ = id_rsa_sha1;
            break;
        case kRSA_SHA224:
            if (pub_key) pub_key->oid_ = id_rsa_sha224;
            if (prv_key) prv_key->oid_ = id_rsa_sha224;
            break;
        case kRSA_SHA256:
            if (pub_key) pub_key->oid_ = id_rsa_sha256;
            if (prv_key) prv_key->oid_ = id_rsa_sha256;
            break;
        case kRSA_SHA384:
            if (pub_key) pub_key->oid_ = id_rsa_sha384;
            if (prv_key) prv_key->oid_ = id_rsa_sha384;
            break;
        case kRSA_SHA512:
            if (pub_key) pub_key->oid_ = id_rsa_sha512;
            if (prv_key) prv_key->oid_ = id_rsa_sha512;
            break;
        case kRSA_SHA512_224:
            if (pub_key) pub_key->oid_ = id_rsa_sha512_224;
            if (prv_key) prv_key->oid_ = id_rsa_sha512_224;
            break;
        case kRSA_SHA512_256:
            if (pub_key) pub_key->oid_ = id_rsa_sha512_256;
            if (prv_key) prv_key->oid_ = id_rsa_sha512_256;
            break;
        case kRSAES_OAEP:
            if (pub_key) pub_key->oid_ = id_rsaes_oaep;
            if (prv_key) prv_key->oid_ = id_rsaes_oaep;
            break;
        case kRSASSA_PSS:
            if (pub_key) pub_key->oid_ = id_rsassa_pss;
            if (prv_key) prv_key->oid_ = id_rsassa_pss;
            break;
    }
}
}  // namespace cryp
