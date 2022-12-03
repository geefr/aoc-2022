
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ITEMS 1024
#define MAX_INPUT_LINES 1024

typedef struct Rucksack {
  char compartment1[MAX_ITEMS]; 
  char compartment2[MAX_ITEMS];
} Rucksack;

/// Parse input line, split contents 50/50 between compartments
/// e.g jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL -> jqHRNqRjqzjGDLGL, rsFMfFZSrLrFZsSL
Rucksack parseLine(char* line) {
    Rucksack r = { .compartment1 = "", .compartment2 = "" };
    size_t length = strlen(line);
    if(line == NULL || length < 2) {
        printf("Invalid Input\n");
        return r;
    }

    if( line[length - 1] == '\n' ) {
        line[length - 1] = '\0';
        length = strlen(line);
    }
    
    if(length % 2 != 0) {
        printf("Invalid Input\n");
        return r;
    }
    if(length > MAX_ITEMS * 2) {
        printf("Uh Oh, line too long :'(");
        return r;
    }

    // printf("Line: %s Length: %ld\n", line, length);

    strncpy(r.compartment1, line, length / 2);
    strncpy(r.compartment2, line + length / 2, length / 2);
    r.compartment1[(length / 2) + 1] = '\0';
    r.compartment2[(length / 2) + 1] = '\0';

    // printf("%s : %s\n", r.compartment1, r.compartment2);

    return r;
}

/// Find the common item (character) between the 2 rucksack pockets
char findCommonItemInPockets(Rucksack r) {
    for( size_t a = 0; a < strlen(r.compartment1); ++a ) {
        for( size_t b = 0; b < strlen(r.compartment2); ++b ) {
            if(r.compartment1[a] == r.compartment2[b]) {
                return r.compartment1[a];
            }
        }
    }
    return '\0';
}

int itemScore(char item) {
    if( isupper(item) ) {
        return 27 + (item - 'A');
    } else {
        return 1 + (item - 'a');
    }
    return 0;
}

int problem1(FILE* f) {
    const int maxLen = MAX_ITEMS * 2;
	char buffer[maxLen];

    int v = 0;

	while(fgets(buffer, maxLen, f))
	{
        Rucksack r = parseLine(buffer);

        // Problem 1 - Sum of priorities of common item between pockets of each rucksack
        char commonItem = findCommonItemInPockets(r);
        // printf("Common Item: %c\n", commonItem);
        if(commonItem == '\0') continue;
        v += itemScore(commonItem);
	}

    return v;
}

char findCommonItemInRuckSacks(char** buffer, int numRucksacks) {
    // This is not efficient :)
    for( size_t testCharI = 0; testCharI < strlen(buffer[0]); ++testCharI ) {
        int foundIn = 0;
        char testChar = buffer[0][testCharI];

        for( int ruckSackI = 0; ruckSackI < numRucksacks; ++ruckSackI ) {
            for( size_t i = 0; i < strlen(buffer[ruckSackI]); ++i ) {
                if( buffer[ruckSackI][i] == testChar ) {
                    foundIn++;
                    break;
                }
            }
        }

        if( foundIn == numRucksacks ) {
            return testChar;
        }
    }
    return '\0';
}

int problem2(FILE* f) {
    const int maxLen = MAX_ITEMS * 2;

    int v = 0;

    const int elvesInGroup = 3;
    int elfInGroup = 0;

    char* buffer[elvesInGroup];
    for(int i = 0; i < elvesInGroup; ++i) {
        buffer[i] = calloc(maxLen, sizeof(char));
    }

	while(fgets(buffer[elfInGroup], maxLen, f))
	{
        // Each group of 3 lines is a group of elves
        // Find the common item between all of their rucksacks (regardless of pockets)
        // Sum the priorities of the common items across all rucksacks

        size_t length = strlen(buffer[elfInGroup]);
        if( buffer[elfInGroup][length - 1] == '\n' ) {
            buffer[elfInGroup][length - 1] = '\0';
        }

        // printf("%s\n", buffer[elfInGroup]);

        elfInGroup++;
        if( elfInGroup == elvesInGroup )
        {
            elfInGroup = 0; // Ring buffer

            char commonItem = findCommonItemInRuckSacks(buffer, elvesInGroup);
            // printf("Common: %c\n", commonItem);
            if(commonItem == '\0') continue;
            v += itemScore(commonItem);
        }
	}

    for(int i = 0; i < elvesInGroup; ++i) {
        free(buffer[i]);
    }

    return v;
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

    int v = problem2(inputFile);
	fclose(inputFile);

    printf("%d\n", v);

	return 0;
}

