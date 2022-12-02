
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum Play {
  ROCK,
  PAPER,
  SCISSORS
} Play;

typedef enum Outcome {
    LOSS,
    DRAW,
    WIN,
} Outcome;

typedef struct RPS {
  Play opp;
  Play you;
  Outcome outcome;
} RPS;

const int SCORE_LOSS = 0;
const int SCORE_DRAW = 3;
const int SCORE_WIN = 6;

int winScore(RPS p) {
    if(p.opp == ROCK) {
        if(p.you == ROCK) return SCORE_DRAW;
        else if(p.you == PAPER) return SCORE_WIN;
        else if(p.you == SCISSORS) return SCORE_LOSS;
    }
    else if(p.opp == PAPER) {
        if(p.you == ROCK) return SCORE_LOSS;
        else if(p.you == PAPER) return SCORE_DRAW;
        else if(p.you == SCISSORS) return SCORE_WIN;
    }
    else if(p.opp == SCISSORS) {
        if(p.you == ROCK) return SCORE_WIN;
        else if(p.you == PAPER) return SCORE_LOSS;
        else if(p.you == SCISSORS) return SCORE_DRAW;
    }
    return 0;
}

int playScore(Play p) {
    switch(p) {
        case ROCK: return 1;
        case PAPER: return 2;
        case SCISSORS: return 3;
    }
    return 0;
}

Play playForWin( Outcome o, Play against ) {
    if(o == LOSS) {
        switch(against) {
            case ROCK: return SCISSORS;
            case PAPER: return ROCK;
            case SCISSORS: return PAPER;
        }
    } else if (o == DRAW ) {
        return against;
    } else if (o == WIN ) {
        switch(against) {
            case ROCK: return PAPER;
            case PAPER: return SCISSORS;
            case SCISSORS: return ROCK;
        }
    }
    return ROCK;
}

int calculateScore(RPS x) {
  return winScore(x) + playScore(x.you);
}

RPS parseLine(const char* line) {
  RPS p = {.opp = ROCK, .you = ROCK};
  if(line == NULL || strlen(line) < 3) {
    printf("Invalid Input\n");
    return p;
  }
  
  char opp = line[0];
  switch(opp) {
    case 'A': p.opp = ROCK; break;
    case 'B': p.opp = PAPER; break;
    case 'C': p.opp = SCISSORS; break;
  }

/* Problem 1
  char you = line[2];
  switch(you) {
    case 'X': p.you = ROCK; break;
    case 'Y': p.you = PAPER; break;
    case 'Z': p.you = SCISSORS; break;
  }
  */

  char o = line[2];
  switch(o) {
    case 'X': p.outcome = LOSS; break;
    case 'Y': p.outcome = DRAW; break;
    case 'Z': p.outcome = WIN; break;
  }

  return p;
}

int calculateStrategyGuideScore(FILE* f) {
    const int maxLen = 10;
	char buffer[maxLen];

    int score = 0;

	while(fgets(buffer, maxLen, f))
	{
        RPS p = parseLine(buffer);

        /* Problem 1
        score += calculateScore(p); */

        // Problem 2
        p.you = playForWin(p.outcome, p.opp);
        score += calculateScore(p);
	}

    return score;
}

int main(int argc, char** argv) {
	if( argc < 2 )
	{
		printf("USAGE: %s input.txt\n", argv[0]);
		return 1;
	}

    /*
     * Input:
     * [Opponents play] [What you should play]
     * Opponent:
     * A: Rock
     * B: Paper
     * C: Scissors
     * 
     * Your play:
     * X: Rock
     * Y: Paper
     * Z: Scissors
     * 
     * Scoring:
     * Score for your play + win score
     * Rock: 1
     * Paper: 2
     * Scissors: 3
     * 
     * loss: 0
     * draw: 3
     * win: 6
     * 
     * Puzzle 1: Calculate your total score, following strategy guide (input)
     * 
     * Puzzle 2: 2nd column now what should happen
     * X: Lose
     * Y: Draw
     * Z: Win
     */

	const char* inputFilePath = argv[1];

	FILE* inputFile = fopen(inputFilePath, "r");
	if( !inputFile )
	{
		printf("Error: Failed to open file: %s", inputFilePath);
		return 1;
	}

    int score = calculateStrategyGuideScore(inputFile);
	fclose(inputFile);

    printf("%d\n", score);

	return 0;
}

