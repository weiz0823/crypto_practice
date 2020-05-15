#include <iostream>
#include "../src/hexprint.hpp"
#include "../src/md5.hpp"
#include "../src/sha1.hpp"
#include "../src/sha2.hpp"
#include "../src/sha3.hpp"
int main() {
    std::puts("oid about hash functions:");
    std::cout << cryp::id_md5.GetName() << ": " << cryp::id_md5.GetID()
              << std::endl;
    std::cout << cryp::id_sha1.GetName() << ": " << cryp::id_sha1.GetID()
              << std::endl;
    std::cout << cryp::id_sha224.GetName() << ": " << cryp::id_sha224.GetID()
              << std::endl;
    std::cout << cryp::id_sha256.GetName() << ": " << cryp::id_sha256.GetID()
              << std::endl;
    std::cout << cryp::id_sha384.GetName() << ": " << cryp::id_sha384.GetID()
              << std::endl;
    std::cout << cryp::id_sha512.GetName() << ": " << cryp::id_sha512.GetID()
              << std::endl;
    std::cout << cryp::id_sha512_224.GetName() << ": "
              << cryp::id_sha512_224.GetID() << std::endl;
    std::cout << cryp::id_sha512_256.GetName() << ": "
              << cryp::id_sha512_256.GetID() << std::endl;
    std::cout << cryp::id_sha3_224.GetName() << ": "
              << cryp::id_sha3_224.GetID() << std::endl;
    std::cout << cryp::id_sha3_256.GetName() << ": "
              << cryp::id_sha3_256.GetID() << std::endl;
    std::cout << cryp::id_sha3_384.GetName() << ": "
              << cryp::id_sha3_384.GetID() << std::endl;
    std::cout << cryp::id_sha3_512.GetName() << ": "
              << cryp::id_sha3_512.GetID() << std::endl;
    std::cout << cryp::id_shake128.GetName() << ": "
              << cryp::id_shake128.GetID() << std::endl;
    std::cout << cryp::id_shake256.GetName() << ": "
              << cryp::id_shake256.GetID() << std::endl;
    return 0;
}
