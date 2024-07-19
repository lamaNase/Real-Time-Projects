
/*
 * local header file
 */

#ifndef __LOCAL_H_
#define __LOCAL_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <limits.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <wait.h>
#include <signal.h>
#include <sys/msg.h>
#include <ctype.h>
#include <stdbool.h>
#include <GL/glut.h>
#include <math.h>
#include <sys/sem.h> // this library is for semaphores  
#include <semaphore.h>


#define SEED 'g'  /* seed for ftok */
#define SENDER 1L /* message for the server */
#define MAX_LINE_LENGTH 100
#define MAX_NUM_OF_CONTAINERS 100
#define MAX_NUM_OF_BAGS 10
#define CONTAINER_WEIGHT 5
#define Max_arrays_size 30
#define ROWS   5
#define COLS   3
#define X_init_plane 100.0
#define Y_init_plane 320.0
#define X_init_house 30.0
#define Y_init_house 80.0

#define SLOT_LEN  50
#define N_SLOTS   6

// Build a struch for the white flour container
 typedef struct {

	int weight;
	int height;
	int isShot;
	int loss_amount;
}container;

// Build a struch for the cargo plane
typedef struct {

	int id;
	int num_of_containers;
	container containers[MAX_NUM_OF_CONTAINERS];
       	int msg_id_planeToCollectors;
} CargoPlane;

typedef struct {
	int id;
	int martyered;
	float Tiered_level;
	container current_collected_container;
} collect_worker;

typedef struct {
	int id;
	int switched;
	container current_collected_container;
}splitWorker;

typedef struct {
	int id;
	int martyer;
	int num_of_bags;
	float Tiered_level;

}dstrbWorker;

typedef struct{

int id;
int starvaition_level;
int priority;
int isDeath;
int death_rate;
}family;


extern int cargo_planes ;
extern int collect_workers;
extern int split_workers ;
extern int dstrb_workers;
extern int min_containers;
extern int max_containers;
extern int min_drop_period ;
extern int max_drop_period ;
extern int min_refill_period ;
extern int max_refill_period;
extern int min_container_loss;
extern int max_container_loss;
extern int min_container_height;
extern int max_container_height;
extern int saftey_height;
extern int danger_height;
extern int collect_martyred_threshold;
extern int dstrb_martyred_threshold;
extern float tired_worker_threshold;
extern int bags;
extern int num_of_families;
extern int family_death_rate_threshold;
extern int family_starvation_threshold;
extern int death_rate;
extern int id;  
extern int id2; 
extern int id3; 
extern int id4; 
extern int id5;
extern int min_collector_sleep; 
extern int max_collector_sleep; 
extern int simulation_time;
extern pid_t planes_id;
extern pid_t collect_workers_id;  
extern pid_t splitter_workers_id; 
extern pid_t dst_workers_id;
extern pid_t family_id;
extern int max_splitter_sleep;
extern int min_splitter_sleep; 
extern int min_dst_sleep; 
extern int max_dst_sleep;
extern int num_of_bags;
extern int dst_Bags;
extern int tired_min ;
extern int tired_max; 
extern int max_family_sleep;
extern int min_family_sleep;  
extern int max_starvation_level;
extern int min_starvation_level;
extern int shut_containers_threshold;
extern int martied_collectors_threshold;
extern int martied_distributers_threshold;
extern int diead_family_threshold;
extern int diead_family_count;
/// shared Memory between planes and collectors
container *shm_ptr1;
sem_t *semid;  

///  shared Memory between collectors and spilters  
container *shm_ptr2; 
sem_t *semid2;  

//// shared Memory between splitters and distributers
int *shm_ptr3; 
sem_t *semid3; 

//// shared Memory between distributers and families  
int *shm_ptr4;
sem_t *semid4; 

int *shm_ptr5;

void init_dstrb();
void dstrb_tired_level(int i, float level);
void init_collecters();
void simulateCargoPlanes();
void  init_split();
void check_switch(int k);
void disappear_split(int i);
void simulateCollectWorkers();
void simulateSplitWorkers();
void simulateDstrbWorkers();
void simulateFamilies();
void read_arguments(char *argv);
void init_planes(CargoPlane planes[]);
void init_families(family families[]);
void init_height_and_check(CargoPlane planes[],int plane, int container);
void planes_sig(int signo);
void handle_alarm(int sig);
void collect_workers_sig(int sig);
void initializeFamilies();
void check_num_of_containers();
void plane_write_plane(int i);
void check_num_of_martierd_collecters();
void check_num_of_martierd_distributers();
void collector_write_parent(int i);
void distributer_write_parent(int i);
void check_num_of_diead_family();
void family_write_parent(int i);
void distributer_sig(int signo);
void splitter_sig(int signo);
void familly_sig(int signo);

// openGL functions
typedef struct {
	int index;
	int weight;
	bool shm2_write;
	bool replaced;
} container_gl;

typedef struct{
	int index;
	int num_of_bags;
	bool replaced;
} bags_gl;

void dstrb_write(int i, int family);
void family_write_level(int i,int level);
void collect_tired_level(int i, float level);
void disrib_read(int i, bags_gl bag);
void collect_read(int i, container_gl container);
void collect_write(int i, container_gl container);
void collecter_martyered(int i);
void collect_split_write(int i);
void dstrb_martyered(int i);
void dstrb_split_write(int i);
void  split_read(int i, container_gl container);
void split_write(int i,bags_gl bag);
void write_drop(int i, container_gl container);
void draw_planes();void drawCircle(float x, float y);
void print (float x, float y,const char *text);
void draw_ground();
void draw_Houses(); 
void write_containers(char num[],int i);
void write_containers_GL(int plane,int containers);
void draw_container(int i);
void draw_storege_area();
void draw_split_workers();
void draw_dstrb_workers();
void draw_bags();
void timer();

///////////////// Creating for our requiered shared memories and semaphores  
container* createShm(int plane_id);
sem_t* creatSemaphore(char sem_name[]);


// create global arrays
collect_worker collect_worker_array[Max_arrays_size];
splitWorker Split_worker_array[Max_arrays_size];
dstrbWorker dstrb_Worker_array[Max_arrays_size];
CargoPlane planes[Max_arrays_size];
family families[Max_arrays_size];
container container_array[MAX_NUM_OF_CONTAINERS];
int Bags[MAX_NUM_OF_BAGS];
 
// Create or access the shared memory segment for the plane
 
container* createShm(int plane_id){
             int shmid;
           container *shmptr;
             int size_ofArray;
           size_ofArray=MAX_NUM_OF_CONTAINERS *(sizeof(container));
            
            // Create or access the shared memory segment
            if ((shmid = shmget((key_t)plane_id, sizeof(size_ofArray), 0666 | IPC_CREAT)) == -1) {
                perror("shmget -- plane -- access");
                exit(2);
            }
	
          shmptr = (container *) shmat (shmid,NULL,0);
            return shmptr ;


} 

int* creatShmInt(int Dst_id){

	     int shmid;
             int *shmptr;
             int size_ofArray;
           size_ofArray=MAX_NUM_OF_BAGS*(sizeof(int)); 
           
              // Create or access the shared memory segment
            if ((shmid = shmget((key_t)Dst_id, sizeof(size_ofArray), 0666 | IPC_CREAT)) == -1) {
                perror("shmget -- plane -- access");
                exit(2);
            }
	
          shmptr =  shmat(shmid,NULL,0);
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

