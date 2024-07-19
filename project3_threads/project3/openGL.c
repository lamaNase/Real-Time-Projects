#include "header.h"

liquid_medicine liquid_medicines[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE];
pill_based_medicine pill_medicines[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE];
employee employees_gl[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE];

float init_medicine_x = -10.0;
float stop = 300.0;
float init_packedge_pos = 450.0;
shm_cell *shm_ptr1; 

float stop_pos[MAX_ARRAY_SIZE];
int produced_medicines[MAX_ARRAY_SIZE];
float packedges[MAX_ARRAY_SIZE];
bool closed[MAX_ARRAY_SIZE];
float closed_packages_pos[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE];
int closed_packages[MAX_ARRAY_SIZE];
int capacities[MAX_ARRAY_SIZE];

void display() {
  glClear(GL_COLOR_BUFFER_BIT);
  
  draw_garbage();
  draw_package();
  draw_employees();
  draw_production_lines();
  draw_liquid_medicine();
  deaw_pill_medicine();
  glutSwapBuffers();
}

void timer() {
  shm_cell *shm_ptr1 = createShm(id);  
  for (int i = 0; i < num_of_lines; i++){
    char fifo_name[20] = "Lines-GL";
    char fifo_num[20];
    sprintf(fifo_num,"%d",i);
    strcat(fifo_name,fifo_num);
    int f;
    if (i % 2 != 0){ // liquid line
      liquid_medicine liq_medicine;
      if ((f = open(fifo_name, O_RDWR | O_NONBLOCK)) == -1){
	perror("can't open the fifo\n");
	exit(-1);
      } else if (read(f,&liq_medicine,sizeof(liquid_medicine)) != -1){
	liquid_medicines[i][produced_medicines[i]].produced = 1;
	produced_medicines[i]++;
	//if (liq_medicine.package.folded_prescriptions_attached == true)
	//	closed[i] = true;
      }
      close (f);
      for (int j = 0; j < shm_ptr1[i].numOfEmployees; j++){
	char fifo_name2[20] = "Lines-GL";
	strcat(fifo_name2,fifo_num);
	strcat(fifo_name2,"-");
	char fifo_num2[20];
	sprintf(fifo_num2,"%d",j);
	strcat(fifo_name2,fifo_num2);
	if ((f = open(fifo_name2, O_RDWR | O_NONBLOCK)) == -1){
	  //perror("can't open the fifo employee **************************\n\n");
	  //exit(-1);
	} else if (read(f,&liq_medicine,sizeof(liquid_medicine)) != -1){
	  // employee j in line 0 taked the medicine
	  employees_gl[i][j].hold_medicine = true;
	  liquid_medicines[i][liq_medicine.num].read = true;
	  if (liq_medicine.out_of_spec == true)
	    employees_gl[i][j].do_packedging = false;
	  else
	    employees_gl[i][j].do_packedging = true;
	}
	close (f);
      }
    } else { // pill based medicine
      pill_based_medicine pill_medicine;
      if ((f = open(fifo_name, O_RDWR | O_NONBLOCK)) == -1){
	perror("can't open the fifo\n");
	exit(-1);
      } else if (read(f,&pill_medicine,sizeof(pill_based_medicine)) != -1){
	pill_medicines[i][produced_medicines[i]].produced = 1;
	//printf("read done on the fifo\n");
	produced_medicines[i]++;
      }
      close (f);
      for (int j = 0; j < shm_ptr1[i].numOfEmployees; j++){
	char fifo_name2[20] = "Lines-GL";
	strcat(fifo_name2,fifo_num);
	strcat(fifo_name2,"-");
	char fifo_num2[20];
	sprintf(fifo_num2,"%d",j);
	strcat(fifo_name2,fifo_num2);
	if ((f = open(fifo_name2, O_RDWR | O_NONBLOCK)) == -1){
	  //perror("can't open the fifo employee **************************\n\n");
	  //exit(-1);
	} else if (read(f,&pill_medicine,sizeof(pill_based_medicine)) != -1){
	  // employee j in line 0 taked the medicine
	  employees_gl[i][j].hold_medicine = true;
	  pill_medicines[i][pill_medicine.num].read = true;
	  if (pill_medicine.out_of_spec == true)
	    employees_gl[i][j].do_packedging = false;
	  else
	    employees_gl[i][j].do_packedging = true;
	}
	close (f);
      }
    }
  }
  
  glutPostRedisplay(); // Notify GLUT that the window needs to be redrawn
  glutTimerFunc(5, timer, 0); // Call the timer function after 5 milliseconds
}

int init_openGL(int argc, char** argv) {
  for (int i = 0; i < num_of_lines; i++){
    produced_medicines[i] = 0;
    stop_pos[i] = stop;
    for (int j = 0; j < MAX_ARRAY_SIZE; j++){
      liquid_medicines[i][j].pos = init_medicine_x;
      pill_medicines[i][j].pos = init_medicine_x;
      liquid_medicines[i][j].produced = 0;
      liquid_medicines[i][j].read = false;
      liquid_medicines[i][j].reached = false;
      
      pill_medicines[i][j].produced = 0;
      pill_medicines[i][j].read = false;
      pill_medicines[i][j].reached = false;
      
      employees_gl[i][j].hold_medicine = false;
      employees_gl[i][j].pos = (j * 65.0) + 170.0;
      employees_gl[i][j].go_back1 = false;
      employees_gl[i][j].go_back2 = false;
      closed_packages_pos[i][j] = init_packedge_pos;
      
    }
    closed[i] = false;
    packedges[i] = init_packedge_pos;
    closed_packages[i] = 0;
    capacities[i] = 0;
  }
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(1150, 590);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Pharmaceutical Medicine Inspection & Packeging Simulater");
  glClearColor(0.6,0.6,0.6,0.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  gluOrtho2D(0.0, 600.0, 0.0, 400.0); // Set the coordinate system
  glutDisplayFunc(display);
  glutTimerFunc(0, timer, 0);
  glutMainLoop();
  return 0;
}

// Function to draw a circle (ball) with a specified color
void drawCircle(float x, float y) {
  float radiusX = 5.0;
  float radiusY = 2.5;
  int numSegments = 100;
  glColor3f(0.0,0.0,0.0);
  glBegin(GL_TRIANGLE_FAN);
  for (int i = 0; i <= numSegments; i++) {
    float angle = i * 2.0f * 3.14159 / numSegments;
    float dx = radiusX * cosf(angle);
    float dy = radiusY * sinf(angle);
    glVertex2f(x + dx, y + dy);
  }
  glEnd();
}

// Function to draw a circle (ball) with a specified color
void draw_pill(float x, float y) {
  float radius = 1.5;
  int numSegments = 100;
  glColor3f(0.65,0.16,0.16);
  glBegin(GL_TRIANGLE_FAN);
  for (int i = 0; i <= numSegments; i++) {
    float angle = i * 2.0f * 3.14159 / numSegments;
    float dx = radius * cosf(angle);
    float dy = radius * sinf(angle);
    glVertex2f(x + dx, y + dy);
  }
  glEnd();
}

void print (float x, float y,const char *text) {
  glPointSize(35.0);
  glLineWidth(6.0);
  glRasterPos2f(x,y);
  while (*text) {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *text);
    ++text;
  }
}

void draw_production_lines(){
  for (int i = 0; i < num_of_lines; i++){
    float x = 150.0;
    float y = (i * 130.0) + 30.0;
    
    glColor3f(0.5,0.5,0.5);
    glBegin(GL_QUADS);
    glVertex2f(x - 150.0, y - 20.0);
    glVertex2f(x - 150.0, y + 20.0);
    glVertex2f(x + 150.0, y + 20.0);
    glVertex2f(x + 150.0, y - 20.0);
    glEnd();
    
    glColor3f(0.45,0.45,0.45);
    glBegin(GL_QUADS);
    glVertex2f(x + 150.0, y - 20.0);
    glVertex2f(x + 150.0, y + 20.0);
    glVertex2f(x + 150.0 + 30.0, y + 20.0 + 25.0);
    glVertex2f(x + 150.0 + 30.0, y - 20.0 + 25.0);
    glEnd();
    
    glColor3f(0.2,0.2,0.2);
    glBegin(GL_QUADS);
    glVertex2f(x + 150.0, y + 20.0);
    glVertex2f(x - 150.0, y + 20.0);
    glVertex2f(x - 150.0, y + 20.0 + 25.0);
    glVertex2f(x + 150.0 + 30.0, y + 20.0 + 25.0);
    glEnd();
    
    char line[20];
    sprintf(line,"%d",(i + 1));
    char lineName[20] = "Line #";
    strcat(lineName,line);
    glColor3f(1.0,1.0,1.0);
    print(x,y,lineName);
  }
}

void draw_liquid_medicine(){
  for (int j = 0; j < num_of_lines; j++){
    for (int i = 0; i < MAX_ARRAY_SIZE; i++){
      if(liquid_medicines[j][i].produced == 1 && liquid_medicines[j][i].read == false) {
	float x = liquid_medicines[j][i].pos;
	float y = (j * 130.0) + 70.0;
	
	glColor3f(0.0,0.7,1.0);
	glBegin(GL_QUADS);
	glVertex2f(x + 6.0, y + 12.0);
	glVertex2f(x - 6.0, y + 12.0);
	glVertex2f(x - 6.0, y - 12.0);
	glVertex2f(x + 6.0, y - 12.0 );
	glEnd();
	
	glBegin(GL_QUADS);
	glVertex2f(x + 5.0, y + 12.0);
	glVertex2f(x - 5.0, y + 12.0);
	glVertex2f(x - 5.0, y + 12.0 + 5.0);
	glVertex2f(x + 5.0, y + 12.0 + 5.0 );
	glEnd();
	
	drawCircle(x,y+18.0);
	
	if (liquid_medicines[j][i].pos < stop_pos[j]) 
	  liquid_medicines[j][i].pos += 5.0;
	else if (liquid_medicines[j][i].reached == false){
	  stop_pos[j] -= 15.0;
	  liquid_medicines[j][i].reached = true;
	}
      } else if (liquid_medicines[j][i].pos <= stop && liquid_medicines[j][i].read == true ) {
	stop_pos[j] = stop;
	for (int s = 0; s <= MAX_ARRAY_SIZE; s++)
	  liquid_medicines[j][s].reached = false;
      }
    }
  }
}

void deaw_pill_medicine(){
  for (int j = 0; j < num_of_lines; j++){
    for (int i = 0; i < MAX_ARRAY_SIZE; i++){
      if(pill_medicines[j][i].produced == 1 && pill_medicines[j][i].read == false) {
	float x = pill_medicines[j][i].pos;
	float y = (j * 130.0) + 65.0;
	
	// draw the plastic container
	glColor3f(0.9,0.9,0.9);
	glBegin(GL_QUADS);
	glVertex2f(x + 15.0, y + 10.0);
	glVertex2f(x - 3.0, y + 9.0);
	glVertex2f(x - 16.0, y - 9.0);
	glVertex2f(x + 2.0, y - 10.0 );
	glEnd();
	
	// draw the pills
	for (int d = 0; d < 3; d++){
	  float y_pill = (y + 6.0) - (d * 6.0);	
	  for (int k = 0; k < 3; k++){
	    float x_pill = (6.0 * k) + (x-2.0);
	    draw_pill(x_pill,y_pill);
	  }
	  x = x - 4.5;;
	}
	if (pill_medicines[j][i].pos < stop_pos[j]) 
	  pill_medicines[j][i].pos += 5.0;
	else if (pill_medicines[j][i].reached == false){
	  stop_pos[j] -= 25.0;
	  pill_medicines[j][i].reached = true;
	}
      } else if (pill_medicines[j][i].pos <= stop && pill_medicines[j][i].read == true ) {
	stop_pos[j] = stop;
	for (int s = 0; s <= MAX_ARRAY_SIZE; s++)
	  pill_medicines[j][s].reached = false;
      }
    }
  }
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
    glColor3f(0.0, 0.0, 0.0); // black color for arms
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

void draw_employees(){
  shm_cell *shm_ptr1 = createShm(id);  
  for (int i = 0; i < num_of_lines; i++){
    float y = (i * 132.0) + 78.0;
    for (int j = 0; j < shm_ptr1[i].numOfEmployees; j++){
      float x = employees_gl[i][j].pos;
      drawPerson(employees_gl[i][j].pos, y,1.0, 1.0, 0.0); 
      if (employees_gl[i][j].hold_medicine == true){
	if (i % 2 != 0){
	  // draw a liquid medicine next to this employee
	  if (employees_gl[i][j].do_packedging == false) {
	    x -= 25.0;
	  } else {
	    draw_paper(x - 25.0,y);
	    x += 25.0;
	  }
	  glColor3f(0.0,0.7,1.0);
	  glBegin(GL_QUADS);
	  glVertex2f(x + 6.0, y + 12.0);
	  glVertex2f(x - 6.0, y + 12.0);
	  glVertex2f(x - 6.0, y - 12.0);
	  glVertex2f(x + 6.0, y - 12.0 );
	  glEnd();
	  
	  glBegin(GL_QUADS);
	  glVertex2f(x + 5.0, y + 12.0);
	  glVertex2f(x - 5.0, y + 12.0);
	  glVertex2f(x - 5.0, y + 12.0 + 5.0);
	  glVertex2f(x + 5.0, y + 12.0 + 5.0 );
	  glEnd();
	  
	  drawCircle(x,y+18.0);
	} else {
	  // draw a pill based medicine next to this employee
	  if (employees_gl[i][j].do_packedging == false)
	    x -= 25.0;
	  else {
	    if (capacities[i] == (package_capacity - 1))
	      draw_paper(x - 25.0,y);
	    x += 25.0;
	  }
	  glColor3f(0.9,0.9,0.9);
	  glBegin(GL_QUADS);
	  glVertex2f(x + 15.0, y + 10.0);
	  glVertex2f(x - 3.0, y + 9.0);
	  glVertex2f(x - 16.0, y - 9.0);
	  glVertex2f(x + 2.0, y - 10.0 );
	  glEnd();
	  
	  // draw the pills
	  for (int d = 0; d < 3; d++){
	    float y_pill = (y + 6.0) - (d * 6.0);	
	    for (int k = 0; k < 3; k++){
	      float x_pill = (6.0 * k) + (x-2.0);
	      draw_pill(x_pill,y_pill);
	    }
	    x = x - 4.5;;
	  }
	}
	if(employees_gl[i][j].do_packedging == false && employees_gl[i][j].pos>95.0){
	  employees_gl[i][j].pos -= 4.0;
	} else if (employees_gl[i][j].pos < (init_packedge_pos - 30.0) &&
		   employees_gl[i][j].do_packedging == true)
	  employees_gl[i][j].pos += 4.0;
	else {
	  employees_gl[i][j].hold_medicine = false;
	  // close the package if it is liquid
	  if (i % 2 != 0) {
	    closed[i] = true;
	    closed_packages[i]++;
	  } else {
	    capacities[i]++;
	    if (capacities[i] == package_capacity){
	      closed[i] = true;
	      closed_packages[i]++;
	      capacities[i] = 0;
	    }
	  }
	  // now go back here to initiial position
	  if (employees_gl[i][j].do_packedging == true)
	    employees_gl[i][j].go_back1 = true;
	  else
	    employees_gl[i][j].go_back2 = true;
	}
      } else if (employees_gl[i][j].go_back1 == true)
	employees_gl[i][j].pos -= 4.0;
      else if (employees_gl[i][j].go_back2 == true)
	employees_gl[i][j].pos += 4.0;
      if (employees_gl[i][j].pos <= ((j * 65.0) + 170.0) && employees_gl[i][j].go_back1 == true)
	employees_gl[i][j].go_back1 = false;
      else if(employees_gl[i][j].pos >= ((j * 65.0) + 170.0)&&employees_gl[i][j].go_back2==true)
	employees_gl[i][j].go_back2 = false;
    } // end of employees loop
  } //end of lines loop
}

void draw_package(){
  for (int i = 0; i < num_of_lines; i++){
    float y = (i * 125.0) + 50.0;
    //for(int j = 0; j < MAX_ARRAY_SIZE; j++) {
    float x = packedges[i];
    //if (closed[i] == false){
    glColor3f(1.0,0.88,0.74);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + 20.0, y + 8.0);
    glVertex2f(x + 20.0, y + 38.0);
    glVertex2f(x, y+ 30.0);
    glEnd();
    
    glLineWidth(3.0);
    glColor3f(0.0,0.0,0.0);
    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x + 20.0, y + 8.0);
    glVertex2f(x + 20.0, y + 8.0);
    glVertex2f(x + 20.0, y + 38.0);
    glVertex2f(x + 20.0, y + 38.0);
    glVertex2f(x, y+ 30.0);
    glVertex2f(x, y+ 30.0);
    glVertex2f(x, y);
    glEnd();
    
    glColor3f(1.0,0.88,0.74);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x - 20.0, y + 8.0);
    glVertex2f(x - 20.0, y + 38.0);
    glVertex2f(x, y+ 30.0);
    glEnd();
    
    glLineWidth(3.0);
    glColor3f(0.0,0.0,0.0);
    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x - 20.0, y + 8.0);
    glVertex2f(x - 20.0, y + 8.0);
    glVertex2f(x - 20.0, y + 38.0);
    glVertex2f(x - 20.0, y + 38.0);
    glVertex2f(x, y+ 30.0);
    glVertex2f(x, y+ 30.0);
    glVertex2f(x, y);
    glEnd();
    
    glColor3f(1.0,0.88,0.74);
    glBegin(GL_QUADS);
    glVertex2f(x, y+30.0);
    glVertex2f(x + 20.0, y + 38.0);
    glVertex2f(x + 28.0, y + 30.0);
    glVertex2f(x + 6.0, y+ 20.0);
    glEnd();
    
    glLineWidth(3.0);
    glColor3f(0.0,0.0,0.0);
    glBegin(GL_LINES);
    glVertex2f(x, y+30.0);
    glVertex2f(x + 20.0, y + 38.0);
    glVertex2f(x + 20.0, y + 38.0);
    glVertex2f(x + 28.0, y + 30.0);
    glVertex2f(x + 28.0, y + 30.0);
    glVertex2f(x + 6.0, y+ 20.0);
    glVertex2f(x + 6.0, y+ 20.0);
    glVertex2f(x, y+30.0);
    glEnd();
    
    glColor3f(0.8,0.704,0.592);
    glBegin(GL_QUADS);
    glVertex2f(x, y+30.0);
    glVertex2f(x + 20.0, y + 38.0);
    glVertex2f(x - 2.0, y + 45.0);
    glVertex2f(x - 20.0, y + 38.0);
    glEnd();
    
    glLineWidth(3.0);
    glColor3f(0.0,0.0,0.0);
    glBegin(GL_LINES);
    glVertex2f(x, y+30.0);
    glVertex2f(x + 20.0, y + 38.0);
    glVertex2f(x + 20.0, y + 38.0);
    glVertex2f(x - 2.0, y + 45.0);
    glVertex2f(x - 2.0, y + 45.0);
    glVertex2f(x - 20.0, y + 38.0);
    glVertex2f(x - 20.0, y + 38.0);
    glVertex2f(x, y+30.0);
    glEnd();
    
    glColor3f(1.0,0.88,0.74);
    glBegin(GL_QUADS);
    glVertex2f(x, y+30.0);
    glVertex2f(x - 20.0, y + 38.0);
    glVertex2f(x - 28.0, y + 30.0);
    glVertex2f(x - 6.0, y+ 20.0);
    glEnd();
    
    glLineWidth(3.0);
    glColor3f(0.0,0.0,0.0);
    glBegin(GL_LINES);
    glVertex2f(x, y+30.0);
    glVertex2f(x - 20.0, y + 38.0);
    glVertex2f(x - 20.0, y + 38.0);
    glVertex2f(x - 28.0, y + 30.0);
    glVertex2f(x - 28.0, y + 30.0);
    glVertex2f(x - 6.0, y+ 20.0);
    glVertex2f(x - 6.0, y+ 20.0);
    glVertex2f(x, y+30.0);
    glEnd();
    //} 
    if (closed[i] == true) { // packedgeing done
      for (int j = 0; j < closed_packages[i];j++) {
	x = closed_packages_pos[i][j];
	glColor3f(1.0,0.88,0.74);
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + 20.0, y + 8.0);
	glVertex2f(x + 20.0, y + 38.0);
	glVertex2f(x, y+ 30.0);
	glEnd();
	
	glLineWidth(3.0);
	glColor3f(0.0,0.0,0.0);
	glBegin(GL_LINES);
	glVertex2f(x, y);
	glVertex2f(x + 20.0, y + 8.0);
	glVertex2f(x + 20.0, y + 8.0);
	glVertex2f(x + 20.0, y + 38.0);
	glVertex2f(x + 20.0, y + 38.0);
	glVertex2f(x, y+ 30.0);
	glVertex2f(x, y+ 30.0);
	glVertex2f(x, y);
	glEnd();
	
	glColor3f(1.0,0.88,0.74);
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x - 20.0, y + 8.0);
	glVertex2f(x - 20.0, y + 38.0);
	glVertex2f(x, y+ 30.0);
	glEnd();
	
	glLineWidth(3.0);
	glColor3f(0.0,0.0,0.0);
	glBegin(GL_LINES);
	glVertex2f(x, y);
	glVertex2f(x - 20.0, y + 8.0);
	glVertex2f(x - 20.0, y + 8.0);
	glVertex2f(x - 20.0, y + 38.0);
	glVertex2f(x - 20.0, y + 38.0);
	glVertex2f(x, y+ 30.0);
	glVertex2f(x, y+ 30.0);
	glVertex2f(x, y);
	glEnd();
	
	glColor3f(1.0,0.88,0.74);
	glBegin(GL_QUADS);
	glVertex2f(x, y+30.0);
	glVertex2f(x + 20.0, y + 38.0);
	glVertex2f(x - 2.0, y + 45.0);
	glVertex2f(x - 20.0, y + 38.0);
	glEnd();
	
	glLineWidth(3.0);
	glColor3f(0.0,0.0,0.0);
	glBegin(GL_LINES);
	glVertex2f(x, y+30.0);
	glVertex2f(x + 20.0, y + 38.0);
	glVertex2f(x + 20.0, y + 38.0);
	glVertex2f(x - 2.0, y + 45.0);
	glVertex2f(x - 2.0, y + 45.0);
	glVertex2f(x - 20.0, y + 38.0);
	glVertex2f(x - 20.0, y + 38.0);
	glVertex2f(x, y+30.0);
	glEnd();
	closed_packages_pos[i][j] += 5.0;
      }
    }
  }
}

void draw_garbage() {
  for (int i = 0; i < num_of_lines; i++){
    float y = (i * 130.0) + 80.0;
    float x = 70.0;
    
    glColor3f(1.0,1.0,1.0);
    glBegin(GL_QUADS);
    glVertex2f(x - 10.0, y);
    glVertex2f(x + 10.0, y);
    glVertex2f(x + 12.0, y + 35.0);
    glVertex2f(x - 12.0, y + 35.0);
    glEnd();
    
    glColor3f(0.0,0.0,0.0);
    glLineWidth(3.0);
    glBegin(GL_LINES);
    glVertex2f(x - 10.0, y);
    glVertex2f(x + 10.0, y);
    glVertex2f(x + 10.0, y);
    glVertex2f(x + 12.0, y + 35.0);
    glVertex2f(x + 12.0, y + 35.0);
    glVertex2f(x - 12.0, y + 35.0);
    glVertex2f(x - 12.0, y + 35.0);
    glVertex2f(x - 10.0, y);
    glEnd();
    
    glLineWidth(8.0);
    glBegin(GL_LINES);
    glVertex2f(x + 15.0, y + 40.0);
    glVertex2f(x - 15.0, y + 40.0);
    glEnd();
    
    glColor3f(1.0,1.0,1.0);
    glBegin(GL_QUADS);
    glVertex2f(x - 5.0, y + 40.0);
    glVertex2f(x + 5.0, y + 40.0);
    glVertex2f(x + 5.0, y + 47.0);
    glVertex2f(x - 5.0, y + 47.0);
    glEnd();
    
    glColor3f(0.0,0.0,0.0);
    glLineWidth(3.0);
    glBegin(GL_LINES);
    glVertex2f(x - 5.0, y + 40.0);
    glVertex2f(x + 5.0, y + 40.0);
    glVertex2f(x + 5.0, y + 40.0);
    glVertex2f(x + 5.0, y + 47.0);
    glVertex2f(x + 5.0, y + 47.0);
    glVertex2f(x - 5.0, y + 47.0);
    glVertex2f(x - 5.0, y + 47.0);
    glVertex2f(x - 5.0, y + 40.0);
    glEnd();
    
    glLineWidth(8.0);
    glBegin(GL_LINES);
    // line one
    glVertex2f(x - 6.0, y + 30.0);
    glVertex2f(x - 6.0, y + 5.0);
    
    // line two
    glVertex2f(x, y + 30.0);
    glVertex2f(x, y + 5.0);
    
    // line three
    glVertex2f(x + 6.0, y + 30.0);
    glVertex2f(x + 6.0, y + 5.0);
    glEnd();
  }
}

void draw_paper(float x, float y) {
  glColor3f(1.0,1.0,1.0);
  glBegin(GL_QUADS);
  glVertex2f(x - 5.0, y - 5.0);
  glVertex2f(x + 5.0, y - 5.0);
  glVertex2f(x + 5.0, y + 15.0);
  glVertex2f(x - 5.0, y + 15.0);
  glEnd();
}
