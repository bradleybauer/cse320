ERROR = 0x80000000

.global sub
.global mul
.global divide
.global power
.global factorial
.global add
.global absolute
.global negate

@ int negate( int N );
negate:
    rsb r0, #0
    bx lr

@ int absolute( int N );
absolute:
    @ if we use bic then -2^31 goes to 0
    @ but -2^31 should go to ERROR because 2^31 cant be represented in 32 bit
    @ twos complement
    cmp r0, #0
    rsblt r0, #0
    bx lr

@ int add( int A, int B );
add:
    adds r0, r0, r1
    movvs r0, #ERROR
    bx lr

@ int sub( int A, int B );
sub:
    subs r0, r0, r1
    @ unfortunately (-2^32 + 1) - 1 == error code
    movvs r0, #ERROR
    bx lr

@ r2 = mulLo
@ r3 = mulHi
@ r0 = A
@ r1 = B
@ int mul( int A, int B );
mul:
    smull r2, r3, r0, r1

    @ if r3 not equal to 0 or -1 then fail
    cmp r3, #0
    cmpne r3, #-1
    movne r0, #ERROR
    bxne lr

    @ if sign(lo) not equal to sign(hi) fail
    eor r3, r2
    ands r3, #0x80000000
    movne r0, #ERROR
    bxne lr

    mov r0, r2
    bx lr

@ int divide( int A, int B );
divide:
    cmp r1, #0
    moveq r0, #ERROR
    bxeq lr

    sdiv r0, r0, r1
    bx lr

@ int power( int x, int p );
@ r0 x
@ r1 p
@ r2 x^p
@ r3 temp values for func return handling
@ i know i could just use a simple loop and do checks for x=+-1 and x=0 to avoid
@ timeout on huge p, but i wanted to implement pow like this because its more
@ interesting.
@ result = x^(b0*2^0)x^(b1*2^1) ... x^(bn*2^n) if p has binary rep b0b1 ... bn
@        = x^(b0*2^0 + b1*2^1 + ... + bn*2^n)
@        = x^p
power:
    @ for x = -1 or x = 1 compute x^|p|
    cmp r0, #-1
    cmpne r0, #1
    biceq r1, r1, #0x80000000

    @ fail if p < 0
    cmp r1, #0
    movlt r0, #ERROR
    bxlt lr

    @ 0^0 undefined
    cmp r1, r0
    cmpeq r0, #0
    moveq r0, #ERROR
    bxeq lr

    mov r2, #1
POW_LOOP:
    @ if kth bit is not set in p
    ands r3, r1, #1
    beq dont_mul

        @ multiply x^(2^k) into the result (r2)
        push {r0,r1,r2,lr}
        mov r1, r2
        bl mul
        mov r3, r0
        pop {r0,r1,r2,lr}
        cmp r3, #ERROR
        moveq r0, r3
        bxeq lr
        mov r2, r3

    dont_mul:

    lsrs r1, r1, #1
    beq POW_LOOP_END

    @ compute x^(2^(k+1))
    push {r0,r1,r2,lr}
    mov r1, r0
    bl mul
    mov r3, r0
    pop {r0,r1,r2,lr}
    cmp r3, #ERROR
    moveq r0, r3
    bxeq lr
    mov r0, r3

    b POW_LOOP
POW_LOOP_END:
    mov r0, r2
    bx lr

@ int factorial( int N );
factorial:
    @ check errors
    cmp r0, #12
    movgt r0, #ERROR
    bxgt lr
    cmp r0, #0
    movlt r0, #ERROR
    bxlt lr

    ldr r1, =FACTORIAL_LUT
    @ r0 = *(r1 + r0 * 4)
    @ int array = r1
    @ index = r0
    @ r0 = array[index]
    ldr r0, [r1, r0, LSL#2]
    bx lr

.balign 4
FACTORIAL_LUT:
    .word 0x00000001
    .word 0x00000001
    .word 0x00000002
    .word 0x00000006
    .word 0x00000018
    .word 0x00000078
    .word 0x000002d0
    .word 0x000013b0
    .word 0x00009d80
    .word 0x00058980
    .word 0x00375f00
    .word 0x02611500
    .word 0x1c8cfc00

