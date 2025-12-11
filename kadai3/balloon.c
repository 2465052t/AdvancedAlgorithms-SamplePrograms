#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#define MAX_BALLOONS 40
#define MAX_T 100

/******************************************************************************
 * boardの情報
 *****************************************************************************/
/* 風船 struct / balloon struct */
typedef struct balloon {
	int time;
	int pos;
} balloon_t;

/*
 * 結果 struct
 * Result struct
 */
typedef struct result {
	int isOK;
	int num;
} result_t;

/* 風船の配列 Balloon array */
balloon_t balloons[MAX_BALLOONS];

/*
 * 次のｎ風船をファイルから読み込む
 * Parse the next n balloons from the file
 */
void setupBalloons(FILE * in, int n) {
  int i=0;
  for(i=0; i< n; i++) {
    int time, pos;
    fscanf(in, "%d %d", &pos, &time);
    balloons[i].time = time;
    balloons[i].pos = pos;
  }
}

int distance[MAX_BALLOONS+1][4];
/*
 * balloons配列に記録されている問題を解ける
 * solve the problem contained in the balloons array
 * 
 * parameter:
 * - n: 風船の数 number of balloons
 */
result_t solve(int n) {
  /* Initialization */
  for (int i = 0; i < n + 1; i++) {
    for (int j = 0; j < 4; j++) {
      distance[i][j] = INT_MAX;
    }
  }
  distance[0][0] = 0;
  int current_time = 0;
  int current_pos = 0;
  
  for (int b = 0; b < n; b++) { /* Loop on the balloons */
    balloon_t balloon = balloons[b];
    int time_available = balloon.time - current_time;

    int flag_reached_next_balloon = 0;

    for (int carrying = 0; carrying < 4; carrying++) { /* Loop on nb of balloons carried by robot */
      if (distance[b][carrying] == INT_MAX) {
        continue;
      }

      /* Check if going back home yields a shorter path */
      int time_via_home = current_pos * (1 + carrying) + balloon.pos;
      if (time_via_home <= time_available) {
        int distance_via_home = distance[b][carrying] + current_pos + balloon.pos;
        if (distance_via_home < distance[b+1][1]) {
          distance[b + 1][1] = distance_via_home;
          flag_reached_next_balloon = 1;
        }
      }
      
      if (carrying <= 2) {
        /* Check if can go directly */
        int dist_to_balloon = abs(balloon.pos - current_pos);
        int time_direct = dist_to_balloon * (1 + carrying);
        if (time_direct <= time_available) {
          int distance_direct = distance[b][carrying] + dist_to_balloon;
          distance[b + 1][carrying + 1] = distance_direct;
          flag_reached_next_balloon = 1;
        }
      }
    }
    
    /* Check if next ballon reached */
    if (!flag_reached_next_balloon) {
      return (result_t){ false, b + 1 };
    }

    /* Preparation for next iteration */
    current_time = balloon.time;
    current_pos = balloon.pos;
  }

  /* Find minimum distance to last balloon */
  int min_distance = INT_MAX;
  for (int carrying = 0; carrying < 4; carrying++) {
    if (distance[n][carrying] < min_distance) {
      min_distance = distance[n][carrying];
    }
  }
  /* Add distance back to home */
  min_distance += current_pos;
	result_t result = { true, min_distance };

	return result;
}

/*******
 * こちらで用意したmain 関数。
 * 問題準備してから、solve() をよび、正解比較もおこなう。
 */
int main(int argc, char* argv[]) {
    struct {
        char * in, * ans;
    } filepairs[] = { {"sample.in", "sample.ans"},
                      { "B.in", "B.ans"}};

    int i;
    for(i=0; i<2; i++) {
        char* inFile = filepairs[i].in;
        char* ansFile = filepairs[i].ans;
        FILE * in = fopen(inFile, "r");
        FILE * ansIn = fopen(ansFile, "r");
        if(in==NULL) {
          printf("Can't open file: %s.\n", inFile);
          return 0;
        }
        if(ansIn==NULL) {
          printf("Can't open file: %s.\n", ansFile);
          return 0;
        }
        int failCount = 0;
        int totalCount = 0;
        printf("Processing input: %s\n", inFile);
        while(1) {
          int n, ans; char isOK[10];
          result_t result;
          fscanf(in, "%d", &n);
          if(n==0) break;
          setupBalloons(in, n);
          result = solve(n);
          fscanf(ansIn, "%s %d", isOK, &ans);
          if((strcmp("OK", isOK) ==0) != result.isOK ||	ans != result.num) {
            failCount++;
            printf("You failed problem No. %d (result: %s %d, ans: %s, %d)\n",
                   totalCount, result.isOK? "OK": "NG", result.num, isOK, ans);
          }
          totalCount++;
        }
        if(failCount==0) {
            printf("Congratulation! You passed all problems (%d) in %s!\n", totalCount, inFile);
        } else {
            printf("Im sorry you missed %d/%d in %s!\n", failCount, totalCount, inFile);
            return 0;
        }
    }
    return 0;
}

