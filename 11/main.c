
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXLEN 128
#define MAXPATH 128
#define MAXMONKEYS 8

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
typedef int bool;
#define true 1
#define false 0

typedef enum Operation {
  MULTIPLY,
  ADD,
  SQUARE
} Operation;

#define Vec_Type long unsigned int
#include "../common/vec.h"

typedef struct Monkey {
    Vec items;
    Operation operation;
    int operand;
    int testDivisibleBy;
    int ifTestTrueThrowToMonkeyN;
    int ifTestFalseThrowToMonkeyN;
    int nTimesInspectedItems;
} Monkey;

Monkey Monkey_create() {
    Monkey m;
    m.items = Vec_create();
    m.operation = ADD;
    m.operand = 0;
    m.testDivisibleBy = 0;
    m.ifTestTrueThrowToMonkeyN = 0;
    m.ifTestFalseThrowToMonkeyN = 0;
    m.nTimesInspectedItems = 0;
    return m;
}
void Monkey_destroy(Monkey m) {
    Vec_destroy(&m.items);
}

void Monkey_print(Monkey m, int id) {
    printf("Monkey %d\n", id);
    printf("  Items: ");
    for( size_t i = 0; i < m.items.size; ++i ) {
        printf("%lu ", m.items.items[i]);
    }
    printf("\n");
    printf("  Operation: %s\n", m.operation == SQUARE ? "SQUARE" : m.operation == ADD ? "ADD" : m.operation == MULTIPLY ? "MULTIPLY" : "");
    printf("  Operand: %d\n", m.operand);
    printf("  testDivisibleBy: %d\n", m.testDivisibleBy);
    printf("  ifTestTrueThrowTo: %d\n", m.ifTestTrueThrowToMonkeyN);
    printf("  ifTestFalseThrowTo: %d\n", m.ifTestFalseThrowToMonkeyN);
}

void printMonkeys( Monkey* monkeys, int nMonkeys ) {
    for( int i = 0; i < nMonkeys; ++i ) {
        Monkey_print(monkeys[i], i);
    }
}

void parseLine( Monkey* monkeys, char* buffer, int* currentMonkey ) {
    if( *currentMonkey == MAXMONKEYS ) {
        printf("Too many monkey!\n");
        exit(1);
    }

    const char* markerMonkey = "Monkey";
    const char* markerItems = "  Starting items";
    const char* markerOperation = "  Operation";
    const char* markerTest = "  Test";
    const char* markerTestFull = "  Test: divisible by";
    const char* markerTestIfTrue = "    If true";
    const char* markerTestIfTrueFull = "    If true: throw to monkey";
    const char* markerTestIfFalse = "    If false";
    const char* markerTestIfFalseFull = "    If false: throw to monkey";

    Monkey* m = monkeys + *currentMonkey;

    if( strncmp(buffer, markerMonkey, strlen(markerMonkey)) == 0 ) {
        sscanf(buffer, "Monkey %d:", currentMonkey);
    }
    else if( strncmp(buffer, markerItems, strlen(markerItems)) == 0 ) {
        char* itemStr = strchr(buffer, ':') + 1;
        char* n = strtok(itemStr, ",");
        while( n != NULL ) {
            int it = atoi(n);
            Vec_push_back(&m->items, it);
            n = strtok(NULL, ",");
        }
    }
    else if( strncmp(buffer, markerOperation, strlen(markerOperation)) == 0 ) {
        char* opStr = strchr(buffer, '=') + 1;

        if( strchr(opStr, '*') != NULL ) {
            char* a = strtok(opStr, "*");
            char* b = strtok(NULL, "*");
            // char* b = opStr + 7;
            if( strcmp(b, " old") == 0 ) {
                m->operation = SQUARE;
            }
            else
            {
                m->operation = MULTIPLY;
                m->operand = atoi(b);
            }
        }
        else if( strchr(opStr, '+') != NULL ) {
            char* a = strtok(opStr, "+");
            char* b = strtok(NULL, "+");
            m->operation = ADD;
            m->operand = atoi(b);
        }
    }
    else if( strncmp(buffer, markerTest, strlen(markerTest)) == 0 ) {
        char* valStr = buffer + strlen(markerTestFull) + 1;
        m->testDivisibleBy = atoi(valStr);
    }
    else if( strncmp(buffer, markerTestIfTrue, strlen(markerTestIfTrue)) == 0 ) {
        char* valStr = buffer + strlen(markerTestIfTrueFull) + 1;
        m->ifTestTrueThrowToMonkeyN = atoi(valStr);
    }
    else if( strncmp(buffer, markerTestIfFalse, strlen(markerTestIfFalse)) == 0 ) {
        char* valStr = buffer + strlen(markerTestIfFalseFull) + 1;
        m->ifTestFalseThrowToMonkeyN = atoi(valStr);
    }
}


void simulateRound(Monkey* monkeys, int nMonkeys, bool canRelax, int blackMagic) {
    for( int mn = 0; mn < nMonkeys; ++mn ) {
        Monkey* m = monkeys + mn;

        while( m->items.size > 0 ) {
            int in = 0;
            // Monkey looky, get worried
            if (m->operation == MULTIPLY)
            {
                m->items.items[in] *= m->operand;
            }
            else if (m->operation == ADD)
            {
                m->items.items[in] += m->operand;
            }
            else if (m->operation == SQUARE)
            {
                m->items.items[in] *= m->items.items[in];
            }
            m->nTimesInspectedItems++;

            // Relax
            if(canRelax) {
                m->items.items[in] /= 3;
            }
            else {
                m->items.items[in] %= blackMagic;
            }

            // Monkey throw?
            if( m->items.items[in] % m->testDivisibleBy == 0 )
            {
                int thrown = m->items.items[in];
                Vec_erase(&m->items, in);
                Vec_push_back(&monkeys[m->ifTestTrueThrowToMonkeyN].items, thrown);
            }
            else
            {
                int thrown = m->items.items[in];
                Vec_erase(&m->items, in);
                Vec_push_back(&monkeys[m->ifTestFalseThrowToMonkeyN].items, thrown);
            }
        }
    }
}

void problems(FILE *f)
{
    char buffer[MAXLEN];
    memset(buffer, '\0', MAXLEN * sizeof(char));

    Monkey monkeys[MAXMONKEYS];
    for( size_t i = 0; i < MAXMONKEYS; ++i ) {
        monkeys[i] = Monkey_create();
    }
    int currentMonkey = 0;
    int maxMonkeyID = 0;

    while (fgets(buffer, MAXLEN, f))
    {
        char direction;
        int distance;
        size_t length = strlen(buffer);
        if( buffer[length - 1] == '\n' ) {
            buffer[length - 1] = '\0';
            length = strlen(buffer);
        }   

        parseLine(monkeys, buffer, &currentMonkey);
        maxMonkeyID = MAX(maxMonkeyID, currentMonkey);
    }

    int nMonkeys = maxMonkeyID + 1;
    printMonkeys(monkeys, nMonkeys);

    // Problem 2
    // Keep our worry levels in a valid range, but still divisible
    // by the test values using prime number theory magic.
    // This is just cruel leetcode maths hackery.
    // I have never needed to use this in the real world.
    // -_-
    int blackMagic = monkeys[0].testDivisibleBy;
    for( int i = 1; i < nMonkeys; ++i ) {
        blackMagic *= monkeys[i].testDivisibleBy;
    }

    int nRounds = 10000;
    for( int round = 0; round < nRounds; ++round ) {
        // Problem 1
        // simulateRound(monkeys, nMonkeys, true, 0);
        simulateRound(monkeys, nMonkeys, false, blackMagic);
        // printf("ROUND %d\n", round + 1);
        // printMonkeys(monkeys, nMonkeys);
        // for( int i = 0; i < nMonkeys; ++i ) {
        //     printf("Monkey %d inspected items %d times\n", i, monkeys[i].nTimesInspectedItems);
        // }
        // printf("\n\n");
    }

    for( int i = 0; i < nMonkeys; ++i ) {
        printf("Monkey %d inspected items %d times\n", i, monkeys[i].nTimesInspectedItems);
    }

    for( size_t i = 0; i < MAXMONKEYS; ++i ) {
        Monkey_destroy(monkeys[i]);
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("USAGE: %s input.txt\n", argv[0]);
        return 1;
    }

    const char *inputFilePath = argv[1];

    FILE *inputFile = fopen(inputFilePath, "r");
    if (!inputFile)
    {
        printf("Error: Failed to open file: %s", inputFilePath);
        return 1;
    }

    problems(inputFile);
    fclose(inputFile);

    return 0;
}
