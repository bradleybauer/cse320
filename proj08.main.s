
.global main

.data
FORMAT_STR:
    .ascii  "\nchars: %d\n"
    .ascii  "letters: %d\n"
    .ascii  "lines: %d\n"
    .ascii  "ws: %d\n"
    .ascii  "oct: %d\n"
    .ascii  "dec: %d\n"
    .asciz  "hex: %d\n"

.text
@leaf functions
inc_if_ws:
    cmp r0, #' '
    cmpne r0, #'\t'
    cmpne r0, #'\n'
    bxne lr

    @ if equal
    add r7, #1
    bx lr

inc_if_newline:
    cmp r0, #'\n'
    bxne lr

    @ if equal
    add r6, #1
    bx lr

inc_if_octal:
    cmp r0, #'0'
    bxlt lr
    cmp r0, #'7'
    bxgt lr

    add r8, #1
    bx lr

inc_if_dec:
    cmp r0, #'0'
    bxlt lr
    cmp r0, #'9'    @ at this point we are >= 0
    bxgt lr
    @ here we are >= 0 and <= 9
    add r9, #1
    bx lr

inc_if_hex:
    cmp r0, #'0'
    bxlt lr         @ cant be letter or digit if less than '0'
    cmp r0, #'9'
    bgt check_upper
    b inc_hex

    check_upper:
    cmp r0, #'A'
    bxlt lr         @ cant be letter if less than 'A'
    cmp r0, #'F'
    bgt check_lower
    b inc_hex

    check_lower:
    cmp r0, #'a'
    bxlt lr         @ cant be lowercase if less than 'a'
    cmp r0, #'f'
    bxgt lr

    inc_hex:
    add r10, #1
    bx lr

inc_if_letter:
    cmp r0, #'A'
    bxlt lr
    cmp r0, #'Z'
    bgt check_lower1
    b inc_letter

    check_lower1:
    cmp r0, #'a'
    bxlt lr
    cmp r0, #'z'
    bxgt lr

    inc_letter:
    add r5, #1
    bx lr

inc_num_chars:
    add r4, #1
    bx lr

main:
    push {r4-r10,lr}

    @ init counts
    mov r4, #0
    mov r5, #0
    mov r6, #0
    mov r7, #0
    mov r8, #0
    mov r9, #0
    mov r10, #0

EOF = -1
loop:
    bl getchar
    cmp r0, #EOF
    beq loopend

    bl inc_num_chars
    bl inc_if_octal
    bl inc_if_hex
    bl inc_if_dec
    bl inc_if_ws
    bl inc_if_newline
    bl inc_if_letter

    bl putchar
    b loop
loopend:

    ldr r0, =FORMAT_STR
    mov r1, r4
    mov r2, r5
    mov r3, r6
    push {r7-r10}
    bl printf
    pop {r7-r10}

    pop {r4-r10,lr}
    bx lr

