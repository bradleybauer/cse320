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
// Turn off assert
#define NDEBUG

#include <stdint.h> // uint64_t
#include <stdbool.h>
#include <assert.h>

#include "project06.support.h"

#define EXP_MASK 0x7ff0000000000000ull
#define FRAC_MASK 0x000fffffffffffffull
#define NUM_ROUND_BITS 2
#define ROUND_BITS_MASK ((1ull << NUM_ROUND_BITS) - 1)

// get/set the ith bit from the right
// LSB is the 1st bit from the right
// MSB is the 64th bit from the right
#define getbit(x, i) (x & (1ull << (i-1)))
#define setbit(x, i) (x |= (1ull << (i-1)))
#define nlowerbits(x, n) (x & ((1ull << (n)) - 1))

// get the twos place bit
#define getoverflow(x) getbit(x, 54 + NUM_ROUND_BITS)
// get the ones place bit
#define gethidden(x) getbit(x, 53 + NUM_ROUND_BITS)
#define sethidden(x) setbit(x, 53 + NUM_ROUND_BITS)

#define INF_EXP 0x7ffu

union ud_rep {
    uint64_t urep;
    double drep;
};

typedef struct pack {
    bool sign;
    uint32_t biased_exp;
    uint64_t fraction;
} pack;

// convert a pack into a double
static double repack(pack num) {
    union ud_rep ud = {.urep = ((uint64_t)num.sign << 63) | ((uint64_t)num.biased_exp << 52) | num.fraction};
    return ud.drep;
}

// convert a double into a pack
static pack unpack(double num) {
    union ud_rep ud = {.drep = num};
    pack ret = {
      .sign        = 0 != getbit(ud.urep, 64),
      .biased_exp  = (ud.urep & EXP_MASK) >> 52,
      .fraction    = ud.urep & FRAC_MASK
    };
    return ret;
}

// Returns whether left represents the number of lesser absolute value
// if the two numbers have equal magnitude, then return true if left is negative,
static bool abs_less(pack left, pack right) {
    return left.biased_exp < right.biased_exp
        || (left.biased_exp == right.biased_exp && left.fraction < right.fraction)
        || (left.biased_exp == right.biased_exp && left.fraction == right.fraction && left.sign);
}

// is infinity
static bool is_inf(pack num) {
    return num.biased_exp == INF_EXP && num.fraction == 0;
}

// is not a number
static bool is_nan(pack num) {
    return num.biased_exp == INF_EXP && num.fraction != 0;
}

// is denormal
static bool is_den(pack num) {
    return num.biased_exp == 0;
}

// is zero
static bool is_zero(pack num) {
    return num.biased_exp == 0 && num.fraction == 0;
}

double add(double in1, double in2) {
    assert(NUM_ROUND_BITS > 1 && NUM_ROUND_BITS < 12);

    pack num1 = unpack(in1);
    pack num2 = unpack(in2);

    // check inf / nan cases
    if (is_nan(num1)) return in1;
    if (is_nan(num2)) return in2;
    if (is_inf(num1)) return in1;
    if (is_inf(num2)) return in2;

    // process denormals as zero
    if (is_den(num1)) num1.fraction = 0;
    if (is_den(num2)) num2.fraction = 0;

    // early exit if either op is zero
    if (is_zero(num1)) return repack(num2);
    if (is_zero(num2)) return repack(num1);


    // if, for subtraction, we compute sign_max*(|max| - |min|), then we dont have
    // to worry about unsigned underflow when the exps of the inputs are equal but
    // one of the significands is larger than the other.
    pack abs_max = num1;
    pack abs_min = num2;
    pack sum;
    if (abs_less(num1, num2)) {
        abs_max = num2;
        abs_min = num1;
    }


    // I will store the significand at the right end of a uint64_t
    // the uint64_t will be in this format
    // [8: zeros][1: overflow bit][1: hidden bit][52: fraction][2: rounding bits]
    uint64_t max_sig = abs_max.fraction << NUM_ROUND_BITS;
    uint64_t min_sig = abs_min.fraction << NUM_ROUND_BITS;
    sethidden(max_sig);
    sethidden(min_sig);

    uint64_t sum_sig = 0;
    // sign of output is the sign of the input that has maximum abs value
    sum.sign = abs_max.sign;
    sum.biased_exp = abs_max.biased_exp;





    // SHIFT
    const uint32_t exp_diff = abs_max.biased_exp - abs_min.biased_exp;
    bool has_lost_bits;
    if (exp_diff >= 64) { // undefined behavior to shift >= type's width
        has_lost_bits = min_sig != 0; // true
        min_sig = 0;
    } else {
        has_lost_bits = 0 != nlowerbits(min_sig, exp_diff);
        min_sig >>= exp_diff;
    }
    // abs_min.exp = abs_max.exp;

    // since max.exp >= min.exp the difference does not underflow
    assert(exp_diff < abs_max.biased_exp);

    // if max.exp <= (INF_EXP - 1) and min.exp >= 1
    // then (max.exp - min.exp) <= (INF_EXP - 2)
    assert(exp_diff <= INF_EXP - 2);

    // useful for determining that the significand sum will not underflow (when subtracting)
    assert(max_sig >= min_sig);






    // ADD / SUBTRACT
    const bool will_subtract = abs_max.sign != abs_min.sign;
    if (will_subtract) {
        // we have to subtract 1 from max.sig if we've lost bits after shifting abs_min
        // since those bits still contribute to the sum, although only in a round about
        // way when we round :)
        sum_sig = (max_sig - has_lost_bits) - min_sig;
        // if we sum to zero
        if (sum_sig == 0) {
            sum.biased_exp = 0;
            sum.fraction = 0;
            return repack(sum);
        }
    } else {
        sum_sig = max_sig + min_sig;
    }






    // NORMALIZE
    if (will_subtract) {
        while (sum.biased_exp > 1 && !gethidden(sum_sig)) {
            sum_sig <<= 1;
            sum.biased_exp -= 1;
        }
        // the difference is denormal, it would be incorrect to decrement the
        // index and right shift since the exp of 000 and 001 represent the same
        // true exp for doubles
        if (sum.biased_exp <= 1 && !gethidden(sum_sig)) {
            sum.biased_exp = 0;
            sum.fraction = (sum_sig >> NUM_ROUND_BITS) & FRAC_MASK;
            return repack(sum);
        }
    } else if (getoverflow(sum_sig)) {
        has_lost_bits |= sum_sig & 1; // we might lose bits during normalization
        sum_sig >>= 1;
        sum.biased_exp += 1;
        if (sum.biased_exp >= INF_EXP) {
            return sum.sign ? -INFINITY : INFINITY;
        }
    }

    // at this point we are normalized
    assert(gethidden(sum.significand) && !getoverflow(sum.significand));







    // ROUND
    const bool LSB = getbit(sum_sig, NUM_ROUND_BITS + 1);
    const bool b53 = getbit(sum_sig, NUM_ROUND_BITS); // one bit to right LSB
    const bool bits_after_b53 = has_lost_bits || nlowerbits(sum_sig, NUM_ROUND_BITS-1); // any bit to right b53
    if (b53 && (LSB || bits_after_b53)) {
        sum_sig += 1 << NUM_ROUND_BITS;
    }
    // zero out rounding bits part
    sum_sig &= ~ROUND_BITS_MASK;
    has_lost_bits = false;

    // we are normal or rounding caused the significand to be in the form 10.000..0
    assert(gethidden(sum_sig) || getoverflow(sum_sig));






    // NORMALIZE
    if (getoverflow(sum_sig)) {
        // if rounding caused us to unnormalize then our significand is now 10.000..0
        assert(sum_sig == getbit(sum_sig, 2 + 52 + NUM_ROUND_BITS));

        sum_sig >>= 1;
        sum.biased_exp += 1;
        if (sum.biased_exp >= INF_EXP) {
            return sum.sign ? -INFINITY : INFINITY;
        }
    }

    // at this point we are normalized
    assert(gethidden(sum_sig) && !getoverflow(sum_sig));






    // get fraction from significand
    sum.fraction = (sum_sig >> NUM_ROUND_BITS) & FRAC_MASK;
    return repack(sum);
}
