#include <cassert>
#include <iostream>
#include <string>
#include "../src/bigint.hpp"
// using namespace std;

#ifdef __cpp_impl_three_way_comparison
#pragma message("feature: three-way comparison ok")
#endif
#if __cplusplus > 201703L  // c++2a=201707L
#pragma message("std version > c++17: ok")
#endif

int PutInfo(const std::string& str) {
    return std::printf("\033[1m\033[30mInfo\033[0m: %s\n", str.c_str());
}
int PutOK(const std::string& str) {
    return std::printf("\033[32mOK\033[0m: %s\n", str.c_str());
}
int PutWarning(const std::string& str) {
    return std::fprintf(stderr, "\033[1m\033[35mWarning\033[0m: %s\n",
                        str.c_str());
}
int PutError(const std::string& str) {
    return std::fprintf(stderr, "\033[1m\033[31mError\033[0m: %s\n",
                        str.c_str());
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    // get random ready
    std::random_device rand_dev_;
    auto rand_gen_ = static_cast<std::mt19937>(rand_dev_());
    std::uniform_int_distribution<unsigned> rand_;
    PutInfo("What are the outputs: Each test/section ENDS with an OK.");
    PutInfo("BigUInt tests begin...");
    cryp::uint128_t a;
    PutOK("default construct cryp::uint128_t, a=0");
    cryp::uint128_t b(10u);
    PutOK("construct by int literal, b=10");
    std::printf("a in hex is ");
    a.PrintHexU();
    std::printf("\na in bin is ");
    a.PrintBinU();
    std::printf("\nb in HEX is ");
    b.PrintHEXU();
    std::printf("\nb in bin is ");
    b.PrintBinU();
    std::printf("\n10<<63 in hex is ");
    (b <<= 31).PrintHexU();
    std::puts("");
    b = cryp::uint128_t(10u);
    PutOK("assigned b=10. Move assignment. Could be checked by gdb.");
    // b = cryp::int1024_t(10);
    // PutError("previous line should cause error -- length not match.");
    a = b;
    PutOK("assigned a=b. Copy assignment. Could be checked by gdb.");
    std::printf("Now a is ");
    a.PrintHexU();
    std::puts("");
    b = b;
    PutOK("assigned b=b. Copy assignment self-assignment check.");

    PutInfo("Jumping to multiplication...");
    a.GenRandom(2);
    // assert for bit shift
    // for (int i = 0; i < 32; ++i) assert(((a << i) >> i) == a);
    a.PrintHexU();
    std::printf(" * ");
    b.GenRandom(2);
    b.PrintHexU();
    std::printf(" gets ");
    a.MulEq_Plain(b);
    a.PrintHexU();
    std::printf("\n");
    PutOK("Plain mul tests finished. Verify through wolframalpha.com");

    PutInfo("Testing division big/int...");
    unsigned r;
    unsigned c = rand_(rand_gen_);
    cryp::uint128_t temp = a;
    a.PrintHexU();
    std::printf(" / %#x gets quotient = ", c);
    a.DivEq_Basic(c, &r);
    a.PrintHexU();
    std::printf(", remainder = %#x\n", r);

    std::printf("In decimal: ");
    temp.Print();
    std::printf(" / %u gets quotient = ", c);
    a.Print();
    std::printf(", remainder = %u\n", r);

    a.Print(16);
    std::printf(" == ");
    a.Print(10);
    std::printf("\n");
    cryp::uint256_t t1;
    cryp::uint512_t t2;
    // explicit type conversion
    t2 = static_cast<decltype(t2)>(t1);

    cryp::uint128_t rr;
    PutInfo("Testing division big/big (len: 4/2)...");
    a.GenRandom(4);
    b.GenRandom(2);
    a.PrintHexU();
    std::printf(" / ");
    b.PrintHexU();
    std::printf(" gets quotient ");
    a.DivEq_Plain(b, &rr);
    a.PrintHexU();
    std::printf(" and remainder ");
    rr.PrintHexU();
    std::puts("");
    PutOK("Plain div tests finished. Verify through wolframalpha.com");

    cryp::uint512_t a5, b5, r5;
    PutInfo("Testing division big/big (len: 16/6)...");
    a5.GenRandom(16);
    b5.GenRandom(6);
    a5.Print(16);
    std::printf(" - ");
    b5.Print(16);
    std::printf(" * ");
    a5.DivEq_Plain(b5, &r5);
    a5.Print(16);
    std::printf(" - ");
    r5.Print(16);
    std::puts(" should be 0");
    PutOK("Plain div tests finished. Verify on python");
    return 0;
}
