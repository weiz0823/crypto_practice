#include "bigint.hpp"

#include "bigint_addsub.cpp"
#include "bigint_bit_arith.cpp"
#include "bigint_divmod.cpp"
#include "bigint_ext.cpp"
#include "bigint_io.cpp"
#include "bigint_mul.cpp"
// If there are other cpp files, include them here,
// because instanizaiton is only here.
namespace calc {
// constructors
template <typename IntT>
BigInt<IntT>::BigInt(int value)
    : cap_(sizeof(int) * 8 <= LIMB ? 1 : sizeof(int) * 8 / LIMB),
      len_(0),
      val_(new IntT[cap_]) {
    if constexpr (sizeof(int) * 8 <= LIMB) {
        val_[0] = IntT(value);
        len_ = 1;
    } else {
        do {
            val_[len_++] = IntT(value);
            value >>= LIMB;
        } while (value != 0 && value != -1);
        if (len_ < cap_) std::fill(val_ + len_, val_ + cap_, IntT(0));
        if (len_ == 0) len_ = 1;
        ShrinkLen();
    }
}
template <typename IntT>
BigInt<IntT>::BigInt(uint64_t value)
    : cap_(64 / LIMB + 1), len_(0), val_(new IntT[cap_]) {
    do {
        val_[len_++] = IntT(value);
        value >>= LIMB;
    } while (value != 0);
    std::fill(val_ + len_, val_ + cap_, IntT(0));
    if (Sign()) ++len_;
    ShrinkLen();
}
// copy constructor
template <typename IntT>
BigInt<IntT>::BigInt(const BigInt<IntT>& rhs) {
    if (&rhs == this) {
        // self-copy
        cap_ = 1;
        len_ = cap_;
        val_ = new IntT[cap_];
        std::fill(val_, val_ + cap_, IntT(0));
    } else {
        is_signed_ = rhs.is_signed_;
        cap_ = rhs.cap_;
        len_ = rhs.len_;
        val_ = new IntT[rhs.cap_];
        std::copy(rhs.val_, rhs.val_ + rhs.cap_, val_);
    }
}
// move constructor
template <typename IntT>
BigInt<IntT>::BigInt(BigInt<IntT>&& rhs) noexcept : BigInt() {
    // self-move guaranteed by swap
    std::swap(is_signed_, rhs.is_signed_);
    std::swap(cap_, rhs.cap_);
    std::swap(len_, rhs.len_);
    std::swap(val_, rhs.val_);
}

// destructor
template <typename IntT>
BigInt<IntT>::~BigInt() {
    delete[] val_;
}

// assignment & type conversion operators
// copy assignment, *only copy the value*
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::operator=(const BigInt<IntT>& rhs) {
    // check for self-copy assignment
    if (&rhs == this) return *this;
    if (len_ > rhs.len_) {
        std::copy(rhs.val_, rhs.val_ + rhs.len_, val_);
        std::fill(val_ + rhs.len_, val_ + len_, IntT(0));
    } else if (cap_ >= rhs.len_) {
        std::copy(rhs.val_, rhs.val_ + rhs.len_, val_);
    } else {
        delete[] val_;
        val_ = new IntT[rhs.cap_];
        std::copy(rhs.val_, rhs.val_ + rhs.cap_, val_);
        cap_ = rhs.cap_;
    }
    is_signed_ = rhs.is_signed_;
    len_ = rhs.len_;
    return *this;
}
// move assignment, after move becomes 0, but with whatever capacity
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::operator=(BigInt<IntT>&& rhs) noexcept {
    std::fill(val_, val_ + len_, IntT(0));
    len_ = 1;
    is_signed_ = true;
    // self-safe guaranteed by swap
    std::swap(is_signed_, rhs.is_signed_);
    std::swap(cap_, rhs.cap_);
    std::swap(len_, rhs.len_);
    std::swap(val_, rhs.val_);
    return *this;
}
template <typename IntT>
BigInt<IntT>::operator bool() const {
    for (size_t i = 0; i < len_; ++i)
        if (val_[i]) return true;
    return false;
}
template <typename IntT>
BigInt<IntT>::operator uint64_t() const {
    constexpr size_t seg = 64 / LIMB;
    size_t i = len_ < seg ? len_ : seg;
    uint64_t rv = 0;
    while (i > 0) rv = (rv << LIMB) | val_[--i];
    return rv;
}

// basic operations
template <typename IntT>
bool BigInt<IntT>::Sign() const {
    return val_[len_ - 1] >> (LIMB - 1);
}
template <typename IntT>
bool BigInt<IntT>::Parity() const {
    return val_[0] & 1;
}
template <typename IntT>
size_t BigInt<IntT>::TrailingZero() const {
    size_t i = 0, j = 0;
    while (i < len_ && val_[i] == 0) ++i;
    if (i == len_) {
        return 0;  // is zero
    } else {
        IntT t = val_[i];
        while ((t & 1) == 0) {
            ++j;
            t >>= 1;
        }
    }
    return i * LIMB + j;
}
template <typename IntT>
size_t BigInt<IntT>::BitLen() const {
    size_t i = len_ - 1, j = LIMB;
    while (i > 0 && val_[i] == 0) --i;
    while (j > 0 && val_[i] < (IntT(1) << (j - 1))) --j;
    return i * LIMB + j;
}
template <typename IntT>
size_t BigInt<IntT>::Shrink() {
    ShrinkLen();
    size_t target = len_ << 1;
    size_t new_cap = cap_;
    if (len_ < MAX_CAP && cap_ >= target) {
        while (new_cap >= target) new_cap >>= 1;
        Resize(new_cap);
    }
    return cap_;
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::CutLen(size_t seg_len, size_t bit_len) {
    if (seg_len > len_) return *this;
    bit_len %= LIMB;
    if (bit_len != 0) {
        SetLen(seg_len, true);
        auto mask = IntT(((IntT(1) << bit_len) - 1) | (IntT(1) << (LIMB - 1)));
        val_[len_ - 1] &= mask;
    } else if (Sign() != (val_[seg_len - 1] >> (LIMB - 1))) {
        SetLen(seg_len + 1, true);
        val_[len_ - 1] = Sign() ? IntT(-1) : IntT(0);
    }
    return *this;
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::CutBit(size_t bitlen) {
    if (bitlen % LIMB == 0)
        return CutLen(bitlen / LIMB, 0);
    else
        return CutLen(bitlen / LIMB + 1, bitlen % LIMB);
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::GenRandom(size_t length, size_t fixed) {
    if (length == 0) length = len_;
    SetLen(length, false);
    for (size_t i = 0; i < len_; ++i) val_[i] = rand_(rand_gen_);
    if (fixed != 0) {
        fixed %= LIMB;
        if (fixed == 0) {
            val_[len_ - 1] |= IntT(1) << (LIMB - 1);
            SetLen(len_ + 1, false);
        } else {
            val_[len_ - 1] <<= (LIMB - fixed);
            val_[len_ - 1] >>= (LIMB - fixed);
            val_[len_ - 1] |= IntT(1) << (fixed - 1);
        }
    } else if (Sign()) {
        SetLen(len_ + 1, false);
    }
    return *this;
}
template <typename IntT>
const IntT* BigInt<IntT>::Data() const {
    return val_;
}
template <typename IntT>
size_t BigInt<IntT>::Length() const {
    return len_;
}

// comparison
#ifdef __cpp_impl_three_way_comparison
template <typename IntT>
std::strong_ordering BigInt<IntT>::operator<=>(const BigInt& rhs) const {
    const bool sign[2] = {Sign(), rhs.Sign()};
    auto cmp = len_ <=> rhs.len_;
    if (sign[0] != sign[1]) {
        if (sign[1])
            return std::strong_ordering::greater;
        else
            return std::strong_ordering::less;
    } else if (cmp == 0) {
        // i>=0 is always true!
        for (size_t i = len_ - 1; i != size_t(-1); --i)
            if ((cmp = val_[i] <=> rhs.val_[i]) != 0) return cmp;
    } else if (cmp < 0) {
        const IntT empty_limb = sign[0] ? IntT(-1) : IntT(0);
        // implicit alignment
        for (size_t i = rhs.len_ - 1; i >= len_; --i)
            if (rhs.val_[i] != empty_limb) {
                if (sign[0])
                    return std::strong_ordering::greater;
                else
                    return std::strong_ordering::less;
            }
        for (size_t i = len_ - 1; i != size_t(-1); --i)
            if ((cmp = val_[i] <=> rhs.val_[i]) != 0) return cmp;
    } else {
        const IntT empty_limb = sign[0] ? IntT(-1) : IntT(0);
        // implicit alignment
        for (size_t i = len_ - 1; i >= rhs.len_; --i)
            if (val_[i] != empty_limb) {
                if (!sign[0])
                    return std::strong_ordering::greater;
                else
                    return std::strong_ordering::less;
            }
        for (size_t i = rhs.len_ - 1; i != size_t(-1); --i)
            if ((cmp = val_[i] <=> rhs.val_[i]) != 0) return cmp;
    }
    return std::strong_ordering_equal;
}
#else
template <typename IntT>
int BigInt<IntT>::Compare(const BigInt& rhs) const {
    const bool sign[2] = {Sign(), rhs.Sign()};
    if (sign[0] != sign[1]) {
        if (sign[1])
            return 1;
        else
            return -1;
    } else if (len_ == rhs.len_) {
        // i>=0 is always true!
        for (size_t i = len_ - 1; i != size_t(-1); --i) {
            if (val_[i] != rhs.val_[i]) {
                if (val_[i] > rhs.val_[i])
                    return 1;
                else
                    return -1;
            }
        }
    } else if (len_ < rhs.len_) {
        const IntT empty_limb = sign[0] ? IntT(-1) : IntT(0);
        // implicit alignment
        for (size_t i = rhs.len_ - 1; i >= len_; --i)
            if (rhs.val_[i] != empty_limb) {
                if (sign[0])
                    return 1;
                else
                    return -1;
            }
        for (size_t i = len_ - 1; i != size_t(-1); --i) {
            if (val_[i] != rhs.val_[i]) {
                if (val_[i] > rhs.val_[i])
                    return 1;
                else
                    return -1;
            }
        }
    } else {
        const IntT empty_limb = sign[0] ? IntT(-1) : IntT(0);
        // implicit alignment
        for (size_t i = len_ - 1; i >= rhs.len_; --i)
            if (val_[i] != empty_limb) {
                if (!sign[0])
                    return 1;
                else
                    return -1;
            }
        for (size_t i = rhs.len_ - 1; i != size_t(-1); --i) {
            if (val_[i] != rhs.val_[i]) {
                if (val_[i] > rhs.val_[i])
                    return 1;
                else
                    return -1;
            }
        }
    }
    return 0;
}
#endif

// private constructor
template <typename IntT>
template <typename _IntT>
BigInt<IntT>::BigInt(const _IntT* data, size_t length) {
    cap_ = 1;
    constexpr size_t limb_len = sizeof(_IntT) << 3;
    if (length == 0) {
        len_ = 1;
        val_ = new IntT[1];
        val_[0] = IntT(0);
    } else if constexpr (limb_len > LIMB) {
        size_t t = limb_len / LIMB;
        len_ = length * t;
        if (len_ > MAX_CAP) len_ = MAX_CAP;
        while (cap_ < len_) cap_ <<= 1;
        val_ = new IntT[cap_];
        size_t mov = 0;
        size_t i;
        switch (t) {
            case 2:
                for (i = 0; i < length; ++i) {
                    val_[i << 1] = IntT(data[i]);
                    val_[(i << 1) + 1] = IntT(data[i] >> LIMB);
                }
                break;
            case 4:
                for (i = 0; i < length; ++i) {
                    mov = 0;
                    for (uint8_t j = 0; j < 4; ++j) {
                        val_[(i << 2) + j] = IntT(data[i] >> mov);
                        mov += LIMB;
                    }
                }
                break;
        }
        if (cap_ > len_) std::fill(val_ + len_, val_ + cap_, IntT(0));
        ShrinkLen();
    } else if constexpr (limb_len < LIMB) {
        size_t t = LIMB / limb_len;
        len_ = (length + t - 1) / t;
        if (len_ > MAX_CAP) len_ = MAX_CAP;
        while (cap_ < len_) cap_ <<= 1;
        val_ = new IntT[cap_];
        size_t mov = 0;
        size_t i;
        switch (t) {
            case 2:
                for (i = 0; i < len_ - 1; ++i) {
                    val_[i] = IntT(IntT(data[(i << 1) + 1]) << limb_len) |
                              data[i << 1];
                }
                if (!(length & 1)) {
                    val_[len_ - 1] = IntT(IntT(data[length - 1]) << limb_len) |
                                     data[length - 2];
                } else if (data[length - 1] >> (limb_len - 1)) {
                    val_[len_ - 1] =
                        IntT(IntT(-1) << limb_len) | data[length - 1];
                } else {
                    val_[len_ - 1] = data[length - 1];
                }
                break;
            case 4:
                for (i = 0; i < len_ - 1; ++i) {
                    mov = 0;
                    val_[i] = 0;
                    for (uint8_t j = 0; j < 4; ++j) {
                        val_[i] |= IntT(data[(i << 2) + j]) << mov;
                        mov += limb_len;
                    }
                }
                mov = 0;
                val_[i] = 0;
                for (size_t j = i << 2; j < length; ++j) {
                    val_[i] |= IntT(data[j]) << mov;
                    mov += limb_len;
                }
                if (mov < LIMB && (data[length - 1] >> (limb_len - 1)))
                    val_[i] |= IntT(-1) << mov;
                break;
        }
        if (cap_ > len_) std::fill(val_ + len_, val_ + cap_, IntT(0));
    } else {
        len_ = length;
        if (len_ > MAX_CAP) len_ = MAX_CAP;
        while (cap_ < len_) cap_ <<= 1;
        val_ = new IntT[cap_];
        std::copy(data, data + len_, val_);
        if (cap_ > len_) std::fill(val_ + len_, val_ + cap_, IntT(0));
    }
}

// private functions
template <typename IntT>
void BigInt<IntT>::SetLen(size_t new_len, bool preserve_sign) {
    if (new_len == 0)
        new_len = 1;
    else if (new_len > MAX_CAP)
        new_len = MAX_CAP;
    const bool sign = Sign();
    const auto empty_limb = (sign && preserve_sign) ? IntT(-1) : IntT(0);
    if (new_len < len_) {
        while (len_ > new_len) val_[--len_] = IntT(0);
        if (preserve_sign) {
            // set the highest as sign bit
            val_[len_ - 1] &= IntT(-1) >> 1;
            val_[len_ - 1] |= IntT(sign << (LIMB - 1));
        }
        AutoShrinkSize();
    } else if (new_len > len_) {
        if (new_len > cap_) AutoExpandSize(new_len);
        while (len_ < new_len) val_[len_++] = empty_limb;
    }
    // if new_len == len_: do nothing
}
template <typename IntT>
void BigInt<IntT>::ShrinkLen() {
    const bool sign = Sign();
    const auto empty_limb = sign ? IntT(-1) : IntT(0);
    while (len_ > 1 && val_[len_ - 1] == empty_limb &&
           (val_[len_ - 2] >> (LIMB - 1)) == sign)
        val_[--len_] = IntT(0);
}
template <typename IntT>
void BigInt<IntT>::Resize(size_t new_cap) {
    if (new_cap == cap_) return;
    auto tmp_val = new IntT[new_cap];
    if (new_cap > cap_) {
        std::copy(val_, val_ + cap_, tmp_val);
        std::fill(tmp_val + cap_, tmp_val + new_cap, IntT(0));
    } else {
        std::copy(val_, val_ + new_cap, tmp_val);
    }
    std::swap(val_, tmp_val);
    delete[] tmp_val;
    cap_ = new_cap;
}
template <typename IntT>
void BigInt<IntT>::AutoExpandSize(size_t target_len) {
    size_t new_cap = cap_;
    while (new_cap < MAX_CAP && new_cap < target_len) new_cap <<= 1;
    Resize(new_cap);
}
// Shrink size if certain condition is met
template <typename IntT>
void BigInt<IntT>::AutoShrinkSize() {
    if (len_ <= (cap_ >> 3)) {
        size_t new_cap = cap_ >> 3;
        while (new_cap > 1 && len_ <= new_cap) new_cap >>= 1;
        Resize(new_cap << 1);
    }
}

#ifndef __cpp_impl_three_way_comparison
template <typename IntT>
bool operator<(const BigInt<IntT>& lhs, const BigInt<IntT>& rhs) {
    return lhs.Compare(rhs) < 0;
}
template <typename IntT>
bool operator>(const BigInt<IntT>& lhs, const BigInt<IntT>& rhs) {
    return lhs.Compare(rhs) > 0;
}
template <typename IntT>
bool operator<=(const BigInt<IntT>& lhs, const BigInt<IntT>& rhs) {
    return lhs.Compare(rhs) <= 0;
}
template <typename IntT>
bool operator>=(const BigInt<IntT>& lhs, const BigInt<IntT>& rhs) {
    return lhs.Compare(rhs) >= 0;
}
template <typename IntT>
bool operator==(const BigInt<IntT>& lhs, const BigInt<IntT>& rhs) {
    return lhs.Compare(rhs) == 0;
}
template <typename IntT>
bool operator!=(const BigInt<IntT>& lhs, const BigInt<IntT>& rhs) {
    return lhs.Compare(rhs) != 0;
}
#endif

// explicit instanization
template class BigInt<uint8_t>;
template class BigInt<uint16_t>;
template class BigInt<uint32_t>;

// explicit instanizaiton of functions
template BigInt<uint8_t> operator&(BigInt<uint8_t> lhs,
                                   const BigInt<uint8_t>& rhs);
template BigInt<uint8_t> operator|(BigInt<uint8_t> lhs,
                                   const BigInt<uint8_t>& rhs);
template BigInt<uint8_t> operator^(BigInt<uint8_t> lhs,
                                   const BigInt<uint8_t>& rhs);
template BigInt<uint8_t> operator<<(BigInt<uint8_t> lhs, size_t rhs);
template BigInt<uint8_t> operator>>(BigInt<uint8_t> lhs, size_t rhs);
template std::istream& operator>>(std::istream& in, BigInt<uint8_t>& rhs);
template std::ostream& operator<<(std::ostream& out,
                                  const BigInt<uint8_t>& rhs);
template BigInt<uint8_t> operator+(BigInt<uint8_t> lhs,
                                   const BigInt<uint8_t>& rhs);
template BigInt<uint8_t> operator-(BigInt<uint8_t> lhs,
                                   const BigInt<uint8_t>& rhs);
template BigInt<uint8_t> operator*(BigInt<uint8_t> lhs, uint8_t rhs);
template BigInt<uint8_t> operator*(BigInt<uint8_t> lhs,
                                   const BigInt<uint8_t>& rhs);
template BigInt<uint8_t> operator/(BigInt<uint8_t> lhs, uint8_t rhs);
template BigInt<uint8_t> operator/(BigInt<uint8_t> lhs,
                                   const BigInt<uint8_t>& rhs);
template BigInt<uint8_t> operator%(BigInt<uint8_t> lhs, uint8_t rhs);
template BigInt<uint8_t> operator%(BigInt<uint8_t> lhs,
                                   const BigInt<uint8_t>& rhs);
template BigInt<uint8_t> BigProduct(uint64_t a, uint64_t b);
template BigInt<uint8_t> Factorial(uint64_t n);
template BigInt<uint8_t> Power(const BigInt<uint8_t>& a, uint64_t p);
template BigInt<uint8_t> PowMod(const BigInt<uint8_t>& a, uint64_t p,
                                const BigInt<uint8_t>& n);
template BigInt<uint8_t> PowMod(const BigInt<uint8_t>& a,
                                const BigInt<uint8_t>& p,
                                const BigInt<uint8_t>& n);
template BigInt<uint8_t> GcdBin(BigInt<uint8_t> a, BigInt<uint8_t> b);
template BigInt<uint8_t> ExtGcdBin(BigInt<uint8_t> a, BigInt<uint8_t> b,
                                   BigInt<uint8_t>* x, BigInt<uint8_t>* y);

template BigInt<uint16_t> operator&(BigInt<uint16_t> lhs,
                                    const BigInt<uint16_t>& rhs);
template BigInt<uint16_t> operator|(BigInt<uint16_t> lhs,
                                    const BigInt<uint16_t>& rhs);
template BigInt<uint16_t> operator^(BigInt<uint16_t> lhs,
                                    const BigInt<uint16_t>& rhs);
template BigInt<uint16_t> operator<<(BigInt<uint16_t> lhs, size_t rhs);
template BigInt<uint16_t> operator>>(BigInt<uint16_t> lhs, size_t rhs);
template std::istream& operator>>(std::istream& in, BigInt<uint16_t>& rhs);
template std::ostream& operator<<(std::ostream& out,
                                  const BigInt<uint16_t>& rhs);
template BigInt<uint16_t> operator+(BigInt<uint16_t> lhs,
                                    const BigInt<uint16_t>& rhs);
template BigInt<uint16_t> operator-(BigInt<uint16_t> lhs,
                                    const BigInt<uint16_t>& rhs);
template BigInt<uint16_t> operator*(BigInt<uint16_t> lhs, uint16_t rhs);
template BigInt<uint16_t> operator*(BigInt<uint16_t> lhs,
                                    const BigInt<uint16_t>& rhs);
template BigInt<uint16_t> operator/(BigInt<uint16_t> lhs, uint16_t rhs);
template BigInt<uint16_t> operator/(BigInt<uint16_t> lhs,
                                    const BigInt<uint16_t>& rhs);
template BigInt<uint16_t> operator%(BigInt<uint16_t> lhs, uint16_t rhs);
template BigInt<uint16_t> operator%(BigInt<uint16_t> lhs,
                                    const BigInt<uint16_t>& rhs);
template BigInt<uint16_t> BigProduct(uint64_t a, uint64_t b);
template BigInt<uint16_t> Factorial(uint64_t n);
template BigInt<uint16_t> Power(const BigInt<uint16_t>& a, uint64_t p);
template BigInt<uint16_t> PowMod(const BigInt<uint16_t>& a, uint64_t p,
                                 const BigInt<uint16_t>& n);
template BigInt<uint16_t> PowMod(const BigInt<uint16_t>& a,
                                 const BigInt<uint16_t>& p,
                                 const BigInt<uint16_t>& n);
template BigInt<uint16_t> GcdBin(BigInt<uint16_t> a, BigInt<uint16_t> b);
template BigInt<uint16_t> ExtGcdBin(BigInt<uint16_t> a, BigInt<uint16_t> b,
                                    BigInt<uint16_t>* x, BigInt<uint16_t>* y);

template BigInt<uint32_t> operator&(BigInt<uint32_t> lhs,
                                    const BigInt<uint32_t>& rhs);
template BigInt<uint32_t> operator|(BigInt<uint32_t> lhs,
                                    const BigInt<uint32_t>& rhs);
template BigInt<uint32_t> operator^(BigInt<uint32_t> lhs,
                                    const BigInt<uint32_t>& rhs);
template BigInt<uint32_t> operator<<(BigInt<uint32_t> lhs, size_t rhs);
template BigInt<uint32_t> operator>>(BigInt<uint32_t> lhs, size_t rhs);
template std::istream& operator>>(std::istream& in, BigInt<uint32_t>& rhs);
template std::ostream& operator<<(std::ostream& out,
                                  const BigInt<uint32_t>& rhs);
template BigInt<uint32_t> operator+(BigInt<uint32_t> lhs,
                                    const BigInt<uint32_t>& rhs);
template BigInt<uint32_t> operator-(BigInt<uint32_t> lhs,
                                    const BigInt<uint32_t>& rhs);
template BigInt<uint32_t> operator*(BigInt<uint32_t> lhs, uint32_t rhs);
template BigInt<uint32_t> operator*(BigInt<uint32_t> lhs,
                                    const BigInt<uint32_t>& rhs);
template BigInt<uint32_t> operator/(BigInt<uint32_t> lhs, uint32_t rhs);
template BigInt<uint32_t> operator/(BigInt<uint32_t> lhs,
                                    const BigInt<uint32_t>& rhs);
template BigInt<uint32_t> operator%(BigInt<uint32_t> lhs, uint32_t rhs);
template BigInt<uint32_t> operator%(BigInt<uint32_t> lhs,
                                    const BigInt<uint32_t>& rhs);
template BigInt<uint32_t> BigProduct(uint64_t a, uint64_t b);
template BigInt<uint32_t> Factorial(uint64_t n);
template BigInt<uint32_t> Power(const BigInt<uint32_t>& a, uint64_t p);
template BigInt<uint32_t> PowMod(const BigInt<uint32_t>& a, uint64_t p,
                                 const BigInt<uint32_t>& n);
template BigInt<uint32_t> PowMod(const BigInt<uint32_t>& a,
                                 const BigInt<uint32_t>& p,
                                 const BigInt<uint32_t>& n);
template BigInt<uint32_t> GcdBin(BigInt<uint32_t> a, BigInt<uint32_t> b);
template BigInt<uint32_t> ExtGcdBin(BigInt<uint32_t> a, BigInt<uint32_t> b,
                                    BigInt<uint32_t>* x, BigInt<uint32_t>* y);
#ifndef __cpp_impl_three_way_comparison
template bool operator<(const BigInt<uint8_t>& lhs, const BigInt<uint8_t>& rhs);
template bool operator>(const BigInt<uint8_t>& lhs, const BigInt<uint8_t>& rhs);
template bool operator<=(const BigInt<uint8_t>& lhs,
                         const BigInt<uint8_t>& rhs);
template bool operator>=(const BigInt<uint8_t>& lhs,
                         const BigInt<uint8_t>& rhs);
template bool operator==(const BigInt<uint8_t>& lhs,
                         const BigInt<uint8_t>& rhs);
template bool operator!=(const BigInt<uint8_t>& lhs,
                         const BigInt<uint8_t>& rhs);
template bool operator<(const BigInt<uint16_t>& lhs,
                        const BigInt<uint16_t>& rhs);
template bool operator>(const BigInt<uint16_t>& lhs,
                        const BigInt<uint16_t>& rhs);
template bool operator<=(const BigInt<uint16_t>& lhs,
                         const BigInt<uint16_t>& rhs);
template bool operator>=(const BigInt<uint16_t>& lhs,
                         const BigInt<uint16_t>& rhs);
template bool operator==(const BigInt<uint16_t>& lhs,
                         const BigInt<uint16_t>& rhs);
template bool operator!=(const BigInt<uint16_t>& lhs,
                         const BigInt<uint16_t>& rhs);
template bool operator<(const BigInt<uint32_t>& lhs,
                        const BigInt<uint32_t>& rhs);
template bool operator>(const BigInt<uint32_t>& lhs,
                        const BigInt<uint32_t>& rhs);
template bool operator<=(const BigInt<uint32_t>& lhs,
                         const BigInt<uint32_t>& rhs);
template bool operator>=(const BigInt<uint32_t>& lhs,
                         const BigInt<uint32_t>& rhs);
template bool operator==(const BigInt<uint32_t>& lhs,
                         const BigInt<uint32_t>& rhs);
template bool operator!=(const BigInt<uint32_t>& lhs,
                         const BigInt<uint32_t>& rhs);
#endif
}  // namespace calc
