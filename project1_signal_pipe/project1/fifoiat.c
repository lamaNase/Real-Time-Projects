#include "header.h"

void fifoat(){

// create fifo's for communication
	if(mkfifo("A-leader-1",0666) == -1) {
		perror("Fifo Error\n");
		exit(-1);  	
	}  
	
	if(mkfifo("A-leader-1G",0666) == -1) {
		perror("Fifo Error\n");
		exit(-1);  	
	}  
	
	if(mkfifo("A12",0666) == -1) {
		perror("Fifo Error\n");
		exit(-1);
	} 
	if(mkfifo("A12G",0666) == -1) {
		perror("Fifo Error\n");
		exit(-1);
	}
	
	 if(mkfifo("A23",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	} 
	if(mkfifo("A23G",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	} 
	
	if(mkfifo("A34",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	} 
	if(mkfifo("A34G",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	} 
	
	if(mkfifo("A45",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	} 
	
	if(mkfifo("A45G",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	}
	
	 if(mkfifo("A-leader-5G",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	}        /////// Now fifo's for Team B

	 if(mkfifo("A-leader-5",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	}        /////// Now fifo's for Team B

 	if(mkfifo("B-leader-1",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	} 
	if(mkfifo("B-leader-1G",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	}
	
	if(mkfifo("B12",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	}  
	if(mkfifo("B12G",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	} 
	
	if(mkfifo("B23",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	}
	if(mkfifo("B23G",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	} 
	
	 if(mkfifo("B34",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	}  
	if(mkfifo("B34G",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	} 
	
	if(mkfifo("B45",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	} 
	if(mkfifo("B45G",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	} 
	
	if(mkfifo("B-leader-5",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	} 
	if(mkfifo("B-leader-5G",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	}  
	
	if(mkfifo("leader-A-To-leader-B",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	}  
	
	if(mkfifo("leader-A-To-leader-BG",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	} 
	if(mkfifo("leader-B-To-leader-A",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	}  
	
	if(mkfifo("leader-B-To-leader-AG",0666)==-1) {
		perror("Fifo Error\n");
		exit(-1);
	} 
	
	if(mkfifo("To_team_leader_A",0666)==-1) {
      		perror("open Error\n");
		exit(-1);
	} 
	if(mkfifo("To_team_leader_AG",0666)==-1) {
      		perror("open Error\n");
		exit(-1);
	}  
	
	if(mkfifo("To_team_leader_BG",0666)==-1) {
      		perror("open Error\n");
		exit(-1);
	}  
	
     if(mkfifo("To_team_leader_B",0666)==-1) {
       		perror("open Error\n");
		exit(-1);
	} 

}
