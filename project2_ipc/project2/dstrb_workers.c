#include "header.h"



void simulateDstrbWorkers()
{

  init_dstrb();

    for (int i = 0; i < dstrb_workers; i++)
    {

        pid_t child = fork();

        if (child == -1)
        {

            printf("faild to fork dstrb workers\n");

            exit(-1);

        }
        else if (child == 0)
        {

            dstrb_Worker_array[i].id = getpid();

            sleep(3);

            break;

        }

        sleep(2);

    }

    while(1)
    {

        for (int i = 0; i< dstrb_workers; i++)
        {

            if (getpid() == dstrb_Worker_array[i].id)
            {

                shm_ptr3=creatShmInt(id3);

                shm_ptr4=creatShmInt(id4);

                srand(time(NULL));

                int pauseTime = rand() % (max_dst_sleep - min_dst_sleep+ 1);

                pauseTime += min_dst_sleep;

                sleep(pauseTime);



                if (dstrb_Worker_array[i].martyer == 1)
                {

                    dstrb_Worker_array[i].Tiered_level = rand() % (tired_max - tired_min + 1);

                    dstrb_Worker_array[i].Tiered_level  += tired_min;

                    dstrb_Worker_array[i].martyer = 0;

                    dstrb_martyered(i);

                    distributer_write_parent(i);

                }

                dstrb_tired_level(i, dstrb_Worker_array[i].Tiered_level);



                sem_wait(semid3);

                int index = 0;

                bool empty = false;

                while (shm_ptr3[index] == 0)
                {

                    if (index == 99)
                    {

                        // the sharef memory is empty

                        empty = true;

                        break;

                    }

                    index++;

                }

                int num_of_bags = 0;

                if (empty == false)
                {

                    for(int k =0; k < dst_Bags; k++)
                    {

                        if (shm_ptr3[index+k] != 0)
                        {

                            shm_ptr3[index+k]=0;

                            num_of_bags++;

                        }

                    }  // end of Bags

                    bags_gl bag;

                    bag.index = index;

                    bag.num_of_bags = num_of_bags;

                    dstrb_Worker_array[i].num_of_bags = num_of_bags ;

                    dstrb_Worker_array[i].Tiered_level

                    +=(dstrb_Worker_array[i].num_of_bags)*(0.25);

                    disrib_read(i,bag);

                    dstrb_tired_level(i, dstrb_Worker_array[i].Tiered_level);

                }

                // post the semaphore

                sem_post(semid3); // semid2 = semid2+ 1



                // calculate the tired level



                if(dstrb_Worker_array[i].Tiered_level > tired_worker_threshold)
                {

                    dstrb_Worker_array[i].martyer=1;

                    dstrb_split_write(i);

                    break;

                }



                /////////////////////////////////// The dstrb will sleep Now

                srand(time(NULL));

                int pauseTime4 = rand() % (max_dst_sleep - min_dst_sleep+ 1);

                pauseTime4 += min_dst_sleep;

                sleep(pauseTime4);



                sem_wait(semid4);

                int max=shm_ptr4[0],max_index=0;

                for(int m=0; m<num_of_families; m++)
                {

                    if(shm_ptr4[m]>max)
                    {

                        max_index=m;

                        max=shm_ptr4[m];

                    }

                }

                if (empty == false)
                {

                    float decrementation=dst_Bags/10.0;

                    decrementation *= shm_ptr4[max_index];

                    shm_ptr4[max_index] -=(int)decrementation;

                    dstrb_write(i, max_index);

                    family_write_level(max_index,shm_ptr4[max_index]);

                }

                sem_post(semid4);







            } // end of if id

        } // the end of for loop
 
         if (getpid() == dst_workers_id)
        {

            // planes parent process here

            signal(SIGUSR1,distributer_sig);

        }

    } // end of while



}



void disrib_read(int i, bags_gl bag)
{

    char fifo_name[20] = "distrib-GL";

    char fifo_num[20];

    sprintf(fifo_num,"%d",i);

    strcat(fifo_name,fifo_num);



    int f;

    if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
    {

        perror("open error\n");

        exit(-1);

    }
    else
    {

        if (write(f,&bag,sizeof(bags_gl)) == -1)
        {

            perror("write error\n");

            exit(-1);

        }

    }

}



void init_dstrb()
{

    for (int i = 0; i < dstrb_workers; i++)
    {

        dstrb_Worker_array[i].martyer=0;

        dstrb_Worker_array[i].num_of_bags = 0;

        dstrb_Worker_array[i].Tiered_level = rand() % (tired_max - tired_min + 1);

        dstrb_Worker_array[i].Tiered_level  += tired_min;

        dstrb_tired_level(i, dstrb_Worker_array[i].Tiered_level);

    }

}



void dstrb_tired_level(int i, float level)
{

    char fifo_name[20] = "distrib-GL";

    char fifo_num[20];

    sprintf(fifo_num,"%d",i);

    strcat(fifo_name,fifo_num);

    strcat(fifo_name,"B");



    int f;

    //level = level *100;

    char message[20];

    sprintf(message,"%.1f",level);

    strcat(message,"%");

    if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
    {

        perror("open error\n");

        exit(-1);

    }
    else
    {

        if (write(f,&message,sizeof(message)) == -1)
        {

            perror("write error\n");

            exit(-1);

        }

    }

}



void dstrb_write(int i, int family)
{

    char fifo_name[20] = "distrib-GL";

    char fifo_num[20];

    sprintf(fifo_num,"%d",i);

    strcat(fifo_name,fifo_num);

    strcat(fifo_name,"BC");



    int f;

    if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
    {

        perror("open error\n");

        exit(-1);

    }
    else
    {

        if (write(f,&family,sizeof(int)) == -1)
        {

            perror("write error\n");

            exit(-1);

        }

    }

}



void dstrb_split_write(int i)
{

    char fifo_name[20] = "dstrb-split";

    char fifo_num[20];

    sprintf(fifo_num,"%d",i);

    strcat(fifo_name,fifo_num);



    int f;

    char message[20] = "martyered";

    if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
    {

        perror("open error\n");

        exit(-1);

    }
    else
    {

        if (write(f,&message,sizeof(message)) == -1)
        {

            perror("write error\n");

            exit(-1);

        }

    }

}



void dstrb_martyered(int i)
{

    char fifo_name[20] = "distrib-GL";

    char fifo_num[20];

    sprintf(fifo_num,"%d",i);

    strcat(fifo_name,fifo_num);

    strcat(fifo_name,"BCD");



    int f;

    char message[20] = "martyered";

    if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
    {

        perror("open error\n");

        exit(-1);

    }
    else
    {

        if (write(f,&message,sizeof(message)) == -1)
        {

            perror("write error\n");

            exit(-1);

        }

    }

}



//////////////////////////////

void distributer_write_parent(int i)
{

    // write to the fifo

    char fifo_name[20] = "distributer-parent";

    char fifo_num[20];

    sprintf(fifo_num,"%d",i);

    strcat(fifo_name,fifo_num);

    int f;

    int num = 1;



    // open the fifo

    if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
    {

        perror("open error\n");

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
////////////////////////////
void distributer_sig(int signo)
{

    for (int i = 0; i < dstrb_workers; i++)
    {

        // kill all planes processes

        kill(dstrb_Worker_array[i].id,SIGKILL);

    }

    kill(dst_workers_id,SIGKILL);

}

