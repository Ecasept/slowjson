#include "jsonperf.h"
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static bool is_number(const char *value) {
	if (value == NULL || *value == '\0') return false;
	for (const unsigned char *p = (const unsigned char *)value; *p != '\0'; p++) {
		if (!isdigit(*p)) return false;
	}
	return true;
}

int main(int argc, char **argv) {
	const char *benchmark_name = NULL;
	size_t iterations = 1;
	size_t warmup_iterations = 10;

	if (argc >= 2) {
		if (is_number(argv[1])) {
			iterations = (size_t)atoi(argv[1]);
			if (argc >= 3 && is_number(argv[2])) {
				warmup_iterations = (size_t)atoi(argv[2]);
				if (argc >= 4) benchmark_name = argv[3];
			} else if (argc >= 3) {
				benchmark_name = argv[2];
				if (argc >= 4 && is_number(argv[3])) {
					warmup_iterations = (size_t)atoi(argv[3]);
				}
			}
		} else {
			benchmark_name = argv[1];
			if (argc >= 3 && is_number(argv[2])) {
				iterations = (size_t)atoi(argv[2]);
				if (argc >= 4 && is_number(argv[3])) {
					warmup_iterations = (size_t)atoi(argv[3]);
				}
			}
		}
	}

	if (benchmark_name != NULL && strcmp(benchmark_name, "all") == 0) {
		benchmark_name = NULL;
	}

	return run_jsonperf(iterations, warmup_iterations, benchmark_name);
}
