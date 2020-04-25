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
RELEASE_TARGETS=base64 md5 sha1 sha2 sha3
DEBUG_TARGETS=rsa_test
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

compile/base64.o: src/base64.cpp src/base64.hpp
	$(CXX) $(CXXFLAGS) -c src/base64.cpp -o compile/base64.o

base64: src/base64_app.cpp compile/base64.o
	$(CXX) $(CXXFLAGS) -c src/base64_app.cpp -o compile/base64_app.o
	$(CXX) $(CXXFLAGS) compile/base64_app.o compile/base64.o \
		-o base64

compile/md5.o: src/md5.cpp src/md5.hpp
	$(CXX) $(CXXFLAGS) -c src/md5.cpp -o compile/md5.o

md5: src/md5_app.cpp compile/md5.o compile/array_stream.o
	$(CXX) $(CXXFLAGS) -c src/md5_app.cpp -o compile/md5_app.o
	$(CXX) $(CXXFLAGS) compile/md5_app.o compile/md5.o compile/array_stream.o \
		-o md5

compile/sha1.o: src/sha1.cpp src/sha1.hpp
	$(CXX) $(CXXFLAGS) -c src/sha1.cpp -o compile/sha1.o

sha1: src/sha1_app.cpp compile/sha1.o compile/array_stream.o
	$(CXX) $(CXXFLAGS) -c src/sha1_app.cpp -o compile/sha1_app.o
	$(CXX) $(CXXFLAGS) compile/sha1_app.o compile/sha1.o compile/array_stream.o \
		-o sha1

compile/sha2.o: src/sha2.cpp src/sha2.hpp
	$(CXX) $(CXXFLAGS) -c src/sha2.cpp -o compile/sha2.o

sha2: src/sha2_app.cpp compile/sha2.o compile/array_stream.o
	$(CXX) $(CXXFLAGS) -c src/sha2_app.cpp -o compile/sha2_app.o
	$(CXX) $(CXXFLAGS) compile/sha2_app.o compile/sha2.o compile/array_stream.o \
		-o sha2

compile/sha3.o: src/sha3.cpp src/sha3.hpp
	$(CXX) $(CXXFLAGS) -c src/sha3.cpp -o compile/sha3.o

sha3: src/sha3_app.cpp compile/sha3.o compile/array_stream.o
	$(CXX) $(CXXFLAGS) -c src/sha3_app.cpp -o compile/sha3_app.o
	$(CXX) $(CXXFLAGS) compile/sha3_app.o compile/sha3.o compile/array_stream.o \
		-o sha3

compile/rsa.o: src/rsa.cpp src/rsa.hpp
	$(CXX) $(CXXFLAGS) -c src/rsa.cpp -o compile/rsa.o

rsa_test: compile/rsa.o src/bigint64.a tests/rsa_test.cpp
	$(CXX) $(CXXFLAGS) -c tests/rsa_test.cpp -o compile/rsa_test.o
	$(CXX) $(CXXFLAGS) compile/rsa_test.o compile/rsa.o src/bigint64.a \
		-o rsa_test

.PHONY: all clean clean-all
clean:
	-rm compile/*

clean-all: clean
	-rm $(TARGETS)
