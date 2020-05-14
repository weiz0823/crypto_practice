# MODE: debug/benchmark/release
MODE=debug
CXX=g++

ifeq ($(CXX),g++)

WARNINGFLAGS=-Wall -Weffc++ -pedantic \
			 -pedantic-errors -Wextra -Waggregate-return -Wcast-align \
			 -Wcast-qual -Wconversion \
			 -Wdisabled-optimization \
			 -Wfloat-equal -Wformat=2 \
			 -Wformat-nonliteral -Wformat-security \
			 -Wformat-y2k \
			 -Wimport -Winit-self -Winline \
			 -Winvalid-pch \
			 -Wlong-long \
			 -Wmissing-field-initializers -Wmissing-format-attribute \
			 -Wmissing-include-dirs -Wmissing-noreturn \
			 -Wpacked -Wpointer-arith \
			 -Wredundant-decls \
			 -Wshadow -Wstack-protector \
			 -Wstrict-aliasing=2 -Wswitch-default \
			 -Wswitch-enum \
			 -Wunreachable-code -Wunused \
			 -Wunused-parameter \
			 -Wvariadic-macros \
			 -Wwrite-strings \
			 -Wtautological-compare \
			 -Wno-unused-result \
			 -Wno-conversion
INSTRUMENTFLAGS=-Og -g -fsanitize=address \
				-fsanitize=leak -fsanitize=undefined \
				-fsanitize-address-use-after-scope \
				-fsanitize=float-divide-by-zero -fsanitize=bounds-strict \
				-fstack-protector-all -fstack-check

else

# assume it's clang
WARNINGFLAGS=-Weverything -Wno-c++98-compat -Wno-missing-prototypes \
			 -Wno-c++98-compat-pedantic -Wno-weak-template-vtables \
			 -Wno-global-constructors -Wno-exit-time-destructors \
			 -Wno-unused-command-line-argument
INSTRUMENTFLAGS=-O0 -g -fsanitize=undefined  \
				-fsanitize=address

# -fsanitize=safe-stack
# -fsanitize=memory \

endif

CXXFLAGS=-std=c++17
TARGETS=
RELEASE_TARGETS=base64 md5 sha1 sha2 sha3 randomart print_oid
LIB_TARGETS=array_stream.a hash.a mgf.a bin2text.a randomart.a pubkeycrypto.a \
			pubkey_encode.a
DEBUG_TARGETS=rsa_test rsa_keygen_test
BENCHMARK_TARGETS=


ifeq ($(MODE),debug)

TARGETS=$(DEBUG_TARGETS) $(BENCHMARK_TARGETS) $(LIB_TARGETS) $(RELEASE_TARGETS)
CXXFLAGS+= $(WARNINGFLAGS) $(INSTRUMENTFLAGS)

else ifeq ($(MODE), benchmark)

TARGETS=$(BENCHMARK_TARGETS)
CXXFLAGS+= -O2

else ifeq ($(MODE), lib)

TARGETS=$(LIB_TARGETS)
CXXFLAGS+= -O2

else

TARGETS=$(RELEASE_TARGETS)
CXXFLAGS+= -O2

endif

all: $(TARGETS)

# array stream supporting tool
compile/array_stream.o: src/array_stream.cpp src/array_stream.hpp
	$(CXX) $(CXXFLAGS) -c src/array_stream.cpp -o compile/array_stream.o

array_stream.a: compile/array_stream.o
	ar rcs array_stream.a $?

# hash functions
compile/md5.o: src/md5.cpp src/md5.hpp \
	src/hash.hpp src/array_stream.hpp src/bitrotate.hpp src/oid.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

md5: compile/md5_app.o compile/md5.o compile/array_stream.o
	$(CXX) $(CXXFLAGS) $^ -o $@

compile/sha1.o: src/sha1.cpp src/sha1.hpp \
	src/hash.hpp src/array_stream.hpp src/bitrotate.hpp src/oid.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

sha1: compile/sha1_app.o compile/sha1.o compile/array_stream.o
	$(CXX) $(CXXFLAGS) $^ -o $@

compile/sha2.o: src/sha2.cpp src/sha2.hpp \
	src/hash.hpp src/array_stream.hpp src/bitrotate.hpp src/oid.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

sha2: compile/sha2_app.o compile/sha2.o compile/array_stream.o
	$(CXX) $(CXXFLAGS) $^ -o $@

compile/sha3.o: src/sha3.cpp src/sha3.hpp \
	src/hash.hpp src/array_stream.hpp src/bitrotate.hpp src/oid.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

sha3: compile/sha3_app.o compile/sha3.o compile/array_stream.o
	$(CXX) $(CXXFLAGS) $^ -o $@

hash.a: compile/md5.o compile/sha1.o compile/sha2.o compile/sha3.o \
	compile/array_stream.o
	ar rcs $@ $?

# mask generation functions
compile/mgf1.o: src/mgf1.cpp src/mgf1.hpp \
	src/mgf.hpp src/type_aliases.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

mgf.a: compile/mgf1.o
	ar rcs $@ $?

# binary-to-text encoding
compile/hexprint.o: src/hexprint.cpp src/hexprint.hpp \
	src/bin2text.hpp src/type_aliases.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

compile/base64.o: src/base64.cpp src/base64.hpp \
	src/bin2text.hpp src/type_aliases.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

base64: src/base64_app.cpp compile/base64.o
	$(CXX) $(CXXFLAGS) $^ -o $@

bin2text.a: compile/base64.o compile/hexprint.o
	ar rcs $@ $?

# randomart key visualization
compile/randomart.o: src/randomart.cpp src/randomart.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

randomart: compile/randomart_app.o compile/randomart.o
	$(CXX) $(CXXFLAGS) $^ -o $@

randomart.a: compile/randomart.o
	ar rcs $@ $?

# data serialization
compile/serialize.o: src/serialize.cpp src/serialize.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# data encoding for public key cryptography
compile/oaep.o: src/oaep.cpp src/oaep.hpp \
	src/pubkey_msg_encode.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

compile/pkcs1_encode.o: src/pkcs1_encode.cpp src/pkcs1_encode.hpp \
	src/pubkey_msg_encode.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

compile/emsapss.o: src/emsapss.cpp src/emsapss.hpp \
	src/pubkey_sign_encode.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

pubkey_encode.a: compile/oaep.o compile/pkcs1_encode.o compile/emsapss.o
	ar rcs $@ $?

# public key cryptography
compile/rsa.o: src/rsa.cpp src/rsa.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

pubkeycrypto.a: compile/rsa.o
	ar rcs $@ $?

# tests
compile/rsa_test.o: tests/rsa_test.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

rsa_test: compile/rsa.o src/bigint64.a compile/hexprint.o compile/serialize.o \
	compile/rsa_test.o compile/oaep.o compile/pkcs1_encode.o compile/emsapss.o \
	compile/sha1.o compile/mgf1.o compile/array_stream.o
	$(CXX) $(CXXFLAGS) $^ -o $@

compile/rsa_keygen_test.o: tests/rsa_keygen_test.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

rsa_keygen_test: compile/rsa.o src/bigint64.a compile/rsa_keygen_test.o \
	compile/base64.o compile/serialize.o compile/oaep.o compile/pkcs1_encode.o
	$(CXX) $(CXXFLAGS) $^ -o $@

compile/print_oid.o: tests/print_oid.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

print_oid: compile/print_oid.o compile/hexprint.o hash.a
	$(CXX) $(CXXFLAGS) $^ -o $@

# wildcard match
compile/%_app.o: src/%_app.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# clean
.PHONY: all clean clean-all
clean:
	-rm compile/*

clean-all: clean
	-rm $(TARGETS)
