#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define POPSIZE 1000
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

#define MIN_GEAR 1
#define MAX_GEAR 14

#define crossover_p 0.3
#define mutation_p 0.3
#define selection_tournament_p 0.5
#define selection_p 0.4
#define maxgen 100


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
  // for space efficiency parents are stored as first element in pop
  wa_element pop[POPSIZE];
  unsigned int seed;
}wa_state_s;


void wa_init(wa_state_s*);
void wa_eval_element(wa_element* elem);

void wa_init_pop(wa_state_s*);
void wa_eval_pop(wa_state_s*);
void wa_selection(wa_state_s*);
void wa_crossover(wa_state_s*);
void wa_mutate(wa_state_s*);

void wa_init(wa_state_s* wa_state){
  srandom(wa_state->seed);
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

void wa_print_elem(wa_element* elem){
    printf("g= %d,p= %f,eval= %f\n",elem->gear,elem->pitch,elem->cp);
}
void wa_print_pop(wa_state_s* s){
  wa_element* elem;

  for(int i=0;i<POPSIZE;i++){
    elem = &(s->pop[i]);
    printf("[%d]",i);wa_print_elem(elem);
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
  int n_elem = round(POPSIZE* selection_tournament_p);
  if(n_elem > POPSIZE){
    fprintf(stderr,"ERROR: Size of selection is greater than population...");
    exit(0);
  }

  wa_element tpop[n_elem];

  int tpop_i=0;
  for(int i=0;tpop_i<n_elem;i++){
    if( (i%POPSIZE) % ((int) round(1 / selection_tournament_p)) == 0){
      tpop[tpop_i]= s->pop[i];
      tpop_i++;
    }
    i++;
  }
  // select selection_tournament_p percent of pop
  // get the selection_p best of the tournament
  qsort(tpop,n_elem,sizeof(tpop[0]),wa_element_r_comparator);

  n_elem = round(POPSIZE * selection_p);

  for(int i=0;i<n_elem;i++){
    s->pop[i]= tpop[i];
  }
}

void wa_crossover(wa_state_s* s){
// TODO: FAIRE LE CROSSOVER
}

void wa_mutate(wa_state_s* s){
  int n_elem = round(POPSIZE * mutation_p);
  int n_parent = round(POPSIZE * selection_p);

  wa_element tpop[n_elem];
  int tpop_i=0;

   for(int i=0;tpop_i<n_elem;i++){ 

    int randomnum = random();
    if( (randomnum*i)%n_parent==0){
      wa_element e = s->pop[i%n_parent];

      // pitch modification percentage 70% de 255 -> 
      if((randomnum&0xFF) <= 179){
        
        // change a random decimal number to another
        int n = (randomnum&0xFF00>>8)%10;
        int d = ((randomnum&0xFF0)>>4)%4;
        float a = e.pitch/10;

        a = a - (round(a*pow(10,d))-round(a*pow(10,d-1))*10) / pow(10,d) + n/pow(10,d);
        e.pitch = a*10 ;

        if(e.pitch<MIN_PITCH)
          e.pitch = MIN_PITCH;
        if(e.pitch>MAX_PITCH)
          e.pitch = MAX_PITCH;
      }
      // gear modification percentage 20% de 255
      if(((randomnum&0xFF00)>>8) <= 51){
        e.gear ^= 0x01<< (randomnum%4);
          if(e.gear>MAX_GEAR)
            e.gear= MAX_GEAR;
      }
      tpop[tpop_i]=e;
      tpop_i++;
    }
  }

  int off = round(POPSIZE*selection_p)+ round(POPSIZE*crossover_p);
  for(int i=0;i<n_elem;i++){
    s->pop[off+i]= tpop[i];
    
  }
}

int main(int argc,char** argv){

  printf("Wind apprentice genetic algorithm solver\n\n");
  wa_state_s state;

  state.seed= time(0);
  printf("Seed is %d\n",state.seed);
  wa_init(&state);

  state.wind_speed=6; // m/s
  state.pitch=0;
  state.gear=4;
  state.turbine_speed=1000; // rad/s
  state.turbine_radius=TURBINE_RADIUS;

  wa_init_pop(&state);

  //wa_eval_pop(&state);
  //wa_print_pop(&state);
  
  for(int i=0; i<(maxgen+1);i++){
    // Eval cp in the population
    wa_eval_pop(&state);
    // Sort pop by cp

    if(i < maxgen){
      // Select parents
      wa_selection(&state);
      // Crossover the things
      wa_crossover(&state);
      // Mutate the things
      wa_mutate(&state);

      // Per generation status
      printf("Gen %d best:",i);
      wa_print_elem(&state.pop[0]);
    }else{
      //qsort(state.pop,POPSIZE,sizeof(state.pop[0]),wa_element_r_comparator);
      printf("BEST CHOICE IS:");
      wa_print_elem(&state.pop[0]);
    }

  }

}
