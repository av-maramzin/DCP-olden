#include <cstdlib>
#include <ctime>
#include <cmath>

#include <iostream>
#include <chrono>
using namespace std;

#include "power_original.h"
#include "power.h"
#include <fenv.h>
#include <signal.h>

void handler(int signum) {
    //cout << "OK!";
}

int main(int argc, char *argv[]) {
    
    cout << "====================================================================" << endl;
    cout << "= Olden power (power pricing problem) benchmark feasibility study! =" << endl;
    cout << "====================================================================" << endl;
    
    std::srand(std::time(0)); // use current time as a seed for random generator

    //feenableexcept(FE_ALL_EXCEPT & ~FE_INEXACT);

    //signal(SIGFPE, handler);

    cout << "[1] Original power pricing problem" << endl;
    auto start_time = chrono::high_resolution_clock::now();
    power_pricing_problem_original();
    auto end_time = chrono::high_resolution_clock::now();
    auto powerTimeOriginal = end_time - start_time;

    cout << "[2] My power pricing problem" << endl;
    start_time = chrono::high_resolution_clock::now();
    power_pricing_problem();
    end_time = chrono::high_resolution_clock::now();
    auto powerTime = end_time - start_time;

    cout << "======================================" << endl;
    cout << "= Results (running times in seconds) =" << endl;
    cout << "======================================" << endl;
    cout << "\tpower original: ";
    cout << (double)chrono::duration_cast<chrono::microseconds>(powerTimeOriginal).count()/1000000 << " seconds.\n";
    cout << "\tpower: ";
    cout << (double)chrono::duration_cast<chrono::microseconds>(powerTime).count()/1000000 << " seconds.\n";

    return 0;
}
