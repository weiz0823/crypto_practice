#ifndef BIGINT_H
#define BIGINT_H
#include <cstring>
#include <iostream>
namespace cryp {
// NOTE: "fixed length" version
template <int LEN>
class BigInt {
   public:
    BigInt(long val = 0);
    // TODO
    BigInt(const char* str);
    BigInt(const BigInt& rhs);  // copy constructor
    BigInt(BigInt&& rhs);       // move constructor
    virtual ~BigInt();
    BigInt& operator=(const BigInt& rhs);  // copy assignment
    BigInt& operator=(BigInt&& rhs);       // move assignment
    operator bool() const;
    template <int _LEN>
    friend void swap(BigInt<_LEN>& lhs, BigInt<_LEN>& rhs);
    bool Sign() const;
    BigInt& ToBitRev();
    BigInt& operator+=(const BigInt& rhs);
    BigInt& operator-=(const BigInt& rhs);
    // TODO starts here
    // DON'T write +=1, for this is fixed length
    BigInt& operator++();  // ++i
    BigInt& operator--();
    BigInt operator++(int);  // i++
    BigInt operator--(int);
    BigInt& ToOpposite();
    BigInt operator~() const;
    BigInt operator-() const;
    // TODO: three-way comparison in c++20
    // TODO: bit arithmetic
    // TODO: write mul&div in another file
    // TODO: arbitrary base (2--32) I/O

   private:
    unsigned* val_;
};

template <int LEN>
void swap(BigInt<LEN>& lhs, BigInt<LEN>& rhs);
template <int LEN>
BigInt<LEN> operator+(BigInt<LEN> lhs, const BigInt<LEN>& rhs);
template <int LEN>
BigInt<LEN> operator-(BigInt<LEN> lhs, const BigInt<LEN>& rhs);

// explicit instantiation
template class BigInt<4>;
template class BigInt<8>;
template class BigInt<16>;
template class BigInt<32>;
// in bytes, 1024 to 8192 bits, an extension of builtin integers
typedef BigInt<4> int128_t;
typedef BigInt<8> int256_t;
typedef BigInt<16> int512_t;
typedef BigInt<32> int1024_t;

// unsigned to signed, just the output representation changes
// by number theory, it's just a case of representation in modular system
template <int LEN>
class BigUInt : BigInt<LEN> {
    template <int _LEN>
    friend std::ostream& operator<<(std::ostream& out, const BigInt<_LEN>& rhs);
};

// TODO
template <int LEN>
std::ostream& operator<<(std::ostream& out, const BigInt<LEN>& rhs);

// explicit instantiation
template class BigUInt<4>;
template class BigUInt<8>;
template class BigUInt<16>;
template class BigUInt<32>;
// in bytes, 1024 to 8192 bits, an extension of builtin integers
typedef BigUInt<4> uint128_t;
typedef BigUInt<8> uint256_t;
typedef BigUInt<16> uint512_t;
typedef BigUInt<32> uint1024_t;
}  // namespace cryp
#endif /* ifndef BIGINT_H */
