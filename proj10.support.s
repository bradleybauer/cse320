@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Project 10
@ Author: Bradley Bauer
@ The search function searches through an array of struct player instances
@ to find an instance that has a certain value in the 'number' field.
@ Uses a binary search since the input is ordered according to jersey numbers.
@ If any of the inputs are invalid, then returns 0.
@ If no struct player instance with the correct value for the field 'number' is
@ found then return 0.
@ Returns 1 for a successful search.
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
.global search

PLAYER_STRUCT_SIZE = 40

@ int search( struct table*, unsigned long, struct player** );
search:
    push {r4-r11, lr}

    @ Register purposes
    @
    @ search arguments
    @ r0 = struct table*
    @ r1 = jersey to search for: unsigned long
    @ r2 = result: struct player**
    @
    @ struct table
    @ r3 = capacity
    @ r4 = count
    @ r5 = player array
    @
    @ bsearch info
    @ r6 = jersey of current player
    @ r7 = start
    @ r8 = mid
    @ r9 = end
    @ r10 = PLAYER_STRUCT_SIZE
    @ r11 = r8 * r10, ldrh offset

    @ check error cases
    @ table ptr not null
    cmp r0, #0
    beq ERROR
    @ table ptr aligned
    tst r0, #3
    bne ERROR
    @ result ptr not null
    cmp r2, #0
    beq ERROR
    @ result ptr aligned
    tst r2, #3
    bne ERROR

    ldrh r3, [r0, #0]
    ldrh r4, [r0, #2]
    ldr  r5, [r0, #4]

    @ player array ptr not null
    cmp r5, #0
    beq ERROR
    @ player array ptr aligned
    tst r2, #3
    bne ERROR
    @ count <= capacity
    cmp r4, r3
    bgt ERROR
    @ count > 0
    cmp r4, #0
    ble ERROR

    @ setup binary search
    mov r10, #PLAYER_STRUCT_SIZE
    mov r7, #0
    mov r9, r4
    sub r9, #1

SEARCH_LOOP:
    @ compute mid
    add r8, r7, r9
    lsr r8, #1

    @ get jersey number for player at mid
    @ convert index (r8) into byte offset (r11)
    mul r11, r8, r10
    ldrh r6, [r5, r11]

    @ do binary search comparison
    cmp r1, r6
        movlt r9, r8
        sublt r9, #1

        movgt r7, r8
        addgt r7, #1

        addeq r5, r11
        streq r5, [r2]
        moveq r0, #1
        beq DONE

    @ while start <= end
    cmp r7, r9
    bgt ERROR
b SEARCH_LOOP

ERROR:
    mov r0, #0
DONE:
    pop {r4-r11, lr}
    bx lr

