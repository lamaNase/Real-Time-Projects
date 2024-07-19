#include "header.h"
#include "planes.c"
#include "collect_workers.c"
#include "split_workers.c"
#include "dstrb_workers.c"
#include "family.c"
#include "openGL.c"
#include "fifoiat.c"

int cargo_planes = 0;
int collect_workers=0;
int split_workers=0 ;
int dstrb_workers=0;
int min_containers = 0;
int max_containers = 0;
int min_drop_period  = 0;
int max_drop_period  = 0;
int min_refill_period  = 0;
int max_refill_period = 0;
int min_container_loss = 0;
int max_container_loss = 0;
int min_container_height = 0;
int max_container_height = 0;
int saftey_height = 0;
int danger_height = 0;
float tired_worker_threshold=0.0;
int family_death_rate_threshold=0;
int bags=0; // to split the containers
int num_of_families=0;
int death_rate=0;
int family_starvation_threshold=0;
int id = 0;
int id2=0;
int id3=0;
int id4=0;
int id5=0;
int min_collector_sleep=0;
int max_collector_sleep=0;
int simulation_time = 0;
pid_t planes_id;
pid_t collect_workers_id;
pid_t splitter_workers_id;
pid_t dst_workers_id;
pid_t family_id;
int max_splitter_sleep=0;
int min_splitter_sleep=0;
int min_dst_sleep=0;
int max_dst_sleep=0;
int num_of_bags=0;
int dst_Bags=0;
int tired_min = 0;
int tired_max=0;
int max_family_sleep=0;
int min_family_sleep=0;
int max_starvation_level=0;
int min_starvation_level=0;
int shut_containers_threshold = 0;
int shut_containers = 0;
int martied_collectors_threshold=0;
int martied_collector_counter=0;
int martied_distributers_threshold=0;
int martied_distributers_counter=0;
int diead_family_threshold=0;
int diead_family_count=0;


int main (int argc, char **argv) {

    if (argc != 2) {

        printf("Missing arguments\n");
        exit(-1);

    }

    /////// Read all required  arguments from the file
    read_arguments(argv[1]);

    signal(SIGALRM,handle_alarm);
    alarm(simulation_time);


    id = ftok(".",'L');
    container *shm_ptr1=createShm(id);
    semid = creatSemaphore("plane_collect");

    ///////////  create shared memory and semaphore for collectors and splitter
    id2=ftok(".",'M');
    container *shm_ptr2=createShm(id2);
    semid2= creatSemaphore("collect_split");


    //////////  create shared memory and semaphore for collectors and splitter

    id3=ftok(".",'S');
    int *shm_ptr3=creatShmInt(id3);
    semid3= creatSemaphore("split_dst");

    ///// create shared memory and semaphore for spilliters and distributers
    id4=ftok(".",'Q');
    int *shm_ptr4=creatShmInt(id4);
    semid4= creatSemaphore("dst_family");

    /// create shared memory for the family to write the starvation level and the to dstrb to read
    id5=ftok(".",'O');
    int *shm_ptr5=creatShmInt(id5);


    for (int i = 0; i < 100; i++) {

        shm_ptr1[i].weight = 0;
        shm_ptr2[i].weight = 0;
        shm_ptr3[i]=0;
        shm_ptr4[i]=-1;

    }

     // Initialize fifos
    init_fifoiat();
    
    pid_t child = fork();
    if (child == -1) {

        perror("create error\n");
        exit(-1);

    } else if (child == 0) {

        // openGL process
        init_openGL(argc,argv);

    } else {

          // Parent
        pid_t child2 = fork();

        if (child2 == -1) {

            perror("create error\n");
            exit(-1);

        } else if (child2 == 0) {

            planes_id = getpid();
            // planes process
            simulateCargoPlanes();

        } else {
            // Parent
          pid_t child3=fork();
            if (child3 == -1) {

                perror("create error\n");
                exit(-1);

            } else if(child3==0) {

                collect_workers_id = getpid();
                simulateCollectWorkers();

            } else {
            
              pid_t child4=fork();
                if(child4==-1) {

                    perror("create error\n");
                    exit(-1);

                } else if(child4==0) {

                    splitter_workers_id = getpid();
                    simulateSplitWorkers();

                } else {

                 pid_t child5=fork();
                    if(child5==-1) {

                        perror("create error \n");
                        exit(-1);

                    } else if(child5==0) {

                        dst_workers_id = getpid();
                        simulateDstrbWorkers();

                    } else {

                     pid_t child6=fork();
                        if(child6==-1) {

                            perror("create error \n");
                            exit(-1);

                        } else if(child6==0) {

                            family_id = getpid();
                            simulateFamilies();

                        }

                    }

                }

            }

        }

    }


    while(1) {

        check_num_of_containers();
        check_num_of_martierd_collecters();
        check_num_of_martierd_distributers();
         check_num_of_diead_family();

    }

    return 0;

}



void read_arguments(char *argv) {

    FILE *file = fopen(argv,"r");

    if (file == NULL) {

        printf("failed to open the file\n");
        exit(-1);

    } else {

        char line[MAX_LINE_LENGTH];

        while (fgets(line,MAX_LINE_LENGTH,file) != NULL) {

            char *token = strtok(line," ");

            if (strcmp(token,"cargo_planes") == 0) {

                token = strtok(NULL,"\n");
                cargo_planes = atoi(token);

            } else if (strcmp(token,"min_white_flour_container") == 0) {

                token = strtok(NULL,"\n");
                min_containers = atoi(token);

            } else if (strcmp(token,"max_white_flour_container") == 0) {

                token = strtok(NULL,"\n");
                max_containers = atoi(token);

            } else if (strcmp(token,"min_drop_period") == 0) {

                token = strtok(NULL,"\n");
                min_drop_period = atoi(token);

            } else if (strcmp(token,"max_drop_period") == 0) {

                token = strtok(NULL,"\n");
                max_drop_period = atoi(token);

            } else if (strcmp(token,"min_refill_period") == 0) {

                token = strtok(NULL,"\n");
                min_refill_period = atoi(token);

            } else if (strcmp(token,"max_refill_period") == 0) {

                token = strtok(NULL,"\n");
                max_refill_period = atoi(token);

            } else if (strcmp(token,"min_container_loss") == 0) {

                token = strtok(NULL,"\n");
                min_container_loss = atoi(token);

            } else if (strcmp(token,"max_container_loss") == 0) {

                token = strtok(NULL,"\n");
                max_container_loss = atoi(token);

            } else if (strcmp(token,"min_container_height") == 0) {

                token = strtok(NULL,"\n");
                min_container_height = atoi(token);

            } else if (strcmp(token,"max_container_height") == 0) {

                token = strtok(NULL,"\n");
                max_container_height = atoi(token);

            } else if (strcmp(token,"saftey_height") == 0) {

                token = strtok(NULL,"\n");
                saftey_height = atoi(token);

            } else if (strcmp(token,"danger_height") == 0) {

                token = strtok(NULL,"\n");
                danger_height = atoi(token);

            } else if (strcmp(token,"family_starvation_threshold") == 0) {

                token = strtok(NULL,"\n");
                family_starvation_threshold = atoi(token);

            } else if(strcmp(token,"collect_workers")==0) {

                token = strtok(NULL,"\n");
                collect_workers=atoi(token);

            } else if(strcmp(token,"split_workers")==0) {

                token = strtok(NULL,"\n");
                split_workers=atoi(token);

            } else if(strcmp(token,"distribut_workers")==0) {

                token = strtok(NULL,"\n");
                dstrb_workers=atoi(token);

            } else if(strcmp(token,"tired_worker_threshold")==0) {

                token = strtok(NULL,"\n");
                tired_worker_threshold=atof(token);

            } else if(strcmp(token,"num_of_families")==0) {

                token = strtok(NULL,"\n");
                num_of_families=atoi(token);

            } else if(strcmp(token,"family_death_rate_threshold")==0) {

                token = strtok(NULL,"\n");
                family_death_rate_threshold=atoi(token);

            } else if (strcmp(token,"min_collector_sleep") == 0) {

                token = strtok(NULL,"\n");
                min_collector_sleep = atoi(token);

            } else if (strcmp(token,"max_collector_sleep") == 0) {

                token = strtok(NULL,"\n");
                max_collector_sleep = atoi(token);

            } else if (strcmp(token,"simulation_time") == 0) {

                token = strtok(NULL,"\n");
                simulation_time = atoi(token);

            } else if (strcmp(token,"max_splitter_sleep") == 0) {

                token = strtok(NULL,"\n");
                max_splitter_sleep= atoi(token);

            } else if (strcmp(token,"min_splitter_sleep") == 0) {

                token = strtok(NULL,"\n");
                min_splitter_sleep= atoi(token);

            } else if (strcmp(token,"min_dst_sleep") == 0) {

                token = strtok(NULL,"\n");
                min_dst_sleep= atoi(token);

            } else if (strcmp(token,"max_dst_sleep") == 0) {

                token = strtok(NULL,"\n");
                max_dst_sleep= atoi(token);

            } else if (strcmp(token,"dst_Bags") == 0) {

                token = strtok(NULL,"\n");
                dst_Bags= atoi(token);

            } else if (strcmp(token,"tired_min") == 0) {

                token = strtok(NULL,"\n");
                tired_min= atoi(token);

            } else if (strcmp(token,"tired_max") == 0) {

                token = strtok(NULL,"\n");
                tired_max= atoi(token);

            } else if (strcmp(token,"min_family_sleep") == 0) {

                token = strtok(NULL,"\n");
                min_family_sleep= atoi(token);

            } else if (strcmp(token,"max_family_sleep") == 0) {

                token = strtok(NULL,"\n");
                max_family_sleep= atoi(token);

            } else if(strcmp(token,"min_starvation_level")==0) {

                token=strtok(NULL,"\n");
                min_starvation_level=atoi(token);

            } else if(strcmp(token,"max_starvation_level")==0) {

                token=strtok(NULL,"\n");
                max_starvation_level=atoi(token);

            } else if(strcmp(token,"shut_containers_threshold")==0) {

                token=strtok(NULL,"\n");
                shut_containers_threshold=atoi(token);

            } else if(strcmp(token,"martied_collectors_threshold")==0) {

                token=strtok(NULL,"\n");
                martied_collectors_threshold=atoi(token);

            } else if(strcmp(token,"martied_distributers_threshold")==0) {

                token=strtok(NULL,"\n");
                martied_distributers_threshold=atoi(token);

            } else if(strcmp(token,"diead_family_threshold")==0) {

                token=strtok(NULL,"\n");
                diead_family_threshold=atoi(token);

            }

        }

    }

    fclose(file);

}


void handle_alarm(int sig) {

    // send signals to child processes
    kill(planes_id,SIGUSR1);
    kill(collect_workers_id,SIGUSR2);

    // kill main process
    exit(EXIT_SUCCESS);

}


void check_num_of_containers() {

    int num,f;

    for (int i = 0; i < cargo_planes; i++)
    {

        char fifo_name[20] = "plane-parent";

        char fifo_num[20];

        sprintf(fifo_num,"%d",i);

        strcat(fifo_name,fifo_num);

        if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
        {

            perror("open error parent containers\n");

            exit(-1);

        }
        else
        {

            if (read(f,&num,sizeof(int)) != -1)
            {

                if (num == 1)

                    shut_containers++;

            }

            close(f);

        }

    }

    if (shut_containers > shut_containers_threshold)
    {

        printf("The number of shut containers has exceeds the threshold\n");

        kill(planes_id,SIGUSR1);

        kill(collect_workers_id,SIGUSR2);

        // kill main process

        exit(EXIT_SUCCESS);

    }

}

/////////////////////////////////////////



void check_num_of_martierd_collecters()
{

    int num,f;

    for (int i = 0; i < collect_workers; i++)
    {

        char fifo_name[20] = "collector-parent";

        char fifo_num[20];

        sprintf(fifo_num,"%d",i);

        strcat(fifo_name,fifo_num);

        if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
        {

            perror("open error parent collect\n");

            exit(-1);

        }
        else
        {

            if (read(f,&num,sizeof(int)) != -1)
            {

                if (num == 1)

                    martied_collector_counter++;

            }

            close(f);

        }

    }

    if (martied_collector_counter > martied_collectors_threshold)
    {

        printf("The number of martierd collectors has exceeds the threshold\n");

        kill(planes_id,SIGUSR1);

        kill(collect_workers_id,SIGUSR2);
        kill(splitter_workers_id,SIGUSR2);
        kill(dst_workers_id,SIGUSR2);
        kill(family_id,SIGUSR2);
        

        // kill main process

        exit(EXIT_SUCCESS);
        

    }

}

////////////////////////////////////////



void check_num_of_martierd_distributers()
{

    int num,f;

    for (int i = 0; i < dstrb_workers; i++)
    {

        char fifo_name[20] = "distributer-parent";

        char fifo_num[20];

        sprintf(fifo_num,"%d",i);

        strcat(fifo_name,fifo_num);

        if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
        {

            perror("open error parent dis \n");

            exit(-1);

        }
        else
        {

            if (read(f,&num,sizeof(int)) != -1)
            {

                if (num == 1)

                    martied_distributers_counter++;

            }

            close(f);

        }

    }

    if (martied_distributers_counter > martied_distributers_threshold)
    {

        printf("The number of martierd distributer has exceeds the threshold\n");

         kill(planes_id,SIGUSR1);

        kill(collect_workers_id,SIGUSR2);
        kill(splitter_workers_id,SIGUSR2);
        kill(dst_workers_id,SIGUSR2);
        kill(family_id,SIGUSR2);

        // kill main process

        exit(EXIT_SUCCESS);

    }

}

/////////////////////////////////////////////

void check_num_of_diead_family()
{

    int num,f;

    for (int i = 0; i < num_of_families; i++)
    {

        char fifo_name[20] = "family-parent";

        char fifo_num[20];

        sprintf(fifo_num,"%d",i);

        strcat(fifo_name,fifo_num);

        if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
        {

            perror("open error  family\n");

            exit(-1);

        }
        else
        {

            if (read(f,&num,sizeof(int)) != -1)
            {

                if (num == 1)

                    diead_family_count++;

            }

            close(f);

        }

    }

    if (diead_family_count > diead_family_threshold)
    {

        printf("The number of diead  family has exceeds the threshold\n");

         kill(planes_id,SIGUSR1);

        kill(collect_workers_id,SIGUSR2);
        kill(splitter_workers_id,SIGUSR2);
        kill(dst_workers_id,SIGUSR2);
        kill(family_id,SIGUSR2);

        // kill main process

        exit(EXIT_SUCCESS);

    }

}

