#ifndef FOO_DEF_H
#define FOO_DEF_H
// include headers (in which there are declarations), not implementation files
#include "verbose.h"

#define FOO_VERBOSE  // macros are not encouraged (use global constants instead)
#define FOO_DEBUG  // but I think controlling debug/release is ok in test phase

// don't pollute the global namespace
namespace foo {
class Foo {
   public:
    Foo(int data = 0) : data_(data) {
#ifdef FOO_VERBOSE
        Verb("foo_def.h", kCallFunc, "calling constructor of class Foo");
#endif
    }
    void Func();
    virtual ~Foo();

   private:
    int data_;
};
// maybe specify global variables with a `g_' ? I'm not sure.
extern const int global_variable;
}  // namespace foo
#endif /* ifndef FOO_DEF_H */
