#include <cassert>
#include <string>

#include "bigint.hpp"
namespace calc {
// constructors
// construct by \0 terminated c-style string, base=0 is auto-detect
template <typename IntT>
BigInt<IntT>::BigInt(const char* str, size_t base) : BigInt() {
    const IntT decode[75] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  64, 64, 64,
                             64, 64, 64, 64, 10, 11, 12, 13, 14, 15, 16, 17, 18,
                             19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
                             32, 33, 34, 35, 64, 64, 64, 64, 64, 64, 10, 11, 12,
                             13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
                             26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
    bool bNegative = false;
    size_t p = 0;
    if (str[p] == '-') {
        ++p;
        bNegative = true;
    } else if (str[p] == '+') {
        ++p;
    }
    if (str[p] == '0') {
        if ((base == 0 || base == 2) &&
            (str[p + 1] == 'B' || str[p + 1] == 'b')) {
            base = 2;
            p += 2;
        } else if ((base == 0 || base == 16) &&
                   (str[p + 1] == 'X' || str[p + 1] == 'x')) {
            base = 16;
            p += 2;
        } else if (base == 0 || base == 8) {
            base = 8;
            ++p;
        }
    }
    if (base > 36 || base < 2) base = 10;
    size_t tmp_base = base;
    size_t log_base = 0;
    while (!(tmp_base & 1)) {
        ++log_base;
        tmp_base >>= 1;
    }
    if (tmp_base == 1) {
        while (std::isalnum(str[p]) && decode[int(str[p] - '0')] < base) {
            *this <<= log_base;
            *this += decode[int(str[p++] - '0')];
        }
    } else if (base == 10) {
        while (std::isalnum(str[p]) && decode[int(str[p] - '0')] < base) {
            *this = (*this << 3) + (*this << 1);
            *this += decode[int(str[p++] - '0')];
        }
    } else {
        while (std::isalnum(str[p]) && decode[int(str[p] - '0')] < base) {
            *this *= static_cast<IntT>(base);
            *this += decode[int(str[p++] - '0')];
        }
    }
    if (bNegative) ToOpposite();
}

// assignment & type conversion operators
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::operator=(const char* str) {
    std::fill(val_, val_ + len_, IntT(0));
    ShrinkLen();
    const IntT decode[75] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  64, 64, 64,
                             64, 64, 64, 64, 10, 11, 12, 13, 14, 15, 16, 17, 18,
                             19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
                             32, 33, 34, 35, 64, 64, 64, 64, 64, 64, 10, 11, 12,
                             13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
                             26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
    bool bNegative = false;
    size_t p = 0;
    if (str[p] == '-') {
        ++p;
        bNegative = true;
    } else if (str[p] == '+') {
        ++p;
    }
    if (str[p] == '0') {
        if (str[p + 1] == 'B' || str[p + 1] == 'b') {
            p += 2;
            while (str[p] == '0' || str[p] == '1') {
                *this <<= 1;
                if (str[p] > '0') ++*this;
            }
        } else if (str[p + 1] == 'X' || str[p + 1] == 'x') {
            p += 2;
            while (std::isxdigit(str[p])) {
                *this <<= 1;
                *this += decode[int(str[p++] - '0')];
            }
        } else {
            ++p;
            while (std::isdigit(str[p]) && int(str[p] - '0') < 8) {
                *this <<= 3;
                *this += IntT(str[p++] - '0');
            }
        }
    } else {
        while (std::isdigit(str[p])) {
            *this = (*this << 3) + (*this << 1);
            *this += IntT(str[p++] - '0');
        }
    }
    if (bNegative) ToOpposite();
    return *this;
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::operator=(const std::string& str) {
    *this = str.data();
    return *this;
}
template <typename IntT>
BigInt<IntT>::operator std::string() const {
    return ToString(10, false, 0);
}
template <typename IntT>
std::string BigInt<IntT>::ToString(size_t base, bool uppercase,
                                   int showbase) const {
    bool sign = is_signed_ && Sign();
    if (sign) {
        BigInt<IntT> tmp_obj = -*this;
        return "-" + tmp_obj.ToString(base, uppercase, showbase);
    }
    const BigInt<IntT> zero(0);
    if (*this == zero) return std::string(1, '0');
    if (base < 2 || base > 36) base = 10;
    char charset[37] = "0123456789abcdefghijklmnopqrstuvwxyz";
    if (uppercase) std::strcpy(charset + 10, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    IntT mask = IntT(1);
    size_t i = 0;
    size_t tmp_base = base;
    size_t log_base = 0, offset;
    bool suffix_base = (showbase == 2);
    while (!(tmp_base & 1)) {
        ++log_base;
        tmp_base >>= 1;
    }
    IntT mask_init = IntT(((IntT(1) << log_base) - 1) << (LIMB - log_base));
    std::string str;
    str.reserve(LIMB * len_ / log_base + 5);
    if (showbase == 1) {
        switch (base) {
            case 2:
                if (uppercase)
                    str.append("0B", 2);
                else
                    str.append("0b", 2);
                break;
            case 8:
                str.append(1, '0');
                break;
            case 16:
                if (uppercase)
                    str.append("0X", 2);
                else
                    str.append("0x", 2);
                break;
            default:
                suffix_base = true;
                break;
        }
    }
    if (tmp_base == 1 && LIMB % log_base == 0) {
        offset = LIMB - log_base;
        mask = mask_init;
        while (mask && !(mask & val_[len_ - 1])) {
            mask >>= log_base;
            offset -= log_base;
        }
        for (i = len_ - 1; i != size_t(-1); --i) {
            while (mask) {
                str.append(1, charset[(mask & val_[i]) >> offset]);
                mask >>= log_base;
                offset -= log_base;
            }
            offset = LIMB - log_base;
            mask = mask_init;
        }
    } else if (tmp_base == 1) {
        size_t move_len = log_base - LIMB % log_base;
        size_t cur_move = len_ * LIMB % log_base;
        IntT mask_val = IntT((IntT(1) << log_base) - 1);
        offset = LIMB - cur_move;
        if (cur_move && (val_[len_ - 1] >> offset)) {
            str.append(1, charset[val_[len_ - 1] >> offset]);
            offset = LIMB - log_base - cur_move;
            mask = mask_init >> cur_move;
        } else {
            offset = LIMB - log_base - cur_move;
            mask = mask_init >> cur_move;
            while (mask && !(mask & val_[len_ - 1])) {
                mask >>= log_base;
                offset -= log_base;
            }
        }
        for (i = len_ - 1; i != size_t(-1); --i) {
            while (mask >= mask_val) {
                str.append(1, charset[(mask & val_[i]) >> offset]);
                mask >>= log_base;
                offset -= log_base;
            }
            cur_move = (cur_move + move_len) % log_base;
            offset = LIMB - cur_move;
            if (cur_move) {
                str.append(1, charset[(val_[i - 1] >> offset) |
                                      ((val_[i] & mask) << cur_move)]);
            }
            offset = LIMB - log_base - cur_move;
            mask = mask_init >> cur_move;
        }
    } else {
        uint32_t div_base = static_cast<uint32_t>(base);
        uint32_t t = uint32_t(31 / std::log2(base));
        for (i = 1; i < t; ++i) div_base *= base;
        uint32_t mod, tmp_mod;
        std::string rev_str;
        rev_str.reserve(LIMB * len_ / log_base);
        BigInt<uint32_t> tmp_obj(val_, len_);
        if (tmp_obj.Sign()) tmp_obj.SetLen(tmp_obj.len_ + 1, false);
        while (tmp_obj) {
            tmp_obj.BasicDivEq(div_base, &mod);
            for (i = 0; i < t; ++i) {
                tmp_mod = mod;
                mod /= base;
                rev_str.append(1, charset[tmp_mod - mod * base]);
            }
        }
        i = rev_str.length() - 1;
        while (rev_str[i] == '0') --i;
        for (; i != size_t(-1); --i) str.append(1, rev_str[i]);
    }
    if (suffix_base && base != 10) {
        str.append(1, '_');
        str.append(std::to_string(base));
    }
    return str;
}

// I/O
// currently accept 2<=base<=36, other value will be 10
template <typename IntT>
void BigInt<IntT>::Print(size_t base, bool uppercase, int showbase,
                         std::FILE* f) const {
    bool sign = is_signed_ && Sign();
    if (sign) {
        BigInt<IntT> tmp_obj = -*this;
        std::fputc('-', f);
        tmp_obj.Print(base, uppercase, showbase, f);
        return;
    }
    if (*this == BigInt<IntT>(0)) {
        std::fprintf(f, "0");
        return;
    }
    if (base < 2 || base > 36) base = 10;
    char charset[37] = "0123456789abcdefghijklmnopqrstuvwxyz";
    if (uppercase) std::strcpy(charset + 10, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    IntT mask = IntT(1);
    char limb_str[LIMB + 1];
    size_t i = 0, j = 0;
    size_t tmp_base = base;
    size_t log_base = 0, offset;
    bool suffix_base = (showbase == 2);
    while (!(tmp_base & 1)) {
        ++log_base;
        tmp_base >>= 1;
    }
    IntT mask_init = IntT(((IntT(1) << log_base) - 1) << (LIMB - log_base));
    if (showbase == 1) {
        switch (base) {
            case 2:
                if (uppercase)
                    std::fwrite("0B", 1, 2, f);
                else
                    std::fwrite("0b", 1, 2, f);
                break;
            case 8:
                std::fputc('0', f);
                break;
            case 16:
                if (uppercase)
                    std::fwrite("0X", 1, 2, f);
                else
                    std::fwrite("0x", 1, 2, f);
                break;
            default:
                suffix_base = true;
                break;
        }
    }
    if (tmp_base == 1 && LIMB % log_base == 0) {
        offset = LIMB - log_base;
        mask = mask_init;
        while (mask && !(mask & val_[len_ - 1])) {
            mask >>= log_base;
            offset -= log_base;
        }
        for (i = len_ - 1; i != size_t(-1); --i) {
            for (j = 0; mask; ++j) {
                limb_str[j] = charset[(mask & val_[i]) >> offset];
                mask >>= log_base;
                offset -= log_base;
            }
            std::fwrite(limb_str, 1, j, f);
            offset = LIMB - log_base;
            mask = mask_init;
        }
    } else if (tmp_base == 1) {
        size_t move_len = log_base - LIMB % log_base;
        size_t cur_move = len_ * LIMB % log_base;
        IntT mask_val = IntT((IntT(1) << log_base) - 1);
        offset = LIMB - cur_move;
        if (cur_move && (val_[len_ - 1] >> offset)) {
            limb_str[0] = charset[val_[len_ - 1] >> offset];
            offset = LIMB - log_base - cur_move;
            mask = mask_init >> cur_move;
            j = 1;
        } else {
            offset = LIMB - log_base - cur_move;
            mask = mask_init >> cur_move;
            while (mask && !(mask & val_[len_ - 1])) {
                mask >>= log_base;
                offset -= log_base;
            }
            j = 0;
        }
        for (i = len_ - 1; i != size_t(-1); --i) {
            for (; mask >= mask_val; ++j) {
                limb_str[j] = charset[(mask & val_[i]) >> offset];
                mask >>= log_base;
                offset -= log_base;
            }
            std::fwrite(limb_str, 1, j, f);
            cur_move = (cur_move + move_len) % log_base;
            offset = LIMB - cur_move;
            if (cur_move) {
                limb_str[0] = charset[(val_[i - 1] >> offset) |
                                      ((val_[i] & mask) << cur_move)];
                j = 1;
            } else {
                j = 0;
            }
            offset = LIMB - log_base - cur_move;
            mask = mask_init >> cur_move;
        }
    } else {
        std::string str = ToString(base, uppercase, 0);
        std::fwrite(str.data(), 1, str.size(), f);
    }
    if (suffix_base && base != 10) std::fprintf(f, "_%lu", base);
}
template <typename IntT>
std::istream& operator>>(std::istream& in, BigInt<IntT>& rhs) {
    std::string str;
    in >> str;
    if (in.flags() & in.hex) {
        BigInt<IntT> tmp_obj(str, 16);
        rhs = std::move(tmp_obj);
    } else if (in.flags() & in.oct) {
        BigInt<IntT> tmp_obj(str, 8);
        rhs = std::move(tmp_obj);
    } else {
        rhs = str;
    }
    return in;
}
template <typename IntT>
std::ostream& operator<<(std::ostream& out, const BigInt<IntT>& rhs) {
    bool sign = rhs.is_signed_ && rhs.Sign();
    if (sign) {
        out.put('-');
        BigInt<IntT> tmp_obj = -rhs;
        return out << tmp_obj;
    }
    if (rhs == BigInt<IntT>(0)) {
        out.write("0", 1);
        return out;
    }
    char charset[] = "0123456789abcdef";
    if (out.flags() & out.uppercase) std::strcpy(charset + 10, "ABCDEF");
    IntT mask = IntT(1);
    char limb_str[(sizeof(IntT) << 3) + 1];
    size_t i = 0, j = 0, offset;
    if (out.flags() & out.hex) {
        IntT mask_init = IntT(IntT(15) << (rhs.LIMB - 4));
        if (out.flags() & out.showbase) {
            if (out.flags() & out.uppercase)
                out.write("0X", 2);
            else
                out.write("0x", 2);
        }
        offset = rhs.LIMB - 4;
        mask = mask_init;
        while (mask && !(mask & rhs.val_[rhs.len_ - 1])) {
            mask >>= 4;
            offset -= 4;
        }
        for (i = rhs.len_ - 1; i != size_t(-1); --i) {
            for (j = 0; mask; ++j) {
                limb_str[j] = charset[(mask & rhs.val_[i]) >> offset];
                mask >>= 4;
                offset -= 4;
            }
            out.write(limb_str, static_cast<std::streamsize>(j));
            offset = rhs.LIMB - 4;
            mask = mask_init;
        }
    } else if (out.flags() & out.oct) {
        IntT mask_init = IntT(IntT(7) << (rhs.LIMB - 3));
        if (out.flags() & out.showbase) out.write("0", 1);
        size_t move_len = 3 - rhs.LIMB % 3;
        size_t cur_move = rhs.len_ * rhs.LIMB % 3;
        IntT mask_val = IntT((IntT(1) << 3) - 1);
        offset = rhs.LIMB - cur_move;
        if (cur_move && (rhs.val_[rhs.len_ - 1] >> offset)) {
            limb_str[0] = charset[rhs.val_[rhs.len_ - 1] >> offset];
            offset = rhs.LIMB - 3 - cur_move;
            mask = mask_init >> cur_move;
            j = 1;
        } else {
            offset = rhs.LIMB - 3 - cur_move;
            mask = mask_init >> cur_move;
            while (mask && !(mask & rhs.val_[rhs.len_ - 1])) {
                mask >>= 3;
                offset -= 3;
            }
            j = 0;
        }
        for (i = rhs.len_ - 1; i != size_t(-1); --i) {
            for (; mask >= mask_val; ++j) {
                limb_str[j] = charset[(mask & rhs.val_[i]) >> offset];
                mask >>= 3;
                offset -= 3;
            }
            out.write(limb_str, static_cast<std::streamsize>(j));
            cur_move = (cur_move + move_len) % 3;
            offset = rhs.LIMB - cur_move;
            if (cur_move) {
                limb_str[0] = charset[(rhs.val_[i - 1] >> offset) |
                                      ((rhs.val_[i] & mask) << cur_move)];
                j = 1;
            } else {
                j = 0;
            }
            offset = rhs.LIMB - 3 - cur_move;
            mask = mask_init >> cur_move;
        }
    } else {
        std::string str = rhs.ToString(10, false, 0);
        out.write(str.data(), static_cast<std::streamsize>(str.size()));
    }
    return out;
}
}  // namespace calc
