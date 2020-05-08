#include "mgf1.hpp"
namespace cryp {
int MGF1::Generate(const ByteT* src, LenT src_len, ByteT* dst, LenT dst_len) {
    if ((dst_len + hash_->HashLen() - 1) / hash_->HashLen() > (LenT(1) << 32)) {
        std::fprintf(stderr,
                     "Error(MGF1): mask too long. Shouldn't exceeed %llu.\n",
                     hash_->HashLen() * (LenT(1) << 32));
        return 1;
    }
    auto a = new uint8_t[src_len + 4];
    std::copy(src, src + src_len, a);
    std::fill(a + src_len, a + src_len + 4, 0);
    uint64_t p = hash_->HashLen(), i;
    for (; p < dst_len; p += hash_->HashLen()) {
        hash_->Hash(a, src_len + 4, dst + p - hash_->HashLen());
        i = src_len + 4;
        do {
            if (++a[--i]) break;
        } while (i > src_len);
    }
    auto tmp = new uint8_t[hash_->HashLen()];
    p -= hash_->HashLen();
    hash_->Hash(a, src_len + 4, tmp);
    std::copy(tmp, tmp + dst_len - p, dst + p);
    delete[] a;
    delete[] tmp;
    return 0;
}
}  // namespace cryp
