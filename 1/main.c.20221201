
#include <stdio.h>
#include <stdlib.h>

/**
 * Input is for each elf, a list of calories of items they own
 * Elves separated by a blank line
 */
unsigned int findElfWithMostFood(FILE* f, int* outNumCalories) {
	const int maxLen = 1024;
	char buffer[maxLen];

	int fattestElf = -1;
	int currentElf = 0;
	unsigned int currentElfCalories = 0;
	unsigned int highestCalories = 0;

	while(fgets(buffer, maxLen, f))
	{
		if( buffer[0] == '\0' || buffer[0] == '\n' )
		{
			if( currentElfCalories > highestCalories )
			{
				highestCalories = currentElfCalories;
				fattestElf = currentElf;
			}

			currentElf++;
			currentElfCalories = 0;
		}
		else
		{
			int calories = atoi(buffer); // 0 on failure, but that's correct in this case ;)
			currentElfCalories += calories;
		}
	}

	if(outNumCalories)
	{
	  *outNumCalories = highestCalories;
	}
	return fattestElf;
}

typedef struct ElfCalories
{
	int elfIndex;
	int numCalories;
} ElfCalories;
ElfCalories* ElfCalories_array_create(int n) {
    ElfCalories* elves = calloc(n, sizeof(ElfCalories));
	if( !elves ) return NULL;

	for(int i = 0; i < n; ++i ) {
		elves[i].elfIndex = 0;
		elves[i].numCalories = 0;
	}

	return elves;
}
void ElfCalories_array_destroy(ElfCalories* elves) { 
	free(elves);
}
void ElfCalories_array_insert(int n, ElfCalories* elves, ElfCalories elf) {
	// Find the correct place, based on calorie count
	int insertIndex = -1;
	for( int i = n - 1; i >= 0; --i ) {
		if( elf.numCalories > elves[i].numCalories ) insertIndex = i;
	}

	if( insertIndex == -1 || insertIndex >= n )
	{
		return; // Doesn't belong
	}
	// Push any elves down the list if needed
	if( insertIndex < n )
	{
		for( int i = insertIndex; i < n - 1; ++i )
		{
			elves[i+1].elfIndex = elves[i].elfIndex;
			elves[i+1].numCalories = elves[i].numCalories;
		}
		elves[insertIndex].elfIndex = elf.elfIndex;
		elves[insertIndex].numCalories = elf.numCalories;
	}
}

ElfCalories* findTopNElvesWithMostFood(FILE* f, int n) {
	const int maxLen = 1024;
	char buffer[maxLen];

	ElfCalories* fatElves = ElfCalories_array_create(n);

	ElfCalories currentElf = { .elfIndex = 0, .numCalories = 0 };

	while(fgets(buffer, maxLen, f))
	{
		if( buffer[0] == '\0' || buffer[0] == '\n' )
		{
			ElfCalories_array_insert(n, fatElves, currentElf);

			currentElf.elfIndex++;
			currentElf.numCalories = 0;
		}
		else
		{
			int calories = atoi(buffer); // 0 on failure, but that's correct in this case ;)
			currentElf.numCalories += calories;
		}
	}

	return fatElves;
}


int main(int argc, char** argv) {
	if( argc < 3 )
	{
		printf("USAGE: %s input.txt num_elves_to_find\n", argv[0]);
		return 1;
	}

	const char* inputFilePath = argv[1];
	int maxElves = atoi(argv[2]);

	FILE* inputFile = fopen(inputFilePath, "r");
	if( !inputFile )
	{
		printf("Error: Failed to open file: %s", inputFilePath);
		return 1;
	}
	if( maxElves <= 0 )
	{
		printf("Error: Asked for an invalid number of elves\n");
		return 1;
	}

	ElfCalories* fatElves = findTopNElvesWithMostFood(inputFile, maxElves);
	if( !fatElves )
	{
		printf("Error: Failed to find fat elves\n");
		return 1;
	}
	fclose(inputFile);

	// TODO: Edge case when n > 3, Something in the insert function :)
	int totalFatElfCalories = 0;
	printf("Fat Elves:\n");
	for( int i = 0; i < maxElves; ++i )
	{
		printf("%d : %d\n", fatElves[i].elfIndex, fatElves[i].numCalories);
		totalFatElfCalories += fatElves[i].numCalories;
	}
	printf("Total Calories carried by fat elves: %d\n", totalFatElfCalories);
	ElfCalories_array_destroy(fatElves);
	fatElves = NULL;

	return 0;
}

