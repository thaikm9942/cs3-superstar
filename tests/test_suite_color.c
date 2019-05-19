#include "../include/color.h"
#include "../include/test_util.h"
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

void test_make_color(){
    RGBColor c = (RGBColor) {0.1, 0.2, 0.3};
    assert(c.r == 0.1);
    assert(c.g == 0.2);
    assert(c.b == 0.3);
}

int main(int argc, char *argv[]) {
    // Run all tests? True if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests) {
        read_testname(argv[1], testname, sizeof(testname));
    }

    DO_TEST(test_make_color)

    puts("color_test PASS");

    return 0;
}
