/* For copyright information, see olden_v1.0/COPYRIGHT */

/****************************************************************
 * health.h: header file for health.c                           *
 ****************************************************************/

#ifndef _HEALTH
#define _HEALTH

#include <cstdio>
#include <cstdlib>

#include <vector>

#include "Fractal.h"

using namespace abstract;

#define chatting printf

#define IA 16807
#define IM 2147483647
#define AM (1.0 / IM)
#define IQ 127773
#define IR 2836
#define MASK 123459876

extern int  max_level;
extern long max_time;
extern long long seed;

struct Results {
  float                   total_patients;
  float                   total_time;
  float                   total_hosps; 
};

struct Patient {
  int                    hosps_visited;
  int                    time;
  int                    time_left;
  struct Village         *home_village;
};

struct List {
  struct List            *forward;
  struct Patient         *patient;
  struct List            *back;
};

struct Hosp {
  int                    personnel; 
  int                    free_personnel; 
  int                    num_waiting_patients; 
  struct List            waiting; 
  struct List            assess; 
  struct List            inside;
  struct List            up;
};
 
struct Village {
    
    // [*] Remove all pointer-chained links out of 
    // a fractal element and keep them inside 
    // enclosing Fractal class
    //struct Village         *forward[4];
    //struct Village         *back;
    
    // [*] Fractal element body to be processed
    struct List            returned;
    struct Hosp            hosp;   
    int                    label;
    long long              seed;
};
using FractalElement_t = struct Village;

void dealwithargs(int argc, char *argv[]);

// [*] Build the Fractal of Villages
// Call Fractal::grow() instead
//struct Village *alloc_tree(int level, int label, struct Village *back);

// [*] Apply processing functions to the built Fractal
// Functions to be passed into Fractal::apply()
// These functions take Fractal element and process it

//struct List* sim(struct Village *village);
using FractalApply_sim_ret_t = struct List*;
using FractalApply_sim_func_t = FractalApply_sim_ret_t (*)(FractalElement_t* village,
                                                              const std::vector<FractalApply_sim_ret_t>& child_rets);
FractalApply_sim_ret_t sim(FractalElement_t* village, const std::vector<FractalApply_sim_ret_t>& child_rets);


//struct Results get_results(struct Village *village);
using FractalApply_get_results_ret_t = struct Results;
using FractalApply_get_results_func_t = FractalApply_get_results_ret_t (*)(FractalElement_t* village,
                                                                        const std::vector<FractalApply_get_results_ret_t>& child_rets);
FractalApply_get_results_ret_t get_results(FractalElement_t* village, const std::vector<FractalApply_get_results_ret_t>& child_rets);

// [*] Internal functions to be called from enclosing
// application functions

float my_rand(long long idum);
struct Patient *generate_patient(struct Village *village);
void put_in_hosp(struct Hosp *hosp, struct Patient *patient);
void addList(struct List *list, struct Patient *patient);
void removeList(struct List *list, struct Patient *patient);
void check_patients_inside(struct Village *village, struct List *list);
struct List *check_patients_assess(struct Village *village, struct List *list);
void check_patients_waiting(struct Village *village, struct List *list);
float get_num_people(struct Village *village);
float get_total_time(struct Village *village);
float get_total_hosps(struct Village *village);

#endif
