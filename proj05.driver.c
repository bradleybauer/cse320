/*******************************************************************************
 Bradley Bauer
 Project 5
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h> // rand
#include <ctype.h> // toupper
#include <string.h> // memcpy

#include "proj05.support.h"

#define MAX_STR_SZ 8
struct test_case {
    char external_rep[MAX_STR_SZ];
    int* internal_rep;
    int expected_internal_rep;
    int base;
    int has_null_internal_ptr;
    int has_bad_base;
    int should_succeed;
    int case_number;
};

/*------------------------------------------------------------------------------
 Name: get_testcase
 Purpose: Convert an integer into a test case
 Receive: An integer between 0 and ~85 million
 Return: A test case built from the integer and a bit of randomness
------------------------------------------------------------------------------*/
static struct test_case get_testcase(int n) {
    // Format of external_rep string
    // (' ' + '\t' + '\n' + '')^2 ('+' + '-' + '') (alnum + '')^3
    // that is
    // (2, 1, or 0 whitespaces) (+, -, or empty) (3, 2, 1, or 0 alpha numeric)
    // so "abc" is in the set of all possible external rep strings generated by
    // this function
    // so is all of ["+xyz", "", "0", "-", "\n\t+0"]

    // generate test case parameters from integer
    const int case_number = n;
    const int ws1 = n % 4; n /= 4;  // 1st choice of whitespace or empty
    const int ws2 = n % 4; n /= 4;  // 2nd choice of whitespace or empty
    const int sign = n % 3; n /= 3; // choice of sign character or empty
    const int d1 = n % 37; n /= 37; // 1st choice of alpha numeric or empty
    const int d2 = n % 37; n /= 37; // 2nd choice of alpha numeric or empty
    const int d3 = n % 37; n /= 37; // 3rd choice of alpha numeric or empty
    int base = n % 35 + 2; n /= 35; // choice of base

    #define NUM_DIGITS 3
    const int digits[NUM_DIGITS] = {d1, d2, d3};

    const int EMPTY_DIGIT = 36;
    const int EMPTY_CHAR = 0;

    char subject[MAX_STR_SZ] = ""; // inits to array of '\0'
    int i = 0;

    // make whitespace
    const char whitespace[3] = {' ', '\t', '\n'};
    if (ws1 != EMPTY_CHAR) {
        subject[i] = whitespace[ws1-1];
        i += 1;
    }
    if (ws2 != EMPTY_CHAR) {
        subject[i] = whitespace[ws2-1];
        i += 1;
    }

    // make sign
    const char sign_chars[2] = {'+', '-'};
    if (sign != EMPTY_CHAR) {
        subject[i] = sign_chars[sign - 1];
        i += 1;
    }

    // make digits
    for (int j = 0; j < NUM_DIGITS; ++j) {
        if (digits[j] < 10) {
            subject[i] = '0' + digits[j];
            i += 1;
        }
        else if (digits[j] != EMPTY_DIGIT) {
            subject[i] = 'a' + (digits[j] - 10);
            // occasionally use uppercase
            if (rand() % 13 < 2)
                subject[i] = toupper(subject[i]);
            i += 1;
        }
    }

    // conversion should fail if there are no valid base base digits that
    // immediately follow whitespace
    int should_succeed = 0;

    // compute expected result
    int expect = 0;
    for (int j = 0; j < NUM_DIGITS; ++j) {
        const int digit = digits[j];
        if (digit != EMPTY_DIGIT) {
            if (digit < base) {
                expect = expect * base + digit;
                // atleast one of [d1,d2,d3] are valid
                should_succeed = 1;
            }
            else {
                break;
            }
        }
    }
    if (sign == 2)
        expect *= -1;

    // Randomly test convert against null pointer inputs
    int has_null_internal_ptr = rand() % 13 == 1;
    int* internal = NULL;
    if (!has_null_internal_ptr) {
        internal = calloc(1, sizeof(int));
    }
    else {
        should_succeed = 0;
        expect = 0;
    }

    // Randomly test convert against invalid base input
    int has_bad_base = rand() % 13 == 1;
    if (has_bad_base) {
        base = 0;
        expect = 0;
        should_succeed = 0;
    }

    // package and return test case
    struct test_case ret;
    memcpy(ret.external_rep, subject, MAX_STR_SZ);
    ret.internal_rep = internal;
    ret.base = base;
    ret.expected_internal_rep = expect;
    ret.should_succeed = should_succeed;
    ret.has_null_internal_ptr = has_null_internal_ptr;
    ret.has_bad_base = has_bad_base;
    ret.case_number = case_number;

    return ret;
}

/*------------------------------------------------------------------------------
 Name: print_test_case
 Purpose: Print a test case as a row in a table of test cases
 Receive: Pointer to test case
 Return: None
------------------------------------------------------------------------------*/
static void print_test_case(const struct test_case* tcase) {
    printf("| %-9d", tcase->case_number);

    // print external rep
    int ext_rep_pretty_print_width = 0;
    const char* ext_rep = tcase->external_rep;
    char c = *ext_rep;
    printf("| \"");
    while (c != '\0') {
        // replace all tabs and newlines with "\\t" or "\\n" respectively
        if (c == '\t') {
            putchar('\\');
            putchar('t');
            ext_rep_pretty_print_width += 2;
        }
        else if (c == '\n') {
            putchar('\\');
            putchar('n');
            ext_rep_pretty_print_width += 2;
        }
        else {
            putchar(c);
            ext_rep_pretty_print_width += 1;
        }
        ext_rep++;
        c = *ext_rep;
    }
    putchar('\"');
    for (int i = 0; i < 12 - ext_rep_pretty_print_width - 3; ++i) {
        putchar(' ');
    }
    putchar('|');

    // print greatest valid digit
    if (!tcase->has_bad_base) {
        if (tcase->base <= 10)
            printf(" %-2d : %c |", tcase->base, ('0' + (tcase->base-1)));
        else
            printf(" %-2d : %c |", tcase->base, ('a' + (tcase->base-11)));
    }
    else {
        printf(" BAD    |");
    }

    // print other information
    printf("  %-10d|", tcase->expected_internal_rep);
    printf("  %d         |", tcase->has_null_internal_ptr);
    printf("  %d         |", tcase->should_succeed);
}

int main() {
    const char table_divider[] = "---------------------------------------------------------------------------------------\n";
    const char table_header1[] = "|  Case    |  External  |  Base  |  Expected  |  NULL Ptr  |  Expected  |  Passed     |\n";
    const char table_header2[] = "|  Number  |  Rep       |        |  Internal  |  Inputs    |  Return    |  Test Case  |\n";
    const char pass_msg[] = "  Passed!    |\n";
    const char fail_msg[] = "  Failed!    |\n";

    printf("Printing a subset of testing results:\n");
    printf(table_divider);
    printf(table_header1);
    printf(table_header2);
    printf(table_divider);

    for (int i = 0, print = 0; i < 1000000; ++i, ++print) {
        // choose a random test case
        static const int MAX_NUM_TESTCASES = 3 * 4 * 4 * 35 * 37 * 37 * 37;
        const int case_number = rand() % MAX_NUM_TESTCASES;
        const struct test_case tcase = get_testcase(case_number);

        // do the test
        const int success = convert(tcase.external_rep, tcase.base, tcase.internal_rep);

        // check if convert(..) catches errors properly
        if (success != tcase.should_succeed) {
            print_test_case(&tcase);
            printf(fail_msg);
            printf(table_divider);
            printf("Returned wrong status code: %d\n", success);
            return 0;
        }

        // check if convert(..) correctly converts to the internal rep
        if (tcase.should_succeed && tcase.expected_internal_rep != *tcase.internal_rep) {
            print_test_case(&tcase);
            printf(fail_msg);
            printf(table_divider);
            printf("Produced wrong internal rep: %d\n", *tcase.internal_rep);
            return 0;
        }

        // Print only a subset of all test cases tested
        if (print == 20000) {
            print_test_case(&tcase);
            printf(pass_msg);
            print = 0;
        }

        free(tcase.internal_rep);
    }

    printf(table_divider);
    printf("Passed 1 million test cases.\n");

    // no need to 'return 0' from main because main is special
}
