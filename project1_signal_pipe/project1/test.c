#include "header.h"
#include "openGL.h"

#define numberOfTeams 2
#define numberOfPlayers 5 // the number of players per team
// create a struct for players
typedef struct
{
    pid_t player_number;
    pid_t pid;
    int   Energy_level; 
    int has_Ball ;
} player;


// functions prototypes of parent process
void GameRounds(int argc, char *argv[]);
void GameTimeRead(int argc, char *argv[]);
void EnergyBounds(int argc, char  *argv[]);
void TeamLeader(char team);
void playerPrinting(char team, int playerNum);
void assignEnergyToPlayers(player players);
void handle_alarm(int sig);
void count_balls();
void start_new_round();


// Define Gloabal Variables
int numberOfRounds;
int GameTime;
int roundCount = 0;
int teamAleaderEnergry;
int teamBleaderEnergy;
int ball=1;
int Team_A_Balls_Count=0; // initiallize the ball count to 0
int Team_B_Balls_Count=0; // initiallize the ball count to 0
player players[numberOfTeams][numberOfPlayers];
pid_t  teams[numberOfTeams];
pid_t gl_pid;
int f;
int Min_Energy_Bound; 
int leaderA_Has_Ball = 0;
int leaderB_Has_Ball = 0;

int main(int argc, char **argv) {
	// PArent process

	// Read Game Arguments
	GameRounds(argc,argv);
	 GameTimeRead(argc,argv);
	 EnergyBounds(argc,argv);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	signal(SIGALRM,handle_alarm);
	alarm(GameTime);

	// make all fifos
	fifoat();

	int to_leader_A = open("To_team_leader_A",O_RDWR |O_NONBLOCK);
	if(to_leader_A == -1) {
        		perror("open Error\n");
		exit(-1);
	} else  {
		int bytes = write(to_leader_A,&ball,sizeof(int));
    		if (bytes == -1){
    			perror("write Error\n");
			exit(-1);
    		} else
    			printf("parentt process threw the ball to leader A\n");
    		//close(to_leader_A);
	}

    	int to_leader_B= open("To_team_leader_B",O_RDWR |O_NONBLOCK);
	if(to_leader_B == -1) {
       		perror("open Error\n");
		exit(-1);
	} else {
    		int bytes = write(to_leader_B,&ball,sizeof(int));
    		if (bytes == -1){
    			perror("write Error\n");
			exit(-1);
    		} else
    			printf("parentt process threw the ball to leader B\n");
    	}

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// make 3 processes for leader A, leader B and for graphics
	pid_t A_pid = fork();
	if (A_pid < 0){
		printf("can't create leader A\n");
        		return 0;
        	} else if (A_pid == 0){
        		// here is the pid of leader A
      	  	teams[0] = getpid();
        	} else { // parent
      	  	pid_t B_pid = fork();
        		if (B_pid < 0){
      	  		printf("can't create leader A\n");
        			return 0;
        		} else if (B_pid == 0){
        			// here is the pid of leader A
      	  		teams[1] = getpid();
        		}  else { //parent
			pid_t gl_pid1 = fork();
			printf("openGL is forked\n");
			if (gl_pid1 < 0){
				printf("Failing create graphic process");
				return 0;
			} else if (gl_pid1 == 0) // graphics process
					initGraphics(argc,argv);

        		}
       	}

	// processes forked ////////////////////////////////////////////////////////////
	while(numberOfRounds>0) {
		 if(getpid() == teams[0]) {	 // Here is process leader A
		// receive the ball from the parent
            		int fd1= open("To_team_leader_A",O_RDWR | O_NONBLOCK);
            		if (fd1 == -1){
            			perror("open Error\n");
			exit(-1);
            		} else {
            			int num;
            			int bytes = read(fd1,&num,sizeof(int));
            			if (bytes == -1){
            				perror("read Error\n");
				//exit(-1);
            			} else  {
            			        if (num == ball) {
            			      printf("Leader A Recieved the ball............\n"); 
            			      leaderA_Has_Ball=1;
            			      Team_A_Balls_Count++;
            	                                          int fd1G;
            			        if ((fd1G=open("To_team_leader_AG",O_RDWR|O_NONBLOCK ) ) == -1){
            			        	perror("open Error\n");
				exit(-1);
            			        } else {
            			        	if ((write(fd1G,&ball,sizeof(int))) == -1){
            			        		perror("write Error\n");
					exit(-1);
            			        	}
            			        }
            			}
            			        else {
            				printf("leader A did not get the ball\n");
				exit(-1);
            			         }
            			}
            			close(fd1); 
            			TeamLeader('A');
            		}

			// generate a random number to the energy level to the team leader
		int pauseTimeLeaderA=rand()%(teamAleaderEnergry +5) + 3;
             	srand(time(NULL));
                	teamAleaderEnergry = rand() % 30 + 1; // Random pause between 100 to 3
                	printf("Team A Leader Energy : %d \n",teamAleaderEnergry);
                	printf("Team A leader sleep %d ................\n",pauseTimeLeaderA);
                	sleep(pauseTimeLeaderA);
		////////////////////////////Team A leader will write ball to player 1
                		int Apw1;
                		if ((Apw1 =open("A-leader-1",O_RDWR | O_NONBLOCK)) == -1){
                		perror("open Error\n");
			exit(-1);
                		} else  if (leaderA_Has_Ball  == 1){
                			if (write(Apw1,&ball,sizeof(int)) ==  -1){
                			perror("w Error\n");
				exit(-1);
                			} else {
                			printf("team A leader throw the ball to player 1 \n");
                                        		//\leaderA_Has_Ball=0;
                			}
                		}
                		if (numberOfRounds == 0)
                		exit(-1);

		players[0][0].pid=fork();
                     	if(players[0][0].pid == 0){
                    		// this is player 1 in team A
                       		playerPrinting('A',1);
                       		assignEnergyToPlayers(players[0][0]);
                                  	 // player 1 Recieve the ball from the leader Team A
                            		int Arp1 = open("A-leader-1",O_RDWR);
                              		 if(Arp1==-1){
                          		perror("open error \n");
                              		exit(-1);
                               		}  else{
                               			int number;
                               			if((read(Arp1,&number,sizeof(int))) == -1){
                               			perror("read error player 1 \n ");
                               			exit(-1);
                               			} else{
                               				if(number==ball) {
                               		    		printf("player1A recieved the ball .......\n"); 
                               		    		players[0][0].has_Ball=1;
                               			     		int Apw1;
                				     		if ((Apw1=open("A-leader-1G",O_RDWR|O_NONBLOCK))== -1){
                						perror("open Error\n");
							exit(-1);
                					    	} else {
                							if (write(Apw1,&ball,sizeof(int)) ==  -1){
                							perror("w Error\n");
								exit(-1);
                							}
                						}

                               				} else{
                               	 			printf("Player 1 didnt recieve ball ....\n");
                               				exit(-1);
                               				}
                               			}
                         			close(Arp1);
                          		}
			if (players[0][0].has_Ball == 1){
                         		   	int pauseTimeP1= rand()%(players[0][0].Energy_level+3);
                            			pauseTimeP1=pauseTimeP1+4;
                            			printf("player 1 sleep %d seconds ...........\n",pauseTimeP1);
                            			sleep(pauseTimeP1);
			}
			
                            ////////////////////////////Team A player 1will write ball to player 2
                			int p12;
                			if ((p12=open("A12",O_RDWR | O_NONBLOCK)) == -1){
                			perror("open Error\n");
				exit(-1);
                			} else if (players[0][0].has_Ball == 1){
                				if (write(p12,&ball,sizeof(int)) ==  -1){
                				perror("write Error\n");
					exit(-1);
                				} else {
                				printf("team player 1 throw the ball to player 2 \n"); 
                				players[0][0].has_Ball=0;
                				printf("Player 1 A Herreeeeeeeeeeeeeeeeee\n");;

                				}
                			}
                    }

	else if ( players[0][0].pid > 0){
		//wait(NULL);
		// this is team A leader , we will fork player 2
   		players [0][1].pid =fork();
   		if(players[0][1].pid ==0) {

   			// this is player 2 in Team A
   			      // player 2 Recieve the ball from player 1 Team A
                             		int pr12 = open("A12",O_RDWR );
                              		if(pr12==-1){
                               		perror("open error A12 \n");
                               		exit(-1);
                               		}  else{
                               			int number;
                               			int byte= read(pr12,&number,sizeof(int));
                               			if(byte==-1){
                               		perror("read error  1 2\n ");
                               		exit(-1);
                               		}
                               		else{
                               			if(number==ball){
                               			printf("player 2A recieved the ball ..........\n"); 
                               			 players[0][1].has_Ball=1;
                               			//////////////////////////////////////////////////////////////////////////////////////////

                               			     int Apw2;
                				     if ((Apw2=open("A12G",O_RDWR))== -1){
                					perror("open Error\n");
						exit(-1);
                				     } else {
                						if (write(Apw2,&ball,sizeof(int)) ==  -1){
                						perror("w Error\n");
							exit(-1);
                						}
                				}
                               			////////////////////////////////////////////////////////////////////////////////////////////////////////
                               			}else{
                               		 	printf("Player 2A didn't recieve the ball ................\n");
                               			exit(-1);
                               			}
                               		close(pr12);
                               		}

		}
		if (players[0][1].has_Ball == 1){
                            		int pauseTimeP2= rand()%(players[0][1].Energy_level+1)+4;
			//pauseTimeP2=pauseTimeP2+1;
                            		printf("player 2A sleep %d seconds ...........\n",pauseTimeP2);
                            		sleep(pauseTimeP2);
		}
		
		// player 2 throw the ball to player 3
		   ////////////////////////////Team A player 2 will write ball to player 3
                			int p23;
                			if ((p23=open("A23",O_RDWR | O_NONBLOCK )) == -1){
                			perror("open Error\n");
				exit(-1);
                			} else if (players[0][1].has_Ball == 1){
                				if (write(p23,&ball,sizeof(int)) ==  -1){
                				perror("write Error\n");
					exit(-1);
                				} else {
                				printf("team player 2A throw the ball to player 3 \n"); 
                				players[0][1].has_Ball=0;
                				//close(p23);
                				}
                			}
   	}
   	else if (players [0][1].pid < 0 ){
   		printf ("player 2A forking faild \n ");
   		return (0);
   	}
   	else {
   		
   		// team A leader will fork player 3
   		players[0][2].pid =fork();
     	       	if (players[0][2].pid==0){
      			 // this is player 3

      			 // player 3 Recieve the ball from player 2 Team A
                             int pr23= open("A23",O_RDWR );
                               if(pr23==-1){
                               perror("open error \n");
                               exit(-1);
                               }
                               else{
                               		int number;
                               		int byte= read(pr23,&number,sizeof(int));
                               		if(byte==-1){
                               		perror("read error 2 3 \n ");
                               	exit(-1);
                               		}
                               		else{
                               			if(number==ball) {
                               			printf("player 3A recieved the ball ..........\n"); 
                               			 players[0][2].has_Ball=1;
                               			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                               			     int Apw3;
                				     if ((Apw3=open("A23G",O_RDWR|O_NONBLOCK))== -1){
                					perror("open Error\n");
						exit(-1);
                				     } else {
                						if (write(Apw3,&ball,sizeof(int)) ==  -1){
                						perror("w Error\n");
							exit(-1);
                						}
                				}
                               			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                               		 }else{
                               		 	printf("Player 3A didnt recieve the ball ................\n");
                               		exit(-1);
                               		}
                               		}
                               		close(pr23);
                               }
		if (players[0][2].has_Ball == 1){
                            		int pauseTimeP3= rand()%(players[0][2].Energy_level+1)+4;
                            	printf("player 3A sleep %d seconds ...........\n",pauseTimeP3);
                            	sleep(pauseTimeP3);
                            	}
		// player 3 throw the ball to player 4

		 ////////////////////////////Team A player 3 will write ball to player 4
                		int p34;
                		if ((p34=open("A34",O_RDWR| O_NONBLOCK)) == -1){
                		perror("open Error\n");
			exit(-1);
                		} else if(players[0][2].has_Ball == 1) {
                			if (write(p34,&ball,sizeof(int)) ==  -1){
                			perror("write Error\n");
				exit(-1);
                			} else {
                			printf("team player 3A throw the ball to player 4 \n"); 
                			players[0][2].has_Ball=0;
                			}
                		}

       }
       		else if (players[0][3].pid <0){
       		printf("player 3A forking failed \n ");
       			return (0);
       		}
       		else {
       			//wait(NULL);
       			// team A leader will fork player 4
       			players[0][3].pid =fork();
       			if (players[0][3].pid ==0){
       			// this is player 4 in team A
       			// player 4 recieve the ball from player 3
                               		int pr34;
                			if ((pr34=open("A34",O_RDWR )) == -1){
                			perror("open Error\n");
				exit(-1);
                			}  else {
                               			int number;
                               			int byte= read(pr34,&number,sizeof(int));
                               			if(byte==-1){
                               			perror("read error 3 4 \n ");
                               			exit(-1);
                               			}  else {
                               				if(number==ball){
                               				printf("player 4A recieved the ball ..........\n"); 
                               				 players[0][3].has_Ball=1;
                               				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                               			     int Apw4;
                				     if ((Apw4=open("A34G",O_RDWR|O_NONBLOCK))== -1){
                					perror("open Error\n");
						exit(-1);
                				     } else {
                						if (write(Apw4,&ball,sizeof(int)) ==  -1){
                						perror("w Error\n");
							exit(-1);
                						}
                				}
                               				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                               				}else{
                               				printf("Player 4A didnt recieve the ball ................\n");
                               				exit(-1);
                               				}
                               			}
                               	close(pr34);
                            	  	}
		if (players[0][3].has_Ball == 1){
                            		int pauseTimeP4= rand()%(players[0][3].Energy_level+1)+4;
                          	printf("player 4A sleep %d seconds ...........\n",pauseTimeP4);
                            	sleep(pauseTimeP4);
			// player 4 throw the ball to player 5
		}
 			////////////////////////////Team A player 4 will write ball to player 5
                			int p45;
                			if ((p45=open("A45",O_RDWR| O_NONBLOCK)) == -1){
                			perror("open Error\n");
				exit(-1);
                			} else if (players[0][3].has_Ball == 1){
                				if (write(p45,&ball,sizeof(int)) ==  -1){
                				perror("write Error\n");
					exit(-1);
                				} else {
                				printf("team player 4A throw the ball to player 5 \n"); 
                				players[0][3].has_Ball=0;
                				//close(p45);
                				}
                			}

       		} else if (players[0][3].pid <0){
       			printf("player 4 in team A forking failed \n");
       			return(0);
       		} else {
       			//wait(NULL);
       			// team A leader , and 5th player will be forked
       			players[0][4].pid =fork();
       			if (players[0][4].pid == 0){
       				// this is player 5 in Team A
       				// player 5 recieve the ball from player 4
                               			int pr45;
                				if ((pr45=open("A45",O_RDWR)) == -1){
                				perror("open Error\n");
					exit(-1);
                				} else {
                               				int number;
                               				int byte= read(pr45,&number,sizeof(int));
                               				if(byte==-1){
                               				perror("read error  4  5\n ");
                               				exit(-1);
                               				} else {
                               					if(number==ball){
                               					printf("player 5A recieved the ball ..........\n"); 
                               					 players[0][4].has_Ball=1;
                               					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                               			     int Apw5;
                				     if ((Apw5=open("A45G",O_RDWR|O_NONBLOCK))== -1){
                					perror("open Error\n");
						exit(-1);
                				     } else {
                						if (write(Apw5,&ball,sizeof(int)) ==  -1){
                						perror("w Error\n");
							exit(-1);
                						}
                				}
                               					////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                               					}else{
                               		 			printf("Player 5A didnt recieve the ball \n");
                               					exit(-1);
                               					}
                               				}
                               		close(pr45);
                              			}
			if(players[0][4].has_Ball == 1){
                          		int pauseTimeP5= rand()%(players[0][4].Energy_level+1)+2;
                            	printf("player 5A sleep %d seconds ...........\n",pauseTimeP5);
                            	sleep(pauseTimeP5);
                            	}
			// player 5 throw the ball to team leader
			////////////////////////////Team A player 5 will write ball to the team leader
                			int p5ToLeader ;
                			if ((p5ToLeader =open("A-leader-5",O_RDWR | O_NONBLOCK)) == -1){
                			perror("open Error\n");
				exit(-1);
                			} else if (players[0][4].has_Ball == 1){
                				if (write(p5ToLeader,&ball,sizeof(int)) ==  -1){
                				perror("w Error\n");
					exit(-1);
                				} else {
                				printf("team player 5A throw the ball to the team leader \n"); 
                				players[0][4].has_Ball=0;
                				//close(p5ToLeader);
                				}
                			}

    		   } else if (players[0][4].pid <0){
       			printf("team A player 4 forking failed \n");
       			return (0);
       		} else {
      			 // this is team leader finnish forking 5 players
       			//wait(NULL);
       			printf("I'm team A leader, and I finnish forking ............................\n");
       			pid_t child_1 = fork();
       			if (child_1 < 0){
       				printf("failed forking\n");
       				exit(-1);
       			} else if (child_1 == 0){
       				// child
       				// team A leader recieved the ball back from player 5
                               			int p5ToLeader ;
                				if ((p5ToLeader =open("A-leader-5",O_RDWR)) == -1){
                				perror("open Error\n");
						exit(-1);
                				} else {
                               				int number;
                               				int byte= read(p5ToLeader,&number,sizeof(int));
                               				if(byte==-1){
                               				perror("read error  A 5\n ");
                               				exit(-1);
                               				} else {
                               					if(number==ball){
                               				printf("teamA leader got ball back .\n");
                               				leaderA_Has_Ball = 1;
                               			     		int Lead5;
                				     		if ((Lead5=open("A-leader-5G",O_RDWR|O_NONBLOCK))== -1){
                					perror("open Error\n");
						exit(-1);
                				     } else {
                						if (write(Lead5,&ball,sizeof(int)) ==  -1){
                						perror("w Error\n");
							exit(-1);
                						}
                				}
                               				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                               			}else{
                               		 	printf("teamA leader  didnt recieve the ball ...\n");
                               			exit(-1);
                               			}
                               			}
                               		close(p5ToLeader);
                             		  }
                             		  if (leaderA_Has_Ball  == 1){
                		sleep(pauseTimeLeaderA);
                		printf("A sleeps %d s before throw to B leader 	\n",pauseTimeLeaderA);
		}
			////////////////////////////Team A leader will write ball to the team leader B
                			int TAtoTB ;
                			if ((TAtoTB =open("leader-A-To-leader-B",O_RDWR | O_NONBLOCK)) == -1){
                			perror("open Error\n");
				exit(-1);
                			} else {
                				if (write(TAtoTB,&ball,sizeof(int)) ==  -1){
                				perror("write Error\n");
					exit(-1);
                				} else {
                				printf("leader A throw the ball to the team leader  B \n"); 
                				leaderA_Has_Ball=0;
                				Team_A_Balls_Count--;
                				//close(TAtoTB);
                				}
                			}
       			} else {
       				// Leader A
       				pid_t child_2 = fork();
       				if (child_2 < 0 ){
       					printf("faild forking\n");
       					exit(-1);
       				} else if (child_2 == 0){
       					//child
       					// leader A recives from leader B
                				int BA = open("leader-B-To-leader-A",O_RDWR );
                				if (BA == -1){
                			perror("open Error BA \n");
				exit(-1);
                				} else {
                				int num;
                				if ((read(BA,&num,sizeof(int))) == -1){
                				perror("read Error BA \n");
					exit(-1);
                				} else 
                					 if (num == ball){
                			    		printf("Leader A recived from leader B");
                			    		leaderA_Has_Ball = 1;
                			    		Team_A_Balls_Count++;
                					printf("Ball Count = %d \n",Team_B_Balls_Count);
                					numberOfRounds--;
                				    		//write on the openGL fifo
                				   int BAG = open("leader-B-To-leader-AG",O_RDWR|O_NONBLOCK);
                				   if (BAG == -1){
                				perror("open Error BAG \n");
					exit(-1);
                				   } else {
                				   	if (write(BAG,&ball,sizeof(int)) == -1){
                				   	perror("write Error BAG \n");
						exit(-1);
                				   	}
                				   }
                			                  }
                			}
                			int Aw1 = open("A-leader-1",O_RDWR | O_NONBLOCK);
                			if (Aw1 == -1){
                				perror("open Error A1 \n");
					exit(-1);
                			} else if (leaderA_Has_Ball  == 1){
                				if (write(Aw1,&ball,sizeof(int)) == -1){
                				perror("write Error A1 \n");
					exit(-1);
                				} else {
                				printf("leader A throw the ball to player 1\n");
                				leaderA_Has_Ball = 0;
                				}
                			}
                			int to_B = open("To_team_leader_B",O_RDWR | O_NONBLOCK);
                			if (to_B == -1){
                			perror("open Error parent-B \n");
				exit(-1);
                			} else {
                				if (write(to_B,&ball,sizeof(int)) == -1){
                				perror("write Error parent-B \n");
					exit(-1);
                				} else {
                				printf("Parent throw a ball to team B\n");
                				Team_B_Balls_Count++;
                				}
                			}
       			} else {
       				//parent (team A leader)
       				//wait(NULL);
       				printf("finish forkin child 1 and 2\n");
       					
       			}
       			}
	
       	}
       	}
       }
   }

}
                    else if(players[0][0].pid<0)
                    {
                        perror("Team A player 1 generation failed ! \n ");
                        return(1);
                    }

                } // the end of team A

         		 if(getpid() == teams[1]) {

          /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        		
        		// receive the ball from the parent
            		int fd1= open("To_team_leader_B",O_RDWR | O_NONBLOCK);
            		if (fd1 == -1){
            			perror("open Error\n");
			exit(-1);
            		} else {
            			int num;
            			int bytes = read(fd1,&num,sizeof(int));
            			if (bytes == -1){
            				perror("read Error parent - B\n");
				//exit(-1);
            			} else  {
            				if (num == ball){
            					      printf("Leader B Recieved the ball............\n"); 
            					      leaderB_Has_Ball=1;
            					      Team_B_Balls_Count++;
            	                                          int fd1G;
            			        if ((fd1G=open("To_team_leader_BG",O_RDWR|O_NONBLOCK ) ) == -1){
            			        	perror("open Error\n");
				exit(-1);
            			        } else {
            			        	if ((write(fd1G,&ball,sizeof(int))) == -1){
            			        		perror("write Error\n");
					exit(-1);
            			        	}
            			        }
            					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            				}else {
            					printf("leader B did not get the ball\n");
					exit(-1);
            				}
            			}
            			close(fd1);
            			TeamLeader('B');
          		}

          		// generate a random number to the energy level to the team leader
          		int pauseTimeLeaderB=rand()%(teamBleaderEnergy +5) ;
            //    	if (leaderB_Has_Ball == 1){
                	srand(time(NULL));
                	teamBleaderEnergy = rand() % 30 + 1; // Random pause between 100 to 3 seconds
                	printf("Team B Leader Energy : %d \n",teamBleaderEnergy);
                	printf("Team B leader sleep %d ................\n",pauseTimeLeaderB);
                	sleep(pauseTimeLeaderB);
             //   	}
		////////////////////////////Team B leader will write ball to player 1

                		int Apw1;
                		if ((Apw1=open("B-leader-1",O_RDWR | O_NONBLOCK)) == -1){
                		perror("open Error\n");
			exit(-1);
                		} else if (leaderB_Has_Ball == 1){
                			if (write(Apw1,&ball,sizeof(int)) ==  -1){
                			perror("write Error\n");
				exit(-1);
                			} else
                			printf(" Team Leader B  throw to player 1 B\n"); 
                			leaderB_Has_Ball=0;
                		}
		// create players for this team
		if (numberOfRounds == 0)
                		exit(-1);
                    	players[0][0].pid=fork();
                    	if(players[0][0].pid == 0){
                    		// this is player 1 in team B
                       		playerPrinting('B',1);
                       		assignEnergyToPlayers(players[0][0]);
                       		/// player 1 B recieve the ball from the team leader
                       		int Arp1= open("B-leader-1",O_RDWR );
            			if (Arp1 == -1){
            				perror("open Error\n");
				exit(-1);
            			} else {
            				int num;
            				int bytes = read(Arp1,&num,sizeof(int));
            				if (bytes == -1){
            					perror("read Error\n");
					exit(-1);
            				} else  {
            					if (num == ball){
                                    printf("player1B recieved the ball .......\n"); 
                                    players[1][0].has_Ball=1;
                               			     int Bpw1;
                				     if ((Bpw1=open("B-leader-1G",O_RDWR|O_NONBLOCK))== -1){
                					perror("open Error\n");
						exit(-1);
                				     } else {
                						if (write(Bpw1,&ball,sizeof(int)) ==  -1){
                						perror("w Error\n");
							exit(-1);
                						}
                				}


						printf("player 1B recieve ball from leader......\n"); 
						//players[1][0].has_Ball=1;
            					}else {
            						printf("player1B didnt get ball from leader\n");
						exit(-1);
            					}
            				}
            				close(Arp1);
            			}

                            	if (players[1][0].has_Ball == 1){
                            		int pauseTimeP1= rand()%(players[0][0].Energy_level+3);
                            	pauseTimeP1=pauseTimeP1+4;
                            	printf("player 1B sleep %d seconds ...........\n",pauseTimeP1);
                            	sleep(pauseTimeP1);
                            	}
			// player 1 throw the ball to player 2
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                			int p12;
                			if ((p12=open("B12",O_RDWR | O_NONBLOCK)) == -1){
                			perror("open Error\n");
				exit(-1);
                			} else if (players[1][0].has_Ball == 1){
                				if (write(p12,&ball,sizeof(int)) ==  -1){
                				perror("write Error\n");
					exit(-1);
                				} else {
                				printf(" player 1B throw the ball to player 2B\n"); 
                				players[1][0].has_Ball=0;
                				printf("HEeeerreee****************\n");
                				}
                			}
                    }
                  //  }
		else if ( players[0][0].pid > 0){
			//wait(NULL);
			// this is team B leader , we will fork player 2
			players [0][1].pid =fork();
   			if(players[0][1].pid ==0) {
   			// this is player 2 in Team B
   			// player 2 recieve the ball from player1
                       		int pr12= open("B12",O_RDWR );
            			if (pr12 == -1){
            				perror("open Error\n");
				exit(-1);
            			} else {
            				int num;
            				if ((read(pr12,&num,sizeof(int))) == -1){
            					perror("read Error\n");
					exit(-1);
            				} else  {
            					if (num == ball){

                                    printf("player 2B recieved the ball ..........\n"); 
                                    players[1][1].has_Ball=1;
                               			//////////////////////////////////////////////////////////////////////////////////////////

                               			     int Bpw2;
                				     if ((Bpw2=open("B12G",O_RDWR))== -1){
                					perror("open Error\n");
						exit(-1);
                				     } else {
                						if (write(Bpw2,&ball,sizeof(int)) ==  -1){
                						perror("w Error\n");
							exit(-1);
                						}
                				}


					printf("player 2B recieve ball from player 1B..\n"); 
					//players[1][1].has_Ball=1;
            					}
            				
            					else {
            						printf("player 2B didn't recieve ball from  1B\n");
						exit(-1);
            					}
            				}
            				close(pr12);
            			}
                            	if(players[1][1].has_Ball == 1){
                           		int pauseTimeP2= rand()%(players[0][1].Energy_level+1)+4;
			//pauseTimeP2=pauseTimeP2+1;
                            	printf("player 2B sleep %d seconds ...........\n",pauseTimeP2);
                            	sleep(pauseTimeP2);
                            	}
			// player 2 throw the ball to player 3
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			int p23;
                			if ((p23=open("B23",O_RDWR | O_NONBLOCK)) == -1){
                			perror("open Error\n");
				exit(-1);
                			} else if (players[1][1].has_Ball == 1){
                				if (write(p23,&ball,sizeof(int)) ==  -1){
                				perror("write Error\n");
					exit(-1);
                				} else
                				printf(" player 2B throw the ball to player 3B\n"); 
                				players[1][1].has_Ball=0;
                			}

   		}
   		else if (players [0][1].pid < 0 ){
   			printf ("player 2 forking faild \n ");
   			return (0);
   		}
   			else {
   			//wait(NULL);
   			// team B leader will fork player 3
   			players[0][2].pid =fork();
      			 if (players[0][2].pid==0){
       			// this is player 3
       			/// player 3 recieve the ball from player 2

         			int pr23= open("B23",O_RDWR);
            			if (pr23 == -1){
            				perror("open Error\n");
				exit(-1);
            			} else {
            				int num;
            				if ((read(pr23,&num,sizeof(int))) == -1){
            					perror("read Error\n");
					exit(-1);
            				} else  {
            					if (num == ball){

                                    printf("player 3B recieved the ball ..........\n"); 
                                    players[1][2].has_Ball=1;
                               			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                               			     int Bpw3;
                				     if ((Bpw3=open("B23G",O_RDWR|O_NONBLOCK))== -1){
                					perror("open Error\n");
						exit(-1);
                				     } else {
                						if (write(Bpw3,&ball,sizeof(int)) ==  -1){
                						perror("w Error\n");
							exit(-1);
                						}
                				}

					printf("player 1B got ball from leader............\n"); 
					//players[1][0].has_Ball=1;
            					}
            						
            					else {
            						printf("player 1B didn't get ball from leader..\n");
						exit(-1);
            					}
            				}
            				close(pr23);
       			}
       		if (players[1][2].has_Ball == 1){
                            		int pauseTimeP3= rand()%(players[0][2].Energy_level+1)+1;
                          	printf("player 3B sleep %d seconds ...........\n",pauseTimeP3);
                            	sleep(pauseTimeP3);
                            	}
			// player 3 throw the ball to player 4
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			int p34;
                			if ((p34=open("B34",O_RDWR | O_NONBLOCK)) == -1){
                			perror("open Error\n");
				exit(-1);
                			} else if(players[1][2].has_Ball == 1){
                				if (write(p34,&ball,sizeof(int)) ==  -1){
                				perror("write Error\n");
					exit(-1);
                				} else
                				printf(" player 3B throw the ball to player 4B\n"); 
                				players[1][2].has_Ball=0;
                			}

      	 }
       	else if (players[0][3].pid <0){
       		printf("player 3B forking failed \n ");
       		return (0);
       	}
       	else {
       		//wait(NULL);
       		// team A leader and player 4 will be forked
       		players[0][3].pid =fork();
       		if (players[0][3].pid ==0){
       			// this is player 4 in team A
        			/// player 4 recieve the ball from player 3

         			int pr34= open("B34",O_RDWR);
            			if (pr34 == -1){
            				perror("open Error\n");
				exit(-1);
            			} else {
            				int num;
            				if ((read(pr34,&num,sizeof(int))) == -1){
            					perror("read Error\n");
					exit(-1);
            				} else  {
            					if (num == ball){

                                    printf("player 4B recieved the ball ..........\n"); 
                                    players[1][3].has_Ball=1;
                               				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                               			     int Bpw4;
                				     if ((Bpw4=open("B34G",O_RDWR|O_NONBLOCK))== -1){
                					perror("open Error\n");
						exit(-1);
                				     } else {
                						if (write(Bpw4,&ball,sizeof(int)) ==  -1){
                						perror("w Error\n");
							exit(-1);
                						}
                				}
					printf("player 4B got ball from player3B...........\n");
            					}
            				
            					else {
            						printf("player4B didnt get ball from player3B\n");
						exit(-1);
            					}
            				}
            				close(pr34);
        			}
        		if (players[1][3].has_Ball == 1){
                            		int pauseTimeP4= rand()%(players[0][3].Energy_level+1)+2;
                            	printf("player 4B sleep %d seconds ...........\n",pauseTimeP4);
                            	sleep(pauseTimeP4);
                            	}
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 			int p45;
                			if ((p45=open("B45",O_RDWR | O_NONBLOCK)) == -1){
                			perror("open 4 5 B Error..........................\n");
				exit(-1);
                			} else if (players[1][3].has_Ball == 1){
                				if (write(p45,&ball,sizeof(int)) ==  -1){
                				perror("write 4 5 Error..............\n");
					exit(-1);
                				} else
                				printf(" player 4B throw the ball to player 5B.................\n"); 
                				players[1][3].has_Ball=0;
                			}

       	}
       	else if (players[0][3].pid <0){
       		printf("player 4 in team B forking failed \n");
       		return(0);
       	}
       	else {
       		//wait(NULL);
       		// team B leader , and 5th player will be forked
      		 players[0][4].pid =fork();
       		if (players[0][4].pid == 0){
       			// this is player 5 in Team B
      			//////player 5th read from player 4th
      			 /// player 5 recieve the ball from player 4

         			int pr45= open("B45",O_RDWR);
            			if (pr45 == -1){
            				perror("open Error\n");
				exit(-1);
            			} else {
            				int num;
            				int bytes = read(pr45,&num,sizeof(int));
            				if (bytes == -1){
            					perror("read Error\n");
					exit(-1);
            				} else  {
            					if (num == ball){

                                    printf("player 5B recieved the ball ..........\n"); 
                                    players[1][4].has_Ball=1;
                               					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                               			     int Bpw5;
                				     if ((Bpw5=open("B45G",O_RDWR|O_NONBLOCK))== -1){
                					perror("open Error\n");
						exit(-1);
                				     } else {
                						if (write(Bpw5,&ball,sizeof(int)) ==  -1){
                						perror("w Error\n");
							exit(-1);
                						}
                				}
					printf("player 5B got ball from player 4B......\n");
            					}
            						
            					else {
            						printf("player5B didnt get ball from player4B\n");
						exit(-1);
            					}
            				}
            				close(pr45);
      			}
      		if (players[1][4].has_Ball == 1){
                            		int pauseTimeP5= rand()%(players[0][4].Energy_level+1)+1;
                            	printf("player 5B sleep %d seconds ...........\n",pauseTimeP5);
                            	sleep(pauseTimeP5);
                            	}
			// player 5 throw the ball to team leader
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			int p5ToLeader;
                			if ((p5ToLeader=open("B-leader-5",O_RDWR | O_NONBLOCK)) == -1){
                			perror("open 5-leader B Error..........................\n");
				exit(-1);
                			} else if (players[1][4].has_Ball == 1){
                				if (write(p5ToLeader,&ball,sizeof(int)) ==  -1){
                				perror("write 4 5 Error..............\n");
					exit(-1);
                				} else
                				printf("player 5B throw  ball to team leader...............\n");
                				players[1][4].has_Ball=0;
                			}

       	}
       	else if (players[0][4].pid <0){
       		printf("team B player 4 forking failed \n");
       		return (0);
       	}
       	else{
       		//wait(NULL);
       		// this is team leader finnish forking 5 players
       		printf("I'm team B leader, and I finnish forking ............................\n");
		// team B leader recieved the ball back from player 5
		pid_t child_1 = fork();
		if (child_1 < 0){
			printf("forking faild child 1\n");
			exit(-1);
		} else if (child_1 == 0){
			// child 1
			int p5ToLead= open("B-leader-5",O_RDWR);
            		if (p5ToLead == -1){
            			perror("open Error\n");
			exit(-1);
            		} else {
            			int num;
            			int bytes = read(p5ToLead,&num,sizeof(int));
            			if (bytes == -1){
            				perror("read Error\n");
				exit(-1);
            			} else  {
            				if (num == ball){


                                printf("teamB leader recieved the ball back .\n"); 
                                leaderB_Has_Ball=1;
                               				///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                               			     int Lead5;
                				     if ((Lead5=open("B-leader-5G",O_RDWR|O_NONBLOCK))== -1){
                					perror("open Error\n");
						exit(-1);
                				     } else {
                						if (write(Lead5,&ball,sizeof(int)) ==  -1){
                						perror("w Error\n");
							exit(-1);
                						}
                				}
				printf("team B leader got ball from player 5B...........\n");
            				}
            				else {
            					printf("team B didn't get ball from player 5B....\n");
					exit(-1);
            				}
            			}
            			close(p5ToLead);
                		}
                		if (leaderB_Has_Ball == 1){
                		sleep(pauseTimeLeaderB);
                		printf("B sleeps %d s before throw A leader ..\n",pauseTimeLeaderB);
                		}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// team B leader throw the ball to team A leader
		int TAtoTB;
                		if ((TAtoTB=open("leader-B-To-leader-A",O_RDWR | O_NONBLOCK)) == -1){
                		perror("open 5-leader B Error..........................\n");
			exit(-1);
                		} else if (leaderB_Has_Ball == 1){
                			if (write(TAtoTB,&ball,sizeof(int)) ==  -1){
                			perror("write 4 5 Error..............\n");
				exit(-1);
                			} else
                			printf(" B leader throw the ball to team A leader...............\n"); 
                			leaderB_Has_Ball=0;
                			Team_B_Balls_Count--;
                		}
		} else {
			// leader B
			pid_t child_2 = fork();
			if (child_2 < 0){
				printf("forking faild child 2\n");
				exit(-1);
			} else if (child_2 == 0){
				///child 1
				int AB = open("leader-A-To-leader-B",O_RDWR );
        		if (AB == -1){
        			perror("open Error\n");
			exit(-1);
        		} else {
        			int num;
        			if ((read(AB,&num,sizeof(int))) == -1){
        				perror("read Error\n");
				exit(-1);
        			} else {
        				if (num == ball){
        				printf("leader B recived from leader A\n"); 
        				leaderB_Has_Ball=1;
        				Team_B_Balls_Count++;
                			printf("Ball Count = %d \n",Team_B_Balls_Count);
                			numberOfRounds= numberOfRounds - 1;
        				int ABG = open("leader-A-To-leader-BG",O_RDWR | O_NONBLOCK);
        				if (ABG == -1){
        					perror("open Error\n");
					exit(-1);
        				} else 
        					if (write(ABG,&ball,sizeof(int)) == -1){
        						perror("write Error\n");
						exit(-1);
        					}
        				}
        			}
        			close(AB);
        		}		
        		int Bw1 = open("B-leader-1",O_RDWR | O_NONBLOCK);
        		if (Bw1 == -1){
        			perror("open Error B1\n");
			exit(-1);
        		} else if (leaderB_Has_Ball == 1){
        			if (write(Bw1,&ball,sizeof(int)) == -1){
        				perror("write Error B1\n");
				exit(-1);
        			} else {
        				printf("Leader B throw the ball to player 1\n");
        				leaderB_Has_Ball = 0;
        			}
        		}
			} else {
				// leader B
				printf("Leader B finish forking child 1 and 2\n");
			}

       		}
       	}
       }
   }
}
}
                    else if(players[0][0].pid<0)
                    {
                        perror("Team B player 1 generation failed ! \n ");
                      	  return(1);
                    }

                    } // the end of team B

                }  // the end of the while



        ////  ********************************************

        if(getpid() != teams[0] && getpid() != teams[1])
        {
        	//the parent
           	while(1){
           		if (!leaderA_Has_Ball && !players[0][0].has_Ball && !players[0][1].has_Ball
           		&& !players[0][2].has_Ball && !players[0][3].has_Ball && !players[0][4].has_Ball){
           			int to_A = open("To_team_leader_A",O_RDWR | O_NONBLOCK);
           			if (to_A == -1){
           				perror("open Error parent-A\n");
				exit(-1);
           			} else {
           				if (write(to_A,&ball,sizeof(int)) == -1){
           					perror("write Error parent-A\n");
					exit(-1);
           				} else {
           					printf("Parent throw a ball to leader A\n");
           					Team_A_Balls_Count++;
           				}
           			}
           		}
           		if (!leaderB_Has_Ball && !players[1][0].has_Ball && !players[1][1].has_Ball
           		&& !players[1][2].has_Ball && !players[1][3].has_Ball && !players[1][4].has_Ball){
           			int to_B = open("To_team_leader_B",O_RDWR | O_NONBLOCK);
           			if (to_B == -1){
           				perror("open Error parent-B\n");
				exit(-1);
           			} else {
           				if (write(to_B,&ball,sizeof(int)) == -1){
           					perror("write Error parent-B\n");
					exit(-1);
           				} else {
           					printf("Parent throw a ball to leader B\n");
           					Team_B_Balls_Count++;
           				}
           			}
           		}
           		if (numberOfRounds == 0)
           			break;
           	}
        }
    return (0);
}


void GameRounds(int argc, char **argv)
{
    FILE *fp2;// pointer to the file
    fp2= fopen(argv[1],"r");
    if (fp2 == NULL )
    {
        printf("An Error Occured while openning the file ! \n");
        exit(-2);
    }
// Now read the file content
    char fileContent[150];
    while(fgets(fileContent,150,fp2))
    {
        char *ptr = strtok(fileContent, " ");
        if(strcmp(ptr,"NUMBER_OF_ROUNDS")==0)
        {
// the string NUMBER_OF_ROUNDS has been founded in the file
            ptr=strtok(NULL," ");
            numberOfRounds=atoi(ptr);
            printf("Game Rounds = %d\n",numberOfRounds);
        }

    }
    fclose(fp2);
}
void GameTimeRead(int argc, char **argv)
{

    FILE *fp2; // pointer to the file
    fp2=fopen(argv[2],"r");
    if(fp2 == NULL)
    {
        printf("File 2 openning Error !\n");
        exit(-3);
    }
// Now read the file content
    char fileContent[150];
    while(fgets(fileContent,150,fp2))
    {
        char *ptr = strtok(fileContent, " ");
        if(strcmp(ptr,"GameTime")==0)
        {
// the string GameTime has been founded in the file
            ptr=strtok(NULL," ");
            GameTime=atoi(ptr);
            printf("Game Time = %d\n",GameTime);
        }
    }
    fclose(fp2);
}
void TeamLeader(char teamSymbol)
{
    printf("I'm Team %c leader , Im ready...........\n  ",teamSymbol);
}
void playerPrinting(char teamSymbol, int playerNumber)
{
    printf("Im a player %d in team %c \n",playerNumber,teamSymbol);
}

void handle_alarm(int sig)
{

    printf("Timeout occurred . Exiting .............\n");
    // Count the balls and showing the results
   if(Team_A_Balls_Count < Team_B_Balls_Count){
           printf("Team A Wins ...........................................!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! WOOOOOOOOW \n");
   }
   else if (Team_A_Balls_Count > Team_B_Balls_Count){
           printf("Team B Wins ...........................................!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! WOOOOOOOOW \n");
   }
   else {
          printf("Game scores stay unchanged ..............................\n");
   }

    // kill team leaders
   for (int i=0;i<2;i++){
    kill(teams[i],SIGKILL);
    // kill players
    for(int j=0;j<5;j++){
    kill(players[i][j].pid,SIGKILL);
    }
   }

   // kill main process
    exit(EXIT_SUCCESS);
}

void assignEnergyToPlayers(player players)
{

            //    Seed the random number generator
            srand(time(NULL));
            // Generate a random energy level between 1 and MAX_ENERGY
            int energyLevel = (rand() % 20) + 1;
            players.Energy_level=energyLevel;
            sleep(1);
            printf("energy in function = %d\n",players.Energy_level);

}

void EnergyBounds(int argc, char **argv)
{

    FILE *fp3; // pointer to the file
    fp3=fopen(argv[3],"r");
    if(fp3 == NULL)
    {
        printf("File 3 openning Error !\n");
        exit(-4);
    }
// Now read the file content
    char fileContent[150];
    while(fgets(fileContent,150,fp3))
    {
        char *ptr = strtok(fileContent, " ");
        if(strcmp(ptr,"Energy_Level")==0)
        {
// the string GameTime has been founded in the file
            ptr=strtok(NULL," ");
            Min_Energy_Bound=atoi(ptr);
            printf("Energy Level as Min = %d\n",Min_Energy_Bound);
        }
    }
    fclose(fp3);
}


