/* For copyright information, see olden_v1.0/COPYRIGHT */

/****************************************************************
 * health.h: header file for health.c                           *
 ****************************************************************/

#ifndef _HEALTH
#define _HEALTH

#include <stdio.h>
#include <stdlib.h>

#define chatting printf

#define IA 16807
#define IM 2147483647
#define AM (1.0 / IM)
#define IQ 127773
#define IR 2836
#define MASK 123459876

#include "Fractal_dynamic.h"

using namespace abstract;

extern int max_level;
extern long max_time;
extern long long seed;

struct Results {
    float total_patients;
    float total_time;
    float total_hosps; 
};

struct Patient {
    int hosps_visited;
    int time;
    int time_left;
    struct Village* home_village;
};

struct List {
    struct List* forward;
    struct Patient* patient;
    struct List* back;
};

struct Hosp {
    int personnel; 
    int free_personnel; 
    int num_waiting_patients; 
    struct List waiting; 
    struct List assess; 
    struct List inside;
    struct List up;
};
 
using Fractal_Element_t = class Village;
using Fractal_Seed_t = int;
const size_t Fractal_Arity = 4;
using Fractal_t = Fractal<Fractal_Element_t,Fractal_Seed_t,Fractal_Arity>;

class Village : public Fractal_t::Element {

    public:
        
        Village(Fractal_t::ElementInfo info) : Fractal_t::Element(info) {}
        ~Village() {}

        void grow(Fractal_t::Seed_t seed) override;
        Fractal_t::Seed_t spawn_child_seed(int child_id) override;

        struct List returned;
        struct Hosp hosp;
        int label;
        long long seed;
};

class GetResults {
    public:
        using ComputeType = struct Results;
        ComputeType operator()(Village* village, const std::vector<ComputeType>& child_rets);
};

class Sim {
    public:
        using ComputeType = struct List*;
        ComputeType operator()(Village* village, const std::vector<ComputeType>& child_rets);
};

void dealwithargs(int argc, char* argv[]);
float my_rand(long long idum);
struct Patient* generate_patient(struct Village* village);
void put_in_hosp(struct Hosp* hosp, struct Patient* patient);
void addList(struct List *list, struct Patient *patient);
void removeList(struct List *list, struct Patient *patient);
void check_patients_inside(struct Village *village, struct List *list);
struct List *check_patients_assess(struct Village *village, struct List *list);
void check_patients_waiting(struct Village *village, struct List *list);
float get_num_people(struct Village *village);
float get_total_time(struct Village *village);
float get_total_hosps(struct Village *village);

#endif

//
