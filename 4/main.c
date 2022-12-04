
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXLEN 128

// Range of task numbers, inclusive
typedef struct Range {
  int minimum;
  int maximum;
} Range;

typedef struct TaskPair {
    Range a;
    Range b;
} TaskPair;

int splitString(const char* input, char sep, char* a, char* b) {
  char* found = strchr(input, sep);
  if( found == NULL ) {
    return 1;
  }
  int sepI = found - input;
  // Bounds check? What bounds check? >:D

  strncpy(a, input, sepI);
  a[sepI] = '\0';
  int bLen = strlen(input) - sepI;
  strncpy(b, input + sepI + 1, bLen);

  // printf("split %s = %s, %s sepI=%d\n", input, a, b, sepI);
  return 0;
}

TaskPair parseLine(char* line) {
    TaskPair p;
    p.a.minimum = 0;
    p.a.maximum = 0;
    p.b.minimum = 0;
    p.b.maximum = 0;

    size_t length = strlen(line);
    if(line == NULL || length < 3) {
        printf("Invalid Input\n");
        return p;
    }

    if( line[length - 1] == '\n' ) {
        line[length - 1] = '\0';
        length = strlen(line);
    }

    char tasksA[MAXLEN];
    char tasksB[MAXLEN];
    if( splitString(line, ',', tasksA, tasksB) != 0 ) {
        printf("Failed to split line\n");
        return p;
    }

    char aMinStr[MAXLEN];
    char aMaxStr[MAXLEN];
    if( splitString(tasksA, '-', aMinStr, aMaxStr) != 0 ) {
        printf("Failed to split tasksA\n");
        return p;
    }
    char bMinStr[MAXLEN];
    char bMaxStr[MAXLEN];
    if( splitString(tasksB, '-', bMinStr, bMaxStr) != 0 ) {
        printf("Failed to split tasksB\n");
        return p;
    }

    p.a.minimum = atoi(aMinStr);
    p.a.maximum = atoi(aMaxStr);
    p.b.minimum = atoi(bMinStr);
    p.b.maximum = atoi(bMaxStr);

    return p;
}

int problem1(FILE* f) {
   
	char buffer[MAXLEN];

    int c = 0;

	while(fgets(buffer, MAXLEN, f))
	{
        TaskPair p = parseLine(buffer);

        // If one task range fully contains the other
        if( ((p.a.minimum <= p.b.minimum) && (p.a.maximum >= p.b.maximum)) ||
            ((p.b.minimum <= p.a.minimum) && (p.b.maximum >= p.a.maximum))) {
                ++c;
            }
	}

    return c;
}

int problem2(FILE* f) {
   
	char buffer[MAXLEN];

    int c = 0;

	while(fgets(buffer, MAXLEN, f))
	{
        TaskPair p = parseLine(buffer);

        // If ranges overlap at all
        // if( !(p.a.maximum < p.b.minimum || p.b.minimum > p.a.maximum) ) ++c;

        if( p.a.maximum < p.b.minimum ||
            p.a.minimum > p.b.maximum ||
            p.b.maximum < p.a.minimum ||
            p.b.minimum > p.b.maximum ) {

            } else {
                ++c;
            }
            
	}

    return c;
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

    printf("%d\n", problem2(inputFile));
    
	fclose(inputFile);

	return 0;
}

