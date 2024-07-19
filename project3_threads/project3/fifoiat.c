#include "header.h"

void init_fifoiat() {
	for (int i = 0; i < num_of_lines; i++) {
		char fifo_name[20] = "Lines-GL";
		char fifo_num[20];
		sprintf(fifo_num,"%d",i);
		strcat(fifo_name,fifo_num);
		if(mkfifo(fifo_name,0666) == -1) {
			perror("Fifo Error\n");
			exit(-1);
		}
		char fifo_name6[20] = "line-parent";
		strcat(fifo_name6,fifo_num);
		if(mkfifo(fifo_name6,0666) == -1) {
			perror("Fifo Error\n");
			exit(-1);
		}
		
	}
	int employees_in_each_line = num_of_lines * num_of_employees;
	for (int k = 0; k < num_of_lines;k++){
		for (int j = 0; j < employees_in_each_line;j++){
			char fifo_name7[20] = "Lines-GL";
			char fifo_num4[20];
			sprintf(fifo_num4,"%d",k);
			strcat(fifo_name7,fifo_num4);
			strcat(fifo_name7,"-");
			char fifo_num5[20];
			sprintf(fifo_num5,"%d",j);
			strcat(fifo_name7,fifo_num5);
			if(mkfifo(fifo_name7,0666) == -1) {
				perror("Fifo Error\n");
				exit(-1);
			}
		}
	}
    
	for (int i = 0; i< num_of_lines; i++) {
		if (i % 2 != 0){
			char fifo_name6[20] = "botteled-parent";
			char fifo_num[20];
			sprintf(fifo_num,"%d",i);
			strcat(fifo_name6,fifo_num);
			if(mkfifo(fifo_name6,0666) == -1) {
				perror("Fifo Error\n");
				exit(-1);
			}
		} else {
			char fifo_name6[20] = "pill-parent";
			char fifo_num[20];
			sprintf(fifo_num,"%d",i);
			strcat(fifo_name6,fifo_num);
			if(mkfifo(fifo_name6,0666) == -1) {
				perror("Fifo Error\n");
				exit(-1);
			}
		}
	}		
}

void add_employee(int line, int emp) {
	char fifo_name[20] = "Lines-GL";
	char fifo_num[20];
	sprintf(fifo_num,"%d",line);
	strcat(fifo_name,fifo_num);
	strcat(fifo_name,"-");
	char fifo_num2[20];
	sprintf(fifo_num2,"%d",emp);
	strcat(fifo_name,fifo_num2);
	
	if(mkfifo(fifo_name,0666) == -1) {
		perror("Fifo Error\n");
		exit(-1);
	}
}

void delete_employee(int line,int emp) {
	char fifo_name[20] = "Lines-GL";
	char fifo_num[20];
	sprintf(fifo_num,"%d",line);
	strcat(fifo_name,fifo_num);
	strcat(fifo_name,"-");
	char fifo_num2[20];
	sprintf(fifo_num2,"%d",emp);
	strcat(fifo_name,fifo_num2);
	
	remove(fifo_name);
}

