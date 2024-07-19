#include "header.h"



void simulateFamilies()
{

    initializeFamilies();

    for(int i=0; i<num_of_families; i++)
    {

        pid_t child = fork();

        if (child == -1)
        {

            printf("faild to fork  family \n");

            exit(-1);

        }
        else if (child == 0)
        {

            families[i].id = getpid();

            shm_ptr4=creatShmInt(id4);



            srand(time(NULL));

            int starvation = rand() % (max_starvation_level - min_starvation_level+ 1);

            starvation += min_starvation_level;

            families[i].starvaition_level = starvation;



            sem_wait(semid4);

            shm_ptr4[i] = families[i].starvaition_level;

            family_write_level(i,shm_ptr4[i]);

            sem_post(semid4); // semid4= semid4+ 1



            //sleep(3);

            break;

        }

        sleep(2);

    }

    while(1)
    {

        for (int i = 0; i< num_of_families; i++)
        {

            if (getpid() == families[i].id && families[i].isDeath == 0)
            {

                srand(time(NULL));

                int pauseTime = rand() % (max_family_sleep - min_family_sleep+ 1);

                pauseTime += min_family_sleep;

                sleep(pauseTime);



                sem_wait(semid4);

                // now we will write the starvation level on the shared memory

                families[i].starvaition_level = shm_ptr4[i] + (shm_ptr4[i] * (pauseTime/20.0));

                if (families[i].starvaition_level > family_starvation_threshold)
                {

                    families[i].isDeath = 1;

                    family_write_level(i,-1);

                    shm_ptr4[i] = -1;
                    
                    family_write_parent(i);

                    sem_post(semid4); // semid4= semid4+ 1

                    break;

                }

                shm_ptr4[i] = families[i].starvaition_level;

                family_write_level(i,shm_ptr4[i]);

                sem_post(semid4); // semid4= semid4+ 1



                srand(time(NULL));

                int pauseTime2 = rand() % (max_family_sleep - min_family_sleep+ 1);

                pauseTime2 += min_family_sleep;

                //printf("family process sleeps %d seconds\n",pauseTime2);

                sleep(pauseTime2);

            }// ens od if id



        }// end of for loop
        
        if (getpid() == family_id)
        {

            // planes parent process here

            signal(SIGUSR1,familly_sig);

        }

    } // end of while

} // end of the simulation function



void initializeFamilies()
{

    for (int i=0; i<num_of_families; i++)
    {

        // initialize starvation levels

        families[i].isDeath = 0;

    }

}



void family_write_level(int i,int level)
{

    char fifo_name[20] = "family-GL";

    char fifo_num[20];

    sprintf(fifo_num,"%d",i);

    strcat(fifo_name,fifo_num);

    int f;

    //char message[20];

    //sprintf(message,"%d",level);

    //strcat(message,"%");

    if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
    {

        perror("open error\n");

        exit(-1);

    }
    else
    {

        if (write(f,&level,sizeof(int)) == -1)
        {

            perror("write error\n");

            exit(-1);

        }

    }

}

///////////////////////////////

void family_write_parent(int i)
{

    // write to the fifo

    char fifo_name[20] = "family-parent";

    char fifo_num[20];

    sprintf(fifo_num,"%d",i);

    strcat(fifo_name,fifo_num);

    int f;

    int num = 1;



    // open the fifo

    if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
    {

        perror("open error family write to parent\n");

        exit(-1);

    }
    else
    {

        if (write(f,&num,sizeof(int)) == -1)
        {

            perror("write error\n");

            exit(-1);

        }

    }

}
/////////////////////////////
void familly_sig(int signo)
{

    for (int i = 0; i < num_of_families; i++)
    {

        // kill all planes processes

        kill(families[i].id,SIGKILL);

    }

    kill(family_id,SIGKILL);

}


