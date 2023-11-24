#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "headers/list.h"
#include "headers/timer.h"

#define TEST_FROM 7
#define TEST_UNTIL 16
#define SEARCH_ATTEMPTS 10000
int main() {
	srand(time(NULL));
	t_list *lst;
	double lvl10_time,
			multilevel_time;
	int max_value,
		i, j;
	FILE *results_file = fopen("times.csv", "w");
	fprintf(results_file, "N,Duration for %d searches with Level 0 search (ms),Duration for %d searches with Multilevel search (ms)\n", SEARCH_ATTEMPTS, SEARCH_ATTEMPTS);
	for (i = TEST_FROM; i <= TEST_UNTIL; i++) {
		printf("Timing for n = %d\n", i);
		lst = create_sample_list(i);
		max_value = pow(2, i) - 1;
		startTimer();
		for (j = 0; j < SEARCH_ATTEMPTS; j++) {
			search_list_level(lst, 0, rand() % max_value);
		}
		stopTimer();
		lvl10_time = getMilliseconds();
		startTimer();
		for (j = 0; j < SEARCH_ATTEMPTS; j++) {
			search_list(lst, rand() % max_value);
		}
		stopTimer();
		multilevel_time = getMilliseconds();
		fprintf(results_file, "%d,%f,%f\n", i, lvl10_time, multilevel_time);
	}
	fclose(results_file);
	return 0;
}