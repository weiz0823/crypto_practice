#ifndef FOO_VERBOSE_H
#define FOO_VERBOSE_H

namespace foo {
enum VerbTypes {
    kCallFunc,
};
// functions for verbose output & debug information
// encouraged for all libraries
int Verb(const char filename[], VerbTypes verbtype, const char words[]);
int VerbDebug(const char filename[], const char words[]);
}  // namespace foo
#endif /* ifndef FOO_VERBOSE_H */
