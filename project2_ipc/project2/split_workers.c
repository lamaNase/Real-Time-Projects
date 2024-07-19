#include "header.h"


void simulateSplitWorkers() {
  
  init_split();
  
  for (int i = 0; i < split_workers; i++) {
    
    pid_t child = fork();
    
    if (child == -1) {
	
	printf("faild to fork split workers\n");
	
	exit(-1);
	
      } else if (child == 0) {
	
	Split_worker_array[i].id = getpid();
	
	//sleep(3);
	
	break;
	
      }
    
    sleep(2);
    
  }

  while(1) {
    
    for (int i = 0; i< split_workers; i++) {
      
      if (getpid() == Split_worker_array[i].id) {
	
	while (Split_worker_array[i].switched == 0) {
	  
	  shm_ptr2=createShm(id2);
	  
	  shm_ptr3=creatShmInt(id3);
	  
	  
	  
	  srand(time(NULL));
	  
	  int pauseTime2 = rand() % (max_splitter_sleep - min_splitter_sleep+ 1);
	  
	  pauseTime2 += min_splitter_sleep;
	  
	  sleep(pauseTime2);
	  
	  
	  
	  check_switch(i);
	  
	  if (Split_worker_array[i].switched == 1)
	    
	    break;
	  
	  
	  
	  sem_wait(semid2);
	  
	  int index = 0;
	  
	  bool empty = false;
	  
	  while (shm_ptr2[index].weight == 0) {
	      
	    if (index == 99) {
	      
	      // the sharef memory is empty
	      
	      empty = true;
	      
	      break;
	      
	    }
	    
	    index++;
	    
	  }
	  
	  int weight=0;
	  
	  if (empty == false) {
	    
	    weight = shm_ptr2[index].weight;
	    
	    // splitting into bags
	    
	    num_of_bags=weight;
	    
	    container_gl container;
	    
	    container.index = index;
	    
	    container.weight = weight;
	    
	    shm_ptr2[index].weight =0; // this container has been splitted
	    
	    split_read(i, container);
	    
	  }
	  
	  
	  
	  sem_post(semid2); // semid2 = semid2+ 1
	  
	  /////////////////////////////////// The collector will sleep Now
	  
	  srand(time(NULL));
	  
	  int pauseTime = rand() % (max_splitter_sleep - min_splitter_sleep+ 1);
	  
	  pauseTime += min_splitter_sleep;
	  
	  //printf("Splitter process sleeps %d seconds\n",pauseTime);
	  
	  sleep(pauseTime);
	  
	  
	  
	  
	  sem_wait(semid3); // while it is less than or equal 0, keep waiting
	  
	  int index2 = 0;
	  
	  bool full = false;
	  
	  while (shm_ptr3[index2] != 0) {
	    
	    if (index2 == 99) {
	      
	      // the sharef memory is full
	      
	      full = true;
	      
	      break;
	      
	    }
	    
	    index2++;
	    
	  }
	  
	  if (full == false && weight != 0) {
	    
	    for(int k=0; k<weight; k++)
	      
	      shm_ptr3[index2+k] =1;
	    
	    bags_gl bag;
	    
	    bag.index = index2;
	    
	    bag.num_of_bags = weight;
	    
	    split_write(i,bag);
	    
	  }
	  
	  // post the semaphore after writting all bags
	  
	  sem_post(semid3); // semid = semid + 1
	  
	}
	
      }
      
    }
        
    if (getpid() == splitter_workers_id) {
      
      // splitter parent process here
      
      signal(SIGUSR1,splitter_sig);
      
    }

  }// ene of while(1)
  
}



void  init_split() {

  for (int i = 0; i< split_workers; i++) {

    Split_worker_array[i].switched = 0;
    
  }
  
}



void  split_read(int i, container_gl container) {
  
  char fifo_name[20] = "split-GL";
  
  char fifo_num[20];
  
  sprintf(fifo_num,"%d",i);
  
  strcat(fifo_name,fifo_num);
  
  int f;
  
  if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1) {
    
    perror("open error\n");
    
    exit(-1);
    
  } else {

    if (write(f,&container,sizeof(container_gl)) == -1) {
      
      perror("write error\n");
      
      exit(-1);
      
    }
    
  }

}



void split_write(int i,bags_gl bag) {

  char fifo_name[20] = "split-GL";
  
  char fifo_num[20];
  
  sprintf(fifo_num,"%d",i);
  
  strcat(fifo_name,fifo_num);
  
  strcat(fifo_name,"B");
  
  
  
  int f;
  
  if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1) {
    
    perror("open error\n");
    
    exit(-1);
    
  } else {
    
    if (write(f,&bag,sizeof(bags_gl)) == -1) {

	perror("write error\n");
	exit(-1);
	
      }
      
      }
    
  }



void check_switch(int k) {
  
  int f;
  
  bool flag = false;
  
  for (int j = 0; j < collect_workers; j++) {
    
    char fifo_name[20] = "collect-split";
    
    char fifo_num[20];
    
    sprintf(fifo_num,"%d",j);
    
    strcat(fifo_name,fifo_num);
    
    
    
    char message[20];
    
    if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
      {
	
	perror("open error\n");
	
	exit(-1);
	
      }
    else
      {
	
	if (read(f,&message,sizeof(message)) != -1)
	  {
	    
	    if (strcmp(message,"martyered") == 0)
	      {
		
                    if(Split_worker_array[k].switched == 0)
		      {
			
                        Split_worker_array[k].switched = 1;

                        disappear_split(k);
			
                        break;

		      }
		    
	      }
	    
	  }
	else
	  {
	    
	    close(f);
	    
	    if (j == collect_workers - 1)
	      
	      flag = true;
	    
	  }
	
      }
    
  } // end of collect workers loop

  if (flag == false)
    
    close(f);
  
  
  
  int f2;
  
  bool flag2 = false;
  
  for (int j = 0; j < dstrb_workers; j++)
    {
      
        char fifo_name[20] = "dstrb-split";
	
        char fifo_num[20];
	
        sprintf(fifo_num,"%d",j);
	
        strcat(fifo_name,fifo_num);
	
	
	
        char message[20];
	
        if ((f2 = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
	  {
	    
            perror("open error\n");
	    
            exit(-1);
	    
	  }
        else
        {

            if (read(f2,&message,sizeof(message)) != -1)
            {

                if (strcmp(message,"martyered") == 0)
                {

                    if(Split_worker_array[k].switched == 0)
                    {

                        Split_worker_array[k].switched = 1;

                        disappear_split(k);

                        break;

                    }

                }

            }
            else
            {

                close(f2);

                if (j == collect_workers - 1)

                    flag2 = true;

            }

        }

    } // end of dstrb workers loop

    if (flag2 == false)

        close(f2);

}



void disappear_split(int i) {
  
  int f;
  
  char fifo_name[20] = "split-GL";
  
  char fifo_num[20];
  
  sprintf(fifo_num,"%d",i);
  
  strcat(fifo_name,fifo_num);
  
  strcat(fifo_name,"BC");
  
  
  
  char message[20] = "disappear";
  
  if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1) {
    
    perror("open error\n");
    
    exit(-1);

  } else {
    
    if (write(f,&message,sizeof(message)) == -1) {
      
      perror("write error\n");
      
      exit(-1);
      
    }

  }
  
}

///////////////////
void splitter_sig(int signo) {
  
  for (int i = 0; i < split_workers; i++) {
    
    // kill all planes processes
    
    kill(Split_worker_array[i].id,SIGKILL);
    
  }
  
  kill(splitter_workers_id,SIGKILL);
  
}
 
