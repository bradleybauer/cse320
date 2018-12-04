#include <stdio.h>
#include <stdlib.h>

#include "project11.support.h"

void printT(struct table* table) {
    printf("table capac: %d\n", table->capacity);
    printf("table count: %d\n", table->count);
    for (int i = 0; i < table->count; ++i) {
        printf("\n\tnumber: %d\n", table->memory[i].number);
        printf("\tname: %s\n", table->memory[i].name);
        printf("\tgames: %d\n", table->memory[i].games);
        printf("\tgoals: %d\n", table->memory[i].goals);
        printf("\tassists: %d\n", table->memory[i].assists);
        printf("\tpoints: %d\n", table->memory[i].points);
        printf("\tppgame: %f\n", table->memory[i].points_per_game);
    }
}

int main() {
    struct table table;
    table.capacity = 10;
    table.count = 10;
    table.memory = calloc(1, table.capacity*sizeof(struct player)); // because iirc i've had issues with calloc and this worked
    for (int i = 0; i < table.count; ++i) {
        struct player p;
        p.number = i;
        p.name[0] = 'a' + i;
        p.name[1] = '\0';
        if (i == 2) {
            p.name[0] = 'h';
            p.name[1] = 'i';
            p.name[2] = '\0';
        }
        p.games = 1 + i;
        p.goals = 2 + i;
        p.assists = 3 + i;
        p.points = 4 + i;
        p.points_per_game = (float)p.points / (float)p.games;
        table.memory[i] = p;
    }
    struct player* player = NULL;
    int number = 2;
    int success = search(&table, number, &player);
    printf("success: %d\n", success);
    if (success) {
        printf("%s\n", player->name);
    }
    return 0;

    // delete( struct table*, unsigned long );
    int del_suc = delete(&table, 9);
    if (!del_suc)
        return 0;
    printT(&table);

    del_suc = delete(&table, 3);
    if (!del_suc)
        return 0;
    printT(&table);

    del_suc = delete(&table, 0);
    if (!del_suc)
        return 0;
    printT(&table);

    del_suc = delete(&table, 7);
    if (!del_suc)
        return 0;
    printT(&table);

    del_suc = delete(&table, 8);
    if (!del_suc)
        return 0;
    printT(&table);

    del_suc = delete(&table, 5);
    if (!del_suc)
        return 0;
    printT(&table);

    del_suc = delete(&table, 4);
    if (!del_suc)
        return 0;
    printT(&table);

    del_suc = delete(&table, 2);
    if (!del_suc)
        return 0;
    printT(&table);

    del_suc = delete(&table, 1);
    if (!del_suc)
        return 0;
    printT(&table);

    del_suc = delete(&table, 6);
    if (!del_suc)
        return 0;
    printT(&table);

    // insert( struct table*, unsigned long, char*, int, int, int );
    int ins_suc = insert(&table, 10, "ricky", 8, 6, 19);
    if (!ins_suc)
        return 0;
    printT(&table);

    ins_suc = insert(&table, 11, "julian", 3, 6, 1);
    if (!ins_suc)
        return 0;
    printT(&table);

    ins_suc = insert(&table, 1, "bubbles", 1, 7, 9);
    if (!ins_suc)
        return 0;
    printT(&table);

    ins_suc = insert(&table, 8, "bubbles", 1, 7, -9);
    if (!ins_suc) {
        printf("insert failed\n");
    }
    printT(&table);

    ins_suc = insert(&table, 8, "ubbles", 1, 7, 0);
    if (!ins_suc) {
        printf("insert failed\n");
    }
    printT(&table);

    ins_suc = insert(&table, 9, "bubbles", 1, 1, 0);
    if (!ins_suc) {
        printf("insert failed\n");
    }
    printT(&table);

    ins_suc = insert(&table, 12, "twelvve", 0, 1, 0);
    if (!ins_suc) {
        printf("insert failed\n");
    }
    printT(&table);

    ins_suc = insert(&table, 2, "two", 2, 1, 0);
    if (!ins_suc) {
        printf("insert failed\n");
    }
    printT(&table);

    ins_suc = insert(&table, 3, "three", 3, 1, 0);
    if (!ins_suc) {
        printf("insert failed\n");
    }
    printT(&table);

    ins_suc = insert(&table, 4, "four", 4, 1, 0);
    if (!ins_suc) {
        printf("insert failed\n");
    }
    printT(&table);

    ins_suc = insert(&table, 5, "five", 5, 1, 0);
    if (!ins_suc) {
        printf("insert failed\n");
    }
    printT(&table);

    ins_suc = insert(&table, 6, "six", 6, 1, 0);
    if (!ins_suc) {
        printf("insert failed\n");
    }
    printT(&table);

    ins_suc = insert(&table, 7, "seven", 7, 1, 0);
    if (!ins_suc) {
        printf("insert failed\n");
    }
    printT(&table);

    ins_suc = insert(&table, 13, "13", 13, 1, 0);
    if (!ins_suc) {
        printf("insert failed\n");
    }
    printT(&table);

    ins_suc = insert(&table, 14, "14", 14, 1, 0);
    if (!ins_suc) {
        printf("insert failed\n");
    }
    printT(&table);

    // del_suc = delete(&table, 6);
    // if (!del_suc)
    //     return 0;
    // printT(&table);
}
