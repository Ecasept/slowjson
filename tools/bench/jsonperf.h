#pragma once
#include <stddef.h>
int run_jsonperf(size_t iterations, size_t warmup_iterations,
		const char *benchmark_name);
