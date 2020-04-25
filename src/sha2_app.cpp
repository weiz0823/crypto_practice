#include <cstring>
#include <iostream>

#include "sha2.hpp"
void PrintHelp([[maybe_unused]] int argc, char* argv[]) {
    std::fprintf(stderr,
                 "Usage: %s sha(224|256|384|512(/[0-9]+)?) "
                 "[input_file] [known_hash]\n",
                 argv[0]);
    std::puts("    [0-9]+ specifies t in SHA512/t.");
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
    if (!std::strcmp(argv[1], "sha224")) {
        cryp::SHA224 sha224;
        b = sha224.Hash(f, a);
        std::printf("Message length in bits (mod 2^64): %llu (%llu bytes)\n", b,
                    b >> 3);
        std::printf("SHA-224: ");
        for (uint8_t i = 0; i < 28; ++i) std::printf("%02hhx", a[i]);
        std::puts("");
        if (argc > 3) {
            uint8_t num = 0;
            for (uint8_t i = 0; i < 56; i += 2) {
                if (!std::isxdigit(argv[3][i]) ||
                    !std::isxdigit(argv[3][i + 1])) {
                    std::puts(
                        "The given SHA-224 isn't long enough, can't compare.");
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
                    std::puts("SHA-224 not equal!");
                    if (argc > 1) std::fclose(f);
                    return 0;
                }
            }
            std::puts("SHA-224 ok.");
        }
    } else if (!std::strcmp(argv[1], "sha256")) {
        cryp::SHA256 sha256;
        b = sha256.Hash(f, a);
        std::printf("Message length in bits (mod 2^64): %llu (%llu bytes)\n", b,
                    b >> 3);
        std::printf("SHA-256: ");
        for (uint8_t i = 0; i < 32; ++i) std::printf("%02hhx", a[i]);
        std::puts("");
        if (argc > 3) {
            uint8_t num = 0;
            for (uint8_t i = 0; i < 64; i += 2) {
                if (!std::isxdigit(argv[3][i]) ||
                    !std::isxdigit(argv[3][i + 1])) {
                    std::puts(
                        "The given SHA-256 isn't long enough, can't compare.");
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
                    std::puts("SHA-256 not equal!");
                    if (argc > 1) std::fclose(f);
                    return 0;
                }
            }
            std::puts("SHA-256 ok.");
        }
    } else if (!std::strcmp(argv[1], "sha384")) {
        cryp::SHA384 sha384;
        b = sha384.Hash(f, a);
        std::printf("Message length in bits (mod 2^64): %llu (%llu bytes)\n", b,
                    b >> 3);
        std::printf("SHA-384: ");
        for (uint8_t i = 0; i < 48; ++i) std::printf("%02hhx", a[i]);
        std::puts("");
        if (argc > 3) {
            uint8_t num = 0;
            for (uint8_t i = 0; i < 96; i += 2) {
                if (!std::isxdigit(argv[3][i]) ||
                    !std::isxdigit(argv[3][i + 1])) {
                    std::puts(
                        "The given SHA-384 isn't long enough, can't compare.");
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
                    std::puts("SHA-384 not equal!");
                    if (argc > 1) std::fclose(f);
                    return 0;
                }
            }
            std::puts("SHA-384 ok.");
        }
    } else if (!std::strcmp(argv[1], "sha512")) {
        cryp::SHA512 sha512;
        b = sha512.Hash(f, a);
        std::printf("Message length in bits (mod 2^64): %llu (%llu bytes)\n", b,
                    b >> 3);
        std::printf("SHA-512: ");
        for (auto x : a) std::printf("%02hhx", x);
        std::puts("");
        if (argc > 3) {
            uint8_t num = 0;
            for (uint8_t i = 0; i < 128; i += 2) {
                if (!std::isxdigit(argv[3][i]) ||
                    !std::isxdigit(argv[3][i + 1])) {
                    std::puts(
                        "The given SHA-512 isn't long enough, can't compare.");
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
                    std::puts("SHA-512 not equal!");
                    if (argc > 1) std::fclose(f);
                    return 0;
                }
            }
            std::puts("SHA-512 ok.");
        }
    } else if (!std::strncmp(argv[1], "sha512/", 7) &&
               std::isdigit(argv[1][7])) {
        auto t = uint32_t(std::atoi(argv[1] + 7));
        auto hash_len = (t + 7) >> 3;
        if (hash_len > 64) hash_len = 64;
        if (t == 384) {
            std::fputs("Warning: t=384 not allowed.", stderr);
        } else if (t == 0) {
            std::fputs("Warning: t=0, nothing will be print.", stderr);
        } else if (t > 512) {
            std::fprintf(stderr, "Warning: t>512 will get 512-bit hash,");
            std::fputs(" but it's a value different from SHA512/512.", stderr);
        } else if (t & 7) {
            std::fprintf(stderr, "Warning: t=%u not divisible by 8. ", t);
            std::fputs("result will be trunucated to ceil(t/8) bytes.", stderr);
            std::fputs(
                "    But the result will be different from SHA512/ceil(t/8)",
                stderr);
        }
        cryp::SHA512t sha512t(t);
        b = sha512t.Hash(f, a);
        std::printf("Message length in bits (mod 2^64): %llu (%llu bytes)\n", b,
                    b >> 3);
        std::printf("SHA-512/%u: ", t);
        for (uint8_t i = 0; i < hash_len; ++i) std::printf("%02hhx", a[i]);
        std::puts("");
        if (argc > 3) {
            uint8_t num = 0;
            for (uint8_t i = 0; i < (hash_len << 1); i += 2) {
                if (!std::isxdigit(argv[3][i]) ||
                    !std::isxdigit(argv[3][i + 1])) {
                    std::printf(
                        "The given SHA-512/%u isn't long enough, can't "
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
                if (a[i >> 1] != num) {
                    std::printf("SHA-512/%u not equal!\n", t);
                    if (argc > 1) std::fclose(f);
                    return 0;
                }
            }
            std::printf("SHA-512/%u ok.\n", t);
        }
    } else {
        std::fprintf(stderr, "Error: '%s' not recognized.\n", argv[1]);
        PrintHelp(argc, argv);
    }
    if (argc > 2) std::fclose(f);
    return 0;
}
