#include "bigint.h"
namespace cryp {

template <int LEN>
BigInt<LEN>::BigInt(long val) {
    val_ = new unsigned[LEN];
    val_[0] = (unsigned)val;
    val_[1] = (unsigned)(val >> 32);
}

template <int LEN>
BigInt<LEN>::BigInt(const BigInt<LEN>& rhs) {
    val_ = new unsigned[LEN];
    std::memcpy(val_, rhs.val_, LEN * sizeof(unsigned));
}

template <int LEN>
BigInt<LEN>::BigInt(BigInt<LEN>&& rhs) {
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
BigInt<LEN>& BigInt<LEN>::operator=(BigInt<LEN>&& rhs) {
    ::cryp::swap(*this, rhs);
    return *this;
    // rhs will be destroyed elsewhere
}

template <int LEN>
BigInt<LEN>::operator bool() const {
    for (int i = 0; i < LEN; ++i)
        if (val_[i]) return 1;
    return 0;
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
    // a-b=~(~a+b)
    ToBitRev();
    *this += rhs;
    ToBitRev();
    return *this;
}

template <int LEN>
BigInt<LEN> operator-(BigInt<LEN> lhs, const BigInt<LEN>& rhs) {
    return lhs -= rhs;
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
}  // namespace cryp
