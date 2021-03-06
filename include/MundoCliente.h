// MundoCliente.h: interface for the CMundo class.

#if !defined(AFX_MUNDO_H__9510340A_3D75_485F_93DC_302A43B8039A__INCLUDED_)
#define AFX_MUNDO_H__9510340A_3D75_485F_93DC_302A43B8039A__INCLUDED_

#include <vector>
#include "Plano.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <Socket.h>

#if _MSC_VER > 1000
#pragma once
#endif //_MSC_VER > 1000

#include "DatosMemCompartida.h"

class CMundo
{
public:
	void Init();
	CMundo();
	virtual ~CMundo();	
	
	void InitGL();	
	void OnKeyboardDown(unsigned char key, int x, int y);
	void OnTimer(int value);
	void OnDraw();	

	Esfera esfera;
	Esfera esfera2;
	std::vector<Plano> paredes;
	Plano fondo_izq;
	Plano fondo_dcho;
	Raqueta jugador1;
	Raqueta jugador2;
	Disparo disparo1;
	Disparo disparo2;

	int puntos1;
	int puntos2;
	int fdBot;

	Socket Soc_ComunicacionServ;
	char nombreCliente[200];

	DatosMemCompartida datosmemcomp;
	DatosMemCompartida *memcomp;
};

#endif // !defined(AFX_MUNDO_H__9510340A_3D75_485F_93DC_302A43B8039A__INCLUDED_)
