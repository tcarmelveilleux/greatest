#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <sys/time.h>
#include <err.h>

#include "greatest.h"

#define TEST_COUNT 1000

static char test_has_run[(TEST_COUNT / 8) + 1];

static int check_run(unsigned int id) {
    size_t offset = id / 8;
    char bit = 1U << (id & 0x07);
    return 0 != (test_has_run[offset] & bit);
}

static void set_run(unsigned int id) {
    size_t offset = id / 8;
    char bit = 1U << (id & 0x07);
    test_has_run[offset] |= bit;
}


TEST print_and_pass(unsigned int id) {
    if (0) {
        printf("running test %u\n", id);
    }
    if (check_run(id)) {
        fprintf(stderr, "Error: Shuffling made test run multiple times!\n");
        assert(0);
    } else {
        set_run(id);
    }
    PASS();
}

SUITE(suite) {
    struct timeval tv;
    long seed;
    const size_t limit = TEST_COUNT;
    unsigned int i = 0;
    if (0 != gettimeofday(&tv, NULL)) {
        err(1, "gettimeofday");
    }
    seed = tv.tv_sec ^ tv.tv_usec;

    SHUFFLE_TESTS(seed, {
        for (i = 0; i < limit; i++) {
            RUN_TEST1(print_and_pass, i);
        }
    });

    for (i = 0; i < limit; i++) {
        if (!check_run(i)) {
            fprintf(stderr, "Error: Test %u got lost in the shuffle!\n", i);
            assert(0);
        }
    }
}

/* Add all the definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();      /* command-line arguments, initialization. */

    RUN_SUITE(suite);

    GREATEST_MAIN_END();        /* display results */
}
