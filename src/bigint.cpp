#include "bigint.hpp"
namespace cryp {

template <int LEN>
BigInt<LEN>::BigInt(int val) {
    val_ = new unsigned[LEN];
    std::memset(val_, 0, sizeof(unsigned) * static_cast<unsigned>(LEN));
    // google-readability-casting told me to do that
    val_[0] = static_cast<unsigned>(val);
}

template <int LEN>
BigInt<LEN>::BigInt(int64_t val) {
    val_ = new unsigned[LEN];
    std::memset(val_, 0, sizeof(unsigned) * static_cast<unsigned>(LEN));
    // google-readability-casting told me to do that
    val_[0] = static_cast<unsigned>(val);
    val_[1] = static_cast<unsigned>(val >> 32);
}

template <int LEN>
BigInt<LEN>::BigInt(const BigInt<LEN>& rhs) {
    val_ = new unsigned[LEN];
    std::memcpy(val_, rhs.val_, LEN * sizeof(unsigned));
}

template <int LEN>
BigInt<LEN>::BigInt(BigInt<LEN>&& rhs) noexcept {
    val_ = rhs.val_;
    rhs.val_ = nullptr;
}

template <int LEN>
BigInt<LEN>::~BigInt() {
    delete[] val_;
}

template <int LEN>
BigInt<LEN>& BigInt<LEN>::operator=(const BigInt<LEN>& rhs) {
    // self assignment check
    if (this == &rhs) return *this;
    BigInt<LEN> obj = rhs;  // copy construct
    ::cryp::swap(obj, *this);
    return *this;
    // obj will be automatically destroyed
}

template <int LEN>
BigInt<LEN>& BigInt<LEN>::operator=(BigInt<LEN>&& rhs) noexcept {
    ::cryp::swap(*this, rhs);
    return *this;
    // rhs will be destroyed elsewhere
}

template <int LEN>
BigInt<LEN>::operator bool() const {
    for (int i = 0; i < LEN; ++i)
        if (val_[i]) return true;
    return false;
}

template <int LEN>
void swap(BigInt<LEN>& lhs, BigInt<LEN>& rhs) {
    if (&lhs == &rhs) return;
    std::swap(lhs.val_, rhs.val_);
}

template <int LEN>
bool BigInt<LEN>::Sign() const {
    return val_[LEN - 1] >> 31;
}

template <int LEN>
BigInt<LEN>& BigInt<LEN>::ToBitRev() {
    // self-modify
    for (int i = 0; i < LEN; ++i) val_[i] = ~val_[i];
    return *this;
}

template <int LEN>
BigInt<LEN>& BigInt<LEN>::operator+=(const BigInt<LEN>& rhs) {
    unsigned overflow_flag = 0;
    for (int i = 0; i < LEN; ++i) {
        val_[i] += overflow_flag;
        val_[i] += rhs.val_[i];
        if (val_[i] < rhs.val_[i] || (overflow_flag && val_[i] <= rhs.val_[i]))
            overflow_flag = 1;
        else
            overflow_flag = 0;
    }
    return *this;
}

template <int LEN>
BigInt<LEN> operator+(BigInt<LEN> lhs, const BigInt<LEN>& rhs) {
    return lhs += rhs;
}

template <int LEN>
BigInt<LEN>& BigInt<LEN>::operator-=(const BigInt<LEN>& rhs) {
    // alternative: a-b=~(~a+b)
    // ToBitRev();
    // *this += rhs;
    // ToBitRev();
    unsigned overflow_flag = 0;
    for (int i = 0; i < LEN; ++i) {
        val_[i] -= overflow_flag;
        val_[i] -= rhs.val_[i];
        if (val_[i] > rhs.val_[i] || (overflow_flag && val_[i] >= rhs.val_[i]))
            overflow_flag = 1;
        else
            overflow_flag = 0;
    }
    return *this;
}

template <int LEN>
BigInt<LEN> operator-(BigInt<LEN> lhs, const BigInt<LEN>& rhs) {
    return lhs -= rhs;
}

template <int LEN>
BigInt<LEN>& BigInt<LEN>::operator++() {
    for (int i = 0; i < LEN; ++i) {
        ++val_[i];
        if (val_[i]) return *this;
    }
    return *this;
}

template <int LEN>
BigInt<LEN>& BigInt<LEN>::operator--() {
    // alternative: a-1=~(~a+1)
    for (int i = 0; i < LEN; ++i) {
        --val_[i];
        if (val_[i] != unsigned(-1)) return *this;
    }
    return *this;
}

template <int LEN>
BigInt<LEN> BigInt<LEN>::operator++(int) {
    BigInt obj = *this;  // copy
    ++*this;
    return obj;
}

template <int LEN>
BigInt<LEN> BigInt<LEN>::operator--(int) {
    BigInt obj = *this;  // copy
    --*this;
    return obj;
}

template <int LEN>
BigInt<LEN>& BigInt<LEN>::ToOpposite() {
    // self-modify
    return ++ToBitRev();
}

template <int LEN>
BigInt<LEN> BigInt<LEN>::operator~() const {
    BigInt obj = *this;  // copy
    obj.ToBitRev();
    return obj;
}

template <int LEN>
BigInt<LEN> BigInt<LEN>::operator-() const {
    BigInt obj = *this;  // copy
    obj.ToOpposite();
    return obj;
}

template <int LEN>
BigInt<LEN>& BigInt<LEN>::operator&=(const BigInt<LEN>& rhs) {
    for (int i = 0; i < LEN; ++i) val_[i] &= rhs.val_[i];
    return *this;
}

template <int LEN>
BigInt<LEN> operator&(BigInt<LEN> lhs, const BigInt<LEN>& rhs) {
    return lhs &= rhs;
}

template <int LEN>
BigInt<LEN>& BigInt<LEN>::operator|=(const BigInt<LEN>& rhs) {
    for (int i = 0; i < LEN; ++i) val_[i] |= rhs.val_[i];
    return *this;
}

template <int LEN>
BigInt<LEN> operator|(BigInt<LEN> lhs, const BigInt<LEN>& rhs) {
    return lhs |= rhs;
}

template <int LEN>
BigInt<LEN>& BigInt<LEN>::operator^=(const BigInt<LEN>& rhs) {
    for (int i = 0; i < LEN; ++i) val_[i] ^= rhs.val_[i];
    return *this;
}

template <int LEN>
BigInt<LEN> operator^(BigInt<LEN> lhs, const BigInt<LEN>& rhs) {
    return lhs ^= rhs;
}

template <int LEN>
BigInt<LEN>& BigInt<LEN>::operator<<=(int shift) {
    if (shift < 0) return *this >>= -shift;
    shift &= (LEN << 5) - 1;
    // shift = q*32 + r
    int q = shift >> 5, r = shift & 31;
    unsigned mask1 = (1 << (32 - r)) - 1;
    unsigned mask2 = unsigned(-1) - mask1;
    for (int i = LEN - 1; i > q; --i) {
        val_[i] = ((val_[i - q] & mask1) << r) +
                  ((val_[i - q - 1] & mask2) >> (32 - r));
    }
    val_[q] = (val_[0] & mask1) << r;
    if (q) std::memset(val_, 0, sizeof(unsigned) * static_cast<unsigned>(q));
    return *this;
}

template <int LEN>
BigInt<LEN> operator<<(BigInt<LEN> lhs, int shift) {
    return lhs <<= shift;
}

template <int LEN>
BigInt<LEN>& BigInt<LEN>::operator>>=(int shift) {
    if (shift < 0) return *this <<= -shift;
    shift &= (LEN << 5) - 1;
    // shift = q*32 + r
    int q = shift >> 5, r = shift & 31;
    unsigned mask2 = (1 << r) - 1;
    unsigned mask1 = unsigned(-1) - mask2;
    for (int i = 0; i < LEN - q - 1; ++i) {
        val_[i] = ((val_[i + q] & mask1) >> r) +
                  ((val_[i + q + 1] & mask2) << (32 - r));
    }
    val_[LEN - q - 1] = (val_[LEN - 1] & mask1) << r;
    if (q)
        std::memset(val_ + LEN - q, 0,
                    sizeof(unsigned) * static_cast<unsigned>(q));
    return *this;
}

template <int LEN>
BigInt<LEN> operator>>(BigInt<LEN> lhs, int shift) {
    return lhs >>= shift;
}

template <int LEN>
void BigInt<LEN>::PrintBinU(std::FILE* f) {
    int st = LEN;
    // template inheritance requires this for base member access
    while (st > 0 && !this->val_[--st])
        ;
    unsigned mask[32];
    char str[33];
    str[32] = '\0';
    int i, j;
    for (i = 0; i < 32; ++i) mask[i] = 1 << i;
    for (j = 31; j >= 0; --j)
        if (mask[j] & this->val_[st])
            str[31 - j] = '1';
        else
            str[31 - j] = '0';
    j = 0;
    while (j < 31 && str[j] == '0') ++j;
    std::fprintf(f, "0b%s", str + j);
    for (i = st - 1; i >= 0; --i) {
        for (j = 31; j >= 0; --j)
            if (mask[j] & this->val_[i])
                str[j] = '1';
            else
                str[j] = '0';
        std::fprintf(f, "%s", str);
    }
}

template <int LEN>
void BigInt<LEN>::PrintHexU(std::FILE* f) {
    int st = LEN;
    // template inheritance requires this for base member access
    while (st > 0 && !this->val_[--st])
        ;
    std::fprintf(f, "0x%x", this->val_[st]);
    for (int i = st - 1; i >= 0; --i) std::fprintf(f, "%08x", this->val_[i]);
}

template <int LEN>
void BigInt<LEN>::PrintHEXU(std::FILE* f) {
    int st = LEN;
    // template inheritance requires this for base member access
    while (st > 0 && !this->val_[--st])
        ;
    std::fprintf(f, "0X%X", this->val_[st]);
    for (int i = st - 1; i >= 0; --i) std::fprintf(f, "%08X", this->val_[i]);
}

// explicit instantiation
template class BigInt<4>;
template class BigInt<8>;
template class BigInt<16>;
template class BigInt<32>;
template class BigInt<64>;
template class BigInt<128>;
template class BigInt<256>;
}  // namespace cryp
