#include <iostream>
#include "sha1.hpp"
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    uint8_t a[20];
    uint64_t b;
    FILE* f = nullptr;
    if (argc > 1) {
        f = std::fopen(argv[1], "rb");
        if (!f) {
            std::perror("File opening failed");
            return EXIT_FAILURE;
        }
        b = cryp::SHA1::Calculate(f, a);
    } else {
        b = cryp::SHA1::Calculate(stdin, a);
    }

    std::printf("Message length in bits (mod 2^64): %lu (%lu bytes)\n", b,
                b >> 3);
    std::printf("SHA-1: ");
    for (auto x : a) std::printf("%02hhx", x);
    std::puts("");
    if (argc > 2) {
        uint8_t num = 0;
        for (uint8_t i = 0; i < 40; i += 2) {
            if (!std::isxdigit(argv[2][i]) || !std::isxdigit(argv[2][i + 1])) {
                std::puts("The given SHA-1 isn't long enough, can't compare.");
                return 0;
            }
            if (argv[2][i] <= '9')
                num = uint8_t((argv[2][i] - '0') << 4);
            else
                num = uint8_t((argv[2][i] - 'a' + 10) << 4);
            if (argv[2][i + 1] <= '9')
                num += uint8_t(argv[2][i + 1] - '0');
            else
                num += uint8_t(argv[2][i + 1] - 'a' + 10);
            if (a[i >> 1] != num) {
                std::puts("SHA-1 not equal!");
                return 0;
            }
        }
        std::puts("SHA-1 ok.");
    }
    if (argc > 1) std::fclose(f);
    return 0;
}
