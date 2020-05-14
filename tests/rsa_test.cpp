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
void DoTest(const cryp::RSAPubKey& pub_key, const cryp::RSAPrvKey& prv_key,
            const ByteT* data, LenT len) {
    cryp::HexPrint hex_print;
    LenT wrap_len = 80;
    std::puts("Start testing RSAES-OAEP...");
    auto v1 = pub_key.OAEPEncrypt(data, len);
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
    v1 = pub_key.PKCS1Encrypt(data, len);
    std::puts("RSAES-PKCS1-v1_5 encryption ends.");
    std::puts("Ciphertext (hexadecimal):");
    hex_print.Print(stdout, v1, wrap_len);
    v2 = prv_key.PKCS1Decrypt(v1.data(), v1.size());
    std::puts("RSAES-PKCS1-v1_5 decryption ends.");
    std::printf("Recovered message: ");
    cryp::HalfTextPrint(v2);
    std::puts("");
    std::puts("");
    std::puts("Start testing RSASSA-PSS...");
    v1 = prv_key.PSSSign(data, len);
    std::puts("RSASSA-PSS signature ends.");
    std::puts("Signature (hexadecimal):");
    hex_print.Print(stdout, v1, wrap_len);
    if (pub_key.PSSVerify(data, len, v1.data(), v1.size())) {
        std::puts(
            "RSASSA-PSS verification ends."
            " Signature inconsistent with the message.");
    } else {
        std::puts("RSASSA-PSS verification ends. Signature OK.");
    }
}
int main() {
    std::string str;
    std::printf("Please input message: ");
    std::getline(std::cin, str);
    cryp::SHA1 sha1;
    cryp::MGF1 mgf1(&sha1);
    cryp::RSAPubKey pub_key(&sha1, &mgf1);
    cryp::RSAPrvKey prv_key(&sha1, &mgf1);
    LenT key_len = 1024;
    auto start_time = std::chrono::high_resolution_clock::now();
    cryp::RSA::KeyGen(&pub_key, &prv_key, key_len, 0);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto time_dur = end_time - start_time;
    std::printf("\nRSA-%llu Key generation ends. Costs %.3lfs.\n\n", key_len,
                time_dur.count() / 1e9);
    DoTest(pub_key, prv_key, reinterpret_cast<const ByteT*>(str.data()),
           str.size());

    key_len = 2048;
    start_time = std::chrono::high_resolution_clock::now();
    cryp::RSA::KeyGen(&pub_key, &prv_key, key_len, 0);
    end_time = std::chrono::high_resolution_clock::now();
    time_dur = end_time - start_time;
    std::printf("\nRSA-%llu Key generation ends. Costs %.3lfs.\n\n", key_len,
                time_dur.count() / 1e9);
    DoTest(pub_key, prv_key, reinterpret_cast<const ByteT*>(str.data()),
           str.size());

    key_len = 3072;
    start_time = std::chrono::high_resolution_clock::now();
    cryp::RSA::KeyGen(&pub_key, &prv_key, key_len, 0);
    end_time = std::chrono::high_resolution_clock::now();
    time_dur = end_time - start_time;
    std::printf("\nRSA-%llu Key generation ends. Costs %.3lfs.\n\n", key_len,
                time_dur.count() / 1e9);
    DoTest(pub_key, prv_key, reinterpret_cast<const ByteT*>(str.data()),
           str.size());
    return 0;
}
