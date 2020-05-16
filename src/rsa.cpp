#include "rsa.hpp"
namespace cryp {
void RSAPrvKey::PrintInfo() const {
    LenT bit_len = n_.BitLen();
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
BI RSAPrvKey::DecryptPrimitive(const BI& cipher) const {
    if (cipher.Sign() || cipher >= n_) {
        std::fputs("Warning(RSADP): ciphertext representative out of range.\n",
                   stderr);
    }
    return calc::PowMod(cipher, d_, n_);
}
BI RSAPrvKey::DecryptPrimitiveCRT(const BI& cipher) const {
    if (cipher.Sign() || cipher >= n_) {
        std::fputs("Error(RSADP): ciphertext representative out of range.\n",
                   stderr);
    }
    // m2 = msg % q
    BI m2 = calc::PowMod(cipher, dq_, q_);
    BI tmp = (calc::PowMod(cipher, dp_, p_) - m2) * qinv_ % p_;
    if (tmp.Sign()) tmp += p_;
    return std::move(tmp) * q_ + std::move(m2);
}
BytesT RSAPrvKey::OAEPDecrypt(const ByteT* code, LenT code_len,
                              const ByteT* label, LenT label_len) const {
    if (label_len == 0 || label == nullptr) {
        label = nullptr;
        label_len = 0;
    }
    LenT k = (keylen_ + 7) >> 3;
    if (code_len != k) {
        std::fputs(
            "Security warning(RSAES-OAEP): ciphertext length incorrect.\n",
            stderr);
    }
    BI m = OS2IP(code, code + code_len);
    m = DecryptPrimitiveCRT(m);
    BytesT em = I2OSP(m, k);
    EMEOAEP eme_oaep(hash_, mgf_);
    BytesT msg;
    if (eme_oaep.Decode(em.data(), em.size(), label, label_len, &msg)) {
        std::fputs("Warning(RSAES-OAEP): decrypt error.\n", stderr);
    }
    return msg;
}
BytesT RSAPrvKey::PKCS1Decrypt(const ByteT* code, LenT code_len) const {
    LenT k = (keylen_ + 7) >> 3;
    if (code_len != k) {
        std::fputs(
            "Security warning(RSAES-PKCS1-v1_5): ciphertext length "
            "incorrect.\n",
            stderr);
    }
    BI m = OS2IP(code, code + code_len);
    m = DecryptPrimitiveCRT(m);
    BytesT em = I2OSP(m, k);
    EMEPKCS1 eme_pkcs1;
    BytesT msg;
    if (eme_pkcs1.Decode(em.data(), em.size(), &msg)) {
        std::fputs("Warning(RSAES-PKCS1-v1_5): decrypt error.\n", stderr);
    }
    return msg;
}
BytesT RSAPrvKey::PSSSign(const ByteT* msg, LenT msg_len, LenT slen) const {
    LenT em_len = (keylen_ + 6) >> 3;
    auto em = new ByteT[em_len];
    EMSAPSS emsa_pss(hash_, mgf_, slen);
    if (emsa_pss.Encode(msg, msg_len, em, em_len)) {
        std::fputs("Error(RSASSA-PSS): encoding error.\n", stderr);
        delete[] em;
        return BytesT();
    }
    LenT mov = keylen_ & 7;
    // mov=(1-keylen)%8
    mov = (9 - mov) & 7;
    em[0] <<= mov;
    em[0] >>= mov;
    BI m = OS2IP(em, em + em_len);
    m = SignaturePrimitive(m);
    LenT k = (keylen_ + 7) >> 3;
    delete[] em;
    return I2OSP(m, k);
}

RSAPubKey::RSAPubKey(const ByteT* data, enum RSAPubKeyFmt fmt)
    : PKCPublic(id_unknown, 0) {
    // TODO(): complete other key formats
    switch (fmt) {
        case kSSH:
            const ByteT *x, *y;
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
void RSAPubKey::PrintInfo() const {
    LenT bit_len = n_.BitLen();
    std::printf("---Start RSA-%llu public key---\n", bit_len);
    std::puts("n = ");
    n_.Print();
    std::puts("");
    std::puts("Public exponent e =");
    e_.Print();
    std::puts("");
    std::printf("---End RSA-%llu public key---\n", bit_len);
}
BytesT RSAPubKey::Serialize(enum RSAPubKeyFmt fmt) const {
    BytesT v;
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
BI RSAPubKey::EncryptPrimitive(const BI& msg) const {
    if (msg.Sign() || msg >= n_) {
        std::fputs("Warning(RSAEP): message representative out of range.\n",
                   stderr);
    }
    return calc::PowMod(msg, e_, n_);
}
BytesT RSAPubKey::OAEPEncrypt(const ByteT* msg, LenT msg_len,
                              const ByteT* label, LenT label_len) const {
    if (label_len == 0 || label == nullptr) {
        label = nullptr;
        label_len = 0;
    }
    EMEOAEP eme_oaep(hash_, mgf_);
    LenT k = (keylen_ + 7) >> 3;
    auto em = new ByteT[k];
    if (eme_oaep.Encode(msg, msg_len, label, label_len, em, k)) {
        std::fputs("Error(RSAES-OAEP): encrypt error.\n", stderr);
        delete[] em;
        return BytesT();
    }
    BI m = OS2IP(em, em + k);
    m = EncryptPrimitive(m);
    delete[] em;
    return I2OSP(m, k);
}
BytesT RSAPubKey::PKCS1Encrypt(const ByteT* msg, LenT msg_len) const {
    EMEPKCS1 eme_pkcs1;
    LenT k = (keylen_ + 7) >> 3;
    auto em = new ByteT[k];
    if (eme_pkcs1.Encode(msg, msg_len, em, k)) {
        std::fputs("Error(RSAES-PKCS1-v1_5): encrypt error.\n", stderr);
        delete[] em;
        return BytesT();
    }
    BI m = OS2IP(em, em + k);
    m = EncryptPrimitive(m);
    delete[] em;
    return I2OSP(m, k);
}
int RSAPubKey::PSSVerify(const ByteT* msg, LenT msg_len, const ByteT* sign,
                         LenT sign_len, LenT slen) const {
    int rv = 0;
    LenT k = (keylen_ + 7) >> 3;
    if (sign_len != k) {
        std::fputs(
            "Security warning(RSASSA-PSS): signature length incorrect.\n",
            stderr);
        rv |= 1;
    }
    BI m = OS2IP(sign, sign + sign_len);
    m = VerificationPrimitive(m);
    LenT em_len = (keylen_ + 6) >> 3;
    BytesT em = I2OSP(m, em_len);
    LenT mov = (keylen_ - 1) & 7;
    if (mov && (em[0] >> mov)) {
        std::fputs("Security warning(RSASSA-PSS): leading bits not zero.\n",
                   stderr);
        rv |= 2;
    }
    EMSAPSS emsa_pss(hash_, mgf_, slen);
    int tmp = emsa_pss.Verify(msg, msg_len, em.data(), em_len);
    if (tmp >= 0)
        rv |= tmp << 2;
    else
        rv = -rv + tmp * 4;
    return rv;
}

// must assign private key, may assign public key
void RSA::KeyGen(RSAPubKey* pub_key, RSAPrvKey* prv_key, int bit_len,
                 int verbose) {
    if (!prv_key) {
        std::fprintf(stderr, "Error(RSA Keygen): prv_key is nullptr.\n");
        return;
    }
    if (bit_len < 1024) {
        std::fputs(
            "Security warning(RSA Keygen): keylen shall be at least "
            "1024bits.\n",
            stderr);
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
    calc::ExtGcdBin(prv_key->e_, prv_key->m_, &prv_key->d_, nullptr);
    // extended gcd ensures magnitude small, but don't ensure sign
    if (prv_key->d_.Sign()) prv_key->d_ += prv_key->m_;
    prv_key->dp_ = prv_key->d_ % prv_key->p_;
    prv_key->dq_ = prv_key->d_ % prv_key->q_;
    calc::ExtGcdBin(++prv_key->q_, ++prv_key->p_, &prv_key->qinv_, nullptr);
    if (prv_key->qinv_.Sign()) prv_key->qinv_ += prv_key->p_;
    prv_key->keylen_ = bit_len;
    if (verbose > 0) prv_key->PrintInfo();
    // output information
    if (pub_key) {
        pub_key->e_ = prv_key->e_;
        pub_key->n_ = prv_key->n_;
        pub_key->keylen_ = bit_len;
        if (verbose > 0) pub_key->PrintInfo();
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
}
void RSAPubKey::SetScheme(RSAScheme scheme) {
    scheme_ = scheme;
    switch (scheme) {
        case kRSAEncryption:
            oid_ = id_rsa_encryption;
            break;
        case kRSA_MD5:
            oid_ = id_rsa_md5;
            break;
        case kRSA_SHA1:
            oid_ = id_rsa_sha1;
            break;
        case kRSA_SHA224:
            oid_ = id_rsa_sha224;
            break;
        case kRSA_SHA256:
            oid_ = id_rsa_sha256;
            break;
        case kRSA_SHA384:
            oid_ = id_rsa_sha384;
            break;
        case kRSA_SHA512:
            oid_ = id_rsa_sha512;
            break;
        case kRSA_SHA512_224:
            oid_ = id_rsa_sha512_224;
            break;
        case kRSA_SHA512_256:
            oid_ = id_rsa_sha512_256;
            break;
        case kRSAES_OAEP:
            oid_ = id_rsaes_oaep;
            break;
        case kRSASSA_PSS:
            oid_ = id_rsassa_pss;
            break;
        default:
            oid_ = id_unknown;
            break;
    }
}
}  // namespace cryp
