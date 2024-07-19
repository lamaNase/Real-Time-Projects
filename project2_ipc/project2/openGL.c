#include "header.h"



void draw_planes();

void drawCircle(float x, float y) ;

void print (float x, float y,const char *text);

void draw_ground();

void draw_Houses();

void drawPerson(float centerX, float centerY,float r, float g, float b) ;

void write_containers(char num[],int i);

void draw_container(int i);

void draw_collect_workers();

void draw_storege_area();

void draw_split_workers();

void draw_dstrb_workers();

void draw_bags();

void timer();



// height and width of planes

float height = 50.0;

float width = 80.0;



// the height of each container

float container_y[Max_arrays_size] ;

// this flags for planes (droped a container or did not)

int flags[Max_arrays_size];

char string[Max_arrays_size][20];

char collecters_levels[Max_arrays_size][20];

char dstrb_levels[Max_arrays_size][20];



float container_ground_y = 250.0;

container_gl collect_workers_hasContainer[Max_arrays_size];

float collecter_x[Max_arrays_size];

float collect_x_init = 30.0;

int split_workers_gl[Max_arrays_size];

bags_gl dstrb_workers_gl[Max_arrays_size];

float dstrb_x[Max_arrays_size];

float dstrb_x_init = 400.0;



container_gl shm1[MAX_NUM_OF_CONTAINERS];

container_gl shm2[MAX_NUM_OF_CONTAINERS];

int shm3[MAX_NUM_OF_CONTAINERS];

int families_gl[Max_arrays_size];

int dstrb_destination[Max_arrays_size];

// storge area parameters

float storge_x = 350.0;



//int plane = 0, collect = 0;

void display()
{

    glClear(GL_COLOR_BUFFER_BIT);

    for (int plane = 0; plane < cargo_planes; plane++)
    {

        if (flags[plane] == 1)
        {

            draw_container(plane);

            container_y[plane] -= 3.0; // Move the square downward

        }

    }

    draw_planes();

    draw_ground();

    draw_Houses();

    draw_storege_area();

    draw_collect_workers();

    draw_split_workers();

    draw_bags();

    draw_dstrb_workers();

    glutSwapBuffers();

}



void timer()
{

    int f;

    char message[20];

    for (int plane = 0; plane < cargo_planes; plane++)
    {

        char fifo_name[20] = "plane-GL";

        char fifo_num[20];

        sprintf(fifo_num,"%d",plane);

        strcat(fifo_name,fifo_num);

        if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
        {

            perror("open error\n");

            exit(-1);

        }
        else
        {

            if (read(f,&message,sizeof(message)) != -1)
            {

                //printf("message: %s\n",message);

                flags[plane] = 1;

                strcpy(string[plane],message);

            }

            close(f);

        }

        strcat(fifo_name,"B");

        container_gl container;

        if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
        {

            perror("open error\n");

            exit(-1);

        }
        else
        {

            if (read(f,&container,sizeof(container_gl)) != -1)
            {

                //printf("container read: weight = %d------------------------------------\n",container.weight);

                shm1[container.index].weight = container.weight;

            }

            close(f);

        }
        if (container_y[plane] <= container_ground_y)
        {

            container_y[plane] = Y_init_plane;

            flags[plane] = 0;

        }

    }



    for (int collect = 0; collect < collect_workers; collect++)
    {

        container_gl container;

        char fifo_name[20] = "collect-GL";

        char fifo_num[20];

        sprintf(fifo_num,"%d",collect);

        strcat(fifo_name,fifo_num);

        if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
        {

            perror("open error\n");

            exit(-1);

        }
        else
        {

            if (read(f,&container,sizeof(container_gl)) != -1)
            {

                collect_workers_hasContainer[collect].weight = container.weight;

                shm1[container.index].weight = 0;

            }

            close(f);

        }

        strcat(fifo_name,"B");

        if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
        {

            perror("open error\n");

            exit(-1);

        }
        else
        {

            if (read(f,&container,sizeof(container_gl)) != -1)
            {

                shm2[container.index].weight = container.weight;

                collect_workers_hasContainer[collect].shm2_write = true;

                collect_workers_hasContainer[collect].weight = 0;

            }

            close(f);

        }



        strcat(fifo_name,"C");

        char message2[20];

        if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
        {

            perror("open error\n");

            exit(-1);

        }
        else
        {

            if (read(f,&message2,sizeof(message2)) != -1)
            {

                if (strcmp(message2,"martyered") == 0)

                    collect_workers_hasContainer[collect].replaced = true;

            }

            close(f);

        }



        strcat(fifo_name,"D");

        char message3[20];

        if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
        {

            perror("open error\n");

            exit(-1);

        }
        else
        {

            if (read(f,&message3,sizeof(message3)) != -1)
            {

                strcpy(collecters_levels[collect],message3);

            }

            close(f);

        }

    }



    for (int i = 0; i < split_workers; i++)
    {

        container_gl container;

        char fifo_name[20] = "split-GL";

        char fifo_num[20];

        sprintf(fifo_num,"%d",i);

        strcat(fifo_name,fifo_num);

        if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
        {

            perror("open error\n");

            exit(-1);

        }
        else
        {

            if (read(f,&container,sizeof(container_gl)) != -1)
            {

                split_workers_gl[i] = container.weight;

                shm2[container.index].weight = 0;

            }

            close(f);

        }

        strcat(fifo_name,"B");

        bags_gl bag;

        if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
        {

            perror("open error\n");

            exit(-1);

        }
        else
        {

            if (read(f,&bag,sizeof(bags_gl)) != -1)
            {

                split_workers_gl[i] = 0;

                int counter = 0;

                int k = bag.index;

                while (counter < bag.num_of_bags)
                {

                    shm3[k] = 1;

                    k++;

                    counter++;

                }

            }

            close(f);

        }



        strcat(fifo_name,"C");

        char message2[20];

        if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
        {

            perror("open error\n");

            exit(-1);

        }
        else
        {

            if (read(f,&message2,sizeof(message2)) != -1)
            {

                if (strcmp(message2,"disappear") == 0)
                {

                    split_workers_gl[i] = -1;

                }

            }

            close(f);

        }

    }



    for (int i = 0; i < dstrb_workers; i++)
    {

        bags_gl bag;

        char fifo_name[20] = "distrib-GL";

        char fifo_num[20];

        sprintf(fifo_num,"%d",i);

        strcat(fifo_name,fifo_num);

        if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
        {

            perror("open error\n");

            exit(-1);

        }
        else
        {

            if (read(f,&bag,sizeof(bags_gl)) != -1)
            {

                dstrb_workers_gl[i].num_of_bags = bag.num_of_bags;

                int counter = 0;

                int k = bag.index;

                while (counter < bag.num_of_bags)
                {

                    shm3[k] = 0;

                    k++;

                    counter++;

                }

            }

            close(f);

        }

        strcat(fifo_name,"B");

        char mesg2[20];

        if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
        {

            perror("open error\n");

            exit(-1);

        }
        else
        {

            if (read(f,&mesg2,sizeof(mesg2)) != -1)
            {

                strcpy(dstrb_levels[i],mesg2);

            }

            close(f);

        }



        strcat(fifo_name,"C");

        int family;

        if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
        {

            perror("open error\n");

            exit(-1);

        }
        else
        {

            if (read(f,&family,sizeof(int)) != -1)
            {

                dstrb_destination[i] = (family * 50) + X_init_house;

            }

            close(f);

        }

        strcat(fifo_name,"D");

        char msgD[20];

        if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
        {

            perror("open error\n");

            exit(-1);

        }
        else
        {

            if (read(f,&msgD,sizeof(msgD)) != -1)
            {

                if (strcmp(msgD,"martyered") == 0)

                    dstrb_workers_gl[i].replaced = true;

            }

            close(f);

        }

    }



    for (int i = 0; i < num_of_families; i++)
    {

        int level;

        char fifo_name[20] = "family-GL";

        char fifo_num[20];

        sprintf(fifo_num,"%d",i);

        strcat(fifo_name,fifo_num);

        if ((f = open(fifo_name,O_RDWR | O_NONBLOCK)) == -1)
        {

            perror("open error\n");

            exit(-1);

        }
        else
        {

            if (read(f,&level,sizeof(level)) != -1)
            {

                families_gl[i] = level;

            }

            close(f);

        }

    }

    glutPostRedisplay(); // Notify GLUT that the window needs to be redrawn

    glutTimerFunc(5, timer, 0); // Call the timer function after 5 milliseconds

}



int init_openGL(int argc, char** argv)
{

    // initalize all flags to zeros

    for (int i = 0; i < cargo_planes; i++)
    {

        flags[i] = 0;

        strcpy(string[i],"filling");

        container_y[i] = Y_init_plane;

    }

    for (int i = 0; i < MAX_NUM_OF_CONTAINERS; i++)
    {

        shm1[i].weight = 0;

        shm2[i].weight = 0;

        shm3[i] = 0;

    }

    for (int i = 0; i < collect_workers; i++)
    {

        collect_workers_hasContainer[i].weight = 0;

        collect_workers_hasContainer[i].replaced = false;

        collect_workers_hasContainer[i].shm2_write = false;

        collecter_x[i] = collect_x_init;

        strcpy(collecters_levels[i],"%");

    }

    for(int i = 0; i< split_workers; i++)
    {

        split_workers_gl[i] = 0;

    }

    for(int i = 0; i< dstrb_workers; i++)
    {

        dstrb_workers_gl[i].num_of_bags = 0; // this value represents the number of bags he holds

        dstrb_workers_gl[i].replaced = false;

        dstrb_x[i] = dstrb_x_init;

        dstrb_destination[i] = ((num_of_families - 1) * 50) + X_init_house;

        strcpy(dstrb_levels[i],"%");

    }

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    glutInitWindowSize(1150, 590);

    glutInitWindowPosition(100, 100);

    glutCreateWindow("Distributing White flour bags to our people in Gaza");

    glClearColor(0.0,0.8,1.0,0.0);



    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();



    gluOrtho2D(0.0, 600.0, 0.0, 400.0); // Set the coordinate system

    glutDisplayFunc(display);

    glutTimerFunc(0, timer, 0);

    glutMainLoop();

    return 0;

}



void draw_planes()
{

    for(int i = 0; i < cargo_planes; i++)
    {

        float x = (i * 180) + X_init_plane;

        float y = Y_init_plane;

        drawCircle(x+(width / 2.0),y+(height / 2.0));

        drawCircle(x-(width / 2.0),y+(height / 2.0));

        glLineWidth(5);

        glColor3f(0.0,0.0,0.0);

        glBegin(GL_QUADS);

        glVertex2f(x - (width / 2.0),y);

        glVertex2f(x + (width / 2.0),y);

        glVertex2f(x + (width / 2.0),y + height);

        glVertex2f(x - (width / 2.0),y + height);

        glEnd();



        //draw the tail

        glColor3f(0.0,0.0,0.0);

        glBegin(GL_QUADS);

        glVertex2f(x - 40.0,y+40.0);

        glVertex2f(x - 70.0,y+65.0);

        glVertex2f(x - 90.0,y+70.0);

        glVertex2f(x - 65.0,y+25.0);

        glEnd();



        //draw the

        glBegin(GL_QUADS);

        glVertex2f(x + (width / 2.0)+ 20.0,y+(height / 2.0)-15.0);

        glVertex2f(x + (width / 2.0)+ 30.0,y+(height / 2.0)-15.0);

        glVertex2f(x + (width / 2.0)+ 30.0,y+(height / 2.0)+15.0);

        glVertex2f(x + (width / 2.0)+ 20.0,y+(height / 2.0)+15.0);

        glEnd();



        //draw the

        glBegin(GL_QUADS);

        glVertex2f(x + (width / 2.0)+ 30.0,y+(height / 2.0)+10.0);

        glVertex2f(x + (width / 2.0)+ 40.0,y+(height / 2.0)+10.0);

        glVertex2f(x + (width / 2.0)+ 40.0,y+(height / 2.0)+40.0);

        glVertex2f(x + (width / 2.0)+ 30.0,y+(height / 2.0)+40.0);

        glEnd();



        //draw the

        glBegin(GL_QUADS);

        glVertex2f(x + (width / 2.0)+ 30.0,y+(height / 2.0)-10.0);

        glVertex2f(x + (width / 2.0)+ 40.0,y+(height / 2.0)-10.0);

        glVertex2f(x + (width / 2.0)+ 40.0,y+(height / 2.0)-40.0);

        glVertex2f(x + (width / 2.0)+ 30.0,y+(height / 2.0)-40.0);

        glEnd();



        //draw the right wing

        glBegin(GL_QUADS);

        glVertex2f(x+5.0,y);

        glVertex2f(x-20.0,y-35.0);

        glVertex2f(x-25.0,y-15.0);

        glVertex2f(x-15.0,y);

        glEnd();



        //draw the left wing

        glBegin(GL_QUADS);

        glVertex2f(x+10.0,y+height-10.0);

        glVertex2f(x-25.0,y+height+20.0);

        glVertex2f(x-40.0,y+height+25.0);

        glVertex2f(x-20.0,y+height);

        glEnd();

        write_containers(string[i],i);

    }

}



// Function to draw a circle (ball) with a specified color

void drawCircle(float x, float y)
{

    float radius = 25.0;

    int numSegments = 100;

    glColor3f(0.0,0.0,0.0);

    glBegin(GL_TRIANGLE_FAN);

    for (int i = 0; i <= numSegments; i++)
    {

        float angle = i * 2.0f * 3.14159 / numSegments;

        float dx = radius * cosf(angle);

        float dy = radius * sinf(angle);

        glVertex2f(x + dx, y + dy);

    }

    glEnd();

}



void print (float x, float y,const char *text)
{

    glPointSize(35.0);

    glLineWidth(6.0);

    glRasterPos2f(x,y);

    while (*text)
    {

        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *text);

        ++text;

    }

}



void draw_ground()
{

    glColor3f(0.1,0.6,0.2);

    glLineWidth(6.0);

    // draw ground

    glBegin(GL_QUADS);

    glVertex2f(0,0);

    glVertex2f(1100,0);

    glVertex2f(1100,260);

    glVertex2f(0,260);

    glEnd();



    for (int j = 0; j < Max_arrays_size; j++)
    {

        if (shm1[j].weight != 0)
        {

            float y = container_ground_y;

            float x = (j*25.0) + 25.0;

            glColor3f(1.0,1.0,1.0);

            glBegin(GL_QUADS);

            glVertex2f(x - (width / 7.0),y);

            glVertex2f(x + (width / 7.0),y);

            glVertex2f(x + (width / 7.0),y + (height/2.0));

            glVertex2f(x - (width / 7.0),y + (height / 2.0));

            glEnd();

            char weight_kg[10];

            sprintf(weight_kg,"%d",shm1[j].weight);

            strcat(weight_kg,"kg");

            glColor3f(0.0,0.0,0.0);

            print(x-10.0,y+ (height/6.0),weight_kg);

        }

    }

}



void draw_Houses()
{

    for (int i = 0; i < num_of_families; i++)
    {

        float x = (i * 50) + X_init_house;

        float y = Y_init_house;

        glColor3f(1.0,1.0,1.0);

        glLineWidth(6.0);

        float width = 30.0, height = 30.0;

        //draw the body of the house

        glBegin(GL_QUADS);

        glVertex2f(x-(width / 2.0),y-(height / 2.0));

        glVertex2f(x + (width / 2.0),y - (height / 2.0));

        glVertex2f(x + (width / 2.0),y + (height / 2.0));

        glVertex2f(x - (width / 2.0),y + (height / 2.0));

        glEnd();



        // draw the ceiling of the home

        //glColor3f(0.8,0.4,0.2);

        glColor3f(0.5,0.25,0.15);

        glBegin(GL_TRIANGLES);

        glVertex2f(x-(width / 2.0),y+(height / 2.0));

        glVertex2f(x + (width / 2.0),y + (height / 2.0));

        glVertex2f(x,y + (height / 2.0)+20.0);

        glEnd();



        // draw a windows

        glColor3f(0.0,0.0,0.0);

        glLineWidth(2.0);

        glBegin(GL_LINES);

        glVertex2f(x-2.0,y);

        glVertex2f(x-2.0,y+7.0);

        glVertex2f(x-2.0,y+7.0);

        glVertex2f(x-10.0,y+7.0);

        glVertex2f(x-10.0,y+7.0);

        glVertex2f(x-10.0,y);

        glVertex2f(x-10.0,y);

        glVertex2f(x-2.0,y);

        glVertex2f(x-2.0,y+3.5);

        glVertex2f(x-10.0,y+3.5);

        glVertex2f(x-5.5,y);

        glVertex2f(x-5.5,y+7.0);

        glEnd();



        glBegin(GL_LINES);

        glVertex2f(x+2.0,y);

        glVertex2f(x+2.0,y+7.0);

        glVertex2f(x+2.0,y+7.0);

        glVertex2f(x+10.0,y+7.0);

        glVertex2f(x+10.0,y+7.0);

        glVertex2f(x+10.0,y);

        glVertex2f(x+10.0,y);

        glVertex2f(x+2.0,y);

        glVertex2f(x+2.0,y+3.5);

        glVertex2f(x+10.0,y+3.5);

        glVertex2f(x+5.5,y);

        glVertex2f(x+5.5,y+7.0);

        glEnd();



        // draw a door

        glColor3f(0.0,0.0,0.0);

        glLineWidth(4.0);

        glBegin(GL_QUADS);

        glVertex2f(x-3.0,y-(height / 2.0));

        glVertex2f(x+3.0,y-(height / 2.0));

        glVertex2f(x+3.0,y-(height / 2.0)+8.0);

        glVertex2f(x-3.0,y-(height / 2.0)+8.0);

        glEnd();



        if (families_gl[i] != -1)
        {

            char level[20];

            sprintf(level,"%d",families_gl[i]);

            strcat(level,"%");

            // print starvation rate

            glColor3f(1.0,1.0,1.0);

            print(x-10.0,y + (height / 2.0)+30.0,level);

        }
        else
        {

            glColor3f(1.0,0.0,0.0);

            glLineWidth(7.0);

            glBegin(GL_LINES);

            glVertex2f(x-(width / 2.0),y-(height / 2.0));

            glVertex2f(x+(width / 2.0),y+(height / 2.0));

            glVertex2f(x-(width / 2.0),y+(height / 2.0));

            glVertex2f(x+(width / 2.0),y-(height / 2.0));

            glEnd();

            glColor3f(1.0,1.0,1.0);

            print(x-10.0,y + (height / 2.0)+30.0,"Dead");

        }

    }

}



void drawPerson(float centerX, float centerY,float r, float g, float b)
{

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



void write_containers(char num[],int i)
{

    float x = (i * 180) + X_init_plane;

    float y = Y_init_plane;

    glColor3f(1.0,1.0,1.0);

    print(x-20.0,y+(height / 2.0),num);

}



void draw_container(int i)
{

    // draw a container inside it

    float x = (i * 180) + X_init_plane;

    glColor3f(1.0,1.0,1.0);

    glBegin(GL_QUADS);

    glVertex2f(x - (width / 7.0),container_y[i]);

    glVertex2f(x + (width / 7.0),container_y[i]);

    glVertex2f(x + (width / 7.0),container_y[i] + (height/2.0));

    glVertex2f(x - (width / 7.0),container_y[i] + (height / 2.0));

    glEnd();

}



void draw_collect_workers()
{

    for (int i = 0; i < collect_workers; i++)
    {

        float x = (i * 70.0) + collecter_x[i];

        float y = 200.0;

        if (collect_workers_hasContainer[i].replaced == true)
        {

            drawPerson(x,y,1.0,1.0,0.0); // yellow for split worker replaced to ba as a co worker

            glColor3f(1.0,1.0,1.0);

            print(x-15.0,y-30.0,collecters_levels[i]);

        }

        else
        {

            drawPerson(x,y,1.0,0.0,0.0); // red worker for collect workers

            glColor3f(1.0,1.0,1.0);

            print(x-15.0,y-30.0,collecters_levels[i]);

        }

        if (collect_workers_hasContainer[i].weight != 0)
        {

            glColor3f(1.0,1.0,1.0);

            glBegin(GL_QUADS);

            glVertex2f(x + 20.0,y+(height/4.0));

            glVertex2f(x + 20.0,y-(height/4.0));

            glVertex2f(x + (width / 6.0) + 25.0,y-(height/4.0));

            glVertex2f(x + (width / 6.0) + 25.0,y+(height/4.0));

            glEnd();

            char weight_kg[10];

            sprintf(weight_kg,"%d",collect_workers_hasContainer[i].weight);

            strcat(weight_kg,"kg");

            glColor3f(0.0,0.0,0.0);

            print(x+18.0,y,weight_kg);



            if (x < storge_x)

                collecter_x[i] += 7.0;

        }
        else if (collect_workers_hasContainer[i].shm2_write == true)    // go back
        {

            if (collecter_x[i] > collect_x_init)

                collecter_x[i] -= 7.0;

            else

                collect_workers_hasContainer[i].shm2_write = false;

        }

    }

}



void draw_storege_area()
{

    float x = storge_x;

    float y = 240.0;

    glColor3f(0.0,0.0,0.0);

    glLineWidth(9.0);

    glBegin(GL_LINES);

    glVertex2f(x,y);

    glVertex2f(x+230.0,y);

    glVertex2f(x+230.0,y);

    glVertex2f(x+230.0,y-240.0);

    glVertex2f(x+230.0,y-240.0);

    glVertex2f(x,y-240.0);

    glVertex2f(x,y-240.0);

    glVertex2f(x,y);

    glEnd();

    // write title

    glColor3f(1.0,1.0,1.0);

    print(x+50.0,y+5.0,"Storage Area");



    // draw containers indide it

    for(int i = 0; i < MAX_NUM_OF_CONTAINERS; i++)
    {

        if (shm2[i].weight != 0)
        {

            x = (i * 20.0) + storge_x + 5.0;

            y = 205.0;

            glColor3f(1.0,1.0,1.0);

            glBegin(GL_QUADS);

            glVertex2f(x + 20.0,y+(height/4.0));

            glVertex2f(x + 20.0,y-(height/4.0));

            glVertex2f(x + (width / 6.0) + 25.0,y-(height/4.0));

            glVertex2f(x + (width / 6.0) + 25.0,y+(height/4.0));

            glEnd();

            char weight_kg[10];

            sprintf(weight_kg,"%d",shm2[i].weight);

            strcat(weight_kg,"kg");

            glColor3f(0.0,0.0,0.0);

            print(x+20.0,y,weight_kg);

        }

    }

}



void draw_split_workers()
{

    for (int i = 0; i< split_workers; i++)
    {

        float x = (i * 60.0) + storge_x + 30.0;

        float y = 135.0;

        if (split_workers_gl[i] != -1)

            drawPerson(x,y,1.0,1.0,0.0); // yellow worker for split workers

        if (split_workers_gl[i] > 0)
        {

            glColor3f(1.0,1.0,1.0);

            glBegin(GL_QUADS);

            glVertex2f(x + 20.0,y+(height/4.0));

            glVertex2f(x + 20.0,y-(height/4.0));

            glVertex2f(x + (width / 6.0) + 25.0,y-(height/4.0));

            glVertex2f(x + (width / 6.0) + 25.0,y+(height/4.0));

            glEnd();

            char weight_kg[10];

            sprintf(weight_kg,"%d",split_workers_gl[i]);

            strcat(weight_kg,"kg");

            glColor3f(0.0,0.0,0.0);

            print(x+18.0,y,weight_kg);

        }

    }

}



void draw_bags()
{

    for (int i = 0; i < MAX_NUM_OF_CONTAINERS; i++)
    {

        if (shm3[i] != 0)
        {

            float x = (i * 10.0) + storge_x + 10.0;

            float y = 80.0;

            glColor3f(0.0,0.9,1.0);

            glBegin(GL_QUADS);

            glVertex2f(x - 4.0,y+(height/7.0));

            glVertex2f(x - 4.0,y-(height/7.0));

            glVertex2f(x + 4.0,y-(height/7.0));

            glVertex2f(x + 4.0,y+(height/7.0));

            glEnd();

        }

    }

}



void draw_dstrb_workers()
{

    for (int i = 0; i< dstrb_workers; i++)
    {

        float x = (i * 70.0) + dstrb_x[i];

        float y = 20.0;

        if (dstrb_workers_gl[i].replaced == false)

            drawPerson(x,y,0.0,0.7,1.0); // light blue worker for distribution workers

        else

            drawPerson(x,y,1.0,1.0,0.0); // yellow worker for distribution workers

        glColor3f(1.0,1.0,1.0);

        print(x-40.0,y+15.0,dstrb_levels[i]);

        if (dstrb_workers_gl[i].num_of_bags != 0)
        {

            // draw these bags next to this distributer worker

            glColor3f(0.0,0.9,1.0);

            for(int j = 0; j < dstrb_workers_gl[i].num_of_bags; j++)
            {

                glBegin(GL_QUADS);

                glVertex2f(x - 4.0+20.0,y+(height/7.0));

                glVertex2f(x - 4.0+20.0,y-(height/7.0));

                glVertex2f(x + 4.0+20.0,y-(height/7.0));

                glVertex2f(x + 4.0+20.0,y+(height/7.0));

                glEnd();

                y += 15.0;

            }

            if (x > dstrb_destination[i])

                dstrb_x[i] -= 6.0;

            else

                dstrb_workers_gl[i].num_of_bags = 0;

        }

        // when reading from next fifo, if num of bags = 0, then go back

        else

            if (dstrb_x[i] < dstrb_x_init)

                dstrb_x[i]  += 6.0;

    }

}

