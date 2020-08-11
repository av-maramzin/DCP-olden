/* For copyright information, see olden_v1.0/COPYRIGHT */

/******************************************************************* 
 *  Health.c : Model of the Columbian Health Care System           *
 *******************************************************************/ 

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "health.h"
#include <assert.h>

int  max_level;
long max_time;
long long seed;

void Village::grow(Fractal_t::Seed_t s) {

    if (this->element_info().level == 0) {
        return;
    } else {
        this->label = s;
        this->seed = s * (IQ + ::seed); 
        this->hosp.personnel = (int) pow(2, this->element_info().level-1);
        this->hosp.free_personnel = this->hosp.personnel;
        this->hosp.num_waiting_patients = 0;
        this->hosp.assess.forward = NULL;
        this->hosp.assess.back = NULL;
        this->hosp.assess.patient = NULL;  /* ADDED FOR LLVM [OLDEN BUGS!] */
        this->hosp.waiting.forward = NULL;
        this->hosp.waiting.back = NULL;
        this->hosp.waiting.patient = NULL; /* ADDED FOR LLVM [OLDEN BUGS!] */
        this->hosp.inside.forward = NULL;
        this->hosp.inside.back = NULL;
        this->hosp.inside.patient = NULL;  /* ADDED FOR LLVM [OLDEN BUGS!] */
        this->hosp.up.forward = NULL;      /* ADDED FOR LLVM [OLDEN BUGS!] */
        this->hosp.up.back = NULL;         /* ADDED FOR LLVM [OLDEN BUGS!] */
        this->hosp.up.patient = NULL;      /* ADDED FOR LLVM [OLDEN BUGS!] */
        this->returned.back = NULL;
        this->returned.forward = NULL;
        this->returned.patient = NULL;

        // DEBUG PRINT
        //std::cout << "V[l=" << this->element_info().level 
        //          << ",d=" << this->element_info().depth 
        //          << ",i=" << this->element_info().index << "]" << std::endl;

        //std::cout << "GROW: " << std::endl;
        //std::cout << "label=" << this->label << std::endl; 
        //std::cout << "seed=" << this->seed << std::endl; 
        //std::cout << "personnel=" << this->hosp.personnel << std::endl; 

        //std::cout << std::endl;

        return;
    }
}

Fractal_t::Seed_t Village::spawn_child_seed(int child_id) {
    return (4*this->label+child_id+1);
}

GetResults::Compute_t GetResults::operator()(Village& village, 
                                             const std::vector<Compute_t>& child_rets) 
{
    struct Results r1;
    
    r1.total_hosps = 0.0;
    r1.total_patients = 0.0;
    r1.total_time = 0.0;

    //if (village == nullptr) return r1;

    for (auto& child_ret : child_rets) {
        r1.total_hosps    += child_ret.total_hosps;
        r1.total_patients += child_ret.total_patients;
        r1.total_time     += child_ret.total_time;
    }

    struct List* list;
    list = village.returned.forward;
    while (list != nullptr) {
        
        struct Patient* p = list->patient;
        
        r1.total_hosps += (float)(p->hosps_visited);
        r1.total_time += (float)(p->time); 
        r1.total_patients += 1.0;
        
        list = list->forward;
    }

    return r1; 
}

void check_patients_inside(struct Village *village, struct List *list) 
{
  struct List            *l;
  struct Patient         *p;
  int                     t;
  
  while (list != NULL) {
    p = list->patient;
    t = p->time_left;
    p->time_left = t - 1;
    if (p->time_left == 0) {
      t = village->hosp.free_personnel;
      village->hosp.free_personnel = t+1;
      l = &(village->hosp.inside);
      removeList(l, p); 
      l = &(village->returned);
      addList(l, p); }    
    list = list->forward;       /* :) adt_pf detected */
  } 
}

struct List *check_patients_assess(struct Village *village, struct List *list) {
  float rand;
  struct Patient *p;
  struct List *up = NULL;
  long long s;
  int label, t;

  while (list != NULL) {
    p = list->patient;
    t = p->time_left;
    p->time_left = t - 1;
    label = village->label;
    if (p->time_left == 0) { 
      s = village->seed;
      rand = my_rand(s);
      village->seed = (long long)(rand * IM);
      label = village->label;
      if (rand > 0.1 || label == 0) {
        removeList(&village->hosp.assess, p);
        addList(&village->hosp.inside, p);
        p->time_left = 10;
        t = p->time;
        p->time = t + 10; 
      } else {
        t = village->hosp.free_personnel;
        village->hosp.free_personnel = t+1;
        
        removeList(&village->hosp.assess, p);
        up = &village->hosp.up;
        addList(up, p);
      }
    }
    
    list = list->forward;             /* :) adt_pf detected */
  }
  return up;
}

void check_patients_waiting(struct Village *village, struct List *list) {
  int i, t;
  struct Patient *p;
  
  while (list != NULL) {
    i = village->hosp.free_personnel;
    p = list->patient;
    if (i > 0) {
      t = village->hosp.free_personnel;
      village->hosp.free_personnel = t-1;
      p->time_left = 3;
      t = p->time;
      p->time = t + 3;

      removeList(&village->hosp.waiting, p);
      addList(&village->hosp.assess, p); }
    else {
      t = p->time;
      p->time = t + 1; }
    list = list->forward; }         /* :) adt_pf detected */
}

void put_in_hosp(struct Hosp *hosp, struct Patient *patient) {
  int t = patient->hosps_visited;

  patient->hosps_visited = t + 1;
  if (hosp->free_personnel > 0) {
    t = hosp->free_personnel;
    hosp->free_personnel = t-1;
    addList(&hosp->assess, patient); 
    patient->time_left = 3;
    t = patient->time;
    patient->time = t + 3; 
  } else {
    addList(&hosp->waiting, patient); 
  }
}

struct Patient *generate_patient(struct Village *village) 
{
  long long       s,newseed;
  struct Patient *patient;
  float rand;
  int label;
  
  s = village->seed;
  rand = my_rand(s);
  village->seed = (long long)(rand * IM);
  newseed = village->seed;
  label = village->label;
  if (rand > 0.666) {
    patient = (struct Patient *)malloc(sizeof(struct Patient));
    patient->hosps_visited = 0;
    patient->time = 0;
    patient->time_left = 0;
    patient->home_village = village; 
    return patient;
  }
  return NULL; 
}

int main(int argc, char *argv[]) 
{ 
    dealwithargs(argc, argv);
 
    // create Fractal framework and set its type
    Fractal_t fractal;
    fractal.set_type(Fractal_t::Type::unbalanced);
    fractal.set_impl_type(Fractal_t::ImplType::parallel);

    // grow the created framework
    Fractal_t::Seed_t s = 0;
    fractal.grow(max_level-1, s);

    chatting("\n\n    Columbian Health Care Simulator\n\n");
    chatting("Working...\n");
  
    Sim sim;
    for (int i = 0; i < max_time; i++) {
        if ((i % 50) == 0) chatting("%d\n", i);
        fractal.template compute<Sim::Compute_t>(sim);
    } 

    struct Results results;
    float total_time, total_patients, total_hosps;  
    
    GetResults get_results;
    
    printf("Getting Results\n");
    
    results = fractal.template compute<GetResults::Compute_t>(get_results);

    total_patients = results.total_patients;
    total_time = results.total_time;
    total_hosps = results.total_hosps;

    chatting("Done.\n\n");
    chatting("# of people treated:              %f people\n",
	   total_patients);
    chatting("Average length of stay:           %0.2f time units\n", 
	   total_time / total_patients);
    chatting("Average # of hospitals visited:   %f hospitals\n\n",
	   total_hosps / total_patients);

    return 0;
}

Sim::Compute_t Sim::operator()(Village& village, const std::vector<Sim::Compute_t>& child_rets)
{
    //if (village == nullptr) return nullptr;
   
    // put all patients sent from child hospitals into the local one
    struct Hosp* h = &village.hosp;
    for (struct List* child_ret : child_rets) {
        struct List* l = child_ret;
        if (l != nullptr) {
            l = l->forward;
            while (l != nullptr) {
	        put_in_hosp(h, l->patient);
	        removeList(child_ret, l->patient);
                l = l->forward;
            }
        }
    } 

    // do all modelling functionality
    check_patients_inside(&village, village.hosp.inside.forward);
    struct List* up = check_patients_assess(&village, village.hosp.assess.forward);
    check_patients_waiting(&village, village.hosp.waiting.forward);
  
    // generate new patients
    struct Patient* patient;
    if ( (patient = generate_patient(&village)) != nullptr) {  
        put_in_hosp(&(village.hosp), patient);
    }

    // DEBUG PRINT
    //std::cout << "V[l=" << village.element_info().level 
    //          << ",d=" << village.element_info().depth 
    //          << ",i=" << village.element_info().index << "]" << std::endl;

    //std::cout << "SIM: " << std::endl;
    //std::cout << "hosp personnel=" << village.hosp.personnel << std::endl; 
    //std::cout << "hosp free personnel=" << village.hosp.free_personnel << std::endl; 
    //std::cout << "num waiting patients=" << village.hosp.num_waiting_patients << std::endl; 

    //std::cout << std::endl;
    
    //std::cout << "LIST: hosp waiting" << std::endl;
    //dumpList(&village.hosp.waiting);
    
    //std::cout << "LIST: hosp assess" << std::endl;
    //dumpList(&village.hosp.assess); 
    
    //std::cout << "LIST: hosp inside" << std::endl;
    //dumpList(&village.hosp.inside);
    
    //std::cout << "LIST: hosp up" << std::endl;
    //dumpList(&village.hosp.up);
    
    //std::cout << "LIST: up" << std::endl;
    //dumpList(up);

    return up;
}

//
