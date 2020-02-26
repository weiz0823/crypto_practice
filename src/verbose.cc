#include "verbose.h"
#include <iostream>
#include <string>
#include "../dependencies/outputmod.h"

namespace foo {
int Verb(const char filename[], VerbTypes verbtype, const char words[]) {
    std::string typeword;
    switch (verbtype) {
        case kCallFunc:
            typeword = "Calling function:";
            break;
        default:
            typeword = "";
    }
    std::clog << outm::bold << outm::bright_magneta
              << "Verbose: " << outm::nocolor << outm::nobold << filename
              << ": " << typeword << ' ' << words << std::endl;
    return 0;
}
int VerbDebug(const char filename[], const char words[]) {
    // no std::endl, because there maybe more info followed, and user can decide
    // when to end line
    std::clog << outm::bold << outm::bright_magneta << "Debug info: " << outm::nocolor
              << outm::nobold << filename << ": " << words;
    return 0;
}
}  // namespace foo
