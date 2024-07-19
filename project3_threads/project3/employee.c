#include "header.h"

void *Employee_work(void *vargp){
  employee* emp = (employee*) vargp;
  production_line* line = emp->line; 
  while (true) {
    if (emp->line->is_liquid == true) {
      for (int j = 0; j < num_of_medicines; j++){  
	bool switched = shm_ptr1[emp->line->num].employees[emp->num].switched;
	if(emp->line->liquid_medicines[j].produced== true && emp->line->liquid_medicines[j].read == false &&
	   switched == false){
	  int check_working_on= pthread_mutex_lock(&product_mutex);  
	  if ( check_working_on != EBUSY ) { 	
	    if (emp->num == 4)
	      if (emp->line->liquid_medicines[j].liquid_level > liquid_level_max_range_normal || 
		  emp->line->liquid_medicines[j].liquid_level <  liquid_level_min_range_normal || 
		  emp->line->liquid_medicines[j].color_matched > max_color_normal  || 
		  emp->line->liquid_medicines[j].color_matched < min_color_normal || 
		  emp->line->liquid_medicines[j].sealed > max_sealed_normal  || 
		  emp->line->liquid_medicines[j].sealed < min_sealed_normal ||
		  emp->line->liquid_medicines[j].label_matched > max_label_matched_normal ||
		  emp->line->liquid_medicines[j].label_matched < min_label_matched_normal ||
		  emp->line->liquid_medicines[j].expiry_date > max_expiry_date_normal||
		  emp->line->liquid_medicines[j].expiry_date < min_expiry_date_normal) {
		
	      	emp->line->out_of_spec_count++;
	      	emp->line->liquid_medicines[j].out_of_spec = true;
	      } else
	      	emp->line->liquid_medicines[j].out_of_spec = false;
	    emp->line->liquid_medicines[j].read = true; 
	    emp->line->produced_not_read--;
	    emp->line->liquid_medicines[j].num = j;
	    // the employee taked the medicine
	    emplyee_write_liquid_GL(emp->line->num,emp->num,emp->line->liquid_medicines[j]);
	  } else
	    continue;
	  // unlock  , I taked the medicine, no need for the mutex now
	  pthread_mutex_unlock(&product_mutex);
	  
	  // sleep time
	  srand(time(NULL));
	  int pausTime = rand() % (max_inspection_time - min_inspection_time+ 1);
	  pausTime += min_inspection_time;
	  sleep(pausTime);
          
	  // packedging
	  if(emp->line->liquid_medicines[j].out_of_spec == false) {
	    emp->line->liquid_medicines[j].package.folded_prescriptions_attached = true;
	    emplyee_write_liquid_GL(emp->line->num,emp->num,emp->line->liquid_medicines[j]);
	    
	    int pausTime = rand() % (max_packaging_time - min_packaging_time+ 1);
	    pausTime += min_packaging_time;
	    sleep(pausTime);
	  } else if(emp->line->liquid_medicines[j].out_of_spec == true){
	    //printf("medicine out of specification liquid \n ");
	    emp->line->liquid_medicines[j].package.folded_prescriptions_attached=false;
	    // write to fifo 
	    check_out_of_specification_liquid(emp->line->num);
	  }
	  break;
	} // end of if produced = true and read = false
      } // end of medicines liquide loop 	
    } else if (emp->line->is_liquid == false) {
      for (int j = 0; j < num_of_medicines; j++){  
	bool switched = shm_ptr1[emp->line->num].employees[emp->num].switched;
	if(emp->line->pill_medicines[j].produced== true && emp->line->pill_medicines[j].read == false &&
	   switched == false){ 
	  int check_working_on= pthread_mutex_lock(&product_mutex);  
	  //printf("Here 3*-************------------*********\n");
	  if ( check_working_on != EBUSY ) { 	
	    //printf("Here 4*-************------------*********\n");
	    //    if (emp->num == 4)
	    
	    if (emp->line->pill_medicines[j].pill_count_matched > max_pill_count_matched_normal || 
		emp->line->pill_medicines[j].pill_count_matched < min_pill_count_matched_normal ||
		emp->line->pill_medicines[j].color_matched > max_color_normal || 
		emp->line->pill_medicines[j].color_matched < min_color_normal ||
		emp->line->pill_medicines[j].pill_size_matched > max_pill_size_matched_normal ||
		emp->line->pill_medicines[j].pill_size_matched < min_pill_size_matched_normal ||  
		emp->line->pill_medicines[j].expiry_date > max_expiry_date_normal||
		emp->line->pill_medicines[j].expiry_date < min_expiry_date_normal) {
	      emp->line->out_of_spec_count++;
	      emp->line->pill_medicines[j].out_of_spec = true;
	      
	    } else 
	      emp->line->pill_medicines[j].out_of_spec = false;
	    emp->line->pill_medicines[j].read = true; 
	    emp->line->produced_not_read--;
	    emp->line->pill_medicines[j].num = j;
	    // the employee taked this medicine
	    //sleep(1);
	    emplyee_write_pill_GL(emp->line->num,emp->num,emp->line->pill_medicines[j]);
	  } else
	    continue;
	  // unlock  , I taked the medicine, no need for the mutex now
	  pthread_mutex_unlock(&product_mutex);
	  
	  // sleep time
	  srand(time(NULL));
	  //inspection time
	  int pausTime = rand() % (max_inspection_time - min_inspection_time+ 1);
	  pausTime += min_inspection_time;
	  //printf("sleeeep %d\n",pausTime);
	  sleep(pausTime);
          
	  
	  // packedging
	  if(emp->line->pill_medicines[j].out_of_spec == false){
	    /* 	for(int a;a<MAX_ARRAY_SIZE;a++){
		if(emp->line->Package[a].capacity<package_capacity){
		emp->line->Package[a].capacity++;
		break;
		}
		}*/
	    emp->line->pill_medicines[j].package.folded_prescriptions_attached=true;
	    int pausTime = rand() % (max_packaging_time - min_packaging_time+ 1);
	    pausTime += min_packaging_time;
	    sleep(pausTime);
	  } else if(emp->line->pill_medicines[j].out_of_spec == true){
	    //printf("medicine out of specification pill \n ");
	    emp->line->pill_medicines[j].package.folded_prescriptions_attached=false;
	    // write to fifo 
	    check_out_of_specification_pill(emp->line->num);
                }      
	  break;
	}
      } // end of num_of_medicines loop */
    } // end of pill medicine
    sleep(6);
    // } // end of if id statement 
    // } // end of for loop
  } // end of while
}

void check_out_of_specification_liquid(int i) {
  // write to the fifo
  char fifo_name[20] = "botteled-parent";
  char fifo_num[20];
  sprintf(fifo_num,"%d",i);
  strcat(fifo_name,fifo_num);
  int f;
  int num = 1;
  // open the fifo
  if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1) {
    perror("open error line \n");
    exit(-1);
  } else {
    if (write(f,&num,sizeof(int)) == -1) {
      perror("write error\n");
      exit(-1);
    }
  }
}

void check_out_of_specification_pill(int i) {
  // write to the fifo
  char fifo_name[20] = "pill-parent";
  char fifo_num[20];
  sprintf(fifo_num,"%d",i);
  strcat(fifo_name,fifo_num);
  int f;
  int num = 1;
  // open the fifo
  if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1) {
    perror("open error line out of spec\n");
    exit(-1);
  }
  else {
    if (write(f,&num,sizeof(int)) == -1) {
      perror("write error\n");
      exit(-1);
    }
  }
}

void emplyee_write_pill_GL(int line_num,int emp_num,pill_based_medicine pill_medicine){
  char fifo_name[20] = "Lines-GL";
  char fifo_num[20];
  sprintf(fifo_num,"%d",line_num);
  strcat(fifo_name,fifo_num);
  strcat(fifo_name,"-");
  char fifo_num2[20];
  sprintf(fifo_num2,"%d",emp_num);
  strcat(fifo_name,fifo_num2);
  int f;
  if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1) {
    printf("employee %d------------\n",emp_num);
    perror("open error line pill GL\n");
    exit(-1);
  } else {
    if (write(f,&pill_medicine,sizeof(pill_based_medicine)) == -1) {
      perror("write error\n");
      exit(-1);
    }
  }
}

void emplyee_write_liquid_GL(int line_num,int emp_num,liquid_medicine liquid_medicine){
  char fifo_name[20] = "Lines-GL";
  char fifo_num[20];
  sprintf(fifo_num,"%d",line_num);
  strcat(fifo_name,fifo_num);
  strcat(fifo_name,"-");
  char fifo_num2[20];
  sprintf(fifo_num2,"%d",emp_num);
  strcat(fifo_name,fifo_num2);
  int f;
  if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1) {
    printf("employee %d------------\n",emp_num);
    perror("open error line liquid GL \n");
    exit(-1);
  } else {
    if (write(f,&liquid_medicine,sizeof(liquid_medicine)) == -1) {
      perror("write error\n");
      exit(-1);
    }
  }
}
