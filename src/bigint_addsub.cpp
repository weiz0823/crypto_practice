#include "bigint.hpp"
namespace calc {
// operator +=,-=,++,--
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::operator+=(IntT rhs) {
    auto sign = Sign();
    val_[0] += rhs;
    if (val_[0] < rhs) {
        for (size_t i = 1; i < len_; ++i) {
            ++val_[i];
            if (val_[i]) break;
        }
    }
    // actual overflow is not indicated by variable $(overflow)
    if (!sign && Sign()) {
        SetLen(len_ + 1, false);
    } else {
        ShrinkLen();
    }
    return *this;
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::operator+=(const BigInt& rhs) {
    IntT overflow = 0;
    auto sign = Sign();
    auto rhs_sign = rhs.Sign();
    auto rhs_empty_limb = rhs_sign ? IntT(-1) : IntT(0);
    if (len_ <= rhs.len_) {
        if (len_ < rhs.len_) SetLen(rhs.len_, true);
        for (size_t i = 0; i < len_; ++i) {
            val_[i] += overflow + rhs.val_[i];
            if (val_[i] < rhs.val_[i] || (overflow && val_[i] <= rhs.val_[i]))
                overflow = 1;
            else
                overflow = 0;
        }
    } else {
        for (size_t i = 0; i < rhs.len_; ++i) {
            val_[i] += overflow + rhs.val_[i];
            if (val_[i] < rhs.val_[i] || (overflow && val_[i] <= rhs.val_[i]))
                overflow = 1;
            else
                overflow = 0;
        }
        // implicit alignment of len_
        for (size_t i = rhs.len_; i < len_; ++i) {
            val_[i] += overflow + rhs_empty_limb;
            if (val_[i] < rhs_empty_limb ||
                (overflow && val_[i] <= rhs_empty_limb))
                overflow = 1;
            else
                overflow = 0;
        }
    }
    // actual overflow is not indicated by variable $(overflow)
    if (sign == rhs_sign && Sign() != sign) {
        SetLen(len_ + 1, false);
        val_[len_ - 1] = rhs_empty_limb;
    } else {
        ShrinkLen();
    }
    return *this;
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::operator-=(const BigInt& rhs) {
    // a-b=~(~a+b)
    ToBitInv();
    *this += rhs;
    ToBitInv();
    return *this;
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::operator++() {
    auto sign = Sign();
    auto max_limb = IntT(-1);
    for (size_t i = 0; i < len_; ++i) {
        if (val_[i] == max_limb) {
            val_[i] = IntT(0);
        } else {
            ++val_[i];
            break;
        }
    }
    if (Sign() && !sign) {
        SetLen(len_ + 1, false);
    } else {
        ShrinkLen();
    }
    return *this;
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::operator--() {
    ToBitInv();
    ++*this;
    ToBitInv();
    return *this;
}
template <typename IntT>
BigInt<IntT> BigInt<IntT>::operator++(int) {
    BigInt<IntT> obj = *this;
    ++*this;
    return obj;
}
template <typename IntT>
BigInt<IntT> BigInt<IntT>::operator--(int) {
    BigInt<IntT> obj = *this;
    --*this;
    return obj;
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::ToOpposite() {
    ToBitInv();
    return ++*this;
}
template <typename IntT>
BigInt<IntT> BigInt<IntT>::operator-() const {
    BigInt<IntT> obj = *this;
    return obj.ToOpposite();
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::ToAbsolute() {
    if (Sign())
        return ToOpposite();
    else
        return *this;
}

// non-modifying binary operators
template <typename IntT>
BigInt<IntT> operator+(BigInt<IntT> lhs, const BigInt<IntT>& rhs) {
    return lhs += rhs;
}
template <typename IntT>
BigInt<IntT> operator-(BigInt<IntT> lhs, const BigInt<IntT>& rhs) {
    return lhs -= rhs;
}
}  // namespace calc
