/*
 * This is a scratchpad for testing different C++ features...
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdlib>
#include <cassert>
#include <iostream>

void test_bool() {
    bool t1 = true;
    bool t2 = true;
    bool f1 = false;
    bool f2 = false;

    assert(t1 && t2 == true);
    assert(t1 && t2);
    assert(t1 || f1);
    assert(f1 || t1);
    assert(f1 || f2 == false);


}

void test_modulo() {
    assert((0 + 1) % 3 == 1);
    assert((1 + 1) % 3 == 2);
    assert((2 + 1) % 3 == 0);

    assert((0 + 3 - 1) % 3 == 2);
    assert((1 + 3 - 1) % 3 == 0);
    assert((2 + 3 - 1) % 3 == 1);
}

int main(int argc, char *argv[]) {

    test_bool();
    test_modulo();
    return EXIT_SUCCESS;
}


