#include "header.h"



void init_fifoiat()
{

    for (int i = 0; i < cargo_planes; i++)
    {

        // for the number of remained containers in each plabe

        char fifo_name[20] = "plane-GL";

        char fifo_num[20];

        sprintf(fifo_num,"%d",i);

        strcat(fifo_name,fifo_num);

        if(mkfifo(fifo_name,0666) == -1)
        {

            perror("Fifo Error\n");

            exit(-1);

        }

        // for writing the container on the ground

        strcat(fifo_name,"B");

        if(mkfifo(fifo_name,0666) == -1)
        {

            perror("Fifo Error\n");

            exit(-1);

        }

        // for counting the number of shut containers

        char fifo_name2[20] = "plane-parent";

        strcat(fifo_name2,fifo_num);

        if(mkfifo(fifo_name2,0666) == -1)
        {

            perror("Fifo Error\n");

            exit(-1);

        }





    }



    for (int i = 0; i < collect_workers; i++)
    {

        char fifo_name[20] = "collect-GL";

        char fifo_num[20];

        sprintf(fifo_num,"%d",i);

        strcat(fifo_name,fifo_num);

        if(mkfifo(fifo_name,0666) == -1)
        {

            perror("Fifo Error\n");

            exit(-1);

        }

        strcat(fifo_name,"B");

        if(mkfifo(fifo_name,0666) == -1)
        {

            perror("Fifo Error\n");

            exit(-1);

        }

        char fifo_name3[20] = "collector-parent";

        strcat(fifo_name3,fifo_num);

        if(mkfifo(fifo_name3,0666) == -1)
        {

            perror("Fifo Error\n");

            exit(-1);

        }



        strcat(fifo_name,"C");

        if(mkfifo(fifo_name,0666) == -1)
        {

            perror("Fifo Error\n");

            exit(-1);

        }



        strcat(fifo_name,"D");

        if(mkfifo(fifo_name,0666) == -1)
        {

            perror("Fifo Error\n");

            exit(-1);

        }



        char fifo_name2[20] = "collect-split";

        char fifo_num2[20];

        sprintf(fifo_num2,"%d",i);

        strcat(fifo_name2,fifo_num2);

        if(mkfifo(fifo_name2,0666) == -1)
        {

            perror("Fifo Error\n");

            exit(-1);

        }

    }



    for (int i = 0; i < split_workers; i++)
    {

        char fifo_name[20] = "split-GL";

        char fifo_num[20];

        sprintf(fifo_num,"%d",i);

        strcat(fifo_name,fifo_num);

        if(mkfifo(fifo_name,0666) == -1)
        {

            perror("Fifo Error\n");

            exit(-1);

        }

        strcat(fifo_name,"B");

        if(mkfifo(fifo_name,0666) == -1)
        {

            perror("Fifo Error\n");

            exit(-1);

        }



        strcat(fifo_name,"C");

        if(mkfifo(fifo_name,0666) == -1)
        {

            perror("Fifo Error\n");

            exit(-1);

        }

    }



    for (int i = 0; i< dstrb_workers; i++)
    {

        char fifo_name[20] = "distrib-GL";

        char fifo_num[20];

        sprintf(fifo_num,"%d",i);

        strcat(fifo_name,fifo_num);

        if(mkfifo(fifo_name,0666) == -1)
        {

            perror("Fifo Error\n");

            exit(-1);

        }

        strcat(fifo_name,"B");

        if(mkfifo(fifo_name,0666) == -1)
        {

            perror("Fifo Error\n");

            exit(-1);

        }

        char fifo_name4[20] = "distributer-parent";

        strcat(fifo_name4,fifo_num);

        if(mkfifo(fifo_name4,0666) == -1)
        {

            perror("Fifo Error\n");

            exit(-1);

        }



        strcat(fifo_name,"C");

        if(mkfifo(fifo_name,0666) == -1)
        {

            perror("Fifo Error\n");

            exit(-1);

        }



        strcat(fifo_name,"D");

        if(mkfifo(fifo_name,0666) == -1)
        {

            perror("Fifo Error\n");

            exit(-1);

        }



        char fifo_name2[20] = "dstrb-split";

        char fifo_num2[20];

        sprintf(fifo_num2,"%d",i);

        strcat(fifo_name2,fifo_num2);

        if(mkfifo(fifo_name2,0666) == -1)
        {

            perror("Fifo Error\n");

            exit(-1);

        }

    }



    for (int i = 0; i< num_of_families; i++)
    {

        char fifo_name[20] = "family-GL";

        char fifo_num[20];

        sprintf(fifo_num,"%d",i);

        strcat(fifo_name,fifo_num);

        if(mkfifo(fifo_name,0666) == -1)
        {

            perror("Fifo Error\n");

            exit(-1);

        }

        char fifo_name6[20] = "family-parent";

        strcat(fifo_name6,fifo_num);

        if(mkfifo(fifo_name6,0666) == -1)
        {

            perror("Fifo Error\n");

            exit(-1);

        }

    }

}

