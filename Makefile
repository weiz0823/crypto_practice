# Just an empty makefile now.
# Remember to use TAB to indent.
MODE=debug
CXX=clang++-9

ifeq ($(CXX),g++)

STDVER=-std=c++17
WARNINGFLAGS=-Og -g -Wall -Weffc++ -pedantic \
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
INSTRUMENTFLAGS=-fsanitize=address \
				-fsanitize=leak -fsanitize=undefined \
				-fsanitize-address-use-after-scope \
				-fsanitize=float-divide-by-zero -fsanitize=bounds-strict \
				-fstack-protector-all -fstack-check

else

STDVER=-std=c++2a
WARNINGFLAGS=-O0 -g -Weverything -Wno-c++98-compat -Wno-missing-prototypes \
			 -Wno-c++98-compat-pedantic -Wno-weak-template-vtables \
			 -Wno-global-constructors -Wno-exit-time-destructors
INSTRUMENTFLAGS=-fsanitize=undefined  \
				-fsanitize=address

# -fsanitize=safe-stack
# -fsanitize=memory \

endif

CXXFLAGS=
RELEASE_TARGETS=
DEBUG_TARGETS=uint_basic_test


ifeq ($(MODE),debug)

CXXFLAGS=$(STDVER) $(WARNINGFLAGS) $(INSTRUMENTFLAGS) \
		 -Wno-unused-command-line-argument

else

DEBUG_TARGETS=
CXXFLAGS=$(STDVER) -O2 -Wno-unused-command-line-argument

endif

all: $(RELEASE_TARGETS) $(DEBUG_TARGETS)

compile/bigint.o: src/bigint.cpp src/bigint.hpp src/bigmul_div_mod.cpp
	$(CXX) $(CXXFLAGS) -c src/bigint.cpp -o compile/bigint.o

uint_basic_test: tests/uint_basic_test.cpp compile/bigint.o
	$(CXX) $(CXXFLAGS) -c tests/uint_basic_test.cpp -o compile/uint_basic_test.o
	$(CXX) $(CXXFLAGS) compile/uint_basic_test.o compile/bigint.o \
		-o uint_basic_test

.PHONY: all clean clean-all
clean:
	-rm compile/*

clean-all: clean
	-rm $(RELEASE_TARGETS) $(DEBUG_TARGETS)
