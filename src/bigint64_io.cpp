#include <iomanip>

#include "bigint64.hpp"
namespace calc {
void BigInt<uint128_t>::Print(int base, int showbase, bool uppercase,
                              std::FILE* f) const {
    if (Sign()) {
        auto tmp_obj = -*this;
        std::fputc('-', f);
        tmp_obj.Print(base, showbase, uppercase, f);
        return;
    } else if (!*this) {
        std::fprintf(f, "0");
        return;
    }
    if (base < 2 || base > 36) base = 16;
    bool suffix_base = showbase == 2;
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
    if (base == 16) {
        auto it = end_ - 1;
        if (uppercase) {
            if (!*it) --it;
            if (*(reinterpret_cast<uint64_t*>(it) + 1)) {
                std::fprintf(f, "%llX%016llX",
                             *(reinterpret_cast<uint64_t*>(it) + 1),
                             *reinterpret_cast<uint64_t*>(it));
            } else {
                std::fprintf(f, "%llX", *reinterpret_cast<uint64_t*>(it));
            }
            while (it != val_) {
                --it;
                std::fprintf(f, "%016llX%016llX",
                             *(reinterpret_cast<uint64_t*>(it) + 1),
                             *reinterpret_cast<uint64_t*>(it));
            }
        } else {
            if (!*it) --it;
            if (*(reinterpret_cast<uint64_t*>(it) + 1)) {
                std::fprintf(f, "%llx%016llx",
                             *(reinterpret_cast<uint64_t*>(it) + 1),
                             *reinterpret_cast<uint64_t*>(it));
            } else {
                std::fprintf(f, "%llx", *reinterpret_cast<uint64_t*>(it));
            }
            while (it != val_) {
                --it;
                std::fprintf(f, "%016llx%016llx",
                             *(reinterpret_cast<uint64_t*>(it) + 1),
                             *reinterpret_cast<uint64_t*>(it));
            }
        }
    } else {
        std::string&& str = ToString(base, 0, uppercase);
        std::fwrite(str.data(), 1, str.length(), f);
    }
    if (suffix_base && base != 10) std::fprintf(f, "_%d", base);
}
std::ostream& operator<<(std::ostream& out, const BigInt<uint128_t>& rhs) {
    // std::fprintf(stdout, "high: %llx, %llx\n",
    // *(reinterpret_cast<uint64_t*>(rhs.end_) - 1),
    // *(reinterpret_cast<uint64_t*>(rhs.end_) - 2));
    if (rhs.Sign()) {
        out.put('-');
        return out << -rhs;
    } else if (!rhs) {
        return out.put('0');
    }
    bool showbase = out.flags() & out.showbase;
    if (out.flags() & out.hex) {
        auto it = rhs.end_ - 1;
        if (!*it) --it;
        if (*(reinterpret_cast<uint64_t*>(it) + 1)) {
            out << *(reinterpret_cast<uint64_t*>(it) + 1);
            out.unsetf(out.showbase);
            out << std::setfill('0') << std::setw(16)
                << *reinterpret_cast<uint64_t*>(it);
        } else {
            out << *reinterpret_cast<uint64_t*>(it);
            out.unsetf(out.showbase);
        }
        while (it != rhs.val_) {
            --it;
            out << std::setfill('0') << std::setw(16)
                << *(reinterpret_cast<uint64_t*>(it) + 1);
            out << std::setfill('0') << std::setw(16)
                << *reinterpret_cast<uint64_t*>(it);
        }
    } else {
        std::string&& str =
            rhs.ToString((out.flags() & out.oct) ? 8 : 10, showbase ? 1 : 0,
                         out.flags() & out.uppercase);
        out.write(str.data(), str.length());
    }
    if (showbase) out.setf(out.showbase);
    return out;
}
std::string BigInt<uint128_t>::ToString(int base, int showbase,
                                        bool uppercase) const {
    if (Sign()) {
        auto tmp_obj = -*this;
        return "-" + tmp_obj.ToString(base, showbase, uppercase);
    } else if (!*this) {
        return "0";
    }
    if (base < 2 || base > 36) base = 16;
    bool suffix_base = showbase == 2;
    std::string result = "", rev_str = "";
    if (showbase == 1) {
        switch (base) {
            case 2:
                if (uppercase)
                    result.append("0B", 2);
                else
                    result.append("0b", 2);
                break;
            case 8:
                result += '0';
                break;
            case 16:
                if (uppercase)
                    result.append("0X", 2);
                else
                    result.append("0x", 2);
                break;
            default:
                suffix_base = true;
                break;
        }
    }
    char charset[37] = "0123456789abcdefghijklmnopqrstuvwxyz";
    if (uppercase) std::strcpy(charset + 10, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    char buf[129];
    if (base == 16) {
        auto it = end_ - 1;
        if (uppercase) {
            if (!*it) --it;
            if (*(reinterpret_cast<uint64_t*>(it) + 1)) {
                std::sprintf(buf, "%llX%016llX",
                             *(reinterpret_cast<uint64_t*>(it) + 1),
                             *reinterpret_cast<uint64_t*>(it));
            } else {
                std::sprintf(buf, "%llX", *reinterpret_cast<uint64_t*>(it));
            }
            result += buf;
            while (it != val_) {
                --it;
                std::sprintf(buf, "%016llX%016llX",
                             *(reinterpret_cast<uint64_t*>(it) + 1),
                             *reinterpret_cast<uint64_t*>(it));
                result.append(buf, 32);
            }
        } else {
            if (!*it) --it;
            if (*(reinterpret_cast<uint64_t*>(it) + 1)) {
                std::sprintf(buf, "%llx%016llx",
                             *(reinterpret_cast<uint64_t*>(it) + 1),
                             *reinterpret_cast<uint64_t*>(it));
            } else {
                std::sprintf(buf, "%llx", *reinterpret_cast<uint64_t*>(it));
            }
            result += buf;
            while (it != val_) {
                --it;
                std::sprintf(buf, "%016llx%016llx",
                             *(reinterpret_cast<uint64_t*>(it) + 1),
                             *reinterpret_cast<uint64_t*>(it));
                result.append(buf, 32);
            }
        }
    } else if (base == 8) {
        auto tmp_obj = *this;
        uint128_t tmp, divbase = -1;
        divbase = divbase << 2 >> 2;
        do {
            tmp = *val_ & divbase;
            std::sprintf(buf, "%021llo%021llo", uint64_t(tmp),
                         uint64_t(tmp >> 63));
            rev_str.append(buf, 42);
            tmp_obj >>= 126;
        } while (tmp_obj);
        for (auto it = rev_str.end() - 42; it >= rev_str.begin(); it -= 42) {
            result.append(it, it + 42);
        }
        if (showbase == 1)
            result.erase(1, result.find_first_not_of('0', 1));
        else
            result.erase(0, result.find_first_not_of('0', 0));
    } else if (base == 10) {
        auto tmp_obj = *this;
        int64_t tmp, divbase = 1000000000;
        divbase *= divbase;  // 10^18
        do {
            tmp_obj.DivEq64(divbase, &tmp);
            std::sprintf(buf + 19, "%018lld", tmp);
            tmp_obj.DivEq64(divbase, &tmp);
            std::sprintf(buf, "%018lld", tmp);
            rev_str.append(buf, 18);
            rev_str.append(buf + 19, 18);
        } while (tmp_obj);
        for (auto it = rev_str.end() - 36; it >= rev_str.begin(); it -= 36) {
            result.append(it, it + 36);
        }
        result.erase(0, result.find_first_not_of('0', 0));
    } else if (base == 2) {
        auto it = end_ - 1;
        uint64_t tmp, mask = uint64_t(1) << 63;
        int i;
        if (!*it) --it;
        auto bufit = buf;
        if (*(reinterpret_cast<uint64_t*>(it) + 1)) {
            tmp = *(reinterpret_cast<uint64_t*>(it) + 1);
            for (i = 0; i < 64; ++i, mask >>= 1)
                if (mask & tmp) break;
            for (; i < 64; ++i, mask >>= 1)
                *(bufit++) = (mask & tmp) ? '1' : '0';
            tmp = *reinterpret_cast<uint64_t*>(it);
            mask = uint64_t(1) << 63;
            for (i = 0; i < 64; ++i, mask >>= 1)
                *(bufit++) = (mask & tmp) ? '1' : '0';
            *bufit = '\0';
        } else {
            tmp = *reinterpret_cast<uint64_t*>(it);
            for (i = 0; i < 64; ++i, mask >>= 1)
                if (mask & tmp) break;
            for (; i < 64; ++i, mask >>= 1)
                *(bufit++) = (mask & tmp) ? '1' : '0';
            *bufit = '\0';
        }
        result.append(buf, bufit - buf);
        buf[128] = '\0';
        while (it != val_) {
            --it;
            tmp = *(reinterpret_cast<uint64_t*>(it) + 1);
            for (bufit = buf + 63; bufit >= buf; --bufit, tmp >>= 1)
                *bufit = (tmp & 1) ? '1' : '0';
            tmp = *reinterpret_cast<uint64_t*>(it);
            for (bufit = buf + 127; bufit >= buf + 64; --bufit, tmp >>= 1)
                *bufit = (tmp & 1) ? '1' : '0';
            result.append(buf, 128);
        }
    } else {
        auto tmp_obj = *this;
        int64_t tmp, divbase = base * base * base;
        divbase *= divbase;
        divbase *= divbase;  // base^12
        std::fill(buf, buf + 24, '0');
        buf[24] = '\0';
        do {
            tmp_obj.DivEq64(divbase, &tmp);
            for (int i = 23; i >= 12; --i) {
                buf[i] = charset[tmp % base];
                tmp /= base;
            }
            tmp_obj.DivEq64(divbase, &tmp);
            for (int i = 11; i >= 0; --i) {
                buf[i] = charset[tmp % base];
                tmp /= base;
            }
            rev_str.append(buf, 24);
        } while (tmp_obj);
        for (auto it = rev_str.end() - 24; it >= rev_str.begin(); it -= 24) {
            result.append(it, it + 24);
        }
        result.erase(0, result.find_first_not_of('0', 0));
    }
    if (suffix_base && base != 10) {
        std::sprintf(buf, "_%d", base);
        result += buf;
    }
    return result;
}
BigInt<uint128_t>::BigInt(const std::string& str, size_t base)
    : BigInt(str.c_str(), base) {}
BigInt<uint128_t>::BigInt(const char* str, size_t base) : BigInt(0) {
    const uint64_t decode[75] = {
        0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  64, 64, 64, 64, 64,
        64, 64, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
        23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 64, 64,
        64, 64, 64, 64, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
    bool bNegative = false;
    uint64_t p = 0;
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
    uint64_t buf = 0, t = 0, basebound = base;
    int i = 0;
    if (base == 16) {
        while (std::isxdigit(str[p])) {
            buf = (buf << 4) | decode[int(str[p] - '0')];
            ++p;
            i += 4;
            if (i >= 64) {
                *this <<= 64;
                *this += buf;
                buf = 0;
                i = 0;
            }
        }
        if (i) {
            *this <<= i;
            *this += buf;
        }
    } else if (base == 8) {
        while (std::isdigit(str[p]) && (t = str[p] - '0') < base) {
            buf = (buf << 3) | t;
            ++p;
            i += 3;
            if (i >= 63) {
                *this <<= 63;
                *this += buf;
                buf = 0;
                i = 0;
            }
        }
        if (i) {
            *this <<= i;
            *this += buf;
        }
    } else if (base == 10) {
        basebound = 1000000000;
        basebound *= basebound;  // 10^18
        while (std::isdigit(str[p])) {
            buf = buf * base + (str[p] - '0');
            ++p;
            ++i;
            if (i >= 18) {
                *this *= basebound;
                *this += buf;
                buf = 0;
                i = 0;
            }
        }
        if (i) {
            basebound = base;
            while (--i) basebound *= base;
            *this *= basebound;
            *this += buf;
        }
    } else if (base == 2) {
        while (str[p] == '0' || str[p] == '1') {
            buf = (buf << 1) | (str[p] == '1');
            ++p;
            ++i;
            if (i >= 64) {
                *this <<= 64;
                *this += buf;
                buf = 0;
                i = 0;
            }
        }
        if (i) {
            *this <<= i;
            *this += buf;
        }
    } else {
        basebound = base * base * base;
        basebound *= basebound;
        basebound *= basebound;  // base^12
        while (std::isalnum(str[p]) && (t = decode[str[p] - '0']) < base) {
            buf = buf * base + t;
            ++p;
            ++i;
            if (i >= 12) {
                *this *= basebound;
                *this += buf;
                buf = 0;
                i = 0;
            }
        }
        if (i) {
            basebound = base;
            while (--i) basebound *= base;
            *this *= basebound;
            *this += buf;
        }
    }
    if (bNegative) ToOpposite();
}
std::istream& operator>>(std::istream& in, BigInt<uint128_t>& rhs) {
    std::string str;
    in >> str;
    if (in.flags() & in.hex) {
        rhs = BigInt<uint128_t>(str.c_str(), 16);
    } else if (in.flags() & in.oct) {
        rhs = BigInt<uint128_t>(str.c_str(), 8);
    } else {
        // auto-detect
        rhs = BigInt<uint128_t>(str.c_str(), 0);
    }
    return in;
}
}  // namespace calc
