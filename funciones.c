#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "funciones.h"
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

/**
	\fn int nuevoUsuario()
	\brief funcion para cargar usuarios nuevos en el archivo
	\details permite cargar nuevos usuarios con su respectiva informacion (nombre, apellido, edad, dni, rango
	\author 
	\date 2016.mes.dia
	\param ---- 
	\return estatus: 1 error; 0 carga exitosa
*/
int nuevoUsuario(int uart0_filestream)
{
	FILE *fp;
	int flag=1, codigo, edad, dni, estatus = 0;
	char nombre[30], apellido[30], vectorTag[27];
	printf("Ingrese los datos del nuevo usuario:\n");
	printf("Pase la tarjeta del nuevo usuario: ");
	stringTag(uart0_filestream, vectorTag);
	printf("nombre "); 
	scanf("%s", nombre);
	printf("apellido "); 
	scanf("%s", apellido);
	printf("edad "); 
	scanf("%d", &edad);
	printf("dni "); 
	scanf("%d", &dni);
	camara(dni);
	cvStartWindowThread();
	printf("rango del nuevo usuario: (1 administrador, 2 usuario) \n"); 
	scanf("%d", &estatus);
	printf("Usuario agregado exitosamente!\n");
	fp=fopen("usuarios.txt", "a+");
	if(fp!= NULL) fprintf(fp, "%s,%s,%s,%d,%d,%d\n", vectorTag, nombre, apellido, edad, dni, estatus);
	else estatus = 1;	
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
	if(fp != NULL)
	{
		time_t t;
		char *timeAux;
		time(&t);
		timeAux = ctime(&t); //ctime toma la variable time_t  que contiene la hora actual y la convierte en una cadena de fecha y hora.
		fprintf(fp,"Usuario: %d, %s", dni, timeAux);
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
	char codigo[30], nombre[30], apellido[30], edadString[8] = "", dniString[8];
	int estatus = 0;
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
			nuevo->documento = atoi(dniString);
			nuevo->rango = estatus;
			nuevo->sig = NULL;
			printf("%s ---- %d edadstring:%s     %d\n", nuevo->nombre, nuevo->edad, edadString, nuevo->documento);
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
  FILE *fp = fopen("pass.txt", "r");
  char pass[20] = "", pass_ingresada[20] = "";
  int status = 0;
  if(fp != NULL)
  {
    fscanf(fp, "%s", pass);
    printf("Pass administrador: ");
    scanf("%s", pass_ingresada);
    encriptar(pass_ingresada, strlen(pass_ingresada));
    if(!strcmp(pass, pass_ingresada)) status = 1;
    fclose(fp);
  }
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
	printf("Cambiar contraseña   (4)\n");
	printf("Finalizar programa   (5)\n");
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
				if(flag==2) imprimirUsuarioEncontrado(h);
				imprimirUsuarioEncontrado(h); //para admin, eliminar despues
				logg(h->documento);
			}
			h = h->sig;
		}
	}
	return flag;
}

void imprimirUsuarioEncontrado(struct usuarios *h)
{
	char buf[15];
	IplImage *img = NULL;
	int key = 0;
	if(h != NULL)
	{
		printf("%s--%s--%s--%d--%d\n", h->codigo, h->nombre, h->apellido, h->edad, h->documento);
		sprintf(buf, "%d.jpg", h->documento);
		img = cvLoadImage(buf, CV_LOAD_IMAGE_COLOR);
		cvNamedWindow( "Usuario", CV_WINDOW_AUTOSIZE); 
  		cvShowImage("Usuario", img);
  		if(h->rango == 1)
		{
			while((key = cvWaitKey(1)) < 0){}
	  		cvDestroyWindow("Usuario");
		}
	}
	cvStartWindowThread();
	return;
}

int modificarUsuario(usuarios **h)
{
	int status;
	return status;
}

void stringTag(int uart0_filestream, char vector[27])
{
    int rx_length = 0, hola = 0, contador = 0, flag = 1;
	unsigned char rx_buffer[100];
    tcflush(uart0_filestream, TCIFLUSH);
	while(flag)
	{
		if (uart0_filestream != -1)
		{
			rx_length = read(uart0_filestream, (void *)rx_buffer, 100);				
			if (rx_length > 0)
			{	
				hola = hola + rx_length;
				while(contador < rx_length && hola < 27)
				{
				  vector[(hola - rx_length + contador)] = rx_buffer[contador];
				  contador++;
				}
				contador = 0;
				if(hola > 25)
				{	
					flag=0;
					vector[26] = '\0'; 	
					printf("hola %d    bytes read : %s\n", hola, vector);		
				}
			}			

		}
	}
	return;
}

void encriptar(char* password, int cant)
{  
	int i; 
	for(i = 0;i < cant; i++)
	{
	    if(i%2 != 0) password[i] = password[i]-13;    //Le resto a la letra 13.
	    else password[i] = password[i]+13;    //Le sumo a la letra 13.
	}
}

void camara (int dni)
{
    int key = 0;    
	char buf[15];
	sprintf(buf, "%d.jpg", dni);
    IplImage *frame= NULL;
    CvCapture* capture;
    capture = cvCaptureFromCAM( -1 );
    cvSetCaptureProperty(capture, CV_CAP_PROP_FPS, 31.0);
    cvNamedWindow("imagen", CV_WINDOW_AUTOSIZE);
    do 
    {
        frame = cvQueryFrame( capture );
        cvShowImage("imagen", frame);
    }while((key = cvWaitKey(1)) < 0);    
	cvSaveImage(buf, frame,0);
	cvDestroyWindow("imagen");
	cvReleaseCapture( &capture );
}

int nuevaPass()
{
	FILE *fp = fopen("pass.txt", "a+");
	char pass_nueva[20];
	int status = 0;
	printf("Ingrese la nueva Pass del administrador: ");
	scanf("%s", pass_nueva);
	encriptar(pass_nueva, strlen(pass_nueva));
	if(fp != NULL)
	{
		fprintf(fp, "%s", pass_nueva);
		status = 1;
    	fclose(fp);
  	}
  	return status;
}