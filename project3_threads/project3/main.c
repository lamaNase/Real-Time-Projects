#include "header.h"
#include "openGL.c"
#include "production_lines.c"
#include "fifoiat.c"

int simulation_time = 0;
int num_of_medicines = 0;
int num_of_lines = 0;
int min_line_speed = 0;
int max_line_speed = 0;
int num_medicine_type_threshold = 0;
int liquid_level_min_range = 0;
int liquid_level_max_range = 0;
int liquid_level_min_range_normal = 0;
int liquid_level_max_range_normal = 0;
int botteled_out_of_spec_threshold = 0;
int pill_out_of_spec_threshold = 0;
pid_t production_lines_id;
int num_of_employees = 0;

int min_inspection_time=0;
int max_inspection_time =0;
int min_packaging_time =0;
int max_packaging_time =0;
int number_of_pills_per_package =0;
int number_of_workers_threshold;
int package_capacity;

int min_color =0;
int max_color =0;
int min_color_normal =0;
int max_color_normal =0;
int min_number_of_pills_per_package =0;
int max_number_of_pills_per_package =0;
int min_number_of_pills_per_package_normal =0;
int max_number_of_pills_per_package_normal =0;
int min_sealed =0;
int max_sealed =0;
int min_sealed_normal =0;
int max_sealed_normal =0;
int min_label_matched =0;
int max_label_matched =0;
int min_label_matched_normal =0;
int max_label_matched_normal =0;
int min_expiry_date =0;
int max_expiry_date =0;
int min_expiry_date_normal =0;
int max_expiry_date_normal =0;
int min_pill_size_matched =0;
int max_pill_size_matched =0;
int min_pill_size_matched_normal =0;
int max_pill_size_matched_normal =0;
int produced_medicine_to_switch =0; 

int min_pill_count_matched =0;
int max_pill_count_matched =0;
int min_pill_count_matched_normal =0;
int max_pill_count_matched_normal =0;

int produce_counter_liquid = 0;
int produce_counter_pill = 0;
int botteled_out_of_spec_counter=0;
int pill_out_of_spec_counter=0; 

int need_help=0;
int id = 0;  
int not_need_help = 0;

int main(int argc, char **argv){
  if (argc != 2) {
    printf("Missing arguments\n");
    exit(-1);
  }
  // Read all required  arguments from the file
  read_arguments(argv[1]);
  init_fifoiat();
  signal(SIGALRM,handle_alarm);
  alarm(simulation_time);
  
  id = ftok(".",'H'); 
  shm_cell *shm_ptr1 = createShm(id);  
  semid = creatSemaphore("line_To_line"); 
  for(int i=0;i<num_of_lines;i++) {
    shm_ptr1[i].numOfEmployees = num_of_employees ;
    shm_ptr1[i].production_count = 0;
    for(int j=0;j<num_of_employees;j++) { 
      shm_ptr1[i].employees[j].switched = false;             
    }
  }
  // create multi processes
  pid_t child = fork();
  if (child < 0){
    perror("Faild forking\n");
    exit(-1);
  } else if (child == 0){
    // this is openGL process
    init_openGL(argc,argv);
  } else {
    // Here is the parent
    pid_t child2 = fork();
    if (child2 < 0){
      perror("Faild forking\n");
      exit(-1);
    } else if (child2 == 0){
      // this is production lines process
      production_lines_id = getpid();
      simulate_production_lines();
    } 
  }
  // end of simulation 
  while (1){
    check_num_of_medicine_type();
    check_num_of_out_of_spec_threshold();
  }
  return 0;
}

void read_arguments(char *argv){
  FILE *file = fopen(argv,"r");
  if (file == NULL) {
    printf("failed to open the file\n");
    exit(-1);
  } else {
    char line[MAX_LINE_LENGTH];
    while (fgets(line,MAX_LINE_LENGTH,file) != NULL) {
      char *token = strtok(line," ");
      if (strcmp(token,"simulation_time") == 0) {
	token = strtok(NULL,"\n");
	simulation_time = atoi(token);
      } else if (strcmp(token,"num_of_medicines") == 0) {
	token = strtok(NULL,"\n");
	num_of_medicines = atoi(token);
      } else if (strcmp(token,"num_of_lines") == 0) {
	token = strtok(NULL,"\n");
	num_of_lines = atoi(token);
      } else if (strcmp(token,"min_line_speed") == 0) {
	token = strtok(NULL,"\n");
	min_line_speed = atoi(token);
      } else if (strcmp(token,"max_line_speed") == 0) {
	token = strtok(NULL,"\n");
	max_line_speed = atoi(token);
      } else if (strcmp(token,"num_medicine_type_threshold") == 0) {
	token = strtok(NULL,"\n");
	num_medicine_type_threshold = atoi(token);
      } else if (strcmp(token,"liquid_level_min_range") == 0) {
	token = strtok(NULL,"\n");
	liquid_level_min_range = atoi(token);
      } else if (strcmp(token,"liquid_level_max_range") == 0) {
	token = strtok(NULL,"\n");
	liquid_level_max_range = atoi(token);
      } else if (strcmp(token,"liquid_level_min_range_normal") == 0) {
	token = strtok(NULL,"\n");
	liquid_level_min_range_normal = atoi(token);
      } else if (strcmp(token,"liquid_level_max_range_normal") == 0) {
	token = strtok(NULL,"\n");
	liquid_level_max_range_normal = atoi(token);
      } else if (strcmp(token,"botteled_out_of_spec_threshold") == 0) {
	token = strtok(NULL,"\n");
	botteled_out_of_spec_threshold = atoi(token);
      } else if (strcmp(token,"pill_out_of_spec_threshold") == 0) {
	token = strtok(NULL,"\n");
	pill_out_of_spec_threshold = atoi(token);
      } else if (strcmp(token,"num_of_employees") == 0) {
	token = strtok(NULL,"\n");
	num_of_employees = atoi(token);
      } else if (strcmp(token,"min_inspection_time") == 0) {
	token = strtok(NULL,"\n");
	min_inspection_time = atoi(token);
      } else if (strcmp(token,"max_inspection_time") == 0) {
	token = strtok(NULL,"\n");
	max_inspection_time = atoi(token);
      } else if (strcmp(token,"min_packaging_time") == 0) {
	token = strtok(NULL,"\n");
	min_packaging_time = atoi(token);
      } else if (strcmp(token,"max_packaging_time") == 0) {
	token = strtok(NULL,"\n");
	max_packaging_time = atoi(token);
      } else if (strcmp(token,"number_of_pills_per_package") == 0) {
	token = strtok(NULL,"\n");
	number_of_pills_per_package = atoi(token);
      } else if (strcmp(token,"min_color") == 0) {
	token = strtok(NULL,"\n");
	min_color = atoi(token);
      } else if (strcmp(token,"max_color") == 0) {
	token = strtok(NULL,"\n");
	max_color = atoi(token);
      }else if (strcmp(token,"min_color_normal") == 0) {
	token = strtok(NULL,"\n");
	min_color_normal = atoi(token);
      } else if (strcmp(token,"max_color_normal") == 0) {
	token = strtok(NULL,"\n");
	max_color_normal = atoi(token);
      } else if (strcmp(token,"min_number_of_pills_per_package") == 0) {
	token = strtok(NULL,"\n");
	min_number_of_pills_per_package = atoi(token);
      } else if (strcmp(token,"max_number_of_pills_per_package") == 0) {
	token = strtok(NULL,"\n");
	max_number_of_pills_per_package = atoi(token);
      } else if (strcmp(token,"min_number_of_pills_per_package_normal") == 0) {
	token = strtok(NULL,"\n");
	min_number_of_pills_per_package_normal = atoi(token);
      } else if (strcmp(token,"max_number_of_pills_per_package_normal") == 0) {
	token = strtok(NULL,"\n");
	max_number_of_pills_per_package_normal = atoi(token);
      } else if (strcmp(token,"min_sealed") == 0) {
	token = strtok(NULL,"\n");
	min_sealed = atoi(token);
      } else if (strcmp(token,"max_sealed") == 0) {
	token = strtok(NULL,"\n");
	max_sealed = atoi(token);
      } else if (strcmp(token,"min_sealed_normal") == 0) {
	token = strtok(NULL,"\n");
	min_sealed_normal = atoi(token);
      } else if (strcmp(token,"max_sealed_normal") == 0) {
	token = strtok(NULL,"\n");
	max_sealed_normal = atoi(token);
      }  else if (strcmp(token,"min_label_matched") == 0) {
	token = strtok(NULL,"\n");
	min_label_matched = atoi(token);
      } else if (strcmp(token,"max_label_matched") == 0) {
	token = strtok(NULL,"\n");
	max_label_matched = atoi(token);
      } else if (strcmp(token,"min_label_matched_normal") == 0) {
	token = strtok(NULL,"\n");
	min_label_matched_normal = atoi(token);
      }  else if (strcmp(token,"max_label_matched_normal") == 0) {
	token = strtok(NULL,"\n");
	max_label_matched_normal = atoi(token);
      } else if (strcmp(token,"min_expiry_date") == 0) {
	token = strtok(NULL,"\n");
	min_expiry_date = atoi(token);
      }   else if (strcmp(token,"max_expiry_date") == 0) {
	token = strtok(NULL,"\n");
	max_expiry_date = atoi(token);
      } else if (strcmp(token,"min_expiry_date_normal") == 0) {
	token = strtok(NULL,"\n");
	min_expiry_date_normal = atoi(token);
      }  else if (strcmp(token,"max_expiry_date_normal") == 0) {
	token = strtok(NULL,"\n");
	max_expiry_date_normal = atoi(token);
      } else if (strcmp(token,"min_pill_size_matched") == 0) {
	token = strtok(NULL,"\n");
	min_pill_size_matched = atoi(token);
      }  else if (strcmp(token,"max_pill_size_matched") == 0) {
	token = strtok(NULL,"\n");
	max_pill_size_matched = atoi(token);
      } else if (strcmp(token,"min_pill_size_matched_normal") == 0) {
	token = strtok(NULL,"\n");
	min_pill_size_matched_normal = atoi(token);
      }  else if (strcmp(token,"max_pill_size_matched_normal") == 0) {
	token = strtok(NULL,"\n");
	max_pill_size_matched_normal = atoi(token);
      }  else if (strcmp(token,"produced_medicine_to_switch ") == 0) {
	token = strtok(NULL,"\n");
	produced_medicine_to_switch  = atoi(token);
      }  else if (strcmp(token,"min_pill_count_matched ") == 0) {
	token = strtok(NULL,"\n");
	min_pill_count_matched  = atoi(token);
      }  else if (strcmp(token,"max_pill_count_matched ") == 0) {
	token = strtok(NULL,"\n");
	max_pill_count_matched  = atoi(token);
      }  else if (strcmp(token,"min_pill_count_matched_normal ") == 0) {
	token = strtok(NULL,"\n");
	min_pill_count_matched_normal  = atoi(token);
      }  else if (strcmp(token,"need_help") == 0) {
	token = strtok(NULL,"\n");
	need_help  = atoi(token);
      } else if (strcmp(token,"not_need_help") == 0) {
	token = strtok(NULL,"\n");
	not_need_help  = atoi(token);
      } else if (strcmp(token,"package_capacity") == 0) {
	token = strtok(NULL,"\n");
	package_capacity  = atoi(token);
      }                                                                                                                                                                  	
      
    } // end of while
  } // end of else
}

void handle_alarm(int sig) {
  // send signals to child processes
  kill(production_lines_id,SIGUSR1);
  // kill main process
  exit(EXIT_SUCCESS);
  
}

void check_num_of_medicine_type() {
  int num,f;
  for (int i = 0; i < num_of_lines; i++) {
    char fifo_name[20] = "line-parent";
    char fifo_num[20];
    sprintf(fifo_num,"%d",i);
    strcat(fifo_name,fifo_num);
    if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1) {
      perror("open error parent line\n");
      exit(-1);
    } else {
      if (read(f,&num,sizeof(int)) != -1) {
	if (num == 1) {
	  if (i % 2 == 0)
	    produce_counter_pill++;
	  else
	    produce_counter_liquid++;
	}
      }
      close(f);
    }
    if (produce_counter_pill > num_medicine_type_threshold) {
      printf("The number of produced pill medicines has exceeds the threshold\n");
      kill(production_lines_id,SIGUSR1);
      // kill main process
      exit(EXIT_SUCCESS);
    } else if (produce_counter_liquid > num_medicine_type_threshold){
      printf("The number of produced liquid medicines has exceeds the threshold\n");
      kill(production_lines_id,SIGUSR1);
      // kill main process
      exit(EXIT_SUCCESS);
    }
  }
}

void check_num_of_out_of_spec_threshold() {
  int num,f;
  for (int i = 0; i < num_of_lines; i++) {
    if (i % 2 == 0) {
      char fifo_name[20] = "pill-parent";
      char fifo_num[20];
      sprintf(fifo_num,"%d",i);
      strcat(fifo_name,fifo_num);
      if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1) {
	perror("open error parent line pill\n");
	exit(-1);
      } else {
	if (read(f,&num,sizeof(int)) != -1) {
	  if (num == 1) 
	    pill_out_of_spec_counter++;
	}
	close(f);
      }
      if (pill_out_of_spec_counter > pill_out_of_spec_threshold) {
	printf("The number pill out  of specification has exceeds the threshold\n");
	kill(production_lines_id,SIGUSR1);
	// kill main process
	exit(EXIT_SUCCESS);
      } 
    } else {
      char fifo_name[20] = "botteled-parent";
      char fifo_num[20];
      sprintf(fifo_num,"%d",i);
      strcat(fifo_name,fifo_num);
      if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1) {
	perror("open error parent line botteled\n");
	exit(-1);
      } else {
	if (read(f,&num,sizeof(int)) != -1) {
	  if (num == 1) 
	    botteled_out_of_spec_counter++;
	}
	close(f);
      }
      if (botteled_out_of_spec_counter > botteled_out_of_spec_threshold){
	printf("The number liquid medicines out  of specification has exceeds the threshold\n");
	kill(production_lines_id,SIGUSR1);
	// kill main process
	exit(EXIT_SUCCESS);
      }
    }
  }
} 
