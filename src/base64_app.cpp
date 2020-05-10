#include <iostream>
#include <vector>

#include "base64.hpp"
#include "half_text_print.hpp"
int main(int argc, char* argv[]) {
    cryp::Base64 base64;
    if (argc <= 1 || (argv[1][0] != 'e' && argv[1][0] != 'd')) {
        std::printf("Usage: %s [e]ncode/[d]ecode [charset]\n", argv[0]);
        std::printf("Encode: input from stdin, plain text string.\n");
        std::printf(
            "Decode: input from stdin, encoded string, stop at non-encoding "
            "character or pad.\n");
        std::printf("        Output will be plain-text string.\n");
        std::printf("charset: set the charset to be used.\n");
        std::printf("default charset: %s\n", base64.Charset());
        return 0;
    }
    if (argc >= 3 && base64.SetCharset(argv[2]))
        std::fputs("Error: invalid charset", stderr);
    std::vector<uint8_t> v;
    std::string s, t;
    if (argv[1][0] == 'e') {
        while (std::getline(std::cin, t)) s += t;
        std::cout << base64.Encode(reinterpret_cast<const uint8_t*>(s.data()),
                                   s.size())
                  << std::endl;
    } else {
        std::cin >> s;
        v = base64.Decode(s.data(), s.length());
        std::puts("Hexadecimal:");
        for (auto x : v) std::printf("%02hhx", x);
        std::puts("");
        std::puts("Text assuming ascii, space for non-printable characters:");
        cryp::HalfTextPrint(v);
        std::puts("");
    }
    return 0;
}
