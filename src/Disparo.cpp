#include "Disparo.h"
#include "glut.h"
#include "stdlib.h"

Disparo::Disparo(void)
{
	velocidad.y=0.0F;
	velocidad.x=0.0F;
	radio=0.25f;
	posicion.x=100.0F;
	posicion.y=100.0F;
}

Disparo::~Disparo(void)
{
}

void Disparo::Dibuja()
{
	//color aleatorio
	glColor3f( rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,
 	rand()/(float)RAND_MAX);
	//estela
	//glDisable(GL_LIGHTING);
	//glBegin(GL_LINES);
	////glVertex3f(origen.x,origen.y,0);
	//glVertex3f(posicion.x,posicion.y,0);
	//glEnd();
	//glEnable(GL_LIGHTING);
	//disparo
	glPushMatrix();
	glTranslatef(posicion.x,posicion.y,0);
	glutSolidTeapot(0.1);
	glPopMatrix();
}

void Disparo::Mueve(float t)
{
	posicion=posicion+velocidad*t;
}

void Disparo::SetPos(float ix,float iy)
{	
	posicion.x=ix;
	posicion.y=iy;
	//origen.y=posicion.y+1;
	//origen.x=ix;
}
