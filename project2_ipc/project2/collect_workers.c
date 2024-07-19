#include "header.h"

void simulateCollectWorkers()
{

    init_collecters();

    for (int i = 0; i < collect_workers; i++)
    {

        pid_t child = fork();

        if (child == -1)
        {

            printf("faild to fork workers\n");

            exit(-1);

        }
        else if (child == 0)
        {

            collect_worker_array[i].id = getpid();

            collect_worker_array[i].martyered=0;

            sleep(3);

            break;

        }

        sleep(2);

    }

    while(1)
    {

        for (int i = 0; i< collect_workers; i++)
        {

            if (getpid() == collect_worker_array[i].id)
            {

                shm_ptr1=createShm(id);

                shm_ptr2=createShm(id2);



                srand(time(NULL));

                int pausTime = rand() % (max_collector_sleep - min_collector_sleep+ 1);

                pausTime += max_collector_sleep;

                sleep(pausTime);



                if (collect_worker_array[i].martyered ==1)
                {

                    collecter_martyered(i);

                    collect_worker_array[i].Tiered_level = rand() % (tired_max - tired_min + 1);

                    collect_worker_array[i].Tiered_level  += tired_min;

                    collect_worker_array[i].martyered =0;

                    // fifo to parent

                    collector_write_parent(i);

                }



                collect_tired_level(i,collect_worker_array[i].Tiered_level);



                sem_wait(semid); // while it is less than or equal 0, keep waiting

                // then do semid = semid - 1

                int index = 0;

                bool empty = false;

                while (shm_ptr1[index].weight == 0)
                {

                    if (index == 99)
                    {

                        // the shared memory is empty

                        empty = true;

                        break;

                    }

                    index++;

                }

                int weight=0;

                if (empty == false)
                {

                    weight = shm_ptr1[index].weight;

                    container_gl container;

                    container.index = index;

                    container.weight = weight;

                    collect_worker_array[i].current_collected_container.weight = weight;

                    shm_ptr1[index].weight =0; // this container has been collected

                    collect_read(i,container);

                    collect_worker_array[i].Tiered_level +=

                        (collect_worker_array[i].current_collected_container.weight)*(0.25);

                    collect_tired_level(i,collect_worker_array[i].Tiered_level);

                }

                sem_post(semid); // semid = semid + 1



                /////////////////////////////////// The collector will sleep Now

                srand(time(NULL));

                pausTime = rand() % (max_collector_sleep - min_collector_sleep+ 1);

                pausTime += max_collector_sleep;

                sleep(pausTime);

                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



                if(collect_worker_array[i].Tiered_level > tired_worker_threshold)
                {

                    collect_worker_array[i].martyered=1;

                    collect_split_write(i);

                    break;

                } // end of if that the collecter over tiered level



                // take container to the storage area if it is dropped successfully from plane

                sem_wait(semid2); // while it is less than or equal 0, keep waiting

                // then do semid = semid - 1

                int index2 = 0;

                bool full = false;



                while (shm_ptr2[index2].weight != 0)
                {

                    if (index2 == 99)
                    {

                        // the sharef memory is full

                        full = true;

                        break;

                    }

                    index2++;

                }

                if (full == false && weight != 0)
                {

                    container_gl container;

                    shm_ptr2[index2].weight = weight;

                    container.index = index2;

                    container.weight = weight;

                    collect_write(i,container);

                }

                sem_post(semid2);



                //} // end of while

            } // end of if

        } // the end of if for each collector



        if (getpid() == collect_workers_id)
        {

            signal(SIGUSR2,collect_workers_sig);

        }

    } // end of while(1)

}



void collect_workers_sig(int sig)
{

    for (int i = 0; i< collect_workers; i++)
    {

        if (collect_worker_array[i].martyered == 0)
        {

            kill(collect_worker_array[i].id,SIGKILL);

        }

    }

    kill(collect_workers_id,SIGKILL);

}



void collect_read(int i, container_gl container)
{

    char fifo_name[20] = "collect-GL";

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

        if (write(f,&container,sizeof(container_gl)) == -1)
        {

            perror("write error\n");

            exit(-1);

        }

    }

}



void collect_write(int i, container_gl container)
{

    char fifo_name[20] = "collect-GL";

    char fifo_num[20];

    sprintf(fifo_num,"%d",i);

    strcat(fifo_name,fifo_num);

    strcat(fifo_name,"B");



    int f;

    if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
    {

        perror("open error\n");

        exit(-1);

    }
    else
    {

        if (write(f,&container,sizeof(container_gl)) == -1)
        {

            perror("write error\n");

            exit(-1);

        }

    }

}



void collecter_martyered(int i)
{

    char fifo_name[20] = "collect-GL";

    char fifo_num[20];

    sprintf(fifo_num,"%d",i);

    strcat(fifo_name,fifo_num);

    strcat(fifo_name,"BC");



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



void collect_split_write(int i)
{

    char fifo_name[20] = "collect-split";

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



void init_collecters()
{

    for (int i = 0; i< collect_workers; i++)
    {

        collect_worker_array[i].martyered = 0;

        collect_worker_array[i].Tiered_level = rand() % (tired_max - tired_min + 1);

        collect_worker_array[i].Tiered_level  += tired_min;

        collect_tired_level(i, collect_worker_array[i].Tiered_level);

    }

}



void collect_tired_level(int i, float level)
{

    char fifo_name[20] = "collect-GL";

    char fifo_num[20];

    sprintf(fifo_num,"%d",i);

    strcat(fifo_name,fifo_num);

    strcat(fifo_name,"BCD");



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

//////////////////////////

void collector_write_parent(int i)
{

    // write to the fifo

    char fifo_name[20] = "collector-parent";

    char fifo_num[20];

    sprintf(fifo_num,"%d",i);

    strcat(fifo_name,fifo_num);

    int f;

    int num = 1;



    // open the fifo

    if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
    {

        perror("open error collecter \n");

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
//////////////////////////


