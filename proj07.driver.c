/*******************************************************************************
 Bradley Bauer
 Project 7
*******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "project07.support.h"

#define BUFFER_SIZE 32
struct tcase {
    uint32_t machine_code;
    char instruction[BUFFER_SIZE];
};

void print_tcase(struct tcase tcase, char* result) {
    printf("| %08x | %20s | %20s |\n",
            tcase.machine_code,
            tcase.instruction,
            result);
}

int main() {
    const struct tcase tcases[] = {
        {0xffffffff, "Bad machine code"},
        {0x00000000, "Bad machine code"},
        {0xe1f0e04f, "Bad machine code"}, // bits 8:5 set when Ibit is off
        {0xe1e8e010, "Bad machine code"}, // move instructions dont use Rn
        {0xe1000001, "Bad machine code"}, // comparison but no Sbit
        {0xe1500101, "Bad machine code"}, // comparisons should not have dest reg
        {0xe1e0e00f, "mvn r14, r15"},
        {0xe1f0e00f, "mvns r14, r15"},
        {0xe3e0e0ff, "mvn r14, #0xff"},
        {0xe3f0e0ff, "mvns r14, #0xff"},
        {0xe0000000, "and r0, r0, r0"},
        {0xe0100000, "ands r0, r0, r0"},
        {0xe0012004, "and r2, r1, r4"},
        {0xe20100ff, "and r0, r1, #0xff"},
        {0xe0010002, "and r0, r1, r2"},
        {0xe0110002, "ands r0, r1, r2"},
        {0xe21000ff, "ands r0, r0, #0xff"},
        {0xe20000ff, "and r0, r0, #0xff"},
        {0xe0232004, "eor r2, r3, r4"},
        {0xe22320ff, "eor r2, r3, #0xff"},
        {0xe22230ff, "eor r3, r2, #0xff"},
        {0xe22200ff, "eor r0, r2, #0xff"},
        {0xe23200ff, "eors r0, r2, #0xff"},
        {0xe0421003, "sub r1, r2, r3"},
        {0xe24ff0ff, "sub r15, r15, #0xff"},
        {0xe25ff0ff, "subs r15, r15, #0xff"},
        {0xe26000ff, "rsb r0, r0, #0xff"},
        {0xe26010ff, "rsb r1, r0, #0xff"},
        {0xe0621003, "rsb r1, r2, r3"},
        {0xe0721003, "rsbs r1, r2, r3"},
        {0xe27210ff, "rsbs r1, r2, #0xff"},
        {0xe3cfe0ff, "bic r14, r15, #0xff"},
        {0xe1cfe000, "bic r14, r15, r0"},
        {0xe1dfe000, "bics r14, r15, r0"},
        {0xe1a0e00f, "mov r14, r15"},
        {0xe3a0f0ff, "mov r15, #0xff"},
        {0xe1b0e00f, "movs r14, r15"},
        {0xe1810002, "orr r0, r1, r2"},
        {0xe38100ff, "orr r0, r1, #0xff"},
        {0xe39100ff, "orrs r0, r1, #0xff"},
        {0xe1100001, "tst r0, r1"},
        {0xe1100001, "tst r0, r1"},
        {0xe31100ff, "tst r1, #0xff"},
        {0xe33000ff, "teq r0, #0xff"},
        {0xe1320001, "teq r2, r1"},
        {0xe1700001, "cmn r0, r1"},
        {0xe37100ff, "cmn r1, #0xff"},
        {0xe1500001, "cmp r0, r1"},
        {0xe35100ff, "cmp r1, #0xff"},
        {0xe2c000ff, "sbc r0, r0, #0xff"},
        {0xe2c120ff, "sbc r2, r1, #0xff"},
        {0xe0d10003, "sbcs r0, r1, r3"},
        {0xe0c10003, "sbc r0, r1, r3"},
        {0xe0a00001, "adc r0, r0, r1"},
        {0xe2a100ff, "adc r0, r1, #0xff"},
        {0xe2b1f0ff, "adcs r15, r1, #0xff"},
        {0xe0800001, "add r0, r0, r1"},
        {0xe0810003, "add r0, r1, r3"},
        {0xe281f0ff, "add r15, r1, #0xff"},
        {0xe291f0ff, "adds r15, r1, #0xff"},
        {0xe2e000ff, "rsc r0, r0, #0xff"},
        {0xe0e00001, "rsc r0, r0, r1"},
        {0xe2e100ff, "rsc r0, r1, #0xff"},
        {0xe2f100ff, "rscs r0, r1, #0xff"},
        {0xe24000ff, "sub r0, r0, #0xff"},
        {0xe0400001, "sub r0, r0, r1"},
        {0xe24100ff, "sub r0, r1, #0xff"},
        {0xe25100ff, "subs r0, r1, #0xff"},
        {0xe0400001, "sub r0, r0, r1"},
        {0xe24100ff, "sub r0, r1, #0xff"},
        {0xe0410002, "sub r0, r1, r2"},
        {0xe051f002, "subs r15, r1, r2"},
        {0xe251f0ff, "subs r15, r1, #0xff"},
        {0xe25110ff, "subs r1, r1, #0xff"},
        {0xe25100ff, "subs r0, r1, #0xff"}
    };
    const int num_tcases = sizeof(tcases) / sizeof(tcases[0]);

    printf("----------------------------------------------------------\n");
    printf("|   hex    |   expected output    |    actual output     |\n");
    printf("----------------------------------------------------------\n");

    // run the tests and exit
    for (int i = 0; i < num_tcases; ++i) {
        char result[BUFFER_SIZE];
        memset(result, 0, BUFFER_SIZE);

        decode(tcases[i].machine_code, result);

        print_tcase(tcases[i], result);

        int cmp = strcmp(result, tcases[i].instruction);
        if (0 != cmp) {
            printf(" Failed \n");
            return 0;
        }
    }
    printf("----------------------------------------------------------\n");
    return 0;
}

