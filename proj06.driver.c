/*******************************************************************************
 Bradley Bauer
 Project 6

 Note to TA: I've implemented rounding as described in the text book.
    If we are equally close to two binary numbers then we round to the number
    with the zero in the LSB.
    If we are closer to a whole binary number then we round to that number.

 Testing on random doubles isn't exhaustive ofcourse but it has helped me find
 lots of issues with my rounding code.

*******************************************************************************/

#include "project06.support.h"

#include <stdio.h>
#include <stdint.h> // uint64_t
#include <stdbool.h>
#include <stdlib.h> // rand
#include <math.h> // isnan
#include <float.h> // DBL_MIN

union ud_rep {
    uint64_t urep;
    double drep;
};

typedef struct test_case {
    int id;
    union ud_rep op1;
    union ud_rep op2;
    union ud_rep sum;
} test_case;

#define INIT_TCASE(ty1,op1,ty2,op2,ty3,op3) (test_case){tcase_counter++, {ty1=op1}, {ty2=op2}, {ty3=op3}}
// how many test cases have been created so far?
static uint32_t tcase_counter = 0;

// set or clear the ith bit from the right
#define setbit(x, i) (x |= (1ull << (i-1)))
#define clrbit(x, i) (x &= ~(1ull << (i-1)))

// pseudo random number gen for 64-bits https://stackoverflow.com/a/44836421
static uint64_t mcg64() {
    static uint64_t i = 1;
    return (i = (164603309694725029ull * i) % 14738995463583502973ull);
}

// return a randomly generated test_case
static test_case get_rand_tcase() {
    test_case ret;

    // shows that (has_lost_bits |= sum.significand & 1) is necessary
        // ret.op1.urep = mcg64();
        // ret.op2.urep = mcg64();

    // just a different way to init randomly
        // ret.op1.urep = ((uint64_t)rand()<<32) | rand();
        // ret.op2.urep = ((uint64_t)rand()<<32) | rand();

    // catches some cases where we sum to denormal
        // ret.op1.drep = 2.*drand48();
        // ret.op2.drep = 2.*drand48();

    // catches lots of rounding edge cases
        ret.op1.urep = mcg64();
        ret.op2.drep = ret.op1.drep * (rand() % 5000);

    // choose signs
    uint32_t rand_num = rand();
    if (rand_num % 5 == 0) {        // test pos + neg
        setbit(ret.op1.urep, 64);
        clrbit(ret.op2.urep, 64);
    } else if (rand_num % 3 == 0) {   // test neg + neg
        setbit(ret.op1.urep, 64);
        setbit(ret.op2.urep, 64);
    } else {                        // test pos + pos
        clrbit(ret.op1.urep, 64);
        clrbit(ret.op2.urep, 64);
    }

    // computer doesnt treat denormal inputs as zero so dont test them
    if (ret.op1.drep <= DBL_MIN) {
        ret.op1.urep |= 0x4050000000000000;
    }
    if (ret.op2.drep <= DBL_MIN) {
        ret.op2.urep |= 0x4030000000000000;
    }

    // computer doesnt handle nans how proj06 specifies so dont test them
    if (isnan(ret.op1.drep)) {
       clrbit(ret.op1.urep, 64);
       clrbit(ret.op1.urep, 63);
    }
    if (isnan(ret.op2.drep)) {
       clrbit(ret.op2.urep, 64);
       clrbit(ret.op2.urep, 63);
    }

    // get the expected result
    ret.sum.drep = ret.op1.drep + ret.op2.drep;

    // set the test case id
    tcase_counter += 1;
    ret.id = tcase_counter;

    return ret;
}

// pretty print a test case
static void print_test_case(test_case tcase, union ud_rep result) {
    printf("| %-8d | %16.5e | %016llx | %16.5e | %016llx | %16.5e | %016llx | %16.5e | %016llx |",
        tcase.id,
        tcase.op1.drep,
        tcase.op1.urep,
        tcase.op2.drep,
        tcase.op2.urep,
        tcase.sum.drep,
        tcase.sum.urep,
        result.drep,
        result.urep);
}

static const char pass_msg[] = " Passed! |\n";
static const char fail_msg[] = " Failed! |\n";
static const char table_divider[] = "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
static const char table_header1[] = "|    id    |     op1 dec      |     op1 hex      |     op2 dec      |     op2 hex      |  expect sum dec  |  expect sum hex  |  actual sum dec  |  actual sum hex  | Passed? |\n";

// test code for a given test case, if test fails, then print fail msg and exit
static void test(test_case tcase, bool should_print) {
    double sum = add(tcase.op1.drep, tcase.op2.drep);
    union ud_rep result = {.drep = sum};

    if (result.urep != tcase.sum.urep) {
        print_test_case(tcase, result);
        printf(fail_msg);
        printf("%s", table_divider);
        exit(-1);
    } else if (should_print) {
        print_test_case(tcase, result);
        printf(pass_msg);
    }
}

int main() {
    printf("%s", table_divider);
    printf("%s", table_header1);
    printf("%s", table_divider);

    //          operand 1                  operand 2                  expected sum
    test_case edge_cases[] = {
     INIT_TCASE(.drep, +INFINITY,          .urep, 0,                  .drep, +INFINITY),
     INIT_TCASE(.drep, -INFINITY,          .urep, 0,                  .drep, -INFINITY),
     INIT_TCASE(.drep, +NAN,               .urep, 0,                  .drep, +NAN),
     INIT_TCASE(.drep, -NAN,               .urep, 0,                  .drep, -NAN),
     INIT_TCASE(.drep, -NAN,               .drep, INFINITY,           .drep, -NAN),               // if both nan and inf, return nan
     INIT_TCASE(.drep, -1.,                .urep, 0,                  .drep, -1.),
     INIT_TCASE(.urep, 0x7ff0000f00000000, .drep, 1.,                 .urep, 0x7ff0000f00000000), // specific NAN returned
     INIT_TCASE(.urep, 0x0000000000000001, .urep, 0,                  .urep, 0),                  // denormal inputs treated as zero
     INIT_TCASE(.urep, 0x0008ffffffffffff, .urep, 0x000fffffffffffff, .urep, 0),                  // denormal inputs treated as zero
     INIT_TCASE(.urep, 0xffe0000000000000, .urep, 0xffe0000000000000, .drep, -INFINITY),          // normal sums to -inf
     INIT_TCASE(.drep, 1e308             , .drep, 1e308,              .drep, INFINITY),           // normal sums to inf
     INIT_TCASE(.urep, 0x7fe0000000000000, .urep, 0xffe0000000000000, .drep, 0.),                 // normal sums to 0
     INIT_TCASE(.urep, 0x0010000000000000, .drep, 1.,                 .drep, 1.),                 // rounding causes nop
     INIT_TCASE(.urep, 0x0010000000000000, .urep, 0x8018000000000000, .urep, 0x8008000000000000),
     INIT_TCASE(.urep, 0x7080000000000000, .urep, 0x6d30000000000000, .urep, 0x7080000000000000), // shifting op2 2 soo much it effectively computes sum = op1 + 0
     INIT_TCASE(.urep, 0x7080000000000000, .urep, 0x6d48000000000000, .urep, 0x7080000000000002),
     INIT_TCASE(.urep, 0x7080000000000000, .urep, 0xed38000000000000, .urep, 0x707ffffffffffffe),
     INIT_TCASE(.urep, 0x7080000000000000, .urep, 0xed30000000000000, .urep, 0x707fffffffffffff),
     INIT_TCASE(.urep, 0x7080000000000000, .urep, 0xed10000000000001, .urep, 0x7080000000000000),
     INIT_TCASE(.urep, 0x708c000000000000, .urep, 0x7086000000000000, .urep, 0x7099000000000000),
     INIT_TCASE(.urep, 0x708c000000000000, .urep, 0xf086000000000000, .urep, 0x7068000000000000),
     INIT_TCASE(.urep, 0x0020000000000000, .urep, 0x0020000000000001, .urep, 0x0030000000000000), // shifting deletes the trailing 1 but no round up because of the rounding rules
     INIT_TCASE(.urep, 0x0020000000000000, .urep, 0x0020000000000003, .urep, 0x0030000000000002), // shifting deletes the trailing 1 and we round up so that LSB of result is 0
     INIT_TCASE(.urep, 0x002fffffffffffff, .urep, 0x0020000000000000, .urep, 0x0038000000000000), // normalize twice
     INIT_TCASE(.drep, 1.1               , .drep, 2.1               , .drep, 1.1 + 2.1         ),
     INIT_TCASE(.drep, 3.141592          , .drep, 2.1               , .drep, 3.141592 + 2.1    ),
     INIT_TCASE(.drep, 1.010101010101    , .drep, 2.1               , .drep, 1.010101010101+2.1),
     INIT_TCASE(.drep, 320 + 331 + 299   , .drep, 4.0               , .drep, 320+331+299+4.0   ),
    };
    const uint32_t num_edge_cases = sizeof(edge_cases) / sizeof(edge_cases[0]);

    // Find what binary the computer would produce
    //union ud_rep x = {.urep = 0x0020000000000000};
    //union ud_rep y = {.urep = 0x0020000000000003};
    //union ud_rep z = {.drep = x.drep + y.drep};
    //printf("%016llx\n", z.urep);

    // Test edge cases
    for (uint32_t i = 0; i < num_edge_cases; ++i) {
        bool always_print = true;
        test(edge_cases[i], always_print);
    }

    // Test random doubles
    const uint32_t ntests = 21;
    // const uint32_t ntests = 1e9;
    for (uint32_t i = 0; i < ntests; ++i) {
        bool should_print = true;
        test(get_rand_tcase(), should_print);
    }

    printf(table_divider);
}

