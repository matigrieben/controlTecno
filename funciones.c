/**
	\file funciones.c
	\brief declaracion de funciones
	\details 
	\author 
	\date 2016.mes.dia
	\version 1.0.0
*/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "funciones.h"


/**
	\fn int cargarArchivo()
	\brief funcion para cargar usuarios nuevos en el archivo
	\details permite cargar nuevos usuarios con su respectiva informacion (nombre, apellido, edad, dni, rango
	\author 
	\date 2016.mes.dia
	\param ---- 
	\return estatus: 1 error; 0 carga exitosa
*/
int cargarArchivo()
{
	FILE *fp;
	int codigo, edad, dni, estatus;
	char nombre[30], apellido[30];
	
	printf("ingrese los datos del nuevo usuario: \n");
	printf("codigo "); //tiene que ser leido del rfid
	scanf("%d", &codigo);
	printf("nombre "); 
	scanf("%s", nombre);
	printf("apellido "); 
	scanf("%s", apellido);
	printf("edad "); 
	scanf("%d", &edad);
	printf("dni "); 
	scanf("%d", &dni);
	printf("rango del nuevo usuario: (1 administrador, 2 usuario normal \n"); 
	scanf("%d", &estatus);
	
	fp=fopen("usuarios.txt", "a+");
	if(fp!= NULL)
	{
		fprintf(fp, "%d,%s,%s,%d,%d,%d\n", codigo, nombre, apellido, edad, dni, estatus);
		estatus= 0;
	}
	else estatus=1;
	
	fclose(fp);
	
	return estatus;
}

/**
	\fn void log(char *usuario)
	\brief crea un archivo con la hora en la que la tarjeta fue leida 
	\details carga en un archivo el dni del usuario, seguido por la informacion de la fecha, hora etc
	\author 
	\date 2016.mes.dia
	\param dni: contiene el dni del usuario leido actualmente 
	\return -----
*/
void logg(int dni) 
{
	FILE *fp;
	
	fp=fopen("log.txt","a");
	if(fp!=NULL)
	{
		time_t t;
		char *timeAux;
		
		time(&t);
		timeAux=ctime(&t); //ctime toma la variable time_t  que contiene la hora actual y la convierte en una cadena de fecha y hora.
		
		fprintf(fp,"usuario: %d, %s",dni,timeAux);
	}
		
	fclose(fp); 
}

/**
	\fn int ListarUsuarios(struct usuarios **h, char *archivo)
	\brief Funcion que se encarga de crear la lista con todos los usuarios existentes
	\details Sin detalles
	\author 
	\date 2016.mes.dia
	\param **h header de la estructura
	\param *archivo nombre del archivo que contiene la informacion de los usuarios
	\return estatus 0 si hay error, 1 si se cargo la estructura
*/

int ListarUsuarios(usuarios **h, char *archivo)
{
	FILE *fp; 
	usuarios *nuevo = NULL, *pAux = NULL;
	char codigo[30], nombre[30], apellido[30], edadString[3], dniString[8];
	int estatus=0;
	
	fp = fopen(archivo, "r");
	if(fp == NULL) estatus =0;
	else
	{
		while(!feof(fp))
		{
			fscanf(fp, "%[^,], %[^,], %[^,], %[^,], %[^,], %d\n", codigo, nombre, apellido, edadString, dniString, &estatus);
			nuevo = (usuarios *)malloc(sizeof(usuarios));
			strcpy(nuevo->codigo, codigo);
			strcpy(nuevo->nombre, nombre);
			strcpy(nuevo->apellido, apellido);
			nuevo->edad = atoi(edadString);
			nuevo->documento= atoi(dniString);
			nuevo->rango=estatus; 
			
			nuevo->sig = NULL;
			if(*h == NULL) 
			{
				*h = nuevo;
				estatus=1;
				pAux = *h;

			}
			else
			{
				while(pAux->sig != NULL) pAux = pAux->sig;
				pAux->sig = nuevo;
				estatus=1;
			}
		}
		fclose(fp);
	}
	return estatus;
}

/**
	\fn int contraseña()
	\brief compara si las contraseñas coinciden.
	\details Sin detalles
	\author 
	\date 2016.mes.dia
	\param -----
	\return 0 si la contraseña es distinta, 1 si coinsiden las contraseñas
*/
int contrasena()
{
  char pass[20] = "admin123", pass_ingresada[20] = "";
  int status = 0;
  
  printf("Pass administrador: ");
  scanf("%s", pass_ingresada);
  if(!strcmp(pass, pass_ingresada))
    status = 1;
  
  return status;
}

int opcionesMenu()
{
	int opcion;
	printf("Elija una opcion del menu:\n");
	printf("==========================\n");
	printf("Cargar nuevo usuario (1)\n");
	printf("Modificar usuario    (2)\n");
	printf("Eliminar usuario     (3)\n");
	printf("==========================\n");
	scanf("%d", &opcion);
	
	return opcion;
}

void liberarListaUsuarios(struct usuarios *h)
{
	if(h != NULL)
	{
		liberarListaUsuarios(h->sig);
		free(h);
	}
}

void imprimirListaUsuarios(struct usuarios *h)
{
	while(h != NULL)
	{
		printf("%s--%s--%s--%d--%d\n", h->codigo, h->nombre, h->apellido, h->edad, h->documento); //imprime el dato
		h = h->sig;
	}
}

/**
	\fn int paseUsuario(struct usuarios *h, char *codigoBuscar)
	\brief busca en la estructura si existe el codigo de tarjeta ingresado
	\details Sin detalles
	\author Ariel Fischein (ariifischbein@gmail.com)
	\date 2016.mes.dia
	\param 
	\param 
	\return 
*/
int paseUsuario(struct usuarios *h, char *codigoBuscar)
{
	int flag = 0;
	if(h == NULL) flag = -1;
	else
	{
		while(!flag && h != NULL)
		{
			if(!strcmp(h->codigo, codigoBuscar))
			{
				flag = h->rango;
				if(flag==2)
					imprimirUsuarioEncontrado(h);
			}
			h = h->sig;
		}
	}
	return flag;
}

void imprimirUsuarioEncontrado(struct usuarios *h)
{
	if(h != NULL)
	{
		printf("%s--%s--%s--%d--%d\n", h->codigo, h->nombre, h->apellido, h->edad, h->documento);
	}
	return;
}



int modificarUsuario(usuarios **h)
{
	/*
	 * 
	 * 
	//----- CHECK FOR ANY RX BYTES -----
	if (uart0_filestream != -1)
	{
		// Read up to 255 characters from the port if they are there
		unsigned char rx_buffer[256];
		int rx_length = read(uart0_filestream, (void*)rx_buffer, 255);		//Filestream, buffer to store in, number of bytes to read (max)
		if (rx_length < 0)
		{
			//An error occured (will occur if there are no bytes)
		}
		else if (rx_length == 0)
		{
			//No data waiting
		}
		else
		{
			//Bytes received
			rx_buffer[rx_length] = '\0';
			printf("%i bytes read : %s\n", rx_length, rx_buffer);
		}
	}
	* 
	* */
	
	//while(
}

