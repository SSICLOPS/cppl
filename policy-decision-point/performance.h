#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void performance_start_benchmark();
void performance_stop_benchmark(const char *label);
int timespec_diff(struct timespec *result, struct timespec *start, struct timespec *end);
int timeval_diff (struct timeval *result, struct timeval *start, struct timeval *end);
double time_diff(struct timespec *start, struct timespec *stop);
