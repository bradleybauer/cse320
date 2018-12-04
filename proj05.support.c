/*******************************************************************************
 Bradley Bauer
 Project 5
*******************************************************************************/

#include <stdlib.h> // NULL

#include "proj05.support.h"

static int is_digit_of_base(char c, int base) {
    if ((c >= 'a' && c < ('a' + (base - 10)))
     || (c >= 'A' && c < ('A' + (base - 10)))
     || (c >= '0' && c < ('0' + base)))
        return 1;
    return 0;
}

// Take an alphanumeric character and return its decimal value
static int get_digit_value(char c) {
    if (c <= '9')
        return c - '0';
    // convert to lowercase if uppercase or lowercase
    if (c >= 'A') // make sure we're not a digit
        c |= 0x60;
    return (c - 'a') + 10;
}

static int is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

static int is_sign(char c) {
    return c == '-' || c == '+';
}

int convert(const char external_rep[], int base, int* internal_rep) {
    // check for invalid inputs
    if (base < 2 || base > 36 || internal_rep == NULL) {
        return 0;
    }

    char chr;
    int cindex = 0;

    //// first substr
    // parse whitespace
    chr = external_rep[cindex];
    while (is_whitespace(chr) && chr != '\0') {
        cindex += 1;
        chr = external_rep[cindex];
    }

    // if there is no subject substr then fail
    if (chr == '\0') {
        return 0;
    }

    // parse optional sign
    int sign = 1;
    if (is_sign(chr)) {
        sign = (chr == '-') ? -1 : 1;
        cindex += 1;
        chr = external_rep[cindex];
    }

    //// subject substr
    // if there is no subject substr then fail
    if (!is_digit_of_base(chr, base)) {
        return 0;
    }

    // parse digits
    int twos_cmp = 0;
    while (is_digit_of_base(chr, base) && chr != '\0') {
        twos_cmp = twos_cmp * base + get_digit_value(chr);
        cindex += 1;
        chr = external_rep[cindex];
    }
    *internal_rep = sign * twos_cmp;

    return 1;
}

