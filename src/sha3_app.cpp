#include <cstring>
#include <iostream>

#include "sha3.hpp"
void PrintHelp([[maybe_unused]] int argc, char* argv[]) {
    std::fprintf(stderr,
                 "Usage: %s (sha3-(224|256|384|512)|shake(128|256)/[0-9]+)"
                 "[input_file] [known_hash]\n",
                 argv[0]);
    std::puts("    [0-9]+ specifies output length in shake(128|256).");
    std::puts(
        "    If no input_file is specified, the program will read from "
        "stdin.");
    std::puts(
        "    If known_hash specified, the program will compare the hash "
        "with that.");
}
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    uint8_t a[64];
    uint64_t b;
    FILE* f = stdin;
    if (argc <= 1) {
        PrintHelp(argc, argv);
        return 0;
    }
    if (argc > 2) {
        f = std::fopen(argv[2], "rb");
        if (!f) {
            std::perror("File opening failed");
            return EXIT_FAILURE;
        }
    }
    if (!std::strcmp(argv[1], "sha3-224")) {
        cryp::SHA3_224 sha3_224;
        b = sha3_224.Hash(f, a);
        std::printf("Message length in bits (mod 2^64): %llu (%llu bytes)\n", b,
                    b >> 3);
        std::printf("SHA3-224: ");
        for (uint8_t i = 0; i < 28; ++i) std::printf("%02hhx", a[i]);
        std::puts("");
        if (argc > 3) {
            uint8_t num = 0;
            for (uint8_t i = 0; i < 56; i += 2) {
                if (!std::isxdigit(argv[3][i]) ||
                    !std::isxdigit(argv[3][i + 1])) {
                    std::puts(
                        "The given SHA3-224 isn't long enough, can't compare.");
                    if (argc > 1) std::fclose(f);
                    return 0;
                }
                if (argv[3][i] <= '9')
                    num = uint8_t((argv[3][i] - '0') << 4);
                else
                    num = uint8_t((argv[3][i] - 'a' + 10) << 4);
                if (argv[3][i + 1] <= '9')
                    num += uint8_t(argv[3][i + 1] - '0');
                else
                    num += uint8_t(argv[3][i + 1] - 'a' + 10);
                if (a[i >> 1] != num) {
                    std::puts("SHA3-224 not equal!");
                    if (argc > 1) std::fclose(f);
                    return 0;
                }
            }
            std::puts("SHA3-224 ok.");
        }
    } else if (!std::strcmp(argv[1], "sha3-256")) {
        cryp::SHA3_256 sha3_256;
        b = sha3_256.Hash(f, a);
        std::printf("Message length in bits (mod 2^64): %llu (%llu bytes)\n", b,
                    b >> 3);
        std::printf("SHA3-256: ");
        for (uint8_t i = 0; i < 32; ++i) std::printf("%02hhx", a[i]);
        std::puts("");
        if (argc > 3) {
            uint8_t num = 0;
            for (uint8_t i = 0; i < 64; i += 2) {
                if (!std::isxdigit(argv[3][i]) ||
                    !std::isxdigit(argv[3][i + 1])) {
                    std::puts(
                        "The given SHA3-256 isn't long enough, can't compare.");
                    if (argc > 1) std::fclose(f);
                    return 0;
                }
                if (argv[3][i] <= '9')
                    num = uint8_t((argv[3][i] - '0') << 4);
                else
                    num = uint8_t((argv[3][i] - 'a' + 10) << 4);
                if (argv[3][i + 1] <= '9')
                    num += uint8_t(argv[3][i + 1] - '0');
                else
                    num += uint8_t(argv[3][i + 1] - 'a' + 10);
                if (a[i >> 1] != num) {
                    std::puts("SHA3-256 not equal!");
                    if (argc > 1) std::fclose(f);
                    return 0;
                }
            }
            std::puts("SHA3-256 ok.");
        }
    } else if (!std::strcmp(argv[1], "sha3-384")) {
        cryp::SHA3_384 sha3_384;
        b = sha3_384.Hash(f, a);
        std::printf("Message length in bits (mod 2^64): %llu (%llu bytes)\n", b,
                    b >> 3);
        std::printf("SHA3-384: ");
        for (uint8_t i = 0; i < 48; ++i) std::printf("%02hhx", a[i]);
        std::puts("");
        if (argc > 3) {
            uint8_t num = 0;
            for (uint8_t i = 0; i < 96; i += 2) {
                if (!std::isxdigit(argv[3][i]) ||
                    !std::isxdigit(argv[3][i + 1])) {
                    std::puts(
                        "The given SHA3-384 isn't long enough, can't compare.");
                    if (argc > 1) std::fclose(f);
                    return 0;
                }
                if (argv[3][i] <= '9')
                    num = uint8_t((argv[3][i] - '0') << 4);
                else
                    num = uint8_t((argv[3][i] - 'a' + 10) << 4);
                if (argv[3][i + 1] <= '9')
                    num += uint8_t(argv[3][i + 1] - '0');
                else
                    num += uint8_t(argv[3][i + 1] - 'a' + 10);
                if (a[i >> 1] != num) {
                    std::puts("SHA3-384 not equal!");
                    if (argc > 1) std::fclose(f);
                    return 0;
                }
            }
            std::puts("SHA3-384 ok.");
        }
    } else if (!std::strcmp(argv[1], "sha3-512")) {
        cryp::SHA3_512 sha3_512;
        b = sha3_512.Hash(f, a);
        std::printf("Message length in bits (mod 2^64): %llu (%llu bytes)\n", b,
                    b >> 3);
        std::printf("SHA3-512: ");
        for (auto x : a) std::printf("%02hhx", x);
        std::puts("");
        if (argc > 3) {
            uint8_t num = 0;
            for (uint8_t i = 0; i < 128; i += 2) {
                if (!std::isxdigit(argv[3][i]) ||
                    !std::isxdigit(argv[3][i + 1])) {
                    std::puts(
                        "The given SHA3-512 isn't long enough, can't compare.");
                    if (argc > 1) std::fclose(f);
                    return 0;
                }
                if (argv[3][i] <= '9')
                    num = uint8_t((argv[3][i] - '0') << 4);
                else
                    num = uint8_t((argv[3][i] - 'a' + 10) << 4);
                if (argv[3][i + 1] <= '9')
                    num += uint8_t(argv[3][i + 1] - '0');
                else
                    num += uint8_t(argv[3][i + 1] - 'a' + 10);
                if (a[i >> 1] != num) {
                    std::puts("SHA3-512 not equal!");
                    if (argc > 1) std::fclose(f);
                    return 0;
                }
            }
            std::puts("SHA3-512 ok.");
        }
    } else if (!std::strncmp(argv[1], "shake128/", 9) &&
               std::isdigit(argv[1][9])) {
        auto t = uint32_t(std::atoi(argv[1] + 9));
        auto hash_len = (t + 7) >> 3;
        auto va = new uint8_t[hash_len];
        if (t == 0) {
            std::fputs("Warning: t=0, nothing will be print.", stderr);
        } else if (t & 7) {
            std::fprintf(stderr, "Warning: t=%u not divisible by 8.", t);
            std::fputs("result will be trunucated to ceil(t/8) bytes.", stderr);
        }
        cryp::SHAKE128 shake128(t);
        b = shake128.Hash(f, va);
        std::printf("Message length in bits (mod 2^64): %llu (%llu bytes)\n", b,
                    b >> 3);
        std::printf("SHAKE128(d=%u): ", t);
        for (uint32_t i = 0; i < hash_len; ++i) std::printf("%02hhx", va[i]);
        std::puts("");
        if (argc > 3) {
            uint8_t num = 0;
            for (uint8_t i = 0; i < (hash_len << 1); i += 2) {
                if (!std::isxdigit(argv[3][i]) ||
                    !std::isxdigit(argv[3][i + 1])) {
                    std::printf(
                        "The given SHAKE128(d=%u) isn't long enough, can't "
                        "compare.\n",
                        t);
                    if (argc > 1) std::fclose(f);
                    return 0;
                }
                if (argv[3][i] <= '9')
                    num = uint8_t((argv[3][i] - '0') << 4);
                else
                    num = uint8_t((argv[3][i] - 'a' + 10) << 4);
                if (argv[3][i + 1] <= '9')
                    num += uint8_t(argv[3][i + 1] - '0');
                else
                    num += uint8_t(argv[3][i + 1] - 'a' + 10);
                if (va[i >> 1] != num) {
                    std::printf("SHAKE128(d=%u) not equal!\n", t);
                    if (argc > 1) std::fclose(f);
                    return 0;
                }
            }
            std::printf("SHAKE128(d=%u) ok.\n", t);
        }
        delete[] va;
    } else if (!std::strncmp(argv[1], "shake256/", 9) &&
               std::isdigit(argv[1][9])) {
        auto t = uint32_t(std::atoi(argv[1] + 9));
        auto hash_len = (t + 7) >> 3;
        auto va = new uint8_t[hash_len];
        if (t == 0) {
            std::fputs("Warning: t=0, nothing will be print.", stderr);
        } else if (t & 7) {
            std::fprintf(stderr, "Warning: t=%u not divisible by 8.", t);
            std::fputs("result will be trunucated to ceil(t/8) bytes.", stderr);
        }
        cryp::SHAKE256 shake256(t);
        b = shake256.Hash(f, va);
        std::printf("Message length in bits (mod 2^64): %llu (%llu bytes)\n", b,
                    b >> 3);
        std::printf("SHAKE256(d=%u): ", t);
        for (uint32_t i = 0; i < hash_len; ++i) std::printf("%02hhx", va[i]);
        std::puts("");
        if (argc > 3) {
            uint8_t num = 0;
            for (uint8_t i = 0; i < (hash_len << 1); i += 2) {
                if (!std::isxdigit(argv[3][i]) ||
                    !std::isxdigit(argv[3][i + 1])) {
                    std::printf(
                        "The given SHAKE256(d=%u) isn't long enough, can't "
                        "compare.\n",
                        t);
                    if (argc > 1) std::fclose(f);
                    return 0;
                }
                if (argv[3][i] <= '9')
                    num = uint8_t((argv[3][i] - '0') << 4);
                else
                    num = uint8_t((argv[3][i] - 'a' + 10) << 4);
                if (argv[3][i + 1] <= '9')
                    num += uint8_t(argv[3][i + 1] - '0');
                else
                    num += uint8_t(argv[3][i + 1] - 'a' + 10);
                if (va[i >> 1] != num) {
                    std::printf("SHAKE256(d=%u) not equal!\n", t);
                    if (argc > 1) std::fclose(f);
                    return 0;
                }
            }
            std::printf("SHAKE256(d=%u) ok.\n", t);
        }
        delete[] va;
    } else {
        std::fprintf(stderr, "Error: '%s' not recognized.\n", argv[1]);
        PrintHelp(argc, argv);
    }
    if (argc > 2) std::fclose(f);
    return 0;
}
