#include "mgf1.hpp"
namespace cryp {
int MGF1::Generate(const ByteT* src, LenT src_len, ByteT* dst, LenT dst_len) {
    LenT hlen = (hash_->HashLen() + 7) >> 3;
    if ((dst_len + hlen - 1) / hlen > (LenT(1) << 32)) {
        std::fprintf(stderr,
                     "Error(MGF1): mask too long. Shouldn't exceeed %llu.\n",
                     hlen * (LenT(1) << 32));
        return 1;
    }
    auto a = new uint8_t[src_len + 4];
    std::copy(src, src + src_len, a);
    std::fill(a + src_len, a + src_len + 4, 0);
    uint64_t p = hlen, i;
    for (; p < dst_len; p += hlen) {
        hash_->Hash(a, src_len + 4, dst + p - hlen);
        i = src_len + 4;
        do {
            if (++a[--i]) break;
        } while (i > src_len);
    }
    auto tmp = new uint8_t[hlen];
    p -= hlen;
    hash_->Hash(a, src_len + 4, tmp);
    std::copy(tmp, tmp + dst_len - p, dst + p);
    delete[] a;
    delete[] tmp;
    return 0;
}
}  // namespace cryp
