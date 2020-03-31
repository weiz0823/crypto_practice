#include "../src/rsa.hpp"

#include <iostream>
int main() {
    cryp::RSA r;
    r.KeyGen();
    return 0;
}
