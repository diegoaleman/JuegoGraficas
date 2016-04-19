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
#include <cstdlib>
#include <ctime>
#include <string>

#include "glm.h"
#include "ImageLoader.h"

using namespace std;

int screenWidth = 800, screenHeight = 800;
double fieldWidth = 10.0, fieldHeight = 10.0;
double posX = 0, posY = 0;
double rotado = 0;
double mouseX, mouseY;


// Posiciones de drogas
double xPilAmarilla, yPilAmarilla;
double xPilRoja, yPilRoja;
double xPilBlanca, yPilBlanca;
double xHoja, yHoja;
double xMeds, yMeds;
double xJeringa, yJeringa;

double deltaPilAmarilla;

float delta=0.1;
float t=-1.0;

int sumaTotal = 0;

string fullPath = __FILE__;

int score;
int vidas;

#define MODEL_COUNT 10
#define PLAYER_MOD 0
#define PILL_ONE 1
#define PILL_TWO 2
#define PILL_THREE 3
#define LEAF 4
#define NEEDLE 5
#define MEDS 6
#define TREE 7

GLMmodel models[MODEL_COUNT];

const int TEXTURE_COUNT=8;
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

void timer(int i) {
    //if ( start){
        sumaTotal += 1;
    //}
    
    //delta = 0.1;
    //t += delta;
    glutPostRedisplay();
    glutTimerFunc(100,timer,1);

    //glutPostRedisplay();
    //glutTimerFunc(100, timer, 1);

}

double getRandom(){
    double numRand = rand() % 5 + 10;
    double signo = rand() % 10 +1;
    
    if (signo >= 5){
        return -1 * numRand;
    }
    else{
        return numRand;
    }
}

void dibujaCronometro(){
    GLint k;
    
    char mensaje [200] = "";
    
    int a,b,c;
    
    float x = sumaTotal / 10.0;
    b = x;
    
    x = x - b;
    
    x = x * 10;
    c=x;
    
    a = b /60;
    b = b %60;
    
    
    
    if (b <= 9) {
        sprintf(mensaje, "%d : 0%d : %d", a, b , c);
    } else {
        sprintf(mensaje, "%d : %d : %d", a, b , c);
    }
    
    
    glColor3f(0, 0 , 0);
    glRasterPos2f(-1, 9); // inicializa raster position
    for (k=0; mensaje[k] != '\0'; k++) {
        glColor3f(1, 1, 1);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, mensaje[k]);
    }

}

void dibujaVidas(){
    char mensaje [200] = "";
    
    if (vidas == 3){
        sprintf(mensaje, "%s", "O O O");
    }
    else if(vidas == 2){
        sprintf(mensaje, "%s", "O O");
    }
    else if (vidas == 1){
        sprintf(mensaje, "%s", "O");
    }
    else {
        sprintf(mensaje, "%s", "");
    }
    glColor3f(0, 0 , 0);
    glRasterPos2f(-8, 10); // inicializa raster position
    for (int k=0; mensaje[k] != '\0'; k++) {
        glColor3f(1, 1, 1);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, mensaje[k]);
    }
}

void dibujaPuntaje(){
    char mensaje [200] = "";
    string s = to_string(score);
    sprintf(mensaje, "%s", s.c_str());
    glColor3f(0, 0 , 0);
    glRasterPos2f(8, 10); // inicializa raster position
    for (int k=0; mensaje[k] != '\0'; k++) {
        glColor3f(1, 1, 1);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, mensaje[k]);
    }
}

void dibujaJugador()
{
   
    //glClear(GL_DEPTH_BUFFER_BIT);
    
    glPushMatrix();
    glTranslated(posX, posY, 0);
    glRotated(90, 1, 0, 0);
    glRotated(rotado, 0, 1, 0);
    glScaled(1.2, 1.2, 1.2);
    glmDraw(&models[PLAYER_MOD], GLM_COLOR | GLM_FLAT);
    glPopMatrix();
    
    cout << posX << "-" << posY << endl;

    
 
}

void dibujaPildoraRoja()
{
    
    //glClear(GL_DEPTH_BUFFER_BIT);
    
    float distX = posX - xPilRoja;
    float distY = posY - yPilRoja;
    
    xPilRoja += distX * 0.05;
    yPilRoja += distY * 0.05;
    
    glPushMatrix();
    glTranslated(xPilRoja, yPilRoja, 0);
    glRotated(90, 0, 0, 0);
    glScaled(0.5, 0.5, 0.5);
    glmDraw(&models[PILL_ONE], GLM_COLOR | GLM_FLAT);
    glPopMatrix();
}

bool revisaColision(double x1, double y1, double x2, double y2){
    if(fabs(x1-x2) <= 0.55 && fabs(y1-y2) <= 0.55){
        vidas--;
        return true;
    }
    else{
        return false;
    }
}

void dibujaPildoraAmarilla(){
    //glClear(GL_DEPTH_BUFFER_BIT);
    
    if (revisaColision(xPilAmarilla, yPilAmarilla, posX, posY)){
        deltaPilAmarilla = 0.001;
        xPilAmarilla = getRandom() - (rand() %10);
        yPilAmarilla = getRandom();
    }
    
    float distX = posX - xPilAmarilla;
    float distY = posY - yPilAmarilla;
    
    deltaPilAmarilla += 0.001;
    
    xPilAmarilla += distX * deltaPilAmarilla;
    yPilAmarilla += distY * deltaPilAmarilla;
    
    glPushMatrix();
    //glTranslated(11, 11, 0);
    glTranslated(xPilAmarilla, yPilAmarilla, 0);
    glRotated(90, 0, 0, 0);
    glScaled(0.5, 0.5, 0.5);
    glmDraw(&models[PILL_TWO], GLM_COLOR | GLM_FLAT);
    glPopMatrix();
}

void dibujaPildoraBlanca(){
    //glClear(GL_DEPTH_BUFFER_BIT);
    
    float distX = posX - xPilBlanca;
    float distY = posY - yPilBlanca;
    
    xPilBlanca += distX * 0.05;
    yPilBlanca += distY * 0.05;
    
    glPushMatrix();
    glTranslated(xPilBlanca, yPilBlanca, 0);
    glRotated(90, 0, 0, 0);
    glScaled(0.5, 0.5, 0.5);
    glmDraw(&models[PILL_THREE], GLM_COLOR | GLM_FLAT);
    glPopMatrix();
}

void dibujaArbol(){
  //  glClear(GL_DEPTH_BUFFER_BIT);
    
    
    glPushMatrix();
    glTranslated(-8, -8, 0);
    glRotated(90, 0, 0, 0);
    glScaled(2.5, 2.5, 2.5);
    glmDraw(&models[TREE], GLM_COLOR | GLM_FLAT);
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(8, -8, 0);
    glRotated(90, 0, 0, 0);
    glScaled(2.5, 2.5, 2.5);
    glmDraw(&models[TREE], GLM_COLOR | GLM_FLAT);
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(8, 8, 0);
    glRotated(90, 0, 0, 0);
    glScaled(2.5, 2.5, 2.5);
    glmDraw(&models[TREE], GLM_COLOR | GLM_FLAT);
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(-8, 8, 0);
    glRotated(90, 0, 0, 0);
    glScaled(2.5, 2.5, 2.5);
    glmDraw(&models[TREE], GLM_COLOR | GLM_FLAT);
    glPopMatrix();
}

void dibujaHoja()
{
    //glClear(GL_DEPTH_BUFFER_BIT);
    
    float distX = posX - xHoja;
    float distY = posY - yHoja;
    
    xHoja += distX * 0.05;
    yHoja += distY * 0.05;
    
    glPushMatrix();
    glTranslated(xHoja, yHoja, 0);
    glRotated(90, 0, 0, 0);
    glScaled(0.5, 0.5, 0.5);
    glPushMatrix();
    glRotated(90, 0, 0, 1);
    glmDraw(&models[LEAF], GLM_COLOR | GLM_FLAT);
    glPopMatrix();
    glPopMatrix();
}

void dibujaMeds()
{
    //glClear(GL_DEPTH_BUFFER_BIT);
    
    float distX = posX - xMeds;
    float distY = posY - yMeds;
    
    xMeds += distX * 0.05;
    yMeds += distY * 0.05;
    
    glPushMatrix();
    glTranslated(xMeds, yMeds, 0);
    glRotated(90, 0, 0, 0);
    glScaled(0.5, 0.5, 0.5);
    glmDraw(&models[MEDS], GLM_COLOR | GLM_FLAT);
    glPopMatrix();
}

void dibujaJeringa()
{
   // glClear(GL_DEPTH_BUFFER_BIT);
    
    float distX = posX - xJeringa;
    float distY = posY - yJeringa;
    
    xJeringa += distX * 0.05;
    yJeringa += distY * 0.05;
    
    glPushMatrix();
    glTranslated(xJeringa, yJeringa, 0);
    glRotated(90, 0, 0, 0);
    glScaled(1.5, 1.5, 1.5);
    glmDraw(&models[NEEDLE], GLM_COLOR | GLM_FLAT);
    glPopMatrix();
}

void dibujaEscenario(){
    
    
   // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
  //  glutPostRedisplay();

}

void display(){
    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    dibujaEscenario();
    //dibujaPildoraRoja();
    dibujaPildoraAmarilla();
    //dibujaPildoraBlanca();
    //dibujaArbol();
    //dibujaHoja();
    //dibujaMeds();
    //dibujaJeringa();
    dibujaJugador();
    //dibujaCronometro();
    dibujaVidas();
    dibujaPuntaje();
    

    
    glutSwapBuffers();
    


    
}

void reshape(int w, int h){
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-fieldWidth, fieldWidth, -fieldHeight, fieldHeight, 9.0, 20.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 11, 0, 0, 0, 0, 1, 0);
    
}



void init(){
    srand(time(0));
    
    xPilAmarilla = getRandom() - (rand() %10);
    yPilAmarilla = getRandom();
    
    xPilRoja = getRandom() - (rand() %10);
    yPilRoja = getRandom();

    xPilBlanca = getRandom() - (rand() %10);
    yPilBlanca = getRandom();

    xHoja = getRandom();
    yHoja = getRandom() - (rand() %10);
    
    xMeds = getRandom();
    yMeds = getRandom() - (rand() %10);
    
    xJeringa = getRandom();
    yJeringa = getRandom() - (rand() %10);
    
    
    
    
    deltaPilAmarilla = 0.001;
    score = 0;
    vidas = 3;
    
    
    
    
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
    
    //pildora roja
    ruta = fullPath + "Objetos/pill.obj";
    cout << "Filepath: " << ruta << std::endl;
    models[PILL_ONE] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[PILL_ONE]);
    glmVertexNormals(&models[PILL_ONE], 90.0, GL_TRUE);
    
    //pildora amarilla
    ruta = fullPath + "Objetos/pill2.obj";
    cout << "Filepath: " << ruta << std::endl;
    models[PILL_TWO] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[PILL_TWO]);
    glmVertexNormals(&models[PILL_TWO], 90.0, GL_TRUE);
    
    //pildora blanca
    ruta = fullPath + "Objetos/pill3.obj";
    cout << "Filepath: " << ruta << std::endl;
    models[PILL_THREE] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[PILL_THREE]);
    glmVertexNormals(&models[PILL_THREE], 90.0, GL_TRUE);
    
    //arbol
    ruta = fullPath + "Objetos/alstonia.obj";
    cout << "Filepath: " << ruta << std::endl;
    models[TREE] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[TREE]);
    glmVertexNormals(&models[TREE], 90.0, GL_TRUE);
    
    //hoja
    ruta = fullPath + "Objetos/leaf.obj";
    cout << "Filepath: " << ruta << std::endl;
    models[LEAF] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[LEAF]);
    glmVertexNormals(&models[LEAF], 90.0, GL_TRUE);
    
    //meds
    ruta = fullPath + "Objetos/med+bottle.obj";
    cout << "Filepath: " << ruta << std::endl;
    models[MEDS] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[MEDS]);
    glmVertexNormals(&models[MEDS], 90.0, GL_TRUE);
    
    //jeringa
    ruta = fullPath + "Objetos/needle.obj";
    cout << "Filepath: " << ruta << std::endl;
    models[NEEDLE] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[NEEDLE]);
    glmVertexNormals(&models[NEEDLE], 90.0, GL_TRUE);
    
    
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
    mouseX = (x - 400) * 2;
    mouseY = ((y - 400) * 2) * -1;
    rotado = (atan2(mouseY,mouseX) * 180 / 3.141)+180;
    //cout << rotado <<endl;
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
    glutTimerFunc(10, timer, 1);
    glutMainLoop();
    return EXIT_SUCCESS;
}
