#include "header.h"

container *shm_ptr1;

sem_t *semid;

void simulateCargoPlanes()
{

// invoke function create semaphore to use it later

    semid = creatSemaphore("plane_collect");

    init_planes(planes);

    for (int i = 0; i < cargo_planes; i++)
    {

        pid_t plane = fork();

        if (plane == -1)
        {

            printf("Failed to create a child\n");

            exit(-1);

        }
        else if (plane == 0)
        {

            planes[i].id = getpid();

            sleep(3);

            break;

        }

        sleep(2);

    }



    while(1)
    {

        // Simulate dropping containers

        for (int i = 0; i< cargo_planes; i++)
        {

            if (getpid() == planes[i].id)
            {

                shm_ptr1=createShm(id);

                int containers = planes[i].num_of_containers;

                //sleep(8);

                for(int j = 0; j < containers; j++)
                {

		  if (planes[i].num_of_containers > 0)
                    {

                        // Drop a container settings

                        planes[i].num_of_containers--;

                        srand(time(NULL));

                        planes[i].containers[j].isShot = rand() % 2; //zero or one only

                        // check if it was shut or not

                        if (planes[i].containers[j].isShot == 0)
                        {

                            // Here the container did not shut

                            write_containers_GL(i,planes[i].num_of_containers);

                            // wait a random time till it reach the ground

                            srand(time(NULL));

                            int pausTime = rand() % (max_drop_period - min_drop_period + 1);

                            pausTime += min_drop_period;

                            sleep(pausTime);



                            // write on the shared memory

                            sem_wait(semid); // while it is less than or equal 0, keep waiting



                            // then do semid = semid - 1

                            int index = 0;

                            bool full = false;

                            while (shm_ptr1[index].weight != 0)
                            {

                                if (index == 99)
                                {

                                    full = true;

                                    break;

                                }

                                index++;

                            }

                            if (full == false)
                            {

                                shm_ptr1[index].weight = planes[i].containers[j].weight;

                                container_gl container;

                                container.index = index;

                                container.weight = shm_ptr1[index].weight;

                                write_drop(i,container);

                            }



                            sem_post(semid); // semid = semid + 1

                        }
                        else
                        {

                            // if it is shot then check its height

                            init_height_and_check(planes,i,j);

                            plane_write_plane(i);

                        }

                    } // if statement

                } // containers loop

                // sleep amount of time to refill planes

                srand(time(NULL));

                int pausTime = rand() % (max_refill_period - min_refill_period + 1);

                pausTime += min_refill_period;

                //printf("sleeping to refill planes for %d seconds\n",pausTime);

                sleep(pausTime);

                //Filling planes with random number of containers

                planes[i].num_of_containers = rand() % (max_containers-min_containers+1);

                planes[i].num_of_containers += min_containers;

                for(int j = 0; j < planes[i].num_of_containers; j++)
                {

                    planes[i].containers[j].weight = CONTAINER_WEIGHT;

                }

            } // id if statement

        } // planes loop



        if (getpid() == planes_id)
        {

            // planes parent process here

            signal(SIGUSR1,planes_sig);

        }

    } // while loop

}

void init_planes(CargoPlane planes[])
{

    for(int i = 0; i < cargo_planes; i++)
    {

        srand(time(NULL));

        planes[i].num_of_containers = rand() % (max_containers - min_containers + 1) ;

        planes[i].num_of_containers += min_containers;

        for(int j = 0; j < planes[i].num_of_containers; j++)
        {

            planes[i].containers[j].weight = CONTAINER_WEIGHT;

        }

    }

}



void init_height_and_check(CargoPlane planes[],int i, int j)
{

    srand(time(NULL));

    planes[i].containers[j].height = rand() % (max_container_height-min_container_height+1);

    planes[i].containers[j].height += min_container_height;

    // check if its in safty, in between or in danger height

    if (planes[i].containers[j].height <= saftey_height)
    {

        //printf("The container reached safely\n");

        write_containers_GL(i,planes[i].num_of_containers);



        // Here pause time for dropping

        srand(time(NULL));

        int pausTime = rand() % (max_drop_period - min_drop_period + 1);

        pausTime += min_drop_period;

        //printf("planes process sleeps %d seconds\n",pausTime);

        sleep(pausTime);



        // write on the shared memory

        sem_wait(semid); // while it is less than or equal 0, keep waiting



        // then do semid = semid - 1

        int index = 0;

        bool full = false;

        while (shm_ptr1[index].weight != 0)
        {

            if (index == 99)
            {

                full = true;

                break;

            }

            index++;

        }

        if (full == false)
        {

            shm_ptr1[index].weight = planes[i].containers[j].weight;

            container_gl container;

            container.index = index;

            container.weight = shm_ptr1[index].weight;

            write_drop(i,container);

        }



        sem_post(semid); // semid = semid + 1



    }
    else if (planes[i].containers[j].height > saftey_height &&

             planes[i].containers[j].height < danger_height)
    {

        write_containers_GL(i,planes[i].num_of_containers);



        // init the loss amount of white flour

        srand(time(NULL));

        planes[i].containers[j].loss_amount = rand()%(max_container_loss-min_container_loss + 1) + min_container_loss;



        planes[i].containers[j].weight -= planes[i].containers[j].loss_amount;

        // Here pause time for dropping

        srand(time(NULL));

        int pausTime = rand() % (max_drop_period - min_drop_period + 1);

        pausTime += min_drop_period;

        //printf("planes process sleeps %d seconds\n",pausTime);

        sleep(pausTime);



        // write on the shared memory

        sem_wait(semid); // while it is less than or equal 0, keep waiting



        // then do semid = semid - 1

        int index = 0;

        bool full = false;

        while (shm_ptr1[index].weight != 0)
        {

            if (index == 99)
            {

                full = true;

                break;

            }

            index++;

        }

        if (full == false)
        {

            shm_ptr1[index].weight = planes[i].containers[j].weight;

            container_gl container;

            container.index = index;

            container.weight = shm_ptr1[index].weight;

            write_drop(i,container);

        }



        sem_post(semid); // semid = semid + 1

    }
    else
    {

        planes[i].containers[j].weight = 0;

        write_containers_GL(i,planes[i].num_of_containers);

        //printf("The container is lost\n");

    }

}



void write_containers_GL(int plane,int containers)
{

    char fifo_name[20] = "plane-GL";

    char fifo_num[20];

    sprintf(fifo_num,"%d",plane);

    strcat(fifo_name,fifo_num);

    char message[20];



    if (containers == 0)

        strcpy(message,"refilling");

    else
    {

        sprintf(message,"%d",containers);

        strcat(message," containers");

    }



    int f;

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



void write_drop(int i, container_gl container)
{

    // write to the fifo

    char fifo_name[20] = "plane-GL";

    char fifo_num[20];

    sprintf(fifo_num,"%d",i);

    strcat(fifo_name,fifo_num);

    strcat(fifo_name,"B");

    int f;



    // open the fifo

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



void planes_sig(int signo)
{

    for (int i = 0; i < cargo_planes; i++)
    {

        // kill all planes processes

        kill(planes[i].id,SIGKILL);

    }

    kill(planes_id,SIGKILL);

}



void plane_write_plane(int i)
{

    // write to the fifo

    char fifo_name[20] = "plane-parent";

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

