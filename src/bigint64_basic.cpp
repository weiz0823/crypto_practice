
#include "bigint64.hpp"
namespace calc {
BigInt<uint128_t>::BigInt(int value)
    : val_(new uint128_t[4]), len_(2), cap_(4), end_(val_ + 2) {
    // std::memset(val_, 0, cap_ * sizeof(uint128_t));
    std::fill(val_, val_ + cap_, 0);
    if (value < 0) {
        *val_ = -1;
        *val_ <<= 32;
        *val_ = value;
        *(val_ + 1) = -1;
    } else {
        *val_ = value;
    }
}
BigInt<uint128_t>::BigInt(uint64_t value)
    : val_(new uint128_t[4]), len_(2), cap_(4), end_(val_ + 2) {
    // std::memset(val_, 0, cap_ * sizeof(uint128_t));
    std::fill(val_, val_ + cap_, 0);
    *val_ = value;
}
BigInt<uint128_t>::BigInt(const BigInt<uint128_t>& rhs)
    : val_(new uint128_t[rhs.cap_]),
      len_(rhs.len_),
      cap_(rhs.cap_),
      end_(val_ + len_) {
    // std::memcpy(val_, rhs.val_, rhs.cap_ * sizeof(uint128_t));
    std::copy(rhs.val_, rhs.val_ + rhs.cap_, val_);
}
BigInt<uint128_t>::BigInt(BigInt<uint128_t>&& rhs) noexcept : BigInt() {
    std::swap(val_, rhs.val_);
    std::swap(len_, rhs.len_);
    std::swap(cap_, rhs.cap_);
    std::swap(end_, rhs.end_);
}
BigInt<uint128_t>& BigInt<uint128_t>::operator=(const BigInt<uint128_t>& rhs) {
    if (this == &rhs) return *this;
    if (rhs.len_ < cap_) {
        // std::memcpy(val_, rhs.val_, rhs.len_ * sizeof(uint128_t));
        std::copy(rhs.val_, rhs.val_ + rhs.len_, val_);
        end_ = val_ + rhs.len_;
        if (len_ > rhs.len_)
            // std::memset(end_, 0, (len_ - rhs.len_) * sizeof(uint128_t));
            std::fill(val_ + rhs.len_, val_ + len_, 0);
        len_ = rhs.len_;
    } else {
        delete[] val_;
        val_ = new uint128_t[rhs.cap_];
        // std::memcpy(val_, rhs.val_, rhs.cap_ * sizeof(uint128_t));
        std::copy(rhs.val_, rhs.val_ + rhs.cap_, val_);
        cap_ = rhs.cap_;
        len_ = rhs.len_;
        end_ = val_ + len_;
    }
    return *this;
}
BigInt<uint128_t>& BigInt<uint128_t>::operator=(
    BigInt<uint128_t>&& rhs) noexcept {
    if (this == &rhs) return *this;
    std::swap(val_, rhs.val_);
    std::swap(len_, rhs.len_);
    std::swap(cap_, rhs.cap_);
    std::swap(end_, rhs.end_);
    return *this;
}
BigInt<uint128_t>::~BigInt() { delete[] val_; }
const uint128_t* BigInt<uint128_t>::Data() const { return val_; }
uint64_t BigInt<uint128_t>::Length() const { return len_; }
BigInt<uint128_t>::operator bool() const {
    auto it = val_;
    do {
        if (*it) return true;
        ++it;
    } while (it != end_);
    return false;
}
BigInt<uint128_t>::operator int64_t() const {
    int64_t r = static_cast<int64_t>(*val_);
    if (Sign() != bool(r >> 63)) {
        r <<= 1;
        r >>= 1;
        if (Sign()) r |= int64_t(1) << 63;
    }
    return r;
}
bool BigInt<uint128_t>::Sign() const { return *(end_ - 1) >> (LIMB - 1); }
bool BigInt<uint128_t>::Parity() const { return *val_ & 1; }
uint64_t BigInt<uint128_t>::TrailingZero() const {
    auto it = val_;
    while (!*it && it != end_) ++it;
    if (it == end_) return 1;  // is zero, is even number
    uint64_t j;
    if (*it << 64) {
        asm("bsfq (%1), %%r9\n\tmovq %%r9, %0"
            : "=m"(j)
            : "r"(it)
            : "cc", "memory", "r9");
    } else {
        asm("bsfq 8(%1), %%r9\n\tmovq %%r9, %0"
            : "=m"(j)
            : "r"(it)
            : "cc", "memory", "r9");
        j += 64;
    }
    return ((it - val_) << LOGLIMB) + j;
}
uint64_t BigInt<uint128_t>::BitLen() const {
    auto it = end_ - 1;
    while (it >= val_ && !*it) --it;
    if (it < val_) return 0;  // is zero
    uint64_t j;
    if (*it >> 64) {
        asm("bsrq 8(%1), %%r9\n\tmovq %%r9, %0"
            : "=m"(j)
            : "r"(it)
            : "cc", "memory", "r9");
        j += 65;
    } else {
        asm("bsrq (%1), %%r9\n\tmovq %%r9, %0"
            : "=m"(j)
            : "r"(it)
            : "cc", "memory", "r9");
        ++j;
    }
    return ((it - val_) << LOGLIMB) + j;
}
void BigInt<uint128_t>::SetLen(uint64_t new_len, bool preserve_sign) {
    bool sign = Sign();
    // constexpr auto int_size = sizeof(uint128_t);
    if (!new_len) {
        std::fill(val_, end_, 0);
        len_ = 2;
        end_ = val_ + len_;
        return;
    }
    if (new_len > len_) {
        if (new_len >= cap_) {
            cap_ <<= 1;
            while (cap_ <= new_len) cap_ <<= 1;
            auto tmp_ptr = new uint128_t[cap_];
            // std::memcpy(tmp_ptr, val_, (end_ - val_) * int_size);
            std::copy(val_, end_, tmp_ptr);
            delete[] val_;
            val_ = tmp_ptr;
            tmp_ptr = nullptr;
            // std::memset(val_ + new_len, 0, (cap_ - new_len) * int_size);
            std::fill(val_ + new_len, val_ + cap_, 0);
            if (preserve_sign && sign)
                // std::memset(val_ + len_, -1, (new_len - len_) * int_size);
                std::fill(val_ + len_, val_ + new_len, -1);
            else
                // std::memset(val_ + len_, 0, (new_len - len_) * int_size);
                std::fill(val_ + len_, val_ + new_len, 0);
        } else {
            if (preserve_sign && sign)
                // std::memset(val_ + len_, -1, (new_len - len_) * int_size);
                std::fill(val_ + len_, val_ + new_len, -1);
        }
        len_ = new_len;
        end_ = val_ + len_;
    } else if (new_len < len_) {
        end_ = val_ + new_len;
        auto it = end_ - 1;
        if (preserve_sign) {
            *it <<= 1;
            *it >>= 1;
            if (sign) *it |= uint128_t(1) << 127;
        }
        // std::memset(end_, 0, (len_ - new_len) * int_size);
        std::fill(val_ + new_len, val_ + len_, 0);
        if (new_len == 1) {
            end_ = val_ + 2;
            len_ = 2;
            if (preserve_sign && sign) *(val_ + 1) = -1;
        } else {
            len_ = new_len;
        }
    }
}
void BigInt<uint128_t>::ShrinkLen() {
    if (len_ <= 2) return;
    --end_;
    if (*end_ >> (LIMB - 1)) {
        while (!(++*end_) && end_ - val_ >= 2) --end_;
        --*end_;
        if (!(*end_ >> (LIMB - 1))) {
            ++end_;
            --*end_;  // 0 -> -1
        }
        ++end_;
        len_ = end_ - val_;
    } else {
        while (!*end_ && end_ - val_ >= 2) --end_;
        if (*end_ >> (LIMB - 1))
            end_ += 2;
        else
            ++end_;
        len_ = end_ - val_;
    }
}
void BigInt<uint128_t>::Shrink() {
    uint64_t term = (len_ << 1) + 1;
    if (cap_ > term) {
        do
            cap_ >>= 1;
        while (cap_ > term);
        auto tmp_ptr = new uint128_t[cap_];
        std::copy(val_, val_ + cap_, tmp_ptr);
        delete[] val_;
        val_ = tmp_ptr;
        tmp_ptr = nullptr;
        end_ = val_ + len_;
    }
}
BigInt<uint128_t>& BigInt<uint128_t>::GenRandom(uint64_t length,
                                                uint8_t fixed) {
    if (!length) length = len_;
    SetLen(length + 1, false);
    auto it = val_, term = val_ + length;
    do {
        *it = (uint128_t(rand_(rand_gen_)) << 64) | rand_(rand_gen_);
        ++it;
    } while (it != term);
    *it = 0;
    if (fixed) {
        --it;
        uint128_t mask = 1;
        mask <<= fixed - 1;
        --mask;
        *it &= mask;
        ++mask;
        *it |= mask;
    }
    ShrinkLen();
    return *this;
}
double BigInt<uint128_t>::log2() const {
    auto it = end_ - 1;
    bool sign = Sign();
    uint128_t empty_limb = sign ? -1 : 0;
    while (*it == empty_limb && it > val_) --it;
    if (sign)
        return std::log2(double(~*it + 1)) + ((it - val_) << LOGLIMB);
    else
        return std::log2(double(*it)) + ((it - val_) << LOGLIMB);
}
double BigInt<uint128_t>::log10() const { return log2() / std::log2(10.0); }
}  // namespace calc
