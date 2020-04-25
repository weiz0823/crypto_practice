#!/usr/bin/env bash
# make md5 sha1 sha2 sha3 MODE=release || exit
echo -n "" >hash_func_tests.txt
echo -n 'text="'
cat hash_func_tests.txt
echo '"(no newline)'
./md5 hash_func_tests.txt
./sha1 hash_func_tests.txt | grep -v "length"
./sha2 sha224 hash_func_tests.txt | grep -v "length"
./sha2 sha256 hash_func_tests.txt | grep -v "length"
./sha2 sha384 hash_func_tests.txt | grep -v "length"
./sha2 sha512 hash_func_tests.txt | grep -v "length"
./sha2 "sha512/224" hash_func_tests.txt | grep -v "length"
./sha2 "sha512/256" hash_func_tests.txt | grep -v "length"
./sha3 sha3-224 hash_func_tests.txt | grep -v "length"
./sha3 sha3-256 hash_func_tests.txt | grep -v "length"
./sha3 sha3-384 hash_func_tests.txt | grep -v "length"
./sha3 sha3-512 hash_func_tests.txt | grep -v "length"
./sha3 "shake128/256" hash_func_tests.txt | grep -v "length"
./sha3 "shake256/512" hash_func_tests.txt | grep -v "length"

echo ""
echo -n "The quick brown fox jumps over the lazy dog" >hash_func_tests.txt
echo -n 'text="'
cat hash_func_tests.txt
echo '"(no newline)'
./md5 hash_func_tests.txt
./sha1 hash_func_tests.txt | grep -v "length"
./sha2 sha224 hash_func_tests.txt | grep -v "length"
./sha2 sha256 hash_func_tests.txt | grep -v "length"
./sha2 sha384 hash_func_tests.txt | grep -v "length"
./sha2 sha512 hash_func_tests.txt | grep -v "length"
./sha2 "sha512/224" hash_func_tests.txt | grep -v "length"
./sha2 "sha512/256" hash_func_tests.txt | grep -v "length"
./sha3 sha3-224 hash_func_tests.txt | grep -v "length"
./sha3 sha3-256 hash_func_tests.txt | grep -v "length"
./sha3 sha3-384 hash_func_tests.txt | grep -v "length"
./sha3 sha3-512 hash_func_tests.txt | grep -v "length"
./sha3 "shake128/256" hash_func_tests.txt | grep -v "length"
./sha3 "shake256/512" hash_func_tests.txt | grep -v "length"

echo ""
echo -n "The quick brown fox jumps over the lazy fog" >hash_func_tests.txt
echo -n 'text="'
cat hash_func_tests.txt
echo '"(no newline)'
./md5 hash_func_tests.txt
./sha1 hash_func_tests.txt | grep -v "length"
./sha2 sha224 hash_func_tests.txt | grep -v "length"
./sha2 sha256 hash_func_tests.txt | grep -v "length"
./sha2 sha384 hash_func_tests.txt | grep -v "length"
./sha2 sha512 hash_func_tests.txt | grep -v "length"
./sha2 "sha512/224" hash_func_tests.txt | grep -v "length"
./sha2 "sha512/256" hash_func_tests.txt | grep -v "length"
./sha3 sha3-224 hash_func_tests.txt | grep -v "length"
./sha3 sha3-256 hash_func_tests.txt | grep -v "length"
./sha3 sha3-384 hash_func_tests.txt | grep -v "length"
./sha3 sha3-512 hash_func_tests.txt | grep -v "length"
./sha3 "shake128/256" hash_func_tests.txt | grep -v "length"
./sha3 "shake256/512" hash_func_tests.txt | grep -v "length"

echo ""
echo -n "The quick brown fox jumps over the lazy dog." >hash_func_tests.txt
echo -n 'text="'
cat hash_func_tests.txt
echo '"(no newline)'
./md5 hash_func_tests.txt
./sha1 hash_func_tests.txt | grep -v "length"
./sha2 sha224 hash_func_tests.txt | grep -v "length"
./sha2 sha256 hash_func_tests.txt | grep -v "length"
./sha2 sha384 hash_func_tests.txt | grep -v "length"
./sha2 sha512 hash_func_tests.txt | grep -v "length"
./sha2 "sha512/224" hash_func_tests.txt | grep -v "length"
./sha2 "sha512/256" hash_func_tests.txt | grep -v "length"
./sha3 sha3-224 hash_func_tests.txt | grep -v "length"
./sha3 sha3-256 hash_func_tests.txt | grep -v "length"
./sha3 sha3-384 hash_func_tests.txt | grep -v "length"
./sha3 sha3-512 hash_func_tests.txt | grep -v "length"
./sha3 "shake128/256" hash_func_tests.txt | grep -v "length"
./sha3 "shake256/512" hash_func_tests.txt | grep -v "length"

rm hash_func_tests.txt
