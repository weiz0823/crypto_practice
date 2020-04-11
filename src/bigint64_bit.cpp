#include "bigint64.hpp"
namespace calc {
BigInt<uint128_t>& BigInt<uint128_t>::ToBitInv() {
    uint128_t* it = val_;
    do {
        *it = ~*it;
        ++it;
    } while (it != end_);
    return *this;
}
BigInt<uint128_t> BigInt<uint128_t>::operator~() const {
    auto obj = *this;
    return obj.ToBitInv();
}
BigInt<uint128_t>& BigInt<uint128_t>::operator&=(const BigInt<uint128_t>& rhs) {
    auto it = val_;
    auto cit = rhs.val_;
    if (len_ == rhs.len_) {
        do {
            *it &= *cit;
            ++it;
            ++cit;
        } while (it != end_);
    } else if (len_ < rhs.len_) {
        bool sign = Sign();
        do {
            *it &= *cit;
            ++it;
            ++cit;
        } while (it != end_);
        if (sign) {
            auto old_len = len_;
            SetLen(rhs.len_, false);
            it = val_ + old_len;
            // std::memcpy(it, cit, (end_ - it) * sizeof(uint128_t));
            std::copy(cit, rhs.end_, it);
        }
    } else {
        do {
            *it &= *cit;
            ++it;
            ++cit;
        } while (cit != rhs.end_);
        if (!rhs.Sign()) {
            SetLen(rhs.len_ + 1, false);
            *(end_ - 1) = 0;
        }
    }
    ShrinkLen();
    return *this;
}
BigInt<uint128_t>& BigInt<uint128_t>::operator|=(const BigInt<uint128_t>& rhs) {
    auto it = val_;
    auto cit = rhs.val_;
    if (len_ == rhs.len_) {
        do {
            *it |= *cit;
            ++it;
            ++cit;
        } while (it != end_);
    } else if (len_ < rhs.len_) {
        bool sign = Sign();
        do {
            *it |= *cit;
            ++it;
            ++cit;
        } while (it != end_);
        if (!sign) {
            auto old_len = len_;
            SetLen(rhs.len_, false);
            it = val_ + old_len;
            // std::memcpy(it, cit, (end_ - it) * sizeof(uint128_t));
            std::copy(cit, rhs.end_, it);
        }
    } else {
        do {
            *it |= *cit;
            ++it;
            ++cit;
        } while (cit != rhs.end_);
        if (rhs.Sign()) {
            SetLen(rhs.len_ + 1, false);
            *(end_ - 1) = -1;
        }
    }
    ShrinkLen();
    return *this;
}
BigInt<uint128_t>& BigInt<uint128_t>::operator^=(const BigInt<uint128_t>& rhs) {
    auto it = val_;
    auto cit = rhs.val_;
    if (len_ == rhs.len_) {
        do {
            *it ^= *cit;
            ++it;
            ++cit;
        } while (it != end_);
    } else if (len_ < rhs.len_) {
        bool sign = Sign();
        do {
            *it ^= *cit;
            ++it;
            ++cit;
        } while (it != end_);
        auto old_len = len_;
        if (sign) {
            SetLen(rhs.len_, false);
            it = val_ + old_len;
            do {
                *it = ~*cit;
                ++it;
                ++cit;
            } while (it != end_);
        } else {
            SetLen(rhs.len_, false);
            it = val_ + old_len;
            // std::memcpy(it, cit, (end_ - it) * sizeof(uint128_t));
            std::copy(cit, rhs.end_, it);
        }
    } else {
        bool rhs_sign = rhs.Sign();
        do {
            *it ^= *cit;
            ++it;
            ++cit;
        } while (cit != rhs.end_);
        if (rhs_sign) {
            do {
                *it = ~*it;
                ++it;
            } while (it != end_);
        }
    }
    ShrinkLen();
    return *this;
}
BigInt<uint128_t>& BigInt<uint128_t>::operator<<=(uint64_t rhs) {
    // preserve sign
    if (!rhs) return *this;
    uint64_t q = rhs >> LOGLIMB, r = rhs & (LIMB - 1);
    SetLen(len_ + q + (r > 0), true);
    uint128_t* it1 = end_ - 1;
    uint128_t* it2 = it1 - q;
    if (r) {
        for (; it2 > val_; --it1, --it2)
            *it1 = (*it2 << r) | (*(it2 - 1) >> (LIMB - r));
        *it1 = *it2 << r;
    } else {
        // caution q==0
        for (; it2 >= val_; --it1, --it2) *it1 = *it2;
    }
    std::fill(val_, val_ + q, 0);
    ShrinkLen();
    return *this;
}
BigInt<uint128_t>& BigInt<uint128_t>::operator>>=(uint64_t rhs) {
    if (!rhs) return *this;
    uint64_t q = rhs >> LOGLIMB, r = rhs & (LIMB - 1);
    if (len_ <= q) {
        SetLen(2, false);
        *val_ = *(val_ + 1) = 0;
    } else {
        uint128_t* it1 = val_;
        uint128_t* it2 = it1 + q;
        if (r) {
            if (Sign()) *end_ = -1;  // temporary sign extend
            for (; it2 < end_; ++it1, ++it2)
                *it1 = (*it2 >> r) | (*(it2 + 1) << (LIMB - r));
            *end_ = 0;
        } else {
            // caution q==0
            for (; it2 < end_; ++it1, ++it2) *it1 = *it2;
        }
        SetLen(len_ - q, false);
        ShrinkLen();
    }
    return *this;
}
}  // namespace calc
