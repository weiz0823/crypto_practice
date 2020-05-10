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
DEBUG_TARGETS=rsa_test rsa_keygen_test
BENCHMARK_TARGETS=


ifeq ($(MODE),debug)

TARGETS=$(RELEASE_TARGETS) $(BENCHMARK_TARGETS) $(DEBUG_TARGETS)
CXXFLAGS+= $(WARNINGFLAGS) $(INSTRUMENTFLAGS)

else ifeq ($(MODE), benchmark)

TARGETS=$(RELEASE_TARGETS) $(BENCHMARK_TARGETS)
CXXFLAGS+= -O2

else

TARGETS=$(RELEASE_TARGETS)
CXXFLAGS+= -O2

endif

all: $(TARGETS)

compile/array_stream.o: src/array_stream.cpp src/array_stream.hpp
	$(CXX) $(CXXFLAGS) -c src/array_stream.cpp -o compile/array_stream.o

compile/serialize.o: src/serialize.cpp src/serialize.hpp
	$(CXX) $(CXXFLAGS) -c src/serialize.cpp -o compile/serialize.o

compile/hexprint.o: src/hexprint.cpp src/hexprint.hpp src/bin2text.hpp
	$(CXX) $(CXXFLAGS) -c src/hexprint.cpp -o compile/hexprint.o

compile/mgf1.o: src/mgf1.cpp src/mgf1.hpp src/mgf.hpp
	$(CXX) $(CXXFLAGS) -c src/mgf1.cpp -o compile/mgf1.o

compile/base64.o: src/base64.cpp src/base64.hpp src/bin2text.hpp
	$(CXX) $(CXXFLAGS) -c src/base64.cpp -o compile/base64.o

base64: src/base64_app.cpp compile/base64.o
	$(CXX) $(CXXFLAGS) -c src/base64_app.cpp -o compile/base64_app.o
	$(CXX) $(CXXFLAGS) compile/base64_app.o compile/base64.o \
		-o base64

compile/md5.o: src/md5.cpp src/md5.hpp src/hash.hpp
	$(CXX) $(CXXFLAGS) -c src/md5.cpp -o compile/md5.o

md5: src/md5_app.cpp compile/md5.o compile/array_stream.o
	$(CXX) $(CXXFLAGS) -c src/md5_app.cpp -o compile/md5_app.o
	$(CXX) $(CXXFLAGS) compile/md5_app.o compile/md5.o compile/array_stream.o \
		-o md5

compile/sha1.o: src/sha1.cpp src/sha1.hpp src/hash.hpp
	$(CXX) $(CXXFLAGS) -c src/sha1.cpp -o compile/sha1.o

sha1: src/sha1_app.cpp compile/sha1.o compile/array_stream.o
	$(CXX) $(CXXFLAGS) -c src/sha1_app.cpp -o compile/sha1_app.o
	$(CXX) $(CXXFLAGS) compile/sha1_app.o compile/sha1.o compile/array_stream.o \
		-o sha1

compile/sha2.o: src/sha2.cpp src/sha2.hpp src/hash.hpp
	$(CXX) $(CXXFLAGS) -c src/sha2.cpp -o compile/sha2.o

sha2: src/sha2_app.cpp compile/sha2.o compile/array_stream.o
	$(CXX) $(CXXFLAGS) -c src/sha2_app.cpp -o compile/sha2_app.o
	$(CXX) $(CXXFLAGS) compile/sha2_app.o compile/sha2.o compile/array_stream.o \
		-o sha2

compile/sha3.o: src/sha3.cpp src/sha3.hpp src/hash.hpp
	$(CXX) $(CXXFLAGS) -c src/sha3.cpp -o compile/sha3.o

sha3: src/sha3_app.cpp compile/sha3.o compile/array_stream.o
	$(CXX) $(CXXFLAGS) -c src/sha3_app.cpp -o compile/sha3_app.o
	$(CXX) $(CXXFLAGS) compile/sha3_app.o compile/sha3.o compile/array_stream.o \
		-o sha3

compile/randomart.o: src/randomart.cpp src/randomart.hpp
	$(CXX) $(CXXFLAGS) -c src/randomart.cpp -o compile/randomart.o

randomart: src/randomart_app.cpp compile/randomart.o
	$(CXX) $(CXXFLAGS) -c src/randomart_app.cpp -o compile/randomart_app.o
	$(CXX) $(CXXFLAGS) compile/randomart_app.o compile/randomart.o \
		-o randomart

compile/rsa.o: src/rsa.cpp src/rsa.hpp
	$(CXX) $(CXXFLAGS) -c src/rsa.cpp -o compile/rsa.o

compile/oaep.o: src/oaep.cpp src/oaep.hpp
	$(CXX) $(CXXFLAGS) -c src/oaep.cpp -o compile/oaep.o

compile/pkcs1_encode.o: src/pkcs1_encode.cpp src/pkcs1_encode.hpp
	$(CXX) $(CXXFLAGS) -c src/pkcs1_encode.cpp -o compile/pkcs1_encode.o

rsa_test: compile/rsa.o src/bigint64.a compile/hexprint.o compile/serialize.o \
	tests/rsa_test.cpp compile/oaep.o compile/pkcs1_encode.o \
	compile/sha1.o compile/mgf1.o compile/array_stream.o
	$(CXX) $(CXXFLAGS) -c tests/rsa_test.cpp -o compile/rsa_test.o
	$(CXX) $(CXXFLAGS) compile/rsa_test.o compile/rsa.o src/bigint64.a \
		compile/hexprint.o compile/serialize.o \
		compile/sha1.o compile/mgf1.o compile/array_stream.o \
		compile/oaep.o compile/pkcs1_encode.o -o rsa_test

rsa_keygen_test: compile/rsa.o src/bigint64.a tests/rsa_keygen_test.cpp \
	compile/base64.o compile/serialize.o compile/oaep.o compile/pkcs1_encode.o
	$(CXX) $(CXXFLAGS) -c tests/rsa_keygen_test.cpp -o compile/rsa_keygen_test.o
	$(CXX) $(CXXFLAGS) compile/rsa_keygen_test.o compile/rsa.o src/bigint64.a \
		compile/base64.o compile/serialize.o \
		compile/oaep.o compile/pkcs1_encode.o \
		-o rsa_keygen_test

print_oid: tests/print_oid.cpp compile/hexprint.o
	$(CXX) $(CXXFLAGS) -c tests/print_oid.cpp -o compile/print_oid.o
	$(CXX) $(CXXFLAGS) compile/print_oid.o compile/hexprint.o -o print_oid

.PHONY: all clean clean-all
clean:
	-rm compile/*

clean-all: clean
	-rm $(TARGETS)
