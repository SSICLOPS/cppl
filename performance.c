// Copyright 2015-2018 RWTH Aachen University
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "performance.h"

// select the clock used to retrieve time (see CLOCK_GETRES(2))
#define PERF_CLK_ID     CLOCK_MONOTONIC_RAW

/** imespec time structs for one measurement */
struct timespec timeBefore;
struct timespec timeAfter;

void performance_start_benchmark()  {
    clock_gettime(PERF_CLK_ID, &timeBefore);
}

void performance_stop_benchmark(const char *label)  {
    clock_gettime(PERF_CLK_ID, &timeAfter);
    double timeDiff = time_diff(&timeBefore, &timeAfter);
    printf("%s: %f\n", label, timeDiff);
}

/**
 * original from http://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/
 * Modified to use pointers and use return to display negative values
 *
 * @return 1 if the result is a negative time, 0 otherwise
 */
int timespec_diff(struct timespec *result, struct timespec *start, struct timespec *end)
{
  int ret = 0;
  if ( end->tv_nsec < start->tv_nsec ) {
    result->tv_sec = end->tv_sec - start->tv_sec - 1;
    result->tv_nsec = 1000000000 + end->tv_nsec - start->tv_nsec;
    if ( (end->tv_sec - 1) < start->tv_sec ) {
      ret = 1; // negative time
    }
  } else {
    result->tv_sec = end->tv_sec - start->tv_sec;
    result->tv_nsec = end->tv_nsec - start->tv_nsec;
    if (end->tv_sec < start->tv_sec) {
      ret = 1; // negative time
    }
  }
  return ret;
}

/**
 * from http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
 */
int timeval_diff (struct timeval *result, struct timeval *start, struct timeval *end)
{
 /* Perform the carry for the later subtraction by updating y. */
 if (end->tv_usec < start->tv_usec) {
   int nsec = (start->tv_usec - end->tv_usec) / 1000000 + 1;
   start->tv_usec -= 1000000 * nsec;
   start->tv_sec += nsec;
 }
 if (end->tv_usec - start->tv_usec > 1000000) {
   int nsec = (end->tv_usec - start->tv_usec) / 1000000;
   start->tv_usec += 1000000 * nsec;
   start->tv_sec -= nsec;
 }

 /* Compute the time remaining to wait.
    tv_usec is certainly positive. */
 result->tv_sec = end->tv_sec - start->tv_sec;
 result->tv_usec = end->tv_usec - start->tv_usec;

 /* Return 1 if result is negative. */
 return end->tv_sec < start->tv_sec;
}


double time_diff(struct timespec *start, struct timespec *stop) {
  struct timespec result;
  if (timespec_diff(&result, start, stop)) {
    fprintf(stderr, "WARNING: negative time\n");
  }
  return    (double) (result.tv_sec + (result.tv_nsec / 1000000000.0));
}
