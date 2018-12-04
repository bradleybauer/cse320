/*
Project 11
Author: Bradley Bauer
*/
.global insert
.global delete
.global search

@ int verify_table( struct table* )
@ make sure the table pointers are not null and are aligned
@ make sure that count <= capacity
verify_table:
    @ table ptr not null
    cmp r0, #0
    beq vERROR

    @ table ptr aligned
    tst r0, #3
    bne vERROR

    ldr r1, [r0, #4]
    @ memory ptr not null
    cmp r1, #0
    beq vERROR
    @ memory ptr aligned
    tst r1, #3
    bne vERROR

    ldrh r1, [r0] @ capacity
    ldrh r0, [r0, #2] @ count
    @ count <= capacity
    cmp r0, r1
    bgt vERROR
    @ count >= 0
    cmp r1, #0
    blt vERROR

    @ success
    mov r0, #1
    b vDONE
  vERROR:
    mov r0, #0
  vDONE:
    bx lr

PLAYER_STRUCT_SIZE = 40
MAX_NAME_STR_LEN = 24

/***********************************************************************/
/*  Function:  insert                                                  */
/*                                                                     */
/*  Purpose:  insert a player into the table, as long as there is      */
/*  room in the table and the player is not already present.           */
/*                                                                     */
/*  Arguments:                                                         */
/*    pointer to table of players                                      */
/*    jersey number of player to be inserted                           */
/*    pointer to name of player                                        */
/*    number of games played                                           */
/*    number of goals scored                                           */
/*    number of assists scored                                         */
/*                                                                     */
/*  Return value:                                                      */
/*    1 (true) if player inserted, 0 (false) otherwise                 */
/***********************************************************************/
@ int insert( struct table*, unsigned long, char*, int, int, int );
insert:
    push {r0-r6, lr}
    bl verify_table
    cmp r0, #0
    pop {r0-r3}
    beq iERROR

    @ if char* is 0 then error
    cmp r2, #0
    beq iERROR

    @ games >= 0  ?
    cmp r3, #0
    blt iERROR
    @ goals >= 0  ?
    ldr r4, [sp, #16]
    cmp r4, #0
    blt iERROR
    @ assists >= 0 ?
    ldr r5, [sp, #20]
    cmp r5, #0
    blt iERROR

    @ if games == 0, then goals and assists should == 0 too

    @ call strlen and make sure string fits in struct player.name
    push {r0-r3}
    mov r0, r2
    bl strlen
    cmp r0, #MAX_NAME_STR_LEN
    pop {r0-r3}
    bgt iERROR

    push {r0-r4}
    add r2, sp, #16 @ have search put player* in r6
    bl search
    cmp r0, #1      @ fail if search found player
    pop {r0-r3, r6} @ puts player* in r6
    beq iERROR

    @ Reg usage
    @ r0 - r3 = first 4 args
    @ r4 = capacity
    @ r5 = count
    @ r6 = player* = insert pos*
    ldrh r4, [r0]
    ldrh r5, [r0, #2]

    @ check that there is enough capacity (cap - cnt > 0), fail if not
    cmp r4, r5
    ble iERROR

    @ update count
    add r5, #1
    strh r5, [r0, #2]
    ldr r0, [r0, #4]
    @ r0 = memory

    push {r1-r3}
    @ r1 = src = player ptr
    mov r1, r6
    @ r2 = len = (memory + count*size) - player ptr
    mov r3, #PLAYER_STRUCT_SIZE
    mla r2, r3, r5, r0
    @ r0 = dst = player ptr + size
    add r0, r1, #PLAYER_STRUCT_SIZE
    sub r2, r0
    bl memmove
    pop {r1-r3}

    @ Reg usage
    @ r0 = memory*
    @ r1 = jersey
    @ r2 = name
    @ r3 = games
    @ r4 = goals
    @ r5 = assists
    @ r6 = player* = insert pos*
    ldr r4, [sp, #16]  @ load goals
    ldr r5, [sp, #20]  @ load assists

    strh r1, [r6]      @ store jersey number
    strh r3, [r6, #28] @ store games
    strh r4, [r6, #30] @ store goals
    strh r5, [r6, #32] @ store assists
    add r5, r4
    strh r5, [r6, #34] @ store points
    mov r4, r3         @ protect games from strcpy call
    mov r3, #0
    @ make sure that strcpy src is null terminated
    strb r3, [r2, #MAX_NAME_STR_LEN]
    mov r1, r2         @ put char* into src
    add r0, r6, #2     @ put player_ptr + 2 into dst
    bl strcpy          @ store name

    @ if games is zero then store 0 as PGP
    cmp r4, #0
    @ float 0 is int 0
    streq r4, [r6, #36]
    beq NO_DIV

    fmsr s0, r4
    fsitos s0, s0    @ (float)games
    fmsr s1, r5
    fsitos s1, s1    @ (float)points
    fdivs s1, s1, s0
    fsts s1, [r6, #36] @ store points per game
  NO_DIV:

    mov r0, #1
    b iDONE
  iERROR:
    mov r0, #0
  iDONE:
    pop {r4-r6, pc}

/***********************************************************************/
/*  Function:  delete                                                  */
/*                                                                     */
/*  Purpose:  delete a player from the table, if the                   */
/*  player is present in the table.                                    */
/*                                                                     */
/*  Arguments:                                                         */
/*    pointer to table of players                                      */
/*    jersey number of player to be deleted                            */
/*                                                                     */
/*  Return value:                                                      */
/*    1 (true) if player deleted, 0 (false) otherwise                  */
/***********************************************************************/
@ int delete( struct table*, unsigned long );
@ 19 lines of asm
delete:
    @ Register usage
    @ r0 = table ptr
    @ r1 = jersey number
    @ r2 = player to delete ptr

    mov r3, r0
    push {r0, r3, lr}
    @ make search put struct player* onto stack
    mov r2, sp
    bl search
    @ fail if search did not find player ( or table invalid )
    cmp r0, #0
    @ balance pop's if search failed (avoids useless ldr)
    addeq sp, #8
    beq dDONE
    pop {r0, r1} @ player*, table*

    @ Register usage
    @ r0 = player to delete ptr
    @ r1 = table ptr
    @ r2 = table.count
    @ r3 = table.memory

    ldrh r2, [r1, #2]
    ldr r3, [r1, #4]

    @ shift to the left every player after the player to delete
    @ if player is at end of array then len == 0 and memmove does nothing
    @ if there is only one player then len == 0 and memmove does nothing

    @ update count
    sub r2, #1
    strh r2, [r1, #2]

    @ COMPUTE MEMMOVE PARAMETERS
    @ src = player ptr + size
    @ dst = player ptr
    @ len = (memory + (count-1)*size) - player ptr

    @ r0 = player ptr
    @ r1 = free
    @ r2 = count-1
    @ r3 = memory

    @ compute len into r2
    mov r1, #PLAYER_STRUCT_SIZE
    mla r2, r1, r2, r3
    sub r2, r0

    @ r0 = player ptr
    @ r1 = size
    @ r2 = len
    @ r3 = memory

    @ compute src into r1
    add r1, r0

    bl memmove

    mov r0, #1
  dDONE:
    pop {pc}

/***********************************************************************/
/*  Function:  search                                                  */
/*                                                                     */
/*  Purpose:  locate and return a pointer to a player, if the player   */
/*  is present in the table.                                           */
/*                                                                     */
/*  Arguments:                                                         */
/*    pointer to table of players                                      */
/*    jersey number of player to be located                            */
/*    pointer to pointer to player                                     */
/*                                                                     */
/*  Return value:                                                      */
/*    1 (true) if player located, 0 (false) otherwise                  */
/***********************************************************************/
@ int search( struct table*, unsigned long, struct player** );
search:
    push {r4, lr}

    @ Register purposes
    @ r0 = struct table*
    @ r1 = jersey to search for
    @ r2 = struct player**

    @ setup verify_table function call
    push {r0-r2}
    bl verify_table
    cmp r0, #0
    pop {r0-r2}
    beq sERROR

    @ load table members
    ldrh r4, [r0, #2]
    ldr r0, [r0, #4]
    @ done with table*

    @ reg purposes
    @ r0 = player array
    @ r1 = jeresy to search for
    @ r2 = struct player**
    @ r3 = jersey of current player
    @ r4 = count

    @ result ptr not null
    cmp r2, #0
    beq sERROR
    @ result ptr aligned
    tst r2, #3
    bne sERROR

  sLOOP:
    @ while count > 0
    cmp r4, #0
    @ if all elements in array are less than then set the player** to a valid
    @ insert spot
    streq r0, [r2]
    beq sERROR

    @ get jersey number
    ldrh r3, [r0]

    cmp r3, r1
        @ store address to player with smallest jersey number that is greater
        @ than the desired jersey number so that insert knows where to insert
        @ if the player is not already in the table
        strge r0, [r2]
        moveq r0, #1
        beq sDONE
        bgt sERROR

    add r0, #PLAYER_STRUCT_SIZE
    sub r4, #1
  b sLOOP

  sERROR:
    mov r0, #0
  sDONE:
    pop {r4, pc}

