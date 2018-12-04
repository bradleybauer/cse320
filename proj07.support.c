/*******************************************************************************
 Bradley Bauer
 Project 7
*******************************************************************************/

// Turn off asser
#define NDEBUG

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

struct info {
    char mnemonic[3 + 1];
    bool is_test;
    bool is_mov;
};

static const char* warning = "Bad machine code";
static const uint32_t expected_upper_bits = 0b111000;
static const struct info opcode_to_info[] = {
    {"and", false, false},
    {"eor", false, false},
    {"sub", false, false},
    {"rsb", false, false},
    {"add", false, false},
    {"adc", false, false},
    {"sbc", false, false},
    {"rsc", false, false},
    {"tst", true, false},
    {"teq", true, false},
    {"cmp", true, false},
    {"cmn", true, false},
    {"orr", false, false},
    {"mov", false, true},
    {"bic", false, false},
    {"mvn", false, true}
};

// Bits 31:26    111000
// Bit 25        I bit
// Bits 24:21    opcode
// Bit 20        S bit
// Bits 19:16    Rn
// Bits 15:12    Rd
// Bits 7:0      immediate value (when I=1)
// Bits 3:0      Rm (when I=0)

// get the ith bit from the right
// LSB is the 1st bit from the right
// MSB is the 64th bit from the right
static uint32_t getbit(uint32_t x, uint32_t i) {
    uint32_t bit = x & (1u << (i-1));
    return bit >> (i-1);
}

static uint32_t bitrange(uint32_t x, uint32_t a, uint32_t b) {
    uint32_t ones_upto_a = ((1u << (a-1)) - 1) | (1u << (a-1));
    uint32_t zeros_before_b = ~((1u << (b-1)) - 1);
    uint32_t mask = ones_upto_a & zeros_before_b;
    uint32_t bits = x & mask;
    return bits >> (b - 1);
}

void decode(uint32_t machine_code, char buffer[]) {
    // make sure that buffer is an empty string
    buffer[0] = '\0';

    assert(machine_code == bitrange(machine_code, 32, 1));

    const uint32_t upper_bits = bitrange(machine_code, 32, 27);

    const bool Ibit = getbit(machine_code, 26);
    const uint32_t opcode = bitrange(machine_code, 25, 22);
    const bool Sbit = getbit(machine_code, 21);
    const uint32_t Rn = bitrange(machine_code, 20, 17);
    const uint32_t Rd = bitrange(machine_code, 16, 13);
    const uint32_t unused_bits = bitrange(machine_code, 12, 9);
    const uint32_t Imm = bitrange(machine_code, 8, 1);
    const uint32_t Rm = bitrange(machine_code, 4, 1);
    const struct info info = opcode_to_info[opcode];

    // check errors
    if (upper_bits != expected_upper_bits) {
        sprintf(buffer, "%s", warning);
        return;
    }
    // unconditionally unused bits
    if (unused_bits) {
        sprintf(buffer, "%s", warning);
        return;
    }
    // If Ibit is off then we dont need these bits
    if (!Ibit && bitrange(Imm, 8, 5)) {
        sprintf(buffer, "%s", warning);
        return;
    }
    // comparisons should have the Sbit
    if (info.is_test && !Sbit) {
        sprintf(buffer, "%s", warning);
        return;
    }
    // comparisons do not have a destination register
    if (info.is_test && Rd) {
        sprintf(buffer, "%s", warning);
        return;
    }
    // move instructions do not use Rn
    if (info.is_mov && Rn) {
        sprintf(buffer, "%s", warning);
        return;
    }
    // these bits are not used if Ibit is off
    if (!Ibit && bitrange(machine_code, 8, 5)) {
        sprintf(buffer, "%s", warning);
        return;
    }

    // no need for s postfix on comparisons
    if (Sbit && !info.is_test)
        sprintf(buffer, "%ss ", info.mnemonic);
    else
        sprintf(buffer, "%s ", info.mnemonic);

    if (!info.is_test)
        sprintf(buffer, "%sr%d, ", buffer, Rd);

    if (!info.is_mov)
        sprintf(buffer, "%sr%d, ", buffer, Rn);

    // any of the 16 instrs can take an Imm
    if (Ibit)
        sprintf(buffer, "%s#0x%x", buffer, Imm);
    else
        sprintf(buffer, "%sr%d", buffer, Rm);
}
