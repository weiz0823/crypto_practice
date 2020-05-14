#include "emsapss.hpp"
namespace cryp {
int EMSAPSS::Encode(const ByteT* msg, LenT msg_len, ByteT* dst, LenT dst_len) {
    if (!hash_ || !mgf_ || !rnd_) {
        std::fputs(
            "Error(EMSAPSS::Encode): object parameters not properly set\n",
            stderr);
        return 1;
    }
    auto hlen = (hash_->HashLen() + 7) >> 3;
    if (dst_len < hlen + salt_len_ + 2) {
        std::fputs(
            "Error(EMSAPSS::Encode): intended encoded message too short\n",
            stderr);
        return 2;
    }
    auto hm = new ByteT[hlen + salt_len_ + 8];
    std::fill(hm, hm + 8, 0);
    hash_->Hash(msg, msg_len, hm + 8);
    auto it = hm + hlen + 8;
    for (LenT i = 0; i < salt_len_; ++i) it[i] = rnd_->Generate(0, 255);
    auto dblen = dst_len - hlen - 1;
    auto it2 = dst + dblen - salt_len_;
    // copy salt
    std::copy(it, it + salt_len_, it2);
    // padding and padding end
    *--it2 = 1;
    std::fill(dst, it2, 0);
    it2 = dst + dblen;
    hash_->Hash(hm, hlen + salt_len_ + 8, it2);
    auto mask = new ByteT[dblen];
    mgf_->Generate(it2, hlen, mask, dblen);
    for (LenT i = 0; i < dblen; ++i) dst[i] ^= mask[i];
    dst[dst_len - 1] = trailer_field_;
    delete[] hm;
    delete[] mask;
    return 0;
}
int EMSAPSS::Verify(const ByteT* msg, LenT msg_len, const ByteT* encoded,
                    LenT src_len) {
    if (!hash_ || !mgf_ || !rnd_) {
        std::fputs(
            "Error(EMSAPSS::Decode): object parameters not properly set\n",
            stderr);
        return -1;
    }
    auto hlen = (hash_->HashLen() + 7) >> 3;
    if (src_len < hlen + salt_len_ + 2) {
        std::fputs("Error(EMSAPSS::Decode): encoded message too short\n",
                   stderr);
        return -2;
    }
    int rv = 0;
    if (encoded[src_len - 1] != trailer_field_) {
        std::fputs("Security warning(EMSAPSS::Decode): last byte incorrect\n",
                   stderr);
        rv |= 1;
    }
    auto hm = new ByteT[hlen + salt_len_ + 8];
    std::fill(hm, hm + 8, 0);
    hash_->Hash(msg, msg_len, hm + 8);
    auto dblen = src_len - hlen - 1;
    auto db = new ByteT[dblen];
    mgf_->Generate(encoded + dblen, hlen, db, dblen);
    for (LenT i = 0; i < dblen; ++i) db[i] ^= encoded[i];
    db[0] &= 1;  // db[0] either 0 or 1
    auto pos = dblen - salt_len_ - 1;
    for (LenT i = 0; i < pos; ++i) {
        if (db[i]) {
            std::fputs("Security warning(EMSAPSS::Decode): padding incorrect\n",
                       stderr);
            rv |= 2;
            break;
        }
    }
    if (db[pos] != 1) {
        std::fputs("Security warning(EMSAPSS::Decode): padding end incorrect\n",
                   stderr);
        rv |= 4;
    }
    ++pos;
    std::copy(db + pos, db + dblen, hm + hlen + 8);
    auto hhm = new ByteT[hlen];
    hash_->Hash(hm, hlen + salt_len_ + 8, hhm);
    for (LenT i = 0; i < hlen; ++i) {
        if (encoded[dblen + i] != hhm[i]) {
            std::fputs(
                "Security warning(EMSAPSS::Decode): hash result inconsistent\n",
                stderr);
            rv |= 8;
            break;
        }
    }
    delete[] hhm;
    delete[] hm;
    delete[] db;
    return rv;
}
}  // namespace cryp
