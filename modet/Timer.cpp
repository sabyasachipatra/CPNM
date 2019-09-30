// Copyright © 2019 Sabyasachi Patra, IIIT Bhubaneswar, All rights reserved.
#include "Timer.h"

// Static variables
struct timeval Timer::cstart[MAX_TIMERS];
struct timeval Timer::cend[MAX_TIMERS];

// Stop the clock of a timer
void Timer::start(int n) {
    if (n >= 0 && n < MAX_TIMERS)
        gettimeofday(&cstart[n], NULL);
}

// Elapsed time of a timer
void Timer::stop(int n) {
    if (n >= 0 && n < MAX_TIMERS)
        gettimeofday(&cend[n], NULL);
}

double Timer::elapsed(int n) {
    if (n >= 0 && n < MAX_TIMERS)
        return (cend[n].tv_sec - cstart[n].tv_sec) +
            static_cast<double>(cend[n].tv_usec - cstart[n].tv_usec)/1000000.0;
    else
        return 0;
}
