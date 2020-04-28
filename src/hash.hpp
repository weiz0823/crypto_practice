#pragma once
#include <cstring>
#include <iostream>

#include "array_stream.hpp"
#include "bitrotate.hpp"
// Cryptographic Secure Hash Functions
// Message and hash value always byte-aligned
class SecureHashFunc {
   protected:
    uint64_t hlen_;  // bitlen of hash value

   public:
    SecureHashFunc(uint64_t hashlen) : hlen_(hashlen) {}
    // hash without final padding, and don't get a final hash value
    // hash a (possibly large) file to an octet string (array of bytes)
    // return bitlen of hashed file
    virtual uint64_t HashUpdate(std::FILE* file) = 0;
    // hash array of bytes without padding
    virtual uint64_t HashUpdate(const uint8_t* src, uint64_t bytelen) = 0;
    // hash with final padding, output hash value, and reset to default state
    // okay to 'read' from nullptr
    virtual uint64_t HashFinal(uint8_t* dst) = 0;
    inline virtual uint64_t Hash(std::FILE* file, uint8_t* dst) {
        HashUpdate(file);
        return HashFinal(dst);
    }
    inline virtual uint64_t Hash(const uint8_t* src, uint64_t bytelen,
                                 uint8_t* dst) {
        HashUpdate(src, bytelen);
        return HashFinal(dst);
    }
    // hash length read-only
    inline uint64_t HashLen() const { return hlen_; }
    virtual ~SecureHashFunc() = default;
};
