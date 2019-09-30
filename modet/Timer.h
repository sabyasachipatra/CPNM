// Copyright © 2019 Sabyasachi Patra, IIIT Bhubaneswar, All rights reserved.
#ifndef TIMER_H_
#define TIMER_H_

#include <sys/time.h>
#include <ctime>
#include <iostream>

#define MAX_TIMERS 1000  // Maximum number of timers

class Timer {
 private:
     static struct timeval cstart[MAX_TIMERS];  // Start times
     static struct timeval cend[MAX_TIMERS];  // End times

 public:
     static void start(int n);  // Start the clock of a timer
     static void stop(int n);  // Stop the clock of a timer
     static double elapsed(int n);  // Elapsed time of a timer
};
#endif  // TIMER_H_
