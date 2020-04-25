#include <iostream>
#include <vector>

#include "../src/base64.hpp"
int main(int argc, char* argv[]) {
    if (argc <= 1 || (argv[1][0] != 'e' && argv[1][0] != 'd')) {
        std::printf("Usage: %s [e]ncode/[d]ecode [charset]\n", argv[0]);
        std::printf("Encode: input from stdin, plain text string.\n");
        std::printf(
            "Decode: input from stdin, encoded string, stop at non-encoding "
            "character or pad.\n");
        std::printf("        Output will be plain-text string.\n");
        std::printf("charset: set the charset to be used.\n");
        std::printf("default charset: %s\n", cryp::Base64::Charset());
        return 0;
    }
    if (argc >= 3 && cryp::Base64::SetCharset(argv[2]))
        std::fputs("Error: invalid charset", stderr);
    std::vector<uint8_t> v;
    std::string s, t;
    if (argv[1][0] == 'e') {
        while (std::getline(std::cin, t)) s += t;
        std::cout << cryp::Base64::Encode(
                         reinterpret_cast<const uint8_t*>(s.data()), s.size())
                  << std::endl;
    } else {
        std::cin >> s;
        v = cryp::Base64::Decode(s.data(), s.length());
        for (auto x : v) std::printf("%02hhx", x);
        std::puts("");
    }
    return 0;
}
