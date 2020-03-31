#include "rsa.hpp"
namespace cryp {
const BI& RSA::GetN() { return (n); }
const BI& RSA::GetE() { return (e); }
void RSA::KeyGen() {
    std::puts("Start RSA key generation...");
    std::puts("Begin looking for a prime p...");
    do {
        p.GenRandom(32);
        if (!p.Parity()) ++p;
    } while (!p.isProbablePrime());
    std::cout << "Got 512-bit prime p = " << p << std::endl;
    std::puts("Begin looking for a prime q...");
    do {
        q.GenRandom(32);
        if (!q.Parity()) ++q;
    } while (!q.isProbablePrime());
    std::cout << "Got 512-bit prime q = " << q << std::endl;
    n = p * q;
    std::cout << "n = " << n << std::endl;
    m = calc::GcdBin(--p, --q);
    m = p * q / m;
    std::cout << "Modular base m = " << m << std::endl;
    std::cout << "Public exponent e = " << e << std::endl;
    calc::ExtGcdBin<uint16_t>(e, m, &d, nullptr);
    if (d.Sign()) d += m;
    std::cout << "Calculated private exponent d = " << d << std::endl;
    ++p;
    ++q;
}
}  // namespace cryp
