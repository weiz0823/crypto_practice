#include <iostream>
#include <string>
#include "../src/bigint.hpp"
// using namespace std;

#ifdef __cpp_impl_three_way_comparison
#pragma message("feature: three-way comparison ok")
#endif
#if __cplusplus > 201703L // c++2a=201707L
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
    PutInfo("What are the outputs: Each test/section ENDS with an OK.");
    PutInfo("BigUInt tests begin...");
    cryp::int128_t a;
    PutOK("default construct cryp::uint128_t, a=0");
    cryp::int128_t b(10);
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
    b = cryp::int128_t(10);
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
    PutOK("Part 1 -- basic construct");
    return 0;
}
