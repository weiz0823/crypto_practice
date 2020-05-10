#pragma once
#include <iostream>
#include "type_aliases.hpp"
namespace cryp {
// print binary data that is considered plain text
inline void HalfTextPrint(const ByteT* it_begin, const ByteT* it_end,
                          FILE* f = stdout) {
    for (auto it = it_begin; it < it_end; ++it) {
        if (*it == ' ' || std::isprint(*it))
            std::fputc(*it, f);
        else
            std::fputc(' ', f);
    }
}
inline void HalfTextPrint(const BytesT& vec, FILE* f = stdout) {
    HalfTextPrint(vec.begin().base(), vec.end().base(), f);
}
}  // namespace cryp
