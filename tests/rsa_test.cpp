#include "../src/rsa.hpp"
#include <chrono>
#include <iostream>
#include "../src/half_text_print.hpp"
#include "../src/hexprint.hpp"
#include "../src/mgf1.hpp"
#include "../src/sha1.hpp"
using cryp::BytesT;
using cryp::ByteT;
using cryp::LenT;
int main() {
    LenT wrap_len = 80;
    cryp::SHA1 sha1;
    cryp::HexPrint hex_print;
    cryp::MGF1 mgf1(&sha1);
    cryp::RSAPubKey pub_key(&sha1, &mgf1);
    cryp::RSAPrvKey prv_key(&sha1, &mgf1);
    cryp::RSA::KeyGen(&pub_key, &prv_key, 1024, 0);
    std::puts("RSA Key generation ends.");
    std::string str;
    std::printf("Please input message: ");
    std::getline(std::cin, str);
    std::puts("Start testing RSAES-OAEP...");
    auto v1 = pub_key.OAEPEncrypt(reinterpret_cast<const ByteT*>(str.data()),
                                  str.size());
    std::puts("RSAES-OAEP encryption ends.");
    std::puts("Ciphertext (hexadecimal):");
    hex_print.Print(stdout, v1, wrap_len);
    auto v2 = prv_key.OAEPDecrypt(v1.data(), v1.size());
    std::puts("RSAES-OAEP decryption ends.");
    std::printf("Recovered message: ");
    cryp::HalfTextPrint(v2);
    std::puts("");
    std::puts("");
    std::puts("Start testing RSAES-PKCS1-v1_5...");
    v1 = pub_key.PKCS1Encrypt(reinterpret_cast<const ByteT*>(str.data()),
                              str.size());
    std::puts("RSAES-PKCS1-v1_5 encryption ends.");
    std::puts("Ciphertext (hexadecimal):");
    hex_print.Print(stdout, v1, wrap_len);
    v2 = prv_key.PKCS1Decrypt(v1.data(), v1.size());
    std::puts("RSAES-PKCS1-v1_5 decryption ends.");
    std::printf("Recovered message: ");
    cryp::HalfTextPrint(v2);
    std::puts("");
    return 0;
}
