#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define POPSIZE 100
#define C1 0.5176
#define C2 116.0
#define C3 0.4
#define C4 5.0
#define C5 21.0
#define C6 0.0068
#define TURBINE_RADIUS 0.87

float RATIOS[]={ 0.279,
                 0.316,
                 0.360,
                 0.409,
                 0.464,
                 0.528,
                 0.6,
                 0.682,
                 0.774,
                 0.881,
                 1.0,
                 1.135,
                 1.292,
                 1.467
                };

#define MIN_RAND 0
#define MAX_RAND (exp2(32)-1)

#define MAX_PITCH 20
#define MIN_PITCH -10

#define crossover_p 0.5
#define mutation_p 0.5
#define selection_p 0.2

typedef struct{
  int gear;
  float pitch;
  float cp;
}wa_element;


int wa_element_comparator(const void* p1, const void* p2){
  float c1 = ((wa_element *)p1)->cp; 
  float c2 = ((wa_element *)p2)->cp;
  
  if(c1>c2) 
    return 1;
  if(c1<c2) 
    return -1;
    
  return 0;
}

int wa_element_r_comparator(const void* p1, const void* p2){
  return (-1)*wa_element_comparator(p1,p2);
}
typedef struct{
  float wind_speed; // m/s
  float pitch;
  int gear;
  float turbine_speed; // rad/s
  float turbine_radius;
  wa_element pop[POPSIZE];
}wa_state_s;


void wa_init(wa_state_s*);
void wa_eval_element(wa_element* elem);

void wa_init_pop(wa_state_s*);
void wa_eval_pop(wa_state_s*);
void wa_selection(wa_state_s*);
void wa_crossover(wa_state_s*);
void wa_mutate(wa_state_s*);

void wa_init(wa_state_s* wa_state){
  wa_state->wind_speed = 0;
  srandom(12590951);
}

void wa_eval_element(wa_element* elem){
}

void wa_init_pop(wa_state_s* s){
  int curgear = s->gear;
  wa_element* elem;

  for(int i=0;i<POPSIZE;i++){
    elem = &(s->pop[i]);

    switch(i%3){
      case 0:
        elem->gear = curgear;
        break;
      case 1:
        elem->gear = curgear+1;
        break;
      default:
        elem->gear = (random() & 0xFF) * 14 / 0xFF + 1 ;
        break;
    }

    elem->pitch = random() * (MAX_PITCH-MIN_PITCH) / MAX_RAND + MIN_PITCH;
  }
}

void wa_print_pop(wa_state_s* s){
  wa_element* elem;

  for(int i=0;i<POPSIZE;i++){
    elem = &(s->pop[i]);

    printf("[%d] g= %d,p= %f,eval= %f\n",i,elem->gear,elem->pitch,elem->cp);
  }
}
void wa_eval_pop(wa_state_s* s){
  wa_element* elem;
  
  
  for(int i=0;i<POPSIZE;i++){
    elem = &(s->pop[i]);

    float newturbinespeed = (s->turbine_speed * RATIOS[s->gear-1] /RATIOS[elem->gear-1]);
    float lambda = newturbinespeed * s->turbine_radius / s->wind_speed;
    float pitch =  elem->pitch;
    float lambdai = 1
                    /( 
                        (1 / (lambda + 0.08 * pitch)) 
                      - (0.035 / (1 + pitch * pitch * pitch))
                    );
    float cp = C1 *( C2/lambdai - C3 * pitch - C4) * exp(-(C5/lambdai)) - C6*lambda;
    elem->cp = cp;

  }
}

void wa_selection(wa_state_s* s){


}
void wa_crossover(wa_state_s* s){}
void wa_mutate(wa_state_s* s){}


int main(int argc,char** argv){

  wa_state_s state;

  wa_init(&state);

  state.wind_speed=6; // m/s
  state.pitch=0;
  state.gear=4;
  state.turbine_speed=1000; // rad/s
  state.turbine_radius=TURBINE_RADIUS;


  wa_init_pop(&state);
  wa_eval_pop(&state);
  qsort(state.pop,POPSIZE,sizeof(state.pop[0]),wa_element_r_comparator);

  for(int i=0; i<100;i++){

  }
  wa_print_pop(&state);

}
