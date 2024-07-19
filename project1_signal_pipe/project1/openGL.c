#include "header.h"

// Functions prototypes
void initGraphics(int argc,char * argv[]) ;
void drawGameGraphics();
void drawPerson(float centerX, float centerY,float r, float g, float b);
void drawCircle(float x, float y,float r,float g,float b);
void throw_to_leaderA();
void throw_to_leaderB();
void passBall(char team, int source, int destination);
void print (float x, float y,const char *text);

// Global variabels
// Team A infor: x,y and color (r,g and b)
float team_A[6][6] = {
		{100.0, 300.0,0.0, 0.0, 1.0},
		{150.0, 250.0,1.0, 0.0, 0.0},
		{100.0, 200.0,1.0, 0.0, 0.0},
		{150.0, 150.0,1.0, 0.0, 0.0},
		{100.0, 120.0, 1.0, 0.0, 0.0},
		{150.0, 70.0,1.0, 0.0, 0.0}
				};
// Team B info: x,y and color (r,g and b)				
float team_B[6][6] = {
		{500.0, 300.0, 0.0, 0.0, 1.0},
		{450.0, 250.0, 1.0, 0.0, 0.0},
		{500.0, 200.0, 1.0, 0.0, 0.0},
		{450.0, 150.0, 1.0, 0.0, 0.0},
		{500.0, 120.0, 1.0, 0.0, 0.0},
		{450.0, 70.0, 1.0, 0.0, 0.0}
				};
float drop[2] = {300.0,200.0};
int x_B = 410;
int x_A = 50;
int y = 60;

// Function to initialize OpenGL for graphics
void initGraphics(int argc,char *argv[]) {
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 420);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Beach Ball Game Graphics");

	gluOrtho2D(0.0, 600.0, 0.0, 400.0); // Set the coordinate system
	glutDisplayFunc(drawGameGraphics);
	glutMainLoop();
}

// Function to draw the beach ball game graphics
void drawGameGraphics() {
	//if (numberOfRounds > 0){
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);
    
		//draw the sea
		glColor3f(0.0,0.5,1.0);
		glBegin(GL_QUADS);
    			glVertex2f(0.0,0.0);
    			glVertex2f(0.0,50.0);
    			glVertex2f(700.0,50.0);
    			glVertex2f(700.0,0.0);
		glEnd();
	
		//draw the ground
		glColor3f(0.9,0.8,0.6);
		glBegin(GL_QUADS);
    			glVertex2f(700.0,400.0);
    			glVertex2f(700.0,50.0);
    			glVertex2f(0.0,50.0);
    			glVertex2f(0.0,400.0);
		glEnd();

		// Draw team A
		for (int i =0; i < 6; i++){
    			drawPerson(team_A[i][0], team_A[i][1],team_A[i][2],team_A[i][3],team_A[i][4]);
		}

	// Draw team B
 		for (int i =0; i < 6; i++){
    			drawPerson(team_B[i][0], team_B[i][1],team_B[i][2], team_B[i][3],team_B[i][4]);
		}
		
		print(80,380,"Team A");
		print(480,380,"Team B");
		
		while(1){
			int num;
			int f;
			
			// Ball threw to leader A
			if ((f = open("To_team_leader_AG",O_RDWR |O_NONBLOCK )) == -1){
				perror("openError");
				exit(-1);
			} else {
             				int bytes = read(f,&num,sizeof(int));
             				if (bytes != -1)
             					if (num == 1)
             					throw_to_leaderA();
             			close(f);
             			} 
             		glutSwapBuffers();
             			//Ball threw to leader B 
             			if ((f = open("To_team_leader_BG",O_RDWR |O_NONBLOCK )) == -1){
				perror("openError");
				exit(-1);
			} else {
             				int bytes = read(f,&num,sizeof(int));
             				if (bytes != -1)
             					if (num == 1)
             					throw_to_leaderB();
             			close(f);
             			} 
             		glutSwapBuffers();
             			// leader A passes to player 1 
             			if ((f = open("A-leader-1G",O_RDWR |O_NONBLOCK )) == -1){
				perror("openError");
				exit(-1);
			} else {
             				int bytes = read(f,&num,sizeof(int));
             				if (bytes != -1)
             					if (num == 1)
             					passBall('A', 0, 1);
             			close(f);
             			}
             		glutSwapBuffers();
             			//leader B passes to player 1 
             			if ((f = open("B-leader-1G",O_RDWR |O_NONBLOCK )) == -1){
				perror("openError");
				exit(-1);
			} else {
             				int bytes = read(f,&num,sizeof(int));
             				if (bytes != -1)
             					if (num == 1)
             					passBall('B', 0, 1);
             			close(f);
             			}
             		glutSwapBuffers();	
             			/// team A player 1 passes to player 2 
             			if ((f = open("A12G",O_RDWR |O_NONBLOCK )) == -1){
				perror("openError");
				exit(-1);
			} else {
             				int bytes = read(f,&num,sizeof(int));
             				if (bytes != -1)
             					if (num == 1)
             					passBall('A', 1, 2);
             			close(f);
             			} 
             		glutSwapBuffers();	
             			// team A player 2 passes to player  3
             			if ((f = open("A23G",O_RDWR |O_NONBLOCK )) == -1){
				perror("openError");
				exit(-1);
			} else {
             				int bytes = read(f,&num,sizeof(int));
             				if (bytes != -1)
             					if (num == 1)
             					passBall('A', 2, 3);
             			close(f);
             			} 
             		glutSwapBuffers();
             			// team A player 3 passes to player  4
             			if ((f = open("A34G",O_RDWR |O_NONBLOCK )) == -1){
				perror("openError");
				exit(-1);
			} else {
             				int bytes = read(f,&num,sizeof(int));
             				if (bytes != -1)
             					if (num == 1)
             					passBall('A', 3, 4);
             			close(f);
             			}
             		glutSwapBuffers();	
             			///team A player 4 passes to player  5
             			if ((f = open("A45G",O_RDWR |O_NONBLOCK )) == -1){
				perror("open Error");
				exit(-1);
			} else {
             				int bytes = read(f,&num,sizeof(int));
             				if (bytes != -1)
             					if (num == 1)
             					passBall('A', 4, 5);
             			close(f);
             			}
             		glutSwapBuffers();	
             			//team A player 5 passes to the leader
             			if ((f = open("A-leader-5G",O_RDWR |O_NONBLOCK )) == -1){
				perror("openError");
				exit(-1);
			} else {
             				int bytes = read(f,&num,sizeof(int));
             				if (bytes != -1)
             					if (num == 1)
             					passBall('A', 5, 0);
             			close(f);
             			}
             		glutSwapBuffers();	
             			//team B player 5 passes to the leader
             			if ((f = open("B-leader-5G",O_RDWR |O_NONBLOCK )) == -1){
				perror("openError");
				exit(-1);
			} else {
             				int bytes = read(f,&num,sizeof(int));
             				if (bytes != -1)
             					if (num == 1)
             					passBall('B', 5, 0);
             			close(f);
             			}
             		glutSwapBuffers();	
             			//team B player 1 passes to player 2
             			if ((f = open("B12G",O_RDWR |O_NONBLOCK )) == -1){
				perror("openError");
				exit(-1);
			} else {
             				int bytes = read(f,&num,sizeof(int));
             				if (bytes != -1)
             					if (num == 1)
             					passBall('B', 1, 2);
             			close(f);
             			}
             		glutSwapBuffers();	
             			//team B player 2 passes to player 3
             			if ((f = open("B23G",O_RDWR |O_NONBLOCK )) == -1){
				perror("openError");
				exit(-1);
			} else {
             				int bytes = read(f,&num,sizeof(int));
             				if (bytes != -1)
             					if (num == 1)
             					passBall('B', 2, 3);
             			close(f);
             			}
             		glutSwapBuffers();	
             			 //team B player 3 passes to player 4
             			if ((f = open("B34G",O_RDWR |O_NONBLOCK )) == -1){
				perror("openError");
				exit(-1);
			} else {
             				int bytes = read(f,&num,sizeof(int));
             				if (bytes != -1)
             					if (num == 1)
             					passBall('B', 3, 4);
             			close(f);
             			}
             		glutSwapBuffers();	
             			 //team B player 4 passes to player 5
             			if ((f = open("B45G",O_RDWR |O_NONBLOCK )) == -1){
				perror("openError");
				exit(-1);
			} else {
             				int bytes = read(f,&num,sizeof(int));
             				if (bytes != -1)
             					if (num == 1)
             					passBall('B', 4, 5);
             			close(f);
             			}
             		glutSwapBuffers();	
             			 //team B leader passes to leader A
             			if ((f = open("leader-B-To-leader-AG",O_RDWR |O_NONBLOCK )) == -1){
				perror("openError");
				exit(-1);
			} else {
             				int bytes = read(f,&num,sizeof(int));
             				if (bytes != -1)
             					if (num == 1)
             					passBall('B', 0, 0);
             			close(f);
             			}
             		glutSwapBuffers();	
             			 //team A leader passes to leader B
             			/*if ((f = open("leader-A-To-leader-BG",O_RDWR |O_NONBLOCK )) == -1){
				perror("openError");
				exit(-1);
			} else {
             				int bytes = read(f,&num,sizeof(int));
             				if (bytes != -1)
             					if (num == 1)
             					passBall('A', 0, 0);
             			close(f);
             			}*/
			glutSwapBuffers();
		}
		glutSwapBuffers();
}

void drawPerson(float centerX, float centerY,float r, float g, float b) {
    // Draw head
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0); 
    glTranslatef(centerX, centerY + 30.0, 0.0); // Move to head position
    glLineWidth(5.0);
    glutWireSphere(10.0, 50, 50); // Draw a sphere for the head
    glPopMatrix();

    // Draw body
    glColor3f(r,g,b); // Blue color for body
    glBegin(GL_QUADS);
    glVertex2f(centerX - 8.0, centerY + 12.0); // Top-left corner of body
    glVertex2f(centerX - 8.0, centerY); // Bottom-left corner of body
    glVertex2f(centerX + 8.0, centerY); // Bottom-right corner of body
    glVertex2f(centerX + 8.0, centerY + 12.0); // Top-right corner of body
    glEnd();

    // Draw arms
    glColor3f(1.0, 0.0, 0.0); // Red color for arms
    glBegin(GL_LINES);
    glVertex2f(centerX - 10.0, centerY + 10.0); // Left arm
    glVertex2f(centerX - 20.0, centerY + 5.0);
    glVertex2f(centerX + 10.0, centerY + 10.0); // Right arm
    glVertex2f(centerX + 20.0, centerY + 5.0);
    glEnd();

    // Draw legs
    glColor3f(0.0, 0.0, 0.0); // Green color for legs
    glBegin(GL_LINES);
    glVertex2f(centerX - 5.0, centerY); // Left leg
    glVertex2f(centerX - 5.0, centerY - 12.0);
    glVertex2f(centerX + 5.0, centerY); // Right leg
    glVertex2f(centerX + 5.0, centerY - 12.0);
    glEnd();
}

// Function to draw a circle (ball) with a specified color
void drawCircle(float x, float y,float r, float g, float b) {
	float radius = 10.0;
	int numSegments = 100;
	glColor3f(r,g,b);
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i <= numSegments; i++) {
		float angle = i * 2.0f * 3.14159 / numSegments;
		float dx = radius * cosf(angle);
		float dy = radius * sinf(angle);
		glVertex2f(x + dx, y + dy);
	}
	glEnd();
}
			
void throw_to_leaderA(){
	// draw blue ball
	drawCircle(team_A[0][0] - 30, team_A[0][1],0.0,0.0,1.0);
}

void throw_to_leaderB(){
	// draw blue ball
	drawCircle(team_B[0][0] - 30, team_B[0][1],0.0,0.0,1.0);
}

void passBall(char team, int source, int destination){
	if (team == 'A'){
		if (source >= 0 && source <= 5)
			drawCircle(team_A[source][0] - 30, team_A[source][1],0.9,0.8,0.6);
		else
			// The ball is dropped out and we want to recollect it
			drawCircle(drop[0] , drop[1],0.9,0.8,0.6);
		if (destination >= 0 && destination <= 5){
			if (source == 0 && destination == 0)
				drawCircle(team_B[destination][0] - 30, team_B[destination][1],0.0,0.0,1.0);
			else
				drawCircle(team_A[destination][0] - 30, team_A[destination][1],0.0,0.0,1.0);
		}
		else
			// The ball is dropped out
			drawCircle(drop[0] , drop[1],0.0,0.0,1.0);
	}
	
	if (team == 'B'){
		if (source >= 0 && source <= 5)
			drawCircle(team_B[source][0] - 30, team_B[source][1],0.9,0.8,0.6);
		else
			// The ball is dropped out and we want to recollect it
			drawCircle(drop[0] , drop[1],0.9,0.8,0.6);
		if (destination >= 0 && destination <= 5){
			if (source == 0 && destination == 0)
				drawCircle(team_A[destination][0] - 30, team_A[destination][1],0.0,0.0,1.0);
			else
				drawCircle(team_B[destination][0] - 30, team_B[destination][1],0.0,0.0,1.0);
		}
		else
			// The ball is dropped out
			drawCircle(drop[0] , drop[1],0.0,0.0,1.0);
	}
}

void print (float x, float y,const char *text){
	glColor3f(0.0,0.0,0.0);
	glPointSize(35.0);
	glLineWidth(6.0);
	glRasterPos2f(x,y);
	while (*text){
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *text);
		++text;
	}
}
