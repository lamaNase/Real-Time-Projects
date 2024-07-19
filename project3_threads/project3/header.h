/*
 * local header file for pipe client-server
 */

#ifndef __LOCAL_H_
#define __LOCAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <limits.h>
#include <signal.h> 
#include <math.h>
#include <GL/glut.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <ctype.h>

// constant variables
#define MAX_LINE_LENGTH 50
#define MAX_ARRAY_SIZE 100
#define MAX_NUM_OF_CELLS 10

extern int simulation_time;
extern int num_of_medicines;
extern int num_of_lines;
extern int min_line_speed;
extern int max_line_speed;
extern int num_medicine_type_threshold;
extern int liquid_level_min_range;
extern int liquid_level_max_range;
extern int liquid_level_min_range_normal;
extern int liquid_level_max_range_normal;
extern int botteled_out_of_spec_threshold;
extern int pill_out_of_spec_threshold;
extern pid_t production_lines_id;
extern int num_of_employees; 
extern pthread_t tid;  
extern int status; 
/// here 
extern int number_of_workers_threshold; 
extern int package_capacity;

extern int min_inspection_time;
extern int max_inspection_time;
extern int min_packaging_time;
extern int max_packaging_time ;
extern int number_of_pills_per_package ;
extern int min_color ;
extern int max_color ;
extern int min_color_normal ;
extern int max_color_normal ;
extern int min_number_of_pills_per_package ;
extern int _number_of_pills_per_package ;
extern int min_number_of_pills_per_package_normal ;
extern int max_number_of_pills_per_package_normal ;
extern int min_sealed ;
extern int max_sealed ;
extern int min_sealed_normal ;
extern int max_sealed_normal ;
extern int min_label_matched ;
extern int max_label_matched ;
extern int min_label_matched_normal ;
extern int max_label_matched_normal ;
extern int min_expiry_date ;
extern int max_expiry_date ;
extern int min_expiry_date_normal ;
extern int max_expiry_date_normal ;
extern int min_pill_size_matched ;
extern int max_pill_size_matched ;
extern int min_pill_size_matched_normal ;
extern int max_pill_size_matched_normal ;
extern int produced_medicine_to_switch; 
extern int min_pill_count_matched ;
extern int max_pill_count_matched ;
extern int min_pill_count_matched_normal ;
extern int max_pill_count_matched_normal ;
extern int need_help;
extern int id ;  
extern int not_need_help;

////////////////////////
typedef struct {
  bool folded_prescriptions_attached;
} liquid_package;

typedef struct {
  bool folded_prescriptions_attached;
  int number_of_pills;
}pill_package; 


/////////////////////////////////// struct of the backage 

typedef struct{
  int capacity;
}package;

// structs for medicines type
typedef struct{
  int liquid_level;
  int color_matched;
  int sealed;
  int label_matched;
  int expiry_date;
  int produced;
  bool read;
  float pos;
  bool reached;
  bool out_of_spec;
  liquid_package package;
  int num;
} liquid_medicine;

typedef struct {
  int pill_count_matched;
  int color_matched;
  int pill_size_matched;
  int expiry_date;
  bool produced;
  bool read;
  bool out_of_spec;
  float pos;
  bool reached;
  pill_package package;
  int num;
} pill_based_medicine;

// struct for production line
typedef struct {
  pid_t id;
  int num;
  bool is_liquid;
  int speed;
  int production_count;
  int out_of_spec_count;
  liquid_medicine liquid_medicines[MAX_ARRAY_SIZE];
  pill_based_medicine pill_medicines[MAX_ARRAY_SIZE]; 
  int numOfEmployees; 
  int produced_not_read; 
  int Helper_number; 
  int number_of_workers_threshold; 
  bool has_extra_employees; /// initialy will be set to true  
  package Package[MAX_ARRAY_SIZE];
} production_line;

// struct for employee
typedef struct {
  pthread_t id;
  int num;
  bool switched;
  bool hold_medicine;
  bool do_packedging;
  float pos;
  bool go_back1;
  bool go_back2;
  production_line* line;
  production_line* New_line;
} employee;

typedef struct {
  int numOfEmployees;
  int production_count;
  employee employees[MAX_ARRAY_SIZE]; 
  pthread_t Threads_Array[MAX_ARRAY_SIZE]; 
} shm_cell;

// main functions
void read_arguments(char *argv);
void handle_alarm(int sig);
void check_num_of_medicine_type();
void check_num_of_out_of_spec_threshold();
//void initialize_the_cells();

// production lines functions
void simulate_production_lines();
void init_production_lines();
void line_write_liquid(int i,liquid_medicine liq_medicine);
void lines_sig(int signo) ;
void line_write_pill(int i,pill_based_medicine pill_medicine);
void line_write_parent(int i);
void check_out_of_specification_liquid(int i);
void check_out_of_specification_pill(int i);
void *Employee_work(void *vargp);
void emplyee_write_pill_GL(int line_num,int emp_num,pill_based_medicine pill_medicine);
void emplyee_write_liquid_GL(int line_num,int emp_num,liquid_medicine liquid_medicine); 
void add_employee(int line, int emp);
void delete_employee(int line,int emp);
shm_cell* createShm(int process_id);
sem_t* creatSemaphore(char sem_name[]);

// Define globale arrays
production_line production_lines[MAX_ARRAY_SIZE];
employee* employees[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE]; 
//shm_cell shm_content[MAX_ARRAY_SIZE]; 

// openGL functions
int init_openGL(int argc, char** argv);
void drawCircle(float x, float y);
void print (float x, float y,const char *text);
void timer();
void display();
void draw_production_lines();
void draw_liquid_medicine();
void deaw_pill_medicine();
void draw_pill(float x, float y);
void drawPerson(float centerX, float centerY,float r, float g, float b);
void draw_employees();
void draw_package();
void draw_garbage();
void draw_paper(float x, float y); 

///// define mutexat 
pthread_mutex_t product_mutex=PTHREAD_MUTEX_INITIALIZER;  


//// define shared memory and semaphores
shm_cell *shm_ptr1; 
sem_t *semid;  


/// create or access the shared memory segment 
shm_cell* createShm(int process_id){
  int shmid;
  shm_cell *shmptr;
  int size_ofArray;
  size_ofArray=MAX_NUM_OF_CELLS *(sizeof(shm_cell));
  
  // Create or access the shared memory segment
  if ((shmid = shmget((key_t)process_id, size_ofArray, 0666 | IPC_CREAT)) == -1) {
    perror("shmget -- process -- access");
    exit(2);
  }
  
  shmptr = (shm_cell *) shmat (shmid,NULL,0);
  return shmptr ;
} 

sem_t* creatSemaphore(char sem_name[]){
  sem_t *m;
  if ((m = sem_open(sem_name,O_CREAT,0644,1) )== SEM_FAILED){
    perror("semaphore intialization\n");
    exit(-1);
  }
  sem_init(m,1,1);
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  return m;
}

#endif

