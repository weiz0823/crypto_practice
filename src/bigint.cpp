#include "bigint.hpp"
#include "bigmul_div_mod.cpp"
namespace cryp {

template <unsigned LEN>
BigUInt<LEN>::BigUInt(const unsigned val) {
    val_ = new unsigned[LEN];
    // use std::fill instead of std::memset, it's not necessarily slower
    std::fill(val_, val_ + LEN, 0);
    val_[0] = val;
}

template <unsigned LEN>
BigUInt<LEN>::BigUInt(const uint64_t val) {
    val_ = new unsigned[LEN];
    // use std::fill instead of std::memset
    std::fill(val_, val_ + LEN, 0);
    // google-readability-casting told me to do that
    val_[0] = static_cast<unsigned>(val);
    val_[1] = static_cast<unsigned>(val >> 32);
}

// FIXME(): input may be slow
template <unsigned LEN>
BigUInt<LEN>::BigUInt(const char* str, unsigned base) : BigUInt(0u) {
    if (base <= 1 || base > 32) base = 0;
    int i = 0;
    if (base == 0) {
        if (str[0] == '0' && str[1] == 'b') {
            base = 2;
            i = 2;
        } else if (str[0] == '0' && str[1] == 'x') {
            base = 16;
            i = 2;
        } else {
            base = 10;
        }
    }
    if (base == 10) {
        for (; str[i] >= '0' && str[i] <= '9'; ++i) {
            *this <<= 1;
            *this += (*this << 2) +
                     BigUInt<LEN>(static_cast<unsigned>(str[i] - '0'));
        }
    } else if (base == 16) {
        for (; std::isdigit(str[i]) || ('a' <= str[i] && str[i] <= 'f'); ++i) {
            *this <<= 4;
            if (str[i] >= 'a')
                *this += BigUInt<LEN>(static_cast<unsigned>(str[i] - 'a' + 10));
            else
                *this += BigUInt<LEN>(static_cast<unsigned>(str[i] - '0'));
        }
    } else if (base == 2) {
        for (; str[i] == '0' || str[i] == '1'; ++i) {
            *this <<= 1;
            if (str[i] == '1') ++*this;
        }
    } else if (base < 10) {
        for (; str[i] >= '0' && str[i] < char('0' + base); ++i) {
            (*this) *= base;
            *this += BigUInt<LEN>(static_cast<unsigned>(str[i] - '0'));
        }
    } else {
        for (; (str[i] >= '0' && str[i] <= '9') ||
               (str[i] >= 'a' && str[i] < char('a' + base - 10));
             ++i) {
            (*this) *= base;
            if (str[i] >= 'a')
                *this += BigUInt<LEN>(static_cast<unsigned>(str[i] - 'a' + 10));
            else
                *this += BigUInt<LEN>(static_cast<unsigned>(str[i] - '0'));
        }
    }
}

template <unsigned LEN>
BigUInt<LEN>::BigUInt(const BigUInt<LEN>& rhs) {
    val_ = new unsigned[LEN];
    // copy/copy_backward isn't overlap safe
    // but memmove isn't non-POD (e.g. class type) safe
    // memcpy is really raw but isn't necessarily faster
    std::copy(rhs.val_, rhs.val_ + LEN, val_);
}

template <unsigned LEN>
BigUInt<LEN>::BigUInt(BigUInt<LEN>&& rhs) noexcept {
    val_ = rhs.val_;
    rhs.val_ = nullptr;
}

template <unsigned LEN>
BigUInt<LEN>::~BigUInt() {
    delete[] val_;
}

template <unsigned LEN>
BigUInt<LEN>& BigUInt<LEN>::operator=(const BigUInt<LEN>& rhs) {
    if (this != &rhs) std::copy(rhs.val_, rhs.val_ + LEN, val_);
    return *this;
}

template <unsigned LEN>
BigUInt<LEN>& BigUInt<LEN>::operator=(BigUInt<LEN>&& rhs) noexcept {
    // self-safe guaranteed by std::swap
    std::swap(val_, rhs.val_);
    return *this;
}

// template <unsigned LEN>
// [[deprecated]] BigUInt<LEN>& BigUInt<LEN>::operator=(BigUInt<LEN> rhs)
// noexcept
// {
// // self assignment check
// if (this != &rhs) ::cryp::swap(rhs, *this);
// return *this;
// // rhs will be automatically destroyed
// }

template <unsigned LEN>
template <unsigned _LEN>
BigUInt<LEN>::operator BigUInt<_LEN>() const {
    BigUInt<_LEN> t;
    if (LEN < _LEN)
        std::copy(val_, val_ + LEN, t.val_);
    else
        std::copy(val_, val_ + _LEN, t.val_);
    return t;
}

template <unsigned LEN>
BigUInt<LEN>::operator unsigned() const {
    return val_[0];
}

template <unsigned LEN>
BigUInt<LEN>::operator bool() const {
    for (unsigned i = 0; i < LEN; ++i)
        if (val_[i]) return true;
    return false;
}

template <unsigned LEN>
bool BigUInt<LEN>::Sign() const {
    return val_[LEN - 1] >> 31;
}

template <unsigned LEN>
BigUInt<LEN>& BigUInt<LEN>::ToBitRev() {
    // self-modify
    for (unsigned i = 0; i < LEN; ++i) val_[i] = ~val_[i];
    return *this;
}

template <unsigned LEN>
BigUInt<LEN>& BigUInt<LEN>::operator+=(const BigUInt<LEN>& rhs) {
    unsigned overflow_flag = 0;
    for (unsigned i = 0; i < LEN; ++i) {
        val_[i] += overflow_flag;
        val_[i] += rhs.val_[i];
        if (val_[i] < rhs.val_[i] || (overflow_flag && val_[i] <= rhs.val_[i]))
            overflow_flag = 1;
        else
            overflow_flag = 0;
    }
    return *this;
}

template <unsigned LEN>
BigUInt<LEN> operator+(BigUInt<LEN> lhs, const BigUInt<LEN>& rhs) {
    return lhs += rhs;
}

template <unsigned LEN>
BigUInt<LEN>& BigUInt<LEN>::operator-=(const BigUInt<LEN>& rhs) {
    // alternative: a-b=~(~a+b)
    // ToBitRev();
    // *this += rhs;
    // ToBitRev();
    unsigned overflow_flag = 0;
    for (unsigned i = 0; i < LEN; ++i) {
        val_[i] -= overflow_flag;
        val_[i] -= rhs.val_[i];
        if (val_[i] > rhs.val_[i] || (overflow_flag && val_[i] >= rhs.val_[i]))
            overflow_flag = 1;
        else
            overflow_flag = 0;
    }
    return *this;
}

template <unsigned LEN>
BigUInt<LEN> operator-(BigUInt<LEN> lhs, const BigUInt<LEN>& rhs) {
    return lhs -= rhs;
}

template <unsigned LEN>
BigUInt<LEN>& BigUInt<LEN>::operator++() {
    for (unsigned i = 0; i < LEN; ++i) {
        ++val_[i];
        if (val_[i]) return *this;
    }
    return *this;
}

template <unsigned LEN>
BigUInt<LEN>& BigUInt<LEN>::operator--() {
    // alternative: a-1=~(~a+1)
    for (unsigned i = 0; i < LEN; ++i) {
        --val_[i];
        if (val_[i] != unsigned(-1)) return *this;
    }
    return *this;
}

template <unsigned LEN>
BigUInt<LEN> BigUInt<LEN>::operator++(int) {
    BigUInt obj = *this;  // copy
    ++*this;
    return obj;
}

template <unsigned LEN>
BigUInt<LEN> BigUInt<LEN>::operator--(int) {
    BigUInt obj = *this;  // copy
    --*this;
    return obj;
}

template <unsigned LEN>
BigUInt<LEN>& BigUInt<LEN>::ToOpposite() {
    // self-modify
    return ++ToBitRev();
}

template <unsigned LEN>
BigUInt<LEN> BigUInt<LEN>::operator~() const {
    BigUInt obj = *this;  // copy
    obj.ToBitRev();
    return obj;
}

template <unsigned LEN>
BigUInt<LEN> BigUInt<LEN>::operator-() const {
    BigUInt obj = *this;  // copy
    obj.ToOpposite();
    return obj;
}

template <unsigned LEN>
BigUInt<LEN>& BigUInt<LEN>::operator&=(const BigUInt<LEN>& rhs) {
    for (unsigned i = 0; i < LEN; ++i) val_[i] &= rhs.val_[i];
    return *this;
}

template <unsigned LEN>
BigUInt<LEN> operator&(BigUInt<LEN> lhs, const BigUInt<LEN>& rhs) {
    return lhs &= rhs;
}

template <unsigned LEN>
BigUInt<LEN>& BigUInt<LEN>::operator|=(const BigUInt<LEN>& rhs) {
    for (unsigned i = 0; i < LEN; ++i) val_[i] |= rhs.val_[i];
    return *this;
}

template <unsigned LEN>
BigUInt<LEN> operator|(BigUInt<LEN> lhs, const BigUInt<LEN>& rhs) {
    return lhs |= rhs;
}

template <unsigned LEN>
BigUInt<LEN>& BigUInt<LEN>::operator^=(const BigUInt<LEN>& rhs) {
    for (unsigned i = 0; i < LEN; ++i) val_[i] ^= rhs.val_[i];
    return *this;
}

template <unsigned LEN>
BigUInt<LEN> operator^(BigUInt<LEN> lhs, const BigUInt<LEN>& rhs) {
    return lhs ^= rhs;
}

template <unsigned LEN>
BigUInt<LEN>& BigUInt<LEN>::operator<<=(int shift) {
    if (shift < 0) return *this >>= -shift;
    shift &= (LEN << 5) - 1;
    // shift = q*32 + r
    int q = shift >> 5, r = shift & 31;
    unsigned mask1 = unsigned(-1), mask2 = 0;
    if (r) {
        mask1 = (1 << (32 - r)) - 1;
        mask2 = unsigned(-1) - mask1;
        for (int i = LEN - 1; i > q; --i) {
            val_[i] = ((val_[i - q] & mask1) << r) +
                      ((val_[i - q - 1] & mask2) >> (32 - r));
        }
    } else {
        for (int i = LEN - 1; i > q; --i) val_[i] = val_[i - q];
    }
    val_[q] = (val_[0] & mask1) << r;
    // use std::fill instead of std::memset
    if (q) std::fill(val_, val_ + q, 0);
    return *this;
}

template <unsigned LEN>
BigUInt<LEN> operator<<(BigUInt<LEN> lhs, int shift) {
    return lhs <<= shift;
}

template <unsigned LEN>
BigUInt<LEN>& BigUInt<LEN>::operator>>=(int shift) {
    if (shift < 0) return *this <<= -shift;
    shift &= (LEN << 5) - 1;
    // shift = q*32 + r
    auto q = static_cast<unsigned>(shift >> 5),
         r = static_cast<unsigned>(shift & 31);
    unsigned mask2 = (1 << r) - 1;
    unsigned mask1 = unsigned(-1) - mask2;
    for (unsigned i = 0; i < LEN - q - 1; ++i) {
        val_[i] = ((val_[i + q] & mask1) >> r) +
                  ((val_[i + q + 1] & mask2) << (32 - r));
    }
    val_[LEN - q - 1] = (val_[LEN - 1] & mask1) << r;
    // use std::fill instead of std::memset
    if (q) std::fill(val_ + LEN - q, val_ + LEN, 0);
    return *this;
}

template <unsigned LEN>
BigUInt<LEN> operator>>(BigUInt<LEN> lhs, int shift) {
    return lhs >>= shift;
}

template <unsigned LEN>
void BigUInt<LEN>::PrintBinU(std::FILE* f) const {
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

template <unsigned LEN>
void BigUInt<LEN>::PrintHexU(std::FILE* f) const {
    int st = LEN;
    // template inheritance requires this for base member access
    while (st > 0 && !this->val_[--st])
        ;
    std::fprintf(f, "0x%x", this->val_[st]);
    for (int i = st - 1; i >= 0; --i) std::fprintf(f, "%08x", this->val_[i]);
}

template <unsigned LEN>
void BigUInt<LEN>::PrintHEXU(std::FILE* f) const {
    int st = LEN;
    // template inheritance requires this for base member access
    while (st > 0 && !this->val_[--st])
        ;
    std::fprintf(f, "0X%X", this->val_[st]);
    for (int i = st - 1; i >= 0; --i) std::fprintf(f, "%08X", this->val_[i]);
}

template <unsigned LEN>
template <unsigned _LEN>
int BigUInt<LEN>::Compare(const BigUInt<_LEN>& rhs) const {
    if (LEN > _LEN) {
        for (int i = LEN - 1; i >= _LEN; --i)
            if (val_[i] > 0) return 1;
        for (int i = _LEN - 1; i >= 0; --i)
            if (val_[i] != rhs.val_[i]) {
                if (val_[i] < rhs.val_[i])
                    return -1;
                else
                    return 1;
            }
    } else {
        for (int i = _LEN - 1; i >= LEN; --i)
            if (rhs.val_[i] > 0) return -1;
        for (int i = LEN - 1; i >= 0; --i)
            if (val_[i] != rhs.val_[i]) {
                if (val_[i] < rhs.val_[i])
                    return -1;
                else
                    return 1;
            }
    }
    return 0;
}

template <unsigned LEN1, unsigned LEN2>
bool operator<(const BigUInt<LEN1>& lhs, const BigUInt<LEN2>& rhs) {
    return lhs.Compare(rhs) < 0;
}
template <unsigned LEN1, unsigned LEN2>
bool operator>(const BigUInt<LEN1>& lhs, const BigUInt<LEN2>& rhs) {
    return lhs.Compare(rhs) > 0;
}
template <unsigned LEN1, unsigned LEN2>
bool operator<=(const BigUInt<LEN1>& lhs, const BigUInt<LEN2>& rhs) {
    return lhs.Compare(rhs) <= 0;
}
template <unsigned LEN1, unsigned LEN2>
bool operator>=(const BigUInt<LEN1>& lhs, const BigUInt<LEN2>& rhs) {
    return lhs.Compare(rhs) >= 0;
}
template <unsigned LEN1, unsigned LEN2>
bool operator==(const BigUInt<LEN1>& lhs, const BigUInt<LEN2>& rhs) {
    return lhs.Compare(rhs) == 0;
}
template <unsigned LEN1, unsigned LEN2>
bool operator!=(const BigUInt<LEN1>& lhs, const BigUInt<LEN2>& rhs) {
    return lhs.Compare(rhs) != 0;
}

template <unsigned LEN>
void BigUInt<LEN>::GenRandom(unsigned len) {
    for (unsigned i = 0; i < len; ++i) val_[i] = rand_(rand_gen_);
}

template <unsigned LEN>
void BigUInt<LEN>::Print(unsigned base, std::FILE* f) const {
    std::string str;
    unsigned r;
    char c;
    if (base < 2 || base > 32) base = 10;
    if (base == 16) {
        PrintHexU();
    } else if (base == 2) {
        PrintBinU();
    } else if (base == 10) {
        BigUInt<LEN> tmp = *this;
        if (!tmp) {
            std::fprintf(f, "0");
            return;
        }
        while (tmp) {
            tmp.DivEq_Basic(10, &r);
            str.insert(0, 1, '0' + static_cast<char>(r));
        }
        std::fprintf(f, "%s", str.c_str());
    } else {
        BigUInt<LEN> tmp = *this;
        if (!tmp) {
            std::fprintf(f, "0(base%d)", base);
            return;
        }
        while (tmp) {
            tmp.DivEq_Basic(base, &r);
            if (r < 10)
                c = '0' + static_cast<char>(r);
            else
                c = 'a' + static_cast<char>(r - 10);
            str.insert(0, 1, c);
        }
        std::fprintf(f, "%s(base%d)", str.c_str(), base);
    }
}

template <unsigned LEN>
unsigned BigUInt<LEN>::Mod_Basic(unsigned rhs) const {
    BigUInt<LEN> tmp = *this;
    unsigned r;
    tmp.DivEq_Basic(rhs, &r);
    return r;
}
// explicit instantiation
template class BigUInt<4>;
template class BigUInt<8>;
template class BigUInt<16>;
template class BigUInt<32>;
template class BigUInt<64>;
template class BigUInt<128>;
template class BigUInt<256>;
template BigUInt<4u>::operator BigUInt<8u>() const;
template BigUInt<8u>::operator BigUInt<16u>() const;
template BigUInt<16u>::operator BigUInt<32u>() const;
template BigUInt<32u>::operator BigUInt<64u>() const;
template BigUInt<64u>::operator BigUInt<128u>() const;
template BigUInt<128u>::operator BigUInt<256u>() const;
template BigUInt<256u>::operator BigUInt<128u>() const;
template BigUInt<128u>::operator BigUInt<64u>() const;
template BigUInt<64u>::operator BigUInt<32u>() const;
template BigUInt<32u>::operator BigUInt<16u>() const;
template BigUInt<16u>::operator BigUInt<8u>() const;
template BigUInt<8u>::operator BigUInt<4u>() const;
}  // namespace cryp
