#include "bigint64.hpp"
namespace calc {
BigInt<uint128_t>& BigInt<uint128_t>::operator+=(uint64_t rhs) {
    // terminal condition ensured by *end == 0
    uint128_t* it = val_ + 1;
    *val_ += rhs;
    // asm("movq %1, %0" : "=r"(it) : "m"(it));
bi128_pLEy_loop:
    asm goto(R"(
    adcq $0, %0
    adcq $0, 8%0
    leaq 16(%1), %1
    jc %l3
	jno %l4
	cmpq %2, %1
	jne %l4
)"
             :
             : "m"(*it), "r"(it), "r"(end_)
             : "cc", "memory"
             : bi128_pLEy_loop, bi128_pLEy_ret);
    *end_ = 0;
    SetLen(len_ + 1, false);
bi128_pLEy_ret:
    *end_ = 0;
    return *this;
}
BigInt<uint128_t>& BigInt<uint128_t>::operator++() { return *this += 1; }
BigInt<uint128_t> BigInt<uint128_t>::operator++(int) {
    auto obj = *this;
    *this += 1;
    return obj;
}
BigInt<uint128_t>& BigInt<uint128_t>::operator-=(uint64_t rhs) {
    *end_ = -1;  // ensure terminal condition for 0 - 1
    uint128_t* it = val_ + 1;
    *val_ -= rhs;
    // asm("movq %1, %0" : "=r"(it) : "m"(it));
bi128_mIEy_loop:
    asm goto(R"(
    sbbq $0, %0
    sbbq $0, 8%0
    leaq 16(%1), %1
    jc %l3
	jno %l4
	cmpq %2, %1
	jne %l4
)"
             :
             : "m"(*it), "r"(it), "r"(end_)
             : "cc", "memory"
             : bi128_mIEy_loop, bi128_mIEy_ret);
    *end_ = 0;  // back to default state
    // if signed overflow did occur on highest limb
    SetLen(len_ + 1, false);
    *(end_ - 1) = -1;
bi128_mIEy_ret:
    *end_ = 0;  // back to default state
    return *this;
}
BigInt<uint128_t>& BigInt<uint128_t>::operator--() { return *this -= 1; }
BigInt<uint128_t> BigInt<uint128_t>::operator--(int) {
    auto obj = *this;
    *this -= 1;
    return obj;
}
BigInt<uint128_t>& BigInt<uint128_t>::ToOpposite() {
    ToBitInv();
    return *this += 1;
}
BigInt<uint128_t>& BigInt<uint128_t>::ToAbsolute() {
    if (Sign())
        return ToOpposite();
    else
        return *this;
}
BigInt<uint128_t> BigInt<uint128_t>::operator-() const {
    auto obj = *this;
    return obj.ToOpposite();
}
BigInt<uint128_t>& BigInt<uint128_t>::operator+=(const BigInt& rhs) {
    SetLen(std::max(rhs.len_, len_) + 1, true);
    auto it = val_;
    auto cit = rhs.val_;
    if (rhs.Sign()) {
        *end_ = -1;
        *it += *cit;
        asm("lahf" : : : "ah");
        ++it;
        ++cit;
        do {
            asm(R"(
	movq (%1), %%r8
	movq 8(%1), %%r9
	sahf
	adcq %%r8, (%0)
	adcq %%r9, 8(%0)
	lahf
)"
                : "+r"(it), "+r"(cit)
                :
                : "cc", "memory", "r8", "r9", "rax");
            ++it;
            ++cit;
        } while (cit < rhs.end_);
        // implicit alignment
        asm("sahf" : : : "ah", "cc");
    bi128_pLER_loop11:
        asm goto(R"(
    adcq $-1, (%0)
    adcq $-1, 8(%0)
    leaq 16(%0), %0
    jnc %l1
)"
                 :
                 : "r"(it)
                 : "cc", "memory"
                 : bi128_pLER_loop11);
    } else {
        *it += *cit;
        asm("lahf" : : : "ah");
        ++it;
        ++cit;
        do {
            asm(R"(
	movq (%1), %%r8
	movq 8(%1), %%r9
	sahf
	adcq %%r8, (%0)
	adcq %%r9, 8(%0)
	lahf
)"
                : "+r"(it), "+r"(cit)
                :
                : "cc", "memory", "r8", "r9", "rax");
            ++it;
            ++cit;
        } while (cit < rhs.end_);
        // implicit alignment
        asm("sahf" : : : "ah", "cc");
    bi128_pLER_loop21:
        asm goto(R"(
    adcq $0, (%0)
    adcq $0, 8(%0)
    leaq 16(%0), %0
    jc %l1
)"
                 :
                 : "r"(it)
                 : "cc", "memory"
                 : bi128_pLER_loop21);
    }
    *end_ = 0;
    ShrinkLen();
    return *this;
}
BigInt<uint128_t>& BigInt<uint128_t>::operator-=(const BigInt& rhs) {
    SetLen(std::max(rhs.len_, len_) + 1, true);
    auto it = val_;
    auto cit = rhs.val_;
    if (rhs.Sign()) {
        *it -= *cit;
        asm("lahf" : : : "ah");
        ++it;
        ++cit;
        do {
            asm(R"(
	movq (%1), %%r8
	movq 8(%1), %%r9
	sahf
	sbbq %%r8, (%0)
	sbbq %%r9, 8(%0)
	lahf
)"
                : "+r"(it), "+r"(cit)
                :
                : "cc", "memory", "r8", "r9", "rax");
            ++it;
            ++cit;
        } while (cit < rhs.end_);
        // implicit alignment
        asm("sahf" : : : "ah", "cc");
    bi128_mIER_loop11:
        asm goto(R"(
    sbbq $-1, (%0)
    sbbq $-1, 8(%0)
    leaq 16(%0), %0
    jnc %l1
)"
                 :
                 : "r"(it)
                 : "cc", "memory"
                 : bi128_mIER_loop11);
    } else {
        *end_ = -1;
        *it -= *cit;
        asm("lahf" : : : "ah");
        ++it;
        ++cit;
        do {
            asm(R"(
	movq (%1), %%r8
	movq 8(%1), %%r9
	sahf
	sbbq %%r8, (%0)
	sbbq %%r9, 8(%0)
	lahf
)"
                : "+r"(it), "+r"(cit)
                :
                : "cc", "memory", "r8", "r9", "rax");
            ++it;
            ++cit;
        } while (cit < rhs.end_);
        // implicit alignment
        asm("sahf" : : : "ah", "cc");
    bi128_mIER_loop21:
        asm goto(R"(
    sbbq $0, (%0)
    sbbq $0, 8(%0)
    leaq 16(%0), %0
    jc %l1
)"
                 :
                 : "r"(it)
                 : "cc", "memory"
                 : bi128_mIER_loop21);
    }
    *end_ = 0;
    ShrinkLen();
    return *this;
}
BigInt<uint128_t>& BigInt<uint128_t>::BiasedAddEq(const BigInt& rhs,
                                                  uint64_t bias,
                                                  bool half_more) {
    // add as if rhs is left shifted by bias LIMBs
    bool sign = Sign();
    SetLen(std::max(rhs.len_ + bias, len_) + 1 + half_more, true);
    auto it = val_ + bias;
    if (half_more)
        it = reinterpret_cast<uint128_t*>(reinterpret_cast<uint64_t*>(it) + 1);
    auto cit = rhs.val_;
    if (rhs.Sign()) {
        *end_ = -1;
        *it += *cit;
        asm("lahf" : : : "ah");
        ++it;
        ++cit;
        do {
            asm(R"(
	movq (%1), %%r8
	movq 8(%1), %%r9
	sahf
	adcq %%r8, (%0)
	adcq %%r9, 8(%0)
	lahf
)"
                : "+r"(it), "+r"(cit)
                :
                : "cc", "memory", "r8", "r9", "rax");
            ++it;
            ++cit;
        } while (cit < rhs.end_);
        // implicit alignment
        asm("sahf" : : : "ah", "cc");
    bi128_pLBias_loop11:
        asm goto(R"(
    adcq $-1, (%0)
    adcq $-1, 8(%0)
    leaq 16(%0), %0
    jnc %l1
)"
                 :
                 : "r"(it)
                 : "cc", "memory"
                 : bi128_pLBias_loop11);
    } else {
        *it += *cit;
        asm("lahf" : : : "ah");
        ++it;
        ++cit;
        do {
            asm(R"(
	movq (%1), %%r8
	movq 8(%1), %%r9
	sahf
	adcq %%r8, (%0)
	adcq %%r9, 8(%0)
	lahf
)"
                : "+r"(it), "+r"(cit)
                :
                : "cc", "memory", "r8", "r9", "rax");
            ++it;
            ++cit;
        } while (cit < rhs.end_);
        // implicit alignment
        asm("sahf" : : : "ah", "cc");
    bi128_pLBias_loop21:
        asm goto(R"(
    adcq $0, (%0)
    adcq $0, 8(%0)
    leaq 16(%0), %0
    jc %l1
)"
                 :
                 : "r"(it)
                 : "cc", "memory"
                 : bi128_pLBias_loop21);
    }
    *end_ = 0;
    ShrinkLen();
    return *this;
}
BigInt<uint128_t>& BigInt<uint128_t>::BiasedSubEq(const BigInt& rhs,
                                                  uint64_t bias,
                                                  bool half_more) {
    // sub as if shifted
    bool sign = Sign();
    SetLen(std::max(rhs.len_ + bias, len_) + 1 + half_more, true);
    auto it = val_ + bias;
    if (half_more)
        it = reinterpret_cast<uint128_t*>(reinterpret_cast<uint64_t*>(it) + 1);
    auto cit = rhs.val_;
    if (rhs.Sign()) {
        *it -= *cit;
        asm("lahf" : : : "ah");
        ++it;
        ++cit;
        do {
            asm(R"(
	movq (%1), %%r8
	movq 8(%1), %%r9
	sahf
	sbbq %%r8, (%0)
	sbbq %%r9, 8(%0)
	lahf
)"
                : "+r"(it), "+r"(cit)
                :
                : "cc", "memory", "r8", "r9", "rax");
            ++it;
            ++cit;
        } while (cit < rhs.end_);
        // implicit alignment
        asm("sahf" : : : "ah", "cc");
    bi128_mIBias_loop11:
        asm goto(R"(
    sbbq $-1, (%0)
    sbbq $-1, 8(%0)
    leaq 16(%0), %0
    jnc %l1
)"
                 :
                 : "r"(it)
                 : "cc", "memory"
                 : bi128_mIBias_loop11);
    } else {
        *end_ = -1;
        *it -= *cit;
        asm("lahf" : : : "ah");
        ++it;
        ++cit;
        do {
            asm(R"(
	movq (%1), %%r8
	movq 8(%1), %%r9
	sahf
	sbbq %%r8, (%0)
	sbbq %%r9, 8(%0)
	lahf
)"
                : "+r"(it), "+r"(cit)
                :
                : "cc", "memory", "r8", "r9", "rax");
            ++it;
            ++cit;
        } while (cit < rhs.end_);
        // implicit alignment
        asm("sahf" : : : "ah", "cc");
    bi128_mIBias_loop21:
        asm goto(R"(
    sbbq $0, (%0)
    sbbq $0, 8(%0)
    leaq 16(%0), %0
    jc %l1
)"
                 :
                 : "r"(it)
                 : "cc", "memory"
                 : bi128_mIBias_loop21);
    }
    *end_ = 0;
    ShrinkLen();
    return *this;
}
}  // namespace calc
