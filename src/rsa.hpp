#ifndef RSA_HPP
#define RSA_HPP
#include "bigint64.hpp"

namespace cryp {
using BI = calc::BigInt<calc::uint128_t>;
class RSA {
    BI p, q, m, d;
    BI n, e = BI(65537);
    BI dp, dq, qinv;

   public:
    const BI& GetN();
    const BI& GetE();
    void KeyGen(int bit_len = 1024);
};
}  // namespace cryp

#endif /* RSA_HPP */
