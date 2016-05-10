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

#include "SDL2/SDL.h"
#include "SDL2_mixer/SDL_mixer.h"

using namespace std;

int screenWidth = 800, screenHeight = 800;
double fieldWidth = 10.0, fieldHeight = 10.0;
double posX = 0, posY = -1;
double rotado = 0;
double mouseX, mouseY;
double ballX = 0, ballY = 0;
bool shoot = false, op1 = false, op2 = false, op3 = false, op4 = false;
bool inicio = true, instrucciones = false, creditos = false, gameover = false;
bool informacion1 = false, informacion2 = false, informacion3 = false, informacion4 = false, informacion5 = false, informacion6 = false, jugando = false;
double mouseNormPosX = 0, mouseNormPosY = 0, distanceX = 0, distanceY = 0;
double timerFinal = 0;


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

Mix_Chunk *gamelose = NULL;
Mix_Chunk *gamewin = NULL;
Mix_Chunk *colision_bala_droga = NULL;
Mix_Chunk *colision_droga_jugador = NULL;

#define MODEL_COUNT 10
#define PLAYER_MOD 0
#define PILL_ONE 1
#define PILL_TWO 2
#define PILL_THREE 3
#define LEAF 4
#define NEEDLE 5
#define MEDS 6
#define TREE 7
#define CROSS 8
#define BALL 9

GLMmodel models[MODEL_COUNT];

const int TEXTURE_COUNT=10;
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
    
    sprintf(ruta,"%s%s", fullPath.c_str() , "Texturas/Inicio.bmp");
    image = loadBMP(ruta);loadTexture(image,i++);
    
    sprintf(ruta,"%s%s", fullPath.c_str() , "Texturas/Instrucciones.bmp");
    image = loadBMP(ruta);loadTexture(image,i++);
    
    sprintf(ruta,"%s%s", fullPath.c_str() , "Texturas/creditos.bmp");
    image = loadBMP(ruta);loadTexture(image,i++);
    
    sprintf(ruta,"%s%s", fullPath.c_str() , "Texturas/Gameover.bmp");
    image = loadBMP(ruta);loadTexture(image,i++);
    
    sprintf(ruta,"%s%s", fullPath.c_str() , "Texturas/Informacion1.bmp");
    image = loadBMP(ruta);loadTexture(image,i++);
    
    sprintf(ruta,"%s%s", fullPath.c_str() , "Texturas/Informacion2.bmp");
    image = loadBMP(ruta);loadTexture(image,i++);
    
    sprintf(ruta,"%s%s", fullPath.c_str() , "Texturas/Informacion3.bmp");
    image = loadBMP(ruta);loadTexture(image,i++);
    
    sprintf(ruta,"%s%s", fullPath.c_str() , "Texturas/Informacion4.bmp");
    image = loadBMP(ruta);loadTexture(image,i++);
    
    sprintf(ruta,"%s%s", fullPath.c_str() , "Texturas/Informacion5.bmp");
    image = loadBMP(ruta);loadTexture(image,i++);
    
    sprintf(ruta,"%s%s", fullPath.c_str() , "Texturas/Informacion6.bmp");
    image = loadBMP(ruta);loadTexture(image,i++);
    
    delete image;
}

void timer(int i) {
    if ( jugando){
        sumaTotal += 1;
    }
    
    delta = 0.1;
    t += delta;
    
    cout << "Norm pos" <<endl;
    cout << "mousex: " << mouseX/80 << " mousey: " << mouseY/80 <<endl;
    if (!shoot)
    {
        distanceX = mouseX/80 - posX;
        distanceY = mouseY/80 - posY;
    }
    
    
    double distance = sqrt(distanceX*distanceX + distanceY*distanceY);
    double easingAmount = 0.1;
    
    if (shoot)
    {
        ballY += distanceY * easingAmount;
        ballX += distanceX * easingAmount;
        
        if (ballY >= 10 || ballX >=10 || ballY <= -10 || ballX <= -10)
        {
            shoot = false;
        }
    }
    

    glutPostRedisplay();
    glutTimerFunc(1000,timer,1);
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

void resetPilAmarilla(){
    deltaPilAmarilla = 0.001;
    xPilAmarilla = getRandom() - (rand() %10);
    yPilAmarilla = getRandom();
}

void resetPilBlanca(){
    xPilBlanca = getRandom() - (rand() %10);
    yPilBlanca = getRandom();
}

void resetPilRoja(){
    xPilRoja = getRandom() - (rand() %10);
    yPilRoja = getRandom();
    
}

void resetHoja(){
    xHoja = getRandom();
    yHoja = getRandom() - (rand() %10);
}

void resetMeds(){
    xMeds = getRandom();
    yMeds = getRandom() - (rand() %10);
}

void resetJeringa(){
    xJeringa = getRandom();
    yJeringa = getRandom() - (rand() %10);
}

void resetBallPosition() {
    ballX = posX;
    ballY = posY;
}

void resetGame() {
    posX = 0;
    posY = 0;
    jugando = true;
    vidas = 6;
    sumaTotal = 0;
    score = 0;
    inicio = false;
    instrucciones = false;
    creditos = false;
    informacion6 = false;
    informacion5 = false;
    informacion4 = false;
    informacion3 = false;
    informacion2 = false;
    informacion1 = false;
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
    
    
    glColor3f(255, 255 , 255);
    glRasterPos2f(-1, 10); // inicializa raster position
    for (k=0; mensaje[k] != '\0'; k++) {
        glColor3f(1, 1, 1);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, mensaje[k]);
    }

}

void dibujaVidas(){
    char mensaje [200] = "";
    
    
    if (vidas == 6){
        sprintf(mensaje, "%s", "O O O O O O");
    }
    else if (vidas == 5){
        sprintf(mensaje, "%s", "O O O O O");
    }
    else if (vidas == 4){
        sprintf(mensaje, "%s", "O O O O");
    }
    else if (vidas == 3){
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
    glColor3f(255, 255 , 255);
    glRasterPos2f(-8, 10); // inicializa raster position
    for (int k=0; mensaje[k] != '\0'; k++) {
        glColor3f(255, 255, 255);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, mensaje[k]);
    }
}

void dibujaPuntaje(){
    char mensaje [200] = "";
    string s = to_string(score);
    sprintf(mensaje, "%s", s.c_str());
    glColor3f(255, 255 , 255);
    glRasterPos2f(8, 10); // inicializa raster position
    for (int k=0; mensaje[k] != '\0'; k++) {
        glColor3f(255, 255, 255);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, mensaje[k]);
    }
}

void dibujaJugador(){
   
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



void muestraInfo() {
    
    if (vidas == 5)
    {
        informacion5 = true;
        jugando = false;
    }
    else if (vidas == 4)
    {
        informacion4 = true;
        jugando = false;
    }
    else if (vidas == 3)
    {
        informacion3 = true;
        jugando = false;
    }
    else if (vidas == 2)
    {
        informacion2 = true;
        jugando = false;
    }
    else if (vidas == 1)
        {
        informacion1 = true;
        jugando = false;
    }
    else if (vidas == 0){
        jugando = false;
        gameover = true;
        timerFinal = sumaTotal;
        
        
    }
}

bool revisaColisionDrogaJugador(double x1, double y1, double x2, double y2){
    if(fabs(x1-x2) <= 0.55 && fabs(y1-y2) <= 0.55)
    {
        vidas--;
        muestraInfo();
        Mix_PlayChannel( 0, colision_droga_jugador, 0 );
        return true;
    }
    else
    {
        return false;
    }
}

void dibujaPildoraAmarilla(){
    //glClear(GL_DEPTH_BUFFER_BIT);
    
    if (revisaColisionDrogaJugador(xPilAmarilla, yPilAmarilla, posX, posY))
    {
        resetPilAmarilla();
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


void dibujaPildoraRoja(){
    
    //glClear(GL_DEPTH_BUFFER_BIT);
    
    if (revisaColisionDrogaJugador(xPilRoja, yPilRoja, posX, posY)){
        resetPilRoja();
    }
    
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

void dibujaPildoraBlanca(){
    //glClear(GL_DEPTH_BUFFER_BIT);
    
    if (revisaColisionDrogaJugador(xPilBlanca, yPilBlanca, posX, posY)){
        resetPilBlanca();
    }
    
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

void dibujaHoja(){
    //glClear(GL_DEPTH_BUFFER_BIT);
    
    if (revisaColisionDrogaJugador(xHoja, yHoja, posX, posY)){
        resetHoja();
    }
    
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

void dibujaMeds() {
    //glClear(GL_DEPTH_BUFFER_BIT);
    
    if (revisaColisionDrogaJugador(xMeds, yMeds, posX, posY)){
        resetMeds();
    }
    
    float distX = posX - xMeds;
    float distY = posY - yMeds;
    
    xMeds += distX * 0.05;
    yMeds += distY * 0.05;
    
    glPushMatrix();
    glTranslated(xMeds, yMeds, 0);
    glRotated(45, 1, 1, 0);
    glScaled(0.5, 0.5, 0.5);
    glmDraw(&models[MEDS], GLM_COLOR | GLM_FLAT);
    glPopMatrix();
}

void dibujaJeringa() {
   // glClear(GL_DEPTH_BUFFER_BIT);
    
    if (revisaColisionDrogaJugador(xJeringa, yJeringa, posX, posY)){
        resetJeringa();
    }
    
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


bool revisaColisionBalaDrogas(double x, double y){
    
    if(fabs(x-xPilAmarilla) <= 0.55 && fabs(y-yPilAmarilla) <= 0.55){
        shoot = false;
        resetPilAmarilla();
        Mix_PlayChannel( 0, colision_bala_droga, 0 );
        return true;
    }
    else if (fabs(x-xPilBlanca) <= 0.55 && fabs(y-yPilBlanca) <= 0.55){
        shoot = false;
        resetPilBlanca();
        Mix_PlayChannel( 0, colision_bala_droga, 0 );
        return true;
    }
    else if (fabs(x-xPilRoja) <= 0.55 && fabs(y-yPilRoja) <= 0.55){
        shoot = false;
        resetPilRoja();
        Mix_PlayChannel( 0, colision_bala_droga, 0 );
        return true;
    }
    else if (fabs(x-xHoja) <= 0.55 && fabs(y-yHoja) <= 0.55){
        shoot = false;
        resetHoja();
        Mix_PlayChannel( 0, colision_bala_droga, 0 );
        return true;
    }
    else if (fabs(x-xMeds) <= 0.55 && fabs(y-yMeds) <= 0.55){
        shoot = false;
        resetMeds();
        Mix_PlayChannel( 0, colision_bala_droga, 0 );
        return true;
    }
    else if (fabs(x-xJeringa) <= 0.55 && fabs(y-yJeringa) <= 0.55){
        shoot = false;
        resetJeringa();
        Mix_PlayChannel( 0, colision_bala_droga, 0 );
        return true;
    }
    else{
        return false;
    }

}


void dibujaBola() {
    // glClear(GL_DEPTH_BUFFER_BIT);
    
    
    if (revisaColisionBalaDrogas(ballX, ballY)){
        score++;
        //resetBallPosition();
    }
    
    glPushMatrix();
    glTranslated(ballX, ballY, 0);
    glRotated(90, 0, 0, 0);
    glScaled(0.5, 0.5, 0.5);
    glmDraw(&models[BALL], GLM_COLOR | GLM_FLAT);
    glPopMatrix();
}

void dibujaEscenario(){
   // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glTranslated(0, 0, -5);
    //glScaled(1.5, 1.5, 0);
    //Habilitar el uso de texturas
    glEnable(GL_TEXTURE_2D);
    
    //Elegir la textura del Quads: angulo cambia con el timer
    glBindTexture(GL_TEXTURE_2D, texName[0]);
    
    glBegin(GL_QUADS);
    //Asignar la coordenada de textura 0,0 al vertice
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-20.0f, -20.0f, 0);
    //Asignar la coordenada de textura 1,0 al vertice
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(20.0f, -20.0f, 0);
    //Asignar la coordenada de textura 1,1 al vertice
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(20.0f, 20.0f, 0);
    //Asignar la coordenada de textura 0,1 al vertice
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-20.0f, 20.0f, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    glPopMatrix();
  //  glutPostRedisplay();

}

void dibujaPantallaInicio(){
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
   // glPushMatrix();
    
    //Habilitar el uso de texturas
    glEnable(GL_TEXTURE_2D);
    
    //Elegir la textura del Quads: angulo cambia con el timer
    glBindTexture(GL_TEXTURE_2D, texName[1]);
    
    glBegin(GL_QUADS);
    //Asignar la coordenada de textura 0,0 al vertice
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-13.0f, -13.0f, 0);
    //Asignar la coordenada de textura 1,0 al vertice
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(13.0f, -13.0f, 0);
    //Asignar la coordenada de textura 1,1 al vertice
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(13.0f, 13.0f, 0);
    //Asignar la coordenada de textura 0,1 al vertice
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-13.0f, 13.0f, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
   // glPopMatrix();
    //  glutPostRedisplay();
    
}

void dibujaInstrucciones(){
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //glPushMatrix();
    
    //Habilitar el uso de texturas
    glEnable(GL_TEXTURE_2D);
    
    //Elegir la textura del Quads: angulo cambia con el timer
    glBindTexture(GL_TEXTURE_2D, texName[2]);
    
    glBegin(GL_QUADS);
    //Asignar la coordenada de textura 0,0 al vertice
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-13.0f, -13.0f, 0);
    //Asignar la coordenada de textura 1,0 al vertice
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(13.0f, -13.0f, 0);
    //Asignar la coordenada de textura 1,1 al vertice
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(13.0f, 13.0f, 0);
    //Asignar la coordenada de textura 0,1 al vertice
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-13.0f, 13.0f, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
  //  glPopMatrix();
    //  glutPostRedisplay();
    
}

void dibujaCreditos(){
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
   // glPushMatrix();
    
    //Habilitar el uso de texturas
    glEnable(GL_TEXTURE_2D);
    
    //Elegir la textura del Quads: angulo cambia con el timer
    glBindTexture(GL_TEXTURE_2D, texName[3]);
    
    glBegin(GL_QUADS);
    //Asignar la coordenada de textura 0,0 al vertice
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-13.0f, -13.0f, 0);
    //Asignar la coordenada de textura 1,0 al vertice
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(13.0f, -13.0f, 0);
    //Asignar la coordenada de textura 1,1 al vertice
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(13.0f, 13.0f, 0);
    //Asignar la coordenada de textura 0,1 al vertice
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-13.0f, 13.0f, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
  //  glPopMatrix();
    //  glutPostRedisplay();
    
}

void dibujaGameOver(){
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    Mix_PlayChannel( 0, gamelose, 0 );
    
    
   // glPushMatrix();
    
    //Habilitar el uso de texturas
    glEnable(GL_TEXTURE_2D);
    
    //Elegir la textura del Quads: angulo cambia con el timer
    glBindTexture(GL_TEXTURE_2D, texName[4]);
    
    glBegin(GL_QUADS);
    //Asignar la coordenada de textura 0,0 al vertice
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-13.0f, -13.0f, 0);
    //Asignar la coordenada de textura 1,0 al vertice
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(13.0f, -13.0f, 0);
    //Asignar la coordenada de textura 1,1 al vertice
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(13.0f, 13.0f, 0);
    //Asignar la coordenada de textura 0,1 al vertice
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-13.0f, 13.0f, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    //resetGame();
    
    
    
    
    char mensaje [200] = "";
    string s = to_string(score);
    sprintf(mensaje, "%s", s.c_str());
    glColor3f(0, 0 , 0);
    glRasterPos2f(3, 2.5); // inicializa raster position
    for (int k=0; mensaje[k] != '\0'; k++) {
        glColor3f(1, 1, 1);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, mensaje[k]);
    }
    
    
    GLint k;
    
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
    glRasterPos2f(3, -1.5); // inicializa raster position
    for (k=0; mensaje[k] != '\0'; k++) {
        glColor3f(1, 1, 1);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, mensaje[k]);
    }

    
}

void dibujaInformacion1(){
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
  //  glPushMatrix();
    
    //Habilitar el uso de texturas
    glEnable(GL_TEXTURE_2D);
    
    //Elegir la textura del Quads: angulo cambia con el timer
    glBindTexture(GL_TEXTURE_2D, texName[5]);
    
    glBegin(GL_QUADS);
    //Asignar la coordenada de textura 0,0 al vertice
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-13.0f, -13.0f, 0);
    //Asignar la coordenada de textura 1,0 al vertice
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(13.0f, -13.0f, 0);
    //Asignar la coordenada de textura 1,1 al vertice
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(13.0f, 13.0f, 0);
    //Asignar la coordenada de textura 0,1 al vertice
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-13.0f, 13.0f, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
  //  glPopMatrix();
    //  glutPostRedisplay();
    
}

void dibujaInformacion2(){
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
  //  glPushMatrix();
    
    //Habilitar el uso de texturas
    glEnable(GL_TEXTURE_2D);
    
    //Elegir la textura del Quads: angulo cambia con el timer
    glBindTexture(GL_TEXTURE_2D, texName[6]);
    
    glBegin(GL_QUADS);
    //Asignar la coordenada de textura 0,0 al vertice
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-13.0f, -13.0f, 0);
    //Asignar la coordenada de textura 1,0 al vertice
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(13.0f, -13.0f, 0);
    //Asignar la coordenada de textura 1,1 al vertice
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(13.0f, 13.0f, 0);
    //Asignar la coordenada de textura 0,1 al vertice
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-13.0f, 13.0f, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
  
   // glPopMatrix();
    //  glutPostRedisplay();
    
}

void dibujaInformacion3(){
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
  //  glPushMatrix();
    
    //Habilitar el uso de texturas
    glEnable(GL_TEXTURE_2D);
    
    //Elegir la textura del Quads: angulo cambia con el timer
    glBindTexture(GL_TEXTURE_2D, texName[7]);
    
    glBegin(GL_QUADS);
    //Asignar la coordenada de textura 0,0 al vertice
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-13.0f, -13.0f, 0);
    //Asignar la coordenada de textura 1,0 al vertice
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(13.0f, -13.0f, 0);
    //Asignar la coordenada de textura 1,1 al vertice
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(13.0f, 13.0f, 0);
    //Asignar la coordenada de textura 0,1 al vertice
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-13.0f, 13.0f, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
 //   glPopMatrix();
    //  glutPostRedisplay();
    
}

void dibujaInformacion4(){
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //  glPushMatrix();
    
    //Habilitar el uso de texturas
    glEnable(GL_TEXTURE_2D);
    
    //Elegir la textura del Quads: angulo cambia con el timer
    glBindTexture(GL_TEXTURE_2D, texName[8]);
    
    glBegin(GL_QUADS);
    //Asignar la coordenada de textura 0,0 al vertice
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-13.0f, -13.0f, 0);
    //Asignar la coordenada de textura 1,0 al vertice
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(13.0f, -13.0f, 0);
    //Asignar la coordenada de textura 1,1 al vertice
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(13.0f, 13.0f, 0);
    //Asignar la coordenada de textura 0,1 al vertice
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-13.0f, 13.0f, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    //   glPopMatrix();
    //  glutPostRedisplay();
    
}


void dibujaInformacion5(){
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //  glPushMatrix();
    
    //Habilitar el uso de texturas
    glEnable(GL_TEXTURE_2D);
    
    //Elegir la textura del Quads: angulo cambia con el timer
    glBindTexture(GL_TEXTURE_2D, texName[9]);
    
    glBegin(GL_QUADS);
    //Asignar la coordenada de textura 0,0 al vertice
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-13.0f, -13.0f, 0);
    //Asignar la coordenada de textura 1,0 al vertice
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(13.0f, -13.0f, 0);
    //Asignar la coordenada de textura 1,1 al vertice
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(13.0f, 13.0f, 0);
    //Asignar la coordenada de textura 0,1 al vertice
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-13.0f, 13.0f, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    //   glPopMatrix();
    //  glutPostRedisplay();
    
}

void dibujaInformacion6(){
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //  glPushMatrix();
    
    //Habilitar el uso de texturas
    glEnable(GL_TEXTURE_2D);
    
    //Elegir la textura del Quads: angulo cambia con el timer
    glBindTexture(GL_TEXTURE_2D, texName[10]);
    
    glBegin(GL_QUADS);
    //Asignar la coordenada de textura 0,0 al vertice
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-13.0f, -13.0f, 0);
    //Asignar la coordenada de textura 1,0 al vertice
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(13.0f, -13.0f, 0);
    //Asignar la coordenada de textura 1,1 al vertice
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(13.0f, 13.0f, 0);
    //Asignar la coordenada de textura 0,1 al vertice
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-13.0f, 13.0f, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    //   glPopMatrix();
    //  glutPostRedisplay();
    
}

void display(){
    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (inicio)
    {
        dibujaPantallaInicio();
    }
    else
        if (jugando)
        {
            
            
            GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
            GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
            GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
            /*  light_position is NOT default value */
            GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
            
            glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
            glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
            glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
            glLightfv(GL_LIGHT0, GL_POSITION, light_position);
            
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);
            glEnable(GL_DEPTH_TEST);
            glShadeModel (GL_FLAT);//   GL_SMOOTH
            
            
            
            dibujaEscenario();
            //  dibujaPildoraRoja();
            if (vidas == 6)
            {
                dibujaJeringa();
            }
            else if (vidas == 5)
            {
                dibujaPildoraBlanca();
            }
            else if (vidas == 4)
            {
                dibujaPildoraRoja();
            }
            else if (vidas == 3)
            {
                dibujaPildoraAmarilla();
            }
            else if (vidas == 2)
            {
                dibujaHoja();
            }
            else if (vidas == 1)
            {
                dibujaMeds();
            }

            
            
            
            
            
            if (shoot)
            {
                dibujaBola();
            }
            dibujaJugador();
            dibujaCronometro();
            dibujaVidas();
            dibujaPuntaje();
        } else if (instrucciones) {
            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
            glDisable(GL_DEPTH_TEST);

            dibujaInstrucciones();
        } else if (creditos) {
            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
            glDisable(GL_DEPTH_TEST);
            dibujaCreditos();
        } else if (gameover) {
            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
            glDisable(GL_DEPTH_TEST);
            dibujaGameOver();
        } else if (informacion1) {
            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
            glDisable(GL_DEPTH_TEST);
            dibujaInformacion1();
        } else if (informacion2) {
            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
            glDisable(GL_DEPTH_TEST);
            dibujaInformacion2();
        }
        else if (informacion3) {
            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
            glDisable(GL_DEPTH_TEST);
            dibujaInformacion3();
        }
        else if (informacion4) {
            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
            glDisable(GL_DEPTH_TEST);
            dibujaInformacion4();
        }
        else if (informacion5) {
            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
            glDisable(GL_DEPTH_TEST);
            dibujaInformacion5();
        }
        else if (informacion6) {
            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
            glDisable(GL_DEPTH_TEST);
            dibujaInformacion6();
        }
    
    
    
    
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


/****************************************************************
 *
 *  Funcion que carga todo lo necesario para correr el juego
 *
 ****************************************************************/
void init(){
    srand(time(NULL));
    
    resetPilRoja();
    resetPilBlanca();
    resetPilAmarilla();
    resetHoja();
    resetMeds();
    resetJeringa();
    
    deltaPilAmarilla = 0.001;
    score = 0;
    vidas = 6;
    
    
    //General settings
    glClearColor(0,0,0, 1.0);
    //glEnable(GL_DEPTH_TEST);
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
    
    //cruz
    ruta = fullPath + "Objetos/cross.obj";
    cout << "Filepath: " << ruta << std::endl;
    models[CROSS] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[CROSS]);
    glmVertexNormals(&models[CROSS], 90.0, GL_TRUE);
    
    //bola roja
    ruta = fullPath + "Objetos/ball.obj";
    cout << "Filepath: " << ruta << std::endl;
    models[BALL] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[BALL]);
    glmVertexNormals(&models[BALL], 90.0, GL_TRUE);
    
    
    
    
    
    
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        //success = false;
    }
    
    //Initialize SDL_mixer
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
        //success = false;
    }

    
    
    
    //Load music
    char rutamusica[200] = "";
    
    ruta = fullPath + "Sonidos/gameover.wav";
    gamelose = Mix_LoadWAV(ruta.c_str());
    if( gamelose == NULL ){
        printf( "Failed to load gamefail sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
    }
    
    ruta = fullPath + "Sonidos/gamewin.wav";
    gamewin = Mix_LoadWAV(ruta.c_str());
    if( gamewin == NULL ){
        printf( "Failed to load gamefail sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
    }
    
    ruta = fullPath + "Sonidos/colision_bala_droga.wav";
    colision_bala_droga = Mix_LoadWAV(ruta.c_str());
    if( colision_bala_droga == NULL ){
        printf( "Failed to load gamefail sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
    }

    
    ruta = fullPath + "Sonidos/colision_droga_jugador.wav";
    colision_droga_jugador = Mix_LoadWAV(ruta.c_str());
    if( colision_droga_jugador == NULL ){
        printf( "Failed to load gamefail sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
    }



    
}




/****************************************************************
 *
 *  Funcion para hacer una accion al click de una letra
 *
 ****************************************************************/
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
        case 'i':
        case 'I':
            inicio = false;
            instrucciones = true;
            jugando = false;
            break;
        case 'c':
        case 'C':
            creditos = true;
            inicio = false;
            jugando = false;
            break;
        case 'b':
        case 'B':
            inicio = true;
            creditos = false;
            jugando = false;
            break;
        case 'm':
        case 'M':
            if (gameover) {
                gameover = false;
                inicio = true;
                jugando = false;
            }
            break;
        case 'r':
        case 'R':
            if (gameover) {
                gameover = false;
                resetGame();
            }
            break;
        case 'j':
        case 'J':
            instrucciones = false;
            jugando = true;
            resetGame();
            break;
        case 27:
            exit(0);
            break;
    }
    
}




/****************************************************************
 *
 *  Funciones de mouse para saber su posicion ante una accion
 *  (un click) o cuando se mueve
 *
 ****************************************************************/
void mousePasivo(int x, int y){
    mouseX = (x - (screenWidth/2)) * 2;
    mouseY = ((y - (screenHeight/2)) * 2) * -1;
    rotado = (atan2(mouseY,mouseX) * 180 / 3.141)+180;
    
    //cout << rotado <<endl;
    cout << "posicion x: " << mouseX <<" posicion y: "<< mouseY << endl;
    
}

void mouseActivo(int button, int state, int x, int y){
    if (jugando) {
        resetBallPosition();
        shoot = true;
        glutTimerFunc(800, timer, 1);
    } else if (informacion1|| informacion2 || informacion3 || informacion4 || informacion5 || informacion6) {
        informacion1 = false;
        informacion2 = false;
        informacion3 = false;
        informacion4 = false;
        informacion5 = false;
        informacion6 = false;
        jugando = true;
    }
    
}




/****************************************************************
 *
 *  Funcion main
 *
 ****************************************************************/
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
    glutTimerFunc(800, timer, 1);
    glutMainLoop();
    return EXIT_SUCCESS;
}
