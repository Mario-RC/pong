#pragma once

#include "Disparo.h"
#include "Esfera.h"
#include "Raqueta.h"

class DatosMemCompartida
{
public:         
	Esfera esfera;
	Esfera esfera2;
	Raqueta raqueta1;
	Raqueta raqueta2;
	Disparo disparo1;
	Disparo disparo2;
	float tiempo_bot;
	int accion;
	int fin_bot;
};
