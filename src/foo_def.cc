#include "foo_def.h"
#include <iostream>
namespace foo {
const int global_variable = 1;
void Foo::Func() { ++data_; }
Foo::~Foo() {
#ifdef FOO_DEBUG
    VerbDebug("foo_def.cc", "The value of this->data_: ");
    std::cout << data_ << std::endl;
#endif
}
}  // namespace foo
