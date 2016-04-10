//
//  main.cpp
//  JuegoGraficas
//
//  Created by Diego Aleman & Jorge Perales on 4/9/16.
//  Copyright © 2016 Diego Aleman & Jorge Perales. All rights reserved.
//


#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <cstring>
#include <math.h>

#include "glm.h"
#include "ImageLoader.h"

using namespace std;

int screenWidth = 800,screenHeight = 800;
double fieldWidth = 10.0, fieldHeight = 10.0;
double posX = 0, posY = 0;
double mouseX, mouseY;

string fullPath = __FILE__;

#define MODEL_COUNT 10
#define PLAYER_MOD 0

GLMmodel models[MODEL_COUNT];

const int TEXTURE_COUNT=7;
static GLuint texName[TEXTURE_COUNT];

void getParentPath(){
    for (int i = (int)fullPath.length()-1; i>=0 && fullPath[i] != '/'; i--) {
        fullPath.erase(i,1);
    }
}

void loadTexture(Image* image,int k){
    
    glBindTexture(GL_TEXTURE_2D, texName[k]); //Tell OpenGL which texture to edit
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    //Filtros de ampliacion y redución con cálculo mas cercano no es tan bueno pero es rápido
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    
    //Filtros de ampliacion y redución con cálculo lineal es mejo pero son más calculos
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    
    //Map the image to the texture
    glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
                 0,                            //0 for now
                 GL_RGB,                       //Format OpenGL uses for image
                 image->width, image->height,  //Width and height
                 0,                            //The border of the image
                 GL_RGB, //GL_RGB, because pixels are stored in RGB format
                 GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
                 //as unsigned numbers
                 image->pixels);               //The actual pixel data
}

void initRendering(){
    //Declaración del objeto Image
    Image* image;
    GLuint i=0;
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glGenTextures(TEXTURE_COUNT, texName); //Make room for our texture
    
    
    char  ruta[200];
    sprintf(ruta,"%s%s", fullPath.c_str() , "Texturas/diffuse.bmp");
    image = loadBMP(ruta);loadTexture(image,i++);
    
    delete image;
}


void dibujaJugador()
{
    glClear(GL_DEPTH_BUFFER_BIT);

    
    glPushMatrix();
    glTranslated(posX, posY, 0);
    glRotated(90, 0, 0, 0);
    glScaled(1.5, 1.5, 1.5);
    glmDraw(&models[PLAYER_MOD], GLM_COLOR | GLM_FLAT);
    glPopMatrix();
    
    
 
}

void dibujaEscenario(){
    
    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    
    //Habilitar el uso de texturas
    glEnable(GL_TEXTURE_2D);
    
    //Elegir la textura del Quads: angulo cambia con el timer
    glBindTexture(GL_TEXTURE_2D, texName[0]);
    
    glBegin(GL_QUADS);
    //Asignar la coordenada de textura 0,0 al vertice
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-10.0f, -10.0f, 0);
    //Asignar la coordenada de textura 1,0 al vertice
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(10.0f, -10.0f, 0);
    //Asignar la coordenada de textura 1,1 al vertice
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(10.0f, 10.0f, 0);
    //Asignar la coordenada de textura 0,1 al vertice
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-10.0f, 10.0f, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    glPopMatrix();
    glutPostRedisplay();

}

void display(){
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glPushMatrix();
    dibujaEscenario();
    dibujaJugador();
    glPopMatrix();
    
    glutSwapBuffers();

    
}

void reshape(int w, int h){
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-fieldWidth, fieldWidth, -fieldHeight, fieldHeight, 10.0, 30.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0,15 , 0, 0, 0, 0, 1, 0);
    
}

void init(){
    //General settings
    glClearColor(0,0,0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    
    //player
    string ruta = fullPath + "Objetos/boy.obj";
    cout << "Filepath: " << ruta << std::endl;
    models[PLAYER_MOD] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[PLAYER_MOD]);
    glmVertexNormals(&models[PLAYER_MOD], 90.0, GL_TRUE);
    
    
}

void myKeyboard(unsigned char theKey, int x, int y){

    switch (theKey){
        case 'w':
        case 'W':
            posY += 0.5;
            break;
        case 's':
        case 'S':
            posY-= 0.5;
                break;
        case 'a':
        case 'A':
            posX -= 0.5;
                break;
        case 'd':
        case 'D':
            posX += 0.5;
            break;
        case 27:
            exit(0);
            break;
        
    }
    
}
void mousePasivo(int x, int y){
    mouseX = x;
    mouseY = y;
}

void mouseActivo(int button, int state, int x, int y){
    
}

int main(int argc, char *argv[]) {
    getParentPath();    //Find root path for files
    glutInit(&argc,argv);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowPosition(50,50);
    glutInitWindowSize(screenWidth,screenHeight);
    glutCreateWindow("Juego Graficas");
    initRendering();
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(myKeyboard);
    glutPassiveMotionFunc(mousePasivo);
    glutMouseFunc(mouseActivo);
    //glutTimerFunc(100, timer, 1);
    glutMainLoop();
    return EXIT_SUCCESS;
}
