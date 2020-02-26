#include <iostream>
#include "../src/foo.h"
using namespace std;  // ok ONLY in `client' files
int main(int argc, char* argv[]) {
    cout << "A test!" << endl;
	foo::Foo a(1);
	a.Func();
    return 0;
}
