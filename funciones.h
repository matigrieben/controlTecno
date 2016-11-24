#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
//#include "funciones.h"
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

typedef struct usuarios
{
	char codigo[27];
	char nombre[30];
	char apellido[30];
	int edad;
	int documento;
	int rango; //1 admin, 2 usuario
	struct usuarios *sig;
}usuarios;

void logg(int); 
int nuevoUsuario(int);
int ListarUsuarios(struct usuarios **, char *);
void liberarListaUsuarios(struct usuarios *);
void imprimirListaUsuarios(struct usuarios *);
int paseUsuario(struct usuarios *, char *, int *);
void imprimirUsuarioEncontrado(struct usuarios *, int);
int contrasena();
int modificarUsuario();
void stringTag(int, char *);
void encriptar(char *, int);
void camara (int);
int nuevaPass();
void alarma();
