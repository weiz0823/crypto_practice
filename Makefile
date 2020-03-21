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
			 -Wno-global-constructors -Wno-exit-time-destructors
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
RELEASE_TARGETS=base64 md5 sha1
DEBUG_TARGETS=uint_basic_test
BENCHMARK_TARGETS=bigmul_benchmark


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

compile/bigint.o: src/bigint.cpp src/bigint.hpp src/bigmul_div_mod.cpp
	$(CXX) $(CXXFLAGS) -c src/bigint.cpp -o compile/bigint.o

compile/base64.o: src/base64.cpp src/base64.hpp
	$(CXX) $(CXXFLAGS) -c src/base64.cpp -o compile/base64.o

base64: src/base64_app.cpp compile/base64.o
	$(CXX) $(CXXFLAGS) -c src/base64_app.cpp -o compile/base64_app.o
	$(CXX) $(CXXFLAGS) compile/base64_app.o compile/base64.o \
		-o base64

compile/md5.o: src/md5.cpp src/md5.hpp
	$(CXX) $(CXXFLAGS) -c src/md5.cpp -o compile/md5.o

md5: src/md5_app.cpp compile/md5.o
	$(CXX) $(CXXFLAGS) -c src/md5_app.cpp -o compile/md5_app.o
	$(CXX) $(CXXFLAGS) compile/md5_app.o compile/md5.o \
		-o md5

compile/sha1.o: src/sha1.cpp src/sha1.hpp
	$(CXX) $(CXXFLAGS) -c src/sha1.cpp -o compile/sha1.o

sha1: src/sha1_app.cpp compile/sha1.o
	$(CXX) $(CXXFLAGS) -c src/sha1_app.cpp -o compile/sha1_app.o
	$(CXX) $(CXXFLAGS) compile/sha1_app.o compile/sha1.o \
		-o sha1

uint_basic_test: tests/uint_basic_test.cpp compile/bigint.o
	$(CXX) $(CXXFLAGS) -c tests/uint_basic_test.cpp -o compile/uint_basic_test.o
	$(CXX) $(CXXFLAGS) compile/uint_basic_test.o compile/bigint.o \
		-o uint_basic_test

bigmul_benchmark: tests/bigmul_benchmark.cpp compile/bigint.o
	$(CXX) $(CXXFLAGS) -c tests/bigmul_benchmark.cpp -o compile/bigmul_benchmark.o
	$(CXX) $(CXXFLAGS) compile/bigmul_benchmark.o compile/bigint.o \
		-o bigmul_benchmark

.PHONY: all clean clean-all
clean:
	-rm compile/*

clean-all: clean
	-rm $(TARGETS)
