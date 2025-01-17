#include "header.h"
#include "employee.c"
shm_cell *shm_ptr1;
sem_t *semid;

void simulate_production_lines(){
  semid = creatSemaphore("line_To_line"); 
  init_production_lines(); 
  for (int i = 0; i < num_of_lines; i++){
    pid_t child = fork();
    if (child < 0){
      exit(-1);
    } else if (child == 0){
      production_lines[i].id = getpid(); 
      shm_ptr1 = createShm(id); 
      sleep(3);
      
      /// create workers before produce medicines  
      for(int j=0;j<num_of_employees;j++){
	employee* emp = (employee*) malloc(sizeof(employee));
	emp->line = &production_lines[i]; 
	emp->num = j;
	emp->hold_medicine = false;
	emp->switched = false;
	pthread_create(&emp->id, NULL, Employee_work, (void *)emp);    
	sem_wait(semid); // while it is less than or equal 0, keep waiting  
	shm_ptr1[i].Threads_Array[j] = emp->id;  
	shm_ptr1[i].employees[j].num = j;
	shm_ptr1[i].employees[j].hold_medicine = false;
	shm_ptr1[i].employees[j].switched = false;
	shm_ptr1[i].employees[j].line = &production_lines[i]; 
	sem_post(semid); // semid = semid + 1
      } 
      
      // this is the production line, and it will produce the medicinne in some speed  
      if (production_lines[i].is_liquid == true){ 
	// liquid medicine 
	for (int j = 0; j < num_of_medicines; j++){    
	  srand(time(NULL));
	  int level = rand();
	  level = level % (liquid_level_max_range - liquid_level_min_range + 1);	
	  level += liquid_level_min_range;
	  
	  production_lines[i].liquid_medicines[j].liquid_level = level;
	  //////////////////////
	  srand(time(NULL));
	  int color = rand();
	  color = color % (max_color - min_color + 1);	
	  color += min_color;
	  production_lines[i].liquid_medicines[j].color_matched =color;
	  //////////////////////////////////////////////////
	  srand(time(NULL));
	  int sealed = rand();
	  sealed = sealed % (max_sealed - min_sealed + 1);	
	  sealed += min_sealed;
	  production_lines[i].liquid_medicines[j].sealed =sealed;
	  
	  /////////////////////////////////////////////////////
	  srand(time(NULL));
	  int label = rand();
	  label = label % (max_label_matched - min_label_matched + 1);	
	  label += min_label_matched;
	  production_lines[i].liquid_medicines[j].label_matched = label;
	  ///////////////////////////////////////////////////
	  srand(time(NULL));
	  int expiry = rand();
	  expiry = expiry % (max_expiry_date- min_expiry_date + 1);	
	  expiry += min_expiry_date;
	  production_lines[i].liquid_medicines[j].expiry_date =expiry ;
	  
	  production_lines[i].liquid_medicines[j].produced = true;
	  production_lines[i].production_count++;   
	  production_lines[i].produced_not_read++;
	   ////////////////////////////////////////////////////////////////////
	   // write on the shared memory
	  sem_wait(semid); // while it is less than or equal 0, keep waiting
	  shm_ptr1[i].production_count = production_lines[i].produced_not_read;
	  sem_post(semid); // semid = semid + 1
	  //////////////////////////////////////////////////////////////////

	  line_write_parent(i);
	  // write to the openGL
	  line_write_liquid(i,production_lines[i].liquid_medicines[j]);
	  // check if the line need more employees
	  if(production_lines[i].produced_not_read >= need_help){ 
	    sem_wait(semid); // while it is less than or equal 0, keep waiting 
	    int min_prod_count = shm_ptr1[0].production_count; 
	    int min_index=0;
	    for(int k = 0; k < num_of_lines; k++) { 
	      if(shm_ptr1[k].production_count < min_prod_count && 
		 shm_ptr1[k].numOfEmployees > 1) {
		min_prod_count = shm_ptr1[k].production_count;
		min_index=k;
	      }
	    }   
	    if (shm_ptr1[min_index].numOfEmployees > 1) {
	      shm_ptr1[min_index].numOfEmployees--; 
	      shm_ptr1[i].numOfEmployees++;  
	      shm_ptr1[min_index].employees[shm_ptr1[min_index].numOfEmployees - 1].switched = true;
	      production_lines[i].has_extra_employees = true;
	      
	      // create new thread for this line
	      employee* emp = (employee*) malloc(sizeof(employee));
	      emp->line = &production_lines[i]; 
	      emp->num = shm_ptr1[i].numOfEmployees - 1;
	      emp->hold_medicine = false;
	      emp->switched = false;
	      pthread_create(&emp->id, NULL, Employee_work, (void *)emp);  
	      shm_ptr1[i].Threads_Array[shm_ptr1[i].numOfEmployees] = emp->id; 
	      
	    } // end of if employees > 0 statement
	    ////////////////////////////////////////////////////////////////////////
	    sem_post(semid); // semid = semid + 1
	    // check if the line now does not need extra employees
	  } else if (production_lines[i].produced_not_read <= not_need_help && 
		     production_lines[i].has_extra_employees == true){
	    production_lines[i].has_extra_employees = false;
	    sem_wait(semid); // while it is less than or equal 0, keep waiting 
	    for (int k = 0; k < shm_ptr1[i].numOfEmployees; k++){
	      if (shm_ptr1[i].employees[k].line->num != i) shm_ptr1[shm_ptr1[i].employees[k].line->num].employees[k].switched = false;
	      pthread_kill(shm_ptr1[i].Threads_Array[k],SIGKILL);	
	    }
	    sem_post(semid); // semid = semid + 1
	  }
	  int amountOfSleep=25-production_lines[i].speed; 
	  sleep(amountOfSleep);
	}
      } else if(production_lines[i].is_liquid == false) { 
	//printf("This is a pill medicine Production line ....................\n"); 
	for (int j = 0; j < num_of_medicines; j++){   
	  ////////////////////////////////////////////////////////////
	  srand(time(NULL));
	  int pill_size = rand();
	  pill_size = pill_size % (max_pill_size_matched- min_pill_size_matched + 1);	
	  pill_size += min_pill_size_matched;
	  production_lines[i].pill_medicines[j].pill_size_matched =pill_size ;
	  /////////////////////////////////////////////////////////////////////
	  srand(time(NULL));
	  int count_pill = rand();
	  count_pill = count_pill % (max_pill_count_matched- min_pill_count_matched + 1);	
	  count_pill += min_pill_count_matched;
	  production_lines[i].pill_medicines[j].pill_count_matched =count_pill ;
	  ////////////////////////////////////////////////////////////////////
	  srand(time(NULL));
	  int color = rand();
	  color = color % (max_color - min_color + 1);	
	  color += min_color;
	  production_lines[i].pill_medicines[j].color_matched =color;
	  
	  //////////////////////////////////////////////////////////////////
	  srand(time(NULL));
	  int expiry = rand();
	  expiry = expiry % (max_expiry_date- min_expiry_date + 1);	
	  expiry += min_expiry_date;
	  production_lines[i].pill_medicines[j].expiry_date =expiry ;
	 
	   production_lines[i].pill_medicines[j].produced = true;
	  production_lines[i].production_count++;   
	  production_lines[i].produced_not_read++;
	   ////////////////////////////////////////////////////////////////
	  // write on the shared memory
	  sem_wait(semid); // while it is less than or equal 0, keep waiting
	  shm_ptr1[i].production_count = production_lines[i].produced_not_read;
	  sem_post(semid); // semid = semid + 1
	  /////////////////////////////////////////////////////////////////
	  // write to fifo
	  line_write_parent(i);
	  //write to the openGL
	  line_write_pill(i,production_lines[i].pill_medicines[j]);
	  if(production_lines[i].produced_not_read >= need_help){ 
	    sem_wait(semid); // while it is less than or equal 0, keep waiting 
	    int min_prod_count = shm_ptr1[0].production_count; 
	    int min_index=0;
	    for(int k = 0; k < num_of_lines; k++) { 
	      
	      if(shm_ptr1[k].production_count < min_prod_count && 
		 shm_ptr1[k].numOfEmployees > 1) {
		min_prod_count = shm_ptr1[k].production_count;
		min_index=k;
	      }
	    }   
	    if (shm_ptr1[min_index].numOfEmployees > 1) {
	      shm_ptr1[min_index].numOfEmployees--; 
	      shm_ptr1[i].numOfEmployees++;  
	      shm_ptr1[min_index].employees[shm_ptr1[min_index].numOfEmployees - 1].switched = true;
	      production_lines[i].has_extra_employees = true;
	      // create new thread for this line
	      employee* emp = (employee*) malloc(sizeof(employee));
	      emp->line = &production_lines[i]; 
	      emp->num = shm_ptr1[i].numOfEmployees - 1;
	      emp->hold_medicine = false;
	      pthread_create(&emp->id, NULL, Employee_work, (void *)emp);   
	      shm_ptr1[i].Threads_Array[shm_ptr1[i].numOfEmployees] = emp->id; 
	      
	    } // end of if employees > 0 statement
	    ////////////////////////////////////////////////////////////////////////
	    sem_post(semid); // semid = semid + 1
	  } else if (production_lines[i].produced_not_read <= not_need_help && 
		     production_lines[i].has_extra_employees == true){
	    production_lines[i].has_extra_employees = false;
	    sem_wait(semid); // while it is less than or equal 0, keep waiting 
	    for (int k = 0; k < shm_ptr1[i].numOfEmployees; k++){
	      if (shm_ptr1[i].employees[k].line->num != i)
		shm_ptr1[shm_ptr1[i].employees[k].line->num].employees[k].switched = false;
	      pthread_kill(shm_ptr1[i].Threads_Array[k],SIGKILL);
	    }
	    sem_post(semid); // semid = semid + 1
	  }
	  int amountOfSleep = 25 - production_lines[i].speed; 
	  sleep(amountOfSleep);
	}
      }
      exit(-1);
    } 
    sleep(2);
  }
  while (getpid() == production_lines_id){
    // planes parent process here
    signal(SIGUSR1,lines_sig);
  }
}

void init_production_lines(){
  for (int i = 0; i < num_of_lines; i++){
    if (i % 2 == 0){
      production_lines[i].is_liquid = false;
      for (int j = 0; j < num_of_medicines; j++){
	production_lines[i].pill_medicines[j].produced = false;
	production_lines[i].pill_medicines[j].read = false;
      }
    } else {
      production_lines[i].is_liquid = true;
      for (int j = 0; j < num_of_medicines; j++){
	production_lines[i].liquid_medicines[j].produced = false;
	production_lines[i].liquid_medicines[j].read = false;
	production_lines[i].liquid_medicines[j].package.folded_prescriptions_attached = false;
      }
    }
    production_lines[i].production_count = 0;
    production_lines[i].out_of_spec_count = 0; 
    production_lines[i].produced_not_read=0; /// initialization by maha alarm   
    production_lines[i].has_extra_employees = false; 
    for(int r;r<MAX_ARRAY_SIZE;r++){ 
      production_lines[i].Package[r].capacity=0;
    }
    production_lines[i].numOfEmployees= num_of_employees;
    srand(time(NULL));
    production_lines[i].speed = rand() % (max_line_speed - min_line_speed + 1);
    production_lines[i].speed += min_line_speed;
    production_lines[i].num = i;
  }
}

void line_write_liquid(int i,liquid_medicine liq_medicine){
  char fifo_name[20] = "Lines-GL";
  char fifo_num[20];
  sprintf(fifo_num,"%d",i);
  strcat(fifo_name,fifo_num);
  int f;
  if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1){
    perror("can't open the fifo\n");
    exit(-1);
  } else if (write(f,&liq_medicine,sizeof(liq_medicine)) == -1){
    perror("write error\n");
    exit(-1);
  }
}

void line_write_pill(int i,pill_based_medicine pill_medicine){
  char fifo_name[20] = "Lines-GL";
  char fifo_num[20];
  sprintf(fifo_num,"%d",i);
  strcat(fifo_name,fifo_num);
  int f;
  if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1){
    perror("can't open the fifo\n");
    exit(-1);
  } else if (write(f,&pill_medicine,sizeof(pill_based_medicine)) == -1){
    perror("write error\n");
    exit(-1);
  }
}

void lines_sig(int signo) {
  for (int i = 0; i < num_of_lines; i++) 
    kill(production_lines[i].id,SIGKILL);
  kill(production_lines_id,SIGKILL);
} 

/////////////////////////////////

void line_write_parent(int i) {
  // write to the fifo
  char fifo_name[20] = "line-parent";
  char fifo_num[20];
  sprintf(fifo_num,"%d",i);
  strcat(fifo_name,fifo_num);
  int f;
  int num = 1;
  // open the fifo
  if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1) {
    perror("open error line in production \n");
    exit(-1);
  } else {
    if (write(f,&num,sizeof(int)) == -1) {
      perror("write error\n");
      exit(-1);
    }
  }
}
