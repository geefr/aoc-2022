
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXLEN 128

typedef struct Stack {
  char* items;
  size_t size;
} Stack;
Stack Stack_create() { Stack s = {.items = NULL, .size = 0}; return s; }
void Stack_destroy(Stack* s) {free(s->items);}
void Stack_push(Stack* s, char v) {
    char* oldItems = s->items;
    size_t oldSize = s->size;
    s->size++;
    s->items = malloc(s->size * sizeof(char));
    if( oldItems ) {
        memcpy(s->items, oldItems, oldSize * sizeof(char));
        free(oldItems);
    }
    s->items[s->size - 1] = v;
}
char Stack_pop(Stack* s) {
    if( s->size == 0 ) return '\0';
    char v = s->items[s->size - 1];
    char* oldItems = s->items;
    s->size--;
    if( s->size > 0 ) {
        s->items = malloc(s->size * sizeof(char));
        memcpy(s->items, oldItems, s->size * sizeof(char));
    } else {
        s->items = NULL;
    }
    free(oldItems);
    return v;
}
void Stack_reverse(Stack* s) {
    if( s->size == 0 ) return;
    char* oldItems = s->items;
    s->items = malloc(s->size * sizeof(char));
    for( int i = s->size - 1, j = 0; i >= 0; --i, ++j ) {
        s->items[j] = oldItems[i];
    }
    free(oldItems);
}

void parseLineStack(char* line, Stack* stacks, size_t numStacks) {
    //    [D]    
    //[N] [C]    
    //[Z] [M] [P]
    // 1   2   3 
    //
    //move blah to blah
    if(line == NULL) {
        printf("Invalid Input\n");
        return;
    }
    if( strchr(line, '[') == NULL ) {
        return;
    }

    // printf("Line: %s\n", line);

    for( size_t stackNum = 0; stackNum < numStacks; ++stackNum ) {
        int i = 1 + (4 * stackNum);
        char c = line[i];
        if( c != ' ' ) {
            // printf("Push %lu %c\n", stackNum, c);
            Stack_push(stacks + stackNum, c);
        }
    }
}

void parseLineAndMoveCrateProblem1(char* line, Stack* stacks, size_t numStacks) {
    // move 1 from 2 to 1
    // move 3 from 1 to 3
    // move 2 from 2 to 1
    // move 1 from 1 to 2
    if(line == NULL) {
        printf("Invalid Input\n");
        return;
    }
    if( strchr(line, '[') != NULL ) {
        return;
    }

    int numToMove, from, to;
    if( sscanf(line, "move %d from %d to %d", &numToMove, &from, &to) != 3 ) {
        return;
    }
    --from;
    --to;

    // Crate mover 9000 - One at a time
    for( int i = 0; i < numToMove; ++i ) {
        char v = Stack_pop(stacks + from);
        Stack_push(stacks + to, v);
    }
}

void parseLineAndMoveCrateProblem2(char* line, Stack* stacks, size_t numStacks) {
    // move 1 from 2 to 1
    // move 3 from 1 to 3
    // move 2 from 2 to 1
    // move 1 from 1 to 2
    if(line == NULL) {
        printf("Invalid Input\n");
        return;
    }
    if( strchr(line, '[') != NULL ) {
        return;
    }

    int numToMove, from, to;
    if( sscanf(line, "move %d from %d to %d", &numToMove, &from, &to) != 3 ) {
        return;
    }
    --from;
    --to;

    // Crate mover 9001 - Moving multiple preserves order
    // >:) But we can fake that with a temporary stack
    Stack temp = Stack_create();
    if( numToMove == 1 ) {
        char v = Stack_pop(stacks + from);
        Stack_push(stacks + to, v);
    } else {
        for( int i = 0; i < numToMove; ++i ) {
            char v = Stack_pop(stacks + from);
            Stack_push(&temp, v);
        }
        for( int i = 0; i < numToMove; ++i ) {
            char v = Stack_pop(&temp);
            Stack_push(stacks + to, v);
        }
    }
}

int problem1(FILE* f) {
   
	char buffer[MAXLEN];
    memset(buffer, '\0', MAXLEN * sizeof(char));

    int numStacks = 9;
    Stack stacks[numStacks];
    for( int i = 0; i < numStacks; ++i ) stacks[i] = Stack_create();

	while(fgets(buffer, MAXLEN, f))
	{
        parseLineStack(buffer, stacks, numStacks);
	}
    // But the file is listed top-down..
    for( int i = 0; i < numStacks; ++i ) {
        Stack_reverse(stacks + i);
    }
    // Then move stuff around
    fseek(f, 0, SEEK_SET);
    while(fgets(buffer, MAXLEN, f))
	{
        parseLineAndMoveCrateProblem1(buffer, stacks, numStacks);
	}

    printf("Top of stacks (problem1): ");
    for( int i = 0; i < numStacks; ++i ) {
        printf("%c", stacks[i].items[stacks[i].size - 1]);
    }
    printf("\n");

    for( int i = 0; i < numStacks; ++i ) Stack_destroy(stacks + i);

    return 1;
}

int problem2(FILE* f) {
   
	char buffer[MAXLEN];
    memset(buffer, '\0', MAXLEN * sizeof(char));

    int numStacks = 9;
    Stack stacks[numStacks];
    for( int i = 0; i < numStacks; ++i ) stacks[i] = Stack_create();

	while(fgets(buffer, MAXLEN, f))
	{
        parseLineStack(buffer, stacks, numStacks);
	}
    // But the file is listed top-down..
    for( int i = 0; i < numStacks; ++i ) {
        Stack_reverse(stacks + i);
    }
    // Then move stuff around
    fseek(f, 0, SEEK_SET);
    while(fgets(buffer, MAXLEN, f))
	{
        parseLineAndMoveCrateProblem2(buffer, stacks, numStacks);
	}

    printf("Top of stacks (problem2): ");
    for( int i = 0; i < numStacks; ++i ) {
        printf("%c", stacks[i].items[stacks[i].size - 1]);
    }
    printf("\n");

    for( int i = 0; i < numStacks; ++i ) Stack_destroy(stacks + i);

    return 1;
}

int main(int argc, char** argv) {
	if( argc < 2 )
	{
		printf("USAGE: %s input.txt\n", argv[0]);
		return 1;
	}

	const char* inputFilePath = argv[1];

	FILE* inputFile = fopen(inputFilePath, "r");
	if( !inputFile )
	{
		printf("Error: Failed to open file: %s", inputFilePath);
		return 1;
	}

    // problem1(inputFile);
    problem2(inputFile);
    
	fclose(inputFile);

	return 0;
}

