#include "mgf1.hpp"
namespace cryp {
int MGF1::Generate(const uint8_t* src, uint64_t src_len, uint8_t* dst,
                   uint64_t dst_len) {
    if ((dst_len + hash_->HashLen() - 1) / hash_->HashLen() >
        (uint64_t(1) << 32))
        return 1;
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
