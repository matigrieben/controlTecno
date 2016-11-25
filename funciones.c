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
#include <sys/types.h>

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
	printf("Pase la tarjeta del nuevo usuario:\n");
	stringTag(uart0_filestream, vectorTag);
	__fpurge(stdin);
	printf("nombre:\n"); 
	scanf("%s", nombre);
	printf("apellido:\n"); 
	scanf("%s", apellido);
	printf("edad:\n"); 
	scanf("%d", &edad);
	printf("dni:\n"); 
	scanf("%d", &dni);
	if(verificarExistencia(dni, "usuarios.txt"))
	{
		camara(dni);
		printf("rango del nuevo usuario: (1 administrador, 2 usuario)\n"); 
		scanf("%d", &estatus);
		printf("Usuario agregado exitosamente!\n");
		fp=fopen("usuarios.txt", "a+");
		if(fp!= NULL) fprintf(fp, "%s,%s,%s,%d,%d,%d\n", vectorTag, nombre, apellido, edad, dni, estatus);
		else estatus = 1;	
		fclose(fp);	
	}
	else 
	{
		printf("Ya existe un usuario con ese dni, funcion nuevo usuario finalizado\n");	
		estatus = 1; 
	}
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
	char codigo[30], nombre[30], apellido[30], edadString[3] = "", dniString[8];
	int estatus = 0;
	fp = fopen(archivo, "r");
	if(fp == NULL) estatus =0;
	else
	{
		while(!feof(fp))
		{
			fscanf(fp, "%[^,], %[^,], %[^,], %[^,], %[^,], %d\n", codigo, nombre, apellido, edadString, dniString, &estatus);
			printf("edadstring:%d\n", edadString); //////borrarrrrr!"!!!"
			nuevo = (usuarios *)malloc(sizeof(usuarios));
			strcpy(nuevo->codigo, codigo);
			strcpy(nuevo->nombre, nombre);
			strcpy(nuevo->apellido, apellido);
			nuevo->edad = atoi(edadString);
			nuevo->documento = atoi(dniString);
			nuevo->rango = estatus;
			nuevo->sig = NULL;
			if(*h == NULL)
			{
				*h = nuevo;
				estatus = 1;
				pAux = *h;
			}
			else
			{
				while(pAux->sig != NULL) pAux = pAux->sig;
				pAux->sig = nuevo;
				estatus = 1;
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
    printf("Pass administrador:\n");
    scanf("%s", pass_ingresada);
    encriptar(pass_ingresada, strlen(pass_ingresada));
    if(!strcmp(pass, pass_ingresada)) status = 1;
    fclose(fp);
  }
  return status;
}

/**
	\fn int opcionesMenu()
	\brief opciones brinadas para el administrador
	\details Sin detalles
	\author 
	\date 2016.mes.dia
	\param 
	\param 
	\return el numero de opcion elegida
*/
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

void liberarListaUsuarios(struct usuarios **h)
{
	if(*h != NULL)
	{
		liberarListaUsuarios(&(*h)->sig);
		free(*h);
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
	\brief busca en la estructura si existe el codigo de tarjeta ingresado y llama a la funcion que imprime el usuario encontrado
	\details Sin detalles
	\author 
	\date 2016.mes.dia
	\param 
	\param 
	\return 0 si no se encontro el codigo(usuario), -1 si no existen usuarios (lista vacia), 1 si es admin, 2 si es usuario normal
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
				if(flag==2) imprimirUsuarioEncontrado(h); //para admin, eliminar despues
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
		if(!fork())
		{
			img = cvLoadImage(buf, CV_LOAD_IMAGE_COLOR);
			cvNamedWindow( "Usuario", CV_WINDOW_AUTOSIZE);
	  		cvShowImage("Usuario", img);
			cvWaitKey(3000);
			cvDestroyWindow("Usuario");
			exit(0);
		}
		else
		{
			wait(NULL);
		}
	}
	return;
}

int modificarUsuario(usuarios **h)
{
	
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
	__fpurge(stdin);
	return;
}

/**
	\fn void camara (int dni)
	\brief abre la camara, y saca la foto guardandola con el dni del usuario 
	\details Sin detalles
	\author 
	\date 2016.mes.dia
	\param dni del usuario que se esta ingresando al sistema, para guardar la foto con ese nombre
	\return 
*/
void camara (int dni)
{
	int bucle = 1, key = 0;
	char buf[15];
	sprintf(buf, "%d.jpg", dni); //concatenar el dni con.jpg. Es necesario que sea char, ya que asi lo pide la funcion cvSave...
    IplImage *frame= NULL;
    CvCapture* capture;
    if(!fork())
    {
	    capture = cvCaptureFromCAM( -1 );
	    cvSetCaptureProperty(capture, CV_CAP_PROP_FPS, 31.0);
	    cvNamedWindow("imagen", CV_WINDOW_AUTOSIZE); //crea una ventana para mostrar la camara
	    do
	    {
	        frame = cvQueryFrame( capture );
	        cvShowImage("imagen", frame);
	    }
	    while((key = cvWaitKey(1)) < 0);    
    	cvSaveImage(buf, frame,0); //guardar la imagen
		cvDestroyWindow("imagen"); //cierra la ventana generada
		cvReleaseCapture( &capture ); //cerrar los recursos de la camara pedidos
		exit(0);
	}
	else 
	{
		wait(NULL);
	}
}

/**
	\fn int nuevaPass()
	\brief le permite al administrador cambiar la contraseña
	\details Sin detalles
	\author 
	\date 2016.mes.dia
	\param 
	\param 
	\return 0 si hay error , 1 si el cambio fue exitoso
*/
int nuevaPass()
{
	FILE *fp = fopen("pass.txt", "w");
	char pass_nueva[20];
	int status = 0;
	printf("Ingrese la nueva Pass del administrador:\n");
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

/**
	\fn void encriptar(char* password, int cant)
	\brief encripta  contrseña 
	\details Sin detalles
	\author 
	\date 2016.mes.dia
	\param 
	\param 
	\return 
*/
void encriptar(char* password, int cant)
{  
	int i; 
	for(i = 0;i < cant; i++)
	{
	    if(i%2 != 0) password[i] = password[i]-13;    //Le resto a la letra 13.
	    else password[i] = password[i]+13;    //Le sumo a la letra 13.
	}
}

/**
	\fn int verificarExistencia(int dni, char* archivo)
	\brief cuando el admin quiere cargar un usuario, verifica que no exista en el sistema
	\details 
	\author 
	\date 2016.mes.dia
	\param dni: contiene el dni del usuario que se esta por ingresar al sistema
	\param * archivo nombre del archivo que contiene los usuarios del sistema
	\return 0 si ya existe el usuario, 1 si no existe
*/
int verificarExistencia(int dni, char* archivo) //------------------- 0 si existe, 1 no existe
{
	FILE *fp = fopen(archivo, "r");
	int flag = 1, estatus = 0;
	char codigo[30], nombre[30], apellido[30], edadString[8], dniString[10];
	if(fp != NULL)
	{
		while(!feof(fp))
		{
			fscanf(fp, "%[^,], %[^,], %[^,], %[^,], %[^,], %d\n", codigo, nombre, apellido, edadString, dniString, &estatus);
			if(dni == atoi(dniString)) flag = 0;
		}		
	}
	else printf("No se pudo abrir el archivo de usuarios!\n");
	fclose(fp);
	return flag;
}

void eliminarUsuario(struct usuarios **h)
{
	int dni, enc = 0, contador = 0;
	struct usuarios *p = NULL,*aux = NULL, *aux2 = NULL;
	printf("Ingrese el dni del usuario que quiere eliminar:\n");
	scanf("%d",&dni);
	p = *h;
	aux2 = *h;
	while(enc != 1)
	{
		if(p->sig != NULL)
		{
			aux =p->sig;
		}
		if(p->documento == dni)
		{
			if(contador == 0)
			{
				free(p);
				*h = aux;
				enc = 1;
			}
			if(p->sig == NULL)
			{
				free(p);
				aux2->sig = NULL;
				enc = 1;
			}
			else
			{
				aux2->sig = aux;
				free(p);
				enc = 1;
			}
		}
		else
		{
			p = p->sig;
			if(contador!=0)
			{
				aux2 = aux->sig;
			}
		}
		contador++;
	}
}
/*

void modificar(usuarios **h)
{
int dni,aux=0,mod=0,ncod,nedad,ndoc,nran,sal=1;
char nnom[10],nape[10];
usuarios *paux=NULL;
printf("ingrese el dni del usuario que quiere modificar\n");
scanf("%d",&dni);
paux=*h;
while(sal!=0)
{
	if(dni==paux->documento)
	{
	while(sal!=0)
		{
			printf("que desea modificar\n");
			printf("1)codigo\n");
			printf("2)nombre\n");
			printf("3)apellido\n");
			printf("4)edad\n");
			printf("5)documento\n");
			printf("6)rango\n");
			scanf("%d".&mod);
			switch(mod)
				{
				case 1:
				scanf("%d",&ncod);
				paux->codigo=ncod;
				break;
				case 2:
				scanf("%s",&nnom);
				paux->nombre=nnom;
				break;
				case 3:
				scanf("%s",&nape);
				paux->apellido=nape;
				break;
				case 4:
				scanf("%d",&nedad);
				paux->edad=nedad;
				break;
				case 5:
				scanf("%d",&ndoc);
				paux->documento=ndoc;
				break;
				case 6:
				scanf("%d",&nran);
				paux->rango=nran;
				break;
				}
			printf("si desea modificar algo mas 1 sino 0\n");
			scanf("%d",&sal);
		}
	}
	else
	{
	paux=paux->sig;
	}
}
}

void subir(usuario **h)
{
    FILE *fp;
    int dni,aux2=1,mod=0,ncod,nedad,ndoc,nran,sal=1;
char nnom[10],nape[10];
    usuario *aux=NULL;
    aux=*h;
    fp=fopen("empleados.txt","a");
if(fp==NULL)
	{
	printf("no se encontro\n");
	return 0;
        }
        while(aux2!=0)
        {
            ncod=aux->codigo;
            nnom=aux->nombre;
            nape=paux->apellido;
            nedad=paux->edad;
            ndoc=paux->documento;
            nran=paux->rango;
            fprintf(fpr,"%d,%s,%s,%d,%d,%d",ncod,nnom,nape,nedad,ndoc,nran);
            if(aux->sig==NULL)
            {
                aux2=0;
            }
            else
            {
                aux=aux->sig;
            }
        }
    fclose(fp);
}*/
