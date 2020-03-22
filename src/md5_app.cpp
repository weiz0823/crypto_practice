#include <iostream>
#include "md5.hpp"
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    uint8_t a[16];
    uint64_t b;
    FILE* f = nullptr;
    if (argc > 1) {
        f = std::fopen(argv[1], "rb");
        if (!f) {
            std::perror("File opening failed");
            return EXIT_FAILURE;
        }
        b = cryp::MD5::Calculate(f, a);
    } else {
        b = cryp::MD5::Calculate(stdin, a);
    }

    std::printf("Message length in bits (mod 2^64): %lu (%lu bytes)\n", b,
                b >> 3);
    std::printf("MD5: ");
    for (auto x : a) std::printf("%02hhx", x);
    std::puts("");
    if (argc > 2) {
        uint8_t num = 0;
        for (uint8_t i = 0; i < 32; i += 2) {
            if (!std::isxdigit(argv[2][i]) || !std::isxdigit(argv[2][i + 1])) {
                std::puts("The given MD5 isn't long enough, can't compare.");
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
                std::puts("MD5 not equal!");
                return 0;
            }
        }
        std::puts("MD5 ok.");
    }
    if (argc > 1) std::fclose(f);
    return 0;
}
