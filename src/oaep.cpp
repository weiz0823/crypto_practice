#include "oaep.hpp"
namespace cryp {
int EMEOAEP::Encode(const ByteT* msg, LenT msg_len, const ByteT* label,
                    LenT label_len, ByteT* dst, LenT dst_len) {
    if (!hash_ || !mgf_) {
        std::fputs(
            "Error(EMEOAEP::Encode): object parameters not properly set.",
            stderr);
        return -1;
    }
    if (label == nullptr || label_len == 0) {
        label = nullptr;
        label_len = 0;
    }
    auto hlen = hash_->HashLen() >> 3;
    if (msg_len > dst_len - 2 * hlen - 2) {
        std::fputs("Error(EMEOAEP::Encode): message too long.", stderr);
        return -2;
    }
    LenT pos = dst_len - hlen - 1;
    ByteT* db = dst + 1 + hlen;
    std::fill(db, db + pos, 0);
    hash_->Hash(label, label_len, db);
    pos -= msg_len;
    db[pos - 1] = 1;
    std::copy(msg, msg + msg_len, db + pos);
    BI rnd;
    rnd.GenRandom(((hlen << 3) + 127) >> 7);
    BytesT seed = rnd.Serialize();
    if (seed.size() > hlen) {
        std::copy(seed.end() - hlen, seed.end(), dst + 1);
    } else if (seed.size() < hlen) {
        pos = 1 + hlen - seed.size();
        std::copy(seed.begin(), seed.end(), dst + pos);
        std::fill(dst + 1, dst + pos, 0);
    }
    dst[0] = 0;
    auto mask = new ByteT[dst_len];
    pos = dst_len - hlen - 1;
    mgf_->Generate(dst + 1, hlen, mask, pos);
    for (LenT i = 0; i < pos; ++i) db[i] ^= mask[i];
    mgf_->Generate(db, pos, mask, hlen);
    for (LenT i = 0; i < hlen; ++i) dst[1 + i] ^= mask[i];
    delete[] mask;
    return 0;
}
int EMEOAEP::Decode(const ByteT* encoded, LenT src_len, const ByteT* label,
                    LenT label_len, BytesT* dst) {
    if (!hash_ || !mgf_) {
        std::fputs(
            "Error(EMEOAEP::Decode): object parameters not properly set.",
            stderr);
        return -1;
    }
    if (label == nullptr || label_len == 0) {
        label = nullptr;
        label_len = 0;
    }
    auto hlen = hash_->HashLen() >> 3;
    int rv = 0;  // bit mask return value
    if (encoded[0]) {
        std::fputs(
            "Security warning(EMEOAEP::Decode): first byte of the encoded "
            "message is not zero.",
            stderr);
        rv |= 1;
    }
    auto src = new ByteT[src_len - 1];
    std::copy(encoded + 1, encoded + src_len, src);
    ByteT* db = src + hlen;
    auto mask = new ByteT[src_len];
    LenT db_len = src_len - hlen - 1;
    LenT i = 0;
    mgf_->Generate(db, db_len, mask, hlen);
    for (i = 0; i < hlen; ++i) src[i] ^= mask[i];
    mgf_->Generate(src, hlen, mask, db_len);
    for (i = 0; i < db_len; ++i) db[i] ^= mask[i];
    hash_->Hash(label, label_len, mask);
    for (i = 0; i < hlen; ++i) {
        if (db[i] != mask[i]) {
            std::fputs(
                "Security warning(EMEOAEP::Decode): label hash incorrect.",
                stderr);
            rv |= 2;
            break;
        }
    }
    i = hlen;
    while (i < db_len && !db[i]) ++i;
    if (i >= db_len || db[i] != 1) {
        std::fputs(
            "Security warning(EMEOAEP::Decode): end-of-padding 0x01 not found.",
            stderr);
        rv |= 4;
        i = db_len - 1;
    }
    ++i;
    if (i >= db_len) {
        dst->clear();
    } else {
        dst->resize(db_len - i);
        std::copy(db + i, db + db_len, dst);
    }
    delete[] src;
    delete[] mask;
    return rv;
}
}  // namespace cryp
