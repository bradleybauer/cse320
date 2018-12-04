#include <stdio.h>
#include <stdlib.h>
#include "/user/cse320/Projects/project09.support.h"

// arnt macros beautiful? /s
#define test(f,X,Y) printf("\t%-30s = %d\n", #f"("#X","#Y")", f(X,Y))
// UNARY! test
#define utest(f,X) printf("\t%-30s = %d\n", #f"("#X")", f(X))

int main() {
    printf("Should fail\n");
    test(sub, 0x80000000, 1);
    test(add, 0x7fffffff, 1);
    test(add, 0x7fffffff, 0x7fffffff);
    test(add, 0x7fffffff, 10);
    test(add, 0x80000000, -1);
    test(add, 0x80000000, 0x80000000);
    test(power, -2, 24000001);
    test(power, 2, 31);
    test(power, 0, 0);
    test(power, 0x7fffffff, 0x7fffffff);
    utest(factorial, 13);
    utest(factorial, -1);
    utest(negate, 0x80000000);
    utest(absolute, 0x80000000);
    test(mul, 0x40000000, 2);
    test(mul, 0x7fffffff, 2);
    test(mul, 0x80000000, 0x80000000);
    test(mul, 0x7fffffff, 0x7fffffff);
    test(mul, 0x80000000, -2);
    test(mul, 0x80000000, 2);
    test(divide, 1, 0);

    printf("Should pass\n");
    utest(negate, 0x80000001);
    utest(negate, -12344);
    utest(negate, 1);
    utest(negate, 0);
    utest(negate, -1);
    utest(absolute, -1);
    utest(absolute, 0x80000001);
    utest(absolute, -123);
    utest(absolute, 0);
    utest(absolute, -1);
    utest(absolute, 1);
    test(add, 10,16);
    test(add, 0,0);
    test(add, -1, 0);
    test(sub, 0x80000000 + 1, 1);
    test(sub, 0x80000000 + 2, 1);
    test(sub, 1,2);
    test(sub, 1,1);
    test(sub, 1,-1);
    test(sub, 3, 1);
    test(sub, 3, 10);
    test(sub, 0x80000000, 0x80000001);
    test(mul, 3, 10);
    test(mul, -3, 10);
    test(mul, -13, 5);
    test(mul, -100000000, 10);
    test(mul, 0x7fffffff, -1);
    test(mul, 0x7fffffff, 1);
    test(mul, 0x80000000, 1);
    test(mul, 0x80000000, 1);
    test(mul, 0x80000000, 0);
    test(mul, 0x7fffffff, 0);
    test(mul, -13,28561);
    test(mul, -13, power(-13,4));
    test(power, -13, 5);
    test(power, -1, 24000001);
    test(power, 1, 24000001);
    test(power, 0, 24000001);
    test(power, 2, 10);
    test(power, 3, 3);
    test(power, 5, 1);
    test(power, 5, 3);
    test(power, -5, 3);
    test(power, 1, 1);
    test(power, 1, -1);
    test(power, -1, 1);
    test(power, -1, -1);
    test(power, 3, 10);
    test(power, -3, 10);
    test(power, -3, 11);
    test(divide, 2, 1);
    test(divide, 4, 2);
    test(divide, 2, 2);
    test(divide, 1, 2);
    test(divide, -1, 2);
    test(divide, -1, -1);
    test(divide, 1, -1);
    test(divide, -1, 1);
    test(divide, 2, -1);
    test(divide, -2, -1);
    test(divide, -16, -4);
    test(divide, 0, 0x7fffffff);
    test(divide, 0, 0x80000000);
    test(divide, 1, 0x7fffffff);
    test(divide, 1, 0x80000000);
    test(divide, 0x7fffffff, -1);
    test(divide, 0x7fffffff, 2);
    test(divide, 0x7fffffff, -2);
    printf("Factorial values\n");
    for (int i = 0; i < 13; ++i)
        printf("\t%d! = %d\n", i,factorial(i));
}
