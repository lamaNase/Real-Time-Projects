#ifndef __OPENGL__H__
#define __OPENGL__H__

#include <GL/glut.h>
#include <math.h>
#include <unistd.h>

// Functions prototypes
void initGraphics(int argc, char *argv[]) ;
void drawGameGraphics();
void drawPerson(float centerX, float centerY,float r, float g, float b);
void drawCircle(float x, float y,float r,float g,float b);
void throw_to_leaderA();
void throw_to_leaderB();
void passBall(char team, int source, int destination);
void print (float x, float y,const char *text);
void beginG();
void initGraphics1(int argc,char * argv[]) ;

// Global variabels
extern float team_A[6][6] ;			
extern float team_B[6][6] ;
extern float drop[2] ;

#endif /* OPENGL.H */
