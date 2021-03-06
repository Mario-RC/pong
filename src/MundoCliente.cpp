// MundoCliente.cpp: implementation of the CMundo class.
//
//////////////////////////////////////////////////////////////////////
#include <fstream>
#include "MundoCliente.h"
#include "glut.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

struct sigaction act;
void controlador(int n){
	
	switch(n){
	case SIGUSR1:{
		printf("\nHA ACABADO CORRECTAMENTE\n");
		exit(0);
		break;
		} 
	case SIGINT: printf("\nsignal %d\n",n);exit(n); break;
	case SIGPIPE: printf("\nsignal %d\n",n);exit(n); break;
	case SIGTERM: printf("\nsignal %d\n",n);exit(n); break;
	
	case SIGALRM: printf("\nsignal %d\n",n);exit(n);break;
	default: break;	
	}
}

CMundo::CMundo()
{
	Init();
}

CMundo::~CMundo()
{
	close(fdBot);
	unlink("BotFIFO");

	munmap(memcomp,sizeof(DatosMemCompartida));

	Soc_ComunicacionServ.Close();
}

void CMundo::InitGL()
{
	//Habilitamos las luces, la renderizacion y el color de los materiales
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	
	glMatrixMode(GL_PROJECTION);
	gluPerspective( 40.0, 800/600.0f, 0.1, 150);
}

void print(char *mensaje, int x, int y, float r, float g, float b)
{
	glDisable (GL_LIGHTING);

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT) );

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glColor3f(r,g,b);
	glRasterPos3f(x, glutGet(GLUT_WINDOW_HEIGHT)-18-y, 0);
	int len = strlen (mensaje );
	for (int i = 0; i < len; i++) 
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_18, mensaje[i] );
		
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable( GL_DEPTH_TEST );
}

void CMundo::OnDraw()
{
	//Borrado de la pantalla	
   	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Para definir el punto de vista
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();
	
	gluLookAt(0.0, 0, 17,  // posicion del ojo
		0.0, 0.0, 0.0,      // hacia que punto mira  (0,0,0) 
		0.0, 1.0, 0.0);      // definimos hacia arriba (eje Y)    

	//////////////////////////
	//AQUÍ EMPIEZA MI DIBUJO//
	//////////////////////////

	char cad[100];
	sprintf(cad,"Jugador1: %d",puntos1);
	print(cad,10,0,1,1,1);
	sprintf(cad,"Jugador2: %d",puntos2);
	print(cad,650,0,1,1,1);

	int i;
	for(i=0;i<paredes.size();i++)paredes[i].Dibuja();

	fondo_izq.Dibuja();
	fondo_dcho.Dibuja();
	jugador1.Dibuja();
	jugador2.Dibuja();
	disparo1.Dibuja();
	disparo2.Dibuja();

	esfera.Dibuja();
	if(puntos1>0||puntos2>0) esfera2.Dibuja();

	//////////////////////////
	//AQUÍ TERMINA MI DIBUJO//
	//////////////////////////

	//Al final, cambiar el buffer
	glutSwapBuffers();
}

int disparo_jugador1;
int disparo_jugador2;
float t_bot;
float t_bots;
void CMundo::OnTimer(int value)
{
	jugador1.Mueve(0.025f);
	jugador2.Mueve(0.025f);
	disparo1.Mueve(0.025f);
	disparo2.Mueve(0.025f);
	t_bot+=0.025f;

	char cad[200];
	Soc_ComunicacionServ.Receive(cad,sizeof(cad));
	sscanf(cad,"%f %f %f %f %f %f %f %f %f %f %f %f %d %d %f %f %f %f %d %d %f",&esfera.centro.x,&esfera.centro.y,&esfera2.centro.x,&esfera2.centro.y,&jugador1.x1,&jugador1.y1,&jugador1.x2,&jugador1.y2,&jugador2.x1,&jugador2.y1,&jugador2.x2,&jugador2.y2,&puntos1,&puntos2,&disparo1.posicion.x,&disparo1.posicion.y,&disparo2.posicion.x,&disparo2.posicion.y,&disparo_jugador1,&disparo_jugador2,&t_bots);
	
	if(puntos1==10){
		printf("\n\t\t\t\tFIN: Ganador Jugador 1.\n\n");
		memcomp->fin_bot = 1;
		exit(0);
	}	
	else if(puntos2==10){
		printf("\n\t\t\t\tFIN: Ganador Jugador 2.\n\n");
		memcomp->fin_bot = 1;
		exit(0);
	}

	//Salir del Bot Jugador 2 en el Servidor
	if(t_bots==0.0f)t_bot=0.0f;

	if(puntos1>=5||puntos2>=5){
	esfera.MuevePulsante(0.025f);
	esfera2.MuevePulsante(0.025f);
	}
	else if(puntos1>=3||puntos2>=3){
	esfera.Mueve(0.025f);
	esfera2.Mueve(0.025f);
	}
	else esfera.Mueve(0.025f);

	int i;
	for(i=0;i<paredes.size();i++){
		paredes[i].Rebota(esfera);
		paredes[i].Rebota(esfera2);
		paredes[i].Rebota(jugador1);
		paredes[i].Rebota(jugador2);
	}

	jugador1.Rebota(esfera);
	jugador2.Rebota(esfera);
	jugador1.Rebota(esfera2);
	jugador2.Rebota(esfera2);

	if(fondo_izq.Rebota(esfera)||fondo_izq.Rebota(esfera2)){
		esfera.centro.x=0;
		esfera.centro.y=rand()/(float)RAND_MAX;
		esfera.velocidad.x=2+2*rand()/(float)RAND_MAX;
		esfera.velocidad.y=2+2*rand()/(float)RAND_MAX;
		esfera2.centro.x=0;
		esfera2.centro.y=rand()/(float)RAND_MAX;
		esfera2.velocidad.x=-2-2*rand()/(float)RAND_MAX;
		esfera2.velocidad.y=-2-2*rand()/(float)RAND_MAX;
		puntos2++;
		disparo_jugador1=0;
		disparo_jugador2=0;
		disparo1.posicion=10;
		disparo2.posicion=-10;
		jugador2.x1=6;jugador2.y1=1;
		jugador2.x2=6;jugador2.y2=-1;
		jugador1.x1=-6;jugador1.y1=1;
		jugador1.x2=-6;jugador1.y2=-1;
		jugador1.velocidad.y=0;
		jugador2.velocidad.y=0;
	}

	if(fondo_dcho.Rebota(esfera)||fondo_dcho.Rebota(esfera2)){
		esfera.centro.x=0;
		esfera.centro.y=rand()/(float)RAND_MAX;
		esfera.velocidad.x=-2-2*rand()/(float)RAND_MAX;
		esfera.velocidad.y=-2-2*rand()/(float)RAND_MAX;
		esfera2.centro.x=0;
		esfera2.centro.y=rand()/(float)RAND_MAX;
		esfera2.velocidad.x=2+2*rand()/(float)RAND_MAX;
		esfera2.velocidad.y=2+2*rand()/(float)RAND_MAX;
		puntos1++;
		disparo_jugador1=0;
		disparo_jugador2=0;
		disparo1.posicion=10;
		disparo2.posicion=-10;
		jugador2.x1=6;jugador2.y1=1;
		jugador2.x2=6;jugador2.y2=-1;
		jugador1.x1=-6;jugador1.y1=1;
		jugador1.x2=-6;jugador1.y2=-1;
		jugador1.velocidad.y=0;
		jugador2.velocidad.y=0;
	}

	//Interaccion disparo raqueta izq
	if(jugador1.Rebota(disparo2)){
		if(disparo_jugador2==0){
		jugador1.y1=jugador1.y1-0.4;
		jugador1.y2=jugador1.y2+0.4;
		disparo_jugador2++;
		}
	disparo2.posicion=-10;
	}

	//Interaccion disparo raqueta dcha
	if(jugador2.Rebota(disparo1)){
		if(disparo_jugador1==0){
		jugador2.y1=jugador2.y1-0.4;
		jugador2.y2=jugador2.y2+0.4;
		disparo_jugador1++;
		}
	disparo1.posicion=10;
	}

	//Disparo pared dcha
	if(fondo_dcho.Rebota(disparo1))disparo1.posicion=10;

	//Disparo pared izq
	if(fondo_izq.Rebota(disparo2))disparo2.posicion=-10;

	memcomp->raqueta1 = jugador1;
	memcomp->raqueta2 = jugador2;
	memcomp->disparo1 = disparo1;
	memcomp->disparo2 = disparo2;
	memcomp->esfera = esfera;
	memcomp->esfera2 = esfera2;
	memcomp->tiempo_bot = t_bot;

	if (memcomp->accion == 1) OnKeyboardDown('w',0,0);
	else if (memcomp->accion == -1) OnKeyboardDown('s',0,0);
	else if (memcomp->accion == 2) OnKeyboardDown('y',0,0);
	else if (memcomp->accion == -2)	OnKeyboardDown('h',0,0);
	else if(memcomp->accion == 3) OnKeyboardDown('d',0,0);
	else if(memcomp->accion == -3) OnKeyboardDown('g',0,0);
}

void CMundo::OnKeyboardDown(unsigned char key, int x, int y)
{
	switch(key)
	{
//	case 'a':jugador1.velocidad.x=-1;break;
//	case 'd':jugador1.velocidad.x=1;break;
	case 's':jugador1.velocidad.y=-4;break;
	case 'w':jugador1.velocidad.y=4;break;
	case 'l':jugador2.velocidad.y=-4;t_bot=0.0f;break;
	case 'o':jugador2.velocidad.y=4;t_bot=0.0f;break;
	case 'h':jugador2.velocidad.y=-4;break;
	case 'y':jugador2.velocidad.y=4;break;
	case 'd':disparo1.velocidad.x=6;
		 disparo1.posicion.x=jugador1.x1;
		 disparo1.posicion.y=(jugador1.y1+jugador1.y2)/2;
		 break;
	case 'k':disparo2.velocidad.x=-6;
		 disparo2.posicion.x=jugador2.x1;
		 disparo2.posicion.y=(jugador2.y1+jugador2.y2)/2;
		 t_bot=0.0f;
		 break;
	case 'g':disparo2.velocidad.x=-6;
		 disparo2.posicion.x=jugador2.x1;
		 disparo2.posicion.y=(jugador2.y1+jugador2.y2)/2;
		 break;
	}

	char cad[200];
	sprintf(cad,"%c",key);
	Soc_ComunicacionServ.Send(cad,sizeof(cad));
}

void CMundo::Init()
{
	Plano p;
	//Pared inferior
	p.x1=-7;p.y1=-5;
	p.x2=7;p.y2=-5;
	paredes.push_back(p);

	//Pared superior
	p.x1=-7;p.y1=5;
	p.x2=7;p.y2=5;
	paredes.push_back(p);

	fondo_izq.r=0;
	fondo_izq.x1=-7;fondo_izq.y1=-5;
	fondo_izq.x2=-7;fondo_izq.y2=5;

	fondo_dcho.r=0;
	fondo_dcho.x1=7;fondo_dcho.y1=-5;
	fondo_dcho.x2=7;fondo_dcho.y2=5;

	//a la izq
	jugador1.g=0;
	jugador1.x1=-6;jugador1.y1=-1;
	jugador1.x2=-6;jugador1.y2=1;

	//a la dcha
	jugador2.g=0;
	jugador2.x1=6;jugador2.y1=-1;
	jugador2.x2=6;jugador2.y2=1;

	//Inicio Disparos
	disparo1.velocidad.x=6;
	disparo1.posicion.x=10;
	disparo1.posicion.y=(jugador1.y1+jugador1.y2)/2;

	disparo2.velocidad.x=-6;
	disparo2.posicion.x=-10;
	disparo2.posicion.y=(jugador2.y1+jugador2.y2)/2;

	disparo_jugador1=0;
	disparo_jugador2=0;

	puntos1=0;
	puntos2=0;

	t_bot=0.0f;
	
	//FIFO del Bot
	fdBot=open("BotFIFO",O_CREAT|O_RDWR,0777);
	if(fdBot==-1){
	perror("ERROR MAKE_OPEN BOT");
	exit(1);
	}

	bool err=true;
	while(err){	
		fdBot=open("BotFIFO",O_CREAT|O_RDWR,0777);
		if(fdBot==-1){
		perror("ERROR MAKE_OPEN BOT");
		exit(1);
		}
		else err=false;

		act.sa_handler=controlador;
		act.sa_flags=0;

		sigaction(SIGALRM,&act,NULL);
		sigaction(SIGTERM,&act,NULL);
		sigaction(SIGPIPE,&act,NULL);
		sigaction(SIGINT,&act,NULL);
		sigaction(SIGUSR1,&act,NULL);
	}

	write(fdBot,&datosmemcomp,sizeof(datosmemcomp));

	if((memcomp=(DatosMemCompartida*) mmap(NULL,sizeof(DatosMemCompartida),PROT_READ|PROT_WRITE, MAP_SHARED,fdBot,0))==MAP_FAILED){ //Proyeccion del archivo en memoria (mmap)
	perror("ERROR MAKE MMAP");
	close(fdBot);
	exit(1);
	}

	printf("Escribe tu nombre para empezar:\n");
	scanf("%s",nombreCliente);
	Soc_ComunicacionServ.Connect("127.0.0.1",12000);
	Soc_ComunicacionServ.Send(nombreCliente,sizeof(nombreCliente));
}
