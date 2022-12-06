
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXLEN 128

void QPush(char* q, int qLen, char c) {
  for( int i = 1; i < qLen; ++i ) q[i-1] = q[i];
  q[qLen-1] = c;
  q[qLen] = '\0';
  // printf("Q: %s\n", q);
}
int allDifferent(char* s) {
    for( size_t i = 0; i < strlen(s); ++i ) {
        for( size_t j = 0; j < strlen(s); ++j) {
            if( i != j && s[i] == s[j] ) {
                return 0;
            }
        }
    }
    return 1;
}

int problem1(FILE* f) {
   
	char buffer[MAXLEN];
    memset(buffer, '\0', MAXLEN * sizeof(char));

    size_t qLen = 4;
    char q[qLen + 1];

    int x = 0;
	while(fgets(buffer, MAXLEN, f))
	{
        // printf("line: %s\n", buffer);
        for( size_t i = 0; i < strlen(buffer); ++i ) {
            ++x;
            QPush(q, qLen, buffer[i]);
            if( strlen(q) == qLen && allDifferent(q) )
            {
                return x;
            }
        }
	}

    return -1;
}

int problem2(FILE* f) {
   
	char buffer[MAXLEN];
    memset(buffer, '\0', MAXLEN * sizeof(char));

    size_t qLen = 14;
    char q[qLen + 1];

    int x = 0;
	while(fgets(buffer, MAXLEN, f))
	{
        // printf("line: %s\n", buffer);
        for( size_t i = 0; i < strlen(buffer); ++i ) {
            ++x;
            QPush(q, qLen, buffer[i]);
            if( strlen(q) == qLen && allDifferent(q) )
            {
                return x;
            }
        }
	}

    return -1;
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

    printf("Problem 1: %d\n", problem1(inputFile));
    fseek(inputFile, 0, SEEK_SET);
    printf("Problem 2: %d\n", problem2(inputFile));
    
	fclose(inputFile);

	return 0;
}

