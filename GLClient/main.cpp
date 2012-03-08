#include <iostream>
#include <string>
#include <vector>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>

#include "Client.h"
#include "GLCube.h"

const double PI = 3.1415927;

class GLBoard{
    int topLeftBlack;
    std::vector<std::vector<char> > state;
    GLCube cells[8][8];
    Client c;
public:
    GLBoard():topLeftBlack(false),state(8, std::vector<char>(8, ' ')){
        for (int i=0; i<8; i++){
            for (int j=0; j<8; j++){
                cells[i][j].setSize(0.5,0.5,0.5);
                cells[i][j].setPosition(i-3.5,0,j-3.5);
            }
        }
    }
    
    int init(){
        if (!c.connect("localhost",44101)){
            std::cout<<"Main app isn't running probably..\n";
            //return 0;
        }
        Thread<GLBoard,int>(*this, &GLBoard::pollState, 0).start();
        return 1;
    }
    
    void pollState(int){
        while (1){
            //setState(c.requestLine("GET"));
            setState(".#.#.#.##.#.#.#..#.#R#.##.#R#.#..#.#.#.##.#.#B#..#.#.#.##.#.#.#.");
            Thread<GLBoard,int>::sleep(1000);
        }
    }
    
    void draw(){
        for (int i=0; i<8; i++){
            for (int j=0; j<8; j++){
                glPushMatrix();
                if ((i+j)&1) //draw !topLeftBlack
                    glColor3f(!topLeftBlack, !topLeftBlack, !topLeftBlack);
                else
                    glColor3f(topLeftBlack, topLeftBlack, topLeftBlack);
                cells[i][j].paint();
                
                switch(state[i][j]){
                    case '.':
                    case '#':
                        continue;
                    case 'R':
                        glColor3f(1,0,0);
                        break;
                    case 'G':
                        glColor3f(0,1,0);
                        break;
                    case 'B':
                        glColor3f(0,0,1);
                        break;
                }
                glTranslated(i-3.5,0.2,j-3.5);
                glPushMatrix();
                    GLUquadric *quadric = gluNewQuadric();
                    glRotated(90.0,-1,0,0);
                    gluCylinder(quadric, 0.4, 0.4, 0.4, 20,20);
                glPopMatrix();
                
                glTranslated(0,0.4,0);
                for (double angle=0; angle<=2*PI; angle+=0.01){
                    glBegin(GL_TRIANGLES);
                    glVertex3d(0,0,0);
                    glVertex3d(0.4*cos(angle), 0, 0.4*sin(angle));
                    glVertex3d(0.4*cos(angle+0.1), 0, 0.4*sin(angle+0.1));
                    glEnd();
                }
            
                glPopMatrix();
            }
        }
    }
    void setState(std::string r){
        if (r.length() != 64)
            return;
        size_t pos = -1,bwCell = r.find_first_of("#.");
        int black = r[bwCell] == '#';
        topLeftBlack = bwCell&1? black : !black;
        for (int i=0; i<8; i++)
            for (int j=0; j<8; j++){
                state[i][j] = r[++pos];
            }
    }
} board;

const int WINDOW_WIDTH=500, WINDOW_HEIGHT=500;
 
void init(){
    GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat mat_shininess[]= {50.0};
    GLfloat light_position[] = {10.0, 40.0, 1.0, 0.0};
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glColorMaterial(GL_FRONT,GL_DIFFUSE);
 
    glEnable(GL_COLOR_MATERIAL);
    //glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
}
 
void display(){
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    static double rotate=0;
    gluLookAt(0, 6, 5, 0.0, 0, 0.0, 0.0, 1.0, 0.0);
    rotate+=0.1;
    board.draw();
 
    glFlush();                          // Flush drawing routines
    glutSwapBuffers();                  // Swap buffers
    //Thread<GLBoard,int>::sleep(100);
    glutPostRedisplay();
    return;
}
 
void reshape (int w, int h){
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80.0, (GLfloat)w / (GLfloat)h, 1.00, 30.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv){
    if (!board.init())
        return 1;
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition (0,0);
    glutCreateWindow ("GLCheckers");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    init();
    glutMainLoop();
    return 0;
}